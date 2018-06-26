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

#include "ExecuteTask.h"
#include "servant/Application.h"
#include "util/tc_timeprovider.h"

extern TC_Config * g_pconf;

TaskList::TaskList(const TaskReq &taskReq)
: _taskReq(taskReq)
{
    _adminPrx = CommunicatorFactory::getInstance()->getCommunicator()->stringToProxy<AdminRegPrx>(g_pconf->get("/tars/objname<AdminRegObjName>", ""));

    _taskRsp.taskNo   = _taskReq.taskNo;
    _taskRsp.serial   = _taskReq.serial;
    _taskRsp.userName = _taskReq.userName;

    for (size_t i=0; i < taskReq.taskItemReq.size(); i++)
    {
        TaskItemRsp rsp;
        rsp.req       = taskReq.taskItemReq[i];
        rsp.startTime = "";
        rsp.endTime   = "";
        rsp.status    = EM_I_NOT_START;
        rsp.statusInfo= etos(rsp.status);

        _taskRsp.taskItemRsp.push_back(rsp);
    }

    _createTime = TC_TimeProvider::getInstance()->getNow();
}


TaskRsp TaskList::getTaskRsp()
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    return _taskRsp;
}


void TaskList::setRspInfo(size_t index, bool start, EMTaskItemStatus status)
{
    TaskItemRsp rsp;
    map<string, string> info;
    {
        TC_LockT<TC_ThreadMutex> lock(*this);

        if (start)
        {
            _taskRsp.taskItemRsp[index].startTime = TC_Common::now2str("%Y-%m-%d %H:%M:%S"); 
            info["start_time"] = _taskRsp.taskItemRsp[index].startTime;
        }
        else
        {
            _taskRsp.taskItemRsp[index].endTime = TC_Common::now2str("%Y-%m-%d %H:%M:%S"); 
            info["end_time"] = _taskRsp.taskItemRsp[index].endTime;
        }

        _taskRsp.taskItemRsp[index].status    = status;
        info["status"] = TC_Common::tostr(_taskRsp.taskItemRsp[index].status);

        _taskRsp.taskItemRsp[index].statusInfo = etos(status); 

        rsp = _taskRsp.taskItemRsp[index];
    }

    try
    {
        _adminPrx->setTaskItemInfo(rsp.req.itemNo, info);
    }
    catch (exception &ex)
    {
//        log = ex.what();
        TLOGERROR("TaskList::setRspInfo error:" << ex.what() << endl);
    }
}

void TaskList::setRspLog(size_t index, const string &log)
{
    TaskItemRsp rsp;
    map<string, string> info;
    {
        TC_LockT<TC_ThreadMutex> lock(*this);

        _taskRsp.taskItemRsp[index].executeLog = log; 

        rsp = _taskRsp.taskItemRsp[index];

        info["log"] = _taskRsp.taskItemRsp[index].executeLog;
    }

    try
    {
        _adminPrx->setTaskItemInfo(rsp.req.itemNo, info);
    }
    catch (exception &ex)
    {
//        log = ex.what();
        TLOGERROR("TaskList::setRspLog error:" << ex.what() << endl);
    }
}

EMTaskItemStatus TaskList::start(const TaskItemReq &req, string &log)
{
    int ret = -1;
    try
    {
        ret = _adminPrx->startServer(req.application, req.serverName, req.nodeName, log);
        if (ret == 0)
            return EM_I_SUCCESS;
    }
    catch (exception &ex)
    {
        log = ex.what();
        TLOGERROR("TaskList::executeSingleTask startServer error:" << log << endl);
    }

    return EM_I_FAILED;
}

EMTaskItemStatus TaskList::restart(const TaskItemReq &req, string &log)
{
    int ret = -1;
    try
    {
        ret = _adminPrx->restartServer(req.application, req.serverName, req.nodeName, log);
        if (ret == 0)
            return EM_I_SUCCESS;
    }
    catch (exception &ex)
    {
        log = ex.what();
        TLOGERROR("TaskList::restartServer error:" << log << endl);
    }

    return EM_I_FAILED;
}

