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

#ifndef __TC_SHM_H__
#define __TC_SHM_H__

#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "util/tc_ex.h"

namespace tars
{
/////////////////////////////////////////////////
/** 
 * @file  tc_shm.h 
 * @brief  共享内存封装类. 
 * 
 */
/////////////////////////////////////////////////


/**
* @brief 共享内存异常类.
*/
struct TC_Shm_Exception : public TC_Exception
{
    TC_Shm_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_Shm_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_Shm_Exception() throw() {};
};

/** 
* @brief  共享内存连接类，说明: 
* 1 用于连接共享内存, 共享内存的权限是 0666 
* 2 _bOwner=false: 析够时不detach共享内存 
* 3 _bOwner=true: 析够时detach共享内存
*/
class TC_Shm
{
public:

    /**
    * @brief 构造函数.
    *  
    * @param bOwner  是否拥有共享内存,默认为false 
    */
    TC_Shm(bool bOwner = false) : _bOwner(bOwner),_shmSize(0),_shmKey(0),_bCreate(true), _pshm(NULL),_shemID(-1) {}

    /**
    * @brief 构造函数. 
    *  
    * @param iShmSize 共享内存大小
    * @param iKey     共享内存Key
    * @throws         TC_Shm_Exception
    */
    TC_Shm(size_t iShmSize, key_t iKey, bool bOwner = false);

    /**
    * @brief 析构函数.
    */
    ~TC_Shm();

    /**
    * @brief 初始化. 
    *  
    * @param iShmSize   共享内存大小
    * @param iKey       共享内存Key
    * @param bOwner     是否拥有共享内存
    * @throws           TC_Shm_Exception
    * @return Ξ
    */
    void init(size_t iShmSize, key_t iKey, bool bOwner = false);

    /** 
    * @brief 判断共享内存的类型，生成的共享内存,还是连接上的共享内存
    * 如果是生成的共享内存,此时可以根据需要做初始化 
    *  
    * @return  true,生成共享内存; false, 连接上的共享内存
    */
    bool iscreate()     {return _bCreate;}

    /**
    * @brief  获取共享内存的指针.
    *  
    * @return   void* 共享内存指针
    */
    void *getPointer() {return _pshm;}

    /**
    * @brief  获取共享内存Key.
    *  
    * @return key_t* ,共享内存key
    */
    key_t getkey()  {return _shmKey;}

    /**
    * @brief  获取共享内存ID.
    * 
    * @return int ,共享内存Id
    */
    int getid()     {return _shemID;}

    /**
    *  @brief  获取共享内存大小.
    *  
    * return size_t,共享内存大小
    */
    size_t size()   {return _shmSize;}

    /** 
    *  @brief 解除共享内存，在当前进程中解除共享内存
    * 共享内存在当前进程中无效
    * @return int
    */
    int detach();

    /** 
     *  @brief 删除共享内存.
     * 
     * 完全删除共享内存
     */
    int del();

protected:

    /**
     * 是否拥有共享内存
     */
    bool            _bOwner;

    /**
    * 共享内存大小
    */
    size_t          _shmSize;

    /**
    * 共享内存key
    */
    key_t           _shmKey;

    /**
    * 是否是生成的共享内存
    */
    bool            _bCreate;

    /**
    * 共享内存
    */
    void            *_pshm;

    /**
    * 共享内存id
    */
    int             _shemID;

};

}

#endif
