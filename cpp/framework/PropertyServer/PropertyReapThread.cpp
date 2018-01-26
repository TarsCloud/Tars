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

#include "PropertyReapThread.h"
#include "util/tc_config.h"
#include "PropertyServer.h"
#include "PropertyDbManager.h"

////////////////////////////////////////////////////////////////
ReapSSDProcThread::ReapSSDProcThread(PropertyReapThread * proc)
: _bTerminate(false)
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
    _bTerminate = true;

    _queue.notifyT();
}

void ReapSSDProcThread::put(QueueItem data)
{
    if(!_bTerminate)
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

    while (!_bTerminate)
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

                    PropertyDbManager::getInstance()->insert2MultiDbs(item._index, *item._statmsg, item._date, item._tflag);

                    iEnd = TNOWMS;

                    TLOGDEBUG("ReapSSDProcThread::run stat ip:" << ServerConfig::LocalIp  << "|dbIndex:" << item._index << "|" << PropertyDbManager::getInstance()->getIpAndPort(item._index) 
                        << "|date:" << item._date << "|tflag:" << item._tflag << "|timecost(ms):" << (iEnd - iBegin) << "|iBegin:" << iBegin << "|iEnd:" << iEnd << endl);

                    FDLOG("PropertyPool") << "ReapSSDProcThread::run stat ip:" << ServerConfig::LocalIp  << "|dbIndex:" << item._index << "|" << PropertyDbManager::getInstance()->getIpAndPort(item._index) 
                        << "|date:" << item._date << "|tflag:" << item._tflag << "|timecost(ms):" << (iEnd - iBegin) << "|iBegin:" << iBegin << "|iEnd:" << iEnd << endl;

                    if((iEnd - iBegin)/1000 > g_app.getInserInterv() * 40)
                    {
                        FDLOG("PropertyPool") << "propertypool ip:" << ServerConfig::LocalIp << "|ReapSSDProcThread::run timeout 8 minute." << endl;
                        string sMsg("stat ip:");
                        sMsg += ServerConfig::LocalIp;
                        sMsg += " ReapSSDProcThread::run write db:";
                        sMsg += PropertyDbManager::getInstance()->getIpAndPort(item._index);
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
            FDLOG("PropertyPool") << "ReapSSDProcThread::run exception:" << e.what() << endl;
        }
    }
}
//////////////////////////////////////////////////////////////
PropertyReapThread::PropertyReapThread()
:  _terminate(false)
, _curWeight(0)
, _lastSq(-1)
{
    TLOGDEBUG("PropertyReapThread begin ok." << endl);
}

PropertyReapThread::~PropertyReapThread()
{
    if (isAlive())
    {
        terminate();

        getThreadControl().join();
    }
}

void PropertyReapThread::terminate()
{
    TLOGDEBUG("PropertyReapThread terminate." << endl);

    _terminate = true;

    TC_ThreadLock::Lock lock(*this);

    notifyAll();
}

void PropertyReapThread::run()
{
    int iInsertDataNum = PropertyDbManager::getInstance()->getInsertDbThreadNum();

    for(int i = 0; i < iInsertDataNum; ++i)
    {
        ReapSSDProcThread *r = new ReapSSDProcThread(this);

        r->start();

        _runners.push_back(r);
    }

    string sDate,sTime;

    int dbNumber = PropertyDbManager::getInstance()->getDbNumber();

    string sRandOrder;

    uint64_t iTotalNum = 0;

    int iLastIndex = -1;

    TLOGDEBUG("propertypool ip:" << ServerConfig::LocalIp << "|PropertyReapThread::run iInsertDataThread:" << iInsertDataNum << "|dbNumber:" << dbNumber << endl);
    FDLOG("PropertyPool") << "propertypool ip:" << ServerConfig::LocalIp << "|PropertyReapThread::run iInsertDataThread:" << iInsertDataNum << "|dbNumber:" << dbNumber << endl;

    while (!_terminate)
    {
        try
        {
            //双buffer中一个buffer入库
            int iBufferIndex = !(g_app.getSelectBufferIndex());
            int64_t iInterval = 1000;
            if(iBufferIndex != iLastIndex && g_app.getSelectBuffer(iBufferIndex, iInterval))
            {
                iLastIndex = iBufferIndex;

                iTotalNum = 0;

                vector<PropertyMsg*> vAllPropertyMsg;
                for(int iStatIndex = 0; iStatIndex < dbNumber; ++iStatIndex)
                {
                    vAllPropertyMsg.push_back(new PropertyMsg());
                }

                int64_t tBegin = TNOWMS;

                getDataFromBuffer(iBufferIndex, vAllPropertyMsg, iTotalNum);

                int64_t tEnd = TNOWMS;

                TLOGDEBUG("propertypool ip:" << ServerConfig::LocalIp << "|Buffer Index:" << iBufferIndex << "|PropertyReapThread::run getDataFromBuffer timecost(ms):" << (tEnd - tBegin) << endl);
                FDLOG("PropertyPool") << "propertypool ip:" << ServerConfig::LocalIp << "|Buffer Index:" << iBufferIndex << "|PropertyReapThread::run getDataFromBuffer timecost(ms):" << (tEnd - tBegin) << endl;

                TLOGDEBUG("propertypool ip:" << ServerConfig::LocalIp << "|Buffer Index:" << iBufferIndex << "|PropertyReapThread::run insert begin _vAllStatMsg.size:" << vAllPropertyMsg.size() << "|record num:" << iTotalNum << endl);
                FDLOG("PropertyPool") << "propertypool ip:" << ServerConfig::LocalIp << "|Buffer Index:" << iBufferIndex << "|PropertyReapThread::run insert begin _vAllStatMsg.size:" << vAllPropertyMsg.size() << "|record num:" << iTotalNum << endl;

                if(iTotalNum <= 0)
                {
                    for(int iStatIndex = 0; iStatIndex < dbNumber; ++iStatIndex)
                    {
                        delete vAllPropertyMsg[iStatIndex];
                    }

                    vAllPropertyMsg.clear();
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

                    map<string, vector<size_t> >& mIpHasDbInfo = PropertyDbManager::getInstance()->getIpHasDbInfo();
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
                            item._statmsg = vAllPropertyMsg[item._index];

                            iInsertThreadIndex = PropertyDbManager::getInstance()->getDbToIpIndex(vDb[k]);

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
                    PropertyHashMap *pHashMap = g_app.getHashMapBuff(iBufferIndex, k);
                    pHashMap->clear();
                }

                TLOGDEBUG("propertypool ip:" << ServerConfig::LocalIp << "|Buffer Index:" << iBufferIndex << "|PropertyReapThread::run insert record num:" << iTotalNum << "|tast patch finished." << endl);
                FDLOG("PropertyPool") << "propertypool ip:" << ServerConfig::LocalIp << "|Buffer Index:" << iBufferIndex << "|PropertyReapThread::run insert record num:" << iTotalNum << "|tast patch finished." << endl;
            }

        }
        catch(exception& ex)
        {
            TLOGERROR("PropertyReapThread::run exception:"<< ex.what() << endl);
        }
        catch(...)
        {
            TLOGERROR("PropertyReapThread::run ReapSSDThread unkonw exception catched" << endl);
        }

        TC_ThreadLock::Lock lock(*this);
        timedWait(REAP_INTERVAL);
    }

    TLOGDEBUG("PropertyReapThread run setTerminateFlag true." << endl);

    PropertyDbManager::getInstance()->setTerminateFlag(true);

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

    TLOGDEBUG("PropertyReapThread run terminate." << endl);
}

