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

#include "servant/ObjectProxyFactory.h"

namespace tars
{

ObjectProxyFactory::ObjectProxyFactory(CommunicatorEpoll * pCommunicatorEpoll)
: _communicatorEpoll(pCommunicatorEpoll)
, _objNum(0)
{
}

ObjectProxyFactory::~ObjectProxyFactory()
{
    for(size_t i = 0; i < _vObjectProxys.size(); i++)
    {
        if(_vObjectProxys[i])
        {
            delete _vObjectProxys[i];
            _vObjectProxys[i] = NULL;
        }
    }
}

ObjectProxy * ObjectProxyFactory::getObjectProxy(const string& sObjectProxyName,const string& setName)
{
    TC_LockT<TC_ThreadRecMutex> lock(*this);

    string tmpObjName = sObjectProxyName + ":" + setName;
    map<string, ObjectProxy*>::iterator it = _objectProxys.find(tmpObjName);
    if(it != _objectProxys.end())
    {
        return it->second;
    }

    ObjectProxy * pObjectProxy = new ObjectProxy(_communicatorEpoll, sObjectProxyName,setName);

    pObjectProxy->initialize();

    _objectProxys[tmpObjName] = pObjectProxy;
    //_objectProxys.insert(make_pair<string,ObjectProxy*>(tmpObjName,pObjectProxy));

    _vObjectProxys.push_back(pObjectProxy);

    _objNum++;

    return pObjectProxy;
}

int ObjectProxyFactory::loadObjectLocator()
{
    TC_LockT<TC_ThreadRecMutex> lock(*this);

    for (size_t i = 0; i < _objNum; i++)
    {
        _vObjectProxys[i]->loadLocator();
    }

    return 0;
}
///////////////////////////////////////////////////////////////////
}
