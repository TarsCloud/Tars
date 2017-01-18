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

#include "ServerObject.h"
#include "RegistryProxy.h"
#include "util/tc_clientsocket.h"
#include "servant/Communicator.h"
#include "NodeServer.h"
#include "CommandStart.h"
#include "CommandNotify.h"
#include "CommandStop.h"
#include "CommandDestroy.h"
#include "CommandAddFile.h"
#include "NodeRollLogger.h"

ServerObject::ServerObject( const ServerDescriptor& tDesc)
: _tarsServer(true)
, _loaded(false)
, _patched(true)
, _noticed(false)
, _noticeFailTimes(0)
, _enSynState(true)
, _pid(0)
, _state(ServerObject::Inactive)
, _limitStateUpdated(false)
, _started(false)
{
    //60秒内最多启动10次，达到10次启动仍失败后,每隔600秒再重试一次
    _activatorPtr  = new Activator(60,10,600);

    //服务相关修改集中放到setServerDescriptor函数中
     setServerDescriptor(tDesc);

    _serviceLimitResource = new ServerLimitResource(5,10,60,_application,_serverName);
}

void ServerObject::setServerDescriptor( const ServerDescriptor& tDesc )
{
    TLOGDEBUG("ServerObject::setServerDescriptor "<< tDesc.application << "." << tDesc.serverName <<endl);
    
    _desc          = tDesc;
    _application   = tDesc.application;
    _serverName    = tDesc.serverName;
    _serverId      = _application+"."+_serverName;
    _desc.settingState == "active"?_enabled = true:_enabled = false;
    
    time_t now = TNOW;
    _adapterKeepAliveTime.clear();

     map<string, AdapterDescriptor>::const_iterator itAdapters;
     for( itAdapters = _desc.adapters.begin(); itAdapters != _desc.adapters.end(); itAdapters++)
     {
         _adapterKeepAliveTime[itAdapters->first] = now;
     }
     _adapterKeepAliveTime["AdminAdapter"] = now;
     setLastKeepAliveTime(now);
}

bool ServerObject::isAutoStart()
{
    Lock lock(*this);
    TLOGDEBUG( "ServerObject::isAutoStart "<< _application << "." << _serverName <<"|"<<_enabled<<"|"<<_loaded<<"|"<<_patched<<"|"<<toStringState(_state)<<endl);
    NODE_LOG("KeepAliveThread")->debug()<<FILE_FUN<< _application << "." << _serverName <<"|"<<_enabled<<"|"<<_loaded<<"|"<<_patched<<"|"<<toStringState(_state)<<endl; 
    if(toStringState(_state).find( "ing" ) != string::npos)  //正处于中间态时不允许重启动
    {
        TLOGDEBUG("ServerObject::isAutoStart " << _application << "." << _serverName <<" not allow to restart in (ing) state"<<endl);
        return false;
    }
    
    if(!_enabled||!_loaded || !_patched || _activatorPtr->isActivatingLimited())
    {
           if(_activatorPtr->isActivatingLimited())
           {
               TLOGDEBUG("ServerObject::isAutoStart " << _application << "." << _serverName <<" not allow to restart in limited state"<<endl);
           }
        return false;
    }
    return true;
}

ServerState ServerObject::getState()
{
    Lock lock(*this);
    TLOGINFO("ServerObject::getState "<<_application << "." << _serverName <<"'s state is "<<toStringState(_state)<<endl);
    return toServerState( _state );
}

void ServerObject::setState(InternalServerState eState, bool bSynState)
{
    Lock lock(*this);

    if (_state == eState  && _noticed == true)
    {
        return;
    }

    if ( _state != eState )
    {
        TLOGDEBUG(FILE_FUN << _application << "." << _serverName << " State changed! old State:" << toStringState( _state ) << " new State:" << toStringState( eState ) << endl);
        _state = eState;
    }

    if(bSynState == true)
    {
        synState();
    }
}

void ServerObject::setLastState(InternalServerState eState)
{
    Lock lock(*this);

    _lastState = eState;
}

bool ServerObject::isScriptFile(const string &sFileName)
{
    if(TC_Common::lower(TC_File::extractFileExt(sFileName)) == "sh" ) //.sh文件为脚本
    {
        return true;
    }

    if(TC_File::extractFileName(_startScript) == sFileName )
    {
        return true;
    }

    if(TC_File::extractFileName(_stopScript) == sFileName )
    {
        return true;
    }

    if(TC_File::extractFileName(_monitorScript) == sFileName )
    {
        return true;
    }

    return false;
}

