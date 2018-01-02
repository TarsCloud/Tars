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

#include "QueryDbThread.h"
#include "QueryServer.h"
#include "DbProxy.h"

/////////////////////////////////////////////////////////////////////////
QueryDbThread::QueryDbThread()
: _terminate(false)
{
    TLOGDEBUG("QueryDbThread init ok" << endl);
}

QueryDbThread::~QueryDbThread()
{
    terminate();

    TLOGDEBUG("QueryDbThread terminate." << endl);
}

void QueryDbThread::start(int iThreadNum)
{
    for(int i = 0; i < iThreadNum; ++i)
    {
        HandleThreadRunner *r = new HandleThreadRunner(this);

        r->start();

        _runners.push_back(r);
    }
}

void QueryDbThread::terminate()
{
    _terminate = true;

    for (uint32_t i = 0; i < _runners.size(); ++i)
    {
        if (_runners[i]->isAlive())
        {
            _runners[i]->terminate();

            _queue.notifyT();
        }
    }

    for (uint32_t i = 0; i < _runners.size(); ++i)
    {
        if(_runners[i]->isAlive())
        {
            _runners[i]->getThreadControl().join();
        }
    }

    _queue.clear();

    for (uint32_t i = 0; i < _runners.size(); ++i)
    {
        if(_runners[i])
        {
            delete _runners[i];
            _runners[i] = NULL;
        }
    }
}

bool QueryDbThread::pop(QueryItem* &pItem)
{
    return _queue.pop_front(pItem, 2000);
}

void QueryDbThread::put(QueryItem* pItem)
{
    if(!_terminate && pItem)
    {
        _queue.push_back(pItem);
    }
}

/////////////////////////////////////////////////////////////////////////
HandleThreadRunner::HandleThreadRunner(QueryDbThread* proc)
: _terminate(false)
, _proc(proc)
{
}

void HandleThreadRunner::terminate()
{
    _terminate = true;
}

void HandleThreadRunner::run()
{
    string sRes("");
    DbProxy    _dbproxy;
    int64_t tStart    = 0;
    int64_t tEnd    = 0;

    while (!_terminate)
    {
        QueryItem* pQueryItem = NULL;

        if(!_terminate && _proc->pop(pQueryItem))
        {
            try
            {
                tStart    = TNOWMS;

                _dbproxy.queryData(pQueryItem->mQuery, sRes, pQueryItem->bFlag);

                tEnd    = TNOWMS;

                sRes += "endline\n";

                pQueryItem->current->sendResponse(sRes.c_str(), sRes.length());

                FDLOG("inout") << "HandleThreadRunner::run sUid:" << pQueryItem->sUid << "queryData  timecost(ms):" << (tEnd - tStart) << endl;
            }
            catch(exception& ex)
            {
                TLOGERROR("HandleThreadRunner::run exception:" << ex.what() << endl);

                string sResult = "Ret:-1\n" +  string(ex.what()) + "\nendline\n";
                pQueryItem->current->sendResponse(sResult.c_str(), sResult.length());

                FDLOG("inout") << "HandleThreadRunner::run sUid:" << pQueryItem->sUid << "exception:" << ex.what() << endl;
            }
            catch(...)
            {
                TLOGERROR("HandleThreadRunner::run exception." << endl);

                string sResult = "Ret:-1\nunknown exception\nendline\n";
                pQueryItem->current->sendResponse(sResult.c_str(), sResult.length());

                FDLOG("inout") << "HandleThreadRunner::run sUid:" << pQueryItem->sUid << "unknown exception." << endl;
            }

            sRes = "";

            delete pQueryItem;
            pQueryItem = NULL;
        }
    }
}
