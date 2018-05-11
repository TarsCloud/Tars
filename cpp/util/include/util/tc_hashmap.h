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

#ifndef __TC_HASHMAP_H__
#define __TC_HASHMAP_H__

#include <vector>
#include <memory>
#include <cassert>
#include <iostream>
#include <functional>
#include "util/tc_ex.h"
#include "util/tc_mem_vector.h"
#include "util/tc_pack.h"
#include "util/tc_mem_chunk.h"
#include "util/tc_hash_fun.h"

namespace tars
{
/////////////////////////////////////////////////
/**
* @file tc_hashmap.h 
* @brief  hashmap类 
*/            
/////////////////////////////////////////////////
/**
* @brief Hash map异常类
*/
struct TC_HashMap_Exception : public TC_Exception
{
    TC_HashMap_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_HashMap_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_HashMap_Exception() throw(){};
};

////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  基于内存的hashmap, 所有操作需要自己加锁 
 *  
 *内存hashmap，不要直接使用该类，通过jmem组件来使用. 
 * 
 *该hashmap通过TC_MemMutilChunkAllocator来分配空间，支持不同大小的内存块的分配； 
 * 
 *支持内存和共享内存,对接口的所有操作都需要加锁,*内部有脏数据链，支持数据缓写； 
 * 
 *当数据超过一个数据块时，则会拼接多个数据块； 
 * 
 *Set时当数据块用完，自动淘汰最长时间没有访问的数据，也可以不淘汰，直接返回错误； 
 * 
 *支持dump到文件，或从文件load； 
 */
class TC_HashMap
{
public:
    struct HashMapIterator;
    struct HashMapLockIterator;

    /**
     * @brief 操作数据
     */
    struct BlockData
    {
        string  _key;       /**数据Key*/
        string  _value;     /**数据value*/
        bool    _dirty;     /**是否是脏数据*/
        time_t  _synct;     /**sync time, 不一定是真正的回写时间*/
        BlockData()
        : _dirty(false)
        , _synct(0)
        {
        }
    };

    ///////////////////////////////////////////////////////////////////////////////////
    /**
    * @brief 内存数据块,读取和存放数据 
    */
    class Block
    {
    public:

        /**
         * @brief block数据头
         */
        struct tagBlockHead
        {
            uint32_t    _iSize;         /**block的容量大小*/
            uint32_t    _iIndex;        /**hash的索引*/
            size_t      _iBlockNext;    /**下一个Block,tagBlockHead, 没有则为0*/
            size_t      _iBlockPrev;    /**上一个Block,tagBlockHead, 没有则为0*/
            size_t      _iSetNext;      /**Set链上的上一个Block*/
            size_t      _iSetPrev;      /**Set链上的上一个Block*/
            size_t      _iGetNext;      /**Get链上的上一个Block*/
            size_t      _iGetPrev;      /**Get链上的上一个Block*/
            time_t      _iSyncTime;     /**上次缓写时间*/
            bool        _bDirty;        /**是否是脏数据*/
            bool        _bOnlyKey;      /**是否只有key, 没有内容*/
            bool        _bNextChunk;    /**是否有下一个chunk*/
            union
            {
                size_t  _iNextChunk;    /**下一个Chunk块, _bNextChunk=true时有效, tagChunkHead*/
                size_t  _iDataLen;      /**当前数据块中使用了的长度, _bNextChunk=false时有效*/
            };
            char        _cData[0];      /**数据开始部分*/
        }__attribute__((packed));

        /**
         * @brief 非头部的block, 称为chunk
         */
        struct tagChunkHead
        {
            uint32_t    _iSize;         /**block的容量大小*/
            bool        _bNextChunk;    /**是否还有下一个chunk*/
            union
            {
                size_t  _iNextChunk;    /**下一个数据块, _bNextChunk=true时有效, tagChunkHead*/
                size_t  _iDataLen;      /**当前数据块中使用了的长度, _bNextChunk=false时有效*/
            };
            char        _cData[0];      /**数据开始部分*/
        }__attribute__((packed));

        /**
         * @brief 构造函数
         * @param Map
         * @param iAddr 当前MemBlock的地址
         */
        Block(TC_HashMap *pMap, size_t iAddr)
        : _pMap(pMap)
        , _iHead(iAddr)
        {
        }

        /**
         * @brief copy
         * @param mb
         */
        Block(const Block &mb)
        : _pMap(mb._pMap)
        , _iHead(mb._iHead)
        {
        }

        /**
         *
         * @param mb
         *
         * @return Block&
         */
        Block& operator=(const Block &mb)
        {
            _iHead  = mb._iHead;
            _pMap   = mb._pMap;
            return (*this);
        }

        /**
         *
         * @param mb
         *
         * @return bool
         */
        bool operator==(const Block &mb) const { return _iHead == mb._iHead && _pMap == mb._pMap; }

        /**
         *
         * @param mb
         *
         * @return bool
         */
        bool operator!=(const Block &mb) const { return _iHead != mb._iHead || _pMap != mb._pMap; }

        /**
         * 获取block头绝对地址
         * @param iAddr
         *
         * @return tagChunkHead*
         */
        tagBlockHead *getBlockHead(size_t iAddr) { return ((tagBlockHead*)_pMap->getAbsolute(iAddr)); }

        /**
         * 获取MemBlock头地址
         *
         * @return void*
         */
        tagBlockHead *getBlockHead() {return getBlockHead(_iHead);}

        /**
         * 头部
         *
         * @return size_t
         */
        size_t getHead() { return _iHead;}

        /**
         * @brief 当前桶链表最后一个block的头部
         *
         * @return size_t
         */
        size_t getLastBlockHead();

        /**
         * @brief 最新Get时间
         *
         * @return time_t
         */
        time_t getSyncTime() { return getBlockHead()->_iSyncTime; }

        /**
         * @brief 设置回写时间
         * @param iSyncTime
         */
        void setSyncTime(time_t iSyncTime) { getBlockHead()->_iSyncTime = iSyncTime; }