EMTaskItemStatus TaskList::undeploy(const TaskItemReq &req, string &log)
{
    int ret = -1;
    try
    {
        ret = _adminPrx->undeploy(req.application, req.serverName, req.nodeName, req.userName, log);
        if (ret == 0)
            return EM_I_SUCCESS;
    }
    catch (exception &ex)
    {
        log = ex.what();
        TLOGERROR("TaskList::undeploy error:" << log << endl);
    }

    return EM_I_FAILED;
}

EMTaskItemStatus TaskList::stop(const TaskItemReq &req, string &log)
{
    int ret = -1;
    try
    {
        ret = _adminPrx->stopServer(req.application, req.serverName, req.nodeName, log);
        if (ret == 0)
            return EM_I_SUCCESS;
    }
    catch (exception &ex)
    {
        log = ex.what();
        TLOGERROR("TaskList::stop error:" << log << endl);
    }

    return EM_I_FAILED;
}

string TaskList::get(const string &name, const map<string, string> &parameters)
{
    map<string, string>::const_iterator it = parameters.find(name);
    if (it == parameters.end())
    {
        return "";
    }
    return it->second;
}

EMTaskItemStatus TaskList::patch(const TaskItemReq &req, string &log)
{

    try
    {

        int ret = EM_TARS_UNKNOWN_ERR;

        TLOGDEBUG("TaskList::patch:" << TC_Common::tostr(req.parameters.begin(), req.parameters.end()) << endl);
        
        string patchId   = get("patch_id", req.parameters);
        string patchType = get("patch_type", req.parameters);

        tars::PatchRequest patchReq;
        patchReq.appname    = req.application;
        patchReq.servername = req.serverName;
        patchReq.nodename   = req.nodeName;
        patchReq.version    = patchId;
        patchReq.user       = req.userName;

        try
        {
            ret = _adminPrx->batchPatch(patchReq, log);
        }
        catch (exception &ex)
        {
            log = ex.what();
            TLOGERROR("TaskList::patch batchPatch error:" << log << endl);
            return EM_I_FAILED;
        }

        if (ret != EM_TARS_SUCCESS)
        {
            log = "batchPatch err:" + log;
            return EM_I_FAILED;
        }

        while (true)
        {
            PatchInfo pi;

            try
            {
                ret = _adminPrx->getPatchPercent(req.application, req.serverName, req.nodeName, pi);
            }
            catch (exception &ex)
            {
                log = ex.what();
                TLOGERROR("TaskList::patch getPatchPercent error, ret:" << ret << endl);
            }

            if (ret != 0)
            {
                _adminPrx->updatePatchLog(req.application, req.serverName, req.nodeName, patchId, req.userName, patchType, false);
                TLOGERROR("TaskList::patch getPatchPercent error, ret:" << ret << endl);
                return EM_I_FAILED;
            }

            if(pi.iPercent == 100 && pi.bSucc)
            {
                _adminPrx->updatePatchLog(req.application, req.serverName, req.nodeName, patchId, req.userName, patchType, true);
                TLOGDEBUG("TaskList::patch getPatchPercent ok, percent:" << pi.iPercent << "%" << endl); 
                break;
            }
            
            TLOGDEBUG("TaskList::patch getPatchPercent percent:" << pi.iPercent << "%, succ:" << pi.bSucc << endl); 

            sleep(1);
        }

    }
    catch (exception &ex)
    {
        TLOGERROR("TaskList::patch error:" << ex.what() << endl);
        return EM_I_FAILED;
    }
    return EM_I_SUCCESS; 
}

