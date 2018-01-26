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

#include "servant/BaseNotify.h"
#include "servant/NotifyObserver.h"

namespace tars
{

BaseNotify::BaseNotify() 
{
}

BaseNotify::~BaseNotify() 
{
}

void BaseNotify::addAdminCommandPrefix(const string& command, TAdminFunc func)
{
    TC_LockT<TC_ThreadRecMutex> lock(*this);

    _procFunctors.insert(std::make_pair(command, func));

    NotifyObserver::getInstance()->registerPrefix(command, this);
}

void BaseNotify::addAdminCommandNormal(const string& command, TAdminFunc func)
{
    TC_LockT<TC_ThreadRecMutex> lock(*this);

    _procFunctors.insert(std::make_pair(command, func));

    NotifyObserver::getInstance()->registerNotify(command, this);
}

bool BaseNotify::notify(const string& cmd, const string& params, TarsCurrentPtr current, string& result)
{
    TC_LockT<TC_ThreadRecMutex> lock(*this);

    map<string, TAdminFunc>::iterator it;

    it =  _procFunctors.find(cmd);

    if (it != _procFunctors.end())
    {
        return (it->second)(cmd, params, result);
    }
    return false;
}
//////////////////////////////////////////////////////////////////
}
