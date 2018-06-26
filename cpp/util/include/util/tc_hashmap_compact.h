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

#ifndef __TC_HASHMAP_COMPACT_H__
#define __TC_HASHMAP_COMPACT_H__

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
#include "util/tc_thread.h"

namespace tars
{
/////////////////////////////////////////////////
/**
 * @file tc_hashmap_compact.h
 * @brief  hashmap类(紧凑型, 64位os下, 每个chunk的额外内存占用比tc_hashmap要小30个字节)
 *
 */
/////////////////////////////////////////////////
/**
* @brief Hash map异常类
*/
struct TC_HashMapCompact_Exception : public TC_Exception
{
    TC_HashMapCompact_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_HashMapCompact_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_HashMapCompact_Exception() throw(){};
};

////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief 紧凑性hashmap.
 * 基于内存的hashmap,所有操作需要自己加锁 , 使用方式同TC_Hashmap，但是64位系统下更节约额外的管理空间
 */
class TC_HashMapCompact
{
public:
    struct HashMapIterator;
    struct HashMapLockIterator;

    /**
     * @brief 操作数据
     * string  _key      数据Key
     * string  _value    数据value
     * bool    _dirty    是否是脏数据
     * uint32_t  _synct  sync time 不一定是真正的回写时间
     * uint32_t _expiret 数据过期的绝对时间
     *                   由设置或更新数据时提供，0表示不关心此时间 uint8_t
     * uint8_t _ver      数据版本，1为初始版本，0为保留
     */
    struct BlockData
    {
        string  _key;       /**数据Key*/
        string  _value;     /**数据value*/
        bool    _dirty;     /**是否是脏数据*/
        uint32_t  _synct;   /**sync time, 不一定是真正的回写时间*/
        uint32_t _expiret;    /**数据过期的绝对时间，由设置或更新数据时提供，0表示不关心此时间*/
        uint8_t    _ver;        /**数据版本，1为初始版本，0为保留*/
        BlockData()
        : _dirty(false)
        , _synct(0)
        , _expiret(0)
        , _ver(1)
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
            uint16_t    _iSize;         /**block的容量大小*/
            uint32_t    _iIndex;        /**hash的索引*/
            uint32_t    _iBlockNext;    /**下一个Block,tagBlockHead, 没有则为0*/
            uint32_t    _iBlockPrev;    /**上一个Block,tagBlockHead, 没有则为0*/
            uint32_t    _iSetNext;      /**Set链上的上一个Block*/
            uint32_t    _iSetPrev;      /**Set链上的上一个Block*/
            uint32_t    _iGetNext;      /**Get链上的上一个Block*/
            uint32_t    _iGetPrev;      /**Get链上的上一个Block*/
            uint32_t    _iSyncTime;     /**上次缓写时间*/
            uint32_t    _iExpireTime;    /** 数据过期的绝对时间，由设置或更新数据时提供，0表示不关心此时间*/
            uint8_t        _iVersion;        /** 数据版本，1为初始版本，0为保留*/
            bool        _bDirty;        /**是否是脏数据*/
            bool        _bOnlyKey;      /**是否只有key, 没有内容*/
            bool        _bNextChunk;    /**是否有下一个chunk*/
            union
            {
                uint32_t  _iNextChunk;    /**下一个Chunk块, _bNextChunk=true时有效, tagChunkHead*/
                uint32_t  _iDataLen;      /**当前数据块中使用了的长度, _bNextChunk=false时有效*/
            };
            char        _cData[0];      /**数据开始部分*/
        }__attribute__((packed));

        /**
         * @brief 非头部的block, 称为chunk
         */
        struct tagChunkHead
        {
            uint16_t    _iSize;         /**block的容量大小*/
            bool        _bNextChunk;    /**是否还有下一个chunk*/
            union
            {
                uint32_t  _iNextChunk;    /**下一个数据块, _bNextChunk=true时有效, tagChunkHead*/
                uint32_t  _iDataLen;      /**当前数据块中使用了的长度, _bNextChunk=false时有效*/
            };
            char        _cData[0];      /**数据开始部分*/
        }__attribute__((packed));

        /**
         * @brief 构造函数
         * @param Map
         * @param 当前MemBlock的地址
         * @param pAdd
         */
        Block(TC_HashMapCompact *pMap, uint32_t iAddr)
        : _pMap(pMap)
        , _iHead(iAddr)
        {
            _pHead = getBlockHead(_iHead);
        }

        /**
         * @brief 拷贝构造
         * @param mb
         */
        Block(const Block &mb)
        : _pMap(mb._pMap)
        , _iHead(mb._iHead)
        {
            _pHead = getBlockHead(_iHead);
        }

        /**
         * @brief 获取block头绝对地址
         * @param iAddr
         *
         * @return tagChunkHead*
         */
        tagBlockHead *getBlockHead(uint32_t iAddr) { return ((tagBlockHead*)_pMap->getAbsolute(iAddr)); }

