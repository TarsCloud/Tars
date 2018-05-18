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

#ifndef __STAT_IMP_H_
#define __STAT_IMP_H_

#include <functional>
#include "util/tc_common.h"
#include "util/tc_thread.h"
#include "util/tc_option.h"
#include "util/tc_file.h"
#include "util/tc_mysql.h"
#include "util/tc_config.h"
#include "util/tc_hash_fun.h"
#include "servant/TarsLogger.h"
#include "jmem/jmem_hashmap.h"
#include "servant/StatF.h"
#include "StatHashMap.h"

using namespace tars;

class StatImpThreadData : public TC_ThreadPool::ThreadData
{
public:
    static TC_ThreadMutex _mutex;        //全局互斥锁
    static pthread_key_t  _key;            //线程私有数据key
    static size_t         _iNo;         

    /**
     * 构造函数
     */
    StatImpThreadData();

    /**
     * 数据资源释放
     * @param p
     */
    static void destructor(void* p);

    /**
     * 获取线程数据，没有的话会自动创建
     * @return ServantProxyThreadData*
     */
    static StatImpThreadData * getData();

public:
    size_t                _iThreadIndex;
};

class StatImp : public StatF,public TC_ThreadLock
{
public:
    /**
     *
     */
    StatImp()
    : _hashf(tars::hash<string>())
    , _threadIndex(0)
    {
    };

    ~StatImp()
    {
    };

    /**
     * 初始化
     *
     * @return int
     */
    virtual void initialize();

    /**
     * 退出
     */
    virtual void destroy()
    {
    };

    /**
     * 上报模块间调用信息
     * @param statmsg, 上报信息
     * @return int, 返回0表示成功
     */
    virtual int reportMicMsg( const map<tars::StatMicMsgHead, tars::StatMicMsgBody>& statmsg, bool bFromClient, tars::TarsCurrentPtr current );
    
    /**
     * 上报模块间调用采样信息
     * @param sample, 上报信息
     * @return int, 返回0表示成功
     */
    virtual int reportSampleMsg(const vector<StatSampleMsg> &msg,tars::TarsCurrentPtr current );

    using hash_functor = std::function<size_t (const std::string& )>;

protected:

    int addHashMap(const StatMicMsgHead &head, const StatMicMsgBody &body);

private:
    void dump2file();

    string getSlaveName(const string& sSlaveName);

private:
    hash_functor                    _hashf;
    size_t                            _threadIndex;
};

#endif


