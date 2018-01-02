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

#include "servant/CommunicatorEpoll.h"
#include "servant/Communicator.h"
#include "servant/TarsLogger.h"
#include "servant/StatReport.h"

using namespace std;

namespace tars
{

CommunicatorEpoll::CommunicatorEpoll(Communicator * pCommunicator,size_t netThreadSeq)
: _communicator(pCommunicator)
, _terminate(false)
, _nextTime(0)
, _nextStatTime(0)
, _objectProxyFactory(NULL)
, _asyncThreadNum(3)
, _asyncSeq(0)
, _netThreadSeq(netThreadSeq)
, _reportAsyncQueue(NULL)
, _noSendQueueLimit(1000)
, _waitTimeout(100)
, _timeoutCheckInterval(100)
{
    _ep.create(1024);

    _shutdown.createSocket();
    _ep.add(_shutdown.getfd(), 0, EPOLLIN);

    //ObjectProxyFactory 对象
    _objectProxyFactory = new ObjectProxyFactory(this);

    //异步线程数
    _asyncThreadNum = TC_Common::strto<size_t>(pCommunicator->getProperty("asyncthread", "3"));

    if(_asyncThreadNum == 0)
    {
        _asyncThreadNum = 3;
    }

    if(_asyncThreadNum > MAX_CLIENT_ASYNCTHREAD_NUM)
    {
        _asyncThreadNum = MAX_CLIENT_ASYNCTHREAD_NUM;
    }

    //节点队列未发送请求的大小限制
    _noSendQueueLimit = TC_Common::strto<size_t>(pCommunicator->getProperty("nosendqueuelimit", "1000"));
    if(_noSendQueueLimit < 1000)
    {
        _noSendQueueLimit = 1000;
    }

    //异步队列的大小
    size_t iAsyncQueueCap = TC_Common::strto<size_t>(pCommunicator->getProperty("asyncqueuecap", "10000"));
    if(iAsyncQueueCap < 10000)
    {
        iAsyncQueueCap = 10000;
    }

    //epollwait的超时时间
    _waitTimeout = TC_Common::strto<int64_t>(pCommunicator->getProperty("epollwaittimeout", "100"));
    if(_waitTimeout < 1)
    {
        _waitTimeout = 1;
    }

    //检查超时请求的时间间隔，单位:ms
    _timeoutCheckInterval = TC_Common::strto<int64_t>(pCommunicator->getProperty("timeoutcheckinterval", "100"));
    if(_timeoutCheckInterval < 1)
    {
        _timeoutCheckInterval = 1;
    }

    //创建异步线程
    for(size_t i = 0; i < _asyncThreadNum; ++i)
    {
        _asyncThread[i] = new AsyncProcThread(iAsyncQueueCap);
        _asyncThread[i]->start();
    }

    //初始化请求的事件通知
    for(size_t i = 0; i < MAX_CLIENT_NOTIFYEVENT_NUM; ++i)
    {
        _notify[i].bValid = false;
    }

    //异步队列数目上报
     string moduleName = pCommunicator->getProperty("modulename", "");
     if(!moduleName.empty())
     {
         PropertyReportPtr asyncQueuePtr = pCommunicator->getStatReport()->createPropertyReport(moduleName + ".asyncqueue"+TC_Common::tostr(netThreadSeq), PropertyReport::avg());
         _reportAsyncQueue = asyncQueuePtr.get();
     }
}

CommunicatorEpoll::~CommunicatorEpoll()
{
    for(size_t i = 0;i < _asyncThreadNum; ++i)
    {
        if(_asyncThread[i])
        {
            if (_asyncThread[i]->isAlive())
            {
                _asyncThread[i]->terminate();
                _asyncThread[i]->getThreadControl().join();
            }

            delete _asyncThread[i];
            _asyncThread[i] = NULL;
        }
    }

    if(_objectProxyFactory)
    {
        delete _objectProxyFactory;
        _objectProxyFactory = NULL;
    }
}

void CommunicatorEpoll::terminate()
{
    _terminate = true;
    //通知epoll响应
    _ep.mod(_shutdown.getfd(), 0, EPOLLOUT);
}

ObjectProxy * CommunicatorEpoll::getObjectProxy(const string & sObjectProxyName,const string& setName)
{
    return _objectProxyFactory->getObjectProxy(sObjectProxyName,setName);
}

void CommunicatorEpoll::addFd(int fd, FDInfo * info, uint32_t events)
{
    _ep.add(fd,(uint64_t)info,events);
}

void CommunicatorEpoll::delFd(int fd, FDInfo * info, uint32_t events)
{
    _ep.del(fd,(uint64_t)info,events);
}

void CommunicatorEpoll::notify(size_t iSeq,ReqInfoQueue * msgQueue)
{
    assert(iSeq < MAX_CLIENT_NOTIFYEVENT_NUM);

    if(_notify[iSeq].bValid)
    {
        _ep.mod(_notify[iSeq].notify.getfd(),(long long)&_notify[iSeq].stFDInfo, EPOLLIN);
        assert(_notify[iSeq].stFDInfo.p == (void*)msgQueue);
    }
    else
    {
        _notify[iSeq].stFDInfo.iType   = FDInfo::ET_C_NOTIFY;
        _notify[iSeq].stFDInfo.p       = (void*)msgQueue;
        _notify[iSeq].stFDInfo.fd      = _notify[iSeq].eventFd;
        _notify[iSeq].stFDInfo.iSeq    = iSeq;
        _notify[iSeq].notify.createSocket();
        _notify[iSeq].bValid           = true;

        _ep.add(_notify[iSeq].notify.getfd(),(long long)&_notify[iSeq].stFDInfo, EPOLLIN);
    }
}

void CommunicatorEpoll::notifyDel(size_t iSeq)
{
    assert(iSeq < MAX_CLIENT_NOTIFYEVENT_NUM);
    if(_notify[iSeq].bValid && NULL != _notify[iSeq].stFDInfo.p)
    {
        _ep.mod(_notify[iSeq].notify.getfd(),(long long)&_notify[iSeq].stFDInfo, EPOLLIN);
    }
}


void CommunicatorEpoll::handleInputImp(Transceiver * pTransceiver)
{
    //检查连接是否有错误
    if(pTransceiver->isConnecting())
    {
        int iVal = 0;
        socklen_t iLen = static_cast<socklen_t>(sizeof(int));
        if (::getsockopt(pTransceiver->fd(), SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&iVal), &iLen) == -1 || iVal)
        {
            pTransceiver->close();
            pTransceiver->getAdapterProxy()->addConnExc(true);
            TLOGERROR("[TARS][CommunicatorEpoll::handleInputImp] connect error "
                    << pTransceiver->getAdapterProxy()->endpoint().desc()
                    << "," << pTransceiver->getAdapterProxy()->getObjProxy()->name()
                    << ",_connExcCnt=" << pTransceiver->getAdapterProxy()->ConnExcCnt()
                    << "," << strerror(iVal) << endl);
            return;
        }

        pTransceiver->setConnected();
    }

