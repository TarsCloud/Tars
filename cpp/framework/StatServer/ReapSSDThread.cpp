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

#include "ReapSSDThread.h"
#include "util/tc_config.h"
#include "StatServer.h"


ReapSSDProcThread::ReapSSDProcThread(ReapSSDThread * proc)
: _terminate(false)
, _proc(proc)
{
}

ReapSSDProcThread::~ReapSSDProcThread()
{
    if(isAlive())
    {
        terminate();
        getThreadControl().join();
    }
    _queue.clear();
}

void ReapSSDProcThread::terminate()
{
    _terminate = true;

    _queue.notifyT();
}

void ReapSSDProcThread::put(QueueItem data)
{
    if(!_terminate)
    {
        _queue.push_back(data);
    }
}

bool ReapSSDProcThread::pop(QueueItem & data)
{
    return _queue.pop_front(data, 1000);
}

int ReapSSDProcThread::sendAlarmSMS(const string &sMsg)
{
    string errInfo = " ERROR:" + ServerConfig::LocalIp + "_" + sMsg;
    TARS_NOTIFY_ERROR(errInfo);

    return 0;
}

void ReapSSDProcThread::run()
{
    string sDate1("");
    string sFlag1("");
    string sDate2("");
    string sFlag2("");

    while (!_terminate)
    {
        try
        {
            sDate1 = "";
            sFlag1 = "";
            sDate2 = "";
            sFlag2 = "";

            QueueItem item;

            if(pop(item))
            {
                if(item._statmsg != NULL)
                {
                    int64_t iBegin = TNOWMS;
                    int64_t iEnd = 0;

                    StatDbManager::getInstance()->insert2MultiDbs(item._index, *item._statmsg, item._date, item._tflag);

                    iEnd = TNOWMS;

                    TLOGDEBUG("ReapSSDProcThread::run stat ip:" << ServerConfig::LocalIp  << "|dbIndex:" << item._index << "|" << StatDbManager::getInstance()->getIpAndPort(item._index) 
                        << "|date:" << item._date << "|tflag:" << item._tflag << "|timecost(ms):" << (iEnd - iBegin) << "|iBegin:" << iBegin << "|iEnd:" << iEnd << endl);

                    FDLOG("CountStat") << "ReapSSDProcThread::run stat ip:" << ServerConfig::LocalIp  << "|dbIndex:" << item._index << "|" << StatDbManager::getInstance()->getIpAndPort(item._index) 
                        << "|date:" << item._date << "|tflag:" << item._tflag << "|timecost(ms):" << (iEnd - iBegin) << "|iBegin:" << iBegin << "|iEnd:" << iEnd << endl;

                    if((iEnd - iBegin)/1000 > (g_app.getInserInterv() - 2) * 60)
                    {
                        FDLOG("CountStat") << "stat ip:" << ServerConfig::LocalIp << "|ReapSSDProcThread::run timeout 8 minute." << endl;
                        string sMsg("stat ip:");
                        sMsg += ServerConfig::LocalIp;
                        sMsg += " ReapSSDProcThread::run write db:";
                        sMsg += StatDbManager::getInstance()->getIpAndPort(item._index);
                        sMsg += "|timeout ";
                        sMsg += TC_Common::tostr(g_app.getInserInterv() - 2);
                        sMsg += " Minute.";

                        sendAlarmSMS(sMsg);
                    }

                    delete item._statmsg;
                    item._statmsg = NULL;
                }
                else
                {
                    TLOGERROR("ReapSSDProcThread::run item._statmsg == NULL." << endl);
                }
            }
        }
        catch(exception& e)
        {
            TLOGERROR("ReapSSDProcThread::run exception:" << e.what() << endl);
            FDLOG("CountStat") << "ReapSSDProcThread::run exception:" << e.what() << endl;
        }
    }
}
//////////////////////////////////////////////////////////////
ReapSSDThread::ReapSSDThread()
:  _terminate(false)
, _curWeight(0)
, _lastSq(-1)
{
    TLOGDEBUG("ReapSSDThread begin ok" << endl);
}

ReapSSDThread::~ReapSSDThread()
{
    if (isAlive())
    {
        terminate();

        getThreadControl().join();
    }
}

void ReapSSDThread::terminate()
{
    TLOGDEBUG("ReapSSDThread terminate." << endl);

    _terminate = true;

    TC_ThreadLock::Lock lock(*this);

    notifyAll();
}