        /**
         * @brief 获取MemBlock头地址
         *
         * @return void*
         */
        tagBlockHead *getBlockHead() {return _pHead;}

        /**
         * @brief 头部
         *
         * @return uint32_t
         */
        uint32_t getHead() { return _iHead;}

        /**
         * @brief 当前桶链表最后一个block的头部
         *
         * @return uint32_t
         */
        uint32_t getLastBlockHead();

        /**
        * @brief 获取数据的过期时间
        *
        * @return uint32_t，单位为秒，返回0表示无过期时间
        */
        uint32_t getExpireTime() { return getBlockHead()->_iExpireTime; }

        /**
        * @brief 设置数据的过期时间
        * @param iExpireTime，过期绝对时间，单位为秒
        */
        void setExpireTime(uint32_t iExpireTime)
        {
            if(iExpireTime != 0)
            {
                getBlockHead()->_iExpireTime = iExpireTime;
            }
        }

        /**
         * @brief 最新Get时间
         *
         * @return uint32_t
         */
        uint32_t getSyncTime() { return getBlockHead()->_iSyncTime; }

        /**
         * @brief 设置回写时间
         * @param iSyncTime
         */
        void setSyncTime(uint32_t iSyncTime) { getBlockHead()->_iSyncTime = iSyncTime; }

        /**
        * @brief 获取数据版本
        */
        uint8_t getVersion() { return getBlockHead()->_iVersion; }

        /**
        * @brief 设置数据版本
        */
        void setVersion(uint8_t iVersion) { getBlockHead()->_iVersion = iVersion; }

        /**
         * @brief 获取Block中的数据
         *
         * @return int
         *          TC_HashMapCompact::RT_OK, 正常, 其他异常
         *          TC_HashMapCompact::RT_ONLY_KEY, 只有Key
         *          其他异常
         */
        int getBlockData(TC_HashMapCompact::BlockData &data);

        /**
         * @brief 获取数据
         * @param pData
         * @param iDatalen
         * @return int,
         *          TC_HashMapCompact::RT_OK, 正常
         *          其他异常
         */
        int get(void *pData, uint32_t &iDataLen);

        /**
         * @brief 获取数据
         * @param s
         * @return int
         *          TC_HashMapCompact::RT_OK, 正常
         *          其他异常
         */
        int get(string &s);

        /**
         * @brief 设置数据
         * @param pData
         * @param iDatalen
         * @param vtData, 淘汰的数据
         */
        int set(const string& k, const string& v, uint32_t iExpireTime, uint8_t iVersion, bool bNewBlock, bool bOnlyKey, vector<TC_HashMapCompact::BlockData> &vtData);

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
         * @brief  是否只有key
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
         * @brief 新block时调用该函数，分配一个新的block
         * @param index, hash索引
         * @param iAllocSize, 内存大小
         */
        void makeNew(uint32_t index, uint32_t iAllocSize);

        /**
         * @brief 从Block链表中删除当前Block，只对Block有效,
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

        Block& operator=(const Block &mb);
        bool operator==(const Block &mb) const;
        bool operator!=(const Block &mb) const;

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
        tagChunkHead *getChunkHead(uint32_t iAddr) { return ((tagChunkHead*)_pMap->getAbsolute(iAddr)); }

        /**
         * @brief 从当前的chunk开始释放
         * @param iChunk 释放地址
         */
        void deallocate(uint32_t iChunk);

        /**
         * @brief 插入到hashmap的链中
         */
        void insertHashMap();

        /**
         * @brief 如果数据容量不够, 则新增加chunk, 不影响原有数据
         *        使新增加的总容量大于iDataLen，释放多余的chunk
         * @param iDataLen
         *
         * @return int,
         */
        int allocate(uint32_t iDataLen, vector<TC_HashMapCompact::BlockData> &vtData);

        /**
         * @brief 挂接chunk, 如果core则挂接失败, 保证内存块还可以用
         * @param pChunk
         * @param chunks
         *
         * @return int
         */
        int joinChunk(tagChunkHead *pChunk, const vector<uint32_t> chunks);

        /**
         * @brief 分配n个chunk地址，
         *        注意释放内存的时候不能释放正在分配的对象
         * @param fn      分配空间大小
         * @param chunks  分配成功返回的chunks地址列表
         * @param vtData  淘汰的数据
         * @return        int
         */
        int allocateChunk(uint32_t fn, vector<uint32_t> &chunks, vector<TC_HashMapCompact::BlockData> &vtData);

        /**
         * @brief 获取数据长度
         *
         * @return uint32_t
         */
        uint32_t getDataLen();

    private:

        /**
         * Map
         */
        TC_HashMapCompact   *_pMap;

        /**
         * block区块首地址, 相对地址
         */
        uint32_t            _iHead;