        /**
         * @brief 获取Block中的数据
         *
         * @return int
         *          TC_HashMap::RT_OK, 正常, 其他异常
         *          TC_HashMap::RT_ONLY_KEY, 只有Key
         *          其他异常
         */
        int getBlockData(TC_HashMap::BlockData &data);

        /**
         * @brief 获取数据
         * @param pData
         * @param iDatalen
         * @return int,
         *          TC_HashMap::RT_OK, 正常
         *          其他异常
         */
        int get(void *pData, size_t &iDataLen);

        /**
         * @brief 获取数据
         * @param s
         * @return int
         *          TC_HashMap::RT_OK, 正常
         *          其他异常
         */
        int get(string &s);

        /**
         * @brief 设置数据
         * @param pData
         * @param iDatalen
         * @param vtData, 淘汰的数据
         */
        int set(const void *pData, size_t iDataLen, bool bOnlyKey, vector<TC_HashMap::BlockData> &vtData);

        /**
         * @brief 是否是脏数据
         *
         * @return bool
         */
        bool isDirty()      { return getBlockHead()->_bDirty; }

        /**
         * @brief 设置数据
         * @param b
         */
        void setDirty(bool b);

        /**
         * @brief 是否只有key
         *
         * @return bool
         */
        bool isOnlyKey()    { return getBlockHead()->_bOnlyKey; }

        /**
         * @brief 当前元素移动到下一个block
         * @return true, 移到下一个block了, false, 没有下一个block
         *
         */
        bool nextBlock();

        /**
         * @brief 当前元素移动到上一个block
         * @return true, 移到下一个block了, false, 没有下一个block
         *
         */
        bool prevBlock();

        /**
         * @brief 释放block的所有空间
         */
        void deallocate();

        /**
         * @brief 新block时调用该函数,分配一个新的block 
         * @param index, hash索引
         * @param iAllocSize, 内存大小
         */
        void makeNew(size_t index, size_t iAllocSize);

        /**
         * @brief 从Block链表中删除当前Block,只对Block有效, 
         *        对Chunk是无效的
         * @return
         */
        void erase();

        /**
         * @brief 刷新set链表, 放在Set链表头部
         */
        void refreshSetList();

        /**
         * @brief 刷新get链表, 放在Get链表头部
         */
        void refreshGetList();

    protected:

        /**
         * @brief 获取Chunk头绝对地址
         *
         * @return tagChunkHead*
         */
        tagChunkHead *getChunkHead() {return getChunkHead(_iHead);}

        /**
         * @brief 获取chunk头绝对地址
         * @param iAddr
         *
         * @return tagChunkHead*
         */
        tagChunkHead *getChunkHead(size_t iAddr) { return ((tagChunkHead*)_pMap->getAbsolute(iAddr)); }

        /**
         * @brief 从当前的chunk开始释放
         * @param iChunk 释放地址
         */
        void deallocate(size_t iChunk);

        /**
         * @brief 如果数据容量不够, 则新增加chunk, 不影响原有数据
         * 使新增加的总容量大于iDataLen,释放多余的chunk 
         * @param iDataLen
         *
         * @return int,
         */
        int allocate(size_t iDataLen, vector<TC_HashMap::BlockData> &vtData);

        /**
         * @brief 挂接chunk, 如果core则挂接失败, 保证内存块还可以用
         * @param pChunk
         * @param chunks
         *
         * @return int
         */
        int joinChunk(tagChunkHead *pChunk, const vector<size_t> chunks);

        /**
         * @brief 分配n个chunk地址, 
         *        注意释放内存的时候不能释放正在分配的对象
         * @param fn, 分配空间大小
         * @param chunks, 分配成功返回的chunks地址列表
         * @param vtData, 淘汰的数据
         * @return int
         */
        int allocateChunk(size_t fn, vector<size_t> &chunks, vector<TC_HashMap::BlockData> &vtData);

        /**
         * @brief 获取数据长度
         *
         * @return size_t
         */
        size_t getDataLen();

    public:

        /**
         * Map
         */
        TC_HashMap         *_pMap;

        /**
         * block区块首地址, 相对地址
         */
        size_t              _iHead;

    };

    ////////////////////////////////////////////////////////////////////////
    /*
    * 内存数据块分配器
    *
    */
    class BlockAllocator
    {
    public:

        /**
         * @brief 构造函数
         */
        BlockAllocator(TC_HashMap *pMap)
        : _pMap(pMap)
        , _pChunkAllocator(new TC_MemMultiChunkAllocator())
        {
        }

        /**
         * @brief 析够函数
         */
        ~BlockAllocator()
        {
            if(_pChunkAllocator != NULL)
            {
                delete _pChunkAllocator;
            }
            _pChunkAllocator = NULL;
        }


        /**
         * @brief 初始化
         * @param pHeadAddr, 地址, 换到应用程序的绝对地址
         * @param iSize, 内存大小
         * @param iMinBlockSize, 最小数据块大小
         * @param iMaxBlockSize, 最大数据块大小
         * @param fFactor, 因子
         */
        void create(void *pHeadAddr, size_t iSize, size_t iMinBlockSize, size_t iMaxBlockSize, float fFactor)
        {
            _pChunkAllocator->create(pHeadAddr, iSize, iMinBlockSize, iMaxBlockSize, fFactor);
        }

        /**
         * @brief 连接上
         * @param pAddr, 地址, 换到应用程序的绝对地址
         */
        void connect(void *pHeadAddr)
        {
            _pChunkAllocator->connect(pHeadAddr);
        }

        /**
         * @brief 扩展空间
         * @param pAddr
         * @param iSize
         */
        void append(void *pAddr, size_t iSize)
        {
            _pChunkAllocator->append(pAddr, iSize);
        }

        /**
         * @brief 重建
         */
        void rebuild()
        {
            _pChunkAllocator->rebuild();
        }

        /**
         * @brief 获取每种数据块头部信息
         *
         * @return TC_MemChunk::tagChunkHead
         */
        vector<TC_MemChunk::tagChunkHead> getBlockDetail() const  { return _pChunkAllocator->getBlockDetail(); }