void ReapSSDThread::run()
{
    int iInsertDataNum = StatDbManager::getInstance()->getDbIpNum();

    for(int i = 0; i < iInsertDataNum; ++i)
    {
        ReapSSDProcThread *r = new ReapSSDProcThread(this);

        r->start();

        _runners.push_back(r);
    }

    string sDate,sTime;

    int dbNumber = StatDbManager::getInstance()->getDbNumber();

    string sRandOrder;

    uint64_t iTotalNum = 0;

    int iLastIndex = -1;

    FDLOG("CountStat") << "stat ip:" << ServerConfig::LocalIp << "|ReapSSDThread::run iInsertDataThread:" << iInsertDataNum << "|dbNumber:" << dbNumber << endl;

    while (!_terminate)
    {
        try
        {
            //双buffer中一个buffer入库
            int iBufferIndex = !(g_app.getSelectBufferIndex());
            int64_t iInterval = 3;
            if(iBufferIndex != iLastIndex && g_app.getSelectBuffer(iBufferIndex, iInterval))
            {
                iLastIndex = iBufferIndex;

                iTotalNum = 0;

                vector<StatMsg*> vAllStatMsg;
                for(int iStatIndex = 0; iStatIndex < dbNumber; ++iStatIndex)
                {
                    vAllStatMsg.push_back(new StatMsg());
                }

                int64_t tBegin = TNOWMS;

                getDataFromBuffer(iBufferIndex, vAllStatMsg, iTotalNum);

                int64_t tEnd = TNOWMS;

                TLOGDEBUG("stat ip:" << ServerConfig::LocalIp << "|Buffer Index:" << iBufferIndex << "|ReapSSDThread::run getDataFromBuffer timecost(ms):" << (tEnd - tBegin) << endl);
                FDLOG("CountStat") << "stat ip:" << ServerConfig::LocalIp << "|Buffer Index:" << iBufferIndex << "|ReapSSDThread::run getDataFromBuffer timecost(ms):" << (tEnd - tBegin) << endl;

                TLOGDEBUG("stat ip:" << ServerConfig::LocalIp << "|Buffer Index:" << iBufferIndex << "|ReapSSDThread::run insert begin _vAllStatMsg.size:" << vAllStatMsg.size() << "|record num:" << iTotalNum << endl);
                FDLOG("CountStat") << "stat ip:" << ServerConfig::LocalIp << "|Buffer Index:" << iBufferIndex << "|ReapSSDThread::run insert begin _vAllStatMsg.size:" << vAllStatMsg.size() << "|record num:" << iTotalNum << endl;

                if(iTotalNum <= 0)
                {
                    for(int iStatIndex = 0; iStatIndex < dbNumber; ++iStatIndex)
                    {
                        delete vAllStatMsg[iStatIndex];
                    }

                    vAllStatMsg.clear();
                }
                else
                {
                    
                    string sFile="";
                    string sDate="";
                    string sFlag="";
                    time_t time=0;
                    g_app.getTimeInfo(time,sDate,sFlag);

                    //size_t iSize = vAllStatMsg.size();

                    QueueItem item;
                    int iInsertThreadIndex = 0;
                    sRandOrder = g_app.getRandOrder();

                    if (sRandOrder == "")
                    {
                        sRandOrder = "0";
                    }

                    map<string, vector<size_t> >& mIpHasDbInfo = StatDbManager::getInstance()->getIpHasDbInfo();
                    map<string, vector<size_t> >::iterator m_iter = mIpHasDbInfo.begin();

                    while(m_iter != mIpHasDbInfo.end())
                    {
                        vector<size_t> &vDb = m_iter->second;

                        for(size_t i = 0; i < vDb.size(); ++i)
                        {
                            int k = (i + TC_Common::strto<int>(sRandOrder)) % vDb.size();

                            item._index = vDb[k];
                            item._date  = sDate;
                            item._tflag = sFlag;
                            item._statmsg = vAllStatMsg[item._index];

                            iInsertThreadIndex = StatDbManager::getInstance()->getDbToIpIndex(vDb[k]);

                            assert(iInsertThreadIndex >= 0);

                            _runners[iInsertThreadIndex]->put(item);
                        }

                        ++m_iter;
                    }

                    if(_terminate)
                    {
                        break;
                    }
                }

                for(int k = 0; k < g_app.getBuffNum(); ++k)
                {
                    StatHashMap *pHashMap = g_app.getHashMapBuff(iBufferIndex, k);
                    pHashMap->clear();
                }

                TLOGDEBUG("stat ip:" << ServerConfig::LocalIp << "|Buffer Index:" << iBufferIndex << "|ReapSSDThread::run insert record num:" << iTotalNum << "|tast patch finished." << endl);
                FDLOG("CountStat") << "stat ip:" << ServerConfig::LocalIp << "|Buffer Index:" << iBufferIndex << "|ReapSSDThread::run insert record num:" << iTotalNum << "|tast patch finished." << endl;
            }

        }
        catch(exception& ex)
        {
            TLOGERROR("ReapSSDThread::run exception:"<< ex.what() << endl);
        }
        catch(...)
        {
            TLOGERROR("ReapSSDThread::run ReapSSDThread unkonw exception catched" << endl);
        }

        TC_ThreadLock::Lock lock(*this);
        timedWait(REAP_INTERVAL);
    }

    TLOGDEBUG("ReapSSDThread run setITerminateFlag true." << endl);

    StatDbManager::getInstance()->setITerminateFlag(true);

    for(size_t i = 0; i < _runners.size(); ++i)
    {
        if(_runners[i]->isAlive())
        {
            _runners[i]->terminate();

            _runners[i]->getThreadControl().join();
        }
    }

    for(size_t i = 0; i < _runners.size(); ++i)
    {
        if(_runners[i])
        {
            delete _runners[i];
            _runners[i] = NULL;
        }
    }

    TLOGDEBUG("ReapSSDThread run terminate." << endl);
}