//同步更新 重启服务 需要准确通知主控状态时调用
void ServerObject::synState()
{
    Lock lock(*this);
    try
    {
        ServerStateInfo tServerStateInfo;
        tServerStateInfo.serverState    = (IsEnSynState()?toServerState(_state):tars::Inactive);
        tServerStateInfo.processId      = _pid;
        //根据uNoticeFailTimes判断同步还是异步更新服务状态
        //防止主控超时导致阻塞服务上报心跳
        if(_noticeFailTimes < 3)
        {
            AdminProxy::getInstance()->getRegistryProxy()->updateServer( _nodeInfo.nodeName,  _application, _serverName, tServerStateInfo);
        }
        else
        {
            AdminProxy::getInstance()->getRegistryProxy()->async_updateServer(NULL, _nodeInfo.nodeName,  _application, _serverName, tServerStateInfo);
        }

        //日志
        stringstream ss;
        tServerStateInfo.displaySimple(ss);
        NODE_LOG("synState")->debug()<<FILE_FUN << "synState" << "|"<< _nodeInfo.nodeName << "|" <<  _application << "|" << _serverName
                << "|" << std::boolalpha << _enSynState <<"|" << ss.str() << endl;

        _noticed = true;
        _noticeFailTimes = 0;
    }
    catch (exception &e)
    {
        _noticed = false;
        _noticeFailTimes ++;
        TLOGERROR("ServerObject::synState "<<_application<<"."<<_serverName<<" error times:"<<_noticeFailTimes<<" reason:"<< e.what() << endl);
    }
}

//异步同步状态
void ServerObject::asyncSynState()
{
    Lock lock(*this);
    try
    {
        ServerStateInfo tServerStateInfo;
        tServerStateInfo.serverState    = (IsEnSynState()?toServerState(_state):tars::Inactive);
        tServerStateInfo.processId      = _pid;
        AdminProxy::getInstance()->getRegistryProxy()->async_updateServer( NULL, _nodeInfo.nodeName,  _application, _serverName, tServerStateInfo);

        //日志
        stringstream ss;
        tServerStateInfo.displaySimple(ss);
        NODE_LOG("synState")->debug()<<FILE_FUN<< _nodeInfo.nodeName << "|" <<  _application << "|" << _serverName
                << "|" << std::boolalpha << _enSynState <<"|" << ss.str() << endl;

    }
    catch (exception &e)
    {
        TLOGERROR("ServerObject::asyncSynState "<<_application<<"."<<_serverName<<" error:" << e.what() << endl);
    }
}

ServerState ServerObject::toServerState(InternalServerState eState) const
{
    switch (eState)
    {
        case Inactive:
        {
            return tars::Inactive;
        }
        case Activating:
        {
            return tars::Activating;
        }
        case Active:
        {
            return tars::Active;
        }
        case Deactivating:
        {
            return tars::Deactivating;
        }
        case Destroying:
        {
            return tars::Destroying;
        }
        case Destroyed:
        {
            return tars::Destroyed;
        }
        default:
        {
            return tars::Inactive;
        }
    }
}

string ServerObject::toStringState(InternalServerState eState) const
{
    switch (eState)
    {
        case Inactive:
        {
            return "Inactive";
        }
        case Activating:
        {
            return "Activating";
        }
        case Active:
        {
            return "Active";
        }
        case Deactivating:
        {
            return "Deactivating";
        }
        case Destroying:
        {
            return "Destroying";
        }
        case Destroyed:
        {
            return "Destroyed";
        }
        case Loading:
        {
            return "Loading";
        }
        case Patching:
        {
            return "Patching";
        }
        case BatchPatching:
        {
            return "BatchPatching";
        }
        case AddFilesing:
        {
            return "AddFilesing";
        }
        default:
        {
            ostringstream os;
            os << "state " << eState;
            return os.str();
        }
    };
}

int ServerObject::checkPid()
{
    Lock lock(*this);
    if(_pid != 0)
    {
        int iRet = _activatorPtr->sendSignal(_pid, 0);
        if (iRet == 0)
        {
            NODE_LOG("KeepAliveThread")->debug() <<FILE_FUN<< _serverId << "|" << _pid << "|" << iRet << endl;
            return 0;
        }
    }
    NODE_LOG("KeepAliveThread")->error() <<FILE_FUN<< _serverId << "|" << _pid << "|-1" << endl;
    return -1;
}

