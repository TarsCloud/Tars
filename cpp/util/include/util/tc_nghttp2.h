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

#ifndef __TC_NGHTTP2_H
#define __TC_NGHTTP2_H

#if TARS_HTTP2

#include <string>
#include <list>
#include <map>
#include "nghttp2/nghttp2.h"

namespace tars
{

struct RequestPacket;
struct ResponsePacket;

enum ResponseState
{
    ResponseNone,
    ResponseHeadersDone,
    ResponseBodyDone,
};

struct Http2Response
{
    int streamId;
    std::map<std::string, std::string> headers;
    std::string body;
    ResponseState state;

    void swap(Http2Response& other);
};

/////////////////////////////////////////////////
/** 
 *@file   tc_nghttp2.h
 *@brief  NGHTTP2封装
 * 
 */
/////////////////////////////////////////////////

/** 
 *@brief  注册给taf client网络线程使用的打包函数
 */
void http2Request(const RequestPacket& , std::string& );

/** 
 *@brief  NGHTTP2封装
 */
class TC_NgHttp2
{
     friend void http2Request(const RequestPacket& , std::string& );
     friend size_t http2Response(const char* , size_t , std::list<tars::ResponsePacket>& , void*);
public:
    /** 
     *@brief  注册给ng session的send回调
     */
    static ssize_t onSend(nghttp2_session *session, const unsigned char* data, size_t length, int flags, void *user_data);
    /** 
     *@brief  注册给ng session的收到新流回调
     */
    static int onBeginHeaders(nghttp2_session *session, const nghttp2_frame *frame, void *user_data);
    /** 
     *@brief  注册给ng session的收到header kv
     */
    static int onHeader(nghttp2_session *session, const nghttp2_frame *frame,
                              const uint8_t *name, size_t namelen,
                              const uint8_t *value, size_t valuelen,
                              uint8_t flags, void *user_data);
    /** 
     *@brief  注册给ng session的收到完整frame
     */
    static int onFrameRecv(nghttp2_session *session, const nghttp2_frame *frame, void *user_data) ;
    /** 
     *@brief  注册给ng session的收到data frame
     */
    static int onDataChunkRecv(nghttp2_session *session, uint8_t flags,
                              int32_t stream_id, const uint8_t *data,
                              size_t len, void *user_data);

    /** 
     *@brief  注册给ng session的收到完整stream
     */
    static int onStreamClose(nghttp2_session *session, int32_t stream_id,
                             uint32_t error_code, void *user_data);
public:
    enum State
    {
        None,
        Negotiating,
        Http2, // use HTTP-2
        Http1, // please use HTTP-1
    };

   /**
    * @brief 构造函数. 
    */
    explicit
    TC_NgHttp2(bool isServer = false);

   /**
    * @brief 析构函数. 
    */
    ~TC_NgHttp2();

   /**
    * @brief 当前状态
    */
    int getState() const;

private:
   /**
    * @brief 禁止复制
    */
    TC_NgHttp2(const TC_NgHttp2& );
    void operator=(const TC_NgHttp2& );

public:
    /** 
     * @brief 初始化
     */
    void Init();
    /** 
     * @brief 协商结果
     */
    void onNegotiateDone(bool succ);
    /** 
     * @brief HTTP2握手+setting
     */
    int settings(unsigned int maxCurrentStreams = 1);
    /** 
     * @brief  当前缓冲区
     */
    std::string& sendBuffer();

    /** 
     * @brief  session
     */
    nghttp2_session* session() const;

private:
    /**
     * session
     */
    nghttp2_session* _session;

    /**
     * 状态
     */
    State _state;

    /**
     * 是否服务端
     */
    bool _isServer;
    /**
     * 发送缓存区，由send callback填充
     */
    std::string _sendBuf;

    /**
     * 收到的响应
     */
    std::map<int, Http2Response> _responses;
    /**
     * 收到的完整响应
     */
    std::map<int, Http2Response> _doneResponses;

    /**
     *协商升级使用
     */
    std::string _settings;
};

} // end namespace tars

#endif // end #if TARS_HTTP2

#endif

