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

#include "servant/AdapterProxy.h"
#include "servant/Communicator.h"
#include "servant/StatReport.h"
#include "servant/Application.h"
#include "servant/AdminF.h"
#include "servant/AppCache.h"
#include "servant/TarsLogger.h"
#include "tup/tup.h"
#include "servant/StatF.h"
#include "servant/StatReport.h"


namespace tars
{

AdapterProxy::AdapterProxy(ObjectProxy * pObjectProxy,const EndpointInfo &ep,Communicator* pCom)
: _communicator(pCom)
, _objectProxy(pObjectProxy)
, _endpoint(ep)
, _activeStateInReg(true)
, _activeStatus(true)
, _totalInvoke(0)
, _timeoutInvoke(0)
, _nextFinishInvokeTime(0)
, _frequenceFailInvoke(0)
, _frequenceFailTime(0)
, _nextRetryTime(0)
, _connTimeout(false)
, _connExc(false)
, _connExcCnt(0)
, _staticWeight(0)
, _timeoutLogFlag(false)
, _noSendQueueLimit(1000)
, _maxSampleCount(1000)
, _sampleRate(0)
{
    _timeoutQueue.reset(new TC_TimeoutQueueNew<ReqMessage*>());

    if(pObjectProxy->getCommunicatorEpoll())
    {
        _noSendQueueLimit = pObjectProxy->getCommunicatorEpoll()->getNoSendQueueLimit();
    }

    if(_communicator)
    {
        _timeoutLogFlag = _communicator->getTimeoutLogFlag();
    }

    if (ep.type() == EndpointInfo::UDP)
    {
        _trans.reset(new UdpTransceiver(this, ep));
    }
    else
    {
        _trans.reset(new TcpTransceiver(this, ep));
    }

    //初始化stat的head信息
    initStatHead();
}

AdapterProxy::~AdapterProxy()
{
}

string AdapterProxy::getSlaveName(const string& sSlaveName)
{
    string::size_type pos = sSlaveName.find(".");
    if (pos != string::npos)
    {
        pos = sSlaveName.find(".", pos + 1);
        if (pos != string::npos)
        {
            return sSlaveName.substr(0, pos);
        }
    }

    return  sSlaveName;
}

void AdapterProxy::initStatHead()
{
    vector <string> v;
    if(!ClientConfig::SetDivision.empty() &&
        StatReport::divison2SetInfo(ClientConfig::SetDivision, v)) //主调(client)启用set
    {
        _statHead.masterName = StatReport::trimAndLimitStr(ClientConfig::ModuleName + "." + v[0] + v[1] + v[2] + "@" + ClientConfig::TarsVersion, StatReport::MAX_MASTER_NAME_LEN);
    }
    else
    {
        _statHead.masterName = StatReport::trimAndLimitStr(ClientConfig::ModuleName + "@" + ClientConfig::TarsVersion, StatReport::MAX_MASTER_NAME_LEN);
    }
    
    const string sSlaveName = getSlaveName(_objectProxy->name());
    string sSlaveSet = _endpoint.setDivision();
    if(!sSlaveSet.empty() &&
        StatReport::divison2SetInfo(sSlaveSet, v)) //被调启用set
    {
        _statHead.slaveSetName  = v[0];
        _statHead.slaveSetArea  = v[1];
        _statHead.slaveSetID    = v[2];
        _statHead.slaveName     = StatReport::trimAndLimitStr(sSlaveName + "." + v[0] + v[1] + v[2], StatReport::MAX_MASTER_NAME_LEN);
    }
    else
    {
        _statHead.slaveName = StatReport::trimAndLimitStr(sSlaveName, StatReport::MAX_MASTER_NAME_LEN);
    }

    _statHead.slaveIp      = StatReport::trimAndLimitStr(_endpoint.host(), StatReport::MAX_MASTER_IP_LEN);
    _statHead.slavePort    = _endpoint.port();
    _statHead.returnValue  = 0;
}

int AdapterProxy::invoke(ReqMessage * msg)
{
    assert(_trans != NULL);

    TLOGINFO("[TARS][AdapterProxy::invoke objname:" << _objectProxy->name() << ",desc:" << _endpoint.desc() << endl);

    //未发链表有长度限制
    if(_timeoutQueue->getSendListSize() >= _noSendQueueLimit)
    {
        TLOGERROR("[TARS][AdapterProxy::invoke fail,ReqInfoQueue.size > " << _noSendQueueLimit << ",objname:" << _objectProxy->name() <<",desc:"<< _endpoint.desc() << endl);
        msg->eStatus = ReqMessage::REQ_EXC;

        finishInvoke(msg);

        return 0;
    }

    //生成requestid
    //tars调用 而且 不是单向调用
    if(!msg->bFromRpc)
    {
        msg->request.iRequestId = _objectProxy->generateId();
    }

    _objectProxy->getProxyProtocol().requestFunc(msg->request,msg->sReqData);

    //交给连接发送数据,连接连上,buffer不为空,直接发送数据成功
    if(_timeoutQueue->sendListEmpty() && _trans->sendRequest(msg->sReqData.c_str(),msg->sReqData.size()) != Transceiver::eRetError)
    {
        TLOGINFO("[TARS][AdapterProxy::invoke push (send) objname:" << _objectProxy->name() << ",desc:" << _endpoint.desc() << ",id:" << msg->request.iRequestId << endl);

        //请求发送成功了，单向调用直接返回
        if(msg->eType == ReqMessage::ONE_WAY)
        {
            delete msg;
            msg = NULL;

            return 0;
        }

        bool bFlag = _timeoutQueue->push(msg, msg->request.iRequestId, msg->request.iTimeout + msg->iBeginTime);
        if(!bFlag)
        {
            TLOGERROR("[TARS][AdapterProxy::invoke fail1 : insert timeout queue fail,queue size:" << _timeoutQueue->size() << ",objname" <<_objectProxy->name() << ",desc" << _endpoint.desc() <<endl);
            msg->eStatus = ReqMessage::REQ_EXC;

            finishInvoke(msg);
        }
    }
    else
    {
        TLOGINFO("[TARS][AdapterProxy::invoke push (no send) " << _objectProxy->name() << ", " << _endpoint.desc() << ",id " << msg->request.iRequestId <<endl);

        //请求发送失败了
        bool bFlag = _timeoutQueue->push(msg,msg->request.iRequestId, msg->request.iTimeout+msg->iBeginTime, false);
        if(!bFlag)
        {
            TLOGERROR("[TARS][AdapterProxy::invoke fail2 : insert timeout queue fail,queue size:" << _timeoutQueue->size() << "," <<_objectProxy->name() << ", " << _endpoint.desc() <<endl);
            
            msg->eStatus = ReqMessage::REQ_EXC;

            finishInvoke(msg);
        }
    }

    return 0;
}

void AdapterProxy::doInvoke()
{
    if(_timeoutQueue->sendListEmpty())
    {
        return ;
    }

    while(!_timeoutQueue->sendListEmpty())
    {
        ReqMessage * msg = NULL;

        _timeoutQueue->getSend(msg);

        int iRet = _trans->sendRequest(msg->sReqData.c_str(), msg->sReqData.size());

        TLOGINFO("[TARS][AdapterProxy::doInvoke sendRequest objname:" << _objectProxy->name() << ",desc" << _endpoint.desc() << ",id " << msg->request.iRequestId << ",ret" << iRet << endl);

        //发送失败 返回
        if(iRet == Transceiver::eRetError)
        {
            TLOGINFO("[TARS][AdapterProxy::doInvoke fail,errono:" << iRet << endl);
            return;
        }

        //请求发送成功了 处理采样
        //...

        //发送完成，要从队列里面清掉
        _timeoutQueue->popSend(msg->eType == ReqMessage::ONE_WAY);
        if(msg->eType == ReqMessage::ONE_WAY)
        {
            delete msg;
            msg = NULL;
        }

        //发送buffer已经满了 要返回
        if(iRet == Transceiver::eRetFull)
        {
            return;
        }
    }
}


void AdapterProxy::finishInvoke(bool bFail)
{
    TLOGINFO("[TARS][AdapterProxy::finishInvoke(bool) objname:" << _objectProxy->name() << ",desc:" << _endpoint.desc() << ",bFail:" << bFail << endl);

    time_t now = TNOW;

    CheckTimeoutInfo& info = _objectProxy->checkTimeoutInfo();

    //处于异常状态 已经屏蔽
    if(!_activeStatus)
    {
        if(!bFail)
        {
            //重试成功,恢复正常状态
            _activeStatus         = true;

            //连续失败次数清零
            _frequenceFailInvoke  = 0;

            _nextFinishInvokeTime = now + info.checkTimeoutInterval;

            _frequenceFailInvoke  = 0;

            _totalInvoke          = 1;

            _timeoutInvoke        = 0;

            _connTimeout          = false;

            _connExc              = false;

            TLOGINFO("[TARS][AdapterProxy::finishInvoke(bool), objname:" << _objectProxy->name() << ",desc:" << _endpoint.desc() << ",retry ok" << endl);
        }
        else
        {
            //结点已经屏蔽 过来失败的包不用处理
            TLOGINFO("[TARS][AdapterProxy::finishInvoke(bool), objname:" << _objectProxy->name() << ",desc:" << _endpoint.desc() << ",retry fail" << endl);
        }
        return;
    }

    ++_totalInvoke;

    if(bFail)
    {
        //调用失败

        //失败次数+1
        ++_timeoutInvoke;

        //连续失败时间间隔重新计算
        if(0 == _frequenceFailInvoke)
        {
            _frequenceFailTime = now + info.minFrequenceFailTime;
        }
         //连续失败次数加1
        _frequenceFailInvoke++;


        //检查是否到了连续失败次数,且至少在5s以上
        if(_frequenceFailInvoke >= info.frequenceFailInvoke && now >= _frequenceFailTime)
        {
            //setInactive();
             _activeStatus = false;
            _nextRetryTime = TNOW + _objectProxy->checkTimeoutInfo().tryTimeInterval;

            TLOGERROR("[TARS][AdapterProxy::finishInvoke(bool) objname:"<< _objectProxy->name() 
                    << ",desc:" << _endpoint.desc()
                    << ",disable frequenceFail,freqtimeout:" << _frequenceFailInvoke
                    << ",timeout:"<< _timeoutInvoke
                    << ",total:" << _totalInvoke << endl);
            return ;
        }
    }
    else
    {
        _frequenceFailInvoke = 0;
    }

    //判断一段时间内的超时比例
    if(now > _nextFinishInvokeTime)
    {
        _nextFinishInvokeTime = now + info.checkTimeoutInterval;

        if(bFail && _timeoutInvoke >= info.minTimeoutInvoke && _timeoutInvoke >= info.radio * _totalInvoke)
        {
            setInactive();
            TLOGERROR("[TARS][AdapterProxy::finishInvoke(bool) objname" << _objectProxy->name() 
                    << ",desc:" << _endpoint.desc()
                    << ",disable radioFail,freqtimeout:" << _frequenceFailInvoke
                    << ",timeout:"<< _timeoutInvoke
                    << ",total:" << _totalInvoke << endl);
        }
        else
        {
            //每一分钟清空一次
            _totalInvoke   = 0;

            _timeoutInvoke = 0;
        }
    }
}


bool AdapterProxy::checkActive(bool bForceConnect)
{
    time_t now = TNOW;

    TLOGINFO("[TARS][AdapterProxy::checkActive objname:" << _objectProxy->name() 
          << ",desc:" << _endpoint.desc() 
          << ",_activeStatus:" << (_activeStatus ? "enable" : "disable")
          << (bForceConnect? ",forceConnect" : "")
          << ",freqtimeout:" << _frequenceFailInvoke
          << ",timeout:" << _timeoutInvoke
          << ",_connExcCnt:"<<_connExcCnt
          << ",total:" << _totalInvoke << endl);

    _trans->checkTimeout();

    //强制重试
    if(bForceConnect)
    {
        //强制重试  肯定是无效结点
        assert(!_activeStatus);

        //有效的连接
        if(_trans->isConnecting() || _trans->hasConnected())
        {
            return true;
        }

        _nextRetryTime = now + _objectProxy->checkTimeoutInfo().tryTimeInterval;

        //连接没有建立或者连接无效, 重新建立连接
        if(!_trans->isValid())
        {

            try
            {
                _trans->reconnect();
            }
            catch(exception &ex)
            {
                _activeStatus = false;

                _trans->close();

                TLOGERROR("[TARS][AdapterProxy::checkActive connect ex:" << ex.what() << endl);
            }
        }

        return (_trans->hasConnected() || _trans->isConnecting());
    }

    //失效且没有到下次重试时间, 直接返回不可用
    if((!_activeStatus) && (now < _nextRetryTime) )
    {
        TLOGINFO("[TARS][AdapterProxy::checkActive,not reach retry time ,objname:" << _objectProxy->name() << ",desc:" << _endpoint.desc()  <<endl);
        return false;
    }

    if(!_activeStatus)
    {
        _nextRetryTime = now + _objectProxy->checkTimeoutInfo().tryTimeInterval;
    }

    //连接没有建立或者连接无效, 重新建立连接
    if(!_trans->isValid())
    {
        try
        {
            _trans->reconnect();
        }
        catch(exception &ex)
        {
            _activeStatus = false;

            _trans->close();

            TLOGERROR("[TARS][AdapterProxy::checkActive connect ex:" << ex.what() << endl);
        }
    }

    return (_trans->hasConnected() || _trans->isConnecting());
}


void AdapterProxy::setConTimeout(bool bConTimeout)
{
    if(bConTimeout != _connTimeout)
    {
        TLOGERROR("[TARS][AdapterProxy::setConTimeout desc:"<< _endpoint.desc() << " connect timeout status is:" << bConTimeout << endl);
        _connTimeout = bConTimeout;
        if(_connTimeout)
        {
            setInactive();
        }
    }
}

int AdapterProxy::getConTimeout() 
{ 
    return _objectProxy->getConTimeout(); 
}

//屏蔽结点
void AdapterProxy::setInactive()
{
    _activeStatus  = false;

    _nextRetryTime = TNOW + _objectProxy->checkTimeoutInfo().tryTimeInterval;

    _trans->close();

    TLOGINFO("[TARS][AdapterProxy::setInactive objname:" << _objectProxy->name() << ",desc:" << _endpoint.desc() << ",inactive" << endl);
}

void AdapterProxy::finishInvoke(ResponsePacket & rsp)
{
    TLOGINFO("[TARS][AdapterProxy::finishInvoke(ResponsePacket) objname:" << _objectProxy->name() << ",desc:" << _endpoint.desc() 
        << ",id:" << rsp.iRequestId << endl);

    if (_trans->getAuthState() != AUTH_SUCC)
    {
        std::string ret(rsp.sBuffer.begin(), rsp.sBuffer.end());
        tars::AUTH_STATE tmp = AUTH_SUCC;
        tars::stoe(ret, tmp);
        int newstate = tmp;

        TLOGINFO("[TARS]AdapterProxy::finishInvoke from state " << _trans->getAuthState() << " to " << newstate << endl);
        _trans->setAuthState(newstate);

        if (newstate == AUTH_SUCC)
        {
            // flush old buffered msg when auth is not complete
            doInvoke();
        }
        else
        {
            TLOGERROR("newstate is " << newstate << ", error close!\n");
            _trans->close();
        }

        return;
    }

    ReqMessage * msg = NULL;

    //requestid 为0 是push消息
    if(rsp.iRequestId == 0)
    {
        if(!_objectProxy->getPushCallback())
        {
            TLOGERROR("[TARS][AdapterProxy::finishInvoke(ResponsePacket)，request id is 0, pushcallback is null, objname:" << _objectProxy->name() 
                << ",desc:" << _endpoint.desc() << endl);
            return;
        }

        msg               = new ReqMessage();
        msg->eStatus      = ReqMessage::REQ_RSP;
        msg->eType        = ReqMessage::ASYNC_CALL;
        msg->bFromRpc     = true;
        msg->bPush        = true;
        msg->proxy        = _objectProxy->getServantProxy();
        msg->pObjectProxy = _objectProxy;
        msg->adapter      = this;
        msg->callback     = _objectProxy->getPushCallback();
    }
    else
    {
        //这里的队列中的发送链表中的数据可能已经在timeout的时候删除了
        bool retErase = _timeoutQueue->erase(rsp.iRequestId, msg);

        //找不到此请求id信息
        if (!retErase)
        {
            if(_timeoutLogFlag)
            {
                TLOGERROR("[TARS][AdapterProxy::finishInvoke(ResponsePacket) objname:"<< _objectProxy->name() << ",get req-ptr NULL,may be timeout,id:" << rsp.iRequestId 
                    << ",desc:" << _endpoint.desc() << endl);
            }
            return ;
        }

        assert(msg->eStatus == ReqMessage::REQ_REQ);

        msg->eStatus = ReqMessage::REQ_RSP;
    }

    msg->response = rsp;

    finishInvoke(msg);
}

void AdapterProxy::finishInvoke(ReqMessage * msg)
{
    assert(msg->eStatus != ReqMessage::REQ_REQ);

    TLOGINFO("[TARS][AdapterProxy::finishInvoke(ReqMessage) objname:" << _objectProxy->name() << ",desc:" << _endpoint.desc() << " ,id:" << msg->response.iRequestId << endl);

    //单向调用
    if(msg->eType == ReqMessage::ONE_WAY)
    {
        delete msg;
        msg = NULL;
        return ;
    }

    //stat 上报调用统计
    stat(msg);

    //超时屏蔽统计,异常不算超时统计
    if(msg->eStatus != ReqMessage::REQ_EXC && !msg->bPush)
    {
        finishInvoke(msg->response.iRet != TARSSERVERSUCCESS);
    }

    //同步调用，唤醒ServantProxy线程
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

        return ;
    }