void ServerObject::keepAlive(pid_t pid,const string &adapter)
{
    Lock lock(*this);
    if (pid <= 0)
    {
        TLOGERROR("ServerObject::keepAlive "<< _application << "." << _serverName << " pid "<<pid<<" error, pid <= 0"<<endl);
        return;
    }
    else
    {
        TLOGDEBUG("ServerObject::keepAlive "<<_serverType<< "|pid|" << pid <<"|server|"<<_application << "." << _serverName <<"|"<<adapter<< endl);
    }
    time_t now  = TNOW;
    setLastKeepAliveTime(now,adapter);
    //setLastKeepAliveTime(now);
    setPid(pid);

    //心跳不改变正在转换期状态(Activating除外)
    if(toStringState(_state).find("ing") == string::npos || _state == ServerObject::Activating)
    {
        TLOGDEBUG("ServerObject::keepAlive "<< _desc.application << "|" << _desc.serverName << "|" << toStringState(_state) << "|" << _state << endl);
        setState(ServerObject::Active);
    }
    else
    {
         TLOGDEBUG("ServerObject::keepAlive " << _application << "." << _serverName << " State no need changed to active!  State:" << toStringState( _state ) << endl);
    }

    if(!_loaded)
    {
        _loaded = true;
    }

    if(!_patched)
    {
        _patched = true;
    }
}

void ServerObject::setLastKeepAliveTime(int t,const string& adapter)
{
    Lock lock(*this);
    _keepAliveTime = t;
    map<string, time_t>::iterator it1 = _adapterKeepAliveTime.begin();
    if(adapter.empty())
    {
        while (it1 != _adapterKeepAliveTime.end() )
        {
            it1->second = t;
            it1++;
        }
        return;
    }
    map<string, time_t>::iterator it2 = _adapterKeepAliveTime.find(adapter);
    if( it2 != _adapterKeepAliveTime.end())
    {
        it2->second = t;
    }
    else
    {
        TLOGERROR("ServerObject::setLastKeepAliveTime "<<adapter<<" not registed "<< endl);
    }
}

int ServerObject::getLastKeepAliveTime(const string &adapter)
{
    if(adapter.empty())
    {
        return _keepAliveTime;
    }
    map<string, time_t>::const_iterator it = _adapterKeepAliveTime.find(adapter);
    if( it != _adapterKeepAliveTime.end())
    {
        return it->second;
    }
    return -1;
}


bool ServerObject::isTimeOut(int iTimeout)
{
    Lock lock(*this);
    time_t now = TNOW;
    if(now - _keepAliveTime > iTimeout)
    {
        TLOGERROR("ServerObject::isTimeOut server time  out "<<now - _keepAliveTime<<"|>|"<<iTimeout<< endl);
        return true;
    }
    map<string, time_t>::const_iterator it = _adapterKeepAliveTime.begin();
    while (it != _adapterKeepAliveTime.end() )
    {
        if(now - it->second > iTimeout)
        {
            TLOGERROR("ServerObject::isTimeOut server "<< it->first<<" time  out "<<now - it->second<<"|>|"<<iTimeout<<"|"<<TC_Common::tostr(_adapterKeepAliveTime)<< endl);
            return true;
        }
        it++;
    }
    return false;
}

bool ServerObject::isStartTimeOut()
{
	Lock lock(*this);
	int64_t now = TNOWMS;

	if (now - _startTime >= 100)//默认为100ms,_timeout
	{
		NODE_LOG("KeepAliveThread")->debug()<<FILE_FUN<<"server start time  out "<<now - _startTime<<"|>|"<<100<<endl;
		return true;
	}
	NODE_LOG("KeepAliveThread")->debug()<<"server start time  out "<<now - _startTime<<"|<|"<<100<<endl;

	return false;
}

void ServerObject::setVersion( const string & version )
{
    Lock lock(*this);
    try
    {
        _version = version;
        AdminProxy::getInstance()->getRegistryProxy()->async_reportVersion(NULL,_application, _serverName, _nodeInfo.nodeName, version);

    }catch(...)
    {
        _noticed = false;
    }
}

void ServerObject::setPid(pid_t pid)
{
    Lock lock(*this);
    if (pid == _pid)
    {
        return;
    }
    NODE_LOG("startServer")->debug()<<FILE_FUN<< _application << "." << _serverName << " pid changed! old pid:" << _pid << " new pid:" << pid << endl;
    _pid = pid;
}