int ReapSSDThread::getIndexWithWeighted(int iMaxDb,int iGcd,int iMaxW,const vector<int>& vDbWeight)
{
    while (true){

        _lastSq = (_lastSq + 1) % iMaxDb;

        if (_lastSq == 0)
        {
            _curWeight = _curWeight - iGcd;
            if (_curWeight <= 0)
            {
                _curWeight = iMaxW;
                if(_curWeight == 0)
                {
                    return 0;
                }
            }
        }

        if (vDbWeight[_lastSq] >= _curWeight)
        {
            return _lastSq;
        }
    }
}
void ReapSSDThread::getDataFromBuffer(int iIndex, vector<StatMsg*> &vAllStatMsg, uint64_t &iTotalNum)
{
    TLOGDEBUG("ReapSSDThread::getDataFromBuffer iIndex:" << iIndex << "|begin..." << endl);

    try
    {
        int iCount = 0,dbSeq=0;

        //获取db个数
        int dbNumber = StatDbManager::getInstance()->getDbNumber();

        vector<int> vDbWeight;
        int iGcd = 0,iMaxW = 0;

        StatDbManager::getInstance()->getDbWeighted(iGcd,iMaxW,vDbWeight);

        bool bEnable = StatDbManager::getInstance()->IsEnableWeighted();

        for(int k = 0; k < g_app.getBuffNum(); ++k)
        {
            if(_terminate)
            {
                break;
            }

            StatHashMap *pHashMap = g_app.getHashMapBuff(iIndex, k);

            if(pHashMap->size() == 0)
            {
                continue ;
            }

            FDLOG("CountStat") << "stat ip:" << ServerConfig::LocalIp << "|Buffer Index:" << iIndex << "|ReapSSDThread::getData load hashmap k:" << k << endl;

            HashMap::lock_iterator it = pHashMap->beginSetTime();
            while ( it != pHashMap->end() )
            {
                if(_terminate)
                {
                    break;
                }

                StatMicMsgHead head;
                StatMicMsgBody body;
                int ret = it->get( head, body );
                if ( ret < 0 )
                {
                    ++it;
                    continue;
                }

                if (dbNumber > 0)
                {
                    if(bEnable)//按权重入库
                    {
                        dbSeq = getIndexWithWeighted(dbNumber,iGcd,iMaxW,vDbWeight);
                        TLOGINFO("ReapSSDThread::getIndexWithWeighted |" << dbSeq << endl);
                    }
                    else
                    {
                        dbSeq = iCount % dbNumber;
                    }

                    (*(vAllStatMsg[dbSeq]))[head] = body;
                }

                iCount++;

                ++it;
            }

        }

        iTotalNum = iCount;

        TLOGDEBUG("ReapSSDThread::getDataFromBuffer Buffer Index:" << iIndex << "|get total size:" << iCount << endl);
        FDLOG("CountStat") << "stat ip:" << ServerConfig::LocalIp << "|Buffer Index:" << iIndex << "|ReapSSDThread::getData get total size:" << iCount << "|end..." << endl;
    }
    catch (exception& ex)
    {
        TLOGERROR("ReapSSDThread::getDataFromBuffer exception:" << ex.what() << endl);
        FDLOG("CountStat") << "stat ip:" << ServerConfig::LocalIp << "|Buffer Index:" << iIndex << "|ReapSSDThread::getData exception:" << ex.what() << endl;

        string sMsg("ReapSSDThread::getDataFromBuffer Buffer Index:");
        sMsg += TC_Common::tostr(iIndex);
        sMsg += " exception:";
        sMsg += ex.what();
        sendAlarmSMS(sMsg);
    }
}

int ReapSSDThread::sendAlarmSMS(const string &sMsg)
{
    string errInfo = " ERROR:" + ServerConfig::LocalIp + "_" + sMsg;
    TARS_NOTIFY_ERROR(errInfo);

    return 0;
}