    //异步调用
    if(msg->eType == ReqMessage::ASYNC_CALL)
    {
        if(!msg->bCoroFlag)
        {
            if(msg->callback->getNetThreadProcess())
            {
                //如果是本线程的回调，直接本线程处理
                //比如获取endpoint
                ReqMessagePtr msgPtr = msg;
                try
                {
                    msg->callback->onDispatch(msgPtr);
                }
                catch(exception & e)
                {
                    TLOGERROR("[TARS][AdapterProxy::finishInvoke(ReqMessage) ex:" << e.what() << ",line:" << __LINE__ << endl);
                }
                catch(...)
                {
                    TLOGERROR("[TARS]AdapterProxy::finishInvoke(ReqMessage) ex:unknown,line:" << __LINE__ << endl);
                }
            }
            else
            {
                //异步回调，放入回调处理线程中
                _objectProxy->getCommunicatorEpoll()->pushAsyncThreadQueue(msg);
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
                TLOGERROR("[TARS][AdapterProxy::finishInvoke(ReqMessage) coro parallel callback error,obj:" << _objectProxy->name() << ",desc:" << _endpoint.desc() 
                    << ",id:" << msg->response.iRequestId << endl);
                delete msg;
                msg = NULL;
            }
        }
        return;
    }

    assert(false);

