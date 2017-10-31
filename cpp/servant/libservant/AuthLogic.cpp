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
#include "util/tc_tea.h"
#include "util/tc_sha.h"
#include "util/tc_md5.h"
#include "servant/Application.h"
#include "servant/AuthLogic.h"
#include <iostream>
#include <cassert>


namespace tars
{

bool processAuth(void* c, const string& data)
{
    TC_EpollServer::NetThread::Connection* const conn = (TC_EpollServer::NetThread::Connection*)c;
    conn->tryInitAuthState(AUTH_INIT);

    if (conn->_authState == AUTH_SUCC)
        return false; // data to be processed

    TC_EpollServer::BindAdapter* adapter = conn->getBindAdapter();

    const int type = adapter->getEndpoint().getAuthType();
    if (type == AUTH_TYPENONE)
    {
        adapter->getEpollServer()->info("no auth func, so eAuthSucc");
        conn->_authState = AUTH_SUCC;
        return false;
    }

    // got auth request
    RequestPacket request;
    if (adapter->isTarsProtocol())
    {
        TarsInputStream<BufferReader> is; 
        is.setBuffer(data.data(), data.size());
        try {
            request.readFrom(is);
             ostringstream oos;
             request.display(oos);
        }
        catch(...) {
            conn->setClose();
            return true;
        }
    }
    else
    {
        request.sBuffer.assign(data.begin(), data.end());
    }

    const int currentState = conn->_authState;
    int newstate = tars::defaultProcessAuthReq(request.sBuffer.data(), request.sBuffer.size(), adapter->getName());
    std::string out = tars::etos((tars::AUTH_STATE)newstate); 

    if (newstate < 0)
    {
        // 验证错误,断开连接 
        adapter->getEpollServer()->error("authProcess failed with new state [" + out + "]");
        conn->setClose();
        return true;
    }

    adapter->getEpollServer()->info(TC_Common::tostr(conn->getId()) + "'s auth response[" + out + "], change state from " +
                                    TC_Common::tostr(currentState) + " to " + out);
    conn->_authState = newstate;

    if (adapter->isTarsProtocol())
    {
        TarsOutputStream<BufferWriter> os;
        ResponsePacket response; 
        response.iVersion = TARSVERSION;
        response.iRequestId = request.iRequestId;
        response.iMessageType = request.iMessageType;
        response.cPacketType = request.cPacketType;
        response.iRet = 0;
        response.sBuffer.assign(out.begin(), out.end());

        response.writeTo(os);

        tars::Int32 iHeaderLen = htonl(sizeof(tars::Int32) + os.getLength());

        std::string s;
        s.append((const char*)&iHeaderLen, sizeof(tars::Int32));
        s.append(os.getBuffer(), os.getLength());

        adapter->getEpollServer()->send(conn->getId(), s, "", 0, conn->getfd());
    }
    else
    {
        adapter->getEpollServer()->send(conn->getId(), out, "", 0, conn->getfd());
    }
        
    return true; // processed
}


int processAuthReqHelper(const BasicAuthPackage& pkg, const BasicAuthInfo& info)
{
    // 明文:objName, accessKey, time, hashMethod
    // 密文:use TmpKey to enc secret1;
    // and tmpKey = sha1(secret2 | timestamp);
    if (pkg.sObjName != info.sObjName)
        return AUTH_WRONG_OBJ;

    if (pkg.sAccessKey != info.sAccessKey)
        return AUTH_WRONG_AK;
        
    time_t now = TNOW;
    const int range = 60 * 60;
    if (!(pkg.iTime > (now - range) && pkg.iTime < (now + range)))
        return AUTH_WRONG_TIME;

    if (pkg.sHashMethod != "sha1")
        return AUTH_NOT_SUPPORT_ENC;

    // 用secret1 = sha1(password); secret2 = sha1(secret1);
    // 1.client create TmpKey use timestamp and secret2;
    // 2.client use TmpKey to enc secret1;
    // 3.server use TmpKey same as client, to dec secret1;
    // 4.server got secret1, then sha1(secret1), to compare secret2;
    // 下面这个是123456的两次sha1值
    //assert (info.sHashSecretKey2 == "69c5fcebaa65b560eaf06c3fbeb481ae44b8d618");

    string tmpKey;
    string hash2;
    {
        string hash1 = TC_SHA::sha1str(info.sHashSecretKey2.data(), info.sHashSecretKey2.size());
        hash2 = TC_SHA::sha1str(hash1.data(), hash1.size());
        string tmp = hash2;
        const char* pt = (const char* )&pkg.iTime;
        for (size_t i = 0; i < sizeof pkg.iTime; ++ i)
        {
            tmp[i] |= pt[i];
        }

        tmpKey = TC_MD5::md5bin(tmp);
    }

    string secret1;
    {
        vector<char> dec;
        try
        {
            dec = TC_Tea::decrypt2(tmpKey.data(), pkg.sSignature.data(), pkg.sSignature.size());
            secret1.assign(dec.begin(), dec.end());
        }
        catch (const TC_Tea_Exception& )
        {
            return AUTH_DEC_FAIL;
        }
    }

    // 4.server got secret1, then sha1(secret1), to compare secret2;
    string clientSecret2 = TC_SHA::sha1str(secret1.data(), secret1.size());
    if (clientSecret2.size() != hash2.size() ||
        !std::equal(clientSecret2.begin(), clientSecret2.end(), hash2.begin()))
    {
        return AUTH_ERROR;
    }

    return AUTH_SUCC;
}

// 只需要传入 expect 的objname；
// 内部根据obj查找access账号集
int defaultProcessAuthReq(const char* request, size_t len, const string& expectObj)
{
    if (len <= 20)
        return AUTH_PROTO_ERR;

    BasicAuthPackage pkg;
    TarsInputStream<BufferReader> is; 
    is.setBuffer(request, len);
    try {
        pkg.readFrom(is);
    }
    catch(...) {
        return AUTH_PROTO_ERR;
    }

    TC_EpollServer::BindAdapterPtr bap = Application::getEpollServer()->getBindAdapter(expectObj);
    if (!bap)
        return AUTH_WRONG_OBJ;

    BasicAuthInfo info;
    string expectServantName = ServantHelperManager::getInstance()->getAdapterServant(expectObj);
    info.sObjName = expectServantName;
    info.sAccessKey = pkg.sAccessKey;
    info.sHashSecretKey2 = bap->getSk(info.sAccessKey);
    if (info.sHashSecretKey2.empty())
        return AUTH_WRONG_AK;

    return processAuthReqHelper(pkg, info);
}

int defaultProcessAuthReq(const string& request, const string& expectObj)
{
    return defaultProcessAuthReq(request.data(), request.size(), expectObj);
}

string defaultCreateAuthReq(const BasicAuthInfo& info, const string& hashMethod)
{
    // 明文:objName, accessKey, time, hashMethod
    // 密文:use TmpKey to enc secret1;
    TarsOutputStream<BufferWriter> os;
    BasicAuthPackage pkg;
    pkg.sObjName = info.sObjName;
    pkg.sAccessKey = info.sAccessKey;
    pkg.iTime = TNOW;

    string secret1 = TC_SHA::sha1str(info.sSecretKey.data(), info.sSecretKey.size());
    string secret2 = TC_SHA::sha1str(secret1.data(), secret1.size());

    // create tmpKey;
    string tmpKey;
    {
        string tmp = secret2;
        const char* pt = (const char* )&pkg.iTime;
        for (size_t i = 0; i < sizeof pkg.iTime; ++ i)
        {
            tmp[i] |= pt[i];
        }
        // 保证key是16字节
        tmpKey = TC_MD5::md5bin(tmp);
    }

    // then use tmpKey to enc secret1, show server that I know secret1, ie, I know secret.
    vector<char> secret1Enc = TC_Tea::encrypt2(tmpKey.data(), secret1.data(), secret1.size());
        
    pkg.sSignature.assign(secret1Enc.begin(), secret1Enc.end());
    pkg.writeTo(os);

    return string(os.getBuffer(), os.getLength());
}

} // end namespace tars