void ServerObject::setPatchPercent(const int iPercent)
{
    Lock lock(*this);
    //下载结束仍需要等待本地copy  进度最多设置99% 本地copy结束后设置为100%
    _patchInfo.iPercent      = iPercent>99?99:iPercent;
    _patchInfo.iModifyTime   = TNOW;
}

void ServerObject::setPatchResult(const string &sPatchResult,const bool bSucc)
{
    Lock lock(*this);
    _patchInfo.sResult = sPatchResult;
    _patchInfo.bSucc   = bSucc;
}

void ServerObject::setPatchVersion(const string &sVersion)
{
    Lock lock(*this);
    _patchInfo.sVersion = sVersion;
}

string ServerObject::getPatchVersion()
{
    Lock lock(*this);
    return _patchInfo.sVersion;
}

int ServerObject::getPatchPercent(PatchInfo &tPatchInfo)
{
    TLOGDEBUG("ServerObject::getPatchPercent"<< _application << "_" << _serverName << "|"<< _serverId<< endl);
    Lock lock(*this);
    TLOGDEBUG("ServerObject::getPatchPercent "<< _application << "_" << _serverName << "|"<< _serverId << "get lock" << endl);

    tPatchInfo              = _patchInfo;
    //是否正在发布
    tPatchInfo.bPatching    = (_state == ServerObject::Patching ||_state==ServerObject::BatchPatching)?true:false;

    if (tPatchInfo.bSucc == true || _state == ServerObject::Patching || _state == ServerObject::BatchPatching)
    {
        TLOGDEBUG("ServerObject::getPatchPercent "<< _desc.application
            << "|" << _desc.serverName << "|succ:" << (tPatchInfo.bSucc?"true":"false") << "|" << toStringState(_state) << "|" << _patchInfo.iPercent << "%|" << _patchInfo.sResult << endl);
        return 0;
    }

    TLOGERROR("ServerObject::getPatchPercent "<< _desc.application
           << "|" << _desc.serverName << "|succ:" << (tPatchInfo.bSucc?"true":"false") << "|" << toStringState(_state) << "|" << _patchInfo.iPercent << "%|" << _patchInfo.sResult << endl);
    return -1;
}

string ServerObject::decodeMacro(const string& value) const
{
    string tmp = value;
    map<string,string>::const_iterator it = _macro.begin();
    while(it != _macro.end())
    {
        tmp = TC_Common::replace(tmp, "${" + it->first + "}", it->second);
        ++it;
    }
    return tmp;
}

void ServerObject::setMacro(const map<string,string>& mMacro)
{
    Lock lock(*this);
    map<string,string>::const_iterator it1 = mMacro.begin();
    for(;it1!= mMacro.end();++it1)
    {
        map<string,string>::iterator it2 = _macro.find(it1->first);

        if(it2 != _macro.end())
        {
            it2->second = it1->second;
        }
        else
        {
            _macro[it1->first] = it1->second;
        }
    }
}

void ServerObject::setScript(const string &sStartScript,const string &sStopScript,const string &sMonitorScript )
{

    _startScript    = TC_File::simplifyDirectory(TC_Common::trim(sStartScript));
    _stopScript     = TC_File::simplifyDirectory(TC_Common::trim(sStopScript));
    _monitorScript  = TC_File::simplifyDirectory(TC_Common::trim(sMonitorScript));
}

bool ServerObject::getRemoteScript(const string &sFileName)
{
    string sFile = TC_File::simplifyDirectory(TC_Common::trim(sFileName));
    if(!sFile.empty())
    {
       CommandAddFile commands(this,sFile);
       commands.doProcess(); //拉取脚本
       return true;
    }
    return false;
}

void ServerObject::doMonScript()
{
    try
    {
        string sResult;
        time_t tNow = TNOW;
        if(TC_File::isAbsolute(_monitorScript) == true) //监控脚本
        {
            if(_state == ServerObject::Activating||tNow - _keepAliveTime > ServantHandle::HEART_BEAT_INTERVAL)
            {
                 map<string,string> mResult;
                 _activatorPtr->doScript(_serverId,_monitorScript,sResult,mResult);
                 if(mResult.find("pid") != mResult.end() && TC_Common::isdigit(mResult["pid"]) == true)
                 {
                     TLOGDEBUG("ServerObject::doMonScript "<< _serverId << "|"<< mResult["pid"] << endl);
                     keepAlive(TC_Common::strto<int>(mResult["pid"]));
                 }
            }
        }
    }
    catch (exception &e)
    {
        TLOGERROR("ServerObject::doMonScript error:" << e.what() << endl);
    }
}