    return;
}


void AdapterProxy::doTimeout()
{
    TLOGINFO("[TARS][AdapterProxy::doTimeout objname:" << _objectProxy->name() << ",desc:" << _endpoint.desc() << endl);

    ReqMessage * msg;
    while(_timeoutQueue->timeout(msg))
    {
        TLOGINFO("[TARS][AdapterProxy::doTimeout objname:" << _objectProxy->name() << ",desc:" << _endpoint.desc() << ",id " << msg->request.iRequestId << endl);

        assert(msg->eStatus == ReqMessage::REQ_REQ);

        msg->eStatus = ReqMessage::REQ_TIME;

        //有可能是单向调用超时了
        if(msg->eType == ReqMessage::ONE_WAY)
        {
            delete msg;
            msg = NULL;
            continue;
        }

        //如果是异步调用超时
        if(msg->eType == ReqMessage::ASYNC_CALL)
        {
            //_connExcCnt大于0说明是网络连接异常引起的超时
            msg->response.iRet = (_connExcCnt > 0 ? TARSPROXYCONNECTERR : TARSASYNCCALLTIMEOUT);
        }

        finishInvoke(msg);
    }
}

void AdapterProxy::sample(ReqMessage * msg)
{
    auto iter = _sample.find(msg->request.sFuncName);
    if(iter == _sample.end())
    {
        vector<StatSampleMsg> vBuf;
        auto result = _sample.insert(make_pair(msg->request.sFuncName,vBuf));
        assert(result.second);
        iter = result.first;
    }
    else
    {
        if(iter->second.size() > _maxSampleCount)
            return;
    }

    StatSampleMsg  sample;
    sample.unid             = msg->sampleKey._unid;
    sample.depth            = msg->sampleKey._depth;
    sample.width            = msg->sampleKey._width;
    sample.parentWidth      = msg->sampleKey._parentWidth;
    sample.masterName       = ClientConfig::ModuleName;
    sample.slaveName        = StatReport::trimAndLimitStr(_objectProxy->name(), StatReport::MAX_MASTER_NAME_LEN);
    sample.interfaceName    = msg->request.sFuncName;
    sample.masterIp         = "";
    sample.slaveIp          = _endpoint.host();

    iter->second.push_back(sample);
}