    list<ResponsePacket> done;
    if(pTransceiver->doResponse(done) > 0)
    {
        list<ResponsePacket>::iterator it = done.begin();
        for (; it != done.end(); ++it)
        {
            pTransceiver->getAdapterProxy()->finishInvoke(*it);
        }
    }
}

void CommunicatorEpoll::handleOutputImp(Transceiver * pTransceiver)
{
    //检查连接是否有错误
    if(pTransceiver->isConnecting())
    {
        int iVal = 0;
        socklen_t iLen = static_cast<socklen_t>(sizeof(int));
        if (::getsockopt(pTransceiver->fd(), SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&iVal), &iLen) == -1 || iVal)
        {
            pTransceiver->close();
            pTransceiver->getAdapterProxy()->addConnExc(true);
            TLOGERROR("[TARS][CommunicatorEpoll::handleOutputImp] connect error "
                    << pTransceiver->getAdapterProxy()->endpoint().desc()
                    << "," << pTransceiver->getAdapterProxy()->getObjProxy()->name()
                    << ",_connExcCnt=" << pTransceiver->getAdapterProxy()->ConnExcCnt()
                    << "," << strerror(iVal) << endl);
            return;
        }

        pTransceiver->setConnected();
    }

    pTransceiver->doRequest();
}