        /**
         * @brief 获取内存大小
         *
         * @return size_t
         */
        size_t getMemSize() const       { return _pChunkAllocator->getMemSize(); }

        /**
         * @brief 获取真正的数据容量
         *
         * @return size_t
         */
        size_t getCapacity() const      { return _pChunkAllocator->getCapacity(); }

        /**
         * @brief 每种block中的chunk个数(每种block中的chunk个数相同)
         *
         * @return vector<size_t>
         */
        vector<size_t> singleBlockChunkCount() const { return _pChunkAllocator->singleBlockChunkCount(); }

        /**
         * @brief 获取所有block的chunk个数
         *
         * @return size_t
         */
        size_t allBlockChunkCount() const    { return _pChunkAllocator->allBlockChunkCount(); }

        /**
         * @brief 在内存中分配一个新的Block
         *
         * @param index, block hash索引
         * @param iAllocSize: in/需要分配的大小, out/分配的块大小
         * @param vtData, 返回释放的内存块数据
         * @return size_t, 相对地址,0表示没有空间可以分配
         */
        size_t allocateMemBlock(size_t index, size_t &iAllocSize, vector<TC_HashMap::BlockData> &vtData);

        /**
         * @brief 为地址为iAddr的Block分配一个chunk
         *
         * @param iAddr,分配的Block的地址
         * @param iAllocSize, in/需要分配的大小, out/分配的块大小
         * @param vtData 返回释放的内存块数据
         * @return size_t, 相对地址,0表示没有空间可以分配
         */
        size_t allocateChunk(size_t iAddr, size_t &iAllocSize, vector<TC_HashMap::BlockData> &vtData);

        /**
         * @brief 释放Block
         * @param v
         */
        void deallocateMemBlock(const vector<size_t> &v);

        /**
         * @brief 释放Block
         * @param v
         */
        void deallocateMemBlock(size_t v);

    protected:
        //不允许copy构造
        BlockAllocator(const BlockAllocator &);
        //不允许赋值
        BlockAllocator& operator=(const BlockAllocator &);
        bool operator==(const BlockAllocator &mba) const;
        bool operator!=(const BlockAllocator &mba) const;
    public:
        /**
         * map
         */
        TC_HashMap                  *_pMap;

        /**
         * chunk分配器
         */
        TC_MemMultiChunkAllocator   *_pChunkAllocator;
    };

    ////////////////////////////////////////////////////////////////
    /** 
      * @brief map的数据项 
       */ 
    class HashMapLockItem
    {
    public:

        /**
         *
         * @param pMap
         * @param iAddr
         */
        HashMapLockItem(TC_HashMap *pMap, size_t iAddr);

        /**
         *
         * @param mcmdi
         */
        HashMapLockItem(const HashMapLockItem &mcmdi);

        /**
         *
         * @param mcmdi
         *
         * @return HashMapLockItem&
         */
        HashMapLockItem &operator=(const HashMapLockItem &mcmdi);

        /**
         *
         * @param mcmdi
         *
         * @return bool
         */
        bool operator==(const HashMapLockItem &mcmdi);

        /**
         *
         * @param mcmdi
         *
         * @return bool
         */
        bool operator!=(const HashMapLockItem &mcmdi);

        /**
         * @brief 是否是脏数据
         *
         * @return bool
         */
        bool isDirty();

        /**
         * @brief 是否只有Key
         *
         * @return bool
         */
        bool isOnlyKey();

        /**
         * @brief 获取最后Sync时间
         *
         * @return time_t
         */
        time_t getSyncTime();

        /**
         * 获取值, 如果只有Key(isOnlyKey)的情况下, v为空
         * @return int
         *          RT_OK:数据获取OK
         *          RT_ONLY_KEY: key有效, v无效为空
         *          其他值, 异常
         *
         */
        int get(string& k, string& v);

        /**
         * @brief 获取值
         * @return int
         *          RT_OK:数据获取OK
         *          其他值, 异常
         */
        int get(string& k);

        /**
         * @brief 获取数据块相对地址
         *
         * @return size_t
         */
        size_t getAddr() const { return _iAddr; }

    protected:

        /**
         * @brief 设置数据
         * @param k
         * @param v
         * @param vtData, 淘汰的数据
         * @return int
         */
        int set(const string& k, const string& v, vector<TC_HashMap::BlockData> &vtData);

        /**
         * @brief 设置Key, 无数据
         * @param k
         * @param vtData
         *
         * @return int
         */
        int set(const string& k, vector<TC_HashMap::BlockData> &vtData);

        /**
         *
         * @param pKey
         * @param iKeyLen
         *
         * @return bool
         */
        bool equal(const string &k, string &v, int &ret);

        /**
         *
         * @param pKey
         * @param iKeyLen
         *
         * @return bool
         */
        bool equal(const string& k, int &ret);

        /**
         * @brief 下一个item
         *
         * @return HashMapLockItem
         */
        void nextItem(int iType);

        /**
         * 上一个item
         * @param iType
         */
        void prevItem(int iType);

        friend class TC_HashMap;
        friend struct TC_HashMap::HashMapLockIterator;

    private:
        /**
         * map
         */
        TC_HashMap *_pMap;

        /**
         * block的地址
         */
        size_t      _iAddr;
    };

    /////////////////////////////////////////////////////////////////////////
    /** 
      * @brief 定义迭代器 
      */ 
    struct HashMapLockIterator
    {
    public:

        /**
         *@brief 定义遍历方式
         */
        enum
        {
            IT_BLOCK    = 0,        /**普通的顺序*/
            IT_SET      = 1,        /**Set时间顺序*/
            IT_GET      = 2,        /**Get时间顺序*/
        };

        /**
         * 迭代器的顺序
         */
        enum
        {
            IT_NEXT     = 0,        /**顺序*/
            IT_PREV     = 1,        /**逆序*/
        };

        /**
         *
         */
        HashMapLockIterator();

        /**
         * @brief 构造函数
         * @param iAddr, 地址
         * @param type
         */
        HashMapLockIterator(TC_HashMap *pMap, size_t iAddr, int iType, int iOrder);

