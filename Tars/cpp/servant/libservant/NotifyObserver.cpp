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

#include "servant/NotifyObserver.h"
#include "servant/BaseNotify.h"

namespace tars
{

NotifyObserver::NotifyObserver()
{
}

NotifyObserver::~NotifyObserver()
{
}

void NotifyObserver::registerObject(const string& command, BaseNotify* obj, map<string, set<BaseNotify*> >& target)
{
    TC_LockT<TC_ThreadRecMutex> lock(*this);

    target[command].insert(obj);
}

void NotifyObserver::unregisterObject(const string& command, BaseNotify* obj, map<string, set<BaseNotify*> >& target)
{
    TC_LockT<TC_ThreadRecMutex> lock(*this);

    map<string, set<BaseNotify*> >::iterator it;

    it = target.find(command);

    if (it != target.end())
    {
        set<BaseNotify*>::iterator sit = it->second.find(obj);

        if (sit != it->second.end())
        {
            it->second.erase(sit);
        }
        if (it->second.empty())
        {
            target.erase(it);
        }
    }
}

void NotifyObserver::registerNotify(const string& command, BaseNotify* obj)
{
    registerObject(command, obj, _notifys);
}

void NotifyObserver::unregisterNotify(const string& command, BaseNotify* obj)
{
    unregisterObject(command, obj, _notifys);
}

void NotifyObserver::registerPrefix(const string& command, BaseNotify* obj)
{
    registerObject(command, obj, _prefix);
}

void NotifyObserver::unregisterPrefix(const string& command, BaseNotify* obj)
{
    unregisterObject(command, obj, _prefix);
}

string NotifyObserver::notify(const string& command, TarsCurrentPtr current)
{
    TC_LockT<TC_ThreadRecMutex> lock(*this);

    string str    = TC_Common::trim(command);

    string name   = str;

    string params = "";

    string::size_type pos = str.find_first_of(" ");

    if (pos != string::npos)
    {
        name = str.substr(0, pos);

        params = str.substr(pos + 1);
    }

    ostringstream os;

    map<string, set<BaseNotify*> >::iterator it = _prefix.find(name);

    if (it != _prefix.end())
    {
        set<BaseNotify*>& sbn = it->second;

        os << "[notify prefix object num:" << sbn.size() << "]" << endl;

        int i = 0;

        for (set<BaseNotify*>::iterator sit = sbn.begin(); sit != sbn.end(); ++sit)
        {
            string result = "";

            if ((*sit)->notify(name, params, current, result))
            {
                os << "[" << ++i << "]:" << result << endl;
            }
            else
            {
                os << "[notify break by server]:" << endl;
                os << result << endl;

                return os.str();
            }
        }
    }

    it = _notifys.find(name);

    if (it != _notifys.end())
    {
        set<BaseNotify*>& sbn = it->second;

        os << "[notify servant object num:" << sbn.size() << "]" << endl;

        int i = 0;

        for (set<BaseNotify*>::iterator sit = sbn.begin(); sit != sbn.end(); ++sit)
        {
            string result = "";

            if ((*sit)->notify(name, params, current, result))
            {
                os << "[" << ++i << "]:" << result << endl;
            }
            else
            {
                os << "[notify break.]" << endl;
                os << result << endl;

                return os.str();
            }
        }
    }
    return os.str();
}

string NotifyObserver::viewRegisterCommand()
{
    TC_LockT<TC_ThreadRecMutex> lock(*this);

    ostringstream os;

    map<string, set<BaseNotify*> >::iterator it = _prefix.begin();

    set<string> command;

    while (it != _prefix.end())
    {
        command.insert(it->first);
        ++it;
    }

    it = _notifys.begin();

    while (it != _notifys.end())
    {
        command.insert(it->first);
        ++it;
    }

    set<string>::const_iterator p = command.begin();

    while(p != command.end())
    {
        os<<*p<<"\r\n";
        ++p;
    }

    return os.str();
}
///////////////////////////////////////////////////////////////////////////
}
