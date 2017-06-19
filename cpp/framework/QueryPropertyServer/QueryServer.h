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

#ifndef _QueryServer_H_
#define _QueryServer_H_

#include <iostream>
#include <set>
#include "servant/Application.h"
#include "util/tc_thread_mutex.h"
#include "util/tc_lock.h"
#include "util/tc_thread_pool.h"
#include "util/tc_atomic.h"
#include "DbThread.h"
#include "QueryDbThread.h"

using namespace std;
using namespace tars;

/////////////////////////////////////////////////////////////////////
class QueryParam
{
public:
    int _run_times;
    int _run_result;
    TC_Atomic _atomic;
    TC_ThreadLock _monitor;

    QueryParam()
    : _run_times(0)
    , _run_result(0)
    , _atomic(0)
    {}
};
/////////////////////////////////////////////////////////////////////
class QueryServer : public Application , public TC_ThreadMutex
{
public:
    /**
     *
     **/
    virtual ~QueryServer() {};

    /**
     *
     **/
    virtual void initialize();

    /**
     *
     **/
    virtual void destroyApp();

    int getInsertInterval() const { return _insertInterval; };

    size_t getDbNumber() const { return _dbStatInfo.size(); }

    size_t getActiveDbSize() const { return _activeDbInfo.size(); }

    void setActiveDb( const vector<TC_DBConf> &vDbInfo);

    vector<TC_DBConf> getAllActiveDbInfo() const;

    vector<TC_DBConf> getDbInfo() const;

    vector<TC_DBConf> getActiveDbInfo() const;

    TC_DBConf getDbInfo(int iIndex) { return _dbStatInfo[iIndex]; }

    string dumpDbInfo(const vector<TC_DBConf>& vDbInfo) const;

    uint32_t genUid();

    TC_ThreadPool & getThreadPoolTimeCheck() { return _timeCheck; }

    TC_ThreadPool & getThreadPoolDb() { return _poolDb; }

    QueryDbThread * getThreadPoolQueryDb() { return _tpoolQueryDb; }

    bool searchQueryFlag(const string &sKey);

    //匹配非tars被调服务名
    set<string>& getNotTarsSlaveName();

private:
    uint32_t            _uniqId;                //唯一id

    int                    _insertInterval;        //查询最后入库时间使用，入库耗时不会超过入库间隔,单位为分钟

    DBThread *            _dBThread;                //定时检查数据库实例是否存活

    QueryDbThread *_tpoolQueryDb;         //用于处理数据库的查询操作

    vector<TC_DBConf>    _dbStatInfo;            //数据库信息

    vector<TC_DBConf>    _activeDbInfo;        //存活的数据库信息

    set<string>            _queryFlag;             //数据库字段

    TC_ThreadPool        _timeCheck;                //处理timecheck操作的线程池

    TC_ThreadPool        _poolDb;             //具体查询压缩维度后的数据库实例数据的线程池

    
    set<string>            _notTarsSlaveName;        //匹配非tars被调服务名
};

extern QueryServer g_app;
extern TC_Config * g_pconf;
////////////////////////////////////////////
#endif
