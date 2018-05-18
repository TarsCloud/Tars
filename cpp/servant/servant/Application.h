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

#ifndef __TARS_APPLICATION_H
#define __TARS_APPLICATION_H

#include <iostream>
#include <set>
#include <signal.h>

#include "util/tc_autoptr.h"
#include "util/tc_config.h"
#include "util/tc_epoll_server.h"
#include "servant/BaseNotify.h"
#include "servant/ServantHelper.h"
#include "servant/ServantHandle.h"
#include "servant/StatReport.h"
#include "servant/CommunicatorFactory.h"
#include "servant/TarsLogger.h"
#include "servant/TarsConfig.h"
#include "servant/TarsNotify.h"

namespace tars
{
//////////////////////////////////////////////////////////////////////
/**
 * 以下定义配置框架支持的命令
 */
#define TARS_CMD_LOAD_CONFIG         "tars.loadconfig"        //从配置中心, 拉取配置下来: tars.loadconfig filename
#define TARS_CMD_SET_LOG_LEVEL       "tars.setloglevel"       //设置滚动日志的等级: tars.setloglevel [NONE, ERROR, WARN, DEBUG]
#define TARS_CMD_VIEW_STATUS         "tars.viewstatus"        //查看服务状态
#define TARS_CMD_VIEW_VERSION        "tars.viewversion"       //查看服务采用TARS的版本
#define TARS_CMD_CONNECTIONS         "tars.connection"        //查看当前链接情况
#define TARS_CMD_LOAD_PROPERTY       "tars.loadproperty"      //使配置文件的property信息生效
#define TARS_CMD_VIEW_ADMIN_COMMANDS "tars.help"              //帮助查看服务支持的管理命令
#define TARS_CMD_SET_DYEING          "tars.setdyeing"         //设置染色信息: tars.setdyeing key servant [interface]
#define TARS_CMD_CLOSE_COUT          "tars.closecout"         //设置是否启关闭cout\cin\cerr: tars.openthreadcontext yes/NO 服务重启才生效
#define TARS_CMD_SET_DAYLOG_LEVEL    "tars.enabledaylog"      //设置按天日志是否输出: tars.enabledaylog [remote|local]|[logname]|[true|false]
#define TARS_CMD_CLOSE_CORE          "tars.closecore"         //设置服务的core limit:  tars.setlimit [yes|no]
#define TARS_CMD_RELOAD_LOCATOR      "tars.reloadlocator"     //重新加载locator的配置信息

//////////////////////////////////////////////////////////////////////
/**
 * 通知信息给notify服务, 展示在页面上
 */
//上报普通信息
#define TARS_NOTIFY_NORMAL(info)     {TarsRemoteNotify::getInstance()->notify(NOTIFYNORMAL, info);}

//上报警告信息
#define TARS_NOTIFY_WARN(info)       {TarsRemoteNotify::getInstance()->notify(NOTIFYWARN, info);}

//上报错误信息
#define TARS_NOTIFY_ERROR(info)      {TarsRemoteNotify::getInstance()->notify(NOTIFYERROR, info);}

//发送心跳给node 多个adapter分别上报
#define TARS_KEEPALIVE(adapter)      {TarsNodeFHelper::getInstance()->keepAlive(adapter);}

//发送TARS版本给node
#define TARS_REPORTVERSION(x)        {TarsNodeFHelper::getInstance()->reportVersion(TARS_VERSION);}

//////////////////////////////////////////////////////////////////////
/**
 * 添加前置的命令处理方法
 * 在所有Normal方法之前执行
 * 多个前置方法之间顺序不确定
 */
#define TARS_ADD_ADMIN_CMD_PREFIX(c,f) \
    do { addAdminCommandPrefix(string(c), std::bind(&f, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)); } while (0);

/**
 * 添加Normal命令处理方法
 * 在所有前置方法最后执行
 * 多个Normal方法之间顺序不确定
 */
#define TARS_ADD_ADMIN_CMD_NORMAL(c,f) \
    do { addAdminCommandNormal(string(c), std::bind(&f, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)); } while (0);

//////////////////////////////////////////////////////////////////////
/**
 * 服务基本信息
 */
struct ServerConfig
{
    static std::string Application;         //应用名称
    static std::string ServerName;          //服务名称,一个服务名称含一个或多个服务标识
    static std::string BasePath;            //应用程序路径，用于保存远程系统配置的本地目录
    static std::string DataPath;            //应用程序数据路径用于保存普通数据文件
    static std::string LocalIp;             //本机IP
    static std::string LogPath;             //log路径
    static int         LogSize;             //log大小(字节)
    static int         LogNum;              //log个数()
    static std::string LogLevel;            //log日志级别
    static std::string Local;               //本地套接字
    static std::string Node;                //本机node地址
    static std::string Log;                 //日志中心地址
    static std::string Config;              //配置中心地址
    static std::string Notify;              //信息通知中心
    static std::string ConfigFile;          //框架配置文件路径
    static int         ReportFlow;          //是否服务端上报所有接口stat流量 0不上报 1上报(用于非tars协议服务流量统计)
    static int         IsCheckSet;          //是否对按照set规则调用进行合法性检查 0,不检查，1检查
    static bool        OpenCoroutine;        //是否启用协程处理方式
    static size_t      CoroutineMemSize;    //协程占用内存空间的最大大小
    static uint32_t    CoroutineStackSize;    //每个协程的栈大小(默认128k)
};

//////////////////////////////////////////////////////////////////////
/**
 * 服务的基类
 */
class Application : public BaseNotify
{
public:

