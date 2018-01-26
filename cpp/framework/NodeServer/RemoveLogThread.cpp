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

#include "util/tc_md5.h"
#include "RemoveLogThread.h"
#include "ServerFactory.h"
#include "NodeRollLogger.h"
#include "util/tc_timeprovider.h"
#include "util.h"

using namespace tars;

RemoveLogManager::RemoveLogManager()
{
}

RemoveLogManager::~RemoveLogManager()
{
    terminate();
}

void RemoveLogManager::start(int iNum)
{
    for (int i = 0; i < iNum; i++)
    {
        RemoveLogThread * t = new RemoveLogThread(this);
        t->start();

        _runners.push_back(t);
    }
}

void RemoveLogManager::terminate()
{
    for (size_t i = 0; i < _runners.size(); ++i)
    {
        if(_runners[i]->isAlive())
        {
            _runners[i]->terminate();
        }
    }

    for (size_t i = 0; i < _runners.size(); ++i)
    {
        if(_runners[i]->isAlive())
        {
            _runners[i]->getThreadControl().join();
        }
    }

    _queueMutex.notifyAll();
}

int RemoveLogManager::push_back(const string& logPath)
{
    {
        TC_ThreadLock::Lock LockQueue(_queueMutex);

        if (_reqSet.count(logPath) == 1)
        {
            return -1;
        }

        _reqQueue.push_back(logPath);
        _reqSet.insert(logPath);
    }

    return 0;
}

bool RemoveLogManager::pop_front(string& logPath)
{
    TC_ThreadLock::Lock LockQueue(_queueMutex);

    bool bRet=false;

    bRet=_reqQueue.pop_front(logPath);

    if(bRet)
    {
        _reqSet.erase(logPath);
    }
    
    return bRet;
}

void RemoveLogManager::timedWait(int millsecond)
{
    TC_ThreadLock::Lock lock(_queueMutex);

    _queueMutex.timedWait(millsecond);
}

/////////////////////////////////////////
RemoveLogThread::RemoveLogThread(RemoveLogManager * manager)
: _manager(manager)
, _shutDown(false)
{
}

RemoveLogThread::~RemoveLogThread()
{
    terminate();
}

void RemoveLogThread::terminate()
{
    _shutDown = true;

    if (isAlive())
    {
        getThreadControl().join();
    }
}

void RemoveLogThread::run()
{
    while (!_shutDown)
    {
        try
        {
            string sLogPath;
            if (_manager->pop_front(sLogPath))
            {
                int64_t startMs = TC_TimeProvider::getInstance()->getNowMs();
                if (TC_File::isFileExistEx(sLogPath, S_IFDIR))
                {
                    int ret = TC_File::removeFile(sLogPath,true);
                    if (ret == 0)
                    {
                        NODE_LOG("RemoveLogThread")->debug() <<FILE_FUN<< "remove log path success:" << sLogPath << ", use:" << (TC_TimeProvider::getInstance()->getNowMs() - startMs) << endl;
                    }
                    else
                    {
                        NODE_LOG("RemoveLogThread")->error()<<FILE_FUN << "failed to remove log path:" << sLogPath << ", use:" << (TC_TimeProvider::getInstance()->getNowMs() - startMs) << endl;
                    }
                }
                else
                {
                    NODE_LOG("RemoveLogThread")->debug()<<FILE_FUN << "log path does not exist:" << sLogPath << ", use:" << (TC_TimeProvider::getInstance()->getNowMs() - startMs) << endl;
                }
            }
            else
            {
                _manager->timedWait(2000);
            }
            
        }
        catch (exception& e)
        {
            NODE_LOG("RemoveLogThread")->error()<<FILE_FUN<<"catch exception|"<<e.what()<<endl;
        }
        catch (...)
        {
            NODE_LOG("RemoveLogThread")->error()<<FILE_FUN<<"catch unkown exception|"<<endl;
        }
    }
}

