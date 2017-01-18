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

#ifndef __QUERY_DB_BY_HASH_THREAD_H_
#define __QUERY_DB_BY_HASH_THREAD_H_

#include <iostream>
#include <vector>
#include "util/tc_thread.h"
#include "util/tc_mysql.h"
#include "util/tc_common.h"
#include "util/tc_thread_queue.h"
#include "QueryItem.h"

using namespace tars;
using namespace std;

//////////////////////////////////////////////////////////////////////////////
class QueryDbThread;

//////////////////////////////////////////////////////////////////////////////
class HandleThreadRunner : public TC_Thread
{
public:

    HandleThreadRunner(QueryDbThread* proc);

    virtual void run();

    void terminate();
private:

    bool                    _terminate;

    QueryDbThread    *    _proc;

};
//////////////////////////////////////////////////////////////////////////////
class QueryDbThread
{
public:

    QueryDbThread();

    ~QueryDbThread();

    void terminate();

    void start(int iThreadNum);

    void put(QueryItem* pItem);

    bool pop(QueryItem* &pItem);

    size_t getQueueSize() { return _queue.size(); };

private:
    bool                        _terminate;

    TC_ThreadQueue<QueryItem*>    _queue;

    vector<HandleThreadRunner*>    _runners;
};

#endif
