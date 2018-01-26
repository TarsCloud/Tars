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

#ifndef __START_COMMAND_H_
#define __START_COMMAND_H_

#include "ServerCommand.h"
#include "NodeRollLogger.h"
#include "util/tc_timeprovider.h"

class CommandStart : public ServerCommand
{
public:
    enum
    {
        START_WAIT_INTERVAL  = 3,        /*服务启动等待时间 秒*/
        START_SLEEP_INTERVAL = 100000,   /*微妙*/
    };
public:
    CommandStart(const ServerObjectPtr& pServerObjectPtr, bool bByNode = false);

    ExeStatus canExecute(string& sResult);

    int execute(string& sResult);

private:
    bool startNormal(string& sResult);

    bool startByScript(string& sResult);

private:
    bool                _byNode;
    string              _exeFile;
    string              _logPath;
    ServerDescriptor    _desc;
    ServerObjectPtr     _serverObjectPtr;
};

//////////////////////////////////////////////////////////////
//
inline CommandStart::CommandStart(const ServerObjectPtr& pServerObjectPtr, bool bByNode)
: _byNode(bByNode)
, _serverObjectPtr(pServerObjectPtr)
{
    _exeFile   = _serverObjectPtr->getExeFile();
    _logPath   = _serverObjectPtr->getLogPath();
    _desc      = _serverObjectPtr->getServerDescriptor();
}
//////////////////////////////////////////////////////////////
//
inline ServerCommand::ExeStatus CommandStart::canExecute(string& sResult)
{

    TC_ThreadRecLock::Lock lock(*_serverObjectPtr);

    NODE_LOG("startServer")->debug() << FILE_FUN << _desc.application << "." << _desc.serverName << " beging activate------|byRegistry|" << _byNode << endl;

    ServerObject::InternalServerState eState = _serverObjectPtr->getInternalState();

    if (_byNode)
    {
        _serverObjectPtr->setEnabled(true);
    }
    else if (!_serverObjectPtr->isEnabled())
    {
        sResult = "server is disabled";
        NODE_LOG("startServer")->debug() << FILE_FUN << sResult << endl;
        return DIS_EXECUTABLE;
    }

    if (eState == ServerObject::Active || eState == ServerObject::Activating)
    {
        _serverObjectPtr->synState();
        sResult = "server is already " + _serverObjectPtr->toStringState(eState);
        NODE_LOG("startServer")->debug() << FILE_FUN << sResult << endl;
        return NO_NEED_EXECUTE;
    }

    if (eState != ServerObject::Inactive)
    {
        sResult = "server state is not Inactive. the curent state is " + _serverObjectPtr->toStringState(eState);
        NODE_LOG("startServer")->debug() << FILE_FUN << sResult << endl;
        return DIS_EXECUTABLE;
    }

    if (TC_File::isAbsolute(_exeFile) &&
        !TC_File::isFileExistEx(_exeFile) &&
        _serverObjectPtr->getStartScript().empty() &&
        _serverObjectPtr->getServerType() != "tars_nodejs")
    {
        _serverObjectPtr->setPatched(false);
        sResult      = "The server exe patch " + _exeFile + " is not exist.";
        NODE_LOG("startServer")->debug() << FILE_FUN << sResult << endl;
        return DIS_EXECUTABLE;
    }

    _serverObjectPtr->setPatched(true);

    _serverObjectPtr->setState(ServerObject::Activating, false); //此时不通知regisrty。checkpid成功后再通知

    return EXECUTABLE;
}

