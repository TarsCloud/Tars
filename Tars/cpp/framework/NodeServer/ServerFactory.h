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

#ifndef __SERVER_FACTORY_H_
#define __SERVER_FACTORY_H_
#include "Node.h"
#include <unistd.h>
#include "ServerObject.h"
#include "util/tc_common.h"
#include "jmem/jmem_hashmap.h"
#include "util/tc_file.h"
#include "util/tc_singleton.h"
#include "util/tc_readers_writer_data.h"

using namespace tars;
using namespace std;

typedef TarsHashMap<ServerInfo,ServerDescriptor,ThreadLockPolicy,FileStorePolicy> HashMap;

extern HashMap g_serverInfoHashmap;

typedef map<string, ServerObjectPtr> ServerGroup;

class ServerFactory : public TC_Singleton<ServerFactory>, public TC_ThreadLock
{
public:
    /**
     * 构造函数
     */
    ServerFactory();

    /**
     * 析构函数
     */
    ~ServerFactory()
    {
    };

    /**
     * 创建指定服务
     * @param ServerDescriptor    服务信息
     * @return  ServerObjectPtr
     */
    ServerObjectPtr createServer( const ServerDescriptor& tDesc)
    {
        string result;
        return createServer(tDesc,result);
    }

    /**
     * 创建指定服务
     * @param ServerDescriptor    服务信息
     * @param result    结果
     * @return  ServerObjectPtr
     */
    ServerObjectPtr createServer( const ServerDescriptor& tDesc,string& result);

    /**
     * 删除指定服务
     * @param application    服务所属应用名
     * @param serverName  服务名
     * @return  int
     */
    int eraseServer( const string& application, const string& serverName );

    /**
     * 获取指定服务
     * @param application    服务所属应用名
     * @param serverName  服务名
     * @return  ServerObjectPtr  服务不存在返回NULL
     */
    ServerObjectPtr getServer( const string& application, const string& serverName );

    /**
     * load服务 若application
     * serverName为空load所有服务，只返回最后一个load对象。
     * @param application    服务所属应用名
     * @param serverName  服务名
     * @return  ServerObjectPtr  服务不存在返回NULL
     */
    ServerObjectPtr loadServer( const string& application="", const string& serverName="",bool enableCache = true)
    {
        string result;
        return loadServer(application,serverName,enableCache,result);
    }

    /**
     * load服务 若application
     * serverName为空load所有服务，只返回最后一个load对象。
     * @param application    服务所属应用名
     * @param serverName  服务名
     * @param result  结果
     * @return  ServerObjectPtr  服务不存在返回NULL
     */
    ServerObjectPtr loadServer( const string& application, const string& serverName,bool enableCache, string& result);

    /**
     * get服务 若application
     * serverName为空获取所有服务，只返回最后一个load对象。
     * @param application    服务所属应用名
     * @param serverName  服务名
     * @return  vector<ServerDescriptor>  服务不存在返回NULL
     */
    vector<ServerDescriptor> getServerFromRegistry( const string& application, const string& serverName, string& result);

    /**
     * get服务 若application
     * serverName为空获取所有服务，只返回最后一个load对象。
     * @param application    服务所属应用名
     * @param serverName  服务名
     * @return  vector<ServerDescriptor>  服务不存在返回NULL
     */
    vector<ServerDescriptor> getServerFromCache( const string& application, const string& serverName, string& result);

    /**
     * 获取node上服务所有服务
     * @return    map<string, ServerGroup>
     */
    map<string, ServerGroup> getAllServers();

    /**
     *@brief 加载服务core属性的配置文件
     *@return bool
     */
    bool loadConfig();

    /**
     *@brief 设置node上所有服务的core属性，如果node更新过core配置，则不需要设置
     *
     */
    void setAllServerResourceLimit();

    /**
     * 获取最小的监控周期
     */
    int32_t getMinMonitorIntervalMs()
    {
        return _iMinMonitorIntervalMs;
    }

    /**
     *    是否上报节点负载信息
     */
    bool getReportLoadInfo() {return _bReportLoadInfo;}

private:

    /**
     * 解析node管理服务的limit资源配置
     */
    void parseLimitInfo(const map<string, string>& confMap, ServerObject::ServerLimitInfo& limitInfo, bool bDefault);

    /**
     * 获取服务进程的limit资源配置
     */
    void loadLimitInfo(const string& sAppId, const string& sServerId, ServerObject::ServerLimitInfo& tInfo);

private:
    //<app,<server,serverObjectPtr>>
    map<string, ServerGroup> _mmServerList;

    PlatformInfo             _tPlatformInfo;

private:

    //<app.server,ServerLimitStateInfo>
    TC_ReadersWriterData<map<string, ServerObject::ServerLimitInfo> > _mServerCoreConfig;

    //<app,ServerLimitStateInfo>
    TC_ReadersWriterData<map<string, ServerObject::ServerLimitInfo> > _mAppCoreConfig;

    //配置是否有更新
    bool                          _bReousceLimitConfChanged;

    //最大服务异常停止个数，超过限制则自动屏蔽coredump
    ServerObject::ServerLimitInfo _defaultLimitInfo;

    //最小的监控周期
    int32_t                       _iMinMonitorIntervalMs;

    //是否上报节点负载信息,默认上报
    bool                          _bReportLoadInfo; 
};


#endif


