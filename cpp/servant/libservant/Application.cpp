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

#include "util/tc_option.h"
#include "util/tc_common.h"
#include "servant/TarsNodeF.h"
#include "servant/Application.h"
#include "servant/AppProtocol.h"
#include "servant/AdminServant.h"
#include "servant/ServantHandle.h"
#include "servant/BaseF.h"
#include "servant/AppCache.h"
#include "servant/NotifyObserver.h"
#include "servant/AuthLogic.h"

#include <signal.h>
#include <sys/resource.h>

#if TARS_SSL
#include "util/tc_sslmgr.h"
#endif


namespace tars
{

static void sighandler( int sig_no )
{
    Application::terminate();
}

std::string ServerConfig::Application;      //应用名称
std::string ServerConfig::ServerName;       //服务名称,一个服务名称含一个或多个服务标识
std::string ServerConfig::LocalIp;          //本机IP
std::string ServerConfig::BasePath;         //应用程序路径，用于保存远程系统配置的本地目录
std::string ServerConfig::DataPath;         //应用程序路径，用于本地数据
std::string ServerConfig::Local;            //本地套接字
std::string ServerConfig::Node;             //本机node地址
std::string ServerConfig::Log;              //日志中心地址
std::string ServerConfig::Config;           //配置中心地址
std::string ServerConfig::Notify;           //信息通知中心
std::string ServerConfig::LogPath;          //logpath
int         ServerConfig::LogSize;          //log大小(字节)
int         ServerConfig::LogNum;           //log个数()
std::string ServerConfig::LogLevel;            //log日志级别
std::string ServerConfig::ConfigFile;       //框架配置文件路径
int         ServerConfig::ReportFlow;       //是否服务端上报所有接口stat流量 0不上报 1上报 (用于非tars协议服务流量统计)
int         ServerConfig::IsCheckSet;       //是否对按照set规则调用进行合法性检查 0,不检查，1检查
bool        ServerConfig::OpenCoroutine;    //是否启用协程处理方式
size_t      ServerConfig::CoroutineMemSize; //协程占用内存空间的最大大小
uint32_t    ServerConfig::CoroutineStackSize;   //每个协程的栈大小(默认128k)

static string outfill(const string& s, char c = ' ', int n = 29)
{
    return (s + string(abs(n - (int)s.length()), c));
}

#define OUT_LINE        (outfill("", '-', 50))
#define OUT_LINE_LONG   (outfill("", '=', 50))

///////////////////////////////////////////////////////////////////////////////////////////
TC_Config                       Application::_conf;
TC_EpollServerPtr               Application::_epollServer  = NULL;
CommunicatorPtr                 Application::_communicator = NULL;

///////////////////////////////////////////////////////////////////////////////////////////
Application::Application()
{
}

Application::~Application()
{
    terminate();
}

TC_Config& Application::getConfig()
{
    return _conf;
}

TC_EpollServerPtr& Application::getEpollServer()
{
    return _epollServer;
}

CommunicatorPtr& Application::getCommunicator()
{
    return _communicator;
}

void Application::waitForQuit()
{
    int64_t iLastCheckTime = TNOW;
    int64_t iNow = iLastCheckTime;

    unsigned int iNetThreadNum = _epollServer->getNetThreadNum();
    vector<TC_EpollServer::NetThread*> vNetThread = _epollServer->getNetThread();

    for (size_t i = 0; i < iNetThreadNum; ++i)
    {
        vNetThread[i]->start();
    }

    _epollServer->debug("server netthread num : " + TC_Common::tostr(iNetThreadNum));

    while(!_epollServer->isTerminate())
    {
        {
            TC_ThreadLock::Lock sync(*_epollServer);
            _epollServer->timedWait(5000);
        }

        iNow = TNOW;

        if(iNow - iLastCheckTime > REPORT_SEND_QUEUE_INTERVAL)
        {
            iLastCheckTime = iNow;

            size_t n = 0;
            for(size_t i = 0;i < iNetThreadNum; ++i)
            {
                n = n + vNetThread[i]->getSendRspSize();
            }

            if(_epollServer->_pReportRspQueue)
            {
                _epollServer->_pReportRspQueue->report(n);
            }
        }
    }

    if(_epollServer->isTerminate())
    {
        for(size_t i = 0; i < iNetThreadNum; ++i)
        {
            vNetThread[i]->terminate();
            vNetThread[i]->getThreadControl().join();
        }

        _epollServer->stopThread();
    }
}

void Application::waitForShutdown()
{
    waitForQuit();

    destroyApp();

    TarsRemoteNotify::getInstance()->report("stop", true);
}

void Application::terminate()
{
    if(_epollServer)
    {
        _epollServer->terminate();
    }
}

bool Application::cmdViewStatus(const string& command, const string& params, string& result)
{
    TLOGINFO("Application::cmdViewStatus:" << command << " " << params << endl);

    ostringstream os;

    os << OUT_LINE_LONG << endl;

    os << outfill("[proxy config]:") << endl;

    outClient(os);

    os << OUT_LINE << "\n" << outfill("[server config]:") << endl;

    outServer(os);

    os << OUT_LINE << endl;

    outAllAdapter(os);

    result = os.str();

    return true;
}
bool Application::cmdCloseCoreDump(const string& command, const string& params, string& result)
{
    struct rlimit tlimit;
    int ret=0;
    ostringstream os;

    ret = getrlimit(RLIMIT_CORE,&tlimit);
    if(ret != 0)
    {
        TLOGERROR("error: "<<strerror(errno)<<endl);
        return false;
    }

    TLOGDEBUG("before :cur:"<<tlimit.rlim_cur<<";max: "<<tlimit.rlim_max<<endl);

    os<<(ServerConfig::Application+"."+ServerConfig::ServerName);

    os<<"|before set:cur:"<<tlimit.rlim_cur<<";max: "<<tlimit.rlim_max;

    string param = TC_Common::lower(TC_Common::trim(params));

    bool bClose = (param == "yes")?true:false;

    if(bClose)
    {
        tlimit.rlim_cur = 0;
    }
    else
    {
        tlimit.rlim_cur = tlimit.rlim_max;
    }


    ret =setrlimit(RLIMIT_CORE,&tlimit);
    if(ret != 0)
    {
        TLOGERROR("error: "<<strerror(errno)<<endl);
       return false;
    }

    ret = getrlimit(RLIMIT_CORE,&tlimit);
    if(ret != 0)
    {
        TLOGERROR("error: "<<strerror(errno)<<endl);
        return false;
    }

    TLOGDEBUG("after cur:"<<tlimit.rlim_cur<<";max: "<<tlimit.rlim_max<<endl);
    os <<"|after set cur:"<<tlimit.rlim_cur<<";max: "<<tlimit.rlim_max<<endl;

    result = os.str();

    return true;
}
bool Application::cmdSetLogLevel(const string& command, const string& params, string& result)
{
    TLOGINFO("Application::cmdSetLogLevel:" << command << " " << params << endl);

    string level = TC_Common::trim(params);

    int ret = TarsRollLogger::getInstance()->logger()->setLogLevel(level);

    if(ret == 0)
    {
        ServerConfig::LogLevel = TC_Common::upper(level);

        result = "set log level [" + level + "] ok";

        AppCache::getInstance()->set("logLevel",level);
    }
    else
    {
        result = "set log level [" + level + "] error";
    }

    return true;
}

bool Application::cmdEnableDayLog(const string& command, const string& params, string& result)
{
    TLOGINFO("Application::cmdEnableDayLog:" << command << " " << params << endl);

    vector<string> vParams = TC_Common::sepstr<string>(TC_Common::trim(params),"|");

    size_t nNum = vParams.size();

    if(!(nNum == 2 || nNum == 3))
    {
        result = "usage: tars.enabledaylog {remote|local}|[logname]|{true|false}";
        return false;
    }

    if((vParams[0] != "local" && vParams[0] != "remote"))
    {
        result = "usage: tars.enabledaylog {remote|local}|[logname]|{true|false}";
        return false;
    }

    if(nNum == 2 && (vParams[1] != "true" && vParams[1] != "false"))
    {
        result = "usage: tars.enabledaylog {remote|local}|[logname]|{true|false}";
        return false;
    }

    if(nNum == 3 && (vParams[2] != "true" && vParams[2] != "false"))
    {
        result = "usage: tars.enabledaylog {remote|local}|[logname]|{true|false}";
        return false;
    }

    bool bEnable = true;
    string sFile;


    if(nNum == 2)
    {
        bEnable = (vParams[1] == "true")?true:false;
        sFile = "";
        result = "set " + vParams[0] + " " + vParams[1] + " ok";
    }
    else if(nNum == 3)
    {
        bEnable = (vParams[2] == "true")?true:false;
        sFile = vParams[1];
        result = "set " + vParams[0] + " " + vParams[1] + " "+vParams[2] + " ok";
    }


    if(vParams[0] == "local")
    {
        TarsTimeLogger::getInstance()->enableLocal(sFile,bEnable);
        return true;
    }

    if(vParams[0] == "remote")
    {
        TarsTimeLogger::getInstance()->enableRemote(sFile,bEnable);
        return true;
    }

    result = "usage: tars.enabledaylog {remote|local}|[logname]|{true|false}";
    return false;

}

bool Application::cmdLoadConfig(const string& command, const string& params, string& result)
{
    TLOGINFO("Application::cmdLoadConfig:" << command << " " << params << endl);

    string filename = TC_Common::trim(params);

    if (TarsRemoteConfig::getInstance()->addConfig(filename, result,false))
    {
        TarsRemoteNotify::getInstance()->report(result);

        return true;
    }

    TarsRemoteNotify::getInstance()->report(result);

    return true;
}

bool Application::cmdConnections(const string& command, const string& params, string& result)
{
    TLOGINFO("Application::cmdConnections:" << command << " " << params << endl);

    ostringstream os;

    os << OUT_LINE_LONG << endl;

    map<int, TC_EpollServer::BindAdapterPtr> m = _epollServer->getListenSocketInfo();

    for(map<int, TC_EpollServer::BindAdapterPtr>::const_iterator it = m.begin(); it != m.end(); ++it)
    {
        vector<TC_EpollServer::ConnStatus> v = it->second->getConnStatus();

        os << OUT_LINE << "\n" << outfill("[adater:" + it->second->getName() + "] [connections:" + TC_Common::tostr(v.size())+ "]") << endl;

        os  << outfill("conn-uid", ' ', 15)
            << outfill("ip:port", ' ', 25)
            << outfill("last-time", ' ', 25)
            << outfill("timeout", ' ', 10) << endl;

        for(size_t i = 0; i < v.size(); i++)
        {
            os  << outfill(TC_Common::tostr<uint32_t>(v[i].uid), ' ', 15)
                << outfill(v[i].ip + ":" + TC_Common::tostr(v[i].port), ' ', 25)
                << outfill(TC_Common::tm2str(v[i].iLastRefreshTime,"%Y-%m-%d %H:%M:%S"), ' ', 25)
                << outfill(TC_Common::tostr(v[i].timeout), ' ', 10) << endl;
        }
    }
    os << OUT_LINE_LONG << endl;

    result = os.str();

    return true;
}

bool Application::cmdViewVersion(const string& command, const string& params, string& result)
{
    result = "$" + string(TARS_VERSION) + "$";
    return true;
}

bool Application::cmdLoadProperty(const string& command, const string& params, string& result)
{
    try
    {
        TLOGINFO("Application::cmdLoadProperty:" << command << " " << params << endl);

        //重新解析配置文件
        _conf.parseFile(ServerConfig::ConfigFile);

        string sResult = "";

        //加载通讯器属性
        _communicator->setProperty(_conf);

        _communicator->reloadProperty(sResult);

        //加载远程对象
        ServerConfig::Log = _conf.get("/tars/application/server<log>");

        TarsTimeLogger::getInstance()->setLogInfo(_communicator, ServerConfig::Log, ServerConfig::Application, ServerConfig::ServerName, ServerConfig::LogPath,setDivision());

        ServerConfig::Config = _conf.get("/tars/application/server<config>");

        TarsRemoteConfig::getInstance()->setConfigInfo(_communicator, ServerConfig::Config, ServerConfig::Application, ServerConfig::ServerName, ServerConfig::BasePath,setDivision());

        ServerConfig::Notify = _conf.get("/tars/application/server<notify>");

        TarsRemoteNotify::getInstance()->setNotifyInfo(_communicator, ServerConfig::Notify, ServerConfig::Application, ServerConfig::ServerName, setDivision());

        result = "loaded config items:\r\n" + sResult +
                 "log=" + ServerConfig::Log + "\r\n" +
                 "config=" + ServerConfig::Config + "\r\n" +
                 "notify=" + ServerConfig::Notify + "\r\n";
    }
    catch (TC_Config_Exception & ex)
    {
        result = "load config " + ServerConfig::ConfigFile + " error:" + ex.what();
    }
    catch (exception &ex)
    {
        result = ex.what();
    }
    return true;
}

bool Application::cmdViewAdminCommands(const string& command, const string& params, string& result)
{
    TLOGINFO("Application::cmdViewAdminCommands:" << command << " " << params << endl);

    result =result +  NotifyObserver::getInstance()->viewRegisterCommand();

    return true;
}

bool Application::cmdSetDyeing(const string& command, const string& params, string& result)
{
    vector<string> vDyeingParams = TC_Common::sepstr<string>(params, " ");

    if(vDyeingParams.size() == 2 || vDyeingParams.size() == 3)
    {
        ServantHelperManager::getInstance()->setDyeing(vDyeingParams[0], vDyeingParams[1], vDyeingParams.size() == 3 ? vDyeingParams[2] : "");

        result = "DyeingKey="       + vDyeingParams[0] + "\r\n" +
                 "DyeingServant="   + vDyeingParams[1] + "\r\n" +
                 "DyeingInterface=" + (vDyeingParams.size() == 3 ? vDyeingParams[2] : "") + "\r\n";
    }
    else
    {
        result = "Invalid parameters.Should be: dyeingKey dyeingServant [dyeingInterface]";
    }
    return true;
}

bool Application::cmdCloseCout(const string& command, const string& params, string& result)
{
    TLOGINFO("Application::cmdCloseCout:" << command << " " << params << endl);

    string s = TC_Common::lower(TC_Common::trim(params));

    if(s == "yes")
    {
        AppCache::getInstance()->set("closeCout","1");
    }
    else
    {
        AppCache::getInstance()->set("closeCout","0");
    }

    result = "set closeCout  [" + s + "] ok";

    return true;
}

bool Application::cmdReloadLocator(const string& command, const string& params, string& result)
{
    TLOGINFO("Application::cmdReloadLocator:" << command << " " << params << endl);

    string sPara = TC_Common::lower(TC_Common::trim(params));

    bool bSucc(true);
    if (sPara == "reload")
    {
        TC_Config reloadConf;

        reloadConf.parseFile(ServerConfig::ConfigFile);
        string sLocator = reloadConf.get("/tars/application/client/<locator>", "");

        TLOGINFO(__FUNCTION__ << "|" << __LINE__ << "|conf file:" << ServerConfig::ConfigFile << "\n"
            << "|sLocator:" << sLocator << endl);

        if (sLocator.empty())
        {
            bSucc = false;
            result = "locator info is null.";
        }
        else
        {
            _communicator->setProperty("locator", sLocator);
            _communicator->reloadLocator();
            result = sLocator + " set succ.";
        }

    }
    else
    {
        result = "please input right paras.";
        bSucc = false;
    }

    return bSucc;
}

void Application::outAllAdapter(ostream &os)
{
    map<int, TC_EpollServer::BindAdapterPtr> m = _epollServer->getListenSocketInfo();

    for(map<int, TC_EpollServer::BindAdapterPtr>::const_iterator it = m.begin(); it != m.end(); ++it)
    {
        outAdapter(os, ServantHelperManager::getInstance()->getAdapterServant(it->second->getName()),it->second);

        os << OUT_LINE << endl;
    }
}

bool Application::addConfig(const string &filename)
{
    string result;

    if (TarsRemoteConfig::getInstance()->addConfig(filename, result, false))
    {
        TarsRemoteNotify::getInstance()->report(result);

        return true;
    }
    TarsRemoteNotify::getInstance()->report(result);

    return true;
}

bool Application::addAppConfig(const string &filename)
{
    string result = "";

    // true-只获取应用级别配置
    if (TarsRemoteConfig::getInstance()->addConfig(filename, result, true))

    {
        TarsRemoteNotify::getInstance()->report(result);

        return true;
    }

    TarsRemoteNotify::getInstance()->report(result);

    return true;
}

void Application::setHandle(TC_EpollServer::BindAdapterPtr& adapter)
{
    adapter->setHandle<ServantHandle>();
}

void Application::main(int argc, char *argv[])
{
    try
    {
#if TARS_SSL
        SSLManager::GlobalInit();
#endif
        TC_Common::ignorePipe();

        //解析配置文件
        parseConfig(argc, argv);

        //初始化Proxy部分
        initializeClient();

        //初始化Server部分
        initializeServer();

        vector<TC_EpollServer::BindAdapterPtr> adapters;

        //绑定对象和端口
        bindAdapter(adapters);

        //业务应用的初始化
        initialize();

        //输出所有adapter
        outAllAdapter(cout);

        //设置HandleGroup分组，启动线程
        for (size_t i = 0; i < adapters.size(); ++i)
        {
            string name = adapters[i]->getName();

            string groupName = adapters[i]->getHandleGroupName();

            if(name != groupName)
            {
                TC_EpollServer::BindAdapterPtr ptr = _epollServer->getBindAdapter(groupName);

                if (!ptr)
                {
                    throw runtime_error("[TARS][adater `" + name + "` setHandle to group `" + groupName + "` fail!");
                }

            }
            setHandle(adapters[i]);
        }

        //启动业务处理线程
        _epollServer->startHandle();
        _epollServer->createEpoll();

        cout << "\n" << outfill("[initialize server] ", '.')  << " [Done]" << endl;

        cout << OUT_LINE_LONG << endl;

        //动态加载配置文件
        TARS_ADD_ADMIN_CMD_PREFIX(TARS_CMD_LOAD_CONFIG, Application::cmdLoadConfig);

        //动态设置滚动日志等级
        TARS_ADD_ADMIN_CMD_PREFIX(TARS_CMD_SET_LOG_LEVEL, Application::cmdSetLogLevel);

        //动态设置按天日志等级
        TARS_ADD_ADMIN_CMD_PREFIX(TARS_CMD_SET_DAYLOG_LEVEL, Application::cmdEnableDayLog);

        //查看服务状态
        TARS_ADD_ADMIN_CMD_PREFIX(TARS_CMD_VIEW_STATUS, Application::cmdViewStatus);

        //查看当前链接状态
        TARS_ADD_ADMIN_CMD_PREFIX(TARS_CMD_CONNECTIONS, Application::cmdConnections);

        //查看编译的TARS版本
        TARS_ADD_ADMIN_CMD_PREFIX(TARS_CMD_VIEW_VERSION, Application::cmdViewVersion);

        //加载配置文件中的属性信息
        TARS_ADD_ADMIN_CMD_PREFIX(TARS_CMD_LOAD_PROPERTY, Application::cmdLoadProperty);

        //查看服务支持的管理命令
        TARS_ADD_ADMIN_CMD_PREFIX(TARS_CMD_VIEW_ADMIN_COMMANDS, Application::cmdViewAdminCommands);

        //设置染色信息
        TARS_ADD_ADMIN_CMD_PREFIX(TARS_CMD_SET_DYEING, Application::cmdSetDyeing);

        //设置服务的core limit
        TARS_ADD_ADMIN_CMD_PREFIX(TARS_CMD_CLOSE_CORE, Application::cmdCloseCoreDump);

        //重新加载locator信息
        TARS_ADD_ADMIN_CMD_PREFIX(TARS_CMD_RELOAD_LOCATOR, Application::cmdReloadLocator);

        //上报版本
        TARS_REPORTVERSION(TARS_VERSION);

        //发送心跳给node, 表示启动了
        TARS_KEEPALIVE("");

        //发送给notify表示服务启动了
        TarsRemoteNotify::getInstance()->report("restart");

        //ctrl + c能够完美结束服务
        signal(SIGINT, sighandler);


        if(_conf.get("/tars/application/server<closecout>",AppCache::getInstance()->get("closeCout")) != "0")
        {
            // 重定向stdin、stdout、stderr
            int fd = open("/dev/null", O_RDWR );
            if(fd != -1)
            {
               dup2(fd, 0);
               dup2(fd, 1);
               dup2(fd, 2);
            }
            else
            {
               close(0);
               close(1);
               close(2);
            }
        }
    }
    catch (exception &ex)
    {
        TarsRemoteNotify::getInstance()->report("exit: " + string(ex.what()));

        cout << "[main exception]:" << ex.what() << endl;

        terminate();
    }

    //初始化完毕后, 日志再修改为异步
    TarsRollLogger::getInstance()->sync(false);
}

void Application::parseConfig(int argc, char *argv[])
{
    TC_Option op;

    op.decode(argc, argv);

    //直接输出编译的TARS版本
    if(op.hasParam("version"))
    {
        cout << "TARS:" << TARS_VERSION << endl;
        exit(0);
    }

    //加载配置文件
    ServerConfig::ConfigFile = op.getValue("config");

    if(ServerConfig::ConfigFile == "")
    {
        cerr << "start server with config, for example: " << argv[0] << " --config=config.conf" << endl;

        exit(0);
    }

    _conf.parseFile(ServerConfig::ConfigFile);
}

TC_EpollServer::BindAdapter::EOrder Application::parseOrder(const string &s)
{
    vector<string> vtOrder = TC_Common::sepstr<string>(s,";, \t", false);

    if(vtOrder.size() != 2)
    {
        cerr << "invalid order '" << TC_Common::tostr(vtOrder) << "'."<< endl;

        exit(0);
    }
    if((TC_Common::lower(vtOrder[0]) == "allow")&&(TC_Common::lower(vtOrder[1]) == "deny"))
    {
        return TC_EpollServer::BindAdapter::ALLOW_DENY;
    }
    if((TC_Common::lower(vtOrder[0]) == "deny")&&(TC_Common::lower(vtOrder[1]) == "allow"))
    {
         return TC_EpollServer::BindAdapter::DENY_ALLOW;
    }

     cerr << "invalid order '" << TC_Common::tostr(vtOrder) << "'."<< endl;

     exit(0);
}

void Application::initializeClient()
{
    cout << "\n" << OUT_LINE_LONG << endl;

    //初始化通信器
    _communicator = CommunicatorFactory::getInstance()->getCommunicator(_conf);

    cout << outfill("[proxy config]:") << endl;

    //输出
    outClient(cout);
#if TARS_SSL
    try {
        string path = _conf.get("/tars/application/clientssl/<path>", "./");
        if (path.empty() || path[path.length() - 1] != '/')
            path += "/";

        string ca = path + _conf.get("/tars/application/clientssl/<ca>");
        string cert = path + _conf.get("/tars/application/clientssl/<cert>");
        if (cert == path) cert.clear();
        string key = path + _conf.get("/tars/application/clientssl/<key>");
        if (key == path) key.clear();

        if (!SSLManager::getInstance()->AddCtx("client", ca, cert, key, false))
            cout << "failed add client cert " << ca << endl;
        else
            cout << "succ add client cert " << ca << endl;
    }
    catch(...) {
    }
#endif
}

void Application::outClient(ostream &os)
{
    os << outfill("locator")                     << _communicator->getProperty("locator") << endl;
    os << outfill("sync-invoke-timeout")         << _communicator->getProperty("sync-invoke-timeout") << endl;
    os << outfill("async-invoke-timeout")        << _communicator->getProperty("async-invoke-timeout") << endl;
    os << outfill("refresh-endpoint-interval")   << _communicator->getProperty("refresh-endpoint-interval") << endl;
    os << outfill("stat")                        << _communicator->getProperty("stat") << endl;
    os << outfill("property")                    << _communicator->getProperty("property") << endl;
    os << outfill("report-interval")             << _communicator->getProperty("report-interval") << endl;
    os << outfill("sample-rate")                 << _communicator->getProperty("sample-rate") << endl;
    os << outfill("max-sample-count")            << _communicator->getProperty("max-sample-count") << endl;
    os << outfill("netthread")                  << _communicator->getProperty("netthread") << endl;
    os << outfill("recvthread")                  << _communicator->getProperty("recvthread") << endl;
    os << outfill("asyncthread")                 << _communicator->getProperty("asyncthread") << endl;
    os << outfill("modulename")                  << _communicator->getProperty("modulename") << endl;
    os << outfill("enableset")                     << _communicator->getProperty("enableset") << endl;
    os << outfill("setdivision")                 << _communicator->getProperty("setdivision") << endl;
}

string Application::toDefault(const string &s, const string &sDefault)
{
    if(s.empty())
    {
        return sDefault;
    }
    return s;
}
string Application::setDivision()
{
    bool bEnableSet = TC_Common::lower(_conf.get("/tars/application<enableset>", "n"))=="y"?true:false;;

    string sSetDevision = bEnableSet?_conf.get("/tars/application<setdivision>", ""):"";
    return sSetDevision;
}

void Application::addServantProtocol(const string& servant, const TC_EpollServer::protocol_functor& protocol)
{
    string adapterName = ServantHelperManager::getInstance()->getServantAdapter(servant);

    if (adapterName == "")
    {
        throw runtime_error("[TARS]addServantProtocol fail, no found adapter for servant:" + servant);
    }
    getEpollServer()->getBindAdapter(adapterName)->setProtocol(protocol);
}

void Application::initializeServer()
{
    cout << OUT_LINE << "\n" << outfill("[server config]:") << endl;

    ServerConfig::Application  = toDefault(_conf.get("/tars/application/server<app>"), "UNKNOWN");

    //缺省采用进程名称
    string exe = "";

    try
    {
        exe = TC_File::extractFileName(TC_File::getExePath());
    }
    catch(TC_File_Exception & ex)
    {
        //取失败则使用ip代替进程名
        exe = _conf.get("/tars/application/server<localip>");
    }

    ServerConfig::ServerName        = toDefault(_conf.get("/tars/application/server<server>"), exe);
    ServerConfig::BasePath          = toDefault(_conf.get("/tars/application/server<basepath>"), ".") + "/";
    ServerConfig::DataPath          = toDefault(_conf.get("/tars/application/server<datapath>"), ".") + "/";
    ServerConfig::LogPath           = toDefault(_conf.get("/tars/application/server<logpath>"),  ".") + "/";
    ServerConfig::LogSize           = TC_Common::toSize(toDefault(_conf.get("/tars/application/server<logsize>"), "52428800"), 52428800);
    ServerConfig::LogNum            = TC_Common::strto<int>(toDefault(_conf.get("/tars/application/server<lognum>"), "10"));
    ServerConfig::LocalIp           = _conf.get("/tars/application/server<localip>");
    ServerConfig::Local             = _conf.get("/tars/application/server<local>");
    ServerConfig::Node              = _conf.get("/tars/application/server<node>");
    ServerConfig::Log               = _conf.get("/tars/application/server<log>");
    ServerConfig::Config            = _conf.get("/tars/application/server<config>");
    ServerConfig::Notify            = _conf.get("/tars/application/server<notify>");
    ServerConfig::ReportFlow        = _conf.get("/tars/application/server<reportflow>")=="0"?0:1;
    ServerConfig::IsCheckSet        = _conf.get("/tars/application/server<checkset>","1")=="0"?0:1;
    ServerConfig::OpenCoroutine        = TC_Common::strto<bool>(toDefault(_conf.get("/tars/application/server<opencoroutine>"), "0"));
    ServerConfig::CoroutineMemSize    =  TC_Common::toSize(toDefault(_conf.get("/tars/application/server<coroutinememsize>"), "1073741824"), 1073741824);
    ServerConfig::CoroutineStackSize    = TC_Common::toSize(toDefault(_conf.get("/tars/application/server<coroutinestack>"), "131072"), 131072);

    if(ServerConfig::LocalIp.empty())
    {
        vector<string> v = TC_Socket::getLocalHosts();

        ServerConfig::LocalIp = "127.0.0.1";
        //获取第一个非127.0.0.1的IP
        for(size_t i = 0; i < v.size(); i++)
        {
            if(v[i] != "127.0.0.1")
            {
                ServerConfig::LocalIp = v[i];
                break;
            }
        }
    }

    //输出信息
    outServer(cout);

    string sNetThread = _conf.get("/tars/application/server<netthread>", "1");
    unsigned int iNetThreadNum = TC_Common::strto<unsigned int>(sNetThread);

    if(iNetThreadNum < 1)
    {    
        iNetThreadNum = 1;
        cout << OUT_LINE << "\nwarning:netThreadNum < 1." << endl;
    }

    //网络线程的配置数目不能15个
    if(iNetThreadNum > 15)
    {    
        iNetThreadNum = 15;
        cout << OUT_LINE << "\nwarning:netThreadNum > 15." << endl;
    }

    _epollServer = new TC_EpollServer(iNetThreadNum);

	//网络线程的内存池配置
    {
        size_t minBlockSize = TC_Common::strto<size_t>(toDefault(_conf.get("/tars/application/server<poolminblocksize>"), "1024")); // 1KB
        size_t maxBlockSize = TC_Common::strto<size_t>(toDefault(_conf.get("/tars/application/server<poolmaxblocksize>"), "8388608")); // 8MB
        size_t maxBytes = TC_Common::strto<size_t>(toDefault(_conf.get("/tars/application/server<poolmaxbytes>"), "67108864")); // 64MB
        _epollServer->setNetThreadBufferPoolInfo(minBlockSize, maxBlockSize, maxBytes);
    }


    //初始化服务是否对空链接进行超时检查
    bool bEnable = (_conf.get("/tars/application/server<emptyconcheck>","0")=="1")?true:false;

    _epollServer->EnAntiEmptyConnAttack(bEnable);
    _epollServer->setEmptyConnTimeout(TC_Common::strto<int>(toDefault(_conf.get("/tars/application/server<emptyconntimeout>"), "3")));

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //初始化本地文件cache
    cout << OUT_LINE << "\n" << outfill("[set file cache ]") << "OK" << endl;
    AppCache::getInstance()->setCacheInfo(ServerConfig::DataPath+ServerConfig::ServerName+".tarsdat",0);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //初始化本地Log
    cout << OUT_LINE << "\n" << outfill("[set roll logger] ") << "OK" << endl;
    TarsRollLogger::getInstance()->setLogInfo(ServerConfig::Application, ServerConfig::ServerName, ServerConfig::LogPath, ServerConfig::LogSize, ServerConfig::LogNum, _communicator, ServerConfig::Log);
    _epollServer->setLocalLogger(TarsRollLogger::getInstance()->logger());

    //初始化是日志为同步
    TarsRollLogger::getInstance()->sync(true);

    //设置日志级别
    string level = AppCache::getInstance()->get("logLevel");
    if(level.empty())
    {
        level = _conf.get("/tars/application/server<logLevel>","DEBUG");
    }

    TarsRollLogger::getInstance()->logger()->setLogLevel(TC_Common::upper(level));

    ServerConfig::LogLevel = TC_Common::upper(level);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //初始化到LogServer代理
    cout << OUT_LINE << "\n" << outfill("[set time logger] ") << "OK" << endl;
    bool bLogStatReport = (_conf.get("/tars/application/server<logstatreport>", "0") == "1") ? true : false;
    TarsTimeLogger::getInstance()->setLogInfo(_communicator, ServerConfig::Log, ServerConfig::Application, ServerConfig::ServerName, ServerConfig::LogPath, setDivision(), bLogStatReport);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //初始化到配置中心代理
    cout << OUT_LINE << "\n" << outfill("[set remote config] ") << "OK" << endl;
    TarsRemoteConfig::getInstance()->setConfigInfo(_communicator, ServerConfig::Config, ServerConfig::Application, ServerConfig::ServerName, ServerConfig::BasePath,setDivision());

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //初始化到信息中心代理
    cout << OUT_LINE << "\n" << outfill("[set remote notify] ") << "OK" << endl;
    TarsRemoteNotify::getInstance()->setNotifyInfo(_communicator, ServerConfig::Notify, ServerConfig::Application, ServerConfig::ServerName, setDivision());

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //初始化到Node的代理
    cout << OUT_LINE << "\n" << outfill("[set node proxy]") << "OK" << endl;
    TarsNodeFHelper::getInstance()->setNodeInfo(_communicator, ServerConfig::Node, ServerConfig::Application, ServerConfig::ServerName);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //初始化管理对象
    cout << OUT_LINE << "\n" << outfill("[set admin adapter]") << "OK" << endl;

    if(!ServerConfig::Local.empty())
    {
        ServantHelperManager::getInstance()->addServant<AdminServant>("AdminObj");

        ServantHelperManager::getInstance()->setAdapterServant("AdminAdapter", "AdminObj");

        TC_EpollServer::BindAdapterPtr lsPtr = new TC_EpollServer::BindAdapter(_epollServer.get());

        lsPtr->setName("AdminAdapter");

        lsPtr->setEndpoint(ServerConfig::Local);

        lsPtr->setMaxConns(TC_EpollServer::BindAdapter::DEFAULT_MAX_CONN);

        lsPtr->setQueueCapacity(TC_EpollServer::BindAdapter::DEFAULT_QUEUE_CAP);

        lsPtr->setQueueTimeout(TC_EpollServer::BindAdapter::DEFAULT_QUEUE_TIMEOUT);

        lsPtr->setProtocolName("tars");

        lsPtr->setProtocol(AppProtocol::parse);

        lsPtr->setHandleGroupName("AdminAdapter");

        lsPtr->setHandleNum(1);

        lsPtr->setHandle<ServantHandle>();

        _epollServer->bind(lsPtr);
    }

    //队列取平均值
    if(!_communicator->getProperty("property").empty())
    {
        string sRspQueue("");
        sRspQueue += ServerConfig::Application;
        sRspQueue += ".";
        sRspQueue += ServerConfig::ServerName;
        sRspQueue += ".sendrspqueue";

        PropertyReportPtr p;
        p = _communicator->getStatReport()->createPropertyReport(sRspQueue, PropertyReport::avg());

        _epollServer->_pReportRspQueue = p.get();
    }

#if TARS_SSL
    try {
        string path = _conf.get("/tars/application/serverssl/<path>", "./");
        if (path.empty() || path[path.length() - 1] != '/')
            path += "/";

        string ca = path + _conf.get("/tars/application/serverssl/<ca>");
        if (ca == path) ca.clear();
        string cert = path + _conf.get("/tars/application/serverssl/<cert>");
        string key = path + _conf.get("/tars/application/serverssl/<key>");
        bool verifyClient = (_conf.get("/tars/application/serverssl/<verifyclient>", "0") == "0") ? false : true;

        if (!SSLManager::getInstance()->AddCtx("server", ca, cert, key, verifyClient))
            cout << "failed add server cert " << ca << endl;
        else
            cout << "succ add server cert " << ca << ", verifyClient " << verifyClient << endl;
    } catch(...) {
    }
#endif
}

void Application::outServer(ostream &os)
{
    os << outfill("Application")        << ServerConfig::Application << endl;
    os << outfill("ServerName")         << ServerConfig::ServerName << endl;
    os << outfill("BasePath")           << ServerConfig::BasePath << endl;
    os << outfill("DataPath")           << ServerConfig::DataPath << endl;
    os << outfill("LocalIp")            << ServerConfig::LocalIp << endl;
    os << outfill("Local")              << ServerConfig::Local << endl;
    os << outfill("LogPath")            << ServerConfig::LogPath << endl;
    os << outfill("LogSize")            << ServerConfig::LogSize << endl;
    os << outfill("LogNum")             << ServerConfig::LogNum << endl;
    os << outfill("Log")                << ServerConfig::Log << endl;
    os << outfill("Node")               << ServerConfig::Node << endl;
    os << outfill("Config")             << ServerConfig::Config << endl;
    os << outfill("Notify")             << ServerConfig::Notify << endl;
    os << outfill("OpenCoroutine")      << ServerConfig::OpenCoroutine << endl;
    os << outfill("CoroutineMemSize")   << ServerConfig::CoroutineMemSize << endl;
    os << outfill("CoroutineStackSize") << ServerConfig::CoroutineStackSize << endl;
    os << outfill("CloseCout")          << TC_Common::tostr(_conf.get("/tars/application/server<closecout>",AppCache::getInstance()->get("closeCout")) == "0"?0:1)<< endl;
    os << outfill("netthread")          << TC_Common::tostr(_conf.get("/tars/application/server<netthread>","1")) << endl;
    os << outfill("BackPacketBuffLimit") << TC_Common::strto<size_t>(toDefault(_conf.get("/tars/application/server<BackPacketBuffLimit>", "0"), "0")) << endl;

    string level = AppCache::getInstance()->get("logLevel");
    if(level.empty())
    {
        level = _conf.get("/tars/application/server<logLevel>","DEBUG");
    }
    os << outfill("logLevel")           << level<< endl;

    os << outfill("ReportFlow")         << ServerConfig::ReportFlow<< endl;
}

void Application::bindAdapter(vector<TC_EpollServer::BindAdapterPtr>& adapters)
{
    size_t iBackPacketBuffLimit = TC_Common::strto<size_t>(toDefault(_conf.get("/tars/application/server<BackPacketBuffLimit>", "0"), "0"));

    string sPrefix = ServerConfig::Application + "." + ServerConfig::ServerName + ".";

    vector<string> adapterName;

    map<string, ServantHandle*> servantHandles;

    if (_conf.getDomainVector("/tars/application/server", adapterName))
    {
        for (size_t i = 0; i < adapterName.size(); i++)
        {
            string servant = _conf.get("/tars/application/server/" + adapterName[i] + "<servant>");

            checkServantNameValid(servant, sPrefix);

            ServantHelperManager::getInstance()->setAdapterServant(adapterName[i], servant);

            TC_EpollServer::BindAdapterPtr bindAdapter = new TC_EpollServer::BindAdapter(_epollServer.get());
               
            // 设置该obj的鉴权账号密码，只要一组就够了
            {    
                std::string accKey = _conf.get("/tars/application/server/" + adapterName[i] + "<accesskey>"); 
                std::string secretKey = _conf.get("/tars/application/server/" + adapterName[i] + "<secretkey>"); 

                if (!accKey.empty()) 
                    bindAdapter->setAkSk(accKey, secretKey); 

                bindAdapter->setAuthProcessWrapper(&tars::processAuth); 
            }  

            string sLastPath = "/tars/application/server/" + adapterName[i];

            bindAdapter->setName(adapterName[i]);

            bindAdapter->setEndpoint(_conf[sLastPath + "<endpoint>"]);

            bindAdapter->setMaxConns(TC_Common::strto<int>(_conf.get(sLastPath + "<maxconns>", "128")));

            bindAdapter->setOrder(parseOrder(_conf.get(sLastPath + "<order>","allow,deny")));

            bindAdapter->setAllow(TC_Common::sepstr<string>(_conf[sLastPath + "<allow>"], ";,", false));

            bindAdapter->setDeny(TC_Common::sepstr<string>(_conf.get(sLastPath + "<deny>",""), ";,", false));

            bindAdapter->setQueueCapacity(TC_Common::strto<int>(_conf.get(sLastPath + "<queuecap>", "1024")));

            bindAdapter->setQueueTimeout(TC_Common::strto<int>(_conf.get(sLastPath + "<queuetimeout>", "10000")));

            bindAdapter->setProtocolName(_conf.get(sLastPath + "<protocol>", "tars"));

            if (bindAdapter->isTarsProtocol())
            {
                bindAdapter->setProtocol(AppProtocol::parse);
            }

            bindAdapter->setHandleGroupName(_conf.get(sLastPath + "<handlegroup>", adapterName[i]));

            bindAdapter->setHandleNum(TC_Common::strto<int>(_conf.get(sLastPath + "<threads>", "0")));

            bindAdapter->setBackPacketBuffLimit(iBackPacketBuffLimit);

            _epollServer->bind(bindAdapter);

            adapters.push_back(bindAdapter);

            //队列取平均值
            if(!_communicator->getProperty("property").empty())
            {
                PropertyReportPtr p;
                p = _communicator->getStatReport()->createPropertyReport(bindAdapter->getName() + ".queue", PropertyReport::avg());
                bindAdapter->_pReportQueue = p.get();

                p = _communicator->getStatReport()->createPropertyReport(bindAdapter->getName() + ".connectRate", PropertyReport::avg());
                bindAdapter->_pReportConRate = p.get();

                p = _communicator->getStatReport()->createPropertyReport(bindAdapter->getName() + ".timeoutNum", PropertyReport::sum());
                bindAdapter->_pReportTimeoutNum = p.get();
            }
        }
    }
}

void Application::checkServantNameValid(const string& servant, const string& sPrefix)
{
    if((servant.length() <= sPrefix.length()) || (servant.substr(0, sPrefix.length()) != sPrefix))
    {
        ostringstream os;

        os << "Servant '" << servant << "' error: must be start with '" << sPrefix << "'";

        TarsRemoteNotify::getInstance()->report("exit:" + os.str());

        cout << os.str() << endl;

        terminate();

        exit(-1);
    }
}

void Application::outAdapter(ostream &os, const string &v, TC_EpollServer::BindAdapterPtr lsPtr)
{
    os << outfill("name")             << lsPtr->getName() << endl;
    os << outfill("servant")          << v << endl;
    os << outfill("endpoint")         << lsPtr->getEndpoint().toString() << endl;
    os << outfill("maxconns")         << lsPtr->getMaxConns() << endl;
    os << outfill("queuecap")         << lsPtr->getQueueCapacity() << endl;
    os << outfill("queuetimeout")     << lsPtr->getQueueTimeout() << "ms" << endl;
    os << outfill("order")            << (lsPtr->getOrder()==TC_EpollServer::BindAdapter::ALLOW_DENY?"allow,deny":"deny,allow") << endl;
    os << outfill("allow")            << TC_Common::tostr(lsPtr->getAllow()) << endl;
    os << outfill("deny")             << TC_Common::tostr(lsPtr->getDeny()) << endl;
    os << outfill("queuesize")        << lsPtr->getRecvBufferSize() << endl;
    os << outfill("connections")      << lsPtr->getNowConnection() << endl;
    os << outfill("protocol")         << lsPtr->getProtocolName() << endl;
    os << outfill("handlegroup")      << lsPtr->getHandleGroupName() << endl;
    os << outfill("handlethread")     << lsPtr->getHandleNum() << endl;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
}
