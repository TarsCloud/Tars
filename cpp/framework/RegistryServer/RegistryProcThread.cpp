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

#include "RegistryProcThread.h"

using namespace tars;

//初始化数据库实例用
extern TC_Config * g_pconf;

//////////////////////////////////////////////////////
RegistryProcThread::RegistryProcThread()
: _terminate(false)
{
    TLOGDEBUG("RegistryProcThread init ok"<<endl);
}

RegistryProcThread::~RegistryProcThread()
{
   terminate();
}

void RegistryProcThread::terminate()
{
    _terminate = true;

    for (uint32_t i = 0; i < _runners.size(); ++i)
    {
        if (_runners[i]->isAlive())
        {
            _runners[i]->terminate();

            _queue.notifyT();
        }
    }

    for (uint32_t i = 0; i < _runners.size(); ++i)
    {
        if(_runners[i]->isAlive())
        {
            _runners[i]->getThreadControl().join();
        }
    }

    _queue.clear();

}

void RegistryProcThread::start(int num)
{
    for (int i = 0; i < num; ++i)
    {
        RegistryProcThreadRunnerPtr r = new RegistryProcThreadRunner(this);

        r->start();

        _runners.push_back(r);
    }
}

void RegistryProcThread::put(const RegistryProcInfo& info)
{
    if(!_terminate)
    {
        _queue.push_back(info);
    }
}

bool RegistryProcThread::pop(RegistryProcInfo& info)
{
    return _queue.pop_front(info,1000);
}

//////////////////////////////////////////////////////
RegistryProcThreadRunner::RegistryProcThreadRunner(RegistryProcThread* proc)
: _terminate(false)
, _proc(proc)
{
    _db.init(g_pconf);
}

void RegistryProcThreadRunner::terminate()
{
    _terminate = true;
}

void RegistryProcThreadRunner::run()
{
    while (!_terminate)
    {
        try
        {
            RegistryProcInfo info;
            if(_proc->pop(info) && !_terminate)
            {
                TLOGDEBUG("RegistryProcThreadRunner:run cmd:"<<info.cmd<<endl);

                if(info.cmd == EM_NODE_KEEPALIVE)
                {
                    _db.keepAlive(info.nodeName,info.loadinfo);
                }
                else if(info.cmd == EM_UPDATEPATCHRESULT)
                {
                    _db.setPatchInfo(info.appName,info.serverName,info.nodeName,info.patchVersion,info.patchUserName);
                }
                else if(info.cmd == EM_REPORTVERSION)
                {
                    _db.setServerTarsVersion(info.appName, info.serverName, info.nodeName, info.tarsVersion);
                }
                else
                {
                    TLOGERROR("RegistryProcThreadRunner:run cmd:" << info.cmd << "|no support." << endl);
                }
            }
        }
        catch (exception& e)
        {
            TLOGERROR("RegistryProcThreadRunner::run catch exception:"<<e.what()<<endl);
        }
        catch (...)
        {
            TLOGERROR("RegistryProcThreadRunner::run catch unkown exception."<<endl);
        }
    }
}

