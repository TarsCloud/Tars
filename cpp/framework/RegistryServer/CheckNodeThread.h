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

#ifndef __CHECK_NODE_THREAD_H__
#define __CHECK_NODE_THREAD_H__

#include <iostream>
#include "util/tc_thread.h"
#include "DbHandle.h"

using namespace tars;

//////////////////////////////////////////////////////
/**
 * 监控tarsnode超时的线程类
 */
class CheckNodeThread : public TC_Thread, public TC_ThreadLock
{
public:

    /**
     * 构造函数
     */
    CheckNodeThread();

    /**
     * 析构函数
     */
    ~CheckNodeThread();

    /**
     * 结束线程
     */
    void terminate();

    /**
     * 初始化
     */
    int init();

    /**
     * 轮询函数
     */
    virtual void run();

protected:
    /*
     * 线程结束标志
     */
    bool         _terminate;

    /*
     * 数据库操作
     */
    CDbHandle    _db;

    /*
     * node心跳超时时间
     */
    int          _nodeTimeout;

    /*
     * 线程检查周期
     */
    int          _nodeTimeoutInterval;

};

#endif
