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

#ifndef __TC_MMAP_H
#define __TC_MMAP_H

#include <sys/mman.h>
#include <string>
#include "util/tc_ex.h"
using namespace std;

namespace tars
{
/////////////////////////////////////////////////
/** 
* @file tc_mmap.h 
* @brief  mmap封装类. 
*  
*/           
/////////////////////////////////////////////////
/**
* @brief MMap异常
*/
struct TC_Mmap_Exception : public TC_Exception
{
    TC_Mmap_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_Mmap_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_Mmap_Exception() throw() {};
};


/**
 *  @brief  mmap的操作类.
 *  
 *  说明:
 *   1: fd可以指定为-1，此时须指定flags参数中的MAP_ANONYMOUS
 *  
 *        表明进行的是匿名映射,不涉及具体的文件名,避免了文件的创建及打开,很显然只能用于具有亲缘关系的进程间通信
 *  
 *   2: 创建map时,文件一定需要有length的长度, 否则可能导致越界
 *  
 *   3: 2中说的情况一般通过产生空洞文件避免,int mmap(const char
 *  
 *      file, size_t length);
 *      
 *   实现了类似的封装, 推荐直接使用
 */
class TC_Mmap
{
public:

    /**
     * @brief 构造函数. 
     *  
     * @param bOwner, 如果拥有, 则析够的时候unmap
     */
    TC_Mmap(bool bOwner = true);

    /**
     * @brief 析够
     */
    ~TC_Mmap();

    /**
     * @brief 映射到进程空间. 
     *  
     * @param length, 映射到进程地址空间的字节数,从offset个字节算起
     * @param prot,指定共享内存的访问权限,可取值为(几个值的或):
     *              PROT_READ: 可读
     *  
     *              PROT_WRITE: 可写
     *  
     *              PROT_EXEC : 可执行
     *  
     *              PROT_NONE: 不可访问
     * @param flags, MAP_SHARED,MAP_PRIVATE,MAP_FIXED:一般取:MAP_SHARED
     * @param fd, 进程空间中的文件描述符
     * @param offset,文件的偏移地址, 一般为0
     * @throws TC_Mmap_Exception
     * @return
     */
    void mmap(size_t length, int prot, int flags, int fd, off_t offset = 0);

    /**
     * @brief 映射到进程空间，采用: 
     *  
     * PROT_READ|PROT_WRITE,MAP_SHARED方式 
     *  
     * 注意的文件大小会比length大一个字节(初始化时生成空洞文件的原因) 
     * @param file    文件名
     * @param length  映射文件的长度
     * @throws        TC_Mmap_Exception
     * @return
     */
    void mmap(const char *file, size_t length);

    /**
     * @brief 解除映射关系, 解除后不能在访问这段空间了. 
     *  
     * @throws TC_Mmap_Exception
     * @return
     */
    void munmap();

    /**
     * @brief 把共享内存中的改变写回磁盘中.
     *
     * @param bSync true:同步写回, false:异步写回
     * @throws      TC_Mmap_Exception
     * @return
     */
    void msync(bool bSync = false);

    /**
     * @brief 获取映射的指针地址.
     *
     * @return void* 映射的指针地址
     */
    void *getPointer() const    { return _pAddr; }

    /**
     * @brief 获取映射的空间大小.
     *
     * @return size_t 映射的空间大小
     */
    size_t getSize() const      { return _iLength; }

    /**
     * @brief 是否创建出来的，文件的存在可分为两种形式： 
     *  
     *        一种是创建的，一种是开始就存在的
     * @return 如果是创建出来的就返回true，否则返回false
     */
    bool iscreate() const       { return _bCreate; }

    /**
     * @brief 设置是否拥有. 
     *  
     * @param bOwner ture or fale
     */
    void setOwner(bool bOwner)  { _bOwner = bOwner; }

protected:

    /**
     * 是否拥有
     */
    bool    _bOwner;

    /**
     * 映射到进程空间的地址
     */
    void    *_pAddr;

    /**
     * 映射的空间大小
     */
    size_t  _iLength;

    /**
     * 是否创建出来的
     */
    bool    _bCreate;
};

}
#endif

