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

#include "ServerFactory.h"
#include "RegistryProxy.h"
#include "CommandLoad.h"
#include "NodeRollLogger.h"

HashMap g_serverInfoHashmap;

ServerFactory::ServerFactory()
: _bReousceLimitConfChanged(false)
, _iMinMonitorIntervalMs(60000)
, _bReportLoadInfo(true)
{
}

ServerObjectPtr ServerFactory::getServer( const string& application, const string& serverName )
{
    TLOGINFO("ServerFactory::getServer: "<<application << "." << serverName << endl);

    Lock lock( *this );

    map<string, ServerGroup>::const_iterator p1 = _mmServerList.find( application );
    if ( p1 != _mmServerList.end() )
    {
        map<string, ServerObjectPtr>::const_iterator p2 = p1->second.find( serverName );
        if ( p2 != p1->second.end() && p2->second )
        {
            return p2->second;
        }
    }
    return NULL;
}

int ServerFactory::eraseServer( const string& application, const string& serverName )
{
    Lock lock( *this );

    map<string, ServerGroup>::const_iterator p1 = _mmServerList.find( application );
    if ( p1 == _mmServerList.end() )
    {
        return 0;
    }

    map<string, ServerObjectPtr>::const_iterator p2 = p1->second.find( serverName );
    if ( p2 == p1->second.end() )
    {
        return 0;
    }

    _mmServerList[application].erase( serverName );
    if ( p1->second.empty() )
    {
        _mmServerList.erase( application );
    }

    return 0;
}

ServerObjectPtr ServerFactory::loadServer( const string& application, const string& serverName,bool enableCache,string& result)
{
    ServerObjectPtr pServerObjectPtr;
    vector<ServerDescriptor> vServerDescriptor;

    vServerDescriptor = getServerFromRegistry(application,serverName,result);

    //全量加载失败后从cache里面读取
    if(vServerDescriptor.size() < 1 && enableCache == true)
    {
        vServerDescriptor = getServerFromCache(application,serverName,result);
    }

    if(vServerDescriptor.size() < 1)
    {
        result += " cannot load server description from regisrty ";
        return NULL;
    }

    for(unsigned i = 0; i < vServerDescriptor.size(); i++)
    {
        pServerObjectPtr = createServer(vServerDescriptor[i],result);
    }

    return  pServerObjectPtr;
}

vector<ServerDescriptor> ServerFactory::getServerFromRegistry( const string& application, const string& serverName, string& result)
{
    TLOGDEBUG("ServerFactory::getServerFromRegistry"<<application <<"."<<serverName<<endl);

    vector<ServerDescriptor> vServerDescriptor;
    try
    {
        RegistryPrx _pRegistryPrx = AdminProxy::getInstance()->getRegistryProxy();

        if(!_pRegistryPrx)
        {
            TLOGERROR("ServerFactory::getServerFromRegistry cann't get the proxy of registry. "<<endl);
            return vServerDescriptor;
        }

        vServerDescriptor =_pRegistryPrx->getServers( application, serverName, _tPlatformInfo.getNodeName());
        //清空cache
        if( vServerDescriptor.size()> 0 && application == "" && serverName == "")
        {
            g_serverInfoHashmap.clear();
            TLOGINFO("ServerFactory::getServerFromRegistry hashmap clear ok "<<endl);

        }

        //重置cache
        for(unsigned i = 0; i < vServerDescriptor.size(); i++)
        {
            ServerInfo tServerInfo;
            tServerInfo.application = vServerDescriptor[i].application;
            tServerInfo.serverName  = vServerDescriptor[i].serverName;

            g_serverInfoHashmap.set(tServerInfo,vServerDescriptor[i]);

            TLOGINFO("ServerFactory::getServerFromRegistry hashmap set ok "<<tServerInfo.application<<"."<<tServerInfo.serverName<<endl);
        }

    }
    catch(exception &e)
    {
        TLOGERROR("ServerFactory::getServerFromRegistry exception"<<e.what()<<endl);
    }
    return  vServerDescriptor;
}

