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

#ifndef __TC_MEM_QUEUE_H__
#define __TC_MEM_QUEUE_H__

#include <string>

using namespace std;

namespace tars
{
/////////////////////////////////////////////////
/** 
 * @file   tc_mem_queue.h 
 * @brief  共享内存循环队列类. 
 *  
 */
             
/////////////////////////////////////////////////
/**
* @brief 内存循环队列, 同时读写必须加锁 
*  
*  内存循环队列，建议不要直接使用该类，通过jmem组件来使用
*  
* 做了保护，即使被kill掉，队列不会坏掉，最多错误一个数据
*/
class TC_MemQueue
{
public:

    /**
    * @brief 构造函数
    */
    TC_MemQueue();

    /**
    * @brief 析构
    */
    ~TC_MemQueue(){}

    /**
    * @brief 初始化
    * @param pAddr 指令队列空间的指针
    * @param iSize 空间的指针
    */
    void create(void *pAddr, size_t iSize);

    /**
    * @brief 连接上队列
    * @param pAddr 指令队列空间的指针
    */
    void connect(void *pAddr, size_t iSize);

    /**
    * @brief 弹出数据
    * @param sOut   输出的数据快
    * @return       bool,true:正确, false: 错误,无数据输出,队列空 
    */
    bool pop_front(string &sOut);

    /**
    * @brief 进入数据
    * @param  sIn 输入的数据快
    * @return     bool,true:正确, false: 错误, 队列满
    */
    bool push_back(const string &sIn);

    /**
    * @brief 进入数据
    * @param pvIn   输入的数据快
    * @param iSize  输入数据块长度
    * @return       bool,true:正确, false: 错误, 队列满
    */
    bool push_back(const char *pvIn, size_t iSize);

    /**
    * @brief 队列是否满
    * @param iSize 输入数据块长度
    * @return      bool,true:满, false: 非满
    */
    bool isFull(size_t iSize);


    /**
     * 获取空闲的空间大小
     */
    size_t getFreeSize();

    /**
    * @brief 队列是否空
    * @return  bool,true: 满, false: 非满
    */
    bool isEmpty();

    /**
    * @brief 队列中元素个数, 不加锁的情况下不保证一定正确
    * @return size_t, 元素个数
    */
    size_t elementCount();

    /**
    * @brief 队列长度(字节), 
    *        小于总存储区长度(总存储区长度包含了控制快)
    * @return size_t，队列长度
    */
    size_t queueSize();

    /**
    * @brief 共享内存长度
    * @return size_t 共享内存长度
    */
    size_t memSize() const {return _size;};

protected:
    /**
     * @brief 修改具体的值
     * @param iModifyAddr   需要被修改的值
     * @param iModifyValue  替换的值
     */
    void update(void* iModifyAddr, size_t iModifyValue);

    /**
     * @brief 修改具体的值
     * @param iModifyAddr   需要被修改的值
     * @param iModifyValue  替换的值
     */
    void update(void* iModifyAddr, bool bModifyValue);

    /**
     * @brief 修改更新到内存中
     */
    void doUpdate(bool bUpdate = false);

protected:

    /**
    *  @brief 队列控制结构
    */
    struct CONTROL_BLOCK
    {
        size_t iMemSize;            /**内存大小*/
        size_t iTopIndex;           /**顶部元素索引,内容结束地址*/
        size_t iBotIndex;           /**底部元素索引,内容开始地址*/
        size_t iPushCount;          /**队列中进入元素的个数*/
        size_t iPopCount;           /**队列中弹出元素的个数*/
    }__attribute__((packed));

    /**
     * @brief 需要修改的地址
     */
    struct tagModifyData
    {
        size_t  _iModifyAddr;       /**修改的地址*/
        char    _cBytes;            /**字节数*/
        size_t  _iModifyValue;      /**值*/
    }__attribute__((packed));

    /**
     * @brief 修改数据块头部
     */
    struct tagModifyHead
    {
        char            _cModifyStatus;         /**修改状态: 0:目前没有人修改, 1: 开始准备修改, 2:修改完毕, 没有copy到内存中*/
        size_t          _iNowIndex;             /**更新到目前的索引, 不能操作10个*/
        tagModifyData   _stModifyData[5];       /**一次最多5次修改*/
    }__attribute__((packed));

    /**
    * 队列控制快(内存的起点)
    */
    CONTROL_BLOCK   *_pctrlBlock;

    /**
    * 内存数据地址
    */
    void            *_paddr;

    /**
    * 共享内存
    */
    size_t            _size;

    /**
     * 修改数据块
     */
    tagModifyHead   *_pstModifyHead;


};

}

#endif