inline bool CommandStart::startByScript(string& sResult)
{
    pid_t iPid = -1;
    bool bSucc = false;
    string sStartScript     = _serverObjectPtr->getStartScript();
    string sMonitorScript   = _serverObjectPtr->getMonitorScript();

    string sServerId    = _serverObjectPtr->getServerId();
    iPid = _serverObjectPtr->getActivator()->activate(sServerId, sStartScript, sMonitorScript, sResult);

    vector<string> vtServerName =  TC_Common::sepstr<string>(sServerId, ".");
    if (vtServerName.size() != 2)
    {
        sResult = sResult + "|failed to get pid for  " + sServerId + ",server id error";
        NODE_LOG("startServer")->error() << FILE_FUN << sResult  << endl;
        throw runtime_error(sResult);
    }

    //默认使用启动脚本路径
    string sFullExeFileName = TC_File::extractFilePath(sStartScript) + vtServerName[1];
    if (!TC_File::isFileExist(sFullExeFileName))
    {
        NODE_LOG("startServer")->error() << FILE_FUN << "file name " << sFullExeFileName << " error, use exe file" << endl;

        //使用exe路径
        sFullExeFileName = _serverObjectPtr->getExeFile();
        if (!TC_File::isFileExist(sFullExeFileName))
        {
            sResult = sResult + "|failed to get full exe file name|" + sFullExeFileName;
            NODE_LOG("startServer")->error() << FILE_FUN << sResult << endl;
            throw runtime_error(sResult);
        }
    }
    time_t tNow = TNOW;
    int iStartWaitInterval = START_WAIT_INTERVAL;
    try
    {
        //服务启动,超时时间自己定义的情况
        TC_Config conf;
        conf.parseFile(_serverObjectPtr->getConfigFile());
        iStartWaitInterval = TC_Common::strto<int>(conf.get("/tars/application/server<activating-timeout>", "3000")) / 1000;
        if (iStartWaitInterval < START_WAIT_INTERVAL)
        {
            iStartWaitInterval = START_WAIT_INTERVAL;
        }
        if (iStartWaitInterval > 60)
        {
            iStartWaitInterval = 60;
        }

    }
    catch (...)
    {
    }

    string sPidFile = "/usr/local/app/tars/tarsnode/util/" + sServerId + ".pid";
    string sGetServerPidScript = "ps -ef | grep -v 'grep' |grep -iE ' " + sFullExeFileName + " '| awk '{print $2}' > " + sPidFile;

    while ((TNOW - iStartWaitInterval) < tNow)
    {
        //注意:由于是守护进程,不要对sytem返回值进行判断,始终wait不到子进程
        system(sGetServerPidScript.c_str());
        string sPid = TC_Common::trim(TC_File::load2str(sPidFile));
        if (TC_Common::isdigit(sPid))
        {
            iPid = TC_Common::strto<int>(sPid);
            _serverObjectPtr->setPid(iPid);
            if (_serverObjectPtr->checkPid() == 0)
            {
                bSucc = true;
                break;
            }
        }

        NODE_LOG("startServer")->debug() << FILE_FUN << _desc.application << "." << _desc.serverName << " activating usleep " << int(iStartWaitInterval) << endl;
        usleep(START_SLEEP_INTERVAL);
    }

    if (_serverObjectPtr->checkPid() != 0)
    {
        sResult = sResult + "|get pid for server[" + sServerId + "],pid is not digit";
        NODE_LOG("startServer")->error() << FILE_FUN << sResult << endl;
        if (TC_File::isFileExist(sPidFile))
        {
            TC_File::removeFile(sPidFile, false);
        }
        throw runtime_error(sResult);
    }

    if (TC_File::isFileExist(sPidFile))
    {
        TC_File::removeFile(sPidFile, false);
    }
    return bSucc;
}

