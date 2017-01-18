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

#ifndef _TC_LOCK_H
#define _TC_LOCK_H

#include <string>
#include <stdexcept>
#include <cerrno>
#include "util/tc_ex.h"

using namespace std;

namespace tars
{
/////////////////////////////////////////////////
/**
 * @file tc_lock.h 
 * @brief  锁类 
 */           
/////////////////////////////////////////////////


/**
* @brief  锁异常
*/
struct TC_Lock_Exception : public TC_Exception
{
    TC_Lock_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_Lock_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_Lock_Exception() throw() {};
};

/**
 * @brief  锁模板类其他具体锁配合使用，
 * 构造时候加锁，析够的时候解锁
 */
template <typename T>
class TC_LockT
{
public:

    /**
     * @brief  构造函数，构造时枷锁
     *  
     * @param mutex 锁对象
     */
    TC_LockT(const T& mutex) : _mutex(mutex)
    {
        _mutex.lock();
        _acquired = true;
    }

    /**
     * @brief  析构，析构时解锁
     */
    virtual ~TC_LockT()
    {
        if (_acquired)
        {
            _mutex.unlock();
        }
    }

    /**
     * @brief  上锁, 如果已经上锁,则抛出异常
     */
    void acquire() const
    {
        if (_acquired)
        {
            throw TC_Lock_Exception("thread has locked!");
        }
        _mutex.lock();
        _acquired = true;
    }

    /**
     * @brief  尝试上锁.
     *
     * @return  成功返回true，否则返回false
     */
    bool tryAcquire() const
    {
        _acquired = _mutex.tryLock();
        return _acquired;
    }

    /**
     * @brief  释放锁, 如果没有上过锁, 则抛出异常
     */
    void release() const
    {
        if (!_acquired)
        {
            throw TC_Lock_Exception("thread hasn't been locked!");
        }
        _mutex.unlock();
        _acquired = false;
    }

    /**
     * @brief  是否已经上锁.
     *
     * @return  返回true已经上锁，否则返回false
     */
    bool acquired() const
    {
        return _acquired;
    }

protected:

    /**
     * @brief 构造函数
     * 用于锁尝试操作，与TC_LockT相似
     *  
     */
    TC_LockT(const T& mutex, bool) : _mutex(mutex)
    {
        _acquired = _mutex.tryLock();
    }

private:

    // Not implemented; prevents accidental use.
    TC_LockT(const TC_LockT&);
    TC_LockT& operator=(const TC_LockT&);

protected:
    /**
     * 锁对象
     */
    const T&        _mutex;

    /**
     * 是否已经上锁
     */
    mutable bool _acquired;
};

/**
 * @brief  尝试上锁
 */
template <typename T>
class TC_TryLockT : public TC_LockT<T>
{
public:

    TC_TryLockT(const T& mutex) : TC_LockT<T>(mutex, true)
    {
    }
};

/**
 * @brief  空锁, 不做任何锁动作
 */
class TC_EmptyMutex
{
public:
    /**
    * @brief  写锁.
    *  
    * @return int, 0 正确
    */
    int lock()  const   {return 0;}

    /**
    * @brief  解写锁
    */
    int unlock() const  {return 0;}

    /**
    * @brief  尝试解锁. 
    *  
    * @return int, 0 正确
    */
    bool trylock() const {return true;}
};

/**
 * @brief  读写锁读锁模板类
 * 构造时候加锁，析够的时候解锁
 */

template <typename T>
class TC_RW_RLockT
{
public:
    /**
     * @brief  构造函数，构造时枷锁
     *
     * @param lock 锁对象
     */
    TC_RW_RLockT(T& lock)
        : _rwLock(lock),_acquired(false)
    {
        _rwLock.ReadLock();
        _acquired = true;
    }

    /**
     * @brief 析构时解锁
     */
    ~TC_RW_RLockT()
    {
        if (_acquired)
        {
            _rwLock.Unlock();
        }
    }
private:
    /**
     *锁对象
     */
    const T& _rwLock;

    /**
     * 是否已经上锁
     */
    mutable bool _acquired;

    TC_RW_RLockT(const TC_RW_RLockT&);
    TC_RW_RLockT& operator=(const TC_RW_RLockT&);
};

template <typename T>
class TC_RW_WLockT
{
public:
    /**
     * @brief  构造函数，构造时枷锁
     *
     * @param lock 锁对象
     */
    TC_RW_WLockT(T& lock)
        : _rwLock(lock),_acquired(false)
    {
        _rwLock.WriteLock();
        _acquired = true;
    }
    /**
     * @brief 析构时解锁
     */
    ~TC_RW_WLockT()
    {
        if(_acquired)
        {
            _rwLock.Unlock();
        }
    }
private:
    /**
     *锁对象
     */
    const T& _rwLock;
    /**
     * 是否已经上锁
     */
    mutable bool _acquired;

    TC_RW_WLockT(const TC_RW_WLockT&);
    TC_RW_WLockT& operator=(const TC_RW_WLockT&);
};

};
#endif

