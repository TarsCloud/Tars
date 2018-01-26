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

#ifndef __EXECUTE_TASK_H__
#define __EXECUTE_TASK_H__

#include <iostream>
#include "util/tc_thread_pool.h"
#include "util/tc_singleton.h"
#include "AdminReg.h"

using namespace tars;

class TaskList : public TC_ThreadMutex
{
public:
    /**
     * 构造函数
     */
    TaskList(const TaskReq &taskReq);

    /**
     * 得到返回任务
     */
    TaskRsp getTaskRsp();


    virtual void execute() = 0;
    /**
     * 创建时间
     */
    time_t getCreateTime() { return _createTime; }

protected:

    //设置应答信息
    void setRspInfo(size_t index, bool start, EMTaskItemStatus status);

    //设置应答的log
    void setRspLog(size_t index, const string &log);

    EMTaskItemStatus executeSingleTask(size_t index, const TaskItemReq &req);

    EMTaskItemStatus start   (const TaskItemReq &req, string &log);
    EMTaskItemStatus restart (const TaskItemReq &req, string &log);
    EMTaskItemStatus stop    (const TaskItemReq &req, string &log);
    EMTaskItemStatus patch   (const TaskItemReq &req, string &log);
    EMTaskItemStatus undeploy(const TaskItemReq &req, string &log);

    string get(const string &name, const map<string, string> &parameters);

protected:
    
    //线程池
    TC_ThreadPool   _pool;
    //请求任务
    TaskReq         _taskReq;
    //返回任务
    TaskRsp         _taskRsp;

    AdminRegPrx     _adminPrx;

    time_t          _createTime;
};

class TaskListSerial : public TaskList
{
public:
    TaskListSerial(const TaskReq &taskReq);

    virtual void execute();

protected:
    void doTask();
};

class TaskListParallel : public TaskList
{
public:
    TaskListParallel(const TaskReq &taskReq);

    virtual void execute();

protected:
    void doTask(TaskItemReq req, size_t index);
};

class ExecuteTask : public TC_Singleton<ExecuteTask>, public TC_ThreadLock, public TC_Thread
{
public:

    ExecuteTask();

    ~ExecuteTask();

    virtual void run();

    void terminate();

    /**
     * 添加任务请求
     * 
     * @param taskList 
     * @param serial 
     * @param current 
     * 
     * @return string
     */
    int addTaskReq(const TaskReq &taskReq);

    /**
     * 获取任务状态
     *
     * @param taskIdList : 任务列表id
     *
     * @return 任务状态
     */
    bool getTaskRsp(const string &taskNo, TaskRsp &taskRsp);

    /**
     * 检查task的状态
     * 
     * @param taskRsp 
     */
    void checkTaskRspStatus(TaskRsp &taskRsp);

protected:

    map<string, TaskList*> _task;

    //线程结束标志
    bool _terminate;
};

#endif
