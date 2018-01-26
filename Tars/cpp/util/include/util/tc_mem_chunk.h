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

#ifndef    __TC_MEM_CHUNK_H__
#define __TC_MEM_CHUNK_H__

#include <string>
#include <vector>

using namespace std;

namespace tars
{
/////////////////////////////////////////////////
/**  
* @file tc_mem_chunk.h
* @brief 内存分配器 
*          
*/ 
/////////////////////////////////////////////////
/**
* @brief 原始内存块, 由TC_MemChunkAllocator来负责分配和维护
*
* 将连续的内存分成大小相同的块,形成链表,并能够分配和释放这些大小相同的快
*/
class TC_MemChunk
{
public:

    /**
    * @brief 构造函数
    */
    TC_MemChunk();

    /**
     * @brief 计算Chunk需要的内存块大小
     * @param iBlockSize
     * @param iBlockCount
     *
     * @return size_t
     */
    static size_t calcMemSize(size_t iBlockSize, size_t iBlockCount);

    /**
     * @brief 计算block个数
     * @param iMemSize
     * @param iBlockSize
     *
     * @return size_t
     */
    static size_t calcBlockCount(size_t iMemSize, size_t iBlockSize);

     /**
     * @brief tagChunkHead的大小
     *
     * @return size_t
     */
    static size_t getHeadSize() { return sizeof(tagChunkHead); }

    /**
     * @brief 初始化, 要保证p指向的内存指针=getMemSize大小
     * @param pAddr        地址, 换到应用程序的绝对地址
     * @param iBlockSize   block大小
     * @param iBlockCount  block个数
     */
    void create(void *pAddr, size_t iBlockSize, size_t iBlockCount);

    /**
     * @brief 连接上
     * @param pAddr 地址, 换到应用程序的绝对地址
     */
    void connect(void *pAddr);

    /**
     * @brief 获取block的大小 
     * @return block的大小
     */
    size_t getBlockSize() const { return _pHead->_iBlockSize; }

    /**
     * @brief 获取所有的内存大小
     *
     * @return 所有的内存大小
     */
    size_t getMemSize() const { return _pHead->_iBlockSize * _pHead->_iBlockCount + sizeof(tagChunkHead); }

    /**
     * @brief 获取可以存放数据的总容量
     *
     * @return 总容量
     */
    size_t getCapacity() const { return _pHead->_iBlockSize * _pHead->_iBlockCount; }

    /**
     * @brief 获取block的个数
     *
     * @return block的个数
     */
    size_t getBlockCount() const { return _pHead->_iBlockCount; }

    /**
     * @brief 是否还有可用block 
     * @return 可用返回true，否则返回false
     */
    bool isBlockAvailable() const { return _pHead->_blockAvailable > 0; }

    /**
     * @brief  获取可以利用的block的个数 
     * @return 可用的block的个数 
     */
    size_t getBlockAvailableCount() const { return _pHead->_blockAvailable; }

    /**
     * @brief 分配一个区块
     *
     * @return 指向分配的区块的指针
     */
    void* allocate();

    /**
    * @brief 分配一个区块. 
    * 返回以1为基数的区块索引，没有可分配空间时返回 0 , 
    * 通查索引都是比较小(即使在64位操作系统上), 4个字节以内 
    * 便于节省内存 
    */
    void* allocate2(size_t &iIndex);

    /**
     * @brief 释放区块
     * @param 指向要释放区块的指针
     */
    void deallocate(void *pAddr);

    /**
     * @brief 根据索引释放区块
     * @param 区块索引
     */
    void deallocate2(size_t iIndex);

    /**
     * @brief 重建
     */
    void rebuild();

    /**
     * @brief chunk头部
     */
    struct tagChunkHead
    {
        size_t  _iBlockSize;            /**区块大小*/
        size_t  _iBlockCount;           /**block个数*/
        size_t  _firstAvailableBlock;   /**第一个可用的block索引*/
        size_t  _blockAvailable;        /**可用block个数*/
    }__attribute__((packed));

    /**
     * @brief 获取头部信息
     *
     * @return 头部信息
     */
    tagChunkHead getChunkHead() const;

    /**
    * @brief 根据索引获取绝对地址
    */
    void* getAbsolute(size_t iIndex);

    /**
     * @brief 绝对地址换成索引
     * 
     * @param pAddr    绝对地址 
     * @return size_t  索引值
     */
    size_t getRelative(void *pAddr);

protected:
    /**
     * @brief 初始化
     */
    void init(void *pAddr);

private:

    /**
     * @brief 区块头指针
     */
    tagChunkHead    *_pHead;

    /**
     * @brief 数据区指针
     */
    unsigned char   *_pData;
};

/**
* @brief 内存块分配器，提供分配和释放的功能  
*  
* 只能分配相同大小的内存块,最下层的原始内存块分配, 
*  
* 内存结构: 内存块长度, 4个字节 ;
*  
* Block大 小, 4个字节; 
*  
* Chunk个数, 4个字节 ;
*  
* TC_MemChunk 暂时只支持同一个Block大小的MemChunk 
*/
class TC_MemChunkAllocator
{
public:

