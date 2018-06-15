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

#if TARS_HTTP2

#include <string>
#include <algorithm>
#include "nghttp2/nghttp2.h"

#include "util/tc_nghttp2.h"
#include "util/tc_http2clientmgr.h"
#include "util/tc_base64.h"


namespace tars
{

void Http2Response::swap(Http2Response& other)
{
    if (this == &other)
        return;

    std::swap(streamId, other.streamId);
    headers.swap(other.headers);
    body.swap(other.body);
    std::swap(state, other.state);
}

ssize_t TC_NgHttp2::onSend(nghttp2_session* session,
                           const uint8_t* data,
                           size_t length,
                           int flags,
                           void* user_data)
{
    TC_NgHttp2* nghttp2 = (TC_NgHttp2* )user_data;
    nghttp2->_sendBuf.append((const char*)data, length);
    return length;
}

int TC_NgHttp2::onBeginHeaders(nghttp2_session* session,
                               const nghttp2_frame* frame,
                               void* user_data)
{
    TC_NgHttp2* nghttp2 = (TC_NgHttp2* )user_data;

    if (frame->hd.type == NGHTTP2_HEADERS)
    {
        if (frame->headers.cat == NGHTTP2_HCAT_RESPONSE)
        {
            Http2Response rsp;
            rsp.streamId = frame->hd.stream_id;
            rsp.state = ResponseNone;
            nghttp2->_responses[rsp.streamId] = rsp;
        }
    }

    return 0;
}

int TC_NgHttp2::onHeader(nghttp2_session* session, const nghttp2_frame* frame,
                         const uint8_t* name, size_t namelen,
                         const uint8_t* value, size_t valuelen,
                         uint8_t flags, void* user_data)
{
    TC_NgHttp2* nghttp2 = (TC_NgHttp2* )user_data;

    int streamId = frame->hd.stream_id;
    std::map<int, Http2Response>::iterator it = nghttp2->_responses.find(streamId);
    if (it == nghttp2->_responses.end())
    {
        return NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    std::string n((const char*)name, namelen);
    std::string v((const char*)value, valuelen);
    it->second.headers.insert(std::make_pair(n, v));

    return 0;
}

int TC_NgHttp2::onFrameRecv(nghttp2_session* session,
                            const nghttp2_frame* frame,
                            void* user_data)
{
    TC_NgHttp2* nghttp2 = (TC_NgHttp2* )user_data;

    int streamId = frame->hd.stream_id;
    if (streamId == 0)
        return 0;

    std::map<int, Http2Response>::iterator it = nghttp2->_responses.find(streamId);
    if (it == nghttp2->_responses.end())
    {
        return NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    switch (frame->hd.type)
    {
        case NGHTTP2_HEADERS:
            if (frame->hd.flags & NGHTTP2_FLAG_END_HEADERS)
            {                                              
                it->second.state = ResponseHeadersDone;
            }
            return 0;

        default:
            break;
    }

    return 0;
}

int TC_NgHttp2::onDataChunkRecv(nghttp2_session* session, uint8_t flags,
                                int32_t stream_id, const uint8_t* data,
                                size_t len, void* user_data)
{
    TC_NgHttp2* nghttp2 = (TC_NgHttp2* )user_data;
    std::map<int, Http2Response>::iterator it = nghttp2->_responses.find(stream_id);
    if (it == nghttp2->_responses.end())
    {
        return NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    it->second.body.append((const char* )data, len);
    return 0;
}

int TC_NgHttp2::onStreamClose(nghttp2_session* session, int32_t stream_id,
                              uint32_t error_code, void* user_data)
{
    TC_NgHttp2* nghttp2 = (TC_NgHttp2* )user_data;
    std::map<int, Http2Response>::iterator it = nghttp2->_responses.find(stream_id);
    if (it == nghttp2->_responses.end())
    {
        return NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    it->second.state = ResponseBodyDone;

    nghttp2->_doneResponses[stream_id].swap(it->second);
    nghttp2->_responses.erase(it);

    return 0;
}



TC_NgHttp2::TC_NgHttp2(bool isServer) :
    _session(NULL),
    _state(None),
    _isServer(isServer)
{
    nghttp2_session_callbacks* callbacks;
    nghttp2_session_callbacks_new(&callbacks);
    nghttp2_session_callbacks_set_send_callback(callbacks, &TC_NgHttp2::onSend);
    nghttp2_session_callbacks_set_on_begin_headers_callback(callbacks, &TC_NgHttp2::onBeginHeaders);
    nghttp2_session_callbacks_set_on_header_callback(callbacks, &TC_NgHttp2::onHeader);
    nghttp2_session_callbacks_set_on_frame_recv_callback(callbacks, &TC_NgHttp2::onFrameRecv);
    nghttp2_session_callbacks_set_on_data_chunk_recv_callback(callbacks, &TC_NgHttp2::onDataChunkRecv);
    nghttp2_session_callbacks_set_on_stream_close_callback(callbacks, &TC_NgHttp2::onStreamClose);

    if (isServer)
        nghttp2_session_server_new(&_session, callbacks, this);
    else
        nghttp2_session_client_new(&_session, callbacks, this);

    nghttp2_session_callbacks_del(callbacks);
}

TC_NgHttp2::~TC_NgHttp2()
{
}

int TC_NgHttp2::getState() const
{
    return (int)_state;
}

void TC_NgHttp2::Init()
{
    if (_state != None)
        return;

    _state = Http2;
}

int TC_NgHttp2::settings(unsigned int maxCurrentStreams)
{
    nghttp2_settings_entry iv[2] = { 
                                        {NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS, maxCurrentStreams},
                                        {NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE, 1024 * 1024 * 1024},
                                   };

    /* 24 bytes magic string also will be sent*/
    int rv = nghttp2_submit_settings(_session,
                                     NGHTTP2_FLAG_NONE,
                                     iv,
                                     sizeof(iv)/sizeof(iv[0]));
    rv = nghttp2_session_send(_session);
    return rv;
}

void TC_NgHttp2::onNegotiateDone(bool succ)
{
    assert (_state == Negotiating);
    _state = succ ? Http2: Http1;
    if (succ) 
    {
        int rv = nghttp2_session_upgrade(_session,
                                         (const uint8_t*)_settings.data(),
                                         _settings.size(),
                                         NULL);
        if (rv)
            cerr << "nghttp2_session_upgrade error: " << nghttp2_strerror(rv) << endl;
    }
}

string& TC_NgHttp2::sendBuffer()
{
    return _sendBuf;
}

nghttp2_session* TC_NgHttp2::session() const
{
    return _session;
}

} // end namespace tars

#endif // end #if TARS_HTTP2

