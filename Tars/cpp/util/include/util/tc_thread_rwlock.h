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

#ifndef __TC_THREAD_RWLOCK_H
#define __TC_THREAD_RWLOCK_H

#include <pthread.h>
#include "util/tc_lock.h"

#if !defined(linux) && (defined(__linux) || defined(__linux__))
#define linux
#endif

using namespace std;

namespace tars
{

/////////////////////////////////////////////////
/**
 * @file tc_thread_rwlock.h
 * @brief 读写锁
 *
 */

/////////////////////////////////////////////////

/**
 * @brief读写锁异常类
 */
struct TC_ThreadRW_Exception : public TC_Exception
{
    TC_ThreadRW_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_ThreadRW_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_ThreadRW_Exception() throw() {};
};


#if !defined(linux) || (defined __USE_UNIX98 || defined __USE_XOPEN2K)
class TC_ThreadRWLocker
{
public:
    /**
     * @brief 构造函数
     */
    TC_ThreadRWLocker();

    /**
     * @brief 析够函数
     */
    ~TC_ThreadRWLocker();

    /**
     *@brief 读锁定,调用pthread_rwlock_rdlock
     *return : 失败则抛异常TC_ThreadRW_Exception
     */
    void ReadLock() const;

    /**
     *@brief 写锁定,调用pthread_rwlock_wrlock
     *return : 失败则抛异常TC_ThreadRW_Exception
     */
    void WriteLock() const;

    /**
     *@brief 尝试读锁定,调用pthread_rwlock_tryrdlock
     *return : 失败则抛异常TC_ThreadRW_Exception
     */
    void TryReadLock() const;

    /**
     *@brief 尝试写锁定,调用pthread_rwlock_trywrlock
     *return : 失败则抛异常TC_ThreadRW_Exception
     */
    void TryWriteLock() const ;

    /**
     *@brief 解锁,调用pthread_rwlock_unlock
     *return : 失败则抛异常TC_ThreadRW_Exception
     */
    void Unlock() const;

private:

    mutable pthread_rwlock_t m_sect;

    // noncopyable
    TC_ThreadRWLocker(const TC_ThreadRWLocker&);
    TC_ThreadRWLocker& operator=(const TC_ThreadRWLocker&);

};

typedef TC_RW_RLockT<TC_ThreadRWLocker> TC_ThreadRLock;
typedef TC_RW_WLockT<TC_ThreadRWLocker> TC_ThreadWLock;

#endif

}
#endif


