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

#include "servant/ServantHelper.h"

namespace tars
{

ServantPtr ServantHelperManager::create(const string &sAdapter)
{
    if(_adapter_servant.find(sAdapter) == _adapter_servant.end())
    {
        return NULL;
    }

    ServantPtr servant = NULL;

    //根据adapter查找servant名称
    string s = _adapter_servant[sAdapter];

    if(_servant_creator.find(s) != _servant_creator.end())
    {
        servant = _servant_creator[s]->create(s);
    }
    return servant;
}

void ServantHelperManager::setAdapterServant(const string &sAdapter, const string &sServant)
{
    _adapter_servant[sAdapter] = sServant;

    _servant_adapter[sServant] = sAdapter;
}

bool ServantHelperManager::setDyeing(const string & sDyeingKey, const string & sDyeingServant,
        const string & sDyeingInterface)
{
    TC_LockT<TC_ThreadMutex> lock(_mutex);

    _dyeingKey       = sDyeingKey;
    _dyeingServant   = sDyeingServant;
    _dyeingInterface = sDyeingInterface;

    _isDyeing        = !sDyeingKey.empty();

    return true;
}

bool ServantHelperManager::isDyeingReq(const string & sKey, const string & sServant, const string & sInterface)
{
    TC_LockT<TC_ThreadMutex> lock(_mutex);

    return  ((_dyeingKey == sKey) && (_dyeingServant == sServant) &&
        (_dyeingInterface == "" || _dyeingInterface == sInterface) );
}

}