vector<ServerDescriptor> ServerFactory::getServerFromCache( const string& application, const string& serverName, string& result)
{
    TLOGDEBUG("ServerFactory::getServerFromCache: "<< application <<"."<<serverName<<endl);

    ServerInfo tServerInfo;
    ServerDescriptor tServerDescriptor;
    vector<ServerDescriptor> vServerDescriptor;
    if(application != "" && serverName != "")
    {
        tServerInfo.application = application;
        tServerInfo.serverName  = serverName;
        if(g_serverInfoHashmap.get(tServerInfo,tServerDescriptor) == TC_HashMap::RT_OK)
        {
            vServerDescriptor.push_back(tServerDescriptor);
        }
    }
    else
    {
        HashMap::lock_iterator it = g_serverInfoHashmap.beginSetTime();
        while(it != g_serverInfoHashmap.end())
        {
            ServerInfo   tServerInfo;
            ServerDescriptor tServerDescriptor;
            int ret = it->get(tServerInfo,tServerDescriptor);
            if(ret != TC_HashMap::RT_OK)
            {
                result =result + "\n hashmap erro:"+TC_Common::tostr(ret);
                TLOGERROR("ServerFactory::getServerFromCache "<<result<< endl);
                break;
            }

            if(serverName == "" && application == "")
            {
                vServerDescriptor.push_back(tServerDescriptor);
            }
            else if(application == "" && serverName == tServerInfo.serverName)
            {
                vServerDescriptor.push_back(tServerDescriptor);
            }
            else if(serverName == "" && application == tServerInfo.application)
            {
                vServerDescriptor.push_back(tServerDescriptor);
            }

            ++it;
        }
    }
    return vServerDescriptor;
}

ServerObjectPtr ServerFactory::createServer(const ServerDescriptor& tDesc, string& result)
{
    Lock lock( *this );
    string application  = tDesc.application;
    string serverName   = tDesc.serverName;

    map<string, ServerGroup>::const_iterator p1 = _mmServerList.find( application );
    if ( p1 != _mmServerList.end() )
    {
        map<string, ServerObjectPtr>::const_iterator p2 = p1->second.find( serverName );
        if ( p2 != p1->second.end() && p2->second )
        {
            p2->second->setServerDescriptor(tDesc);
            CommandLoad command(p2->second,_tPlatformInfo.getNodeInfo());
            int iRet = command.doProcess(result);
            if( iRet == 0)
            {
                return p2->second;
            }
            else
            {
                return NULL;
            }
        }
    }

    ServerObjectPtr pServerObjectPtr = new ServerObject(tDesc);
    CommandLoad command(pServerObjectPtr,_tPlatformInfo.getNodeInfo());
    int iRet = command.doProcess(result);
    if(iRet ==0)
    {
        ServerObject::ServerLimitInfo tInfo;
        loadLimitInfo(application, pServerObjectPtr->getServerId(), tInfo);
        pServerObjectPtr->setServerLimitInfo(tInfo);

        if(tInfo.iMonitorIntervalMs < _iMinMonitorIntervalMs)
        {
            NODE_LOG("KeepAliveThread")->debug() <<FILE_FUN<< "_iMinMonitorIntervalMs " << _iMinMonitorIntervalMs << "->" << tInfo.iMonitorIntervalMs << "|" << pServerObjectPtr->getServerId() << endl;
            _iMinMonitorIntervalMs = tInfo.iMonitorIntervalMs;
        }

        if (!tInfo.bReportLoadInfo)
        {
            NODE_LOG("KeepAliveThread")->debug() <<FILE_FUN<< "bReportLoadInfo:"<<tInfo.bReportLoadInfo<<"|"<<pServerObjectPtr->getServerId()<<endl;
            _bReportLoadInfo=false;
        }

        _mmServerList[application][serverName] = pServerObjectPtr;
        return pServerObjectPtr;
    }
    return NULL;
}

map<string, ServerGroup> ServerFactory::getAllServers()
{
    Lock lock( *this );
    return _mmServerList;
}

