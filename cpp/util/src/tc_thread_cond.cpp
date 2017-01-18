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

#include "util/tc_thread_cond.h"
#include "util/tc_timeprovider.h"
#include <string.h>
#include <cassert>
#include <iostream>

using namespace std;


namespace tars
{

TC_ThreadCond::TC_ThreadCond()
{
    int rc;

    pthread_condattr_t attr;

    rc = pthread_condattr_init(&attr);
    if(rc != 0)
    {
        throw TC_ThreadCond_Exception("[TC_ThreadCond::TC_ThreadCond] pthread_condattr_init error", errno);
    }

    rc = pthread_cond_init(&_cond, &attr);
    if(rc != 0)
    {
        throw TC_ThreadCond_Exception("[TC_ThreadCond::TC_ThreadCond] pthread_cond_init error", errno);
    }

    rc = pthread_condattr_destroy(&attr);
    if(rc != 0)
    {
        throw TC_ThreadCond_Exception("[TC_ThreadCond::TC_ThreadCond] pthread_condattr_destroy error", errno);
    }
}

TC_ThreadCond::~TC_ThreadCond()
{
    int rc = 0;
    rc = pthread_cond_destroy(&_cond);
    if(rc != 0)
    {
        cerr << "[TC_ThreadCond::~TC_ThreadCond] pthread_cond_destroy error:" << string(strerror(rc)) << endl;
    }
//    assert(rc == 0);
}

void TC_ThreadCond::signal()
{
    int rc = pthread_cond_signal(&_cond);
    if(rc != 0)
    {
        throw TC_ThreadCond_Exception("[TC_ThreadCond::signal] pthread_cond_signal error", errno);
    }
}

void TC_ThreadCond::broadcast()
{
    int rc = pthread_cond_broadcast(&_cond);
    if(rc != 0)
    {
        throw TC_ThreadCond_Exception("[TC_ThreadCond::broadcast] pthread_cond_broadcast error", errno);
    }
}

timespec TC_ThreadCond::abstime( int millsecond) const
{
    struct timeval tv;

    //gettimeofday(&tv, 0);
    TC_TimeProvider::getInstance()->getNow(&tv);

    int64_t it  = tv.tv_sec * (int64_t)1000000 + tv.tv_usec + (int64_t)millsecond * 1000;

    tv.tv_sec   = it / (int64_t)1000000;
    tv.tv_usec  = it % (int64_t)1000000;

    timespec ts;
    ts.tv_sec   = tv.tv_sec;
    ts.tv_nsec  = tv.tv_usec * 1000; 
      
    return ts; 
}


}

