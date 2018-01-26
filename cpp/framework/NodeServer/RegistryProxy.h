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

#ifndef __REGISTRY_PROXY_H_
#define __REGISTRY_PROXY_H_

#include "Node.h"
#include "ServerFactory.h"
#include "util/tc_config.h"
#include "QueryF.h"
#include "servant/Communicator.h"
#include "util/tc_singleton.h"
#include "NodeServer.h"

using namespace tars;
using namespace std;

class AdminProxy;

class AdminProxy : public TC_Singleton<AdminProxy>
{
public:
    RegistryPrx getRegistryProxy()
    {
        RegistryPrx pRistryPrx;
        string sRegistry = g_app.getConfig().get("/tars/node<registryObj>",_registryProxyName);

        Application::getCommunicator()->stringToProxy(sRegistry, pRistryPrx);
        
        return pRistryPrx;
    }

    QueryFPrx getQueryProxy()
    {
        QueryFPrx pQueryPrx;
        string sQuery = g_app.getConfig().get("/tars/node<queryObj>", _queryProxyName);

         Application::getCommunicator()->stringToProxy(sQuery, pQueryPrx);

         return pQueryPrx;
    }

    inline void setRegistryObjName(const string &sRegistryProxyName, const string &sQueryProxyName)
    {
        _registryProxyName = sRegistryProxyName;
        _queryProxyName       = sQueryProxyName;
    }

    inline string& getRegistryProxyName(){ return _registryProxyName; }

    inline string& getQueryProxyName() { return _queryProxyName; }    

private:
    
    string _registryProxyName;
    string _queryProxyName;
};
#endif


