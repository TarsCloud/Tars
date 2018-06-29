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
#include "util/tc_clientsocket.h"
#include "util/tc_common.h"
#include <iostream>
#include <limits>
#include <cassert>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <net/if_arp.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#if TARS_SSL
#include "util/tc_openssl.h"
#endif


namespace tars
{
#define H64(x) (((uint64_t)x) << 32)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// handle的实现
TC_EpollServer::Handle::Handle()
: _pEpollServer(NULL)
, _handleGroup(NULL)
, _iWaitTime(100)
{
}

TC_EpollServer::Handle::~Handle()
{
}

void TC_EpollServer::Handle::handleClose(const tagRecvData &stRecvData)
{
}

void TC_EpollServer::Handle::sendResponse(uint32_t uid, const string &sSendBuffer, const string &ip, int port, int fd)
{
    _pEpollServer->send(uid, sSendBuffer, ip, port, fd);
}

void TC_EpollServer::Handle::close(uint32_t uid, int fd)
{
    _pEpollServer->close(uid, fd);
}

void TC_EpollServer::Handle::handleTimeout(const tagRecvData &stRecvData)
{
    _pEpollServer->error("[Handle::handleTimeout] queue timeout, close [" + stRecvData.ip + ":" + TC_Common::tostr(stRecvData.port)+ "].");

    close(stRecvData.uid, stRecvData.fd);
}

void TC_EpollServer::Handle::handleOverload(const tagRecvData &stRecvData)
{
    _pEpollServer->error("[Handle::handleOverload] adapter '"
                         + stRecvData.adapter->getName() + "',over load:"
                         + TC_Common::tostr(stRecvData.adapter->getRecvBufferSize()) +  ">"
                         + TC_Common::tostr(stRecvData.adapter->getQueueCapacity()) + ".");

    close(stRecvData.uid, stRecvData.fd);
}

void TC_EpollServer::Handle::run()
{
    initialize();

    handleImp();
}

bool TC_EpollServer::Handle::allAdapterIsEmpty()
{
    map<string, BindAdapterPtr>& adapters = _handleGroup->adapters;

    for (const auto& kv : adapters)
    {
        if (kv.second->getRecvBufferSize() > 0)
        {
            return false;
        }
    }

    return true;
}

bool TC_EpollServer::Handle::allFilterIsEmpty()
{
    return true;
}

void TC_EpollServer::Handle::setEpollServer(TC_EpollServer *pEpollServer)
{
    TC_ThreadLock::Lock lock(*this);

    _pEpollServer = pEpollServer;
}

TC_EpollServer* TC_EpollServer::Handle::getEpollServer()
{
    return _pEpollServer;
}

void TC_EpollServer::Handle::setHandleGroup(TC_EpollServer::HandleGroupPtr& pHandleGroup)
{
    TC_ThreadLock::Lock lock(*this);

    _handleGroup = pHandleGroup;
}

TC_EpollServer::HandleGroupPtr& TC_EpollServer::Handle::getHandleGroup()
{
    return _handleGroup;
}

void TC_EpollServer::Handle::notifyFilter()
{
    TC_ThreadLock::Lock lock(_handleGroup->monitor);

    //如何做到不唤醒所有handle呢？
    _handleGroup->monitor.notifyAll();
}

void TC_EpollServer::Handle::setWaitTime(uint32_t iWaitTime)
{
    TC_ThreadLock::Lock lock(*this);

    _iWaitTime = iWaitTime;
}

void TC_EpollServer::Handle::handleImp()
{
    startHandle();

    while (!getEpollServer()->isTerminate())
    {
        {
            TC_ThreadLock::Lock lock(_handleGroup->monitor);

            if (allAdapterIsEmpty() && allFilterIsEmpty())
            {
                _handleGroup->monitor.timedWait(_iWaitTime);
            }
        }

        //上报心跳
        heartbeat();

        //为了实现所有主逻辑的单线程化,在每次循环中给业务处理自有消息的机会
        handleAsyncResponse();
        handleCustomMessage(true);


        tagRecvData* recv = NULL;

        map<string, BindAdapterPtr>& adapters = _handleGroup->adapters;

        for (auto& kv : adapters)
        {
            BindAdapterPtr& adapter = kv.second;

            try
            {

                while (adapter->waitForRecvQueue(recv, 0))
                {
                    //上报心跳
                    heartbeat();

                    //为了实现所有主逻辑的单线程化,在每次循环中给业务处理自有消息的机会
                    handleAsyncResponse();

                    tagRecvData& stRecvData = *recv;

                    int64_t now = TNOWMS;


                    stRecvData.adapter = adapter;

                    //数据已超载 overload
                    if (stRecvData.isOverload)
                    {
                        handleOverload(stRecvData);
                    }
                    //关闭连接的通知消息
                    else if (stRecvData.isClosed)
                    {
                        handleClose(stRecvData);
                    }
                    //数据在队列中已经超时了
                    else if ( (now - stRecvData.recvTimeStamp) > (int64_t)adapter->getQueueTimeout())
                    {
                        handleTimeout(stRecvData);
                    }
                    else
                    {
                        handle(stRecvData);
                    }
                    handleCustomMessage(false);

                    delete recv;
                    recv = NULL;
                }
            }
            catch (exception &ex)
            {
                if(recv)
                {
                    close(recv->uid, recv->fd);
                    delete recv;
                    recv = NULL;
                }

                getEpollServer()->error("[Handle::handleImp] error:" + string(ex.what()));
            }
            catch (...)
            {
                if(recv)
                {
                    close(recv->uid, recv->fd);
                    delete recv;
                    recv = NULL;
                }

                getEpollServer()->error("[Handle::handleImp] unknown error");
            }
        }
    }

    stopHandle();
}

////////////////////////////BindAdapter///////////////////////////////////
TC_EpollServer::BindAdapter::BindAdapter(TC_EpollServer *pEpollServer)
: _pReportQueue(NULL)
, _pReportConRate(NULL)
, _pReportTimeoutNum(NULL)
, _pEpollServer(pEpollServer)
, _handleGroup(NULL)
, _pf(echo_protocol)
, _hf(echo_header_filter)
, _name("")
, _handleGroupName("")
, _iMaxConns(DEFAULT_MAX_CONN)
, _iCurConns(0)
, _iHandleNum(0)
, _eOrder(ALLOW_DENY)
, _iQueueCapacity(DEFAULT_QUEUE_CAP)
, _iQueueTimeout(DEFAULT_QUEUE_TIMEOUT)
, _iHeaderLen(0)
, _iHeartBeatTime(0)
, _protocolName("tars")
, _iBackPacketBuffLimit(0)
{
}

TC_EpollServer::BindAdapter::~BindAdapter()
{
    //adapter析够的时候, 服务要退出
    _pEpollServer->terminate();
}

void TC_EpollServer::BindAdapter::setProtocolName(const string& name)
{
    TC_ThreadLock::Lock lock(*this);

    _protocolName = name;
}

const string& TC_EpollServer::BindAdapter::getProtocolName()
{
    return _protocolName;
}

bool TC_EpollServer::BindAdapter::isTarsProtocol()
{
    return (_protocolName == "tars");
}

bool TC_EpollServer::BindAdapter::isIpAllow(const string& ip) const
{
    TC_ThreadLock::Lock lock(*this);

    if(_eOrder == ALLOW_DENY)
    {
        if(TC_Common::matchPeriod(ip,_vtAllow))
        {
            return true;
        }
        if(TC_Common::matchPeriod(ip,_vtDeny))
        {
            return false;
        }
    }
    else
    {
        if(TC_Common::matchPeriod(ip,_vtDeny))
        {
            return false;
        }
        if(TC_Common::matchPeriod(ip,_vtAllow))
        {
            return true;
        }
    }
    return _vtAllow.size() == 0;
}

void TC_EpollServer::BindAdapter::insertRecvQueue(const recv_queue::queue_type &vtRecvData, bool bPushBack)
{
    {
        if (bPushBack)
        {
            _rbuffer.push_back(vtRecvData);
        }
        else
        {
            _rbuffer.push_front(vtRecvData);
        }
    }

    TC_ThreadLock::Lock lock(_handleGroup->monitor);

    _handleGroup->monitor.notify();
}

bool TC_EpollServer::BindAdapter::waitForRecvQueue(tagRecvData* &recv, uint32_t iWaitTime)
{
    bool bRet = false;

    bRet = _rbuffer.pop_front(recv, iWaitTime);

    if(!bRet)
    {
        return bRet;
    }

    return bRet;
}

size_t TC_EpollServer::BindAdapter::getRecvBufferSize() const
{
    return _rbuffer.size();
}

TC_EpollServer* TC_EpollServer::BindAdapter::getEpollServer()
{
    return _pEpollServer;
}

int TC_EpollServer::BindAdapter::echo_protocol(string &r, string &o)
{
    o = r;

    r = "";

    return 1;
}

int TC_EpollServer::BindAdapter::echo_header_filter(int i, string &o)
{
    return 1;
}

void TC_EpollServer::BindAdapter::setHandleGroupName(const string& handleGroupName)
{
    _handleGroupName = handleGroupName;
}

string TC_EpollServer::BindAdapter::getHandleGroupName() const
{
    return _handleGroupName;
}

void TC_EpollServer::BindAdapter::setName(const string &name)
{
    TC_ThreadLock::Lock lock(*this);

    _name = name;
}

string TC_EpollServer::BindAdapter::getName() const
{
    return _name;
}

int TC_EpollServer::BindAdapter::getHandleNum()
{
    return _iHandleNum;
}

void TC_EpollServer::BindAdapter::setHandleNum(int n)
{
    TC_ThreadLock::Lock lock(*this);

    _iHandleNum = n;
}

int TC_EpollServer::BindAdapter::getQueueCapacity() const
{
    return _iQueueCapacity;
}

void TC_EpollServer::BindAdapter::setQueueCapacity(int n)
{
    _iQueueCapacity = n;
}

int TC_EpollServer::BindAdapter::isOverloadorDiscard()
{
    int iRecvBufferSize = _rbuffer.size();

    if(iRecvBufferSize <= (_iQueueCapacity / 2))//未过载
    {
        return 0;
    }
    else if(iRecvBufferSize > (_iQueueCapacity / 2) && (iRecvBufferSize < _iQueueCapacity) && (_iQueueCapacity > 0))//overload
    {
        return -1;
    }
    else//队列满需要丢弃接受的数据包
    {
        return -2;
    }

    return 0;
}

void TC_EpollServer::BindAdapter::setQueueTimeout(int t)
{
    if (t >= MIN_QUEUE_TIMEOUT)
    {
        _iQueueTimeout = t;
    }
    else
    {
        _iQueueTimeout = MIN_QUEUE_TIMEOUT;
    }
}

int TC_EpollServer::BindAdapter::getQueueTimeout() const
{
    return _iQueueTimeout;
}

void TC_EpollServer::BindAdapter::setEndpoint(const string &str)
{
    TC_ThreadLock::Lock lock(*this);

    _ep.parse(str);
}

TC_Endpoint TC_EpollServer::BindAdapter::getEndpoint() const
{
    return _ep;
}

TC_Socket& TC_EpollServer::BindAdapter::getSocket()
{
    return _s;
}

void TC_EpollServer::BindAdapter::setMaxConns(int iMaxConns)
{
    _iMaxConns = iMaxConns;
}

size_t TC_EpollServer::BindAdapter::getMaxConns() const
{
    return _iMaxConns;
}

void TC_EpollServer::BindAdapter::setHeartBeatTime(time_t t)
{
    TC_ThreadLock::Lock lock(*this);

    _iHeartBeatTime = t;
}

time_t TC_EpollServer::BindAdapter::getHeartBeatTime() const
{
    return _iHeartBeatTime;
}

void TC_EpollServer::BindAdapter::setOrder(EOrder eOrder)
{
    _eOrder = eOrder;
}

void TC_EpollServer::BindAdapter::setAllow(const vector<string> &vtAllow)
{
    TC_ThreadLock::Lock lock(*this);

    _vtAllow = vtAllow;
}

void TC_EpollServer::BindAdapter::setDeny(const vector<string> &vtDeny)
{
    TC_ThreadLock::Lock lock(*this);

    _vtDeny = vtDeny;
}

TC_EpollServer::BindAdapter::EOrder TC_EpollServer::BindAdapter::getOrder() const
{
    return _eOrder;
}

vector<string> TC_EpollServer::BindAdapter::getAllow() const
{
    TC_ThreadLock::Lock lock(*this);

    return _vtAllow;
}

vector<string> TC_EpollServer::BindAdapter::getDeny() const
{
    TC_ThreadLock::Lock lock(*this);

    return _vtDeny;
}

bool TC_EpollServer::BindAdapter::isLimitMaxConnection() const
{
    return (_iCurConns + 1 > _iMaxConns) || (_iCurConns + 1 > ((int)((uint32_t)1 << 22) -1));
}

void TC_EpollServer::BindAdapter::decreaseNowConnection()
{
    _iCurConns.dec();
}

void TC_EpollServer::BindAdapter::increaseNowConnection()
{
    _iCurConns.inc();
}

size_t TC_EpollServer::BindAdapter::getNowConnection() const
{
    return _iCurConns.get();
}

vector<TC_EpollServer::ConnStatus> TC_EpollServer::BindAdapter::getConnStatus()
{
    return _pEpollServer->getConnStatus(_s.getfd());
}

void TC_EpollServer::BindAdapter::setProtocol(const TC_EpollServer::protocol_functor &pf, int iHeaderLen, const TC_EpollServer::header_filter_functor &hf)
{
    _pf = pf;

    _hf = hf;

    _iHeaderLen = iHeaderLen;
}

TC_EpollServer::protocol_functor& TC_EpollServer::BindAdapter::getProtocol()
{
    return _pf;
}

TC_EpollServer::header_filter_functor& TC_EpollServer::BindAdapter::getHeaderFilterFunctor()
{
    return _hf;
}

int TC_EpollServer::BindAdapter::getHeaderFilterLen()
{
    return _iHeaderLen;
}

void TC_EpollServer::BindAdapter::setBackPacketBuffLimit(size_t iLimitSize)
{
    _iBackPacketBuffLimit = iLimitSize;
}

size_t TC_EpollServer::BindAdapter::getBackPacketBuffLimit()
{
    return _iBackPacketBuffLimit;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 服务连接
TC_EpollServer::NetThread::Connection::Connection(TC_EpollServer::BindAdapter *pBindAdapter, int lfd, int timeout, int fd, const string& ip, uint16_t port)
: _pBindAdapter(pBindAdapter)
, _uid(0)
, _lfd(lfd)
, _timeout(timeout)
, _ip(ip)
, _port(port)
, _iHeaderLen(0)
, _bClose(false)
, _iMaxTemQueueSize(100)
, _enType(EM_TCP)
, _bEmptyConn(true)
, _pRecvBuffer(NULL)
, _nRecvBufferSize(DEFAULT_RECV_BUFFERSIZE)
, _authInit(false)

{
    assert(fd != -1);

    _iLastRefreshTime = TNOW;

    _sock.init(fd, true, AF_INET);
}

TC_EpollServer::NetThread::Connection::Connection(BindAdapter *pBindAdapter, int fd)
: _pBindAdapter(pBindAdapter)
, _uid(0)
, _lfd(-1)
, _timeout(2)
, _port(0)
, _iHeaderLen(0)
, _bClose(false)
, _iMaxTemQueueSize(100)
, _enType(EM_TCP)
,_bEmptyConn(false) /*udp is always false*/
,_pRecvBuffer(NULL)
,_nRecvBufferSize(DEFAULT_RECV_BUFFERSIZE)
,_authInit(false)

{
    _iLastRefreshTime = TNOW;

    _sock.init(fd, false, AF_INET);
}

TC_EpollServer::NetThread::Connection::Connection(BindAdapter *pBindAdapter)
: _pBindAdapter(pBindAdapter)
, _uid(0)
, _lfd(-1)
, _timeout(0)
, _port(0)
, _iHeaderLen(0)
, _bClose(false)
, _iMaxTemQueueSize(100)
, _enType(EM_TCP)
,_bEmptyConn(false) /*udp is always false*/
,_pRecvBuffer(NULL)
,_nRecvBufferSize(DEFAULT_RECV_BUFFERSIZE)
,_authInit(false)

{
    _iLastRefreshTime = TNOW;
}
TC_EpollServer::NetThread::Connection::~Connection()
{
    if(_pRecvBuffer)
    {
        delete _pRecvBuffer;
        _pRecvBuffer = NULL;
    }
    
    clearSlices(_sendbuffer);

    if(_lfd != -1)
    {
        assert(!_sock.isValid());
    }
}

void TC_EpollServer::NetThread::Connection::tryInitAuthState(int initState)
{
    if (!_authInit)
    {
        _authState = initState;
        _authInit = true;
    }
}

void TC_EpollServer::NetThread::Connection::close()
{
    if(_lfd != -1)
    {
#if TARS_SSL
        if (_openssl)
        {
            _openssl->Release();
            _openssl.reset();
        }
#endif
        if(_sock.isValid())
        {
            _sock.close();
        }
    }
}

void TC_EpollServer::NetThread::Connection::insertRecvQueue(recv_queue::queue_type &vRecvData)
{
    if(!vRecvData.empty())
    {
        int iRet = _pBindAdapter->isOverloadorDiscard();

        if(iRet == 0)//未过载
        {
            _pBindAdapter->insertRecvQueue(vRecvData);
        }
        else if(iRet == -1)//超过接受队列长度的一半，需要进行overload处理
        {
            recv_queue::queue_type::iterator it = vRecvData.begin();

            recv_queue::queue_type::iterator itEnd = vRecvData.end();

            while(it != itEnd)
            {
                (*it)->isOverload = true;

                ++it;
            }

            _pBindAdapter->insertRecvQueue(vRecvData,false);
        }
        else//接受队列满，需要丢弃
        {
            recv_queue::queue_type::iterator it = vRecvData.begin();

            recv_queue::queue_type::iterator itEnd = vRecvData.end();

            while(it != itEnd)
            {
                delete (*it);
                ++it;
            }
        }
    }
}

int TC_EpollServer::NetThread::Connection::parseProtocol(recv_queue::queue_type &o)
{
    try
    {
        while (true)
        {
            //需要过滤首包包头
            if(_iHeaderLen > 0)
            {
                if(_recvbuffer.length() >= (unsigned) _iHeaderLen)
                {
                    string header = _recvbuffer.substr(0, _iHeaderLen);
                    _pBindAdapter->getHeaderFilterFunctor()((int)(TC_EpollServer::PACKET_FULL), header);
                    _recvbuffer = _recvbuffer.substr(_iHeaderLen);
                    _iHeaderLen = 0;
                }
                else
                {
                    _pBindAdapter->getHeaderFilterFunctor()((int)(TC_EpollServer::PACKET_LESS), _recvbuffer);
                    _iHeaderLen -= _recvbuffer.length();
                    _recvbuffer = "";
                    break;
                }
            }

            std::string* rbuf = &_recvbuffer;
#if TARS_SSL
            // ssl connection
            if (_pBindAdapter->getEndpoint().isSSL())
            {
                std::string out;
                if (!_openssl->Read(_recvbuffer.data(), _recvbuffer.size(), out))
                {
                    _pBindAdapter->getEpollServer()->error("[TARS][SSL_read failed");
                    return -1;
                }
                else
                {
                    if (!out.empty())
                        this->send(out, "", 0);

                    rbuf = _openssl->RecvBuffer();
                }

                _recvbuffer.clear();
            }
#endif

            string ro;

            int b = _pBindAdapter->getProtocol()(*rbuf, ro);

            if(b == TC_EpollServer::PACKET_LESS)
            {
                //包不完全
                break;
            }
            else if(b == TC_EpollServer::PACKET_FULL)
            {
                if (_pBindAdapter->_authWrapper &&
                    _pBindAdapter->_authWrapper(this, ro))
                    continue;

                tagRecvData* recv = new tagRecvData();
                recv->buffer           = std::move(ro);
                recv->ip               = _ip;
                recv->port             = _port;
                recv->recvTimeStamp    = TNOWMS;
                recv->uid              = getId();
                recv->isOverload       = false;
                recv->isClosed         = false;
                recv->fd               = getfd();

                //收到完整的包才算
                this->_bEmptyConn = false;

                //收到完整包
                o.push_back(recv);

                if((int) o.size() > _iMaxTemQueueSize)
                {
                    insertRecvQueue(o);
                    o.clear();
                }

                if(rbuf->empty())
                {
                    break;
                }
            }
            else
            {
                _pBindAdapter->getEpollServer()->error("recv [" + _ip + ":" + TC_Common::tostr(_port) + "],packet error.");
                return -1;                      //协议解析错误
            }
        }
    }
    catch(exception &ex)
    {
        _pBindAdapter->getEpollServer()->error("recv protocol error:" + string(ex.what()));
        return -1;
    }
    catch(...)
    {
        _pBindAdapter->getEpollServer()->error("recv protocol error");
        return -1;
    }

    return o.size();
}

int TC_EpollServer::NetThread::Connection::recv(recv_queue::queue_type &o)
{
    o.clear();

    while(true)
    {
        char buffer[32 * 1024];
        int iBytesReceived = 0;

        if(_lfd == -1)
        {
            if(_pRecvBuffer)
            {
                iBytesReceived = _sock.recvfrom((void*)_pRecvBuffer,_nRecvBufferSize, _ip, _port, 0);
            }
            else
            {
                iBytesReceived = _sock.recvfrom((void*)buffer,sizeof(buffer), _ip, _port, 0);
            }
        }
        else
        {
            iBytesReceived = ::read(_sock.getfd(), (void*)buffer, sizeof(buffer));
        }

        if (iBytesReceived < 0)
        {
            if(errno == EAGAIN)
            {
                //没有数据了
                break;
            }
            else
            {
                //客户端主动关闭
                _pBindAdapter->getEpollServer()->debug("recv [" + _ip + ":" + TC_Common::tostr(_port) + "] close connection");
                return -1;
            }
        }
        else if( iBytesReceived == 0)
        {
            //客户端主动关闭
            _pBindAdapter->getEpollServer()->debug("recv [" + _ip + ":" + TC_Common::tostr(_port) + "] close connection");
            return -1;
        }

        //保存接收到数据
        if(_lfd == -1)
        {
            if(_pRecvBuffer)
            {
                _recvbuffer.append(_pRecvBuffer, iBytesReceived);
            }
            else
            {
                _recvbuffer.append(buffer, iBytesReceived);
            }
        }
        else
        {
            _recvbuffer.append(buffer, iBytesReceived);
        }

        //UDP协议
        if(_lfd == -1)
        {
            if(_pBindAdapter->isIpAllow(_ip) == true)
            {
                 parseProtocol(o);
            }
            else
            {
                 //udp ip无权限
                _pBindAdapter->getEpollServer()->debug("accept [" + _ip + ":" + TC_Common::tostr(_port) + "] [" + TC_Common::tostr(_lfd) + "] not allowed");
            }
            _recvbuffer = "";
        }
        else
        {
            //接收到数据不超过buffer,没有数据了(如果有数据,内核会再通知你)
            if((size_t)iBytesReceived < sizeof(buffer))
            {
                break;
            }
            //字符串太长时substr性能会急剧下降
            if(_recvbuffer.length() > 8192)
            {
                parseProtocol(o);
            }
        }
    }

    if(_lfd != -1)
    {
        return parseProtocol(o);
    }

    return o.size();
}

int TC_EpollServer::NetThread::Connection::send(const string& buffer, const string &ip, uint16_t port, bool byEpollOut)
{
    const bool isUdp = (_lfd == -1);
    if(isUdp)
    {
        int iRet = _sock.sendto((const void*) buffer.c_str(), buffer.length(), ip, port, 0);
        if(iRet < 0)
        {
            _pBindAdapter->getEpollServer()->error("[TC_EpollServer::Connection] send [" + _ip + ":" + TC_Common::tostr(_port) + "] error");
            return -1;
        }
        return 0;
    }

    if (byEpollOut)
    {
        int bytes = this->send(_sendbuffer);
        if (bytes == -1) 
        { 
            _pBindAdapter->getEpollServer()->debug("send [" + _ip + ":" + TC_Common::tostr(_port) + "] close connection by peer."); 
            return -1; 
        } 

        this->adjustSlices(_sendbuffer, bytes);
        _pBindAdapter->getEpollServer()->info("byEpollOut [" + _ip + ":" + TC_Common::tostr(_port) + "] send bytes " + TC_Common::tostr(bytes)); 
    }
    else
    {
        const size_t kChunkSize = 8 * 1024 * 1024;
        if (!_sendbuffer.empty()) 
        { 
            TC_BufferPool* pool = _pBindAdapter->getEpollServer()->getNetThreadOfFd(_sock.getfd())->_bufferPool;
            // avoid too big chunk
            for (size_t chunk = 0; chunk * kChunkSize < buffer.size(); chunk ++)
            {
                size_t needs = std::min<size_t>(kChunkSize, buffer.size() - chunk * kChunkSize);

                TC_Slice slice = pool->Allocate(needs);
                ::memcpy(slice.data, buffer.data() + chunk * kChunkSize, needs);
                slice.dataLen = needs;

                _sendbuffer.push_back(slice);
            }
        } 
        else 
        { 
            int bytes = this->tcpSend(buffer.data(), buffer.size()); 
            if (bytes == -1) 
            { 
                _pBindAdapter->getEpollServer()->debug("send [" + _ip + ":" + TC_Common::tostr(_port) + "] close connection by peer."); 
                return -1; 
            } 
            else if (bytes < static_cast<int>(buffer.size())) 
            { 
                const char* remainData = &buffer[bytes];
                const size_t remainLen = buffer.size() - static_cast<size_t>(bytes);
            
                TC_BufferPool* pool = _pBindAdapter->getEpollServer()->getNetThreadOfFd(_sock.getfd())->_bufferPool;
                // avoid too big chunk
                for (size_t chunk = 0; chunk * kChunkSize < remainLen; chunk ++)
                {
                    size_t needs = std::min<size_t>(kChunkSize, remainLen - chunk * kChunkSize);

                    TC_Slice slice = pool->Allocate(needs);
                    ::memcpy(slice.data, remainData + chunk * kChunkSize, needs);
                    slice.dataLen = needs;

                    _sendbuffer.push_back(slice);
                }
                // end
                _pBindAdapter->getEpollServer()->info("EAGAIN[" + _ip + ":" + TC_Common::tostr(_port) +
                        ", to sent bytes " + TC_Common::tostr(remainLen) +
                        ", total sent " + TC_Common::tostr(buffer.size()));
            } 
        } 
    }

    size_t toSendBytes = 0;
    for (const auto& slice : _sendbuffer)
    {
        toSendBytes += slice.dataLen;
    }

    if (toSendBytes >= 8 * 1024)
    {
        _pBindAdapter->getEpollServer()->info("big _sendbuffer > 8K");
        size_t iBackPacketBuffLimit = _pBindAdapter->getBackPacketBuffLimit();

        if(iBackPacketBuffLimit != 0 && toSendBytes >= iBackPacketBuffLimit)
        {
            _pBindAdapter->getEpollServer()->error("send [" + _ip + ":" + TC_Common::tostr(_port) + "] buffer too long close.");
            clearSlices(_sendbuffer);
            return -2;
        }
    }


    //需要关闭链接
    if(_bClose && _sendbuffer.empty())
    {
        _pBindAdapter->getEpollServer()->debug("send [" + _ip + ":" + TC_Common::tostr(_port) + "] close connection by user.");
        return -2;
    }

    return 0;
}

int TC_EpollServer::NetThread::Connection::send()
{
    if(_sendbuffer.empty()) return 0;

    return send("", _ip, _port, true);
}

int TC_EpollServer::NetThread::Connection::send(const std::vector<TC_Slice>& slices)
{
    const int kIOVecCount = std::max<int>(sysconf(_SC_IOV_MAX), 16); // be care of IOV_MAX

    size_t alreadySentVecs = 0;
    size_t alreadySentBytes = 0;
    while (alreadySentVecs < slices.size())
    {
        const size_t vc = std::min<int>(slices.size() - alreadySentVecs, kIOVecCount);

        // convert to iovec array
        std::vector<iovec> vecs;
        size_t expectSent = 0;
        for (size_t i = alreadySentVecs; i < alreadySentVecs + vc; ++ i)
        {
            assert (slices[i].dataLen > 0);

            iovec ivc;
            ivc.iov_base = slices[i].data;
            ivc.iov_len = slices[i].dataLen;
            expectSent += slices[i].dataLen;

            vecs.push_back(ivc);
        }

        int bytes = tcpWriteV(vecs);
        if (bytes == -1)
            return -1; // should close
        else if (bytes == 0)
            return alreadySentBytes; // EAGAIN
        else if (bytes == static_cast<int>(expectSent))
        {
            alreadySentBytes += bytes;
            alreadySentVecs += vc; // continue sent
        }
        else
        {
            assert (bytes > 0); // partial send
            alreadySentBytes += bytes;
            return alreadySentBytes;
        }
    }
                
    return alreadySentBytes;
}


int TC_EpollServer::NetThread::Connection::tcpSend(const void* data, size_t len)
{
    if (len == 0)
        return 0;

    int bytes = ::send(_sock.getfd(), data, len, 0);
    if (bytes == -1)
    {
        if (EAGAIN == errno)
            bytes = 0; 
                      
        if (EINTR == errno)
            bytes = 0; // try ::send later
    }

    return bytes;
}
            
int TC_EpollServer::NetThread::Connection::tcpWriteV(const std::vector<iovec>& buffers)
{
    const int kIOVecCount = std::max<int>(sysconf(_SC_IOV_MAX), 16); // be care of IOV_MAX
    const int cnt = static_cast<int>(buffers.size());

    assert (cnt <= kIOVecCount);
        
    const int sock = _sock.getfd();
        
    int bytes = static_cast<int>(::writev(sock, &buffers[0], cnt));
    if (bytes == -1)
    {
        assert (errno != EINVAL);
        if (errno == EAGAIN)
            return 0;

        return -1;  // can not send any more
    }
    else
    {
        return bytes;
    }
}

void TC_EpollServer::NetThread::Connection::clearSlices(std::vector<TC_Slice>& slices)
{
    adjustSlices(slices, std::numeric_limits<std::size_t>::max());
}

void TC_EpollServer::NetThread::Connection::adjustSlices(std::vector<TC_Slice>& slices, size_t toSkippedBytes)
{
    size_t skippedVecs = 0;
    for (size_t i = 0; i < slices.size(); ++ i)
    {
        assert (slices[i].dataLen > 0);
        if (toSkippedBytes >= slices[i].dataLen)
        {
            toSkippedBytes -= slices[i].dataLen;
            ++ skippedVecs;
        }
        else
        {
            if (toSkippedBytes != 0)
            {
                const char* src = (const char*)slices[i].data + toSkippedBytes;
                memmove(slices[i].data, src, slices[i].dataLen - toSkippedBytes);
                slices[i].dataLen -= toSkippedBytes;
            }

            break;
        }
    }

    // free to pool
    TC_BufferPool* pool = _pBindAdapter->getEpollServer()->getNetThreadOfFd(_sock.getfd())->_bufferPool;
    assert (pool);
    for (size_t i = 0; i < skippedVecs; ++ i)
    {
        pool->Deallocate(slices[i]);
    }

    slices.erase(slices.begin(), slices.begin() + skippedVecs);
}

bool TC_EpollServer::NetThread::Connection::setRecvBuffer(size_t nSize)
{
    //only udp type needs to malloc
    if(_lfd == -1 && !_pRecvBuffer)
    {
        _nRecvBufferSize = nSize;

        _pRecvBuffer = new char[_nRecvBufferSize];
        if(!_pRecvBuffer)
        {
            throw TC_Exception("adapter '" + _pBindAdapter->getName() + "' malloc udp receive buffer fail");
        }
    }
    return true;
}

bool TC_EpollServer::NetThread::Connection::setClose()
{
    _bClose = true;
    return _sendbuffer.empty();
}
////////////////////////////////////////////////////////////////
//
TC_EpollServer::NetThread::ConnectionList::ConnectionList(TC_EpollServer::NetThread *pEpollServer)
:_pEpollServer(pEpollServer)
,_total(0)
,_free_size(0)
,_vConn(NULL)
,_lastTimeoutTime(0)
,_iConnectionMagic(0)
{
}

void TC_EpollServer::NetThread::ConnectionList::init(uint32_t size, uint32_t iIndex)
{
    _lastTimeoutTime = TNOW;

    _total = size;

    _free_size  = 0;

    //初始化链接链表
    if(_vConn) delete[] _vConn;

    //分配total+1个空间(多分配一个空间, 第一个空间其实无效)
    _vConn = new list_data[_total+1];

    _iConnectionMagic   = ((((uint32_t)_lastTimeoutTime) << 26) & (0xFFFFFFFF << 26)) + ((iIndex << 22) & (0xFFFFFFFF << 22));//((uint32_t)_lastTimeoutTime) << 20;

    //free从1开始分配, 这个值为uid, 0保留为管道用, epollwait根据0判断是否是管道消息
    for(uint32_t i = 1; i <= _total; i++)
    {
        _vConn[i].first = NULL;

        _free.push_back(i);

        ++_free_size;
    }
}

uint32_t TC_EpollServer::NetThread::ConnectionList::getUniqId()
{
    TC_ThreadLock::Lock lock(*this);

    uint32_t uid = _free.front();

    assert(uid > 0 && uid <= _total);

    _free.pop_front();

    --_free_size;

    return _iConnectionMagic | uid;
}

TC_EpollServer::NetThread::Connection* TC_EpollServer::NetThread::ConnectionList::get(uint32_t uid)
{
    uint32_t magi = uid & (0xFFFFFFFF << 22);
    uid           = uid & (0x7FFFFFFF >> 9);

    if (magi != _iConnectionMagic) return NULL;

    return _vConn[uid].first;
}

void TC_EpollServer::NetThread::ConnectionList::add(Connection *cPtr, time_t iTimeOutStamp)
{
    TC_ThreadLock::Lock lock(*this);

    uint32_t muid = cPtr->getId();
    uint32_t magi = muid & (0xFFFFFFFF << 22);
    uint32_t uid  = muid & (0x7FFFFFFF >> 9);

    assert(magi == _iConnectionMagic && uid > 0 && uid <= _total && !_vConn[uid].first);

    _vConn[uid] = make_pair(cPtr, _tl.insert(make_pair(iTimeOutStamp, uid)));
}

void TC_EpollServer::NetThread::ConnectionList::refresh(uint32_t uid, time_t iTimeOutStamp)
{
    TC_ThreadLock::Lock lock(*this);

    uint32_t magi = uid & (0xFFFFFFFF << 22);
    uid           = uid & (0x7FFFFFFF >> 9);

    assert(magi == _iConnectionMagic && uid > 0 && uid <= _total && _vConn[uid].first);

    if(iTimeOutStamp - _vConn[uid].first->_iLastRefreshTime < 1)
    {
        return;
    }
    _vConn[uid].first->_iLastRefreshTime = iTimeOutStamp;

    //删除超时链表
    _tl.erase(_vConn[uid].second);

    _vConn[uid].second = _tl.insert(make_pair(iTimeOutStamp, uid));
}

void TC_EpollServer::NetThread::ConnectionList::checkTimeout(time_t iCurTime)
{
    //至少1s才能检查一次
    if(iCurTime - _lastTimeoutTime < 1)
    {
        return;
    }

    _lastTimeoutTime = iCurTime;

    TC_ThreadLock::Lock lock(*this);

    multimap<time_t, uint32_t>::iterator it = _tl.begin();

    while(it != _tl.end())
    {
        //已经检查到当前时间点了, 后续不用在检查了
        if(it->first > iCurTime)
        {
            break;
        }

        uint32_t uid = it->second;

        ++it;

        //udp的监听端口, 不做处理
        if(_vConn[uid].first->getListenfd() == -1)
        {
            continue;
        }

        //超时关闭
        _pEpollServer->delConnection(_vConn[uid].first, false,EM_SERVER_TIMEOUT_CLOSE);

        //从链表中删除
        _del(uid);
    }


    if(_pEpollServer->IsEmptyConnCheck())
    {
        it = _tl.begin();
        while(it != _tl.end())
        {
            uint32_t uid = it->second;

            //遍历所有的空连接
            if(_vConn[uid].first->IsEmptyConn())
            {
                //获取空连接的超时时间点
                time_t iEmptyTimeout = (it->first - _vConn[uid].first->getTimeout()) + (_pEpollServer->getEmptyConnTimeout()/1000);

                 //已经检查到当前时间点了, 后续不用在检查了
                if(iEmptyTimeout > iCurTime)
                {
                    break;
                }

                //udp的监听端口, 不做处理
                if(_vConn[uid].first->getListenfd() == -1)
                {
                    ++it;
                    continue;
                }

                //超时关闭
                _pEpollServer->delConnection(_vConn[uid].first, false,EM_SERVER_TIMEOUT_CLOSE);

                //从链表中删除
                _del(uid);
            }

            ++it;
        }
    }
}

vector<TC_EpollServer::ConnStatus> TC_EpollServer::NetThread::ConnectionList::getConnStatus(int lfd)
{
    vector<TC_EpollServer::ConnStatus> v;

    TC_ThreadLock::Lock lock(*this);

    for(size_t i = 1; i <= _total; i++)
    {
        //是当前监听端口的连接
        if(_vConn[i].first != NULL && _vConn[i].first->getListenfd() == lfd)
        {
            TC_EpollServer::ConnStatus cs;

            cs.iLastRefreshTime    = _vConn[i].first->_iLastRefreshTime;
            cs.ip                  = _vConn[i].first->getIp();
            cs.port                = _vConn[i].first->getPort();
            cs.timeout             = _vConn[i].first->getTimeout();
            cs.uid                 = _vConn[i].first->getId();

            v.push_back(cs);
        }
    }

    return v;
}

void TC_EpollServer::NetThread::ConnectionList::del(uint32_t uid)
{
    TC_ThreadLock::Lock lock(*this);

    uint32_t magi = uid & (0xFFFFFFFF << 22);
    uid           = uid & (0x7FFFFFFF >> 9);

    assert(magi == _iConnectionMagic && uid > 0 && uid <= _total && _vConn[uid].first);

    _del(uid);
}

void TC_EpollServer::NetThread::ConnectionList::_del(uint32_t uid)
{
    assert(uid > 0 && uid <= _total && _vConn[uid].first);

    _tl.erase(_vConn[uid].second);

    delete _vConn[uid].first;

    _vConn[uid].first = NULL;

    _free.push_back(uid);

    ++_free_size;
}

size_t TC_EpollServer::NetThread::ConnectionList::size()
{
    TC_ThreadLock::Lock lock(*this);

    return _total - _free_size;
}

//////////////////////////////NetThread//////////////////////////////////
TC_EpollServer::NetThread::NetThread(TC_EpollServer *epollServer)
: _epollServer(epollServer)
, _listSize(0)
, _bTerminate(false)
, _createEpoll(false)
, _handleStarted(false)
, _list(this)
, _hasUdp(false)
, _bEmptyConnAttackCheck(false)
, _iEmptyCheckTimeout(MIN_EMPTY_CONN_TIMEOUT)
, _nUdpRecvBufferSize(DEFAULT_RECV_BUFFERSIZE)
, _bufferPool(NULL)
{
    _shutdown.createSocket();

    _notify.createSocket();
}

TC_EpollServer::NetThread::~NetThread()
{
    for (auto& kv : _listeners)
    {
        ::close(kv.first);
    }
    _listeners.clear();

    delete _bufferPool;
}

map<int, TC_EpollServer::BindAdapterPtr> TC_EpollServer::NetThread::getListenSocketInfo()
{
    return _listeners;
}

void TC_EpollServer::NetThread::debug(const string &s)
{
    _epollServer->debug(s);
}

void TC_EpollServer::NetThread::info(const string &s)
{
    _epollServer->info(s);
}

void TC_EpollServer::NetThread::error(const string &s)
{
    _epollServer->error(s);
}

void TC_EpollServer::NetThread::EnAntiEmptyConnAttack(bool bEnable)
{
    _bEmptyConnAttackCheck  = bEnable;
}

void TC_EpollServer::NetThread::setEmptyConnTimeout(int timeout)
{
    _iEmptyCheckTimeout = (timeout>=MIN_EMPTY_CONN_TIMEOUT)?timeout:MIN_EMPTY_CONN_TIMEOUT;
}

void TC_EpollServer::NetThread::setUdpRecvBufferSize(size_t nSize)
{
    _nUdpRecvBufferSize = (nSize >= 8192 && nSize <=DEFAULT_RECV_BUFFERSIZE)?nSize:DEFAULT_RECV_BUFFERSIZE;
}

bool TC_EpollServer::NetThread::IsEmptyConnCheck() const
{
    return  _bEmptyConnAttackCheck;
}

int  TC_EpollServer::NetThread::getEmptyConnTimeout() const
{
    return _iEmptyCheckTimeout;
}

int  TC_EpollServer::NetThread::bind(BindAdapterPtr &lsPtr)
{
    for (const auto& kv : _listeners)
    {
        if(kv.second->getName() == lsPtr->getName())
        {
            throw TC_Exception("bind name '" + lsPtr->getName() + "' conflicts.");
        }
    }

    const TC_Endpoint &ep = lsPtr->getEndpoint();

    TC_Socket& s = lsPtr->getSocket();

    bind(ep, s);

    _listeners[s.getfd()] = lsPtr;

    return s.getfd();
}

TC_EpollServer::BindAdapterPtr TC_EpollServer::NetThread::getBindAdapter(const string &sName)
{
    for (const auto& kv : _listeners)
    {
        if(kv.second->getName() == sName)
            return kv.second;
    }

    return NULL;
}

void TC_EpollServer::NetThread::bind(const TC_Endpoint &ep, TC_Socket &s)
{
    int type = ep.isUnixLocal()?AF_LOCAL:AF_INET;

    if(ep.isTcp())
    {
        s.createSocket(SOCK_STREAM, type);
    }
    else
    {
        s.createSocket(SOCK_DGRAM, type);
    }

    if(ep.isUnixLocal())
    {
        s.bind(ep.getHost().c_str());
    }
    else
    {
        s.bind(ep.getHost(), ep.getPort());
    }

    if(ep.isTcp() && !ep.isUnixLocal())
    {
        s.listen(1024);
        s.setKeepAlive();
        s.setTcpNoDelay();
        //不要设置close wait否则http服务回包主动关闭连接会有问题
        s.setNoCloseWait();
    }

    s.setblock(false);
}

void TC_EpollServer::NetThread::createEpoll(uint32_t iIndex)
{
    if(!_createEpoll)
    {
        _createEpoll = true;

        // 创建本网络线程的内存池
        assert (!_bufferPool);
        _bufferPool = new TC_BufferPool(_poolMinBlockSize, _poolMaxBlockSize);
        _bufferPool->SetMaxBytes(_poolMaxBytes);

        //创建epoll
        _epoller.create(10240);

        _epoller.add(_shutdown.getfd(), H64(ET_CLOSE), EPOLLIN);
        _epoller.add(_notify.getfd(), H64(ET_NOTIFY), EPOLLIN);

        size_t maxAllConn   = 0;

        //监听socket
        for (const auto& kv : _listeners)
        {
            if(kv.second->getEndpoint().isTcp())
            {
                //获取最大连接数
                maxAllConn += kv.second->getMaxConns();

                _epoller.add(kv.first, H64(ET_LISTEN) | kv.first, EPOLLIN);
            }
            else
            {
                maxAllConn++;
                _hasUdp = true;
            }
        }

        if(maxAllConn == 0)
        {
            //当网络线程中listeners没有监听socket时，使用adapter中设置的最大连接数
            maxAllConn = _listSize;
        }

        if(maxAllConn >= (1 << 22))
        {
            error("createEpoll connection num: " + TC_Common::tostr(maxAllConn) + " >= " + TC_Common::tostr(1 << 22));
            maxAllConn = (1 << 22) - 1;
        }

        //初始化连接管理链表
        _list.init(maxAllConn, iIndex);
    }
}

void TC_EpollServer::NetThread::initUdp()
{
    if(_hasUdp)
    {
        //监听socket
        for (const auto& kv : _listeners)
        {
            if (!kv.second->getEndpoint().isTcp())
            {
                Connection *cPtr = new Connection(kv.second.get(), kv.first);
                //udp分配接收buffer
                cPtr->setRecvBuffer(_nUdpRecvBufferSize);

                _epollServer->addConnection(cPtr, kv.first, UDP_CONNECTION);
            }
        }
    }
}

void TC_EpollServer::NetThread::terminate()
{
    _bTerminate = true;

    //通知队列醒过来
    _sbuffer.notifyT();

    //通知epoll响应, 关闭连接
    _epoller.mod(_shutdown.getfd(), H64(ET_CLOSE), EPOLLOUT);
}

bool TC_EpollServer::NetThread::accept(int fd)
{
    struct sockaddr_in stSockAddr;

    socklen_t iSockAddrSize = sizeof(sockaddr_in);

    TC_Socket cs;

    cs.setOwner(false);

    //接收连接
    TC_Socket s;

    s.init(fd, false, AF_INET);

    int iRetCode = s.accept(cs, (struct sockaddr *) &stSockAddr, iSockAddrSize);

    if (iRetCode > 0)
    {
        string  ip;

        uint16_t port;

        char sAddr[INET_ADDRSTRLEN] = "\0";

        struct sockaddr_in *p = (struct sockaddr_in *)&stSockAddr;

        inet_ntop(AF_INET, &p->sin_addr, sAddr, sizeof(sAddr));

        ip      = sAddr;
        port    = ntohs(p->sin_port);

        debug("accept [" + ip + ":" + TC_Common::tostr(port) + "] [" + TC_Common::tostr(cs.getfd()) + "] incomming");

        if(!_listeners[fd]->isIpAllow(ip))
        {
            debug("accept [" + ip + ":" + TC_Common::tostr(port) + "] [" + TC_Common::tostr(cs.getfd()) + "] not allowed");

            cs.close();

            return true;
        }

        if(_listeners[fd]->isLimitMaxConnection())
        {
            cs.close();

            error("accept [" + ip + ":" + TC_Common::tostr(port) + "][" + TC_Common::tostr(cs.getfd()) + "] beyond max connection:" + TC_Common::tostr(_listeners[fd]->getMaxConns()));

            return true;
        }

        cs.setblock(false);
        cs.setKeepAlive();
        cs.setTcpNoDelay();
        cs.setCloseWaitDefault();

        int timeout = _listeners[fd]->getEndpoint().getTimeout()/1000;

        Connection *cPtr = new Connection(_listeners[fd].get(), fd, (timeout < 2 ? 2 : timeout), cs.getfd(), ip, port);

        //过滤连接首个数据包包头
        cPtr->setHeaderFilterLen(_listeners[fd]->getHeaderFilterLen());

        //addTcpConnection(cPtr);
        _epollServer->addConnection(cPtr, cs.getfd(), TCP_CONNECTION);

        return true;
    }
    else
    {
        //直到发生EAGAIN才不继续accept
        if(errno == EAGAIN)
        {
            return false;
        }

        return true;
    }
    return true;
}

void TC_EpollServer::NetThread::addTcpConnection(TC_EpollServer::NetThread::Connection *cPtr)
{
    uint32_t uid = _list.getUniqId();

    cPtr->init(uid);

    _list.add(cPtr, cPtr->getTimeout() + TNOW);

    cPtr->getBindAdapter()->increaseNowConnection();

#if TARS_SSL
    if (cPtr->getBindAdapter()->getEndpoint().isSSL())
    {
        cPtr->getBindAdapter()->getEpollServer()->info("[TARS][addTcpConnection ssl connection");

        // 分配ssl对象, ctxName 放在obj proxy里
        SSL* ssl = NewSSL("server");
        if (!ssl)
        {
            cPtr->getBindAdapter()->getEpollServer()->error("[TARS][SSL_accept not find server cert");
            this->close(uid);
            return;
        }

        cPtr->_openssl.reset(new TC_OpenSSL());
        cPtr->_openssl->Init(ssl, true);
        std::string out = cPtr->_openssl->DoHandshake();
        if (cPtr->_openssl->HasError())
        {
            cPtr->getBindAdapter()->getEpollServer()->error("[TARS][SSL_accept error: " + cPtr->getBindAdapter()->getEndpoint().toString());
            this->close(uid);
            return;
        }
    
        // send the encrypt data from write buffer
        if (!out.empty())
        {
            this->sendBuffer(cPtr, out, "", 0);
        }
    }
#endif
    //注意epoll add必须放在最后, 否则可能导致执行完, 才调用上面语句
    _epoller.add(cPtr->getfd(), cPtr->getId(), EPOLLIN | EPOLLOUT);
}

void TC_EpollServer::NetThread::addUdpConnection(TC_EpollServer::NetThread::Connection *cPtr)
{
    uint32_t uid = _list.getUniqId();

    cPtr->init(uid);

    _list.add(cPtr, cPtr->getTimeout() + TNOW);

    _epoller.add(cPtr->getfd(), cPtr->getId(), EPOLLIN | EPOLLOUT);
}

vector<TC_EpollServer::ConnStatus> TC_EpollServer::NetThread::getConnStatus(int lfd)
{
    return _list.getConnStatus(lfd);
}

void TC_EpollServer::NetThread::delConnection(TC_EpollServer::NetThread::Connection *cPtr, bool bEraseList,EM_CLOSE_T closeType)
{
    //如果是TCP的连接才真正的关闭连接
    if (cPtr->getListenfd() != -1)
    {
        //false的情况,是超时被主动删除
        if(!bEraseList)
        {
            info("timeout [" + cPtr->getIp() + ":" + TC_Common::tostr(cPtr->getPort()) + "] del from list");
        }

        uint32_t uid = cPtr->getId();

        //构造一个tagRecvData，通知业务该连接的关闭事件

        tagRecvData* recv = new tagRecvData();
        recv->adapter    = cPtr->getBindAdapter();
        recv->uid        =  uid;
        recv->ip         = cPtr->getIp();
        recv->port       = cPtr->getPort();
        recv->isClosed   = true;
        recv->isOverload = false;
        recv->recvTimeStamp = TNOWMS;
        recv->fd         = cPtr->getfd();
        recv->closeType = (int)closeType;

        recv_queue::queue_type vRecvData;

        vRecvData.push_back(recv);

        cPtr->getBindAdapter()->insertRecvQueue(vRecvData);

        cPtr->getBindAdapter()->decreaseNowConnection();

        //从epoller删除句柄放在close之前, 否则重用socket时会有问题
        _epoller.del(cPtr->getfd(), uid, 0);

        cPtr->close();

        //对于超时检查, 由于锁的原因, 在这里不从链表中删除
        if(bEraseList)
        {
            _list.del(uid);
        }
    }
}

int  TC_EpollServer::NetThread::sendBuffer(TC_EpollServer::NetThread::Connection *cPtr, const string &buffer, const string &ip, uint16_t port)
{
    return cPtr->send(buffer, ip, port);
}

int  TC_EpollServer::NetThread::sendBuffer(TC_EpollServer::NetThread::Connection *cPtr)
{
    return cPtr->send();
}

int  TC_EpollServer::NetThread::recvBuffer(TC_EpollServer::NetThread::Connection *cPtr, recv_queue::queue_type &v)
{
    return cPtr->recv(v);
}

void TC_EpollServer::NetThread::close(uint32_t uid)
{
    tagSendData* send = new tagSendData();

    send->uid = uid;

    send->cmd = 'c';

    _sbuffer.push_back(send);

    //通知epoll响应, 关闭连接
    _epoller.mod(_notify.getfd(), H64(ET_NOTIFY), EPOLLOUT);
}

void TC_EpollServer::NetThread::send(uint32_t uid, const string &s, const string &ip, uint16_t port)
{
    if(_bTerminate)
    {
        return;
    }

    tagSendData* send = new tagSendData();

    send->uid = uid;

    send->cmd = 's';

    send->buffer = s;

    send->ip = ip;

    send->port = port;

    _sbuffer.push_back(send);

    //通知epoll响应, 有数据要发送
    _epoller.mod(_notify.getfd(), H64(ET_NOTIFY), EPOLLOUT);
}

void TC_EpollServer::NetThread::processPipe()
{
    send_queue::queue_type deSendData;

    _sbuffer.swap(deSendData);

    send_queue::queue_type::iterator it = deSendData.begin();

    send_queue::queue_type::iterator itEnd = deSendData.end();

    while(it != itEnd)
    {
        switch((*it)->cmd)
        {
        case 'c':
            {
                Connection *cPtr = getConnectionPtr((*it)->uid);

                if(cPtr)
                {
                    if(cPtr->setClose())
                    {
                        delConnection(cPtr,true,EM_SERVER_CLOSE);
                    }
                }
                break;
            }
        case 's':
            {
                Connection *cPtr = getConnectionPtr((*it)->uid);

                if(cPtr)
                {
#if TARS_SSL
                    if (cPtr->getBindAdapter()->getEndpoint().isSSL() && cPtr->_openssl->IsHandshaked())
                    {
                        std::string out = cPtr->_openssl->Write((*it)->buffer.data(), (*it)->buffer.size());
                        if (cPtr->_openssl->HasError())
                            break; // should not happen
    
                        (*it)->buffer = out;
                    }
#endif
                    int ret = sendBuffer(cPtr, (*it)->buffer, (*it)->ip, (*it)->port);

                    if(ret < 0)
                    {
                        delConnection(cPtr,true,(ret==-1)?EM_CLIENT_CLOSE:EM_SERVER_CLOSE);
                    }
                }
                break;
            }
        default:
            assert(false);
        }
        delete (*it);
        ++it;
    }
}

void TC_EpollServer::NetThread::processNet(const epoll_event &ev)
{
    uint32_t uid = ev.data.u32;

    Connection *cPtr = getConnectionPtr(uid);

    if(!cPtr)
    {
        debug("TC_EpollServer::NetThread::processNet connection[" + TC_Common::tostr(uid) + "] not exists.");
        return;
    }

    if (ev.events & EPOLLERR || ev.events & EPOLLHUP)
    {
        delConnection(cPtr,true,EM_SERVER_CLOSE);

        return;
    }

    if(ev.events & EPOLLIN)               //有数据需要读取
    {
        recv_queue::queue_type vRecvData;

        int ret = recvBuffer(cPtr, vRecvData);

        if(ret < 0)
        {
            delConnection(cPtr,true,EM_CLIENT_CLOSE);

            return;
        }

        if(!vRecvData.empty())
        {
            cPtr->insertRecvQueue(vRecvData);
        }
    }

    if (ev.events & EPOLLOUT)              //有数据需要发送
    {
        int ret = sendBuffer(cPtr);

        if (ret < 0)
        {
            delConnection(cPtr,true,(ret==-1)?EM_CLIENT_CLOSE:EM_SERVER_CLOSE);

            return;
        }
    }

    _list.refresh(uid, cPtr->getTimeout() + TNOW);
}

void TC_EpollServer::NetThread::run()
{
    //循环监听网路连接请求
    while(!_bTerminate)
    {
        _list.checkTimeout(TNOW);

        int iEvNum = _epoller.wait(2000);

        for(int i = 0; i < iEvNum; ++i)
        {
            try
            {
                const epoll_event &ev = _epoller.get(i);

                uint32_t h = ev.data.u64 >> 32;

                switch(h)
                {
                case ET_LISTEN:
                    {
                        //监听端口有请求
                        auto it = _listeners.find(ev.data.u32);
                        if( it != _listeners.end())
                        {
                            if(ev.events & EPOLLIN)
                            {
                                bool ret;
                                do
                                {
                                    ret = accept(ev.data.u32);
                                }while(ret);
                            }
                        }
                    }
                    break;
                case ET_CLOSE:
                    //关闭请求
                    break;
                case ET_NOTIFY:
                    //发送通知
                    processPipe();
                    break;
                case ET_NET:
                    //网络请求
                    processNet(ev);
                    break;
                default:
                    assert(true);
                }
            }
            catch(exception &ex)
            {
                error("run exception:" + string(ex.what()));
            }
        }
    }
}
size_t TC_EpollServer::NetThread::getSendRspSize()
{
    return _sbuffer.size();
}
//////////////////////////////////////////////////////////////
TC_EpollServer::TC_EpollServer(unsigned int iNetThreadNum)
: _pReportRspQueue(NULL)
, _netThreadNum(iNetThreadNum)
, _bTerminate(false)
, _handleStarted(false)
, _pLocalLogger(NULL)
{
    if(_netThreadNum < 1)
    {
        _netThreadNum = 1;
    }

    //网络线程的配置数目不能15个
    if(_netThreadNum > 15)
    {
        _netThreadNum = 15;
    }

    for (size_t i = 0; i < _netThreadNum; ++i)
    {
        TC_EpollServer::NetThread* netThreads = new TC_EpollServer::NetThread(this);
        _netThreads.push_back(netThreads);
    }
}

TC_EpollServer::~TC_EpollServer()
{
    terminate();
}

void TC_EpollServer::waitForShutdown()
{
    for (size_t i = 0; i < _netThreadNum; ++i)
    {
        _netThreads[i]->start();
    }

    debug("server netthread num : " + TC_Common::tostr(_netThreadNum));

    while(!_bTerminate)
    {
        {
            TC_ThreadLock::Lock sync(*this);
            timedWait(5000);
        }
    }

    if(_bTerminate)
    {
        for(size_t i = 0; i < _netThreads.size(); ++i)
        {
            _netThreads[i]->terminate();
            _netThreads[i]->getThreadControl().join();
        }

        stopThread();
    }
}

void TC_EpollServer::terminate()
{
    if(!_bTerminate)
    {
        tars::TC_ThreadLock::Lock sync(*this);
        _bTerminate = true;
        notifyAll();
    }
}
void TC_EpollServer::EnAntiEmptyConnAttack(bool bEnable)
{
    for(size_t i = 0; i < _netThreads.size(); ++i)
    {
        _netThreads[i]->EnAntiEmptyConnAttack(bEnable);
    }
}

void TC_EpollServer::setEmptyConnTimeout(int timeout)
{
    for(size_t i = 0; i < _netThreads.size(); ++i)
    {
        _netThreads[i]->setEmptyConnTimeout(timeout);
    }
}

void TC_EpollServer::setNetThreadBufferPoolInfo(size_t minBlock, size_t maxBlock, size_t maxBytes)
{
    for(size_t i = 0; i < _netThreads.size(); ++i)
    {
        _netThreads[i]->_poolMinBlockSize = minBlock;
        _netThreads[i]->_poolMaxBlockSize = maxBlock;
        _netThreads[i]->_poolMaxBytes = maxBytes;
    }
}

int  TC_EpollServer::bind(TC_EpollServer::BindAdapterPtr &lsPtr)
{
    int iRet = 0;

    for(size_t i = 0; i < _netThreads.size(); ++i)
    {
        if(i == 0)
        {
            iRet = _netThreads[i]->bind(lsPtr);
        }
        else
        {
            //当网络线程中listeners没有监听socket时，list使用adapter中设置的最大连接数作为初始化
            _netThreads[i]->setListSize(lsPtr->getMaxConns());
        }
    }

    return iRet;
}

void TC_EpollServer::addConnection(TC_EpollServer::NetThread::Connection * cPtr, int fd, int iType)
{
    TC_EpollServer::NetThread* netThread = getNetThreadOfFd(fd);

    if(iType == 0)
    {
        netThread->addTcpConnection(cPtr);
    }
    else
    {
        netThread->addUdpConnection(cPtr);
    }
}

void TC_EpollServer::startHandle()
{
    if (!_handleStarted)
    {
        _handleStarted = true;

        for (auto& kv : _handleGroups)
        {
            auto& hds = kv.second->handles;

            for (auto& handle : hds)
            {
                if (!handle->isAlive())
                    handle->start();
            }
        }
    }
}

void TC_EpollServer::stopThread()
{
    for (auto& kv : _handleGroups)
    {
        {
            TC_ThreadLock::Lock lock(kv.second->monitor);

            kv.second->monitor.notifyAll();
        }
        auto& hds = kv.second->handles;
        for (auto& handle : hds)
        {
            if (handle->isAlive())
            {
                handle->getThreadControl().join();
            }
        }
    }
}

void TC_EpollServer::createEpoll()
{
    for(size_t i = 0; i < _netThreads.size(); ++i)
    {
        _netThreads[i]->createEpoll(i+1);
    }
    //必须先等所有网络线程调用createEpoll()，初始化list后，才能调用initUdp()
    for(size_t i = 0; i < _netThreads.size(); ++i)
    {
        _netThreads[i]->initUdp();
    }
}

TC_EpollServer::BindAdapterPtr TC_EpollServer::getBindAdapter(const string &sName)
{
    for(size_t i = 0; i < _netThreads.size(); ++i)
    {
        TC_EpollServer::BindAdapterPtr ptr = _netThreads[i]->getBindAdapter(sName);
        if(ptr)
        {
            return ptr;
        }

    }

    return NULL;
}
void TC_EpollServer::close(unsigned int uid, int fd)
{
    TC_EpollServer::NetThread* netThread = getNetThreadOfFd(fd);

    netThread->close(uid);
}

void TC_EpollServer::send(unsigned int uid, const string &s, const string &ip, uint16_t port, int fd)
{
    TC_EpollServer::NetThread* netThread = getNetThreadOfFd(fd);

    netThread->send(uid, s, ip, port);
}

void TC_EpollServer::debug(const string &s)
{
    if(_pLocalLogger)
    {
        _pLocalLogger->debug() << "[TARS]" << s << endl;
    }
}

void TC_EpollServer::info(const string &s)
{
    if(_pLocalLogger)
    {
        _pLocalLogger->info() << "[TARS]" << s << endl;
    }
}

void TC_EpollServer::error(const string &s)
{
    if(_pLocalLogger)
    {
        _pLocalLogger->error() << "[TARS]" << s << endl;
    }
}

vector<TC_EpollServer::ConnStatus> TC_EpollServer::getConnStatus(int lfd)
{
    vector<TC_EpollServer::ConnStatus> vConnStatus;
    for(size_t i = 0; i < _netThreads.size(); ++i)
    {
        vector<TC_EpollServer::ConnStatus> tmp = _netThreads[i]->getConnStatus(lfd);
        for(size_t k = 0; k < tmp.size(); ++k)
        {
            vConnStatus.push_back(tmp[k]);
        }
    }
    return vConnStatus;
}

map<int, TC_EpollServer::BindAdapterPtr> TC_EpollServer::getListenSocketInfo()
{
    map<int, TC_EpollServer::BindAdapterPtr> mListen;
    for(size_t i = 0; i < _netThreads.size(); ++i)
    {
        auto tmp = _netThreads[i]->getListenSocketInfo();
        for (const auto& kv : tmp)
        {
            mListen.insert(kv);
        }
    }
    return mListen;
}

size_t TC_EpollServer::getConnectionCount()
{
    size_t iConnTotal = 0;
    for(size_t i = 0; i < _netThreads.size(); ++i)
    {
        iConnTotal += _netThreads[i]->getConnectionCount();
    }
    return iConnTotal;
}

unsigned int TC_EpollServer::getLogicThreadNum()
{
    unsigned int iNum = 0;
    for (const auto& kv : _handleGroups)
    {
        iNum += kv.second->handles.size();
    }

    return iNum;
}

}
