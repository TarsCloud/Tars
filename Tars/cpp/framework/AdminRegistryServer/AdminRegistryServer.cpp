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

#include "AdminRegistryServer.h"
#include "AdminRegistryImp.h"

TC_Config * g_pconf;
AdminRegistryServer g_app;

extern TC_Config * g_pconf;

//内部版本
const string SERVER_VERSION = "B002";

void AdminRegistryServer::initialize()
{
    TLOGDEBUG("AdminRegistryServer::initialize..." << endl);

    try
    {
        string size = Application::getCommunicator()->getProperty("timeout-queue-size", "");
        if(size.empty())
        {
            Application::getCommunicator()->setProperty("timeout-queue-size","100");
        }

        loadServantEndpoint();

        //轮询线程
        _reapThread.init();
        _reapThread.start();

        //供admin client访问的对象
        string adminObj = g_pconf->get("/tars/objname<AdminRegObjName>", "");
        if(adminObj != "")
        {
            addServant<AdminRegistryImp>(adminObj);
        }
    }
    catch(TC_Exception & ex)
    {
        TLOGERROR("RegistryServer initialize exception:" << ex.what() << endl);
        cerr << "RegistryServer initialize exception:" << ex.what() << endl;
        exit(-1);
    }

    TLOGDEBUG("RegistryServer::initialize OK!" << endl);
}

int AdminRegistryServer::loadServantEndpoint()
{
    map<string, string> mapAdapterServant;
    mapAdapterServant = ServantHelperManager::getInstance()->getAdapterServant();

    map<string, string>::iterator iter;
    for(iter = mapAdapterServant.begin(); iter != mapAdapterServant.end(); iter++ )
    {
        TC_Endpoint ep = getEpollServer()->getBindAdapter(iter->first)->getEndpoint();

        _mapServantEndpoint[iter->second] = ep.toString();

        TLOGDEBUG("registry obj: " << iter->second << " = " << ep.toString() <<endl);
    }

    return 0;
}

void AdminRegistryServer::destroyApp()
{
    _reapThread.terminate();

    TLOGDEBUG("AdminRegistryServer::destroyApp ok" << endl);
}


int main(int argc, char *argv[])
{
    try
    {
        g_pconf =  & g_app.getConfig();
        g_app.main(argc, argv);

        g_app.waitForShutdown();
    }
    catch(exception &ex)
    {
        cerr<< ex.what() << endl;
    }

    return 0;
}