void CommunicatorEpoll::handle(FDInfo * pFDInfo, uint32_t events)
{
    try
    {
        assert(pFDInfo != NULL);

        //队列有消息通知过来
        if(FDInfo::ET_C_NOTIFY == pFDInfo->iType)
        {
            ReqInfoQueue * pInfoQueue=(ReqInfoQueue*)pFDInfo->p;
            ReqMessage * msg = NULL;

            try
            {
                while(pInfoQueue->pop_front(msg))
                {
                    //线程退出了
                    if(ReqMessage::THREAD_EXIT == msg->eType)
                    {
                        assert(pInfoQueue->empty());

                        delete msg;
                        msg = NULL;

                        _ep.del(_notify[pFDInfo->iSeq].notify.getfd(),(long long)&_notify[pFDInfo->iSeq].stFDInfo, EPOLLIN);

                        delete pInfoQueue;
                        pInfoQueue = NULL;

                        _notify[pFDInfo->iSeq].stFDInfo.p = NULL;
                        _notify[pFDInfo->iSeq].notify.close();
                        _notify[pFDInfo->iSeq].bValid = false;

                        return;
                    }

                    try
                    {
                        msg->pObjectProxy->invoke(msg);
                    }
                    catch(exception & e)
                    {
                        TLOGERROR("[TARS]CommunicatorEpoll::handle exp:"<<e.what()<<" ,line:"<<__LINE__<<endl);
                    }
                    catch(...)
                    {
                        TLOGERROR("[TARS]CommunicatorEpoll::handle|"<<__LINE__<<endl);
                    }
                }
            }
            catch(exception & e)
            {
                TLOGERROR("[TARS]CommunicatorEpoll::handle exp:"<<e.what()<<" ,line:"<<__LINE__<<endl);
            }
            catch(...)
            {
                TLOGERROR("[TARS]CommunicatorEpoll::handle|"<<__LINE__<<endl);
            }
        }
        else
        {

            Transceiver *pTransceiver = (Transceiver*)pFDInfo->p;

            //先收包
            if (events & EPOLLIN)
            {
                try
                {
                    handleInputImp(pTransceiver);
                }
                catch(exception & e)
                {
                    TLOGERROR("[TARS]CommunicatorEpoll::handle exp:"<<e.what()<<" ,line:"<<__LINE__<<endl);
                }
                catch(...)
                {
                    TLOGERROR("[TARS]CommunicatorEpoll::handle|"<<__LINE__<<endl);
                }
            }

            //发包
            if (events & EPOLLOUT)
            {
                try
                {
                    handleOutputImp(pTransceiver);
                }
                catch(exception & e)
                {
                    TLOGERROR("[TARS]CommunicatorEpoll::handle exp:"<<e.what()<<" ,line:"<<__LINE__<<endl);
                }
                catch(...)
                {
                    TLOGERROR("[TARS]CommunicatorEpoll::handle|"<<__LINE__<<endl);
                }
            }

            //连接出错 直接关闭连接
            if(events & EPOLLERR)
            {
                try
                {
                    pTransceiver->close();
                }
                catch(exception & e)
                {
                    TLOGERROR("[TARS]CommunicatorEpoll::handle exp:"<<e.what()<<" ,line:"<<__LINE__<<endl);
                }
                catch(...)
                {
                    TLOGERROR("[TARS]CommunicatorEpoll::handle|"<<__LINE__<<endl);
                }
            }
        }
    }
    catch(exception & e)
    {
        TLOGERROR("[TARS]CommunicatorEpoll::handle exp:"<<e.what()<<" ,line:"<<__LINE__<<endl);
    }
    catch(...)
    {
        TLOGERROR("[TARS]CommunicatorEpoll::handle|"<<__LINE__<<endl);
    }
}

