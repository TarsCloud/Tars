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

#include "PropertyImp.h"
#include "PropertyServer.h"

///////////////////////////////////////////////////////////
TC_ThreadMutex PropertyImpThreadData::_mutex;
pthread_key_t PropertyImpThreadData::_key = 0;
size_t PropertyImpThreadData::_no=0;

///////////////////////////////////////////////////////////
PropertyImpThreadData::PropertyImpThreadData()
: _threadIndex(0)
{
}
void PropertyImpThreadData::destructor(void* p)
{
    PropertyImpThreadData * pSptd = (PropertyImpThreadData*)p;
    if(pSptd)
    {    
        delete pSptd;
        pSptd = NULL;
    }
}
PropertyImpThreadData * PropertyImpThreadData::getData()
{
    if(_key == 0)
    {
        TC_LockT<TC_ThreadMutex> lock(_mutex);
        if(_key == 0)
        {
            int iRet = ::pthread_key_create(&_key, PropertyImpThreadData::destructor);

            if (iRet != 0)
            {
                TLOGERROR("PropertyImpThreadData pthread_key_create fail:"<< errno << ":" << strerror(errno) << endl);
                return NULL;
            }
        }
    }

    PropertyImpThreadData * pSptd = (PropertyImpThreadData*)pthread_getspecific(_key);

    if(!pSptd)
    {
        TC_LockT<TC_ThreadMutex> lock(_mutex);

        pSptd = new PropertyImpThreadData();
        pSptd->_threadIndex = _no;
        ++_no;

        int iRet = pthread_setspecific(_key, (void *)pSptd);

        assert(iRet == 0);
    }
    return pSptd;
}
///////////////////////////////////////////////////////////
void PropertyImp::initialize()
{

}

int PropertyImp::reportPropMsg(const map<StatPropMsgHead,StatPropMsgBody>& propMsg, tars::TarsCurrentPtr current )
{
    TLOGINFO("PropertyImp::reportPropMsg size:" << propMsg.size() << endl);

    handlePropMsg(propMsg, current);

    return 0;
}

int PropertyImp::handlePropMsg(const map<StatPropMsgHead, StatPropMsgBody> &propMsg, tars::TarsCurrentPtr current)
{
    for ( map<StatPropMsgHead,StatPropMsgBody>::const_iterator it = propMsg.begin(); it != propMsg.end(); it++ )
    {

        const StatPropMsgHead &head = it->first;
        const StatPropMsgBody &body = it->second;

        PropHead tHead;
        tHead.moduleName    = head.moduleName;
        tHead.propertyName  = head.propertyName;
        tHead.setName       = head.setName;
        tHead.setArea       = head.setArea;
        tHead.setID         = head.setID;
        tHead.ip            = current->getIp();

        size_t iIndex = 0;
        PropertyImpThreadData * td = PropertyImpThreadData::getData();

        if(td)
        {
            iIndex = td->_threadIndex;
        }

        dump2file();

        int iBufferIndex = g_app.getSelectBufferIndex();

        if(_lastBufferIndex != iBufferIndex)
        {
            if(_lastBufferIndex != -1)
            {
                TLOGDEBUG("PropertyImp::handlePropMsg iIndex:" << iIndex << "|iBufferIndex:" << iBufferIndex << "|_lastBufferIndex:" << _lastBufferIndex << endl);

                map<int,vector<pair<int64_t, int> > >& mBuffer = g_app.getBuffer();
                map<int,vector<pair<int64_t, int> > >::iterator iter = mBuffer.find(_lastBufferIndex);
                iter->second[iIndex].first  = TNOWMS;
                iter->second[iIndex].second = 1;
            }

            _lastBufferIndex = iBufferIndex;
        }

        string sKey = tHead.moduleName;
        sKey += tHead.ip;

        int iHashKey = _hashf(sKey) % g_app.getBuffNum();

        PropertyHashMap *pHashMap = g_app.getHashMapBuff(iBufferIndex, iHashKey);
        
        //////////////////////////////////////////////////////////////////////////////////////

        float rate =  (pHashMap->getMapHead()._iUsedChunk) * 1.0/pHashMap->allBlockChunkCount();

        if(rate >0.9)
        {
            pHashMap->expand(pHashMap->getMapHead()._iMemSize * 2);
            TLOGERROR("PropertyImp::handlePropMsg hashmap expand to " << pHashMap->getMapHead()._iMemSize << endl);
        }

        int iRet = pHashMap->add(tHead, body);
        if(iRet != TC_HashMap::RT_OK )
        {
            TLOGERROR("PropertyImp::handlePropMsg add hashmap recourd iRet:" << iRet << endl);
        }

        if(LOG->IsNeedLog(TarsRollLogger::INFO_LOG))
        {
            ostringstream os;
            os.str("");
            head.displaySimple(os);
            body.displaySimple(os);
            TLOGINFO("ret|"<<iRet<<"|"<<os.str()<< endl);
        }

    }

    return 0;
}

void PropertyImp::dump2file()
{
    static string g_sDate;
    static string g_sFlag;

    time_t tTimeNow         = TC_TimeProvider::getInstance()->getNow();
    time_t tTimeInterv      = g_app.getInserInterv() * 60;//second

    static time_t g_tLastDumpTime   = 0;

    if(g_tLastDumpTime == 0)
    {
        g_app.getTimeInfo(g_tLastDumpTime,g_sDate,g_sFlag);
    }

    if(tTimeNow - g_tLastDumpTime > tTimeInterv)
    {
        static  TC_ThreadLock g_mutex;
        TC_ThreadLock::Lock  lock( g_mutex );
        if(tTimeNow - g_tLastDumpTime > tTimeInterv)
        {
            g_app.getTimeInfo(g_tLastDumpTime,g_sDate,g_sFlag);

            int iSelectBuffer = g_app.getSelectBufferIndex();
            iSelectBuffer     = !iSelectBuffer;

            g_app.setSelectBufferIndex(iSelectBuffer);

            TLOGDEBUG("PropertyImp::dump2file select buffer:" << iSelectBuffer << "|TimeInterv:" << tTimeInterv << "|now:" << tTimeNow << "|last:" << g_tLastDumpTime << endl);
            FDLOG("PropertyPool") << "propertypool ip:" << ServerConfig::LocalIp << "|PropertyImp::dump2file select buffer:" << iSelectBuffer << "|TimeInterv:" << tTimeInterv << "|now:" << tTimeNow << "|last:" << g_tLastDumpTime << endl;
        }
    }
}





