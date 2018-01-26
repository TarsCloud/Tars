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
#include "util/tc_mysql.h"
#include "util/tc_config.h"
#include "servant/PropertyF.h"
#include "util/tc_common.h"
#include "servant/TarsLogger.h"
#include "PropertyImp.h"


using namespace tars;

extern TC_Config* g_pconf;

////////////////////////////////////////
class QueueItem
{
public:
    size_t            _index;
    string            _date;
    string            _tflag;
    PropertyMsg        *_statmsg;

    QueueItem()
    : _index(0)
    , _date("")
    , _tflag("")
    , _statmsg(NULL)
    {}
};
////////////////////////////////////////
class PropertyReapThread;
////////////////////////////////////////
/**
 * 向数据库插入数据的线程类
 */
class ReapSSDProcThread : public TC_Thread
{
public:
    enum
    {
        TIME_INTERVAL = 5000//更新业务线程时间
    };
    ReapSSDProcThread(PropertyReapThread * proc);

    ~ReapSSDProcThread();

    void terminate();

    virtual void run();

    void put(QueueItem data);

    bool pop(QueueItem & data);

    int sendAlarmSMS(const string &sMsg);

private:
    bool                            _bTerminate;

    PropertyReapThread *            _proc;

    TC_ThreadQueue<QueueItem>        _queue;
};
//////////////////////////////////////////////
/**
 * 用于执行定时操作的线程类
 */
class PropertyReapThread : public TC_Thread, public TC_ThreadLock
{
public:
     /**
     * 定义常量
     */
    enum
    {
        REAP_INTERVAL = 3000,
    };

    /**
     * 构造
     */
    PropertyReapThread();

    /**
     * 析够
     */
    ~PropertyReapThread();

    /**
     * 结束线程
     */
    void terminate();

    /**
     * 轮询函数
     */
    virtual void run();

private:
    /*
     *    从buffer中取数据
     */
    void getDataFromBuffer(int iIndex, vector<PropertyMsg*> &vAllStatSecMsg, uint64_t &iTotalNum);

    int sendAlarmSMS(const string &sMsg);

    /**
     * 通过权重轮询调度算法获取要插入数据的db index
     * @param iMaxDb db个数
     * @param iGcd  所有权重的最大公约数
     * @param iMaxW 最大权重值
     * @param vDbWeight 所有db的权重值
     *
     * @return int
     */
    int getIndexWithWeighted(int iMaxDb,int iGcd,int iMaxW,const vector<int>& vDbWeight);

private:
    bool                            _terminate;
    int                                _curWeight;
    int                                _lastSq;
    vector<ReapSSDProcThread*>        _runners;
};

#endif
