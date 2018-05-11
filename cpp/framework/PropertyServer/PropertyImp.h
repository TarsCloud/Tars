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

#ifndef __PROPERTY_IMP_H_
#define __PROPERTY_IMP_H_

#include <functional>
#include "util/tc_common.h"
#include "util/tc_thread.h"
#include "util/tc_option.h"
#include "util/tc_hash_fun.h"
#include "jmem/jmem_hashmap.h"
#include "servant/PropertyF.h"
#include "PropertyHashMap.h"

using namespace tars;

class PropertyImpThreadData : public TC_ThreadPool::ThreadData
{
public:
    static TC_ThreadMutex _mutex;        //全局互斥锁
    static pthread_key_t  _key;            //线程私有数据key
    static size_t         _no;         

    /**
     * 构造函数
     */
    PropertyImpThreadData();

    /**
     * 数据资源释放
     * @param p
     */
    static void destructor(void* p);

    /**
     * 获取线程数据，没有的话会自动创建
     * @return ServantProxyThreadData*
     */
    static PropertyImpThreadData * getData();

public:
    size_t                _threadIndex;
};

class PropertyImp : public PropertyF,public TC_ThreadLock
{
public:
 
    using hash_functor = std::function<size_t (const std::string&) >;
    /**
     *
     */
    PropertyImp()
    : _lastBufferIndex(-1)
    , _hashf(tars::hash<string>())
    {
    }

    /**
     * 析够函数
     */
    ~PropertyImp()  { }

    /**
     * 初始化
     *
     * @return int
     */
    virtual void initialize();

    /**
     * 退出
     */
    virtual void destroy() { }

    /**
    * 上报性属信息
    * @param statmsg, 上报信息
    * @return int, 返回0表示成功
    */
    virtual int reportPropMsg(const map<StatPropMsgHead,StatPropMsgBody>& propMsg, tars::TarsCurrentPtr current );

private:
    int handlePropMsg(const map<StatPropMsgHead, StatPropMsgBody> &propMsg, tars::TarsCurrentPtr current);

    void dump2file();

private:

    int                                _lastBufferIndex;
    hash_functor                    _hashf;

};

#endif