    enum
    {
        REPORT_SEND_QUEUE_INTERVAL = 10, /**上报服务端发送队列大小的间隔时间**/
    };

public:
    /**
     * 应用构造
     */
    Application();

    /**
     * 应用析构
     */
    virtual ~Application();

    /**
     * 初始化
     * @param argv
     */
    void main(int argc, char *argv[]);

    /**
     * 运行
     */
    void waitForShutdown();

public:
    /**
     * 获取配置文件
     *
     * @return TC_Config&
     */
    static TC_Config& getConfig();

    /**
     * 获取通信器
     *
     * @return CommunicatorPtr&
     */
    static CommunicatorPtr& getCommunicator();

    /**
     * 获取服务Server对象
     *
     * @return TC_EpollServerPtr&
     */
    static TC_EpollServerPtr& getEpollServer();

    /**
     *  中止应用
     */
    static void terminate();

    /**
     * 添加Config
     * @param filename
     */
    bool addConfig(const string &filename);

    /**
     * 添加应用级的Config
     * @param filename
     */
    bool addAppConfig(const string &filename);

protected:
    /**
     * 初始化, 只会进程调用一次
     */
    virtual void initialize() = 0;

    /**
     * 析够, 进程只会调用一次
     */
    virtual void destroyApp() = 0;

    /**
     * 处理加载配置的命令
     * 处理完成后继续通知Servant
     * @param filename
     */
    bool cmdLoadConfig(const string& command, const string& params, string& result);

    /**
     * 设置滚动日志等级
     * @param command
     * @param params
     * @param result
     *
     * @return bool
     */
    bool cmdSetLogLevel(const string& command, const string& params, string& result);

    /**
     * 设置服务的core limit
     * @param command
     * @param params
     * @param result
     *
     * @return bool
     */
    bool cmdCloseCoreDump(const string& command, const string& params, string& result);

    /**
     * 设置按天日志是否生效
     * @param command
     * @param params [remote|local]|[logname]|[true|false]
     * @param result
     *
     * @return bool
     */
    bool cmdEnableDayLog(const string& command, const string& params, string& result);

