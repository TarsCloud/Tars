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

#ifndef __REPORT_MEM_THREAD_H_
#define __REPORT_MEM_THREAD_H_

#include <unistd.h>
#include "ServerFactory.h"
#include "util/tc_monitor.h"
#include "util/tc_thread.h"

using namespace tars;
using namespace std;

class ReportMemThread : public TC_Thread//,public TC_ThreadLock
{
public:
    /**
     * 构造函数
     */
    ReportMemThread();

    /**
     * 析构函数
     */
    ~ReportMemThread();

    /**
     * 结束线程
     */
    void terminate();

protected:

    virtual void run();
    
    void report();

    bool timedWait(int millsecond);

protected:

    bool                _shutDown;
    int                 _monitorInterval;
    TC_ThreadLock        _lock;
};


#endif