        /**
         * 地址
         */
        tagBlockHead *      _pHead;
    };

    ////////////////////////////////////////////////////////////////////////
    /**
    * @brief 内存数据块分配器
    */
    class BlockAllocator
    {
    public:

        /**
         * @brief 构造函数
         */
        BlockAllocator(TC_HashMapCompact *pMap)
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
        void create(void *pHeadAddr, size_t iSize, uint32_t iMinBlockSize, uint32_t iMaxBlockSize, float fFactor)
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
         * @brief 内存大小
         *
         * @return size_t
         */
        size_t getMemSize() const       { return _pChunkAllocator->getMemSize(); }

        /**
         * @brief 真正的数据容量
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
         * @brief 所有block的chunk个数
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
        uint32_t allocateMemBlock(uint32_t index, uint32_t &iAllocSize, vector<TC_HashMapCompact::BlockData> &vtData);

        /**
         * @brief 为地址为iAddr的Block分配一个chunk
         *
         * @param iAddr,分配的Block的地址
         * @param iAllocSize, in/需要分配的大小, out/分配的块大小
         * @param vtData 返回释放的内存块数据
         * @return size_t, 相对地址,0表示没有空间可以分配
         */
        uint32_t allocateChunk(uint32_t iAddr, uint32_t &iAllocSize, vector<TC_HashMapCompact::BlockData> &vtData);

        /**
         * @brief 释放Block
         * @param v
         */
        void deallocateMemBlock(const vector<uint32_t> &v);

        /**
         * @brief 释放Block
         * @param v
         */
        void deallocateMemBlock(uint32_t v);

    protected:
        /**
         *@brief 不允许copy构造
         * */
        BlockAllocator(const BlockAllocator &);
        //不允许赋值
        BlockAllocator& operator=(const BlockAllocator &);
        bool operator==(const BlockAllocator &mba) const;
        bool operator!=(const BlockAllocator &mba) const;

    public:
        /**
         * map
         */
        TC_HashMapCompact           *_pMap;

        /**
         * chunk分配器
         */
        TC_MemMultiChunkAllocator   *_pChunkAllocator;
    };

    ////////////////////////////////////////////////////////////////
    /**
     *@brief  map的数据项
     * */
    class HashMapLockItem
    {
    public:

        /**
         *
         * @param pMap
         * @param iAddr
         */
        HashMapLockItem(TC_HashMapCompact *pMap, uint32_t iAddr);

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
         * @brief 最后Sync时间
         *
         * @return uint32_t
         */
        uint32_t getSyncTime();

        /**
         * @brief 获取值, 如果只有Key(isOnlyKey)的情况下, v为空
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
         * @brief 数据块相对地址
         *
         * @return uint32_t
         */
        uint32_t getAddr() const { return _iAddr; }

    protected:

        /**
         * @brief 设置数据
         * @param k
         * @param v
         * @param iExpiretime, 数据过期时间，单位为秒，0表示不设置过期时间
         * @param iVersion, 数据版本, 应该根据get出的数据版本写回，为0表示不关心数据版本
         * @param vtData, 淘汰的数据
         * @return int
         */
        int set(const string& k, const string& v, uint32_t iExpireTime, uint8_t iVersion, bool bNewBlock, vector<TC_HashMapCompact::BlockData> &vtData);

        /**
         * @brief 设置Key, 无数据
         * @param k
         * @param vtData
         *
         * @return int
         */
        int set(const string& k, uint8_t iVersion, bool bNewBlock, vector<TC_HashMapCompact::BlockData> &vtData);

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
         * @brief 上一个item
         * @param iType
         */
        void prevItem(int iType);

        friend class TC_HashMapCompact;
        friend struct TC_HashMapCompact::HashMapLockIterator;

    private:
        /**
         * map
         */
        TC_HashMapCompact *_pMap;

        /**
         * block的地址
         */
        uint32_t          _iAddr;
    };

    /////////////////////////////////////////////////////////////////////////
    /**
     * @brief 定义迭代器
     * */
    struct HashMapLockIterator
    {
    public:

        /**
         *@brief 定义遍历方式
         * */
        enum
        {
            IT_BLOCK    = 0,        /**普通的顺序*/
            IT_SET      = 1,        /**Set时间顺序*/
            IT_GET      = 2,        /**Get时间顺序*/
        };

        /**
         * @brief 迭代器的顺序
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
        HashMapLockIterator(TC_HashMapCompact *pMap, uint32_t iAddr, int iType, int iOrder);

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
        TC_HashMapCompact  *_pMap;

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
    // map的HashItem项, 一个HashItem对应多个数据项
    class HashMapItem
    {
    public:

        /**
         *
         * @param pMap
         * @param iIndex
         */
        HashMapItem(TC_HashMapCompact *pMap, uint32_t iIndex);

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
         * @brief 获取当前hash桶的所有数量, 注意只获取有key/value的数据,
         *        对于只有key的数据, 不获取
         *
         * @return
         */
        void get(vector<TC_HashMapCompact::BlockData> &vtData);