void ServerObject::checkServer(int iTimeout)//checkServer时对服务所占用的内存上报到主控
{
    try
    {
        string sResult;
        NODE_LOG("KeepAliveThread")->debug() <<FILE_FUN<<_serverId <<"|"<<toStringState(_state)<< endl;
        //pid不存在属于服务异常
        if( _state != ServerObject::Inactive && _state != ServerObject::Deactivating && (checkPid() != 0))
        {
			bool bShouldStop = false;
			if (_state == ServerObject::Activating)
			{
				if (_started && isStartTimeOut())
				{
					bShouldStop = true;
					sResult="[alarm] activating,pid not exist";
					NODE_LOG("KeepAliveThread")->debug()<<FILE_FUN<<"|sResult:"<<sResult<<endl;
				}
				NODE_LOG("KeepAliveThread")->debug()<<FILE_FUN<<"|is not bstarted"<<endl;
			}
			else
			{
				bShouldStop = true;
				sResult = "[alarm] down,pid not exist";
				NODE_LOG("KeepAliveThread")->debug()<<FILE_FUN<<"|sResult:"<<sResult<<endl;
			}
			
			if (bShouldStop)
			{
				NODE_LOG("KeepAliveThread")->debug()<<FILE_FUN<<_serverId<<" "<<sResult << "|pid|" << _pid  << "|_state:" << toStringState(_state)<<"|_started:"<<_started<< endl;
				CommandStop command(this, false);
				command.doProcess();
			}
        }
        else
        {
            NODE_LOG("KeepAliveThread")->debug() <<FILE_FUN<< _serverId<<"|" << toStringState(_state) << "|" << _pid << endl;
        }

        //正在运行程序心跳上报超时。
        int iRealTimeout = _timeout > 0?_timeout:iTimeout;
        if( _state != ServerObject::Inactive && isTimeOut(iRealTimeout))
        {
            sResult = "[alarm] zombie process,no keep alive msg for " + TC_Common::tostr(iRealTimeout) + " seconds";
            NODE_LOG("KeepAliveThread")->debug()<<FILE_FUN<<_serverId<<" "<<sResult << endl;
            CommandStop command(this, true);
            command.doProcess();
        }

        //启动服务
        if( _state == ServerObject::Inactive && isAutoStart() == true)
        {
            sResult = sResult == ""?"[alarm] down, server is inactive":sResult;
            NODE_LOG("KeepAliveThread")->debug() <<FILE_FUN<<_serverId<<" "<<sResult << "|_state:" << toStringState(_state) << endl;

            g_app.reportServer(_serverId,sResult);

            CommandStart command(this);
            int ret = command.doProcess();
            NODE_LOG("KeepAliveThread")->debug() <<FILE_FUN<<"|start ret:" << ret << endl;
            //配置了coredump检测才进行操作
            if(_limitStateInfo.bEnableCoreLimit)
            {
                _serviceLimitResource->addExcStopRecord();
            }
        }
    }
    catch(exception &ex)
    {
        NODE_LOG("KeepAliveThread")->error()<<FILE_FUN << "ex:" << ex.what() << endl;
        TLOGERROR("ServerObject::checkServer ex:" << ex.what() << endl);
    }
    catch(...)
    {
        NODE_LOG("KeepAliveThread")->error()<<FILE_FUN << "unknown ex." << endl;
        TLOGERROR("ServerObject::checkServer unknown ex." << endl);
    }
}

/**
 * 属性上报单独出来
 */
