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

#ifndef __DB_THREAD_H_
#define __DB_THREAD_H_
#include <iostream>
#include "util/tc_thread.h"
#include "util/tc_mysql.h"
#include "util/tc_common.h"
#include "servant/TarsLogger.h"


using namespace tars;

/**
 * 用于执行定时操作的线程类
 */
class DBThread : public TC_Thread, public TC_ThreadLock
{
public:
     /**
     * 定义常量
     */
    enum
    {
        REAP_INTERVAL = 30000
    };
    /**
     * 构造
     */
    DBThread();

    /**
     * 析够
     */
    ~DBThread();

    /**
     * 结束线程
     */
    void terminate();

    /**
     * 轮询函数
     */
    virtual void run();

    void tryConnect();

    int connect(MYSQL **_pstMql,const TC_DBConf& tcConf);

    void sendAlarmSMS(const string &sMsg);

private:
    bool _bTerminate;

};

#endif