bool ServerFactory::loadConfig()
{
    try
    {
        //首先取默认配置
        string sDefault="/tars/app_conf/default";
        map<string, string> m = g_pconf->getDomainMap(sDefault);
        parseLimitInfo(m, _defaultLimitInfo,true);
        NODE_LOG("KeepAliveThread")->debug()<<FILE_FUN << "_defaultLimitInfo " << "|" << _defaultLimitInfo.str() << endl;

        vector<string> v = g_pconf->getDomainVector("/tars/app_conf/");

        map<string,ServerObject::ServerLimitInfo>& appConfig = _mAppCoreConfig.getWriterData();
        for(size_t i = 0; i < v.size(); i++)
        {
            string path = "/tars/app_conf/" + v[i];
            TLOGDEBUG("ServerFactory::loadConfig path:" << path << endl);
            map<string, string> m = g_pconf->getDomainMap(path);
            if(v[i] == "default")
            {
                continue;
            }
            else
            {
                ServerObject::ServerLimitInfo limitInfo;
                parseLimitInfo(m, limitInfo,false);

                limitInfo.bCloseCore = false;
                limitInfo.eCoreType  = ServerObject::EM_AUTO_LIMIT;

                vector<string> apps = TC_Common::sepstr<string>(g_pconf->get(string("/tars/app_conf<") +v[i]+ ">",""),"|");
                for(size_t i = 0; i < apps.size(); i++)
                {
                    string app = apps[i];
                    appConfig[app] = limitInfo;
                }
            }
        }
        _mAppCoreConfig.swap();

        vector<string> vCloseCoreSrvs     = TC_Common::sepstr<string>(g_pconf->get("/tars/app_conf<closecore>",""),"|");
        vector<string>::const_iterator it = vCloseCoreSrvs.begin();

        map<string,ServerObject::ServerLimitInfo>& serverConfig =  _mServerCoreConfig.getWriterData();
        for(;it != vCloseCoreSrvs.end();it++)
        {
            ServerObject::ServerLimitInfo tInfo = _defaultLimitInfo;
            tInfo.bCloseCore = true;
            tInfo.eCoreType = ServerObject::EM_MANUAL_LIMIT;

            serverConfig[*it] = tInfo;
            TLOGDEBUG("ServerFactory::loadConfig Load ClosecoreLimit:" << *it <<endl);
        }

        _mServerCoreConfig.swap();

        _bReousceLimitConfChanged = true;
    }
    catch (std::exception& e)
    {
        TLOGERROR("ServerFactory::loadConfig exception:" << e.what() <<endl);
        return false;
    }
    catch(...)
    {
        TLOGERROR("ServerFactory::loadConfig exception" <<endl);
        return false;
    }
    return true;
}

