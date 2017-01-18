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

#ifndef __SERVER_OBJECT_H_
#define __SERVER_OBJECT_H_

#include "Node.h"
#include "Registry.h"
#include <unistd.h>
#include "Activator.h"
#include "util/tc_file.h"
#include "util/tc_config.h"
#include "servant/AdminF.h"
#include "servant/NodeF.h"
#include "servant/TarsLogger.h"
#include "PlatformInfo.h"
#include "PropertyReporter.h"
#include "ServerLimitResource.h"

using namespace tars;
using namespace std;

class ServerObject : public TC_ThreadRecLock, public TC_HandleBase
{
public:
    enum InternalServerState
    {
        Inactive,
        Activating,
        Active,
        Deactivating,
        Destroying,
        Destroyed,
        Loading,
        Patching,
        BatchPatching,
        AddFilesing,
    };

    enum EM_CoreType
    {
        EM_AUTO_LIMIT,
        EM_MANUAL_LIMIT
    };

    /**
     *@brief 服务进程的limit资源状态，比如core属性资源
     *
     *服务的corelimit初始状态默认为EM_AUTO_LIMIT+false
     */
    struct ServerLimitInfo
    {
        bool    bEnableCoreLimit;             //资源属性开关
        bool    bCloseCore;                //core属性的状态，屏蔽:true,打开:false
        EM_CoreType eCoreType;             //服务core的当前屏蔽方式
        int     iMaxExcStopCount;          //最大服务异常停止个数
        int32_t iCoreLimitTimeInterval;    //时间间隔内core的限制，单位是分钟
        int32_t iCoreLimitExpiredTime;     //core关闭的持续时间,单位为分钟
        int32_t iMonitorIntervalMs;        //keepalive监控时间间隔

        int     iActivatorMaxCount;        //最大启动次数
        int     iActivatorTimeInterval;    //时间
        int     iActivatorPunishInterval;  //惩罚受限时间间隔
        bool    bReportLoadInfo;           //是否上报节点负载信息,默认上报

        ServerLimitInfo()
        : bEnableCoreLimit(false)
        , bCloseCore(false)
        , eCoreType(EM_AUTO_LIMIT)
        , iMaxExcStopCount(3)
        , iCoreLimitTimeInterval(5)
        , iCoreLimitExpiredTime(30)
        , iMonitorIntervalMs(1000)
        , iActivatorMaxCount(10)
        , iActivatorTimeInterval(60)
        , iActivatorPunishInterval(600)
        , bReportLoadInfo(true)
        {
        }

        ostream& displaySimple(ostream& _os, int _level=0) const
        {
            tars::TarsDisplayer _ds(_os, _level);
            _ds.displaySimple(bEnableCoreLimit, true);
            _ds.displaySimple(bCloseCore, true);
            _ds.displaySimple(eCoreType, true);
            _ds.displaySimple(iMaxExcStopCount, true);
            _ds.displaySimple(iCoreLimitTimeInterval, true);
            _ds.displaySimple(iCoreLimitExpiredTime, true);
            _ds.displaySimple(iMonitorIntervalMs, true);

            _ds.displaySimple(iActivatorMaxCount, true);
            _ds.displaySimple(iActivatorTimeInterval, true);
            _ds.displaySimple(iActivatorPunishInterval, false);

            _ds.displaySimple(bReportLoadInfo, true);
            return _os;
        }

        string str() 
        {
            stringstream ss;
            displaySimple(ss);
            return ss.str();
        }
    };

public:

    /**
     * 构造函数
     * @param tDesc  server描述结构
     */
    ServerObject( const ServerDescriptor& tDesc);

    /**
     * 析够
     */
    ~ServerObject() { };

    /**
     * 是否是tars服务
     *
     * @return bool
     */
    bool isTarsServer() {return _tarsServer;}

    /**
     * 是否是enable
     *
     * @return bool
     */
    bool isEnabled() {return _enabled;}

    /**
    * 服务是否自动启动
    *,用来防止问题服务不断重启影响其它服务
    * @return  bool
    */
    bool isAutoStart() ;

    /**
     * 设置服务enable
     * @param enable
     */
    void setEnabled(bool enable){_enabled = enable; }

     /**
     * 设置服务已patch
     * @param enable
     */
    void setPatched(bool bPatched)
    {
        _patched               = bPatched;
        _patchInfo.iPercent    = 100;
    }

    /**
     *设置服务已load
     * @param enable
     */
    void setLoaded(bool bLoaded){_loaded = bLoaded;}

    /**
     *是否允许同步服务的状态到主控中心，在某些情况下，服务发布了，但是还不想对外提供服务
     *则可以设置这个值
     *@param  bEn
     */
    void setEnSynState(bool bEn) { _enSynState = bEn;}

    bool IsEnSynState(){ return _enSynState;}

public:

    /**
    * 验证server对应pid是否存在
    * @return  int
    */
    int checkPid();