        /**
         * @brief 获取当前hash桶的过期数据, 注意只获取有key/value的数据,
         *        对于只有key的数据, 不获取
         *
         * @return
         */
        void getExpire(uint32_t t, vector<TC_HashMapCompact::BlockData> &vtData);

        /**
         *
         *
         * @return uint32_t
         */
        uint32_t getIndex() const { return _iIndex; }

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

        friend class TC_HashMapCompact;
        friend struct TC_HashMapCompact::HashMapIterator;

    private:
        /**
         * map
         */
        TC_HashMapCompact *_pMap;

        /**
         * 数据块地址
         */
        uint32_t     _iIndex;
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
        HashMapIterator(TC_HashMapCompact *pMap, uint32_t iIndex);

        /**
         * @brief 拷贝构造
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
        TC_HashMapCompact  *_pMap;

        /**
         *
         */
        HashMapItem _iItem;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    /**
     * map头
     */
    struct tagMapHead
    {
        char        _cMaxVersion;        //大版本
        char        _cMinVersion;        //小版本
        bool        _bReadOnly;          //是否只读
        bool        _bAutoErase;         //是否可以自动淘汰
        char        _cEraseMode;         //淘汰方式:0x00:按照Get链淘汰, 0x01:按照Set链淘汰
        size_t      _iMemSize;           //内存大小
        uint32_t    _iMinDataSize;       //最小数据块大小
        uint32_t    _iMaxDataSize;       //最大数据块大小
        float       _fFactor;            //因子
        float       _fRadio;             //chunks个数/hash个数
        uint32_t    _iElementCount;      //总元素个数
        uint32_t    _iEraseCount;        //每次删除个数
        uint32_t    _iDirtyCount;        //脏数据个数
        uint32_t    _iSetHead;           //Set时间链表头部
        uint32_t    _iSetTail;           //Set时间链表尾部
        uint32_t    _iGetHead;           //Get时间链表头部
        uint32_t    _iGetTail;           //Get时间链表尾部
        uint32_t    _iDirtyTail;         //脏数据链尾部
        uint32_t    _iSyncTime;          //回写时间
        uint32_t    _iUsedChunk;         //已经使用的内存块
        uint32_t    _iGetCount;          //get次数
        uint32_t    _iHitCount;          //命中次数
        uint32_t    _iBackupTail;        //热备指针
        uint32_t    _iSyncTail;          //回写链表
        uint32_t    _iOnlyKeyCount;        // OnlyKey个数
        bool         _bInit;                 //是否已经完成初始化
        char        _cReserve[15];          //保留
        //uint32_t    _iReserve[4];       //保留
    }__attribute__((packed));

    /**
     * 需要修改的地址
     */
    struct tagModifyData
    {
        size_t  _iModifyAddr;       /**修改的地址*/
        char    _cBytes;            /**字节数*/
        size_t  _iModifyValue;      /**值*/
    }__attribute__((packed));

    /**
     * 修改数据块头部
     */
    struct tagModifyHead
    {
        char            _cModifyStatus;         /**修改状态: 0:目前没有人修改, 1: 开始准备修改, 2:修改完毕, 没有copy到内存中*/
        uint32_t        _iNowIndex;             /**更新到目前的索引, 不能操作10个*/
        tagModifyData   _stModifyData[50000];     /**一次最多50000次修改*/
    }__attribute__((packed));

    /**
     * HashItem
     */
    struct tagHashItem
    {
        uint32_t _iBlockAddr;     /**指向数据项的偏移地址*/
        uint32_t _iListCount;     /**链表个数*/
    }__attribute__((packed));

    /**
     * @brief 64位操作系统用基数版本号, 32位操作系统用64位版本号
     *  */
    //64位操作系统用基数版本号, 32位操作系统用偶数版本号
#if __WORDSIZE == 64

    /**
     * @brief 定义版本号
      */
    enum
    {
        MAX_VERSION         = 4,    //当前map的大版本号
        MIN_VERSION         = 1,    //当前map的小版本号
    };

#else
    /**
     * @brief 定义版本号
      */
    enum
    {
        MAX_VERSION         = 4,    /**当前map的大版本号*/
        MIN_VERSION         = 0,    /**当前map的小版本号*/
    };

#endif

    /**
     * @brief 定义淘汰方式
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
        RT_DATA_VER_MISMATCH    = 11,   /**写入数据版本不匹配*/
        RT_DECODE_ERR           = -1,   /**解析错误*/
        RT_EXCEPTION_ERR        = -2,   /**异常*/
        RT_LOAD_DATA_ERR        = -3,   /**加载数据异常*/
        RT_VERSION_MISMATCH_ERR = -4,   /**版本不一致*/
        RT_DUMP_FILE_ERR        = -5,   /**dump到文件失败*/
        RT_LOAL_FILE_ERR        = -6,   /**load文件到内存失败*/
        RT_NOTALL_ERR           = -7,   /**没有复制完全*/
    };

