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

#ifndef __TC_TIMEOUT_QUEUE_NEW_H
#define __TC_TIMEOUT_QUEUE_NEW_H
#include <map>
#include <list>
#include <unordered_map>
#include <iostream>
#include <cassert>
#include <functional>
#include "util/tc_autoptr.h"
#include "util/tc_monitor.h"
#include "util/tc_timeprovider.h"

using namespace std;

namespace tars
{
/////////////////////////////////////////////////
/**
 * @file tc_timeout_queue_new.h
 * @brief 超时队列.
 *
 */
/////////////////////////////////////////////////

template<class T>
class TC_TimeoutQueueNew
{
public:

    struct PtrInfo;
    struct NodeInfo;
    struct SendInfo;

    typedef unordered_map<uint32_t, PtrInfo>     data_type;
    typedef multimap<int64_t,NodeInfo>      time_type;
    typedef list<SendInfo>                  send_type;

    using data_functor = std::function<void (T& )>;

    struct PtrInfo
    {
        T ptr;
        bool hasSend;
        typename time_type::iterator timeIter;
        typename send_type::iterator sendIter;
    };

    struct NodeInfo
    {
        typename data_type::iterator dataIter;
    };

    struct SendInfo
    {
        typename data_type::iterator dataIter;
    };
    /**
     * @brief 超时队列，缺省5s超时.
     *
     * @param timeout 超时设定时间
     * @param size
     */
    TC_TimeoutQueueNew(int timeout = 5*1000,size_t size = 100 ) : _uniqId(0)
    {
        _data.reserve(size);
    }

    /**
     * @brief  产生该队列的下一个ID
     */
    uint32_t generateId();

    /**
     * 要发送的链表是否为空
     */
    bool sendListEmpty()
    {
        return _send.empty();
    }

    /**
     * 获取要发送的数据
     */
    bool getSend(T & t);

    /**
     * 把已经发送的数据从list里面删除
     */
    void popSend(bool del = false);

    /**
     *获取要发送list的size
     */
    size_t getSendListSize()
    {
        return _send.size();
    }

    /**
     * @brief 获取指定id的数据.
     *
     * @param id 指定的数据的id
     * @param T 指定id的数据
     * @return bool get的结果
     */
    bool get(uint32_t uniqId, T & t,bool bErase = true);

    /**
     * @brief 删除.
     *
     * @param uniqId 要删除的数据的id
     * @param T     被删除的数据
     * @return bool 删除结果
     */
    bool erase(uint32_t uniqId, T & t);

    /**
     * @brief 设置消息到队列尾端.
     *
     * @param ptr        要插入到队列尾端的消息
     * @param uniqId     序列号
     * @param timeout    超时时间
     * @return true  成功 false 失败
     */
    bool push(T& ptr, uint32_t uniqId,int64_t timeout,bool hasSend = true);

    /**
     * @brief 超时删除数据
     */
    void timeout();

    /**
     * @brief 超时删除数据
     */
    bool timeout(T & t);

    /**
     * @brief 删除超时的数据，并用df对数据做处理
     */
    void timeout(const data_functor &df);

    /**
     * @brief 队列中的数据.
     *
     * @return size_t
     */
    size_t size() const { return _data.size(); }

protected:
    uint32_t                        _uniqId;
    data_type                       _data;
    time_type                       _time;
    send_type                       _send;
};

template<typename T> bool TC_TimeoutQueueNew<T>::getSend(T & t)
{
    //链表为空返回失败
    if(_send.empty())
    {
        return false;
    }

    SendInfo & stSendInfo = _send.back();
    assert(!stSendInfo.dataIter->second.hasSend);
    t = stSendInfo.dataIter->second.ptr;
    return true;
}


template<typename T> void TC_TimeoutQueueNew<T>::popSend(bool del)
{
    assert(!_send.empty());
    SendInfo stSendInfo;
    stSendInfo = _send.back();
    stSendInfo.dataIter->second.hasSend = true;
    if(del)
    {
        _time.erase(stSendInfo.dataIter->second.timeIter);
        _data.erase(stSendInfo.dataIter);
    }
    _send.pop_back();
}

template<typename T> bool TC_TimeoutQueueNew<T>::get(uint32_t uniqId, T & t, bool bErase)
{
    typename data_type::iterator it = _data.find(uniqId);

    if(it == _data.end())
    {
        return false;
    }

    t = it->second.ptr;

    if(bErase)
    {
        _time.erase(it->second.timeIter);
        if(!it->second.hasSend)
        {
            _send.erase(it->second.sendIter);
        }
        _data.erase(it);
    }

    return true;
}

template<typename T> uint32_t TC_TimeoutQueueNew<T>::generateId()
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    while (++_uniqId == 0);

    return _uniqId;
}

template<typename T> bool TC_TimeoutQueueNew<T>::push(T& ptr, uint32_t uniqId,int64_t timeout,bool hasSend)
{
    PtrInfo pi;
    pi.ptr = ptr;
    pi.hasSend = hasSend;

    pair<typename data_type::iterator, bool> result;
    result = _data.insert(make_pair(uniqId, pi));
    if (result.second == false) return false;

    NodeInfo stNodeInfo;
    stNodeInfo.dataIter = result.first;
    result.first->second.timeIter = _time.insert(make_pair(timeout,stNodeInfo));

    //没有发送放到list队列里面
    if(!hasSend)
    {
        SendInfo stSendInfo;
        stSendInfo.dataIter = result.first;
        _send.push_front(stSendInfo);
        result.first->second.sendIter = _send.begin();
    }

    return true;
}

template<typename T> void TC_TimeoutQueueNew<T>::timeout()
{
    int64_t iNow = TNOWMS;
    while(true)
    {
        typename time_type::iterator it = _time.begin();

        if(_time.end() == it || it->first>iNow)
            break;

        if(!it->second.dataIter->second.hasSend)
        {
            _send.erase(it->second.dataIter->second.sendIter);
        }
        _data.erase(it->second.dataIter);
        _time.erase(it);
    }
}

template<typename T> bool TC_TimeoutQueueNew<T>::timeout(T & t)
{
    int64_t iNow = TNOWMS;
    if(_time.empty())
        return false;
    typename time_type::iterator it = _time.begin();
    if(it->first>iNow)
        return false;

    t=it->second.dataIter->second.ptr;
    if(!it->second.dataIter->second.hasSend)
    {
        _send.erase(it->second.dataIter->second.sendIter);
    }
    _data.erase(it->second.dataIter);
    _time.erase(it);
    return true;
}

template<typename T> void TC_TimeoutQueueNew<T>::timeout(const data_functor& df)
{
    while(true)
    {
        int64_t iNow = TNOWMS;
        T ptr;
        typename time_type::iterator it = _time.begin();
        if(_time.end() == it || it->first>iNow)
            break;

        ptr=it->second->second.ptr;
        if(!it->second.dataIter->second.hasSend)
        {
            _send.erase(it->second.dataIter->second.sendIter);
        }
        _data.erase(it->second.dataIter);
        _time.erase(it);

        try { df(ptr); } catch(...) { }
    }
}

template<typename T> bool TC_TimeoutQueueNew<T>::erase(uint32_t uniqId, T & t)
{
    typename data_type::iterator it = _data.find(uniqId);

    if(it == _data.end())
        return false;

    t = it->second.ptr;
    if(!it->second.hasSend)
    {
        _send.erase(it->second.sendIter);
    }
    _time.erase(it->second.timeIter);
    _data.erase(it);

    return true;
}
/////////////////////////////////////////////////////////////////
}
#endif
