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

#ifndef __TC_SEM_MUTEX_H
#define __TC_SEM_MUTEX_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "util/tc_lock.h"

namespace tars
{
/////////////////////////////////////////////////
/** 
* @file tc_sem_mutex.h 
* @brief 信号量锁类. 
* 
*/              
/////////////////////////////////////////////////

/**
* @brief 信号量锁异常类
*/
struct TC_SemMutex_Exception : public TC_Lock_Exception
{
    TC_SemMutex_Exception(const string &buffer) : TC_Lock_Exception(buffer){};
    TC_SemMutex_Exception(const string &buffer, int err) : TC_Lock_Exception(buffer, err){};
    ~TC_SemMutex_Exception() throw() {};
};

/**
* @brief 进程间锁, 提供两种锁机制:共享锁和排斥锁. 
*  
* 1 对于相同的key, 不同进程初始化时连接到相同的sem上
* 2 采用IPC的信号量实现
* 3 信号量采用了SEM_UNDO参数, 当进程结束时会自动调整信号量
*/
class TC_SemMutex
{
public:
    /**
     * @brief 构造函数
     */
    TC_SemMutex();

    /**
    * @brief 构造函数. 
    *  
    * @param iKey, key
    * @throws TC_SemMutex_Exception
    */
    TC_SemMutex(key_t iKey);

    /**
    * @brief 初始化. 
    *  
    * @param iKey, key
    * @throws TC_SemMutex_Exception
    * @return 无
     */
    void init(key_t iKey);

    /**
    * @brief 获取共享内存Key. 
    *  
    * @return key_t ,共享内存key
    */
    key_t getkey() const {return _semKey;}

    /**
    * @brief 获取共享内存ID. 
    *  
    * @return int ,共享内存Id
    */
    int getid() const   {return _semID;}

    /**
    * @brief 加读锁.
    * 
    *@return int
    */
    int rlock() const;

    /**
    * @brief 解读锁. 
    *  
    * @return int
    */
    int unrlock() const;

    /**
    * @brief 尝试读锁. 
    *  
    * @return bool : 加锁成功则返回false, 否则返回false
    */
    bool tryrlock() const;

    /**
    * @brief 加写锁. 
    *  
    * @return int
    */
    int wlock() const;

    /**
    * @brief 解写锁
    */
    int unwlock() const;

    /**
    * @brief 尝试写锁. 
    *  
    * @throws TC_SemMutex_Exception
    * @return bool : 加锁成功则返回false, 否则返回false
    */
    bool trywlock() const;

    /**
    * @brief 写锁. 
    *  
    * @return int, 0 正确
    */
    int lock() const        {return wlock();};

    /**
    * @brief 解写锁
    */
    int unlock() const      {return unwlock();};

    /**
    * @brief  尝试解锁. 
    *  
    * @throws TC_SemMutex_Exception
    * @return int, 0 正确
    */
    bool trylock() const    {return trywlock();};

protected:
    /**
     * 信号量ID
     */
    int _semID;

    /**
     * 信号量key
     */
    key_t _semKey;
};

}

#endif
