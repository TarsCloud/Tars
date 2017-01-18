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

#include "servant/Transceiver.h"
#include "servant/AdapterProxy.h"
#include "servant/Application.h"
#include "servant/TarsLogger.h"

namespace tars
{
///////////////////////////////////////////////////////////////////////
Transceiver::Transceiver(AdapterProxy * pAdapterProxy,const EndpointInfo &ep)
: _adapterProxy(pAdapterProxy)
, _ep(ep)
, _fd(-1)
, _connStatus(eUnconnected)
, _conTimeoutTime(0)
{
    //预分配好一定的内存
    _sendBuffer.reserve(1024);

    _fdInfo.iType = FDInfo::ET_C_NET;
    _fdInfo.p     = (void *)this;
    _fdInfo.fd    = -1;
}

Transceiver::~Transceiver()
{
    close();
}

void Transceiver::checkTimeout()
{
    if(eConnecting == _connStatus && TNOWMS > _conTimeoutTime)
    {
        //链接超时
        TLOGERROR("[TARS][Transceiver::checkTimeout ep:"<<_adapterProxy->endpoint().desc()<<" , connect timeout]"<<endl);
        _adapterProxy->setConTimeout(true);
        close();
    }
}

void Transceiver::reconnect()
{
    close();

    connect();
}

void Transceiver::connect()
{
    if(isValid())
    {
        return;
    }

    if(_connStatus == eConnecting || _connStatus == eConnected)
    {
        return;
    }

    int fd = -1;

    if (_ep.type() == EndpointInfo::UDP)
    {
        fd = NetworkUtil::createSocket(true);
        NetworkUtil::setBlock(fd, false);
        _connStatus = eConnected;
    }
    else
    {
        fd = NetworkUtil::createSocket(false);
        NetworkUtil::setBlock(fd, false);

        bool bConnected = NetworkUtil::doConnect(fd, _ep.addr());
        if(bConnected)
        {
            setConnected();
        }
        else
        {
            _connStatus     = Transceiver::eConnecting;
            _conTimeoutTime = TNOWMS + _adapterProxy->getConTimeout();
        }
    }

    _fd = fd;

    TLOGINFO("[TARS][Transceiver::connect objname:" << _adapterProxy->getObjProxy()->name() 
        << ",connect:" << _ep.desc() << ",fd:" << _fd << "]" << endl);

    //设置网络qos的dscp标志
    if(0 != _ep.qos())
    {
        int iQos=_ep.qos();
        ::setsockopt(fd,SOL_IP,IP_TOS,&iQos,sizeof(iQos));
    }

    //设置套接口选项
    vector<SocketOpt> &socketOpts = _adapterProxy->getObjProxy()->getSocketOpt();
    for(size_t i=0; i<socketOpts.size(); ++i)
    {
        if(setsockopt(_fd,socketOpts[i].level,socketOpts[i].optname,socketOpts[i].optval,socketOpts[i].optlen) == -1)
        {
            TLOGERROR("[TARS][setsockopt error:" << NetworkUtil::errorToString(errno) 
                << ",objname:" << _adapterProxy->getObjProxy()->name() 
                << ",desc:" << _ep.desc()
                << ",fd:" << _fd
                << ",level:" <<  socketOpts[i].level
                << ",optname:" << socketOpts[i].optname
                << ",optval:" << socketOpts[i].optval
                <<"    ]"<< endl);
        }
    }

    _adapterProxy->getObjProxy()->getCommunicatorEpoll()->addFd(fd, &_fdInfo, EPOLLIN|EPOLLOUT);
}

void Transceiver::setConnected()
{
    _connStatus = eConnected;
    _adapterProxy->setConTimeout(false);
    _adapterProxy->addConnExc(false);
}

void Transceiver::close()
{
    if(!isValid()) return;

    _adapterProxy->getObjProxy()->getCommunicatorEpoll()->delFd(_fd,&_fdInfo,EPOLLIN|EPOLLOUT);

    NetworkUtil::closeSocketNoThrow(_fd);

    _connStatus = eUnconnected;

    _fd = -1;

    _sendBuffer.clear();

    _recvBuffer.clear();

    TLOGINFO("[TARS][trans close:"<< _adapterProxy->getObjProxy()->name()<< "," << _ep.desc() << "]" << endl);
}

int Transceiver::doRequest()
{
    if(!isValid())
    {
        return -1;
    }

    int iRet = 0;

    //buf不为空,先发生buffer的内容
    if(!_sendBuffer.empty())
    {
        size_t length = _sendBuffer.length();

        iRet = this->send(_sendBuffer.c_str(), _sendBuffer.size(), 0);

        //失败，直接返回
        if(iRet < 0)
        {
            return iRet;
        }

        if(iRet > 0)
        {
            if((size_t)iRet == length)
            {
                _sendBuffer.clear();
            }
            else
            {
                _sendBuffer.erase(_sendBuffer.begin(), _sendBuffer.begin()+iRet);
                return 0;
            }
        }
    }

    //取adapter里面积攒的数据
    _adapterProxy->doInvoke();

    //object里面应该是空的
    assert(_adapterProxy->getObjProxy()->timeoutQSize()  == 0);
    //_adapterProxy->getObjProxy()->doInvoke();

    return 0;
}

int Transceiver::sendRequest(const char * pData, size_t iSize)
{
    if(_connStatus != eConnected)
    {
        return eRetError;
    }

    //空数据 直接返回成功
    if(iSize == 0)
    {
        return eRetOk;
    }

    //buf不为空,直接返回失败,等buffer可写了,epoll会通知写时间
    if(!_sendBuffer.empty())
    {
        return eRetError;
    }

    int iRet = this->send(pData,iSize,0);

    //失败，直接返回
    if(iRet < 0)
    {
        return eRetError;
    }

    //没有全部发送完,写buffer 返回成功
    if(iRet < (int)iSize)
    {
        _sendBuffer.assign(pData+iRet, iSize-iRet);
        return eRetFull;
    }

    return eRetOk;
}

//////////////////////////////////////////////////////////
TcpTransceiver::TcpTransceiver(AdapterProxy * pAdapterProxy, const EndpointInfo &ep)
: Transceiver(pAdapterProxy, ep)
{
    //预分配好一定的内存
    _recvBuffer.reserve(1024);
}

int TcpTransceiver::doResponse(list<ResponsePacket>& done)
{
    if(!isValid())
    {
        return -1;
    }

    int iRet = 0;

    done.clear();

    char buff[8192] = {0};

    do
    {
        if ((iRet = this->recv(buff, sizeof(buff), 0)) > 0)
        {
            _recvBuffer.append(buff,iRet);
        }
    }
    while (iRet>0);

    TLOGINFO("[TARS][tcp doResponse objname:" << _adapterProxy->getObjProxy()->name() 
        << ",fd:" << _fd << ",recvbuf:" << _recvBuffer.length() << "]" << endl);

    if(!_recvBuffer.empty())
    {
        try
        {
            size_t pos = _adapterProxy->getObjProxy()->getProxyProtocol().responseFunc(
                _recvBuffer.c_str(),
                _recvBuffer.length(), done);

            if(pos > 0)
            {
                //用erase, 不用substr, 从而可以保留预分配的空间
                _recvBuffer.erase(_recvBuffer.begin(), _recvBuffer.begin() + min(pos, _recvBuffer.length()));

                if(_recvBuffer.capacity() - _recvBuffer.length() > 102400)
                {
                   _recvBuffer.reserve(max(_recvBuffer.length(),(size_t)1024));
                }
            }
        }
        catch (exception &ex)
        {
            TLOGERROR("[TARS][tcp doResponse objname:" << _adapterProxy->getObjProxy()->name() 
                << ",fd:" << _fd << ",desc:" << _ep.desc() << ",tcp recv decode error:" << ex.what() << endl);

            close();
        }
        catch (...)
        {
            TLOGERROR("[TARS][tcp doResponse objname:" << _adapterProxy->getObjProxy()->name() 
                << ",fd:" << _fd << ",desc" << _ep.desc() << ",tcp recv decode error." << endl);

            close();
        }
    }

    return done.empty()?0:1;
}

int TcpTransceiver::send(const void* buf, uint32_t len, uint32_t flag)
{
    //只有是连接状态才能收发数据
    if(_connStatus != eConnected)
    {
        return -1;
    }

    int iRet = ::send(_fd, buf, len, flag);

    if (iRet < 0 && errno != EAGAIN)
    {
        TLOGINFO("[TARS][tcp send objname:" << _adapterProxy->getObjProxy()->name() 
            << ",fd:" << _fd << ",desc:" << _ep.desc() 
            << ",fail! errno:" << errno << "," << strerror(errno) << ",close]" << endl);

        close();

        return iRet;
    }

    TLOGINFO("[TARS][tcp send," << _adapterProxy->getObjProxy()->name() << ",fd:" << _fd 
        << ",desc:" << _ep.desc() << ",len:" << iRet << "]" << endl);

    return iRet;
}

int TcpTransceiver::recv(void* buf, uint32_t len, uint32_t flag)
{
    //只有是连接状态才能收发数据
    if(_connStatus != eConnected)
    {
        return -1;
    }

    int iRet = ::recv(_fd, buf, len, flag);

    if (iRet == 0 || (iRet < 0 && errno != EAGAIN))
    {
        TLOGINFO("[TARS][tcp recv objname:" << _adapterProxy->getObjProxy()->name()
            << ",fd:" << _fd << ", " << _ep.desc() <<",ret " << iRet
            << ", fail! errno:" << errno << "," << strerror(errno) << ",close]" << endl);

        close();

        return 0;
    }

    TLOGINFO("[TARS][tcp recv objname:" << _adapterProxy->getObjProxy()->name()
        << ",fd:" << _fd << ",desc:" << _ep.desc() << ",ret:" << iRet << "]" << endl);

    return iRet;
}

/////////////////////////////////////////////////////////////////
UdpTransceiver::UdpTransceiver(AdapterProxy * pAdapterProxy, const EndpointInfo &ep)
: Transceiver(pAdapterProxy, ep)
, _recvBuffer(NULL)
{
    if(!_recvBuffer)
    {
        _recvBuffer = new char[DEFAULT_RECV_BUFFERSIZE];
        if(!_recvBuffer)
        {
            throw TC_Exception("objproxy '" + _adapterProxy->getObjProxy()->name() + "' malloc udp receive buffer fail");
        }
    }

}

UdpTransceiver::~UdpTransceiver()
{
    if(!_recvBuffer)
    {
        delete _recvBuffer;
        _recvBuffer = NULL;
    }
}

int UdpTransceiver::doResponse(list<ResponsePacket>& done)
{
    if(!isValid())
    {
        return -1;
    }

    int recv = 0;

    done.clear();
    do
    {
        if ((recv = this->recv(_recvBuffer, DEFAULT_RECV_BUFFERSIZE, 0)) > 0)
        {
            TLOGINFO("[TARS][udp doResponse objname:" << _adapterProxy->getObjProxy()->name()
                << ",fd:" << _fd << ",recvbuf:" << recv << "]" << endl);

            try
            {
                _adapterProxy->getObjProxy()->getProxyProtocol().responseFunc(_recvBuffer, recv, done);
            }
            catch (exception &ex)
            {
                TLOGERROR("[TARS][udp doResponse, " << _adapterProxy->getObjProxy()->name()
                    << ",fd:" << _fd << ",desc:" << _ep.desc()
                    << ", udp recv decode error:" << ex.what() << endl);
            }
            catch (...)
            {
                TLOGERROR("[TARS][udp doResponse, " << _adapterProxy->getObjProxy()->name()
                    << ",fd:" << _fd << ",desc:" << _ep.desc()
                    << ", udp recv decode error." << endl);
            }
        }
    }
    while (recv > 0);

    return done.empty()?0:1;
}

int UdpTransceiver::send(const void* buf, uint32_t len, uint32_t flag)
{
    if(!isValid())
    {
        return -1;
    }

    int iRet = ::sendto(_fd, buf, len, flag, (struct sockaddr*) &(_ep.addr()), sizeof(sockaddr));

    if (iRet<0)
    {
        if(errno != EAGAIN)
        {
            TLOGERROR("[TARS][udp send objname:" << _adapterProxy->getObjProxy()->name()
                << ",fd:" << _fd << ",desc:" << _ep.desc() 
                << ", fail! errno:" << errno << "," << strerror(errno) << ",close]" << endl);

            close();

            return iRet;
        }

        iRet = 0;
    }
    else if(iRet > 0 && iRet != (int)len)
    {
        TLOGERROR("[TARS][udp send objname:" << _adapterProxy->getObjProxy()->name()
                << ",fd:" << _fd << "," << _ep.desc() << ", send error."
                << ", len:" << len << ", sendLen:" << iRet << endl);

        //udp只发一次 发送一半也算全部发送成功
        iRet = len;
    }

    return iRet;
}

int UdpTransceiver::recv(void* buf, uint32_t len, uint32_t flag)
{
    if(!isValid())
    {
        return -1;
    }

    int iRet = ::recvfrom(_fd, buf, len, flag, NULL, NULL); //need check from_ip & port

    if (iRet < 0  && errno != EAGAIN)
    {
        TLOGERROR("[TARS][udp recv objname:" << _adapterProxy->getObjProxy()->name() 
            << ",fd:" << _fd << ",desc:" << _ep.desc() 
            << ", fail! errno:" << errno << "," << strerror(errno) << ",close]" << endl);

        close();

        return 0;
    }

    return iRet;
}

/////////////////////////////////////////////////////////////////
}