        /**
         * @brief copy
         * @param it
         */
        HashMapLockIterator(const HashMapLockIterator &it);

        /**
         * @brief 复制
         * @param it
         *
         * @return HashMapLockIterator&
         */
        HashMapLockIterator& operator=(const HashMapLockIterator &it);

        /**
         *
         * @param mcmi
         *
         * @return bool
         */
        bool operator==(const HashMapLockIterator& mcmi);

        /**
         *
         * @param mv
         *
         * @return bool
         */
        bool operator!=(const HashMapLockIterator& mcmi);

        /**
         * @brief 前置++
         *
         * @return HashMapLockIterator&
         */
        HashMapLockIterator& operator++();

        /**
         * @brief 后置++
         *
         * @return HashMapLockIterator&
         */
        HashMapLockIterator operator++(int);

        /**
         *
         *
         * @return HashMapLockItem&i
         */
        HashMapLockItem& operator*() { return _iItem; }

        /**
         *
         *
         * @return HashMapLockItem*
         */
        HashMapLockItem* operator->() { return &_iItem; }

    public:
        /**
         *
         */
        TC_HashMap  *_pMap;

        /**
         *
         */
        HashMapLockItem _iItem;

        /**
         * 迭代器的方式
         */
        int        _iType;

        /**
         * 迭代器的顺序
         */
        int        _iOrder;

    };

    ////////////////////////////////////////////////////////////////
    /** 
     * @brief map的HashItem项, 一个HashItem对应多个数据项
     */
    class HashMapItem
    {
    public:

        /**
         *
         * @param pMap
         * @param iIndex
         */
        HashMapItem(TC_HashMap *pMap, size_t iIndex);

        /**
         *
         * @param mcmdi
         */
        HashMapItem(const HashMapItem &mcmdi);

        /**
         *
         * @param mcmdi
         *
         * @return HashMapItem&
         */
        HashMapItem &operator=(const HashMapItem &mcmdi);

        /**
         *
         * @param mcmdi
         *
         * @return bool
         */
        bool operator==(const HashMapItem &mcmdi);

        /**
         *
         * @param mcmdi
         *
         * @return bool
         */
        bool operator!=(const HashMapItem &mcmdi);

        /**
         * @brief 获取当前hash桶的所有数量, 注意只获取有key/value的数据
         * 对于只有key的数据, 不获取
         * 
         * @return
         */
        void get(vector<TC_HashMap::BlockData> &vtData);

        /**
         * 
         * 
         * @return int
         */
        int getIndex() const { return _iIndex; }

        /**
         * @brief 下一个item
         *
         */
        void nextItem();

        /**
         * 设置当前hash桶下所有数据为脏数据，注意只设置有key/value的数据
         * 对于只有key的数据, 不设置
         * @param
         * @return int
         */
        int setDirty();

        friend class TC_HashMap;
        friend struct TC_HashMap::HashMapIterator;

    private:
        /**
         * map
         */
        TC_HashMap *_pMap;

        /**
         * 数据块地址
         */
        size_t      _iIndex;
    };

    /////////////////////////////////////////////////////////////////////////
    /**
    * @brief 定义迭代器
    */
    struct HashMapIterator
    {
    public:

        /**
         * @brief 构造函数
         */
        HashMapIterator();

        /**
         * @brief 构造函数
         * @param iIndex, 地址
         * @param type
         */
        HashMapIterator(TC_HashMap *pMap, size_t iIndex);

        /**
         * @brief copy
         * @param it
         */
        HashMapIterator(const HashMapIterator &it);

        /**
         * @brief 复制
         * @param it
         *
         * @return HashMapLockIterator&
         */
        HashMapIterator& operator=(const HashMapIterator &it);

        /**
         *
         * @param mcmi
         *
         * @return bool
         */
        bool operator==(const HashMapIterator& mcmi);

        /**
         *
         * @param mv
         *
         * @return bool
         */
        bool operator!=(const HashMapIterator& mcmi);

        /**
         * @brief 前置++
         *
         * @return HashMapIterator&
         */
        HashMapIterator& operator++();

        /**
         * @brief 后置++
         *
         * @return HashMapIterator&
         */
        HashMapIterator operator++(int);

        /**
         *
         *
         * @return HashMapItem&i
         */
        HashMapItem& operator*() { return _iItem; }

        /**
         *
         *
         * @return HashMapItem*
         */
        HashMapItem* operator->() { return &_iItem; }

    public:
        /**
         *
         */
        TC_HashMap  *_pMap;

        /**
         *
         */
        HashMapItem _iItem;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
 
    /**
     * @brief map头
     */
    struct tagMapHead
    {
        char   _cMaxVersion;        /**大版本*/
        char   _cMinVersion;         /**小版本*/
        bool   _bReadOnly;           /**是否只读*/
        bool   _bAutoErase;          /**是否可以自动淘汰*/
        char   _cEraseMode;          /**淘汰方式:0x00:按照Get链淘汰, 0x01:按照Set链淘汰*/
        size_t _iMemSize;            /**内存大小*/
        size_t _iMinDataSize;        /**最小数据块大小*/
        size_t _iMaxDataSize;        /**最大数据块大小*/
        float  _fFactor;             /**因子*/
        float   _fRadio;              /**chunks个数/hash个数*/
        size_t _iElementCount;       /**总元素个数*/
        size_t _iEraseCount;         /**每次删除个数*/
        size_t _iDirtyCount;         /**脏数据个数*/
        size_t _iSetHead;            /**Set时间链表头部*/
        size_t _iSetTail;            /**Set时间链表尾部*/
        size_t _iGetHead;            /**Get时间链表头部*/
        size_t _iGetTail;            /**Get时间链表尾部*/
        size_t _iDirtyTail;          /**脏数据链尾部*/
        time_t _iSyncTime;           /**回写时间*/
        size_t _iUsedChunk;          /**已经使用的内存块*/
        size_t _iGetCount;           /**get次数*/
        size_t _iHitCount;           /**命中次数*/
        size_t _iBackupTail;         /**热备指针*/
        size_t _iSyncTail;           /**回写链表*/
        size_t _iOnlyKeyCount;         /** OnlyKey个数*/
        size_t _iReserve[4];        /**保留*/
    }__attribute__((packed));

