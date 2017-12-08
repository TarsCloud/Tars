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

#ifndef __TC_OPENSSL_H
#define __TC_OPENSSL_H

#if TARS_SSL

#include <string>
#include "tc_sslmgr.h"

struct ssl_st;
typedef struct ssl_st SSL;

namespace tars
{

/////////////////////////////////////////////////
/** 
 *@file   tc_openssl.h
 *@brief  OpenSsl封装
 * 
 */
/////////////////////////////////////////////////

/** 
 *@brief  OpenSsl封装
 */
class TC_OpenSSL
{
public:
   /**
    * @brief 构造函数. 
    */
    TC_OpenSSL() :
        _ssl(NULL),
        _bHandshaked(false),
        _isServer(false),
        _err(0)
    {
    }

   /**
    * @brief 析构函数. 
    */
    ~TC_OpenSSL();

private:
   /**
    * @brief 禁止复制
    */
    TC_OpenSSL(const TC_OpenSSL& );
    void operator=(const TC_OpenSSL& );

public:

    /** 
     * @brief 释放SSL 
     */
    void Release();

    /** 
     * @brief 初始化SSL
     */
    void Init(SSL* ssl, bool isServer);

    /**
     * @brief  握手是否完成 
     * @return 握手是否完成 
     */
    bool IsHandshaked() const;

    /** 
     * @brief  当前错误 
     * @return 当前错误 
     */
    bool HasError() const;

    /** 
     * @brief  当前接收缓冲区
     */
    string* RecvBuffer();

    /** 
     * @brief 握手 
     * @return 需要发送的握手数据 
     */
    std::string DoHandshake(const void* data = NULL, size_t size = 0);

    /** 
     * @brief 发送数据前加密 
     * @param data  数据的指针 
     * @param size  数据的大小 
     * @return 加密后的数据 
     */
    std::string Write(const void* data, size_t size);

    /** 
     * @brief 接收数据后解密 
     * @param data  数据的指针 
     * @param size  数据的大小 
     * @param out   需要发送的数据 
     * @return 解密后的数据 
     */
    bool Read(const void* data, size_t size, std::string& out);

private:
    /**
     * ssl handle
     */
    SSL* _ssl;

    /**
     * 是否握手完成了
     */
    bool _bHandshaked;

    /**
     * 是否服务端
     */
    bool _isServer;

    /**
     * 收到的数据解密后
     */
    std::string _plainBuf;
    /**
     * 类似于errno
     */
    int _err;
};

} // end namespace tars

#endif

#endif