    /**
    * 设置server对应pid
    */
    void setPid( pid_t pid );

    /**
    * 获取server对应pid
    */

    int getPid() { return _pid; }

    /**
    * 获取server对应本地socket
    */

    TC_Endpoint getLocalEndpoint() { return _localEndpoint; }

    /**
    * 设置server对应本地socket
    */

    void setLocalEndpoint(const TC_Endpoint &tLocalEndpoint) {  _localEndpoint = tLocalEndpoint; }

    /**
    * 获取server上报信息
    * @para string obj
    * @para pid_t pid上报pid
    * @return void
    */
    void keepAlive( pid_t pid, const string &adapter="");

    /**
    * 取的server最近keepAlive时间
    * @return int
    */
    int getLastKeepAliveTime(const string &adapter="");

    /**
    * 设置server最近keepAlive时间
    * @return void
    */
    void setLastKeepAliveTime(int t,const string &adapter="");

    /**
     * 服务是否已timeout
     * @param iTimeout
     * @return bool
     */
    bool isTimeOut(int iTimeout);

	/*
	 *	是否已经通过commandStart启动成功
	 */
	bool isStarted() {return _started;}

public:

    /**
     * 跟registry同步server当前状态
     * @return void
     */
    void synState();

    /**
     * 异步跟registry同步server当前状态
     * @return void
     */
    void asyncSynState();

    /**
     * 设置server当前状态
     * @param eState
     * @param bSynState  是否同步server状态到registry
     * @return  void
     */
    void setState( InternalServerState eState, bool bSynState=true);

    /**
    *设置server上一次状态，当server状态发生变化时调用
    */
    void setLastState(InternalServerState eState);

    /**
     * 获取服务状态
     *
     * @return tars::ServerState
     */
    tars::ServerState getState();

    /**
     * 获取server内部状态
     * @return void
     */
    InternalServerState getInternalState() { return  _state; }

    /**
    *获取server上一次内部状态
    *@return InternalServerState
    */
    InternalServerState getLastState() {return _lastState;}

    /**
    *转换指定server内部状态为字符串形式
    * @para  eState
    * @return string
    */
    string toStringState( InternalServerState eState ) const;

    /**
    *转换指定server内部状态为one::ServerState形式 consider
    * @para  eState
    * @return ServerState
    */
    ServerState toServerState( InternalServerState eState ) const;

    /**
    *监控server状态
    * @para  iTimeout 心跳超时时间
    * @return void
    */
    void checkServer(int iTimeout);

    /**
     * 属性上报单独出来
     */
    void reportMemProperty();

    /**
    *设置脚本文件
    * @para tConf 脚本文件
    * @return void
    */
    string decodeMacro(const string& value) const ;

public:

    /**
    *是否为脚本文件
    * @para sFileName 文件名
    * @return bool
    */
    bool isScriptFile(const string &sFileName);

    /**
    * 拉取脚本文件
    * @para sFileName 文件名
    * @return bool
    */
    bool getRemoteScript(const string &sFileName);

    /**
    *执行监控脚本文件
    * @para sFileName 文件名
    * @return void
    */
    void doMonScript();

public:
    /**
    *s设置下载百分比
    * @para iPercent 文件已下载百分比
    * @return bool
    */
    void setPatchPercent(const int iPercent);

    /**
    *s设置下载结果
    * @para sPatchResult
    * @return bool
    */
    void setPatchResult(const string &sPatchResult,const bool bSucc = false);

    /**
    *s设置下载版本
    * @para sPatchResult
    * @return bool
    */
    void setPatchVersion(const string &sVersion);

    /**
     * 获取正在下载的版本号
     * @return string
     */
    string getPatchVersion();

    /**
    * 获取下载信息
    * @para tPatchInfo
    * @return int
    */
    int getPatchPercent(PatchInfo &tPatchInfo);

public:
    ServerDescriptor getServerDescriptor() { return  _desc; }
    ActivatorPtr getActivator() { return  _activatorPtr; }
    string getExePath(){return _exePath;}
    string getExeFile(){return _exeFile;}
    string getConfigFile(){return _confFile;}
    string getLogPath(){return _logPath;}
    string getLibPath(){return _libPath;}
    string getServerDir(){return _serverDir;}
    string getServerId(){return _serverId;}
    string getServerType(){return _serverType;}
    string getStartScript() {return _startScript;}
    string getStopScript() {return _stopScript;}
    string getMonitorScript() {return _monitorScript;}
    string getEnv() { return _env; }
    string getRedirectPath() {return _redirectPath;}

    //java服务
    string getJvmParams() {return _jvmParams;}
    string getMainClass() {return _mainClass;}
    string getClassPath() {return _classPath;}
    string getBackupFileNames(){return _backupFiles;}

