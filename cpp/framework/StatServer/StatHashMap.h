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

#ifndef __STAT_HASH_MAP_H_
#define __STAT_HASH_MAP_H_

#include "servant/StatF.h"
#include "util/tc_common.h"
#include "util/tc_thread.h"
#include "util/tc_option.h"
#include "jmem/jmem_hashmap.h"
#include "util/tc_file.h"
#include "util/tc_config.h"
#include "servant/TarsLogger.h"

using namespace tars;

typedef TarsHashMap<StatMicMsgHead, StatMicMsgBody, ThreadLockPolicy,ShmStorePolicy> HashMap;//FileStorePolicy

typedef std::map<tars::StatMicMsgHead, tars::StatMicMsgBody, std::less<tars::StatMicMsgHead>, __gnu_cxx::__pool_alloc<std::pair<tars::StatMicMsgHead const, tars::StatMicMsgBody> > > StatMsg;

class StatHashMap : public HashMap
{
public:
    /**
    * 增加数据
    * @param Key
    * @param Value
    *
    * @return int
    */
    int add(const tars::StatMicMsgHead &head, const tars::StatMicMsgBody &body)
    {
        StatMicMsgBody stBody;
        int ret = TC_HashMap::RT_OK;
        tars::TarsOutputStream<BufferWriter> osk;
        head.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());
        string sv;
        time_t t = 0;

        {

            TC_LockT<ThreadLockPolicy::Mutex> lock(ThreadLockPolicy::mutex());
            ret = this->_t.get(sk, sv,t);

            if (ret < 0)
            {
                return -1;
            }

             //读取到数据了, 解包
            if (ret == TC_HashMap::RT_OK)
            {
                tars::TarsInputStream<BufferReader> is;
                is.setBuffer(sv.c_str(), sv.length());
                stBody.readFrom(is);
            }

            stBody.count            += body.count;
            stBody.execCount        += body.execCount;
            stBody.timeoutCount     += body.timeoutCount;

            for(map<int,int>::const_iterator it = body.intervalCount.begin();it!= body.intervalCount.end();it++)
            {
                stBody.intervalCount[it->first] += it->second;
            }

            stBody.totalRspTime += body.totalRspTime;
            if(stBody.maxRspTime < body.maxRspTime)
            {
                stBody.maxRspTime = body.maxRspTime;
            }
            //非0最小值
            if( stBody.minRspTime == 0 || (stBody.minRspTime > body.minRspTime && body.minRspTime != 0))
            {
                stBody.minRspTime = body.minRspTime;
            }

            tars::TarsOutputStream<BufferWriter> osv;
            stBody.writeTo(osv);
            string stemp(osv.getBuffer(), osv.getLength());
            vector<TC_HashMap::BlockData> vtData;

            TLOGINFO("StatHashMap::add sk.length:" << sk.length() << " v.length:" << stemp.length() << endl);

            ret = this->_t.set(sk, stemp, true, vtData); 
        }

        return   ret;
    }
};

#endif