void ServerObject::reportMemProperty()
{
    try
    {
        char sTmp[64] ={0};
        snprintf(sTmp,sizeof(sTmp), "%u", _pid);
        string spid = string(sTmp);

        string filename = "/proc/" + spid + "/statm";
        string stream;
        stream = TC_File::load2str(filename);
        if(!stream.empty())
        {
            NODE_LOG("ReportMemThread")->debug()<<FILE_FUN<<"filename:"<<filename<<",stream:"<<stream<<endl;
            //>>改成上报物理内存
            vector<string> vtStatm = TC_Common::sepstr<string>(stream, " ");
            if (vtStatm.size() < 2)
            {
                NODE_LOG("ReportMemThread")->error() <<FILE_FUN<< "cannot get server[" + _serverId  + "] physical mem size|stream|" << stream  << endl;
            }
            else
            {
                stream = vtStatm[1];
            }

            if(TC_Common::isdigit(stream))
            {
                REPORT_MAX(_serverId, _serverId+".memsize", TC_Common::strto<int>(stream) * 4);
                NODE_LOG("ReportMemThread")->debug()<<FILE_FUN<<"report_max("<<_serverId<<".memsize,"<<TC_Common::strto<int>(stream)*4<<")OK."<<endl;
                return;
            }
            else
            {
                NODE_LOG("ReportMemThread")->error()<<FILE_FUN<<"stream : "<<stream<<" ,is not a digit."<<endl;
            }
       }
       else
       {
           NODE_LOG("ReportMemThread")->error()<<FILE_FUN<<"stream is empty: "<<stream<<endl;
       }
    }
    catch(exception &ex)
    {
        NODE_LOG("ReportMemThread")->error() << FILE_FUN << " ex:" << ex.what() << endl;
        TLOGERROR(FILE_FUN << " ex:" << ex.what() << endl);
    }
    catch(...)
    {
        NODE_LOG("ReportMemThread")->error() << FILE_FUN << " unknown error" << endl;
        TLOGERROR(FILE_FUN << "unknown ex." << endl);
    }
}

void ServerObject::checkCoredumpLimit()
{
    if(_state != ServerObject::Active || !_limitStateInfo.bEnableCoreLimit)
    {
        TLOGINFO(FILE_FUN<<"checkCoredumpLimit:server is inactive or disable corelimit"<<endl);
        return;
    }

    Lock lock(*this);
    if(_limitStateInfo.eCoreType == EM_AUTO_LIMIT)
    {
        bool bNeedClose=false;
        int iRet =_serviceLimitResource->IsCoreLimitNeedClose(bNeedClose);

        bool bNeedUpdate = (iRet==2)?true:false;

        if(_limitStateUpdated && bNeedClose)
        {
            _limitStateUpdated = setServerCoreLimit(true)?false:true;//设置成功后再屏蔽更新
            _limitStateInfo.bCloseCore = bNeedClose;
        }
        else if(bNeedUpdate && !bNeedClose)
        {
            setServerCoreLimit(false);
            _limitStateInfo.bCloseCore = bNeedClose;
        }
    }
    else
    {
        if(_limitStateUpdated)
        {
            _limitStateUpdated = setServerCoreLimit(_limitStateInfo.bCloseCore)?false:true; //设置成功后再屏蔽更新
        }
    }
}

bool ServerObject::setServerCoreLimit(bool bCloseCore)
{
    string sResult = "succ";
    const string sCmd = (bCloseCore)?("tars.closecore yes"):("tars.closecore no");

    CommandNotify command(this,sCmd);
    int iRet = command.doProcess(sResult);

    NODE_LOG("KeepAliveThread")->debug()<<FILE_FUN<<"setServerCoreLimit|"<<_serverId<<"|"<<sCmd<<"|"<<sResult<<endl;

    return (iRet==0);

}

//服务重启时设置
void ServerObject::setLimitInfoUpdate(bool bUpdate)
{
    Lock lock(*this);

    _limitStateUpdated = true;
}

//手动配置有更新时调用
void ServerObject::setServerLimitInfo(const ServerLimitInfo& tInfo)
{
    Lock lock(*this);

    _limitStateInfo = tInfo;

    NODE_LOG("KeepAliveThread")->debug() << FILE_FUN << _serverId << "|" << _limitStateInfo.str() << endl;

    _serviceLimitResource->setLimitCheckInfo(tInfo.iMaxExcStopCount,tInfo.iCoreLimitTimeInterval,tInfo.iCoreLimitExpiredTime);

    _activatorPtr->setLimitInfo(tInfo.iActivatorTimeInterval, tInfo.iActivatorMaxCount, tInfo.iActivatorPunishInterval);

    _limitStateUpdated = true;
}

//重置core计数信息
void ServerObject::resetCoreInfo()
{
    _serviceLimitResource->resetRunTimeData();
}

void ServerObject::setStarted(bool bStarted)
{
	Lock lock(*this);
	_started=bStarted;
}

void ServerObject::setStartTime(int64_t iStartTime)
{
	Lock lock(*this);
	_startTime = iStartTime;
}