EMTaskItemStatus TaskList::executeSingleTask(size_t index, const TaskItemReq &req)
{
    TLOGDEBUG("TaskList::executeSingleTask: taskNo=" << req.taskNo 
            << ",application=" << req.application 
            << ",serverName="  << req.serverName 
            << ",nodeName="    << req.nodeName
            << ",setName="     << req.setName 
            << ",command="     << req.command << endl);

    EMTaskItemStatus ret = EM_I_FAILED;
    string log;
    if (req.command == "stop")
    {
        ret = stop(req, log);
    }
    else if (req.command == "start")
    {
        ret = start(req, log);
    }
    else if (req.command == "restart")
    {
        ret = restart(req, log);
    }
    else if (req.command == "patch_tars")
    {
        ret = patch(req, log);
        if (ret == EM_I_SUCCESS && get("bak_flag", req.parameters) != "1")
        {
            //不是备机, 需要重启
            ret = restart(req, log); 
        }
    }
    else if (req.command == "undeploy_tars")
    {
        ret = undeploy(req, log);
    }
    else 
    {
        ret = EM_I_FAILED;
        log = "command not support!";
        TLOGDEBUG("TaskList::executeSingleTask command not support!" << endl);
    }

    setRspLog(index, log);

    return ret; 
}

//////////////////////////////////////////////////////////////////////////////
TaskListSerial::TaskListSerial(const TaskReq &taskReq)
: TaskList(taskReq)
{
    _pool.init(1);
    _pool.start();
}

void TaskListSerial::execute()
{
    TLOGDEBUG("TaskListSerial::execute" << endl);
    
    auto cmd = std::bind(&TaskListSerial::doTask, this);
    _pool.exec(cmd);
}

void TaskListSerial::doTask()
{
    size_t len = 0;
    {
        TC_LockT<TC_ThreadMutex> lock(*this);
        len = _taskReq.taskItemReq.size();
    }

    EMTaskItemStatus status = EM_I_SUCCESS;

    for (size_t i=0; i < len; i++)
    {
        TaskItemReq req;

        setRspInfo(i, true, EM_I_RUNNING);
        {
            TC_LockT<TC_ThreadMutex> lock(*this);

            req = _taskReq.taskItemReq[i];
        }

        if (EM_I_SUCCESS == status)
        {
            status = executeSingleTask(i, req); 
        }
        else
        {
            //上一个任务不成功, 后续的任务都cancel掉
            status = EM_I_CANCEL;
        }

        setRspInfo(i, false, status);
    }
}

//////////////////////////////////////////////////////////////////////////////
TaskListParallel::TaskListParallel(const TaskReq &taskReq)
: TaskList(taskReq)
{
    //最大并行线程数
    size_t num = taskReq.taskItemReq.size() > 10 ? 10 : taskReq.taskItemReq.size();
    _pool.init(num); 
    _pool.start();
}

void TaskListParallel::execute()
{
    TLOGDEBUG("TaskListParallel::execute" << endl);
    
    TC_LockT<TC_ThreadMutex> lock(*this);

    for (size_t i=0; i < _taskReq.taskItemReq.size(); i++)
    {
        auto cmd = std::bind(&TaskListParallel::doTask, this, _taskReq.taskItemReq[i], i);
        _pool.exec(cmd);
    }
}

void TaskListParallel::doTask(TaskItemReq req, size_t index)
{
    setRspInfo(index, true, EM_I_RUNNING);

    //do work
    TLOGDEBUG("TaskListParallel::executeTask: taskNo=" << req.taskNo 
                << ",application=" << req.application 
                << ",serverName="  << req.serverName 
                << ",setName="     << req.setName 
                << ",command="     << req.command << endl);

    EMTaskItemStatus status = executeSingleTask(index, req); 

    setRspInfo(index, false, status);
}

/////////////////////////////////////////////////////////////////////////////

ExecuteTask::ExecuteTask()
{
    _terminate = false;
    start();
}

ExecuteTask::~ExecuteTask()
{
    terminate();
}

void ExecuteTask::terminate()
{
    _terminate = true;
    TC_LockT<TC_ThreadLock> lock(*this);
    notifyAll();
}

