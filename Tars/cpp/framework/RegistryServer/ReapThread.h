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

#ifndef __REAP_THREAD_H__
#define __REAP_THREAD_H__

#include <iostream>
#include "util/tc_thread.h"
#include "DbHandle.h"

using namespace tars;

//////////////////////////////////////////////////////
/**
 * 用于执行定时操作的线程类
 */
class ReapThread : public TC_Thread, public TC_ThreadLock
{
public:
    /**
     * 构造函数
     */
    ReapThread();
    
    /**
     * 析构函数
     */
    ~ReapThread();

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
    bool       _terminate;

    /*
     * 数据库操作
     */
    CDbHandle  _db;

    /*
     * 加载对象列表的时间间隔,单位是秒
     * 第一阶段加载时间 consider
     */
    int        _loadObjectsInterval1;
    int        _leastChangedTime1;

    /*
     * 全量加载时间,单位是秒
     */
    int        _loadObjectsInterval2;
    int        _leastChangedTime2;
 
    /*
     * registry心跳超时时间
     */
    int        _registryTimeout;

    /*
     * 是否启用DB恢复保护功能，默认为打开
     */
    bool       _recoverProtect;

    /*
     * 启用DB恢复保护功能状态下极限值
     */
    int        _recoverProtectRate;

    /*
     * 主控心跳时间更新开关
     */
    bool       _heartBeatOff;
};

#endif
