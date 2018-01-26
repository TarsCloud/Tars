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

#include "ReportMemThread.h"
#include "RegistryProxy.h"
#include "NodeRollLogger.h"
#include "util/tc_timeprovider.h"
#include "util.h"

ReportMemThread::ReportMemThread( )
{
    _shutDown          = false;
    //用旧配置
    _monitorInterval   = TC_Common::strto<int>(g_pconf->get("/tars/node/keepalive<monitorInterval>","2"));
    _monitorInterval   = _monitorInterval>10?10:(_monitorInterval<1?1:_monitorInterval);
}

ReportMemThread::~ReportMemThread()
{
    terminate();
}

void ReportMemThread::terminate()
{
    NODE_LOG("ReportMemThread")->debug()<<FILE_FUN<< endl;

    _shutDown = true;

    if(isAlive())
    {
        _lock.notifyAll();
        getThreadControl().join();
    }
}

bool ReportMemThread::timedWait(int millsecond)
{
    TC_ThreadLock::Lock lock(_lock);
    if(_shutDown)
    {
        return true;
    }
    return _lock.timedWait(millsecond);
}

void ReportMemThread::run()
{
    while (!_shutDown)
    {
        try
        {
            report();
        }
        catch(exception& e)
        {
            NODE_LOG("ReportMemThread")->error()<<FILE_FUN<<"catch exception|"<<e.what()<<endl;
        }
        catch(...)
        {
            NODE_LOG("ReportMemThread")->error()<<FILE_FUN<<"catch unkown exception|"<<endl;
        }

        timedWait(_monitorInterval*1000);
    }
}

void ReportMemThread::report()
{
    string sServerId;
    map<string, ServerGroup> mmServerList       = ServerFactory::getInstance()->getAllServers();
    map<string, ServerGroup>::const_iterator it = mmServerList.begin();
    for(;it != mmServerList.end(); it++)
    {
        map<string, ServerObjectPtr>::const_iterator  p = it->second.begin();
        for(;p != it->second.end(); p++)
        {
            try
            {
                sServerId   = it->first+"."+p->first;
                ServerObjectPtr pServerObjectPtr = p->second;
                if(!pServerObjectPtr)
                {
                    NODE_LOG("ReportMemThread")->debug()<<FILE_FUN<<sServerId<<"|=NULL|"<<endl;
                    continue;
                }
                pServerObjectPtr->reportMemProperty();

            }
            catch(exception &e)
            {
                NODE_LOG("ReportMemThread")->error()<<FILE_FUN<<sServerId<<" catch exception|"<<e.what() << endl;
            }
        }
    }
}