    /**
    * @brief 构造函数
    */
    TC_MemChunkAllocator();

    /**
     * @brief 初始化
     * @param pAddr, 地址, 换到应用程序的绝对地址
     * @param iSize, 内存大小
     * @param iBlockSize, block的大小
     */
    void create(void *pAddr, size_t iSize, size_t iBlockSize);

    /**
     * @brief 连接
     * @param pAddr 地址, 换到应用程序的绝对地址
     */
    void connect(void *pAddr);

    /**
     * @brief 获取头地址指针
     */
    void *getHead()    const       { return _pHead; }

    /**
     * @brief 每个block的大小
     *
     * @return block的大小
     */
    size_t getBlockSize()  const { return _pHead->_iBlockSize; }

    /**
     * @brief 总计内存大小 
     * @return 内存大小
     */
    size_t getMemSize()  const { return _pHead->_iSize; }

    /**
     * @brief 可以存放数据的总容量 
     * @return 总容量
     */
    size_t getCapacity() const { return _chunk.getCapacity(); }

    /**
     * @brief 分配一个区块,绝对地址 
     */
    void* allocate();

    /**
    * @brief 分配一个区块，返回以1为基数的区块索引， 
    *        没有可分配空间时返回0
    * @param 区块索引
    */
    void* allocate2(size_t &iIndex);

    /**
     * @brief 释放区块, 绝对地址
     * @param pAddr 区块的绝对地址
     */
    void deallocate(void *pAddr);

    /**
     * @brief 释放区块 
     * @param iIndex 区块索引
     */
    void deallocate2(size_t iIndex);

    /**
     * @brief 获取所有chunk的区块合计的block的个数 
     * @return 合计的block的个数
     */
    size_t blockCount() const           { return _chunk.getBlockCount(); }

    /**
    * @brief 根据索引获取绝对地址 
    * @param 索引
    */
    void* getAbsolute(size_t iIndex)    { return _chunk.getAbsolute(iIndex); };

    /**
     * @brief 绝对地址换成索引
     * @param pAddr   绝对地址 
     * @return size_t 索引
     */
    size_t getRelative(void *pAddr)     { return _chunk.getRelative(pAddr); };

    /**
     * @brief 获取头部信息 
     * @return 头部信息
     */
    TC_MemChunk::tagChunkHead getBlockDetail() const;

    /**
     * @brief 重建
     */
    void rebuild();

    /**
     * @brief 头部内存块
     */
    struct tagChunkAllocatorHead
    {
        size_t  _iSize;
        size_t  _iBlockSize;
    }__attribute__((packed));

    /**
     * @brief 取获头部大小 
     * @return 头部大小
     */
    static size_t getHeadSize() { return sizeof(tagChunkAllocatorHead); }

protected:

    /**
     * @brief 初始化
     */
    void init(void *pAddr);

    /**
     * @brief 初始化
     */
    void initChunk();

    /**
     * @brief 连接
     */
    void connectChunk();

    /**
    *@brief 不允许copy构造 
    */ 
    TC_MemChunkAllocator(const TC_MemChunkAllocator &);
    /**
     *@brief 不允许赋值  
     */ 
    TC_MemChunkAllocator& operator=(const TC_MemChunkAllocator &);
    bool operator==(const TC_MemChunkAllocator &mca) const;
    bool operator!=(const TC_MemChunkAllocator &mca) const;

private:

    /**
     * 头指针
     */
    tagChunkAllocatorHead   *_pHead;

    /**
     *  chunk开始的指针
     */
    void                    *_pChunk;

    /**
     * chunk链表
     */
    TC_MemChunk             _chunk;
};

/**
 * @brief 多块分配器,可以分配多个不同大小的块
 *  
 * 内部每种块用TC_MemChunkAllocator来分配, 
 *  
 * 每种大小不同块的个数是相同的, 内存块分配的策略如下: 
 *  
 *  确定需要分配多大内存，假设需要分配A字节的内存；
 *  
 * 分配大小大于>=A的内存块，优先分配大小最接近的；
 *  
 * 如果都没有合适内存块，则分配大小<A的内存块，优先分配大小最接近的；
 *  
 * 如果仍然没有合适内存块，则返回NULL；
 *  
 * 初始化时指定:最小块大 小, 最大块大小, 块间大小比值
 *  
 * 自动计算出块的个数(每种大小块的个数相同) 
 */
class TC_MemMultiChunkAllocator
{
public:

    /**
    * @brief 构造函数
    */
    TC_MemMultiChunkAllocator();

    /**
     * @brief 析够函数
     */
    ~TC_MemMultiChunkAllocator();


    /**
     * @brief 初始化
     * @param pAddr          地址, 换到应用程序的绝对地址
     * @param iSize          内存大小
     * @param iMinBlockSize  block的大小下限
     * @param iMaxBlockSize  block的大小上限
     * @param fFactor        因子
     */
    void create(void *pAddr, size_t iSize, size_t iMinBlockSize, size_t iMaxBlockSize, float fFactor = 1.1);