    /**
     * @brief  定义迭代器
      */
    typedef HashMapIterator     hash_iterator;
    typedef HashMapLockIterator lock_iterator;

    /**
     * @brief 定义hash处理器
      */
    using hash_functor = std::function<size_t (const string& )>;

    //////////////////////////////////////////////////////////////////////////////////////////////
    //map的接口定义

    /**
     * @brief 构造函数
     */
    TC_HashMapCompact()
    : _iMinDataSize(0)
    , _iMaxDataSize(0)
    , _fFactor(1.0)
    , _fRadio(2)
    , _pDataAllocator(new BlockAllocator(this))
    , _lock_end(this, 0, 0, 0)
    , _end(this, (uint32_t)(-1))
    , _hashf(hash_new<string>())
    {
    }

    /**
     * @brief 初始化数据块平均大小
     * 表示内存分配的时候，会分配n个最小块， n个（最小快*增长因子）, n个（最小快*增长因子*增长因子）..., 直到n个最大块
     * n是hashmap自己计算出来的
     * 这种分配策略通常是数据块记录变长比较多的使用， 便于节约内存，如果数据记录基本不是变长的， 那最小块=最大快，增长因子=1就可以了
     * @param iMinDataSize: 最小数据块大小
     * @param iMaxDataSize: 最大数据块大小
     * @param fFactor: 增长因子
     */
    void initDataBlockSize(uint32_t iMinDataSize, uint32_t iMaxDataSize, float fFactor);

    /**
     * @brief 始化chunk数据块/hash项比值, 默认是2,
     *        有需要更改必须在create之前调用
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
     * 链接到内存块
     * @param pAddr, 地址
     * @param iSize, 内存大小
     * @return 失败则抛出异常
     */
    void connect(void *pAddr, size_t iSize);

    /**
     * @brief 原来的数据块基础上扩展内存, 注意通常只能对mmap文件生效
     * (如果iSize比本来的内存就小,则返回-1)
     * @param pAddr, 扩展后的空间
     * @param iSize
     * @return 0:成功, -1:失败
     */
    int append(void *pAddr, size_t iSize);

    /**
     * @brief 获取每种大小内存块的头部信息
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
     * @brief 获取hash桶的个数
     *
     * @return size_t
     */
    uint32_t getHashCount()                           { return _hash.size(); }

    /**
     * @brief 元素的个数
     *
     * @return size_t
     */
    uint32_t size()                                   { return _pHead->_iElementCount; }

    /**
     * @brief 脏数据元素个数
     *
     * @return uint32_t
     */
    uint32_t dirtyCount()                             { return _pHead->_iDirtyCount;}

    /**
     * @brief OnlyKey数据元素个数
     *
     * @return uint32_t
     */
    uint32_t onlyKeyCount()                             { return _pHead->_iOnlyKeyCount;}

    /**
     * @brief 设置每次淘汰数量
     * @param n
     */
    void setEraseCount(uint32_t n)                    { _pHead->_iEraseCount = n; }

    /**
     * @brief 获取每次淘汰数量
     *
     * @return uint32_t
     */
    uint32_t getEraseCount()                          { return _pHead->_iEraseCount; }

    /**
     * @brief 设置只读
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
     * @brief 设置是否可以自动淘汰
     * @param bAutoErase
     */
    void setAutoErase(bool bAutoErase)              { _pHead->_bAutoErase = bAutoErase; }

    /**
     * @brief 是否可以自动淘汰
     *
     * @return bool
     */
    bool isAutoErase()                              { return _pHead->_bAutoErase; }

    /**
     * @brief 设置淘汰方式
     * TC_HashMapCompact::ERASEBYGET
     * TC_HashMapCompact::ERASEBYSET
     * @param cEraseMode
     */
    void setEraseMode(char cEraseMode)              { _pHead->_cEraseMode = cEraseMode; }

    /**
     * @brief 获取淘汰方式
     *
     * @return bool
     */
    char getEraseMode()                             { return _pHead->_cEraseMode; }

    /**
     * @brief 设置回写时间(秒)
     * @param iSyncTime
     */
    void setSyncTime(uint32_t iSyncTime)              { _pHead->_iSyncTime = iSyncTime; }

    /**
     * @brief 获取回写时间
     *
     * @return uint32_t
     */
    uint32_t getSyncTime()                            { return _pHead->_iSyncTime; }

    /**
     * @brief 获取头部数据信息
     *
     * @return tagMapHead&
     */
    tagMapHead& getMapHead()                        { return *_pHead; }

