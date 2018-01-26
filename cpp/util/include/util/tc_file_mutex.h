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

#ifndef __TC_FILE_MUTEX_H
#define __TC_FILE_MUTEX_H

#include <unistd.h>
#include "util/tc_lock.h"

namespace tars
{
/////////////////////////////////////////////////
/** 
* @file tc_file_mutex.h 
* @brief  文件锁类.     
* 
*/
/////////////////////////////////////////////////

/**
* @brief  异常类
*/
struct TC_FileMutex_Exception : public TC_Lock_Exception
{
    TC_FileMutex_Exception(const string &buffer) : TC_Lock_Exception(buffer){};
    TC_FileMutex_Exception(const string &buffer, int err) : TC_Lock_Exception(buffer, err){};
    ~TC_FileMutex_Exception() throw() {};
};

/**
 * @brief  文件锁, 注意:只能在进程间加锁.
 */
class TC_FileMutex
{
public:

    /**
     * @brief  构造函数.
     */
    TC_FileMutex();

    /**
     * @brief  析够函数.
     */
    virtual ~TC_FileMutex();

    /**
     * @brief  初始化文件锁. 
     *  
     * @param filename 欲操作的文件的名字
     */
    void init(const std::string& filename);

    /**
    * @brief  加读锁.
    * 
    *@return 0-成功加锁，-1-加锁失败
    */
    int rlock();

    /**
    * @brief  解读锁. 
    *  
    * @return 0-成功解锁，-1-解锁失败
    */
    int unrlock();

    /**
    * @brief  尝试读锁. 
    *  
    * @throws TC_FileMutex_Exception
    * @return  加锁成功则返回false, 否则返回false
    */
    bool tryrlock();

    /**
    * @brief  加写锁. 
    *  
    * @return int
    */
    int wlock();

    /**
    * @brief  解写锁.
    */
    int unwlock();

    /**
    * @brief  尝试写锁. 
    *  
    * @return bool，加锁成功则返回false, 否则返回false 
    * @throws TC_FileMutex_Exception 
    */
    bool trywlock();

    /**
    * @brief  写锁. 
    *  
    * @return int, 0 正确
    */
    int lock(){return wlock();};

    /**
    * @brief  解写锁.
    */
    int unlock();

    /**
    * @brief  尝试解锁. 
    *  
    * @throws TC_FileMutex_Exception
    * @return int, 0 正确
    */
    bool trylock() {return trywlock();};

protected:
    /**
     * @brief  设置锁. 
     *  
     * @param fd       欲设置的文件描述词
     * @param cmd      欲操作的指令
     * @param type     三种状态，分别为F_RDLCK ,F_WRLCK ,F_UNLCK 
     * @param offset   偏移量
     * @param whence   锁定的起始位置，三种方式
     * @param len      锁定区域的大小
     * @return         int：成功则返回0，若有错误则返回-1. 
     */
    int lock(int fd, int cmd, int type, off_t offset, int whence, off_t len);

    /**
     * @brief  是否被其他进程锁了. 
     *  
     * @param fd      欲设置的文件描述词
     * @param type    三种状态，分别为F_RDLCK ,F_WRLCK ,F_UNLCK 
     * @param offset  偏移量
     * @param whence  锁定的起始位置，三种方式
     * @param len     锁定区域的大小
     * @return bool   有所返回true，无锁返回false. 
     */
    bool hasLock(int fd, int type, off_t offset, int whence, off_t len);

private:
    int _fd;
};

}

#endif

