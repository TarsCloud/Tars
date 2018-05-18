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

#include "servant/ObjectProxy.h"
#include "servant/Communicator.h"
#include "servant/Global.h"
#include "servant/EndpointManager.h"
#include "servant/AppCache.h"
#include "util/tc_common.h"
#include "util/tc_clientsocket.h"
#include "servant/TarsLogger.h"

namespace tars
{
////////////////////////////////////////////////////////////////////////////////////////////
ObjectProxy::ObjectProxy(CommunicatorEpoll * pCommunicatorEpoll, const string & sObjectProxyName,const string& setName)
: _communicatorEpoll(pCommunicatorEpoll)
, _invokeSetId(setName)
, _isInvokeBySet(false)
, _id(0)
, _hasSetProtocol(false)
, _conTimeout(1000)
, _servantProxy(NULL)
{
    string::size_type pos = sObjectProxyName.find_first_of('@');

    if(pos != string::npos)
    {
        _name = sObjectProxyName.substr(0,pos);
    }
    else
    {
        _name = sObjectProxyName;
        //启用set或者指定set调用
        if(ClientConfig::SetOpen || !_invokeSetId.empty())
        {
            //指定set调用时，指定set的优先级最高
            _invokeSetId  = _invokeSetId.empty()?ClientConfig::SetDivision:_invokeSetId;
            _isInvokeBySet = true;
        }
    }

    _proxyProtocol.requestFunc  = ProxyProtocol::tarsRequest;
    _proxyProtocol.responseFunc = ProxyProtocol::tarsResponse;

    _endpointManger.reset(new EndpointManager(this, _communicatorEpoll->getCommunicator(), sObjectProxyName, pCommunicatorEpoll->isFirstNetThread(), setName));

}

ObjectProxy::~ObjectProxy()
{
}

void ObjectProxy::initialize()
{
}

int ObjectProxy::loadLocator()
{
    if(_endpointManger->getDirectProxy())
    {
        //直接连接
        return 0;
    }

    string locator = _communicatorEpoll->getCommunicator()->getProperty("locator");

    if (locator.find_first_not_of('@') == string::npos)
    {
        TLOGERROR("[Locator is not valid:" << locator << "]" << endl);

        return -1;
    }

    QueryFPrx prx = _communicatorEpoll->getCommunicator()->stringToProxy<QueryFPrx>(locator);

    _endpointManger->setLocatorPrx(prx);

    return 0;
}

void ObjectProxy::setProxyProtocol(const ProxyProtocol& protocol)
{
    if(!_hasSetProtocol)
    {
        _hasSetProtocol = true;
        _proxyProtocol  = protocol;
    }
}

ProxyProtocol& ObjectProxy::getProxyProtocol()
{
    return _proxyProtocol;
}

void ObjectProxy::setSocketOpt(int level, int optname, const void *optval, socklen_t optlen)
{
    SocketOpt socketOpt;

    socketOpt.level      = level;
    socketOpt.optname    = optname;
    socketOpt.optval     = optval;
    socketOpt.optlen     = optlen;

    _socketOpts.push_back(socketOpt);
}

vector<SocketOpt>& ObjectProxy::getSocketOpt()
{
    return _socketOpts;
}

void ObjectProxy::setPushCallbacks(const ServantProxyCallbackPtr& cb)
{
    _pushCallback = cb;
}

ServantProxyCallbackPtr ObjectProxy::getPushCallback()
{
    return _pushCallback;
}

void ObjectProxy::invoke(ReqMessage * msg)
{
    TLOGINFO("[TARS][ObjectProxy::invoke, objname:" << _name << ", begin...]" << endl);

    //选择一个远程服务的Adapter来调用
    AdapterProxy * pAdapterProxy = NULL;
    bool bFirst = _endpointManger->selectAdapterProxy(msg, pAdapterProxy);

    if(bFirst)
    {
        //判断是否请求过主控
        bool bRet = _reqTimeoutQueue.push(msg,msg->request.iTimeout+msg->iBeginTime);

        assert(bRet);

        //把数据缓存在obj里面
        TLOGINFO("[TARS][ObjectProxy::invoke, objname:" << _name << ", select adapter proxy not valid (have not inovoke reg)]" << endl);

        return;
    }

    if(!pAdapterProxy)
    {
        TLOGERROR("[TARS][ObjectProxy::invoke, objname:"<< _name << ", selectAdapterProxy is null]"<<endl);

        msg->response.iRet = TARSADAPTERNULL;

        doInvokeException(msg);

        return ;
    }

    msg->adapter = pAdapterProxy;
    pAdapterProxy->invoke(msg);
}

void ObjectProxy::doInvoke()
{
    TLOGINFO("[TARS][ObjectProxy::doInvoke, objname:" << _name << ", begin...]" << endl);

    while(!_reqTimeoutQueue.empty())
    {
        TLOGINFO("[TARS][ObjectProxy::doInvoke, objname:" << _name << ", pop...]" << endl);

        ReqMessage * msg = NULL;
        _reqTimeoutQueue.pop(msg);

        assert(msg != NULL);

        //选择一个远程服务的Adapter来调用
        AdapterProxy * pAdapterProxy = NULL;
        _endpointManger->selectAdapterProxy(msg,pAdapterProxy);

        if(!pAdapterProxy)
        {
            //这里肯定是请求过主控
            TLOGERROR("[TARS][ObjectProxy::doInvoke, objname:" << _name << ", selectAdapterProxy is null]" << endl);

            msg->response.iRet = TARSADAPTERNULL;

            doInvokeException(msg);

            return;
        }

        msg->adapter = pAdapterProxy;
        pAdapterProxy->invoke(msg);
    }
}

const vector<AdapterProxy*> & ObjectProxy::getAdapters() const
{
    return _endpointManger->getAdapters();
}

void ObjectProxy::doInvokeException(ReqMessage * msg)
{
    TLOGINFO("[TARS][ObjectProxy::doInvokeException, objname:" << _name << "]" << endl);

    //单向调用出现异常直接删除请求
    if(msg->eType == ReqMessage::ONE_WAY)
    {
        delete msg;
        return;
    }

    //标识请求异常
    msg->eStatus = ReqMessage::REQ_EXC;

    if(msg->eType == ReqMessage::SYNC_CALL)
    {
        if(!msg->bCoroFlag)
        {
            assert(msg->pMonitor);

            TC_ThreadLock::Lock sync(*(msg->pMonitor));

            msg->pMonitor->notify();
            msg->bMonitorFin = true;
        }
        else
        {
            msg->sched->put(msg->iCoroId);
        }

        return;
    }

    if(msg->callback)
    {
        if(!msg->bCoroFlag)
        {
            if(msg->callback->getNetThreadProcess())
            {
                ReqMessagePtr msgPtr = msg;
                //如果是本线程的回调，直接本线程处理
                //比如获取endpoint
                try
                {
                    msg->callback->onDispatch(msgPtr);
                }
                catch(exception & e)
                {
                    TLOGERROR("[TARS]ObjectProxy::doInvokeException exp:" << e.what() << " ,line:" << __LINE__ << endl);
                }
                catch(...)
                {
                    TLOGERROR("[TARS]ObjectProxy::doInvokeException exp:unknown line:|" << __LINE__ << endl);
                }
            }
            else
            {
                //异步回调，放入回调处理线程中
                _communicatorEpoll->pushAsyncThreadQueue(msg);
            }
        }
        else
        {
            CoroParallelBasePtr ptr = msg->callback->getCoroParallelBasePtr();
            if(ptr)
            {
                ptr->insert(msg);
                if(ptr->checkAllReqReturn())
                {
                    msg->sched->put(msg->iCoroId);
                }
            }
            else
            {
                TLOGERROR("[TARS]ObjectProxy::doInvokeException coro parallel callback error, objname:" << _name << endl);
                delete msg;
            }
        }
    }
}

void ObjectProxy::doTimeout()
{
    TLOGINFO("[TARS][ObjectProxy::doInvokeException, objname:" << _name << "]" << endl);

    ReqMessage * reqInfo = NULL;
    while(_reqTimeoutQueue.timeout(reqInfo))
    {
        TLOGERROR("[TARS][ObjectProxy::doTimeout, objname:" << _name << ", queue timeout error]" << endl);

        reqInfo->response.iRet = TARSINVOKETIMEOUT;

        doInvokeException(reqInfo);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
}