    /**
     * @brief 设置hash方式
     * @param hash_of
     */
    void setHashFunctor(hash_functor hashf)         { _hashf = hashf; }

    /**
     * @brief 返回hash处理器
     *
     * @return hash_functor&
     */
    hash_functor &getHashFunctor()                  { return _hashf; }

    /**
     * @brief hash item
     * @param index
     *
     * @return tagHashItem&
     */
    tagHashItem *item(uint32_t iIndex)                { return &_hash[iIndex]; }

    /**
     * dump到文件
     * @param sFile
     *
     * @return int
     *          RT_DUMP_FILE_ERR: dump到文件出错
     *          RT_OK: dump到文件成功
     */
    int dump2file(const string &sFile);

    /**
     * @brief 从文件load
     * @param sFile
     *
     * @return int
     *          RT_LOAL_FILE_ERR: load出错
     *          RT_VERSION_MISMATCH_ERR: 版本不一致
     *          RT_OK: load成功
     */
    int load5file(const string &sFile);

    /**
     * @brief 修复hash索引为i的hash链(i不能操作hashmap的索引值)
     * @param i
     * @param bRepair
     *
     * @return int
     */
    int recover(size_t i, bool bRepair);

    /**
     * @brief 清空hashmap，所有map的数据恢复到初始状态
     */
    void clear();

    /**
     * @brief 检查数据干净状态
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
     * @brief 设置为脏数据, 修改SET时间链, 会导致数据回写
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
     * @brief 设置为干净数据, 修改SET链, 导致数据不回写
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
     * @brief 获取数据, 修改GET时间链
     * @param k
     * @param v
     * @param iSyncTime:数据上次回写的时间
     * @param iExpiretime: 数据过期时间，单位为秒，0表示不设置过期时间
     * @param iVersion: 数据版本, 应该根据get出的数据版本写回，为0表示不关心数据版本
     *
     * @return int:
     *          RT_NO_DATA: 没有数据
     *          RT_ONLY_KEY:只有Key
     *          RT_OK:获取数据成功
     *          其他返回值: 错误
     */
    int get(const string& k, string &v, uint32_t &iSyncTime, uint32_t& iExpireTime, uint8_t& iVersion);

    /**
     * @brief 获取数据, 修改GET时间链
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
    int get(const string& k, string &v, uint32_t &iSyncTime);

    /**
     * @brief 获取数据, 修改GET时间链
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
     * @brief 设置数据, 修改时间链, 内存不够时会自动淘汰老的数据
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
     * @brief 设置数据, 修改时间链, 内存不够时会自动淘汰老的数据
     * @param k            关键字
     * @param v            值
     * @param iExpiretime 数据过期时间，单位为秒，
     *                    0表示不设置过期时间
     * @param iVersion    数据版本,
     *                    应该根据get出的数据版本写回，为0表示不关心数据版 本
     * @param bDirty     是否是脏数据
     * @param vtData     被淘汰的记录
     * @return           RT_READONLY: map只读
     *                   RT_NO_MEMORY:没有空间(不淘汰数据情况下会出现）
     *                   RT_OK: 设置成功 其他返回值: 错误
     */
    int set(const string& k, const string& v, uint32_t iExpireTime, uint8_t iVersion, bool bDirty, vector<BlockData> &vtData);

    /**
     * @brief 设置key, 但无数据
     * @param k
     * @param vtData
     *
     * @return  RT_READONLY: map只读
     *          RT_NO_MEMORY: 没有空间(不淘汰数据情况下会出现)
     *          RT_OK: 设置成功
     *          其他返回值: 错误
     */
    int set(const string& k, vector<BlockData> &vtData);

    /**
     * @brief 设置key, 但无数据
     * @param k
     * @param iVersion: 数据版本, 应该根据get出的数据版本写回，为0表示不关心数据版本
     * @param vtData
     *
     * @return int
     *          RT_READONLY: map只读
     *          RT_NO_MEMORY: 没有空间(不淘汰数据情况下会出现)
     *          RT_OK: 设置成功
     *          其他返回值: 错误
     */
    int set(const string& k, uint8_t iVersion, vector<BlockData>& vtData);

    /**
     * @brief 删除数据
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
     * @brief 淘汰数据, 每次删除一条,
     * 根据Get时间淘汰，外部循环调用该接口淘汰数据 直到:
     * 元素个数/chunks * 100 < radio, bCheckDirty
     * 为true时，遇到脏数据则淘汰结束
     * @param radio 共享内存chunks使用比例 0< radio < 100
     * @param data  当前被删除的一条记录
     * @return  RT_READONLY: map只读
     *          RT_OK: 不用再继续淘汰了
     *          RT_ONLY_KEY:只有Key, 删除成功
     *          RT_DIRTY_DATA:数据是脏数据，当bCheckDirty=true时会有可能产生这种返回值
     *          RT_ERASE_OK:淘汰当前数据成功, 继续淘汰
     *          其他返回值: 错误, 通常忽略, 继续调用erase淘汰
     */
    int erase(int radio, BlockData &data, bool bCheckDirty = false);