    void setServerDescriptor( const ServerDescriptor& tDesc );
    void setVersion( const string &version );
    void setExeFile(const string &sExeFile){_exeFile = sExeFile;}
    void setExePath(const string &sExePath){_exePath = sExePath;}
    void setConfigFile(const string &sConfFile){_confFile = sConfFile;}
    void setLogPath(const string &sLogPath){_logPath = sLogPath;}
    void setLibPath(const string &sLibPath){_libPath = sLibPath;}
    void setServerDir(const  string &sServerDir){_serverDir = sServerDir;}
    void setNodeInfo(const NodeInfo &tNodeInfo){_nodeInfo = tNodeInfo;}
    void setServerType( const string &sType ){ _serverType = TC_Common::lower(TC_Common::trim(sType));_serverType == "not_tars"?_tarsServer = false:_tarsServer=true;}
    void setMacro(const map<string,string>& mMacro);
    void setScript(const string &sStartScript,const string &sStopScript,const string &sMonitorScript);

    void setEnv(const string & sEnv) { _env = sEnv; }
    void setHeartTimeout(int iTimeout) { _timeout = iTimeout; }

    //java服务
    void setJvmParams(const string &sJvmParams){_jvmParams = sJvmParams;}
    void setMainClass(const string &sMainClass){_mainClass = TC_Common::trim(sMainClass);}
    void setClassPath(const  string &sClassPath){_classPath = sClassPath;}
    void setRedirectPath(const string& sRedirectpath) {_redirectPath = sRedirectpath;}
    void setBackupFileNames(const string& sFileNames){_backupFiles = sFileNames;}

    //重置core计数信息
    void resetCoreInfo();

	//是否已经通过commandStart启动成功
	void setStarted(bool bStarted);

	//设置启动的时间,作为checkpid系统延迟判断的起点
	void setStartTime(int64_t iStartTime);

	//判断是否启动超时
	bool isStartTimeOut();
public:
    /**
     * auto check routine
     */
    void checkCoredumpLimit();

    /**
     * server restart and set limitupdate state
     */
    void setLimitInfoUpdate(bool bUpdate);

    void setServerLimitInfo(const ServerLimitInfo& tInfo);

    bool setServerCoreLimit(bool bCloseCore);

private:
    bool    _tarsServer;                //是否tars服务
    string  _serverType;               //服务类型  tars_cpp tars_java not_tars

private:
    bool    _enabled;                  //服务是否有效
    bool    _loaded;                   //服务配置是否已成功加载
    bool    _patched;                  //服务可执行程序是否已成功下载
    bool    _noticed;                  //服务当前状态是否已通知registry。
    unsigned _noticeFailTimes;         //同步服务状态到registry连续失败次数
    bool    _enSynState;               //是否允许同步服务的状态到主控

private:
    string  _application;              //服务所属app
    string  _serverName;               //服务名称
    string  _serverId;                 //服务id为App.ServerName.Ip形式

private:
    string _jvmParams;                 //java服务jvm
    string _mainClass;                 //java服务main class
    string _classPath;                 //java服务 class path
    string _redirectPath;               //标准输出和错误输出重定向目录

private:
    string _startScript;               //启动脚本
    string _stopScript;                //停止脚本
    string _monitorScript;             //监控脚本

private:
    string _serverDir;                 //服务数据目录
    string _confFile;                  //服务配置文件目录
    string _exePath;                   //一般为_serverDir+"/bin" 可个性指定
    string _exeFile;                   //一般为_exePath+_serverName 可个性指定
    string _logPath;                   //服务日志目录
    string _libPath;                   //动态库目录 一般为_desc.basePath/lib
    map<string,string> _macro;         //服务宏

private:
    PatchInfo           _patchInfo;            //下载信息

private:
    pid_t               _pid;                  //服务进程号
    string              _version;              //TARS版本
    NodeInfo            _nodeInfo;             //服务所在node信息
    TC_Endpoint         _localEndpoint;        //本地socket
    ServerDescriptor    _desc;                 //服务描述，主要用于生成配置文件等
    ActivatorPtr        _activatorPtr;         //用于启动、停止服务
    time_t              _keepAliveTime;        //服务最近上报时间
    map<string,time_t>  _adapterKeepAliveTime; //各adapter最近上报时间，用来判断adapter是否上报超时

    InternalServerState _state;                //当前服务状态
    InternalServerState _lastState;            //上一次服务状态

    int                 _timeout;              //心跳超时时间
    string              _env;                  //环境变量字符串
    string                 _backupFiles;          //针对java服务发布时bin目录下需要保留的文件；可以用;|来分隔

private:
    bool                 _limitStateUpdated;    //服务的limit配置是否有更新，重启也算更新
    ServerLimitInfo      _limitStateInfo;       //通过node配置设置的资源信息
    ServerLimitResourcePtr _serviceLimitResource;
    bool				 _started;				//是否已经通过commandStart启动成功
	int64_t              _startTime;			//启动的时间,作为checkpid系统延迟判断的起点
};

typedef TC_AutoPtr<ServerObject> ServerObjectPtr;

#endif