    /**
     * @brief 需要修改的地址
     */
    struct tagModifyData
    {
        size_t  _iModifyAddr;       /**修改的地址*/
        char    _cBytes;           /**字节数*/
        size_t  _iModifyValue;      /**值*/
    }__attribute__((packed));

    /**
     * 修改数据块头部
     */
    struct tagModifyHead
    {
        char            _cModifyStatus;         /**修改状态: 0:目前没有人修改, 1: 开始准备修改, 2:修改完毕, 没有copy到内存中*/
        size_t          _iNowIndex;             /**更新到目前的索引, 不能操作10个*/
        tagModifyData   _stModifyData[20];      /**一次最多20次修改*/
    }__attribute__((packed));

    /**
     * HashItem
     */
    struct tagHashItem
    {
        size_t   _iBlockAddr;     /**指向数据项的偏移地址*/
        uint32_t _iListCount;     /**链表个数*/
    }__attribute__((packed));

    //64位操作系统用基数版本号, 32位操作系统用64位版本号
#if __WORDSIZE == 64

    //定义版本号
    enum
    {
        MAX_VERSION         = 0,    /**当前map的大版本号*/
        MIN_VERSION         = 3,    /**当前map的小版本号*/
    };

#else
    //定义版本号
    enum
    {
        MAX_VERSION         = 0,    /**当前map的大版本号*/
        MIN_VERSION         = 2,    /**当前map的小版本号*/
    };

#endif

    /**                            
      *@brief 定义淘汰方式
      */
    enum
    {
        ERASEBYGET          = 0x00, /**按照Get链表淘汰*/
        ERASEBYSET          = 0x01, /**按照Set链表淘汰*/
    };

    /**
     * @brief get, set等int返回值
     */
    enum
    {
        RT_OK                   = 0,    /**成功*/
        RT_DIRTY_DATA           = 1,    /**脏数据*/
        RT_NO_DATA              = 2,    /**没有数据*/
        RT_NEED_SYNC            = 3,    /**需要回写*/
        RT_NONEED_SYNC          = 4,    /**不需要回写*/
        RT_ERASE_OK             = 5,    /**淘汰数据成功*/
        RT_READONLY             = 6,    /**map只读*/
        RT_NO_MEMORY            = 7,    /**内存不够*/
        RT_ONLY_KEY             = 8,    /**只有Key, 没有Value*/
        RT_NEED_BACKUP          = 9,    /**需要备份*/
        RT_NO_GET               = 10,   /**没有GET过*/
        RT_DECODE_ERR           = -1,   /**解析错误*/
        RT_EXCEPTION_ERR        = -2,   /**异常*/
        RT_LOAD_DATA_ERR        = -3,   /**加载数据异常*/
        RT_VERSION_MISMATCH_ERR = -4,   /**版本不一致*/
        RT_DUMP_FILE_ERR        = -5,   /**dump到文件失败*/
        RT_LOAL_FILE_ERR        = -6,   /**load文件到内存失败*/
        RT_NOTALL_ERR           = -7,   /**没有复制完全*/
    };

    /**
     * @brief 定义迭代器
     */
    typedef HashMapIterator     hash_iterator;
    typedef HashMapLockIterator lock_iterator;


    /**
     * @brief 定义hash处理器
     */
    using hash_functor = std::function<size_t (const string& )>;

    //////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * @brief 构造函数
     */
    TC_HashMap()
    : _iMinDataSize(0)
    , _iMaxDataSize(0)
    , _fFactor(1.0)
    , _fRadio(2)
    , _pDataAllocator(new BlockAllocator(this))
    , _lock_end(this, 0, 0, 0)
    , _end(this, (size_t)(-1))
    , _hashf(hash<string>())
    {
    }

    /**
     *  @brief 定义hash处理器初始化数据块平均大小
     * 表示内存分配的时候，会分配n个最小块， n个（最小快*增长因子）, n个（最小快*增长因子*增长因子）..., 直到n个最大块
     * n是hashmap自己计算出来的
     * 这种分配策略通常是数据块记录变长比较多的使用， 便于节约内存，如果数据记录基本不是变长的， 那最小块=最大快，增长因子=1就可以了
     * @param iMinDataSize 最小数据块大小
     * @param iMaxDataSize 最大数据块大小
     * @param fFactor      增长因子
     */
    void initDataBlockSize(size_t iMinDataSize, size_t iMaxDataSize, float fFactor);

    /**
     *  @brief 始化chunk数据块/hash项比值,
     *         默认是2,有需要更改必须在create之前调用
     *
     * @param fRadio
     */
    void initHashRadio(float fRadio)                { _fRadio = fRadio;}

    /**
     * @brief 初始化, 之前需要调用:initDataAvgSize和initHashRadio
     * @param pAddr 绝对地址
     * @param iSize 大小
     * @return 失败则抛出异常
     */
    void create(void *pAddr, size_t iSize);

    /**
     * @brief  链接到内存块
     * @param pAddr, 地址
     * @param iSize, 内存大小
     * @return 失败则抛出异常
     */
    void connect(void *pAddr, size_t iSize);

    /**
     *  @brief 原来的数据块基础上扩展内存,注意通常只能对mmap文件生效
     * (如果iSize比本来的内存就小,则返回-1) 
     * @param pAddr, 扩展后的空间
     * @param iSize
     * @return 0:成功, -1:失败
     */
    int append(void *pAddr, size_t iSize);

    /**
     *  @brief 获取每种大小内存块的头部信息
     *
     * @return vector<TC_MemChunk::tagChunkHead>: 不同大小内存块头部信息
     */
    vector<TC_MemChunk::tagChunkHead> getBlockDetail() { return _pDataAllocator->getBlockDetail(); }

    /**
     * @brief 所有block中chunk的个数
     *
     * @return size_t
     */
    size_t allBlockChunkCount()                     { return _pDataAllocator->allBlockChunkCount(); }

