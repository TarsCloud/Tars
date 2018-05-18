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

#ifndef _TC_TIMEOUT_QUEUE_NOID_H_
#define _TC_TIMEOUT_QUEUE_NOID_H_

#include <map>
#include <list>
#include <iostream>
#include <cassert>
#include "util/tc_autoptr.h"
#include "util/tc_monitor.h"
#include "util/tc_timeprovider.h"

using namespace std;

namespace tars
{
/////////////////////////////////////////////////
/**
 * @file tc_timeout_queue_noid.h
 * @brief 超时队列
 *
 */
/////////////////////////////////////////////////

template<class T>
class TC_TimeoutQueueNoID
{
public:

    struct PtrInfo;
    struct NodeInfo;

    typedef multimap<int64_t,NodeInfo>      time_type;
    typedef list<PtrInfo>                   list_type;

    struct PtrInfo
    {
        T ptr;
        typename time_type::iterator timeIter;
    };

    struct NodeInfo
    {
        typename list_type::iterator listIter;
    };

    /**
     * @brief 超时队列，缺省5s超时.
     *
     * @param timeout 超时设定时间
     * @param size
     */
    TC_TimeoutQueueNoID()
    {
    }

    /**
     * @brief 取出队列第一个数据
     *
     * @param ptr  取出队列 
     * @return true  成功 false 失败
     */
    bool pop(T & ptr);
    bool pop();

    /**
     * @brief 获取队列第一个数据
     *
     * @param ptr   数据
     * @return true  成功 false 失败
     */
    bool front(T & ptr);

    /**
     * @brief 设置消息到队列尾端.
     *
     * @param ptr        要插入到队列尾端的消息
     * @param timeout    超时时间 绝对时间
     * @return true  成功 false 失败
     */
    bool push(T& ptr, int64_t timeout);

    /**
     * @brief 超时删除数据
     */
    void timeout();

    /**
     * @brief 超时删除数据
     */
    bool timeout(T & t);

    /**
     * @brief 是否为空
     */
    bool empty() const
    {
        return _list.empty();
    }

    /**
     * @brief 队列中的数据.
     *
     * @return size_t
     */
    size_t size() const { return _list.size(); }

protected:
    time_type                       _time;
    list_type                       _list;
};


template<typename T> bool TC_TimeoutQueueNoID<T>::front(T & t)
{
    if(_list.empty())
    {
        return false;
    }

    PtrInfo & pi = _list.back();

    t = pi.ptr;

    return true;
}

template<typename T> bool TC_TimeoutQueueNoID<T>::pop()
{
    T t;
    return pop(t);
}

template<typename T> bool TC_TimeoutQueueNoID<T>::pop(T & t)
{
    if(_list.empty())
    {
        return false;
    }

    PtrInfo & pi = _list.back();

    t = pi.ptr;

    _time.erase(pi.timeIter);
    _list.pop_back();

    return true;
}


template<typename T> bool TC_TimeoutQueueNoID<T>::push(T& ptr, int64_t timeout)
{
    PtrInfo pi;
    pi.ptr = ptr;

    _list.push_front(pi);

    PtrInfo & pinfo = _list.front();

    NodeInfo stNodeInfo;
    stNodeInfo.listIter = _list.begin();
    pinfo.timeIter = _time.insert(make_pair(timeout,stNodeInfo));

    return true;
}

template<typename T> void TC_TimeoutQueueNoID<T>::timeout()
{
    int64_t iNow = TNOWMS;
    while(true)
    {
        typename time_type::iterator it = _time.begin();

        if(_time.end() == it || it->first>iNow)
            break;

        _list.erase(it->second.listIter);
        _time.erase(it);
    }
}

template<typename T> bool TC_TimeoutQueueNoID<T>::timeout(T & t)
{
    if(_time.empty())
    {
        return false;
    }

    typename time_type::iterator it = _time.begin();
    if(it->first > TNOWMS)
    {
        return false;
    }

    t=it->second.listIter->ptr;

    _list.erase(it->second.listIter);

    _time.erase(it);

    return true;
}
/////////////////////////////////////////////////////////////////
}
#endif
