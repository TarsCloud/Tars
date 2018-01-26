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

#include "RegistryServer.h"

extern TC_Config *g_pconf;

void RegistryServer::initialize()
{
    TLOGDEBUG("RegistryServer::initialize..." << endl);

    try
    {
        //加载registry对象的端口信息
        loadServantEndpoint(); 

        //ReapThread初始化时会用到
        TarsTimeLogger::getInstance()->initFormat("group_id", "%Y%m%d%H");
        TarsTimeLogger::getInstance()->enableRemote("group_id", true);

        //全量和增量加载路由信息的线程
        _reapThread.init();
        _reapThread.start();

        //检查node超时的线程
        _checkNodeThread.init();
        _checkNodeThread.start();

        //监控所有服务状态的线程
        _checksetingThread.init();
        _checksetingThread.start();

        //异步处理线程
        _registryProcThread = new RegistryProcThread();
        int num = TC_Common::strto<int>(g_pconf->get("/tars/reap<asyncthread>", "3"));
        _registryProcThread->start(num);

        //供node访问的对象
        addServant<RegistryImp>((*g_pconf)["/tars/objname<RegistryObjName>"]);

        //供tars的服务获取路由的对象
        addServant<QueryImp>((*g_pconf)["/tars/objname<QueryObjName>"]);

        TarsTimeLogger::getInstance()->enableRemote("", false);

        TarsTimeLogger::getInstance()->initFormat("query_set", "%Y%m%d%H");
        TarsTimeLogger::getInstance()->enableRemote("query_set", false);

        TarsTimeLogger::getInstance()->initFormat("query_idc", "%Y%m%d%H");
        TarsTimeLogger::getInstance()->enableRemote("query_idc", false);

        TarsTimeLogger::getInstance()->initFormat("query", "%Y%m%d%H");
        TarsTimeLogger::getInstance()->enableRemote("query", false);
    }
    catch (TC_Exception& ex)
    {
        TLOGERROR("RegistryServer initialize exception:" << ex.what() << endl);
        cerr << "RegistryServer initialize exception:" << ex.what() << endl;
        exit(-1);
    }

    TLOGDEBUG("RegistryServer::initialize OK!" << endl);
}


void RegistryServer::destroyApp()
{
    if (_registryProcThread)
    {
        _registryProcThread->terminate();
    }

    TLOGDEBUG("RegistryServer::destroyApp ok" << endl);
}

RegistryProcThread* RegistryServer::getRegProcThread()
{
    return _registryProcThread.get();
}

int RegistryServer::loadServantEndpoint()
{
    map<string, string> mapAdapterServant = ServantHelperManager::getInstance()->getAdapterServant();

    map<string, string>::iterator iter;
    for (iter = mapAdapterServant.begin(); iter != mapAdapterServant.end(); iter++)
    {
        TC_Endpoint ep = getEpollServer()->getBindAdapter(iter->first)->getEndpoint();

        _mapServantEndpoint[iter->second] = ep.toString();

        TLOGDEBUG("registry obj: " << iter->second << " = " << ep.toString() << endl);
    }

    return 0;
}

TC_Endpoint RegistryServer::getAdapterEndpoint(const string& name) const
{
    TC_Endpoint locator;

    try
    {
        locator = getEpollServer()->getBindAdapter(name)->getEndpoint();
    }
    catch (exception& ex)
    {
        TLOGERROR("RegistryServer::getAdapterEndpoint exception: " << ex.what() << endl);
    }

    return locator;
}