    /**
     * @brief 每种block中chunk的个数(不同大小内存块的个数相同)
     *
     * @return vector<size_t>
     */
    vector<size_t> singleBlockChunkCount()          { return _pDataAllocator->singleBlockChunkCount(); }

    /**
     * @brief  获取hash桶的个数
     *
     * @return size_t
     */
    size_t getHashCount()                           { return _hash.size(); }

    /**
     * @brief  获取元素的个数
     *
     * @return size_t
     */
    size_t size()                                   { return _pHead->_iElementCount; }

    /**
     * @brief  脏数据元素个数
     *
     * @return size_t
     */
    size_t dirtyCount()                             { return _pHead->_iDirtyCount;}

    /**
     * @brief  OnlyKey数据元素个数
     *
     * @return size_t
     */
    size_t onlyKeyCount()                             { return _pHead->_iOnlyKeyCount;}

    /**
     * @brief 设置每次淘汰数量
     * @param n
     */
    void setEraseCount(size_t n)                    { _pHead->_iEraseCount = n; }

    /**
     * @brief  获取每次淘汰数量
     *
     * @return size_t
     */
    size_t getEraseCount()                          { return _pHead->_iEraseCount; }

    /**
     * @brief  设置只读
     * @param bReadOnly
     */
    void setReadOnly(bool bReadOnly)                { _pHead->_bReadOnly = bReadOnly; }

    /**
     * @brief 是否只读
     *
     * @return bool
     */
    bool isReadOnly()                               { return _pHead->_bReadOnly; }

    /**
     * @brief  设置是否可以自动淘汰
     * @param bAutoErase
     */
    void setAutoErase(bool bAutoErase)              { _pHead->_bAutoErase = bAutoErase; }

    /**
     * @brief  是否可以自动淘汰
     *
     * @return bool
     */
    bool isAutoErase()                              { return _pHead->_bAutoErase; }

    /**
     * @brief  设置淘汰方式
     * TC_HashMap::ERASEBYGET
     * TC_HashMap::ERASEBYSET
     * @param cEraseMode
     */
    void setEraseMode(char cEraseMode)              { _pHead->_cEraseMode = cEraseMode; }

    /**
     * @brief  获取淘汰方式
     *
     * @return bool
     */
    char getEraseMode()                             { return _pHead->_cEraseMode; }

    /**
     * @brief  设置回写时间(秒)
     * @param iSyncTime
     */
    void setSyncTime(time_t iSyncTime)              { _pHead->_iSyncTime = iSyncTime; }

    /**
     * @brief  获取回写时间
     *
     * @return time_t
     */
    time_t getSyncTime()                            { return _pHead->_iSyncTime; }

    /**
     * @brief  获取头部数据信息
     * 
     * @return tagMapHead&
     */
    tagMapHead& getMapHead()                        { return *_pHead; }

    /**
     * @brief  设置hash方式
     * @param hash_of
     */
    void setHashFunctor(hash_functor hashf)         { _hashf = hashf; }

    /**
     * @brief  返回hash处理器
     * 
     * @return hash_functor&
     */
    hash_functor &getHashFunctor()                  { return _hashf; }

    /**
     * @brief  hash item
     * @param index
     *
     * @return tagHashItem&
     */
    tagHashItem *item(size_t iIndex)                { return &_hash[iIndex]; }

    /**
     * @brief  dump到文件
     * @param sFile
     *
     * @return int
     *          RT_DUMP_FILE_ERR: dump到文件出错
     *          RT_OK: dump到文件成功
     */
    int dump2file(const string &sFile);

    /**
     * @brief  从文件load
     * @param sFile
     *
     * @return int
     *          RT_LOAL_FILE_ERR: load出错
     *          RT_VERSION_MISMATCH_ERR: 版本不一致
     *          RT_OK: load成功
     */
    int load5file(const string &sFile);

    /**
     *  @brief 修复hash索引为i的hash链(i不能操作hashmap的索引值)
     * @param i
     * @param bRepair
     * 
     * @return int
     */
    int recover(size_t i, bool bRepair);

    /**
     * @brief  清空hashmap
     * 所有map的数据恢复到初始状态
     */
    void clear();

    /**
     * @brief  检查数据干净状态
     * @param k
     *
     * @return int
     *          RT_NO_DATA: 没有当前数据
     *          RT_ONLY_KEY:只有Key
     *          RT_DIRTY_DATA: 是脏数据
     *          RT_OK: 是干净数据
     *          其他返回值: 错误
     */
    int checkDirty(const string &k);

    /**
     * @brief  设置为脏数据, 修改SET时间链, 会导致数据回写
     * @param k
     *
     * @return int
     *          RT_READONLY: 只读
     *          RT_NO_DATA: 没有当前数据
     *          RT_ONLY_KEY:只有Key
     *          RT_OK: 设置脏数据成功
     *          其他返回值: 错误
     */
    int setDirty(const string& k);

    /**
     * 数据回写失败后重新设置为脏数据
     * @param k
     *
     * @return int
     *          RT_READONLY: 只读
     *          RT_NO_DATA: 没有当前数据
     *          RT_ONLY_KEY:只有Key
     *          RT_OK: 设置脏数据成功
     *          其他返回值: 错误
     */
    int setDirtyAfterSync(const string& k);

    /**
     * @brief  设置为干净数据, 修改SET链, 导致数据不回写
     * @param k
     *
     * @return int
     *          RT_READONLY: 只读
     *          RT_NO_DATA: 没有当前数据
     *          RT_ONLY_KEY:只有Key
     *          RT_OK: 设置成功
     *          其他返回值: 错误
     */
    int setClean(const string& k);

    /**
     * @brief  获取数据, 修改GET时间链
     * @param k
     * @param v
     * @param iSyncTime:数据上次回写的时间
     *
     * @return int:
     *          RT_NO_DATA: 没有数据
     *          RT_ONLY_KEY:只有Key
     *          RT_OK:获取数据成功
     *          其他返回值: 错误
     */
    int get(const string& k, string &v, time_t &iSyncTime);

