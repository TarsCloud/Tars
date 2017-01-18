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

#ifndef __NODE_KEEPALIVE_THREAD_H_
#define __NODE_KEEPALIVE_THREAD_H_

#include "DbHandle.h"
#include "util/tc_thread_queue.h"
#include "util/tc_autoptr.h"
#include "Node.h"
#include "Registry.h"

using namespace tars;

//////////////////////////////////////////////////////

enum ProcCmd
{
    EM_REPORTVERSION,
    EM_NODE_KEEPALIVE,
    EM_UPDATEPATCHRESULT
};

//////////////////////////////////////////////////////

struct RegistryProcInfo
{
    ProcCmd  cmd;
    string   appName;
    string   serverName;
    string   nodeName;
    /*-------EM_REPORTVERSION-------------*/
    string   tarsVersion;
    /*-------EM_REPORTVERSION-------------*/
    string   patchVersion;
    string   patchUserName;
    /*-------EM_NODE_KEEPALIVE------------*/
    LoadInfo loadinfo;
};

//////////////////////////////////////////////////////

class RegistryProcThreadRunner;

typedef TC_AutoPtr<RegistryProcThreadRunner> RegistryProcThreadRunnerPtr;

//////////////////////////////////////////////////////
/**
 *  处理异步操作的线程池对象
 */
class RegistryProcThread : public TC_HandleBase
{
public:
    /**
     * 构造函数
     */
    RegistryProcThread();

    /**
     * 析构函数
     */
    virtual ~RegistryProcThread();

    /**
     * 停止运行
     */
    void terminate();

    /**
     * 插入请求
     */
    void put(const RegistryProcInfo& info);

    /**
     * 取请求
     */
    bool pop(RegistryProcInfo& info);

    /**
     * 启动处理请求线程
     */
    void start(int num=1);

    friend class RegistryProcThreadRunner;

protected:
    /*
     * 线程结束标志
     */
    bool                                 _terminate;

    /*
     * 请求队列
     */
    TC_ThreadQueue<RegistryProcInfo>     _queue;

    /*
     * 线程对象集合
     */
    vector<RegistryProcThreadRunnerPtr>  _runners;
};

typedef TC_AutoPtr<RegistryProcThread> RegistryProcThreadPtr;

//////////////////////////////////////////////////////
/**
 *  处理异步操作的线程
 */
class RegistryProcThreadRunner : public TC_Thread, public TC_HandleBase
{
public:
    /*
     * 构造函数
     */
    RegistryProcThreadRunner(RegistryProcThread* proc);

    /*
     * 线程执行函数
     */
    virtual void run();

    /*
     * 停止运行
     */
    void terminate();

private:
    /*
     * 线程结束标志
     */
    bool                               _terminate;

    /*
     * 线程池对象指针
     */
    RegistryProcThread *               _proc;

    /*
     * 访问主控db的处理类
     */
    CDbHandle                          _db;
};

#endif