    /**
     * @brief 回写, 每次返回需要回写的一条
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
    int sync(uint32_t iNowTime, BlockData &data);

    /**
     * @brief 开始回写, 调整回写指针
     */
    void sync();

    /**
     * @brief 开始备份之前调用该函数
     *
     * @param bForceFromBegin: 是否强制重头开始备份
     * @return void
     */
    void backup(bool bForceFromBegin = false);

    /**
     * @brief 开始备份数据, 每次返回需要备份的一条数据
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
     *@brief  结束
     *
     * @return
     */
    lock_iterator end() { return _lock_end; }


    /**
     * @brief 根据Key查找数据
     * @param string
     */
    lock_iterator find(const string& k);

    /**
     * @brief block正序
     *
     * @return lock_iterator
     */
    lock_iterator begin();

    /**
     *@brief  block逆序
     *
     * @return lock_iterator
     */
    lock_iterator rbegin();

    /**
     * @brief 以Set时间排序的迭代器
     *
     * @return lock_iterator
     */
    lock_iterator beginSetTime();

    /**
     * @brief Set链逆序的迭代器
     *
     * @return lock_iterator
     */
    lock_iterator rbeginSetTime();

    /**
     * @brief 以Get时间排序的迭代器
     *
     * @return lock_iterator
     */
    lock_iterator beginGetTime();

    /**
     * @brief Get链逆序的迭代器
     *
     * @return lock_iterator
     */
    lock_iterator rbeginGetTime();

    /**
     * @brief 获取脏链表尾部迭代器(最长时间没有操作的脏数据)，
     * 返回的迭代器++表示按照时间顺序==>(最短时间没有操作的脏数据)
     *
     * @return lock_iterator
     */
    lock_iterator beginDirty();

    /////////////////////////////////////////////////////////////////////////////////////////
    // 以下是遍历map函数, 不需要对map加锁

    /**
     * @brief 根据hash桶遍历
     *
     * @return hash_iterator
     */
    hash_iterator hashBegin();

    /**
     * @brief 结束
     *
     * @return
     */
    hash_iterator hashEnd() { return _end; }

    /**
     * @brief 描述
     *
     * @return string
     */
    string desc();

public:

    /**
     * @brief 错误保护
     */
    struct FailureRecover
    {
        FailureRecover(TC_HashMapCompact *pMap) : _pMap(pMap)
        {
            _pMap->doRecover();
        }

        ~FailureRecover()
        {
            _pMap->doUpdate();
        }

    protected:
        TC_HashMapCompact   *_pMap;
    };

protected:

    friend class Block;
    friend class BlockAllocator;
    friend class HashMapIterator;
    friend class HashMapItem;
    friend class HashMapLockIterator;
    friend class HashMapLockItem;

    /**
     * @brief 禁止copy构造
     *  */
    TC_HashMapCompact(const TC_HashMapCompact &mcm);
    /**
     * @brief 禁止复制
     *  */
    TC_HashMapCompact &operator=(const TC_HashMapCompact &mcm);

    /**
     * @brief 初始化
     * @param pAddr
     */
    void init(void *pAddr);


    /**
     * @brief 增加脏数据个数
     */
    void incDirtyCount()    { saveValue(&_pHead->_iDirtyCount, _pHead->_iDirtyCount+1); }

    /**
     * @brief 减少脏数据个数
     */
    void delDirtyCount()    { saveValue(&_pHead->_iDirtyCount, _pHead->_iDirtyCount-1); }

    /**
     * @brief 增加数据个数
     */
    void incElementCount()  { saveValue(&_pHead->_iElementCount, _pHead->_iElementCount+1); }

    /**
     * @brief 减少数据个数
     */
    void delElementCount()  { saveValue(&_pHead->_iElementCount, _pHead->_iElementCount-1); }

    /**
     * @brief 增加OnlyKey数据个数
     */
    void incOnlyKeyCount()    { saveValue(&_pHead->_iOnlyKeyCount, _pHead->_iOnlyKeyCount+1); }

    /**
     * @brief 减少OnlyKey数据个数
     */
    void delOnlyKeyCount()    { saveValue(&_pHead->_iOnlyKeyCount, _pHead->_iOnlyKeyCount-1); }

    /*
     * @brief 增加Chunk数
     * 以选择是否可直接更新, 因为有可能一次分配的chunk个数
     * 多余更新区块的内存空间, 导致越界错误
     */
    void incChunkCount(bool bUpdateDirect = true)
    {
        if(bUpdateDirect)
        {
            ++_pHead->_iUsedChunk;
        }
        else
            saveValue(&_pHead->_iUsedChunk, _pHead->_iUsedChunk+1);
    }