    /**
     * @brief  获取数据, 修改GET时间链
     * @param k
     * @param v
     *
     * @return int:
     *          RT_NO_DATA: 没有数据
     *          RT_ONLY_KEY:只有Key
     *          RT_OK:获取数据成功
     *          其他返回值: 错误
     */
    int get(const string& k, string &v);

    /**
     * @brief  设置数据, 修改时间链, 内存不够时会自动淘汰老的数据
     * @param k: 关键字
     * @param v: 值
     * @param bDirty: 是否是脏数据
     * @param vtData: 被淘汰的记录
     * @return int:
     *          RT_READONLY: map只读
     *          RT_NO_MEMORY: 没有空间(不淘汰数据情况下会出现)
     *          RT_OK: 设置成功
     *          其他返回值: 错误
     */
    int set(const string& k, const string& v, bool bDirty, vector<BlockData> &vtData);

    /**
     * @brief  设置key, 但无数据
     * @param k
     * @param vtData
     *
     * @return int
     *          RT_READONLY: map只读
     *          RT_NO_MEMORY: 没有空间(不淘汰数据情况下会出现)
     *          RT_OK: 设置成功
     *          其他返回值: 错误
     */
    int set(const string& k, vector<BlockData> &vtData);

    /**
     * @brief  删除数据
     * @param k, 关键字
     * @param data, 被删除的记录
     * @return int:
     *          RT_READONLY: map只读
     *          RT_NO_DATA: 没有当前数据
     *          RT_ONLY_KEY:只有Key, 删除成功
     *          RT_OK: 删除数据成功
     *         其他返回值: 错误
     */
    int del(const string& k, BlockData &data);

    /**
     * @brief  淘汰数据, 每次删除一条, 根据Get时间淘汰
     * 外部循环调用该接口淘汰数据
     * 直到: 元素个数/chunks * 100 < radio, bCheckDirty 为true时，遇到脏数据则淘汰结束
     * @param radio: 共享内存chunks使用比例 0< radio < 100
     * @param data: 当前被删除的一条记录
     * @return int:
     *          RT_READONLY: map只读
     *          RT_OK: 不用再继续淘汰了
     *          RT_ONLY_KEY:只有Key, 删除成功
     *          RT_DIRTY_DATA:数据是脏数据，当bCheckDirty=true时会有可能产生这种返回值
     *          RT_ERASE_OK:淘汰当前数据成功, 继续淘汰
     *          其他返回值: 错误, 通常忽略, 继续调用erase淘汰
     */
    int erase(int radio, BlockData &data, bool bCheckDirty = false);

    /**
     * @brief  回写, 每次返回需要回写的一条
     * 数据回写时间与当前时间超过_pHead->_iSyncTime则需要回写
     * _pHead->_iSyncTime由setSyncTime函数设定, 默认10分钟

     * 外部循环调用该函数进行回写
     * map只读时仍然可以回写
     * @param iNowTime: 当前时间
     *                  回写时间与当前时间相差_pHead->_iSyncTime都需要回写
     * @param data : 回写的数据
     * @return int:
     *          RT_OK: 到脏数据链表头部了, 可以sleep一下再尝试
     *          RT_ONLY_KEY:只有Key, 删除成功, 当前数据不要缓写,继续调用sync回写
     *          RT_NEED_SYNC:当前返回的data数据需要回写
     *          RT_NONEED_SYNC:当前返回的data数据不需要回写
     *          其他返回值: 错误, 通常忽略, 继续调用sync回写
     */
    int sync(time_t iNowTime, BlockData &data);

    /**
     * @brief  开始回写, 调整回写指针
     */
    void sync();

    /**
     * @brief  开始备份之前调用该函数
     *
     * @param bForceFromBegin: 是否强制重头开始备份
     * @return void
     */
    void backup(bool bForceFromBegin = false);

    /**
     * @brief  开始备份数据, 每次返回需要备份的一条数据
     * @param data
     *
     * @return int
     *          RT_OK: 备份完毕
     *          RT_NEED_BACKUP:当前返回的data数据需要备份
     *          RT_ONLY_KEY:只有Key, 当前数据不要备份
     *          其他返回值: 错误, 通常忽略, 继续调用backup
     */
    int backup(BlockData &data);

    /////////////////////////////////////////////////////////////////////////////////////////
    // 以下是遍历map函数, 需要对map加锁

    /**
     * @brief  结束
     *
     * @return
     */
    lock_iterator end() { return _lock_end; }


    /**
     * @brief  根据Key查找数据
     * @param string
     */
    lock_iterator find(const string& k);

    /**
     * @brief  block正序
     *
     * @return lock_iterator
     */
    lock_iterator begin();

    /**
     * @brief  block逆序
     *
     * @return lock_iterator
     */
    lock_iterator rbegin();

    /**
     * @brief  以Set时间排序的迭代器
     *
     * @return lock_iterator
     */
    lock_iterator beginSetTime();

    /**
     * @brief  Set链逆序的迭代器
     *
     * @return lock_iterator
     */
    lock_iterator rbeginSetTime();

    /**
     * @brief  以Get时间排序的迭代器
     *
     * @return lock_iterator
     */
    lock_iterator beginGetTime();

    /**
     * @brief  Get链逆序的迭代器
     *
     * @return lock_iterator
     */
    lock_iterator rbeginGetTime();

    /**
     * @brief  获取脏链表尾部迭代器(最长时间没有操作的脏数据)
     *
     * 返回的迭代器++表示按照时间顺序==>(最短时间没有操作的脏数据)
     *
     * @return lock_iterator
     */
    lock_iterator beginDirty();

    /////////////////////////////////////////////////////////////////////////////////////////
    // 以下是遍历map函数, 不需要对map加锁

    /**
     * @brief  根据hash桶遍历
     * 
     * @return hash_iterator
     */
    hash_iterator hashBegin();

    /**
     * @brief  结束
     *
     * @return
     */
    hash_iterator hashEnd() { return _end; }