void AdapterProxy::stat(ReqMessage * msg)
{
    TLOGINFO("[TARS][AdapterProxy::stat(ReqMessage) objname:" << _objectProxy->name() << ",desc:" << _endpoint.desc() << ",id:" << msg->response.iRequestId << endl);

    if(msg->bPush)
    {
        return ;
    }

    StatMicMsgBody body;
    int64_t sptime = 0;

    msg->iEndTime = TNOWMS;

    //包体信息.
    if(msg->eStatus == ReqMessage::REQ_RSP && TARSSERVERSUCCESS == msg->response.iRet)
    {
        body.count = 1;
        sptime = (msg->iEndTime >= msg->iBeginTime) ? (msg->iEndTime - msg->iBeginTime) : 10000;

        body.totalRspTime = body.minRspTime = body.maxRspTime = sptime;
    }
    else if(msg->eStatus == ReqMessage::REQ_TIME)
    {
        body.timeoutCount = 1;
    }
    else
    {
        body.execCount = 1;
    }

    auto it = _statBody.find(msg->request.sFuncName);
    if(it != _statBody.end())
    {
        merge(body,it->second);
    }
    else
    {
        _communicator->getStatReport()->getIntervCount(body.maxRspTime, body);
        _statBody[msg->request.sFuncName] = body;
    }

    if(LOG->IsNeedLog(TarsRollLogger::INFO_LOG))
    {
        ostringstream os;
        os.str("");
        _statHead.displaySimple(os);
        os << "  ";
        _statBody[msg->request.sFuncName].displaySimple(os);
        TLOGINFO("[TARS][AdapterProxy::stat(ReqMessage) display:" << os.str() << endl);
    }
}

