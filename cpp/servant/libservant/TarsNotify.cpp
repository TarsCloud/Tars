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

#include "servant/TarsNotify.h"
#include "servant/Communicator.h"
#include "servant/TarsLogger.h"

namespace tars
{

int TarsRemoteNotify::setNotifyInfo(const CommunicatorPtr &comm, const string &obj, const string & app, const string &serverName, const string &sSetName)
{
    _comm           = comm;
    if(!obj.empty())
    {
        _notifyPrx  = _comm->stringToProxy<NotifyPrx>(obj);
        _notifyPrx->tars_timeout(500);
    }

    _setName        = sSetName;
    _app            = app;
    _serverName     = serverName;

    return 0;
}

void TarsRemoteNotify::report(const string &sResult, bool bSync)
{
    try
    {
        if(_notifyPrx)
        {
            ReportInfo info;
            info.sApp      = _app;
            info.sServer   = _serverName;
            info.sSet      = _setName;
            info.sThreadId = TC_Common::tostr(pthread_self());
            info.sMessage  = sResult;
            if(!bSync)
            {
                //_notifyPrx->async_reportServer(NULL, _app + "." + _serverName, TC_Common::tostr(pthread_self()), sResult);
                _notifyPrx->async_reportNotifyInfo(NULL, info);
            }
            else
            {
                //_notifyPrx->reportServer(_app + "." + _serverName, TC_Common::tostr(pthread_self()), sResult);
                _notifyPrx->reportNotifyInfo(info);
            }
        }
    }
    catch(exception &ex)
    {
        TLOGERROR("TarsRemoteNotify::report error:" << ex.what() << endl);
    }
    catch(...)
    {
        TLOGERROR("TarsRemoteNotify::report unknown error" << endl);
    }
}

void TarsRemoteNotify::notify(NOTIFYLEVEL level, const string &sMessage)
{
    try
    {
        if(_notifyPrx)
        {
            ReportInfo info;
           // info.eType     = 0;
            info.sApp      = _app;
            info.sServer   = _serverName;
            info.sSet      = _setName;
            info.sThreadId = TC_Common::tostr(pthread_self());
            info.sMessage  = sMessage;
            info.eLevel    = level;
            //_notifyPrx->async_notifyServer(NULL, _app + "." + _serverName, level, sMessage);
            _notifyPrx->async_reportNotifyInfo(NULL, info);
        }
    }
    catch(exception &ex)
    {
        TLOGERROR("TarsRemoteNotify::notify error:" << ex.what() << endl);
    }
    catch(...)
    {
        TLOGERROR("TarsRemoteNotify::notify unknown error" << endl);
    }
}

}


