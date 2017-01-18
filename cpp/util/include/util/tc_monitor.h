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

#ifndef _TC_MONITOR_H
#define _TC_MONITOR_H

#include "util/tc_thread_mutex.h"
#include "util/tc_thread_cond.h"

namespace tars
{
/////////////////////////////////////////////////
/** 
 * @file tc_monitor.h 
 * 
 */           
/////////////////////////////////////////////////
/**
 * @brief 线程锁监控模板类.
 * 通常线程锁，都通过该类来使用，而不是直接用TC_ThreadMutex、TC_ThreadRecMutex 
 *  
 * 该类将TC_ThreadMutex/TC_ThreadRecMutex 与TC_ThreadCond结合起来； 
 */
template <class T, class P>
class TC_Monitor
{
public:

    /**
     * @brief 定义锁控制对象
     */
    typedef TC_LockT<TC_Monitor<T, P> > Lock;
    typedef TC_TryLockT<TC_Monitor<T, P> > TryLock;

    /**
     * @brief 构造函数
     */
    TC_Monitor() : _nnotify(0)
    {
    }

    /**
     * @brief 析够
     */
    virtual ~TC_Monitor()
    {
    }

    /**
     * @brief 锁
     */
    void lock() const
    {
        _mutex.lock();
        _nnotify = 0;
    }

    /**
     * @brief 解锁, 根据上锁的次数通知
     */
    void unlock() const
    {
        notifyImpl(_nnotify);
        _mutex.unlock();
    }

    /**
     * @brief 尝试锁.
     *
     * @return bool
     */
    bool tryLock() const
    {
        bool result = _mutex.tryLock();
        if(result)
        {
            _nnotify = 0;
        }
        return result;
    }

    /**
     * @brief 等待,当前调用线程在锁上等待，直到事件通知，
     */
    void wait() const
    {
        notifyImpl(_nnotify);

        try
        {
            _cond.wait(_mutex);
        }
        catch(...)
        {
            _nnotify = 0;
            throw;
        }

        _nnotify = 0;
    }

    /**
     * @brief 等待时间,当前调用线程在锁上等待，直到超时或有事件通知
     *  
     * @param millsecond 等待时间
     * @return           false:超时了, ture:有事件来了
     */
    bool timedWait(int millsecond) const
    {
        notifyImpl(_nnotify);

        bool rc;

        try
        {
            rc = _cond.timedWait(_mutex, millsecond);
        }
        catch(...)
        {
            _nnotify = 0;
            throw;
        }

        _nnotify = 0;
        return rc;
    }

    /**
     * @brief 通知某一个线程醒来 
     *  
     * 通知等待在该锁上某一个线程醒过来 ,调用该函数之前必须加锁, 
     *  
     * 在解锁的时候才真正通知 
     */
    void notify()
    {
        if(_nnotify != -1)
        {
            ++_nnotify;
        }
    }

    /**
     * @brief 通知等待在该锁上的所有线程醒过来，
     * 注意调用该函数时必须已经获得锁.
     *  
     * 该函数调用前之必须加锁, 在解锁的时候才真正通知 
     */
    void notifyAll()
    {
        _nnotify = -1;
    }

protected:

    /**
     * @brief 通知实现. 
     *  
     * @param nnotify 上锁的次数
     */
    void notifyImpl(int nnotify) const
    {
        if(nnotify != 0)
        {
            if(nnotify == -1)
            {
                _cond.broadcast();
                return;
            }
            else
            {
                while(nnotify > 0)
                {
                    _cond.signal();
                    --nnotify;
                }
            }
        }
    }

private:

     /** 
      * @brief noncopyable
      */
    TC_Monitor(const TC_Monitor&);
    void operator=(const TC_Monitor&);

protected:

    /**
     * 上锁的次数
     */
    mutable int     _nnotify;
    mutable P       _cond;
    T               _mutex;
};

/**
 * 普通线程锁
 */
typedef TC_Monitor<TC_ThreadMutex, TC_ThreadCond> TC_ThreadLock;

/**
 * 循环锁(一个线程可以加多次锁)
 */
typedef TC_Monitor<TC_ThreadRecMutex, TC_ThreadCond> TC_ThreadRecLock;

}
#endif

