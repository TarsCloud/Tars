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

#ifndef __TC_SSLMANAGER_H
#define __TC_SSLMANAGER_H

#if TARS_SSL

#include <map>
#include <string>
#include "util/tc_buffer.h"
#include "util/tc_singleton.h"

struct bio_st;
typedef struct bio_st BIO;

struct ssl_st; 
typedef struct ssl_st SSL;

struct ssl_ctx_st;
typedef struct ssl_ctx_st SSL_CTX;

struct ssl_method_st;
typedef struct ssl_method_st SSL_METHOD;

namespace tars
{

/////////////////////////////////////////////////
/** 
 *@file   tc_sslmgr.h
 *@brief  SSL_CTX集合
 */
/////////////////////////////////////////////////

static const size_t kSSLHeadSize = 5;

// new ssl conn
SSL* NewSSL(const std::string& ctxName);
// fetch data from mem bio
void GetMemData(BIO* bio, TC_Buffer& buf);
// fetch ssl head info
void GetSSLHead(const char* data, char& type, unsigned short& ver, unsigned short& len);
// read from ssl
bool DoSSLRead(SSL*, std::string& out);

class SSLManager : public TC_Singleton<SSLManager>
{
public:
    static void GlobalInit();

    SSLManager();
    ~SSLManager();

    bool AddCtx(const std::string& name,
                const std::string& cafile, 
                const std::string& certfile, 
                const std::string& keyfile,
                bool verifyClient);

    SSL_CTX* GetCtx(const std::string& name) const;

private:

    typedef std::map<std::string, SSL_CTX*> CTX_MAP;
    CTX_MAP _ctxSet;
};

} // end namespace tars

#endif // end #if TARS_SSL

#endif