    /**
     * @brief 减少Chunk数
     * 可以选择是否直接更新, 因为有可能一次释放的chunk个数
     * 多余更新区块的内存空间, 导致越界错误
     */
    void delChunkCount(bool bUpdateDirect = true)
    {
        if(bUpdateDirect)
        {
            --_pHead->_iUsedChunk;
        }
        else
            saveValue(&_pHead->_iUsedChunk, _pHead->_iUsedChunk-1);
    }

    /**
     * @brief 增加hit次数
     */
    void incGetCount()      { saveValue(&_pHead->_iGetCount, _pHead->_iGetCount+1); }

    /**
     * @brief 增加命中次数
     */
    void incHitCount()      { saveValue(&_pHead->_iHitCount, _pHead->_iHitCount+1); }

    /**
     * @brief 某hash链表数据个数+1
     * @param index
     */
    void incListCount(uint32_t index) { saveValue(&item(index)->_iListCount, item(index)->_iListCount+1); }

    /**
     * @brief 某hash链表数据个数+1
     * @param index
     */
    void delListCount(uint32_t index) { saveValue(&item(index)->_iListCount, item(index)->_iListCount-1); }

    /**
     * @brief 相对地址换成绝对地址
     * @param iAddr
     *
     * @return void*
     */
    void *getAbsolute(uint32_t iAddr) { if(iAddr == 0) return NULL; return _pDataAllocator->_pChunkAllocator->getAbsolute(iAddr); }

    /**
     * @brief 淘汰iNowAddr之外的数据(根据淘汰策略淘汰)
     * @param iNowAddr, 当前Block不能正在分配空间, 不能被淘汰
     *                  0表示做直接根据淘汰策略淘汰
     * @param vector<BlockData>, 被淘汰的数据
     * @return size_t,淘汰的数据个数
     */
    uint32_t eraseExcept(uint32_t iNowAddr, vector<BlockData> &vtData);

    /**
     * @brief 根据Key计算hash值
     * @param pKey
     * @param iKeyLen
     *
     * @return size_t
     */
    uint32_t hashIndex(const string& k);

    /**
     * @brief 根据Key查找数据
     *
     */
    lock_iterator find(const string& k, uint32_t index, string &v, int &ret);

    /**
     * @brief 根据Key查找数据
     * @param mb
     */
    lock_iterator find(const string& k, uint32_t index, int &ret);

    /**
     * @brief 分析hash的数据
     * @param iMaxHash
     * @param iMinHash
     * @param fAvgHash
     */
    void analyseHash(uint32_t &iMaxHash, uint32_t &iMinHash, float &fAvgHash);

    /**
     * @brief 修改具体的值
     * @param iModifyAddr
     * @param iModifyValue
     */
    template<typename T>
    void saveValue(void* iModifyAddr, T iModifyValue, bool bModify = true)
    {
        //获取原始值
        T tmp = *(T*)iModifyAddr;

        //保存原始值
        _pstModifyHead->_stModifyData[_pstModifyHead->_iNowIndex]._iModifyAddr  = (char*)iModifyAddr - (char*)_pHead;
        _pstModifyHead->_stModifyData[_pstModifyHead->_iNowIndex]._iModifyValue = tmp;
        _pstModifyHead->_stModifyData[_pstModifyHead->_iNowIndex]._cBytes       = sizeof(iModifyValue);
        _pstModifyHead->_cModifyStatus = 1;

        _pstModifyHead->_iNowIndex++;

        if(bModify)
        {
            //修改具体值
            *(T*)iModifyAddr = iModifyValue;
        }

        assert(_pstModifyHead->_iNowIndex < sizeof(_pstModifyHead->_stModifyData) / sizeof(tagModifyData));
    }

    void saveAddr(uint32_t iAddr, char cByte=0);

    /**
     * @brief 恢复数据
     */
    void doRecover();

    /**
     * @brief 确认处理完毕
     */
    void doUpdate();

    /**
     * 确认处理完毕
     */
    void doUpdate2();

    /**
     * 确认处理完毕
     */
    void doUpdate3();

    /**
     * 确认处理完毕
     */
    void doUpdate4();

    /**
     * @brief 获取大于n且离n最近的素数
     * @param n
     *
     * @return size_t
     */
    uint32_t getMinPrimeNumber(uint32_t n);

    /**
     * 释放保护区中的chunk
     */
    void deallocate(uint32_t iChunk, uint32_t iIndex);

    /**
     * 释放保护区中的一个block
     */
    void deallocate2(uint32_t iHead);

protected:

    /**
     * 区块指针
     */
    tagMapHead                  *_pHead;

    /**
     * 最小的数据块大小
     */
    uint32_t                    _iMinDataSize;

    /**
     * 最大的数据块大小
     */
    uint32_t                    _iMaxDataSize;

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