void ExecuteTask::run()
{
    const time_t diff = 2*60;//2分钟
    while (!_terminate)
    {
        {
            TC_ThreadLock::Lock lock(*this);
            map<string, TaskList* >::iterator it = _task.begin();
            while (it != _task.end())
            {
                if(TC_TimeProvider::getInstance()->getNow() - it->second->getCreateTime() > diff)
                {
                    TLOGDEBUG("==============ExecuteTask::run, delete old task, taskNo=" << it->first << endl);
                    TaskList *tmp = it->second;
                    _task.erase(it++);
                    delete tmp;
                }
                else
                {
                    ++it;
                }
            }
        }

        {
            TC_LockT<TC_ThreadLock> lock(*this);
            timedWait(5*1000);
        }
    }
}


int ExecuteTask::addTaskReq(const TaskReq &taskReq)
{
    TLOGDEBUG("ExecuteTask::addTaskReq" <<
              ", taskNo="  << taskReq.taskNo <<   
              ", size="    << taskReq.taskItemReq.size() <<
              ", serial="  << taskReq.serial <<
              ", userName="<< taskReq.userName << endl);

    TaskList *p = NULL;
    if (taskReq.serial)
    {
        p = new TaskListSerial(taskReq);
    }
    else
    {
        p = new TaskListParallel(taskReq);
    }

    {
        TC_ThreadLock::Lock lock(*this);

        _task[taskReq.taskNo] = p;

        //删除历史数据, 当然最好是在定时独立做, 放在这里主要是途方便
        map<string, TaskList*>::iterator it = _task.begin();
        while (it != _task.end())
        {
            static time_t diff = 24 *60 *60;//1天
            //大于两天数据任务就干掉
            if(TC_TimeProvider::getInstance()->getNow() - it->second->getCreateTime() > diff)
            {
                TaskList *temp = it->second;

                _task.erase(it++);

                delete temp;
                temp = NULL;
            }
            else
            {
                ++it;
            }
        }
    }

    p->execute();

    return 0;
}

bool ExecuteTask::getTaskRsp(const string &taskNo, TaskRsp &taskRsp)
{
    TLOGDEBUG("ExecuteTask::getTaskRsp, taskNo=" << taskNo << endl);

    TC_ThreadLock::Lock lock(*this);

    map<string, TaskList*>::iterator it = _task.find(taskNo);

    if( it == _task.end())
    {
        return false;
    }

    taskRsp = (it->second)->getTaskRsp();

    ExecuteTask::getInstance()->checkTaskRspStatus(taskRsp);

    return true;
}


void ExecuteTask::checkTaskRspStatus(TaskRsp &taskRsp) 
{
    size_t not_start = 0;
    size_t running   = 0;
    size_t success   = 0;
    size_t failed    = 0;
    size_t cancel    = 0;

    for (unsigned i = 0; i < taskRsp.taskItemRsp.size(); i++) 
    {
        TaskItemRsp &rsp = taskRsp.taskItemRsp[i];

        switch (rsp.status)
        {
        case EM_I_NOT_START:
            ++not_start;
            break;
        case EM_I_RUNNING:
            ++running;
            break;
        case EM_I_SUCCESS:
            ++success;
            break;
        case EM_I_FAILED:
            ++failed;
            break;
        case EM_I_CANCEL:
            ++cancel;
            break;
        }
    }
    
    if      (not_start == taskRsp.taskItemRsp.size()) taskRsp.status = EM_T_NOT_START;
    else if (running   == taskRsp.taskItemRsp.size()) taskRsp.status = EM_T_RUNNING;
    else if (success   == taskRsp.taskItemRsp.size()) taskRsp.status = EM_T_SUCCESS;
    else if (failed    == taskRsp.taskItemRsp.size()) taskRsp.status = EM_T_FAILED;
    else if (cancel    == taskRsp.taskItemRsp.size()) taskRsp.status = EM_T_CANCEL;
    else taskRsp.status = EM_T_PARIAL;

}



