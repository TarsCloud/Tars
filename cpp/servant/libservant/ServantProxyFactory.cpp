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

#include "servant/ServantProxyFactory.h"
#include "servant/TarsLogger.h"

namespace tars
{

ServantProxyFactory::ServantProxyFactory(Communicator* cm)
: _comm(cm)
{
}

ServantProxyFactory::~ServantProxyFactory()
{
}

ServantPrx::element_type* ServantProxyFactory::getServantProxy(const string& name,const string& setName)
{
    TC_LockT<TC_ThreadRecMutex> lock(*this);

    string tmpObjName = name + ":" + setName;

    map<string, ServantPrx>::iterator it = _servantProxy.find(tmpObjName);
    if(it != _servantProxy.end())
    {
        return it->second.get();
    }

    ObjectProxy ** ppObjectProxy = new ObjectProxy * [_comm->getClientThreadNum()];
    assert(ppObjectProxy != NULL);

    for(size_t i = 0; i < _comm->getClientThreadNum(); ++i)
    {
        ppObjectProxy[i] = _comm->getCommunicatorEpoll(i)->getObjectProxy(name, setName);
    }

    ServantPrx sp = new ServantProxy(_comm, ppObjectProxy, _comm->getClientThreadNum());

    int syncTimeout  = TC_Common::strto<int>(_comm->getProperty("sync-invoke-timeout", "3000"));
    int asyncTimeout = TC_Common::strto<int>(_comm->getProperty("async-invoke-timeout", "5000"));
    int conTimeout   = TC_Common::strto<int>(_comm->getProperty("connect-timeout", "1500"));

    sp->tars_timeout(syncTimeout);
    sp->tars_async_timeout(asyncTimeout);
    sp->tars_connect_timeout(conTimeout);

    _servantProxy[tmpObjName] = sp;

    return sp.get();
}
///////////////////////////////////////////////////////////////////////////////
}
