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

#include "util/tc_thread_pool.h"
#include "util/tc_common.h"

#include <iostream>

namespace tars
{

TC_ThreadPool::ThreadWorker::ThreadWorker(TC_ThreadPool *tpool)
: _tpool(tpool)
, _bTerminate(false)
{
}

void TC_ThreadPool::ThreadWorker::terminate()
{
    _bTerminate = true;
    _tpool->notifyT();
}

void TC_ThreadPool::ThreadWorker::run()
{
    //调用初始化部分
    auto pst = _tpool->get();
    if(pst)
    {
        try
        {
            pst();
        }
        catch ( ... )
        {
        }
    }

    //调用处理部分
    while (!_bTerminate)
    {
        auto pfw = _tpool->get(this);
        if(pfw)
        {
            try
            {
                pfw();
            }
            catch ( ... )
            {
            }

            _tpool->idle(this);
        }
    }

    //结束
    _tpool->exit();
}

//////////////////////////////////////////////////////////////
//
//

TC_ThreadPool::KeyInitialize TC_ThreadPool::g_key_initialize;
pthread_key_t TC_ThreadPool::g_key;

void TC_ThreadPool::destructor(void *p)
{
    ThreadData *ttd = (ThreadData*)p;
    if(ttd)
    {
        delete ttd;
    }
}

void TC_ThreadPool::exit()
{
    TC_ThreadPool::ThreadData *p = getThreadData();
    if(p)
    {
        delete p;
        int ret = pthread_setspecific(g_key, NULL);
        if(ret != 0)
        {
            throw TC_ThreadPool_Exception("[TC_ThreadPool::setThreadData] pthread_setspecific error", ret);
        }
    }

    _jobqueue.clear();
}

void TC_ThreadPool::setThreadData(TC_ThreadPool::ThreadData *p)
{
    TC_ThreadPool::ThreadData *pOld = getThreadData();
    if(pOld != NULL && pOld != p)
    {
        delete pOld;
    }

    int ret = pthread_setspecific(g_key, (void *)p);
    if(ret != 0)
    {
        throw TC_ThreadPool_Exception("[TC_ThreadPool::setThreadData] pthread_setspecific error", ret);
    }
}

TC_ThreadPool::ThreadData* TC_ThreadPool::getThreadData()
{
    return (ThreadData *)pthread_getspecific(g_key);
}

void TC_ThreadPool::setThreadData(pthread_key_t pkey, ThreadData *p)
{
    TC_ThreadPool::ThreadData *pOld = getThreadData(pkey);
    if(pOld != NULL && pOld != p)
    {
        delete pOld;
    }

    int ret = pthread_setspecific(pkey, (void *)p);
    if(ret != 0)
    {
        throw TC_ThreadPool_Exception("[TC_ThreadPool::setThreadData] pthread_setspecific error", ret);
    }
}

TC_ThreadPool::ThreadData* TC_ThreadPool::getThreadData(pthread_key_t pkey)
{
    return (ThreadData *)pthread_getspecific(pkey);
}

TC_ThreadPool::TC_ThreadPool()
: _bAllDone(true)
{
}

TC_ThreadPool::~TC_ThreadPool()
{
    stop();
    clear();
}

void TC_ThreadPool::clear()
{
    std::vector<ThreadWorker*>::iterator it = _jobthread.begin();
    while(it != _jobthread.end())
    {
        delete (*it);
        ++it;
    }

    _jobthread.clear();
    _busthread.clear();
}

void TC_ThreadPool::init(size_t num)
{
    stop();

    Lock sync(*this);

    clear();

    for(size_t i = 0; i < num; i++)
    {
        _jobthread.push_back(new ThreadWorker(this));
    }
}

void TC_ThreadPool::stop()
{
    Lock sync(*this);

    std::vector<ThreadWorker*>::iterator it = _jobthread.begin();
    while(it != _jobthread.end())
    {
        if ((*it)->isAlive())
        {
            (*it)->terminate();
            (*it)->getThreadControl().join();
        }
        ++it;
    }
    _bAllDone = true;
}

void TC_ThreadPool::start()
{
    Lock sync(*this);

    std::vector<ThreadWorker*>::iterator it = _jobthread.begin();
    while(it != _jobthread.end())
    {
        (*it)->start();
        ++it;
    }
    _bAllDone = false;
}

bool TC_ThreadPool::finish()
{
    return _startqueue.empty() && _jobqueue.empty() && _busthread.empty() && _bAllDone;
}

bool TC_ThreadPool::waitForAllDone(int millsecond)
{
    Lock sync(_tmutex);

start1:
    //任务队列和繁忙线程都是空的
    if (finish())
    {
        return true;
    }

    //永远等待
    if(millsecond < 0)
    {
        _tmutex.timedWait(1000);
        goto start1;
    }

    int64_t iNow= TC_Common::now2ms();
    int m       = millsecond;
start2:

    bool b = _tmutex.timedWait(millsecond);
    //完成处理了
    if(finish())
    {
        return true;
    }

    if(!b)
    {
        return false;
    }

    millsecond = max((int64_t)0, m  - (TC_Common::now2ms() - iNow));
    goto start2;

    return false;
}

std::function<void ()> TC_ThreadPool::get(ThreadWorker *ptw)
{
    std::function<void ()> res;
    if(!_jobqueue.pop_front(res, 1000))
    {
        return NULL;
    }

    {
        Lock sync(_tmutex);
        _busthread.insert(ptw);
    }

    return res;
}

std::function<void ()> TC_ThreadPool::get()
{
    std::function<void ()> res;
    if(!_startqueue.pop_front(res))
    {
        return NULL;
    }

    return res;
}

void TC_ThreadPool::idle(ThreadWorker *ptw)
{
    Lock sync(_tmutex);
    _busthread.erase(ptw);

    //无繁忙线程, 通知等待在线程池结束的线程醒过来
    if(_busthread.empty())
    {
        _bAllDone = true;
        _tmutex.notifyAll();
    }
}

void TC_ThreadPool::notifyT()
{
    _jobqueue.notifyT();
}



}