void CommunicatorEpoll::doTimeout()
{
    int64_t iNow = TNOWMS;
    if(_nextTime > iNow)
    {
        return;
    }

    //每_timeoutCheckInterval检查一次
    _nextTime = iNow + _timeoutCheckInterval;

    for(size_t i = 0; i < _objectProxyFactory->getObjNum(); ++i)
    {
        const vector<AdapterProxy*> & vAdapterProxy=_objectProxyFactory->getObjectProxy(i)->getAdapters();
        for(size_t iAdapter=0;iAdapter<vAdapterProxy.size();++iAdapter)
        {
            vAdapterProxy[iAdapter]->doTimeout();
        }
        _objectProxyFactory->getObjectProxy(i)->doTimeout();
    }
}

void CommunicatorEpoll::doStat()
{
    int64_t iNow = TNOW;
    if(_nextStatTime > iNow)
        return;

    //10s上报一次
    _nextStatTime = iNow + 10;

    //异步队列长度上报
    if(_reportAsyncQueue)
    {
        size_t n = 0;
        for(size_t i = 0;i < _asyncThreadNum; ++i)
        {
            n = n + _asyncThread[i]->getSize();
        }
        _reportAsyncQueue->report(n);
    }

    StatReport::MapStatMicMsg mStatMicMsg;

    for(size_t i = 0;i < _objectProxyFactory->getObjNum(); ++i)
    {
        const vector<AdapterProxy*> & vAdapterProxy = _objectProxyFactory->getObjectProxy(i)->getAdapters();
        for(size_t iAdapter = 0;iAdapter < vAdapterProxy.size(); ++iAdapter)
        {
            vAdapterProxy[iAdapter]->doStat(mStatMicMsg);
        }
    }

    //有数据才上报
    if(mStatMicMsg.size() > 0)
    {
        StatReport::MapStatMicMsg* pmStatMicMsg = new StatReport::MapStatMicMsg(mStatMicMsg);
        _communicator->getStatReport()->report(_netThreadSeq,pmStatMicMsg);
    }
}

void CommunicatorEpoll::pushAsyncThreadQueue(ReqMessage * msg)
{
    //先不考虑每个线程队列数目不一致的情况
    _asyncThread[_asyncSeq]->push_back(msg);
    _asyncSeq ++;

    if(_asyncSeq == _asyncThreadNum)
    {
        _asyncSeq = 0;
    }
}

void CommunicatorEpoll::run()
{
    TLOGDEBUG("CommunicatorEpoll::run id:"<<syscall(SYS_gettid)<<endl);

    ServantProxyThreadData * pSptd = ServantProxyThreadData::getData();
    assert(pSptd != NULL);
    pSptd->_netThreadSeq = (int)_netThreadSeq;

    while (!_terminate)
    {
        try
        {
            int iTimeout = ((_waitTimeout < _timeoutCheckInterval) ? _waitTimeout : _timeoutCheckInterval);

            int num = _ep.wait(iTimeout);

            if(_terminate)
            {
                break;
            }

            //先处理epoll的网络事件
            for (int i = 0; i < num; ++i)
            {
                const epoll_event& ev = _ep.get(i);
                uint64_t data = ev.data.u64;

                if(data == 0)
                {
                    continue; //data非指针, 退出循环
                }

                handle((FDInfo*)data, ev.events);
            }

            //处理超时请求
            doTimeout();

            //数据上报
            doStat();
        }
        catch (exception& e)
        {
            TLOGERROR("[TARS][CommunicatorEpoll:run exception:" << e.what() << "]" << endl);
        }
        catch (...)
        {
            TLOGERROR("[TARS][CommunicatorEpoll:run exception.]" << endl);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////
}
