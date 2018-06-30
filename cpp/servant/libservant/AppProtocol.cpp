/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016THL A29 Limited, a Tencent company. All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use this file except 
 * in compliance with the License. You may obtain a copy of the License at
 *
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed 
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the 
 * specific language governing permissions and limitations under the License.
 */

#include "util/tc_epoll_server.h"
#include "servant/AppProtocol.h"
#include "tup/Tars.h"
#include <iostream>


#if TARS_HTTP2
#include "util/tc_nghttp2.h"
#include "util/tc_http2clientmgr.h"

#define MAKE_NV(NAME, VALUE, VALUELEN)                                         \
      {                                                                            \
              (uint8_t *)NAME, (uint8_t *)VALUE, sizeof(NAME) - 1, VALUELEN,             \
                  NGHTTP2_NV_FLAG_NONE                                                   \
            }

#define MAKE_NV2(NAME, VALUE)                                                  \
      {                                                                            \
              (uint8_t *)NAME, (uint8_t *)VALUE, sizeof(NAME) - 1, sizeof(VALUE) - 1,    \
                  NGHTTP2_NV_FLAG_NONE                                                   \
            }

#define MAKE_STRING_NV(NAME, VALUE) {(uint8_t*)(NAME.data()), (uint8_t*)(VALUE.data()), NAME.size(), VALUE.size(), NGHTTP2_NV_FLAG_NONE};
#endif

namespace tars
{

//TARSServer的协议解析器
int AppProtocol::parseAdmin(string &in, string &out)
{
    return parse(in, out);
}

void ProxyProtocol::tarsRequest(const RequestPacket& request, string& buff)
{
    TarsOutputStream<BufferWriter> os;

    request.writeTo(os);

    tars::Int32 iHeaderLen = htonl(sizeof(tars::Int32) + os.getLength());

    buff.clear();

    buff.reserve(sizeof(tars::Int32) + os.getLength());

    buff.append((const char*)&iHeaderLen, sizeof(tars::Int32));

    buff.append(os.getBuffer(), os.getLength());
}
////////////////////////////////////////////////////////////////////////////////////
#if TARS_HTTP2
// nghttp2读取请求包体，准备发送
static ssize_t reqbody_read_callback(nghttp2_session *session, int32_t stream_id,
                                     uint8_t *buf, size_t length,
                                     uint32_t *data_flags,
                                     nghttp2_data_source *source,
                                     void *user_data)
{
    std::vector<char>* body = (std::vector<char>* )source->ptr;
    if (body->empty())
    {
        *data_flags |= NGHTTP2_DATA_FLAG_EOF;
        return 0;
    }

    ssize_t len = length > body->size() ? body->size() : length;
    std::memcpy(buf, &(*body)[0], len);
        
    vector<char>::iterator end = body->begin();
    std::advance(end, len);
    body->erase(body->begin(), end);

    return len;
}

size_t http1Response(const char* recvBuffer, size_t length, std::list<tars::ResponsePacket>& done)
{
    tars::TC_HttpResponse httpRsp;
    bool ok = httpRsp.decode(std::string(recvBuffer, length));
    if(!ok)
        return 0;

    ResponsePacket rsp;
    rsp.status["status"]  = httpRsp.getResponseHeaderLine();
    for (const auto& kv : httpRsp.getHeaders())
    {
        // 响应的头部 
        rsp.status[kv.first] = kv.second; 
    } 

    std::string content(httpRsp.getContent()); 
    rsp.sBuffer.assign(content.begin(), content.end());
    done.push_back(rsp);
    return httpRsp.getHeadLength() + httpRsp.getContentLength();
}

std::string encodeHttp2(RequestPacket& request, TC_NgHttp2* session)
{
    std::vector<nghttp2_nv> nva;

    const std::string method(":method");
    nghttp2_nv nv1 = MAKE_STRING_NV(method, request.sFuncName);
    if (!request.sFuncName.empty())
        nva.push_back(nv1);

    const std::string path(":path");
    nghttp2_nv nv2 = MAKE_STRING_NV(path, request.sServantName);
    if (!request.sServantName.empty())
        nva.push_back(nv2);

    for (std::map<std::string, std::string>::const_iterator
                it(request.context.begin());
                it != request.context.end();
                ++ it)
    {
        nghttp2_nv nv = MAKE_STRING_NV(it->first, it->second);
        nva.push_back(nv);
    }

    nghttp2_data_provider* pData = NULL;
    nghttp2_data_provider data;
    if (!request.sBuffer.empty())
    {
        pData = &data;
        data.source.ptr = (void*)&request.sBuffer;
        data.read_callback = reqbody_read_callback;
    }

    int32_t sid = nghttp2_submit_request(session->session(),
                                         NULL,
                                         &nva[0],
                                         nva.size(),
                                         pData,
                                         NULL);
    if (sid < 0)
    {
        cerr << "Fatal error: nghttp2_submit_request return " << sid << endl;
        return "";
    }

    request.iRequestId = sid;
    nghttp2_session_send(session->session());
        
    // 交给tars发送
    std::string out;
    out.swap(session->sendBuffer());
    return out;
}

// ENCODE function, called by network thread
void http2Request(const RequestPacket& request, std::string& out)
{
    TC_NgHttp2* session = Http2ClientSessionManager::getInstance()->getSession(request.iRequestId);
    if (session->getState() == TC_NgHttp2::None)
    {
        session->Init();
        session->settings();
    }

    assert (session->getState() == TC_NgHttp2::Http2);

    out = encodeHttp2(const_cast<RequestPacket&>(request), session);
}

size_t http2Response(const char* recvBuffer, size_t length, list<ResponsePacket>& done, void* userptr)
{
    const int sessionId = *(int*)&userptr;
    TC_NgHttp2* session = Http2ClientSessionManager::getInstance()->getSession(sessionId);
    assert (session->getState() == TC_NgHttp2::Http2);

    int readlen = nghttp2_session_mem_recv(session->session(),
                                           (const uint8_t*)recvBuffer,
                                           length);

    if (readlen < 0)
    {
        throw std::runtime_error("nghttp2_session_mem_recv return error");
        return 0;
    }

    std::map<int, Http2Response>::const_iterator it(session->_doneResponses.begin());
    for (; it != session->_doneResponses.end(); ++ it)
    {
        ResponsePacket rsp;
             
        rsp.iRequestId = it->second.streamId;
        rsp.status = it->second.headers;
        rsp.sBuffer.assign(it->second.body.begin(), it->second.body.end());

        done.push_back(rsp);
    }

    session->_doneResponses.clear();
    return readlen;
}

#endif
}

