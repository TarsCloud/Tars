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

#ifndef __REMOVE_LOG_THREAD_H_
#define __REMOVE_LOG_THREAD_H_

#include "servant/Application.h"
#include "Node.h"
#include "ServerObject.h"
#include "util/tc_thread_queue.h"

class RemoveLogThread;

class RemoveLogManager// : public tars::TC_ThreadLock
{
public:
    /**
    * 构造函数
    */
    RemoveLogManager();

    /**
    * 析构函数
    */
    ~RemoveLogManager();

    /**
     * 结束线程
     */
    void terminate();

    /**
     * 线程运行
     */
    void start(int iThreadNum);

    /**
     * 等待
     */
    void timedWait(int millsecond);

    /**
     * 插入发布请求
     */
    int push_back(const string& logPath);

    /**
     * 从发布队列中获取发布请求
     */
    bool pop_front(string& logPath);

private:
    tars::TC_ThreadLock                  _queueMutex;

    TC_ThreadQueue<string>               _reqQueue;

    std::set<string>                   _reqSet; //用于去重

    std::vector<RemoveLogThread *>     _runners;
};

class RemoveLogThread : public TC_Thread
{
public:
    RemoveLogThread(RemoveLogManager * manager);

    ~RemoveLogThread();

    virtual void run();

    void terminate();

protected:
    RemoveLogManager *    _manager;

    bool                  _shutDown; 
};

#endif