    /**
     * 获取指定下标的hash_iterator
     * @param iIndex
     * 
     * @return hash_iterator
     */
    hash_iterator hashIndex(size_t iIndex);
    
    /**
     * @brief 描述
     *
     * @return string
     */
    string desc();

    /**
     * @brief  修改更新到内存中
     */
    void doUpdate(bool bUpdate = false);

protected:

    friend class Block;
    friend class BlockAllocator;
    friend class HashMapIterator;
    friend class HashMapItem;
    friend class HashMapLockIterator;
    friend class HashMapLockItem;

    //禁止copy构造
    TC_HashMap(const TC_HashMap &mcm);
    //禁止复制
    TC_HashMap &operator=(const TC_HashMap &mcm);

    /**
     * @brief  初始化
     * @param pAddr
     */
    void init(void *pAddr);


    /**
     * @brief  增加脏数据个数
     */
    void incDirtyCount()    { update(&_pHead->_iDirtyCount, _pHead->_iDirtyCount+1); }

    /**
     * @brief  减少脏数据个数
     */
    void delDirtyCount()    { update(&_pHead->_iDirtyCount, _pHead->_iDirtyCount-1); }

    /**
     * @brief  增加数据个数
     */
    void incElementCount()  { update(&_pHead->_iElementCount, _pHead->_iElementCount+1); }

    /**
     * @brief  减少数据个数
     */
    void delElementCount()  { update(&_pHead->_iElementCount, _pHead->_iElementCount-1); }

    /**
     * @brief  增加OnlyKey数据个数
     */
    void incOnlyKeyCount()    { update(&_pHead->_iOnlyKeyCount, _pHead->_iOnlyKeyCount+1); }

    /**
     * 减少OnlyKey数据个数
     */
    void delOnlyKeyCount()    { update(&_pHead->_iOnlyKeyCount, _pHead->_iOnlyKeyCount-1); }

    /**
     * @brief  增加Chunk数
     * 直接更新, 因为有可能一次分配的chunk个数
     * 多余更新区块的内存空间, 导致越界错误
     */
    void incChunkCount()    { _pHead->_iUsedChunk++; }

    /**
     * @brief  减少Chunk数
     * 直接更新, 因为有可能一次释放的chunk个数
     * 多余更新区块的内存空间, 导致越界错误
     */
    void delChunkCount()    { _pHead->_iUsedChunk--; }

    /**
     * @brief  增加hit次数
     */
    void incGetCount()      { update(&_pHead->_iGetCount, _pHead->_iGetCount+1); }

    /**
     * @brief  增加命中次数
     */
    void incHitCount()      { update(&_pHead->_iHitCount, _pHead->_iHitCount+1); }

    /**
     * @brief  某hash链表数据个数+1
     * @param index
     */
    void incListCount(uint32_t index) { update(&item(index)->_iListCount, item(index)->_iListCount+1); }

    /**
     * @brief  某hash链表数据个数+1
     * @param index
     */
    void delListCount(size_t index) { update(&item(index)->_iListCount, item(index)->_iListCount-1); }

    /**
     * @brief 相对地址换成绝对地址
     * @param iAddr
     *
     * @return void*
     */
    void *getAbsolute(size_t iAddr) { return (void *)((char*)_pHead + iAddr); }

    /**
     * @brief  绝对地址换成相对地址
     *
     * @return size_t
     */
    size_t getRelative(void *pAddr) { return (char*)pAddr - (char*)_pHead; }

    /**
     * @brief  淘汰iNowAddr之外的数据(根据淘汰策略淘汰)
     * @param iNowAddr, 当前Block不能正在分配空间, 不能被淘汰
     *                  0表示做直接根据淘汰策略淘汰
     * @param vector<BlockData>, 被淘汰的数据
     * @return size_t,淘汰的数据个数
     */
    size_t eraseExcept(size_t iNowAddr, vector<BlockData> &vtData);

    /**
     * @brief  根据Key计算hash值
     * @param pKey
     * @param iKeyLen
     *
     * @return size_t
     */
    size_t hashIndex(const string& k);

    /**
     * @brief  根据Key查找数据
     *
     */
    lock_iterator find(const string& k, size_t index, string &v, int &ret);

    /**
     * @brief  根据Key查找数据
     * @param mb
     */
    lock_iterator find(const string& k, size_t index, int &ret);

    /**
     * @brief  分析hash的数据
     * @param iMaxHash
     * @param iMinHash
     * @param fAvgHash
     */
    void analyseHash(uint32_t &iMaxHash, uint32_t &iMinHash, float &fAvgHash);

    /**
     * @brief  修改具体的值
     * @param iModifyAddr
     * @param iModifyValue
     */
    void update(void* iModifyAddr, size_t iModifyValue);

#if __WORDSIZE == 64
    void update(void* iModifyAddr, uint32_t iModifyValue);
#endif

    /**
     *
     * @param iModifyAddr
     * @param iModifyValue
     */
    void update(void* iModifyAddr, bool bModifyValue);

    /**
     * @brief  获取大于n且离n最近的素数
     * @param n
     *
     * @return size_t
     */
    size_t getMinPrimeNumber(size_t n);

protected:

    /**
     * 区块指针
     */
    tagMapHead                  *_pHead;

    /**
     * 最小的数据块大小
     */
    size_t                      _iMinDataSize;

    /**
     * 最大的数据块大小
     */
    size_t                      _iMaxDataSize;

    /**
     * 变化因子
     */
    float                       _fFactor;

    /**
     * 设置chunk数据块/hash项比值
     */
    float                       _fRadio;

    /**
     * hash对象
     */
    TC_MemVector<tagHashItem>   _hash;

    /**
     * 修改数据块
     */
    tagModifyHead               *_pstModifyHead;

    /**
     * block分配器对象
     */
    BlockAllocator              *_pDataAllocator;

    /**
     * 尾部
     */
    lock_iterator               _lock_end;

    /**
     * 尾部
     */
    hash_iterator               _end;

    /**
     * hash值计算公式
     */
    hash_functor                _hashf;
};

}

#endif

