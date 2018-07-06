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

#ifndef __STOP_COMMAND_H_
#define __STOP_COMMAND_H_

#include "ServerCommand.h"

class CommandStop : public ServerCommand
{
public:
    enum
    {
        STOP_WAIT_INTERVAL  = 2,        /*服务关闭等待时间 秒*/
        STOP_SLEEP_INTERVAL = 100000,   /*微妙*/
    };
public:
    CommandStop(const ServerObjectPtr& pServerObjectPtr, bool bUseAdmin = true, bool bByNode = false, bool bGenerateCore = false);

    ExeStatus canExecute(string& sResult);

    int execute(string& sResult);

private:
    ServerObjectPtr  _serverObjectPtr;
    bool             _useAdmin; //是否使用管理端口停服务
    bool             _byNode;
    bool             _generateCore;
    ServerDescriptor _desc;
};

//////////////////////////////////////////////////////////////
//
inline CommandStop::CommandStop(const ServerObjectPtr& pServerObjectPtr, bool bUseAdmin, bool bByNode, bool bGenerateCore)
: _serverObjectPtr(pServerObjectPtr)
, _useAdmin(bUseAdmin)
, _byNode(bByNode)
, _generateCore(bGenerateCore)
{
    _desc      = _serverObjectPtr->getServerDescriptor();
}
//////////////////////////////////////////////////////////////
//
inline ServerCommand::ExeStatus CommandStop::canExecute(string& sResult)
{
    TC_ThreadRecLock::Lock lock(*_serverObjectPtr);

    NODE_LOG("stopServer")->debug() << FILE_FUN << _desc.application << "." << _desc.serverName << " beging deactivate------|byRegistry|" << _byNode << endl;

    ServerObject::InternalServerState eState = _serverObjectPtr->getInternalState();

    if (_byNode)
    {
        _serverObjectPtr->setEnabled(false);
    }

    if (eState == ServerObject::Inactive)
    {
        _serverObjectPtr->synState();
        sResult = "server state is Inactive. ";
        NODE_LOG("stopServer")->debug() << FILE_FUN << sResult << endl;
        return NO_NEED_EXECUTE;
    }

    if (eState == ServerObject::Destroying)
    {
        sResult = "server state is Destroying. ";
        NODE_LOG("stopServer")->debug() << FILE_FUN << sResult << endl;
        return DIS_EXECUTABLE;
    }

    if (eState == ServerObject::Patching || eState == ServerObject::BatchPatching)
    {
        PatchInfo tPatchInfo;
        _serverObjectPtr->getPatchPercent(tPatchInfo);
        //状态为Pathing时 10秒无更新才允许stop
        std::string sPatchType = eState == ServerObject::BatchPatching ? "BatchPatching" : "Patching";
        time_t iTimes          = eState == ServerObject::BatchPatching ? 10 * 60 : 10;
        if (TNOW - tPatchInfo.iModifyTime < iTimes)
        {
            sResult = "server is " + sPatchType + " " + TC_Common::tostr(tPatchInfo.iPercent) + "%, please try again later.....";
            NODE_LOG("stopServer")->debug() << FILE_FUN << "CommandStop::canExecute|" << sResult << endl;
            return DIS_EXECUTABLE;
        }
        else
        {
            NODE_LOG("stopServer")->debug() << FILE_FUN << "server is patching " << tPatchInfo.iPercent << "% ,and no modify info for " << TNOW - tPatchInfo.iModifyTime << "s" << endl;
        }
    }

    _serverObjectPtr->setState(ServerObject::Deactivating);

    return EXECUTABLE;
}

