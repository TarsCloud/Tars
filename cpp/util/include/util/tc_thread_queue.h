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

#ifndef __TC_THREAD_QUEUE_H_
#define __TC_THREAD_QUEUE_H_

#include <deque>
#include <vector>
#include <cassert>
#include "util/tc_monitor.h"

namespace tars
{
/////////////////////////////////////////////////
/** 
 * @file tc_thread_queue.h
 * @brief 线程队列类.
 * 
 */
       
/////////////////////////////////////////////////
/**
 * @brief 线程安全队列
 */
template<typename T, typename D = deque<T> >
class TC_ThreadQueue : protected TC_ThreadLock
{
public:
    TC_ThreadQueue():_size(0){};

public:

    typedef D queue_type;

    /**
     * @brief 从头部获取数据, 没有数据则等待.
     *
     * @param t 
     * @param millsecond   阻塞等待时间(ms) 
     *                    0 表示不阻塞 
     *                      -1 永久等待
     * @return bool: true, 获取了数据, false, 无数据
     */
    bool pop_front(T& t, size_t millsecond = 0);

    /**
     * @brief 通知等待在队列上面的线程都醒过来
     */
    void notifyT();

    /**
     * @brief 放数据到队列后端. 
     *  
     * @param t
     */
    void push_back(const T& t);

    /**
     * @brief  放数据到队列后端. 
     *  
     * @param vt
     */
    void push_back(const queue_type &qt);
    
    /**
     * @brief  放数据到队列前端. 
     *  
     * @param t
     */
    void push_front(const T& t);

    /**
     * @brief  放数据到队列前端. 
     *  
     * @param vt
     */
    void push_front(const queue_type &qt);

    /**
     * @brief  等到有数据才交换. 
     *  
     * @param q
     * @param millsecond  阻塞等待时间(ms) 
     *                   0 表示不阻塞 
     *                      -1 如果为则永久等待
     * @return 有数据返回true, 无数据返回false
     */
    bool swap(queue_type &q, size_t millsecond = 0);

    /**
     * @brief  队列大小.
     *
     * @return size_t 队列大小
     */
    size_t size() const;

    /**
     * @brief  清空队列
     */
    void clear();

    /**
     * @brief  是否数据为空.
     *
     * @return bool 为空返回true，否则返回false
     */
    bool empty() const;

protected:
    /**
     * 队列
     */
    queue_type          _queue;

    /**
     * 队列长度
     */
    size_t              _size;

};

template<typename T, typename D> bool TC_ThreadQueue<T, D>::pop_front(T& t, size_t millsecond)
{
    Lock lock(*this);

    if (_queue.empty())
    {
        if(millsecond == 0)
        {
            return false;
        }
        if(millsecond == (size_t)-1)
        {
            wait();
        }
        else
        {
            //超时了
            if(!timedWait(millsecond))
            {
                return false;
            }
        }
    }

    if (_queue.empty())
    {
        return false;
    }

    t = _queue.front();
    _queue.pop_front();
    assert(_size > 0);
    --_size;


    return true;
}

template<typename T, typename D> void TC_ThreadQueue<T, D>::notifyT()
{
    Lock lock(*this);
    notifyAll();
}

template<typename T, typename D> void TC_ThreadQueue<T, D>::push_back(const T& t)
{
    Lock lock(*this);

    notify();

    _queue.push_back(t);
    ++_size;
}

template<typename T, typename D> void TC_ThreadQueue<T, D>::push_back(const queue_type &qt)
{
    Lock lock(*this);

    typename queue_type::const_iterator it = qt.begin();
    typename queue_type::const_iterator itEnd = qt.end();
    while(it != itEnd)
    {
        _queue.push_back(*it);
        ++it;
        ++_size;
        notify();
    }
}

template<typename T, typename D> void TC_ThreadQueue<T, D>::push_front(const T& t)
{
    Lock lock(*this);

    notify();

    _queue.push_front(t);

    ++_size;
}

template<typename T, typename D> void TC_ThreadQueue<T, D>::push_front(const queue_type &qt)
{
    Lock lock(*this);

    typename queue_type::const_iterator it = qt.begin();
    typename queue_type::const_iterator itEnd = qt.end();
    while(it != itEnd)
    {
        _queue.push_front(*it);
        ++it;
        ++_size;

        notify();
    }
}

template<typename T, typename D> bool TC_ThreadQueue<T, D>::swap(queue_type &q, size_t millsecond)
{
    Lock lock(*this);

    if (_queue.empty())
    {
        if(millsecond == 0)
        {
            return false;
        }
        if(millsecond == (size_t)-1)
        {
            wait();
        }
        else
        {
            //超时了
            if(!timedWait(millsecond))
            {
                return false;
            }
        }
    }

    if (_queue.empty())
    {
        return false;
    }

    q.swap(_queue);
    //_size = q.size();
    _size = _queue.size();

    return true;
}

template<typename T, typename D> size_t TC_ThreadQueue<T, D>::size() const
{
    Lock lock(*this);
    //return _queue.size();
    return _size;
}

template<typename T, typename D> void TC_ThreadQueue<T, D>::clear()
{
    Lock lock(*this);
    _queue.clear();
    _size = 0;
}

template<typename T, typename D> bool TC_ThreadQueue<T, D>::empty() const
{
    Lock lock(*this);
    return _queue.empty();
}

}
#endif