int PropertyReapThread::getIndexWithWeighted(int iMaxDb,int iGcd,int iMaxW,const vector<int>& vDbWeight)
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
void PropertyReapThread::getDataFromBuffer(int iIndex, vector<PropertyMsg*> &vAllPropertyMsg, uint64_t &iTotalNum)
{
    TLOGDEBUG("PropertyReapThread::getDataFromBuffer iIndex:" << iIndex << "|begin..." << endl);

    try
    {
        int iCount = 0,dbSeq=0;

        //获取db个数
        int dbNumber = PropertyDbManager::getInstance()->getDbNumber();

        vector<int> vDbWeight;
        int iGcd = 0,iMaxW = 0;

        PropertyDbManager::getInstance()->getDbWeighted(iGcd,iMaxW,vDbWeight);

        bool bEnable = PropertyDbManager::getInstance()->hasEnableWeighted();

        for(int k = 0; k < g_app.getBuffNum(); ++k)
        {
            if(_terminate)
            {
                break;
            }

            PropertyHashMap *pHashMap = g_app.getHashMapBuff(iIndex, k);

            if(pHashMap->size() == 0)
            {
                continue ;
            }

            FDLOG("PropertyPool") << "property ip:" << ServerConfig::LocalIp << "|Buffer Index:" << iIndex << "|PropertyReapThread::getData load hashmap k:" << k << endl;

            PropertyHashMap::lock_iterator it = pHashMap->beginSetTime();
            while ( it != pHashMap->end() )
            {
                if(_terminate)
                {
                    break;
                }

                PropHead head;
                PropBody body;
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
                        TLOGINFO("PropertyReapThread::getIndexWithWeighted |" << dbSeq << endl);
                    }
                    else
                    {
                        dbSeq = iCount % dbNumber;
                    }

                    (*(vAllPropertyMsg[dbSeq]))[head] = body;
                }

                iCount++;

                ++it;
            }

        }

        iTotalNum = iCount;

        TLOGDEBUG("PropertyReapThread::getDataFromBuffer Buffer Index:" << iIndex << "|get total size:" << iCount << endl);
        FDLOG("PropertyPool") << "propertypool ip:" << ServerConfig::LocalIp << "|Buffer Index:" << iIndex << "|PropertyReapThread::getData get total size:" << iCount << "|end..." << endl;
    }
    catch (exception& ex)
    {
        TLOGERROR("PropertyReapThread::getDataFromBuffer exception:" << ex.what() << endl);
        FDLOG("PropertyPool") << "propertypool ip:" << ServerConfig::LocalIp << "|Buffer Index:" << iIndex << "|PropertyReapThread::getData exception:" << ex.what() << endl;

        string sMsg("PropertyReapThread::getDataFromBuffer Buffer Index:");
        sMsg += TC_Common::tostr(iIndex);
        sMsg += " exception:";
        sMsg += ex.what();
        sendAlarmSMS(sMsg);
    }
}

int PropertyReapThread::sendAlarmSMS(const string &sMsg)
{
    string errInfo = " ERROR:" + ServerConfig::LocalIp + "_" + sMsg;
    TARS_NOTIFY_ERROR(errInfo);

    return 0;
}


