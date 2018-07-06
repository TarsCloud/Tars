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

#include "util/tc_http_async.h"
#include "util/tc_common.h"

namespace tars
{

TC_HttpAsync::AsyncRequest::AsyncRequest(TC_HttpRequest &stHttpRequest, TC_HttpAsync::RequestCallbackPtr &callbackPtr)
    : _pHttpAsync(NULL),_iUniqId(0), _callbackPtr(callbackPtr)
{
    memset(&_bindAddr,0,sizeof(struct sockaddr));
    _bindAddrSet = false;

    _sReq = stHttpRequest.encode();

    stHttpRequest.getHostPort(_sHost, _iPort);
}

TC_HttpAsync::AsyncRequest::~AsyncRequest()
{
    doClose();
}

void TC_HttpAsync::AsyncRequest::doClose()
{
    if(_fd.isValid())
    {
        if(_callbackPtr) _callbackPtr->onClose();
        if(_pHttpAsync) _pHttpAsync->erase(_iUniqId);
        _fd.close();
    }
}

void TC_HttpAsync::AsyncRequest::setBindAddr(const struct sockaddr* addr)
{
    memcpy(&_bindAddr, addr, sizeof(struct sockaddr));

    _bindAddrSet = true;
}

int TC_HttpAsync::AsyncRequest::doConnect()
{
    _fd.createSocket();
    _fd.setblock();

    //不产生TimeWait状态
    _fd.setNoCloseWait();

    try
    {
        if(_bindAddrSet)
        {
            _fd.bind(&_bindAddr,sizeof(_bindAddr));
        }

        int ret = _fd.connectNoThrow(_sHost, _iPort);

        if(ret < 0 && errno != EINPROGRESS)
        {
            _fd.close();
            return ret;
        }
    }
    catch(exception &ex)
    {
        _fd.close();
        return -1;
    }

    return 0;
}

int TC_HttpAsync::AsyncRequest::doConnect(struct sockaddr* proxyAddr)
{
    _fd.createSocket();
    _fd.setblock();

    //不产生TimeWait状态
    _fd.setNoCloseWait();

    try
    {
        if(_bindAddrSet)
        {
            _fd.bind(&_bindAddr,sizeof(_bindAddr));
        }

        int ret = _fd.connectNoThrow(proxyAddr);
        if(ret < 0 && errno != EINPROGRESS)
        {
            _fd.close();
            return ret;
        }
    }
    catch(exception &ex)
    {
        _fd.close();
        return -1;
    }

    return 0;
}

int TC_HttpAsync::AsyncRequest::recv(void* buf, uint32_t len, uint32_t flag)
{
    int ret = ::recv(_fd.getfd(), buf, len, flag);

    if (ret == 0)
    {
        return 0;
    }
    else if(ret < 0 && errno == EAGAIN)
    {
        return -1;
    }
    else if(ret < 0)
    {
        //其他网络异常
        return -2;
    }

    //正常发送的数据
    return ret;
}

int TC_HttpAsync::AsyncRequest::send(const void* buf, uint32_t len, uint32_t flag)
{
    int ret = ::send(_fd.getfd(), buf, len, flag);

    if (ret < 0 && errno == EAGAIN)
    {
        return -1;
    }
    else if (ret < 0)
    {
        return -2;
    }
    return ret;
}

void TC_HttpAsync::AsyncRequest::timeout()
{
    try
    {
        doClose();

        if(_callbackPtr) _callbackPtr->onTimeout();
    }
    catch(exception &ex)
    {
        if(_callbackPtr) _callbackPtr->onException(ex.what());
    }
    catch(...)
    {
        if(_callbackPtr) _callbackPtr->onException("unknown error.");
    }
}

void TC_HttpAsync::AsyncRequest::doException()
{
    string err("unknown error.");

    if(_fd.isValid())
    {
        int error;
        socklen_t len = sizeof(error);
        _fd.getSockOpt(SO_ERROR, (void*)&error, len, SOL_SOCKET);
        err = strerror(error);

        doClose();
    }

    try { if(_callbackPtr) _callbackPtr->onException(err); } catch(...) { }
}

void TC_HttpAsync::AsyncRequest::doRequest()
{
    if(!_fd.isValid()) return;

    int ret = -1;

    do
    {
        ret = -1;

        if (!_sReq.empty())
        {
            if ((ret = this->send(_sReq.c_str(), _sReq.length(), 0)) > 0)
            {
                _sReq = _sReq.substr(ret);
            }
        }
    }while(ret > 0);

    //网络异常
    if(ret == -2)
    {
        doException();
    }
}

void TC_HttpAsync::AsyncRequest::doReceive()
{
    if(!_fd.isValid()) return;

    int recv = 0;

    char buff[8192] = {0};

    do
    {
        if ((recv = this->recv(buff, sizeof(buff), 0)) > 0)
        {
            _sRsp.append(buff, recv);
        }
    }
    while (recv > 0);

    if(recv == -2)
    {
        doException();
    }
    else
    {
        try
        {
            //增量decode
            bool ret    = _stHttpResp.incrementDecode(_sRsp);

            //有头部数据了
            if(_callbackPtr && !_stHttpResp.getHeaders().empty())
            {
                bool bContinue = _callbackPtr->onReceive(_stHttpResp);
                if(!bContinue)
                {
                    doClose();
                    return;
                }
            }

            //服务器关闭了连接
            bool bClose = (recv == 0);

            //如果远程关闭连接或者增量decode完毕
            if(bClose || ret)
            {
                doClose();

                if(_callbackPtr) _callbackPtr->onResponse(bClose, _stHttpResp);
            }
        }
        catch(exception &ex)
        {
            if(_callbackPtr) _callbackPtr->onException(ex.what());
        }
        catch(...)
        {
            if(_callbackPtr) _callbackPtr->onException("unknown error.");
        }
    }
}

///////////////////////////////////////////////////////////////////////////

TC_HttpAsync::TC_HttpAsync() : _terminate(false)
{
    memset(&_proxyAddr,0,sizeof(struct sockaddr));
    memset(&_bindAddr,0,sizeof(struct sockaddr));

    _bindAddrSet=false;

    _data = new http_queue_type(10000);

    _epoller.create(1024);
}

TC_HttpAsync::~TC_HttpAsync()
{
    terminate();

    delete _data;
}

void TC_HttpAsync::start(int iThreadNum)
{
    _tpool.init(1);
    _tpool.start();

    _tpool.exec(std::bind(&TC_HttpAsync::run, this));
}

void TC_HttpAsync::waitForAllDone(int millsecond)
{
    time_t now = TNOW;

    while(_data->size() > 0)
    {
        if(millsecond < 0)
        {
            TC_ThreadLock::Lock lock(*this);
            timedWait(100);
            continue;
        }

        {
            //等待100ms
            TC_ThreadLock::Lock lock(*this);
            timedWait(100);
        }

        if((TNOW - now) >= (millsecond/1000))
            break;
    }

    terminate();
}

void TC_HttpAsync::erase(uint32_t uniqId)
{
    _data->erase(uniqId);

    TC_ThreadLock::Lock lock(*this);
    notify();
}

void TC_HttpAsync::terminate()
{
    _terminate = true;
    _tpool.waitForAllDone();
}

void TC_HttpAsync::timeout(AsyncRequestPtr& ptr)
{
    ptr->timeout();
}

int TC_HttpAsync::doAsyncRequest(TC_HttpRequest &stHttpRequest, RequestCallbackPtr &callbackPtr, bool bUseProxy,struct sockaddr* addr)
{
    int ret;

    AsyncRequestPtr req = new AsyncRequest(stHttpRequest, callbackPtr);

    if(_bindAddrSet)
    {
        req->setBindAddr(&_bindAddr);
    }

    //发起异步连接请求
    if(bUseProxy)
    {
        ret = req->doConnect(&_proxyAddr);
    }
    else if(NULL != addr)
    {
        ret = req->doConnect(addr);
    }
    else
    {
        ret = req->doConnect();
    }

    if(ret < 0) return -1;

    uint32_t uniqId = _data->generateId();

    req->setUniqId(uniqId);

    req->setHttpAsync(this);

    _data->push(req, uniqId);

    _epoller.add(req->getfd(), uniqId, EPOLLIN | EPOLLOUT);

    return 0;
}

int TC_HttpAsync::setBindAddr(const char* sBindAddr)
{
    bzero(&_bindAddr,sizeof(_bindAddr));

    struct sockaddr_in* p = (struct sockaddr_in *)&_bindAddr;

    try
    {
        TC_Socket::parseAddr(sBindAddr, p->sin_addr);
    }
    catch(exception &ex)
    {
        return -1;
    }

    p->sin_family = AF_INET;
    p->sin_port   = htons(0);

    _bindAddrSet  = true;

    return 0;
}


int TC_HttpAsync::setProxyAddr(const char* sProxyAddr)
{
    vector<string> v = TC_Common::sepstr<string>(sProxyAddr, ":");

    if(v.size() < 2)
        return -1;

    return setProxyAddr(v[0].c_str(), TC_Common::strto<uint16_t>(v[1]));
}

int TC_HttpAsync::setProxyAddr(const char* sHost, uint16_t iPort)
{
    bzero(&_proxyAddr,sizeof(_proxyAddr));

    struct sockaddr_in *p = (struct sockaddr_in *)&_proxyAddr;

    try
    {
        TC_Socket::parseAddr(sHost, p->sin_addr);
    }
    catch(exception &ex)
    {
        return -1;
    }

    p->sin_family = AF_INET;
    p->sin_port   = htons(iPort);

    return 0;
}

void TC_HttpAsync::setProxyAddr(const struct sockaddr* addr)
{
    memcpy(&_proxyAddr, addr, sizeof(struct sockaddr));
}

void TC_HttpAsync::process(AsyncRequestPtr& p, int events)
{
    if (events & (EPOLLERR | EPOLLHUP))
    {
        p->doException();
        return;
    }

    if(events & EPOLLIN)
    {
        p->doReceive();
    }

    if(events & EPOLLOUT)
    {
        p->doRequest();
    }
}

void TC_HttpAsync::run()
{
    TC_TimeoutQueue<AsyncRequestPtr>::data_functor df(&TC_HttpAsync::timeout);

    async_process_type apt(&TC_HttpAsync::process);

    int64_t lastDealTimeout = 0;

    while(!_terminate)
    {
        try
        {
            int64_t now = TNOWMS;
            if (lastDealTimeout + 500 < now)
            {
                lastDealTimeout = now;
                _data->timeout(df);
            }

            int num = _epoller.wait(100);

            for (int i = 0; i < num; ++i)
            {
                epoll_event ev = _epoller.get(i);

                uint32_t uniqId = (uint32_t)ev.data.u64;

                AsyncRequestPtr p = _data->get(uniqId, false);

                if(!p) continue;

//                async_process_type::wrapper_type w(apt, p, ev.events);

//                _npool[uniqId%_npool.size()]->exec(w);
                process(p, ev.events);
            }
        }
        catch(exception &ex)
        {
            cerr << "[TC_HttpAsync::run] error:" << ex.what() << endl;
        }
    }
}

}