inline bool CommandStart::startNormal(string& sResult)
{
    pid_t iPid = -1;
    bool bSucc  = false;
    string sRollLogFile;
    vector<string> vEnvs;
    vector<string> vOptions;
    string sConfigFile      = _serverObjectPtr->getConfigFile();
    string sLogPath         = _serverObjectPtr->getLogPath();
    string sServerDir       = _serverObjectPtr->getServerDir();
    string sLibPath         = _serverObjectPtr->getLibPath();
    string sExePath         = _serverObjectPtr->getExePath();

    //环境变量设置
    vector<string> vecEnvs = TC_Common::sepstr<string>(_serverObjectPtr->getEnv(), ";|");
    for (vector<string>::size_type i = 0; i < vecEnvs.size(); i++)
    {
        vEnvs.push_back(vecEnvs[i]);
    }

    {
        vEnvs.push_back("LD_LIBRARY_PATH=$LD_LIBRARY_PATH:" + sExePath + ":" + sLibPath);
    }

    //生成启动脚本
    std::ostringstream osStartStcript;
    osStartStcript << "#!/bin/sh" << std::endl;
    for (vector<string>::size_type i = 0; i < vEnvs.size(); i++)
    {
        osStartStcript << "export " << vEnvs[i] << std::endl;
    }

    osStartStcript << std::endl;
    if (_serverObjectPtr->getServerType() == "tars_java")
    {
        //java服务
        string sClassPath       = _serverObjectPtr->getClassPath();
        string sJarList         = sExePath + "/classes";
        vector<string> vJarList;
        TC_File::scanDir(sClassPath, vJarList);

        sJarList = sJarList + ":" + sClassPath + "/*";

        vOptions.push_back("-Dconfig=" + sConfigFile);
        string s = _serverObjectPtr->getJvmParams() + " -cp " + sJarList + " " + _serverObjectPtr->getMainClass();
        vector<string> v = TC_Common::sepstr<string>(s, " \t");
        vOptions.insert(vOptions.end(), v.begin(), v.end());

        osStartStcript << _exeFile << " " << TC_Common::tostr(vOptions) << endl;
    }
    else if (_serverObjectPtr->getServerType() == "tars_nodejs") 
    { 
        vOptions.push_back(sExePath + "/tars_nodejs/node-agent/bin/node-agent");
        string s = sExePath + "/src/ -c " + sConfigFile; 
        vector<string> v = TC_Common::sepstr<string>(s," \t"); 
        vOptions.insert(vOptions.end(), v.begin(), v.end());

        //对于tars_nodejs类型需要修改下_exeFile
        _exeFile = sExePath+"/tars_nodejs/node";

        osStartStcript<<sExePath+"/tars_nodejs/node" <<" "<<TC_Common::tostr(vOptions)<<" &"<< endl;
    }
    else
    {
        //c++服务
        vOptions.push_back("--config=" + sConfigFile);
        osStartStcript << _exeFile << " " << TC_Common::tostr(vOptions) << " &" << endl;
    }

    //保存启动方式到bin目录供手工启动
    TC_File::save2file(sExePath + "/tars_start.sh", osStartStcript.str());
    TC_File::setExecutable(sExePath + "/tars_start.sh", true);

    if (sLogPath != "")
    {
        sRollLogFile = sLogPath + "/" + _desc.application + "/" + _desc.serverName + "/" + _desc.application + "." + _desc.serverName + ".log";
    }

    const string sPwdPath = sLogPath != "" ? sLogPath : sServerDir; //pwd patch 统一设置至log目录 以便core文件发现 删除
    iPid = _serverObjectPtr->getActivator()->activate(_exeFile, sPwdPath, sRollLogFile, vOptions, vEnvs);
    if (iPid == 0)  //child process
    {
        return false;
    }

    _serverObjectPtr->setPid(iPid);

    bSucc = (_serverObjectPtr->checkPid() == 0) ? true : false;

    return bSucc;
}
//////////////////////////////////////////////////////////////
//
inline int CommandStart::execute(string& sResult)
{
    int64_t startMs = TC_TimeProvider::getInstance()->getNowMs();
    try
    {
        bool bSucc  = false;
        //set stdout & stderr
        _serverObjectPtr->getActivator()->setRedirectPath(_serverObjectPtr->getRedirectPath());

        if (!_serverObjectPtr->getStartScript().empty() || _serverObjectPtr->isTarsServer() == false)
        {
            bSucc = startByScript(sResult);
        }
        else
        {
            bSucc = startNormal(sResult);
        }

        if (bSucc == true)
        {
            _serverObjectPtr->synState();
            _serverObjectPtr->setLastKeepAliveTime(TNOW);
            _serverObjectPtr->setLimitInfoUpdate(true);
            _serverObjectPtr->setStarted(true);
			_serverObjectPtr->setStartTime(TNOWMS);
            NODE_LOG("startServer")->debug() << FILE_FUN << _desc.application << "." << _desc.serverName << "_" << _desc.nodeName << " start succ " << sResult << ", use:" << (TC_TimeProvider::getInstance()->getNowMs() - startMs) << " ms" << endl;
            return 0;
        }
    }
    catch (exception& e)
    {
        sResult = e.what();
    }
    catch (const std::string& e)
    {
        sResult = e;
    }
    catch (...)
    {
        sResult = "catch unkwon exception";
    }

    NODE_LOG("startServer")->error() << FILE_FUN << _desc.application << "." << _desc.serverName << " start  failed :" << sResult << ", use:" << (TC_TimeProvider::getInstance()->getNowMs() - startMs) << " ms" << endl;
    
    _serverObjectPtr->setPid(0);
    _serverObjectPtr->setState(ServerObject::Inactive);
	_serverObjectPtr->setStarted(false);

    return -1;
}
#endif
