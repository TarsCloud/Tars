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

#include "servant/Servant.h"
#include "servant/BaseF.h"
#include "servant/Application.h"
#include "servant/AppCache.h"
#include "servant/TarsLogger.h"

#include <cerrno>

namespace tars
{

TC_ThreadMutex CallbackThreadData::_mutex;
pthread_key_t CallbackThreadData::_key = 0;

Servant::Servant():_handle(NULL)
{
}

Servant::~Servant()
{
}

void Servant::setName(const string &name)
{
    _name = name;
}

string Servant::getName() const
{
    return _name;
}

void Servant::setHandle(TC_EpollServer::Handle* handle)
{
    _handle = handle;
}

TC_EpollServer::Handle* Servant::getHandle()
{
    return _handle;
}

int Servant::dispatch(TarsCurrentPtr current, vector<char> &buffer)
{
    int ret = TARSSERVERUNKNOWNERR;

    if (current->getFuncName() == "tars_ping")
    {
        TLOGINFO("[TARS][Servant::dispatch] tars_ping ok from [" << current->getIp() << ":" << current->getPort() << "]" << endl);

        ret = TARSSERVERSUCCESS;
    }
    else if (!current->getBindAdapter()->isTarsProtocol())
    {
        TC_LockT<TC_ThreadRecMutex> lock(*this);

        ret = doRequest(current, buffer);
    }
    else
    {
        TC_LockT<TC_ThreadRecMutex> lock(*this);

        ret = onDispatch(current, buffer);
    }
    return ret;
}

TC_ThreadQueue<ReqMessagePtr>& Servant::getResponseQueue()
{
    return _asyncResponseQueue;
}

///////////////////////////////////////////////////////////////////////////
ServantCallback::ServantCallback(const string& type, const ServantPtr& servant, const TarsCurrentPtr& current)
: _servant(servant)
, _current(current)
{
    ServantProxyCallback::setType(type);
}

int ServantCallback::onDispatch(ReqMessagePtr msg)
{
    _servant->getResponseQueue().push_back(msg);

    _servant->getHandle()->notifyFilter();

    return 0;
}

const ServantPtr& ServantCallback::getServant()
{
    return _servant;
}

const TarsCurrentPtr& ServantCallback::getCurrent()
{
    return _current;
}

///////////////////////////////////////////////////////////////////////////
CallbackThreadData::CallbackThreadData():_contextValid(false)
{
}

void CallbackThreadData::destructor(void* p)
{
    CallbackThreadData * pCbtd = (CallbackThreadData*)p;
    if(pCbtd)
        delete pCbtd;
}

CallbackThreadData * CallbackThreadData::getData()
{
    if(_key == 0)
    {
        TC_LockT<TC_ThreadMutex> lock(_mutex);
        if(_key == 0)
        {
            int iRet = ::pthread_key_create(&_key, CallbackThreadData::destructor);

            if (iRet != 0)
            {
                TLOGERROR("[TARS][CallbackThreadData pthread_key_create fail:" << errno << ":" << strerror(errno) << "]" << endl);
                return NULL;
            }
        }
    }

    CallbackThreadData * pCbtd = (CallbackThreadData*)pthread_getspecific(_key);

    if(!pCbtd)
    {
        TC_LockT<TC_ThreadMutex> lock(_mutex);

        pCbtd = new CallbackThreadData();

        int iRet = pthread_setspecific(_key, (void *)pCbtd);

        assert(iRet == 0);
    }
    return pCbtd;
}


void CallbackThreadData::setResponseContext(const map<std::string, std::string> & context)
{
    _contextValid = true;
    if(context.empty())
    {
        _responseContext.clear();
    }
    else
    {
        _responseContext = context;
    }
}

map<std::string, std::string> & CallbackThreadData::getResponseContext()
{
    return _responseContext;
}

void CallbackThreadData::delResponseContext()
{
    _contextValid = false;
    _responseContext.clear();
}

////////////////////////////////////////////////////////////////////////
}