//////////////////////////////////////////////////////////////
//
inline int CommandStop::execute(string& sResult)
{
    bool needWait = false;
    try
    {
        pid_t pid = _serverObjectPtr->getPid();
        NODE_LOG("stopServer")->debug() << FILE_FUN << "pid:" << pid << endl;
        if (pid != 0)
        {
            string f = "/proc/" + TC_Common::tostr(pid) + "/status";
            NODE_LOG("stopServer")->debug() << FILE_FUN << "print the server status :" << f << "|" << TC_File::load2str(f) << endl;
        }

        string sStopScript   = _serverObjectPtr->getStopScript();
        //配置了脚本或者非tars服务
        if( TC_Common::lower(TC_Common::trim(_desc.serverType)) == "tars_php" ){
            //生成停止shell脚本 
            string sConfigFile      = _serverObjectPtr->getConfigFile();
            string sLogPath         = _serverObjectPtr->getLogPath();
            string sServerDir       = _serverObjectPtr->getServerDir();
            string sLibPath         = _serverObjectPtr->getLibPath();
            string sExePath         = _serverObjectPtr->getExePath();
            string sLogRealPath     = sLogPath + _desc.application +"/" + _desc.serverName + "/" ;
            string sLogRealPathFile = sLogRealPath +_serverObjectPtr->getServerId() +".log";

            TC_Config conf;
            conf.parseFile(sConfigFile);
            string phpexecPath = "";
            string entrance = "";
            try{
                phpexecPath = TC_Common::strto<string>(conf["/tars/application/server<php>"]);
                entrance =  TC_Common::strto<string>(conf["/tars/application/server<entrance>"]);
            } catch (...){}
            entrance = entrance=="" ? sServerDir+"/bin/src/index.php" : entrance;

            std::ostringstream osStopStcript;
            osStopStcript << "#!/bin/sh" << std::endl;
            osStopStcript << "if [ ! -d \"" << sLogRealPath << "\" ];then mkdir -p \"" << sLogRealPath << "\"; fi" << std::endl;
            osStopStcript << phpexecPath << " " << entrance <<" --config=" << sConfigFile << " stop  >> " << sLogRealPathFile << " 2>&1 " << std::endl;
            osStopStcript << "echo \"end-tars_stop.sh\"" << std::endl;

            TC_File::save2file(sExePath + "/tars_stop.sh", osStopStcript.str());
            TC_File::setExecutable(sExePath + "/tars_stop.sh", true);

            sStopScript = sStopScript=="" ? _serverObjectPtr->getExePath() + "/tars_stop.sh" : sStopScript;

            map<string, string> mResult;
            string sServerId     = _serverObjectPtr->getServerId();
            _serverObjectPtr->getActivator()->doScript(sServerId, sStopScript, sResult, mResult);
            needWait = true;

        }else if (!sStopScript.empty() || _serverObjectPtr->isTarsServer() == false) {
            map<string, string> mResult;
            string sServerId     = _serverObjectPtr->getServerId();
            _serverObjectPtr->getActivator()->doScript(sServerId, sStopScript, sResult, mResult);
            needWait = true;
        }
        else
        {
            if (_useAdmin)
            {
                AdminFPrx pAdminPrx;    //服务管理代理
                string  sAdminPrx = "AdminObj@" + _serverObjectPtr->getLocalEndpoint().toString();
                NODE_LOG("stopServer")->debug() << FILE_FUN << _desc.application << "." << _desc.serverName
                    << " call " << sAdminPrx << endl;
                pAdminPrx = Application::getCommunicator()->stringToProxy<AdminFPrx>(sAdminPrx);
                pAdminPrx->async_shutdown(NULL);
                needWait = true;
            }
        }

    }
    catch (exception& e)
    {
        NODE_LOG("stopServer")->debug() << FILE_FUN << _desc.application << "." << _desc.serverName << " shut down result:|" << e.what() << "|use kill -9 for check" << endl;
        pid_t pid = _serverObjectPtr->getPid();
        _serverObjectPtr->getActivator()->deactivate(pid);
    }
    catch (...)
    {
        NODE_LOG("stopServer")->debug() << FILE_FUN << _desc.application << "." << _desc.serverName << " shut down fail:|" << "|use kill -9 for check" << endl;
        pid_t pid = _serverObjectPtr->getPid();
        _serverObjectPtr->getActivator()->deactivate(pid);
    }

    //等待STOP_WAIT_INTERVAL秒
    time_t tNow = TNOW; 
    int iStopWaitInterval = STOP_WAIT_INTERVAL;
    try
    {
        //服务停止,超时时间自己定义的情况
        TC_Config conf;
        conf.parseFile(_serverObjectPtr->getConfigFile());
        iStopWaitInterval = TC_Common::strto<int>(conf["/tars/application/server<deactivating-timeout>"]) / 1000;

        if (iStopWaitInterval < STOP_WAIT_INTERVAL)
        {
            iStopWaitInterval = STOP_WAIT_INTERVAL;
        }

        if (iStopWaitInterval > 60)
        {
            iStopWaitInterval = 60;
        }

    }
    catch (...)
    {
    }

    if (needWait)
    {
        while (TNOW - iStopWaitInterval < tNow)
        {
            if (_serverObjectPtr->checkPid() != 0)  //如果pid已经不存在
            {
                _serverObjectPtr->setPid(0);
                _serverObjectPtr->setState(ServerObject::Inactive);
                _serverObjectPtr->setStarted(false);
                NODE_LOG("stopServer")->debug() << FILE_FUN << _desc.application << "." << _desc.serverName << "server already stop" << endl;
                return 0;
            }
            NODE_LOG("stopServer")->debug() << FILE_FUN << _desc.application << "." << _desc.serverName << " deactivating usleep " << int(STOP_SLEEP_INTERVAL) << endl;
            usleep(STOP_SLEEP_INTERVAL);
        }
        NODE_LOG("stopServer")->debug() << FILE_FUN << _desc.application << "." << _desc.serverName << " shut down timeout ( used " << iStopWaitInterval << "'s), use kill -9" << endl;
    }

    //仍然失败。用kill -9，再等待STOP_WAIT_INTERVAL秒。
    pid_t pid = _serverObjectPtr->getPid();
    if (_generateCore == true)
    {
        _serverObjectPtr->getActivator()->deactivateAndGenerateCore(pid);
    }
    else
    {
        _serverObjectPtr->getActivator()->deactivate(pid);
    }

    tNow = TNOW;
    while (TNOW - STOP_WAIT_INTERVAL < tNow)
    {
        if (_serverObjectPtr->checkPid() != 0)  //如果pid已经不存在
        {
            _serverObjectPtr->setPid(0);
            _serverObjectPtr->setState(ServerObject::Inactive);
            _serverObjectPtr->setStarted(false);
            return 0;
        }
        usleep(STOP_SLEEP_INTERVAL);
    }
    sResult = "server pid " + TC_Common::tostr(pid) + " is still exist";

    NODE_LOG("stopServer")->debug() << FILE_FUN << _desc.application << "." << _desc.serverName << ", " << sResult << endl;

    return  -1;
}

#endif