    /**
     * 查看服务状态
     * @param command
     * @param params
     * @param result
     *
     * @return bool
     */
    bool cmdViewStatus(const string& command, const string& params, string& result);

    /**
     * 查看链接状态
     * @param command
     * @param params
     * @param result
     *
     * @return bool
     */
    bool cmdConnections(const string& command, const string& params, string& result);

    /**
     * 查看编译的版本
     * @param command
     * @param params
     * @param result
     *
     * @return bool
     */
    bool cmdViewVersion(const string& command, const string& params, string& result);

    /**
     * 使配置文件的property信息生效
     * @param command
     * @param params
     * @param result
     *
     * @return bool
     */
    bool cmdLoadProperty(const string& command, const string& params, string& result);

    /**
     *查看服务支持的管理命令
     * @param params
     * @param result
     *
     * @return bool
     */
    bool cmdViewAdminCommands(const string& command, const string& params, string& result);

    /**
     * 设置染色消息
     * @param command
     * @param params
     * @param result
     *
     * @return bool
     */
    bool cmdSetDyeing(const string& command, const string& params, string& result);


    /**
     * 设置是否关闭stdcout/stderr/stdin 服务重启能才生效
     * @param command
     * @param params
     * @param result
     *
     * @return bool
     */
    bool cmdCloseCout(const string& command, const string& params, string& result);

    /*
    *通过命令动态加载配置文件，获取最新的locator，以方便应对主控便更
    * @param command
    * @param params
    * @param result
    */
    bool cmdReloadLocator(const string& command, const string& params, string& result);

protected:

    /**
     * 为Adapter绑定对应的handle类型
     * 缺省实现是ServantHandle类型
     * @param adapter
     */
    virtual void setHandle(TC_EpollServer::BindAdapterPtr& adapter);

    /**
     * 添加Servant
     * @param T
     * @param id
     */
    template<typename T> void addServant(const string &id)
    {
        ServantHelperManager::getInstance()->addServant<T>(id,true);
    }

    /**
     * 非tars协议server，设置Servant的协议解析器
     * @param protocol
     * @param servant
     */
    void addServantProtocol(const string& servant, const TC_EpollServer::protocol_functor& protocol);

protected:
    /**
     * 读取基本信息
     */
    void initializeClient();

    /**
     * 输出
     * @param os
     */
    void outClient(ostream &os);

    /**
     * 初始化servant
     */
    void initializeServer();

    /**
     * 输出
     * @param os
     */
    void outServer(ostream &os);

    /**
     * 输出所有的adapter
     * @param os
     */
    void outAllAdapter(ostream &os);

    /**
     * 输出
     * @param os
     */
    void outAdapter(ostream &os, const string &v, TC_EpollServer::BindAdapterPtr lsPtr);

    /**
     * 解析配置文件
     */
    void parseConfig(int argc, char *argv[]);

     /**
     * 解析ip权限allow deny 次序
     */
    TC_EpollServer::BindAdapter::EOrder parseOrder(const string &s);

    /**
     * 绑定server配置的Adapter和对象
     */
    void bindAdapter(vector<TC_EpollServer::BindAdapterPtr>& adapters);

    /**
     * @param servant
     * @param sPrefix
     */
    void checkServantNameValid(const string& servant, const string& sPrefix);

    /**
     * 换成缺省值
     * @param s
     *
     * @return string
     */
    string toDefault(const string &s, const string &sDefault);

    /**
     * 获取服务的set分组信息,setname.setarea.setgroup
     *
     * @return string 没有按set分组则返回空""
     */
     string setDivision(void);

     /*
     * 等待服务退出
     */
     void waitForQuit();

protected:
    /**
     * 配置文件
     */
    static TC_Config           _conf;

    /**
     * 服务
     */
    static TC_EpollServerPtr   _epollServer;

    /**
     * 通信器
     */
    static CommunicatorPtr     _communicator;
};
////////////////////////////////////////////////////////////////////
}

#endif

