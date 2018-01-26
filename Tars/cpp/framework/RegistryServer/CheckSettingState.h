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

#ifndef __CHECKSETINGSTATE_H__
#define __CHECKSETINGSTATE_H__

#include <iostream>

#include "util/tc_thread.h"
#include "Node.h"
#include "DbHandle.h"

using namespace tars;

//////////////////////////////////////////////////////
/**
 * 用于执行定时操作的线程类
 */
class CheckSettingState : public TC_Thread, public TC_ThreadLock
{
public:

    /*
     * 构造函数
     */
    CheckSettingState();

    /*
     * 析构函数
     */
    ~CheckSettingState();

    /*
     * 结束线程
     */
    void terminate();

    /**
     * 初始化
     */
    int init();

    /**
     * 线程执行函数
     */
    virtual void run();

private:
    /*
     * 线程结束标志
     */
    bool            _terminate;

    /*
     * 数据库操作
     */
    CDbHandle       _db;

    /*
     * 轮询server状态的间隔时间
     */
    int             _checkingInterval;

    /*
     * 增量查询服务状态的时间,单位是秒
     */
    int             _leastChangedTime;
};

#endif