    /**
     * @brief 连接上
     * @param pAddr 地址, 换到应用程序的绝对地址
     */
    void connect(void *pAddr);

    /**
     * @brief 扩展空间
     * 
     * @param pAddr 已经是空间被扩展之后的地址
     * @param iSize
     */
    void append(void *pAddr, size_t iSize);

    /**
     * @brief 获取每个block的大小, 包括后续增加的内存块的大小
     *
     * @return vector<size_t>block大小的vector
     */
    vector<size_t> getBlockSize()  const;

    /**
     * @brief 每个block中chunk个数(都是相等的) 
     * @return chunk个数 
     */
    size_t getBlockCount() const { return _iBlockCount; }

    /**
     * @brief 获取每个块头部信息, 包括后续增加的内存块的大小
     * @param i
     *
     * @return vector<TC_MemChunk::tagChunkHead>
     */
    vector<TC_MemChunk::tagChunkHead> getBlockDetail() const;

    /**
     * @brief 总计内存大小, 包括后续增加的内存块的大小
     *
     * @return size_t
     */
    size_t getMemSize()  const              { return _pHead->_iTotalSize; }

    /**
     * @brief 真正可以放数据的容量, 包括后续增加的内存块的数据容量 
     * @return 可以放数据的容量
     */
    size_t getCapacity() const;

    /**
     * @brief 一个chunk的block个数, 包括后续增加的内存块的 
     * @return vector<size_t>block个数
     */
    vector<size_t> singleBlockChunkCount() const;

    /**
     * @brief 所有chunk的区块合计的block的个数 
     * @return 合计的block的个数
     */
    size_t allBlockChunkCount() const;

    /**
     * @brief 分配一个区块,绝对地址 
     * @param iNeedSize   需要分配的大小
     * @param iAllocSize  分配的数据块大小
     */
    void* allocate(size_t iNeedSize, size_t &iAllocSize);

    /**
     * @brief 分配一个区块, 返回区块索引
     * @param iNeedSize    需要分配的大小
     * @param iAllocSize   分配的数据块大小
     * @param               size_t，以1为基数的索引，0表示无效
     */
    void* allocate2(size_t iNeedSize, size_t &iAllocSize, size_t &iIndex);

    /**
     * @brief 释放区块
     * @param p 绝对地址
     */
    void deallocate(void *pAddr);

    /**
     * @brief 释放区块
     * @param iIndex 区块索引
     */
    void deallocate2(size_t iIndex);

    /**
     * @brief 重建
     */
    void rebuild();

    /**
     * @brief 相对索引换算成绝对地址
     * @param iIndex 相对索引 
     * @return       绝对地址指针
     */
    void *getAbsolute(size_t iIndex);

    /**
     * @brief  绝对地址换成索引地址
     * @param  绝对地址
     * @return 索引地址
     */
    size_t getRelative(void *pAddr);

    /**
     * @brief 头部内存块
     */
    struct tagChunkAllocatorHead
    {
        size_t  _iSize;             /**当前块大小*/
        size_t  _iTotalSize;        /**后续分配块合在一起的大小*/
        size_t  _iMinBlockSize;
        size_t  _iMaxBlockSize;
        float   _fFactor;
        size_t  _iNext;             /**下一个分配器地址, 如果没有则为0*/
    }__attribute__((packed));

    /**
     * @brief 头部大小
     *
     * @return size_t
     */
    static size_t getHeadSize() { return sizeof(tagChunkAllocatorHead); }

protected:

    /**
     * @brief 初始化
     */
    void init(void *pAddr);

    /**
     * @brief 计算
     */
    void calc();

    /**
     * @brief 清空
     */
    void clear();

    /**
     * @brief 最后一个分配器
     * 
     * @return TC_MemMultiChunkAllocator*
     */
    TC_MemMultiChunkAllocator *lastAlloc();

    /**
     *@brief 不允许copy构造
     */
    TC_MemMultiChunkAllocator(const TC_MemMultiChunkAllocator &);
    /** 
      * @brief 不允许赋值
      */
    TC_MemMultiChunkAllocator& operator=(const TC_MemMultiChunkAllocator &);
    bool operator==(const TC_MemMultiChunkAllocator &mca) const;
    bool operator!=(const TC_MemMultiChunkAllocator &mca) const;

private:

    /**
     * 头指针
     */
    tagChunkAllocatorHead   *_pHead;

    /**
     *  chunk开始的指针
     */
    void                    *_pChunk;

    /**
     * 区块大小
     */
    vector<size_t>          _vBlockSize;

    /**
     * 每个chunk中block的个数
     */
    size_t                  _iBlockCount;

    /**
     * chunk链表
     */
    vector<TC_MemChunkAllocator*>       _allocator;

    /**
     * 所有的索引个数
     */
    size_t                              _iAllIndex;

    /**
     * 后续的多块分配器
     */
    TC_MemMultiChunkAllocator           *_nallocator;
};

}

#endif