void AdapterProxy::merge(const StatMicMsgBody& inBody,StatMicMsgBody& outBody/*out*/)
{
    outBody.count                += inBody.count;
    outBody.timeoutCount         += inBody.timeoutCount;
    outBody.execCount            += inBody.execCount;
    outBody.totalRspTime         += inBody.totalRspTime;

    if (outBody.maxRspTime < inBody.maxRspTime )
    {
        outBody.maxRspTime = inBody.maxRspTime;
    }

    //非0最小值
    if (outBody.minRspTime == 0 ||(outBody.minRspTime > inBody.minRspTime && inBody.minRspTime != 0))
    {
        outBody.minRspTime = inBody.minRspTime;
    }

    _communicator->getStatReport()->getIntervCount(inBody.maxRspTime, outBody);
}

void AdapterProxy::doStat(map<StatMicMsgHead, StatMicMsgBody> & mStatMicMsg)
{
    TLOGINFO("[TARS][AdapterProxy::doStat objname:" << _objectProxy->name() << ",desc:" << _endpoint.desc() << endl);

    for (const auto& kv : _statBody)
    {
        _statHead.interfaceName = kv.first;
        //有数据就放到map里面
        if(kv.second.count != 0 || kv.second.timeoutCount != 0 || kv.second.execCount != 0)
        {
            //判断是否已经有相同的数据了，需要汇总
            auto it = mStatMicMsg.find(_statHead);
            if(it != mStatMicMsg.end())
            {
                merge(kv.second, it->second);
            }
            else
            {
                mStatMicMsg[_statHead] = kv.second;
            }
        }
    }

    //清空数据
    _statBody.clear();
}

void AdapterProxy::addConnExc(bool bExc)
{
    if(bExc)
    {
        if(!_connExc && _connExcCnt++ >= _objectProxy->checkTimeoutInfo().maxConnectExc)
        {
            if(!_connExc)
            {
                TLOGERROR("[TARS][AdapterProxy::addConnExc desc:"<< _endpoint.desc() << ",connect exception status is true! (connect error)"<<endl);
            }
            setInactive();
            _connExc = true;
        }
    }
    else
    {
        if(_connExc)
        {
            TLOGERROR("[TARS][AdapterProxy::addConnExc desc:"<< _endpoint.desc() << ",connect exception status is false!(connect ok)"<<endl);
        }

         _connExc = false;
        _connExcCnt = 0;

        if(!_activeStatus)
        {
            _activeStatus = true;
        }
    }
}

}
