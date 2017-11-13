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

#if TARS_SSL

#include "util/tc_sslmgr.h"
#include "util/tc_buffer.h"
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace tars
{

SSLManager::SSLManager()
{
}

void SSLManager::GlobalInit()
{
    (void)SSL_library_init();
    OpenSSL_add_all_algorithms();

    ERR_load_ERR_strings();
    SSL_load_error_strings();
}


SSLManager::~SSLManager()
{ 
    for (CTX_MAP::iterator it(_ctxSet.begin());
                           it != _ctxSet.end();
                           ++ it)
    {
        SSL_CTX_free(it->second);
    }

    ERR_free_strings();
    EVP_cleanup();
}

bool SSLManager::AddCtx(const std::string& name,
                        const std::string& cafile, 
                        const std::string& certfile, 
                        const std::string& keyfile,
                        bool verifyClient)
{
    if (_ctxSet.count(name))
        return false;
    
    SSL_CTX* ctx = SSL_CTX_new(SSLv23_method());
    if (!ctx)
        return false;

#define RETURN_IF_FAIL(call) \
    if ((call) <= 0) { \
        ERR_print_errors_fp(stderr); \
        return false;\
    }

    if (verifyClient)
        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
    else
        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_OFF);
    SSL_CTX_clear_options(ctx, SSL_OP_LEGACY_SERVER_CONNECT);
    SSL_CTX_clear_options(ctx, SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION); 

    RETURN_IF_FAIL (SSL_CTX_set_session_id_context(ctx, (const unsigned char*)ctx, sizeof ctx));
    if (!cafile.empty())
        RETURN_IF_FAIL (SSL_CTX_load_verify_locations(ctx, cafile.data(), NULL));

    // 客户端可以不提供证书的
    if (!certfile.empty()) 
        RETURN_IF_FAIL (SSL_CTX_use_certificate_file(ctx, certfile.data(), SSL_FILETYPE_PEM));

    if (!keyfile.empty()) 
    { 
        RETURN_IF_FAIL (SSL_CTX_use_PrivateKey_file(ctx, keyfile.data(), SSL_FILETYPE_PEM)); 
        RETURN_IF_FAIL (SSL_CTX_check_private_key(ctx)); 
    }

#undef RETURN_IF_FAIL

    return _ctxSet.insert(std::make_pair(name, ctx)).second;
}

SSL_CTX* SSLManager::GetCtx(const std::string& name) const
{
    CTX_MAP::const_iterator it = _ctxSet.find(name);
    return it == _ctxSet.end() ? NULL: it->second;
}



SSL* NewSSL(const std::string& ctxName)
{
    SSL_CTX* ctx = SSLManager::getInstance()->GetCtx(ctxName);
    if (!ctx)
        return NULL;

    SSL* ssl = SSL_new(ctx);

    SSL_set_mode(ssl, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER); // allow retry ssl-write with different args
    SSL_set_bio(ssl, BIO_new(BIO_s_mem()), BIO_new(BIO_s_mem()));

    BIO_set_mem_eof_return(SSL_get_rbio(ssl), -1);
    BIO_set_mem_eof_return(SSL_get_wbio(ssl), -1);

    return ssl;
}

void GetMemData(BIO* bio, TC_Buffer& buf)
{
    while (true)
    {
        buf.AssureSpace(16 * 1024);
        int bytes = BIO_read(bio, buf.WriteAddr(), buf.WritableSize());
        if (bytes <= 0)
            return;

        buf.Produce(bytes);
    }
            
    // never here
}

void GetSSLHead(const char* data, char& type, unsigned short& ver, unsigned short& len)
{
    type = data[0];
    ver = *(unsigned short*)(data + 1);
    len = *(unsigned short*)(data + 3);

    ver = ntohs(ver);
    len = ntohs(len);
}

bool DoSSLRead(SSL* ssl, std::string& out)
{
    while (true)
    {
        char plainBuf[32 * 1024];
                    
        ERR_clear_error();
        int bytes = SSL_read(ssl, plainBuf, sizeof plainBuf);
        if (bytes > 0)
        {
            out.append(plainBuf, bytes);
        }
        else
        {
            int err = SSL_get_error(ssl, bytes);
                    
            // when peer issued renegotiation, here will demand us to send handshake data.
            // write to mem bio will always success, only need to check whether has data to send.
            //assert (err != SSL_ERROR_WANT_WRITE);
                
            if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_ZERO_RETURN)
            {
                printf("DoSSLRead err %d\n", err);
                return false;
            }

            break;
        }
    }

    return true;
}

} // end namespace tars

#endif // end #if TARS_SSL