void ServerFactory::parseLimitInfo(const map<string, string>& confMap, ServerObject::ServerLimitInfo& limitInfo,bool bDefault)
{
    map<string, string>::const_iterator it = confMap.find("maxstopcount");
    if(it != confMap.end())
    {
        limitInfo.iMaxExcStopCount = TC_Common::strto<int>(it->second);
    }
    else
    {
        limitInfo.iMaxExcStopCount=(bDefault ? 3 :_defaultLimitInfo.iMaxExcStopCount);
    }

    it = confMap.find("coretimeinterval");
    if(it != confMap.end())
    {
        limitInfo.iCoreLimitTimeInterval = TC_Common::strto<int>(it->second);
    }
    else
    {
        limitInfo.iCoreLimitTimeInterval = (bDefault ? 5 : _defaultLimitInfo.iCoreLimitTimeInterval);
    }

    it = confMap.find("coretimeexpired");
    if(it != confMap.end())
    {
        limitInfo.iCoreLimitExpiredTime = TC_Common::strto<int>(it->second);
    }
    else
    {
        limitInfo.iCoreLimitExpiredTime =(bDefault ? 30 :_defaultLimitInfo.iCoreLimitExpiredTime);
    }

    it = confMap.find("corelimitenable");
    if(it != confMap.end())
    {
        limitInfo.bEnableCoreLimit = it->second == "true";
    }
    else
    {
        limitInfo.bEnableCoreLimit = (bDefault ? false :_defaultLimitInfo.bEnableCoreLimit);
    }

    it = confMap.find("monitorIntervalMs");
    if(it != confMap.end())
    {
        limitInfo.iMonitorIntervalMs = TC_Common::strto<int>(it->second);
    }
    else
    {
        limitInfo.iMonitorIntervalMs =(bDefault ? 1000 : _defaultLimitInfo.iMonitorIntervalMs);
    }

    it = confMap.find("ActivatorMaxCount");
    if(it != confMap.end())
    {
        limitInfo.iActivatorMaxCount = TC_Common::strto<int>(it->second);
    }
    else
    {
        limitInfo.iActivatorMaxCount =(bDefault ? 10 : _defaultLimitInfo.iActivatorMaxCount);
    }

    it = confMap.find("ActivatorTimeInterval");
    if(it != confMap.end())
    {
        limitInfo.iActivatorTimeInterval = TC_Common::strto<int>(it->second);
    }
    else
    {
        limitInfo.iActivatorTimeInterval =(bDefault ? 60 : _defaultLimitInfo.iActivatorTimeInterval);
    }

    it = confMap.find("ActivatorPunishInterval");
    if(it != confMap.end())
    {
        limitInfo.iActivatorPunishInterval = TC_Common::strto<int>(it->second);
    }
    else
    {
        limitInfo.iActivatorPunishInterval =(bDefault ? 600 : _defaultLimitInfo.iActivatorPunishInterval);
    }

    it=confMap.find("reportLoadInfoenable");
    if (it!=confMap.end())
    {
        limitInfo.bReportLoadInfo=TC_Common::strto<int>(it->second)==1 ? true : false;
    }
    else
    {
        limitInfo.bReportLoadInfo=(bDefault ? true : _defaultLimitInfo.bReportLoadInfo);
    }

}

void ServerFactory::setAllServerResourceLimit()
{
    Lock lock( *this );

    //有更新才加载
    if(_bReousceLimitConfChanged)
    {

        map<string, ServerGroup>::const_iterator it = _mmServerList.begin();
        for(;it != _mmServerList.end(); it++)
        {
            map<string, ServerObjectPtr>::const_iterator  p = it->second.begin();
            for(;p != it->second.end(); p++)
            {
                string sAppId = it->first;
                string sServerId    = it->first+"."+p->first;
                ServerObjectPtr pServerObjectPtr = p->second;
                if(!pServerObjectPtr)
                {
                    continue;
                }

                ServerObject::ServerLimitInfo tInfo;
                loadLimitInfo(sAppId, sServerId, tInfo);
                if(tInfo.iMonitorIntervalMs < _iMinMonitorIntervalMs)
                {
                    NODE_LOG("KeepAliveThread")->debug()<<FILE_FUN << "_iMinMonitorIntervalMs " << _iMinMonitorIntervalMs << "->" << tInfo.iMonitorIntervalMs << "|" << sServerId << endl;
                    _iMinMonitorIntervalMs = tInfo.iMonitorIntervalMs;
                }

                TLOGDEBUG("ServerFactory::setAllServerResourceLimit setAllServerResourceLimit|" << sServerId <<"|"<<tInfo.eCoreType <<endl);
                pServerObjectPtr->setServerLimitInfo(tInfo);

            }
        }
        _bReousceLimitConfChanged = false;
    }
}

void ServerFactory::loadLimitInfo(const string& sAppId, const string& sServerId, ServerObject::ServerLimitInfo& tInfo) {

    map<string,ServerObject::ServerLimitInfo>& appConfig    = _mAppCoreConfig.getReaderData();
    map<string,ServerObject::ServerLimitInfo>& serverConfig = _mServerCoreConfig.getReaderData();

    if(serverConfig.count(sServerId) == 1)
    {
        tInfo = serverConfig[sServerId];
    }
    else if(appConfig.count(sAppId) == 1)
    {
        tInfo = appConfig[sAppId];
    }
    else
    {
        tInfo = _defaultLimitInfo;
        tInfo.bCloseCore = false;
        tInfo.eCoreType  = ServerObject::EM_AUTO_LIMIT;
    }

}


