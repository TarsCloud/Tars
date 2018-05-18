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

#ifndef __TC_MULTI_HASHMAP_H__
#define __TC_MULTI_HASHMAP_H__

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
 * @file tc_multi_hashmap.h  
 * @brief 支持多key的hashmap类. 
 * 
 */
/////////////////////////////////////////////////

/**
*  @brief Multi Hash map异常类
*/
struct TC_Multi_HashMap_Exception : public TC_Exception
{
    TC_Multi_HashMap_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_Multi_HashMap_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_Multi_HashMap_Exception() throw(){};
};

////////////////////////////////////////////////////////////////////////////////////
/**
 *  @brief 基于内存的支持多key的hashmap.
 *  
 * 所有操作需要自己加锁 
 *  
 * 所有存储的地址均采用32位保存，为内存块的索引，要求总内存块数不能超过32位范围
 */
class TC_Multi_HashMap
{
public:
    struct HashMapIterator;
    struct HashMapLockIterator;
    /**
    * @brief 真正存储的数据结构
    */
    struct BlockData
    {
        string            _key;       /**数据Key，联合key(联合主键去掉主key后)*/
        string            _value;     /**数据value*/
        bool            _dirty;     /**是否是脏数据*/
        uint8_t            _iVersion;    /**数据版本，1为初始版本，0为保留*/
        time_t            _synct;     /**sync time, 不一定是真正的回写时间*/
        BlockData()
        : _dirty(false)
        , _iVersion(1)
        , _synct(0)
        {
        }
    };

    /**
    * @brief 完整的数据结构，作为get的返回值
    */
    struct Value
    {
        string            _mkey;        /**主key*/
        BlockData        _data;        /**其它数据*/
    };

    /**
    * @brief 判断某个标志位是否已经设置
    * @param bitset, 要判断的字节
    * @param bw, 要判断的位
    */
    static bool ISSET(uint8_t bitset, uint8_t bw) { return bool((bitset & (0x01 << bw)) >> bw); }
    
    /**
    * @brief 设置某个标志位
    */
    static void SET(uint8_t &iBitset, uint8_t bw)
    {
        iBitset |= 0x01 << bw;
    }
    
    /**
    * @brief 清空某个标志位
    */
    static void UNSET(uint8_t &iBitset, uint8_t bw)
    {
        iBitset &= 0xFF ^ (0x01 << bw);
    }

    //////////////////////////////////////////////////////////////////////////
    /**
     * @brief 主key数据块
     */
    class MainKey
    {
        /**
        * @brief 头部中bitwise位的意义
        */
        enum BITWISE
        {
            NEXTCHUNK_BIT = 0,        /**是否有下一个chunk*/
            INTEGRITY_BIT,            /**主key下的数据是否完整*/
        };

    public:
        /** 
          * @brief 主key头
          */
        struct tagMainKeyHead
        {
            uint32_t    _iSize;         /**容量大小*/
            uint32_t    _iIndex;        /**主key hash索引*/
            uint32_t    _iAddr;            /**主key下数据链首地址*/
            uint32_t    _iNext;            /**主key链的下一个主key*/
            uint32_t    _iPrev;            /**主key链的上一个主key*/
            uint32_t    _iGetNext;        /**主key Get链的下一个主key*/
            uint32_t    _iGetPrev;        /**主key Get链的上一个主key*/
            uint32_t    _iBlockCount;    /**主key下数据个数*/
            uint8_t        _iBitset;        /** 8个bit，用于标识不同的bool值，各bit的含义见BITWISE枚举定义*/
            union
            {
                uint32_t    _iNextChunk;    /** 下一个Chunk块地址, _bNextChunk=true时有效*/
                uint32_t    _iDataLen;      /** 当前数据块中使用了的长度, _bNextChunk=false时有效*/
            };
            char            _cData[0];      /** 数据开始地址*/
            tagMainKeyHead()
                : _iSize(0)
                , _iIndex(0)
                , _iAddr(0)
                , _iNext(0)
                , _iPrev(0)
                , _iGetNext(0)
                , _iGetPrev(0)
                , _iBlockCount(0)
                , _iBitset(0)
                , _iDataLen(0)
            {
                _cData[0] = 0;
            }
        }__attribute__((packed));

         /**
         * @brief 
         *        一个chunk放不下数据，后面挂接其它chunk,非第一个chunk的chunk头部
         */
        struct tagChunkHead
        {
            uint32_t    _iSize;        /** 当前chunk的容量大小*/
            bool        _bNextChunk;    /** 是否还有下一个chunk*/
            union
            {
                uint32_t  _iNextChunk;    /** 下一个数据块地址, _bNextChunk=true时有效*/
                uint32_t  _iDataLen;      /** 当前数据块中使用了的长度, _bNextChunk=false时有效*/
            };
            char        _cData[0];      /** 数据开始地址*/

            tagChunkHead()
            :_iSize(0)
            ,_bNextChunk(false)
            ,_iDataLen(0)
            {
                _cData[0] = 0;
            }

        }__attribute__((packed));

        /**
         * @brief 构造函数
         * @param pMap
         * @param iAddr, 主key的地址
         */
        MainKey(TC_Multi_HashMap *pMap, uint32_t iAddr)
        : _pMap(pMap)
        , _iHead(iAddr)
        {
            // 构造时保存绝对地址，避免每次都计算
            _pHead = _pMap->getAbsolute(iAddr);
        }

        /**
         * @brief 拷贝构造函数
         * @param mk
         */
        MainKey(const MainKey &mk)
        : _pMap(mk._pMap)
        , _iHead(mk._iHead)
        , _pHead(mk._pHead)
        {
        }

        /**
         * @brief 赋值操作符
         * @param mk
         *
         * @return Block&
         */
        MainKey& operator=(const MainKey &mk)
        {
            _iHead = mk._iHead;
            _pMap  = mk._pMap;
            _pHead = mk._pHead;
            return (*this);
        }

        /**
         *
         * @param mb
         *
         * @return bool
         */
        bool operator==(const MainKey &mk) const { return _iHead == mk._iHead && _pMap == mk._pMap; }

        /**
         *
         * @param mb
         *
         * @return bool
         */
        bool operator!=(const MainKey &mk) const { return _iHead != mk._iHead || _pMap != mk._pMap; }

        /**
         * @brief 获取主key头指针
         *
         * @return tagMainKeyHead*
         */
        tagMainKeyHead* getHeadPtr() { return (tagMainKeyHead *)_pHead; }


        /**
         * @brief 根据主key头地址获取主key头指针
         * @param iAddr, 主key头地址
         * @return tagMainKeyHead*
         */
        tagMainKeyHead* getHeadPtr(uint32_t iAddr) { return ((tagMainKeyHead*)_pMap->getAbsolute(iAddr)); }

        /**
         * @brief 获取主key头的地址
         *
         * @return uint32_t
         */
        uint32_t getHead() { return _iHead; }
        
        /**
         * @brief 获取主key
         * @param mk, 主key
         * @return int
         *          TC_Multi_HashMap::RT_OK, 正常
         *          其他异常
         */
        int get(string &mk);

        /**
         * @brief 设置主key
         * @param pData
         * @param iDatalen
         * @param vtData, 淘汰的数据
         */
        int set(const void *pData, uint32_t iDataLen, vector<TC_Multi_HashMap::Value> &vtData);

        /**
         * @brief 将当前主key移动到主key链上的下一个主key
         * @return true, 移到下一个主key了, false, 没有下一个主key
         *
         */
        bool next();

        /**
         * @brief 将当前主key移动到主key链上的上一个主key
         * @return true, 移到上一个主key了, false, 没有上一个主key
         *
         */
        bool prev();

        /**
         * @brief 释放所有空间
         */
        void deallocate();

        /**
         * @brief 新主key时调用该函数，初始化主key相关信息
         * @param iIndex, 主key hash索引
         * @param iAllocSize, 内存大小
         */
        void makeNew(uint32_t iIndex, uint32_t iAllocSize);

        /**
         * @brief 从主key链表中删除当前主key
         * 返回被删除的主key下的所有数据
         * @return int, TC_Multi_HashMap::RT_OK成功，其它失败
         */
        int erase(vector<Value> &vtData);

        /**
         * @brief 刷新get链表, 将当前主key放在get链表头部
         */
        void refreshGetList();

    protected:

        /**
         * @brief 获取chunk头指针
         * @param iAddr, chunk头地址索引
         *
         * @return tagChunkHead*
         */
        tagChunkHead *getChunkHead(uint32_t iAddr) { return ((tagChunkHead*)_pMap->getAbsolute(iAddr)); }

        /**
         * @brief 如果数据容量不够, 则新增加chunk, 不影响原有数据
         * 使新增加的总容量大于iDataLen
         * 释放多余的chunk
         * @param iDataLen
         * @param vtData, 返回被淘汰的数据
         *
         * @return int
         */
        int allocate(uint32_t iDataLen, vector<TC_Multi_HashMap::Value> &vtData);

        /**
         * @brief 挂接chunk, 如果core则挂接失败, 保证内存块还可以用
         * @param pChunk, 第一个chunk指针
         * @param chunks, 所有的chunk地址
         *
         * @return int
         */
        int joinChunk(tagChunkHead *pChunk, const vector<uint32_t>& chunks);

        /**
         * @brief 分配指定大小的内存空间，可能会有多个chunk
         * @param fn, 分配的空间大小
         * @param chunks, 分配成功返回的chunks地址列表
         * @param vtData, 淘汰的数据
         * @return int
         */
        int allocateChunk(uint32_t fn, vector<uint32_t> &chunks, vector<TC_Multi_HashMap::Value> &vtData);

        /**
         * @brief 释放指定chunk之后的所有chunk
         * @param iChunk 释放地址
         */
        void deallocate(uint32_t iChunk);

        /**
         * @brief 获取主key存储空间大小
         *
         * @return uint32_t
         */
        uint32_t getDataLen();

        /**
         * @brief 获取主key
         * @param pData
         * @param iDatalen
         * @return int,
         *          TC_Multi_HashMap::RT_OK, 正常
         *          其他异常
         */
        int get(void *pData, uint32_t &iDataLen);

    public:
        bool ISFULLDATA()                        { return TC_Multi_HashMap::ISSET(getHeadPtr()->_iBitset, INTEGRITY_BIT); }
    protected:
        bool HASNEXTCHUNK()                        { return TC_Multi_HashMap::ISSET(getHeadPtr()->_iBitset, NEXTCHUNK_BIT); }
        
    public:
        void SETFULLDATA(bool b)                { if(b) TC_Multi_HashMap::SET(getHeadPtr()->_iBitset, INTEGRITY_BIT); else TC_Multi_HashMap::UNSET(getHeadPtr()->_iBitset, INTEGRITY_BIT); }
    protected:
        void SETNEXTCHUNK(bool b)                { if(b) TC_Multi_HashMap::SET(getHeadPtr()->_iBitset, NEXTCHUNK_BIT); else TC_Multi_HashMap::UNSET(getHeadPtr()->_iBitset, NEXTCHUNK_BIT); }

    private:

        /**
         * Map
         */
        TC_Multi_HashMap         *_pMap;

        /**
         * 主key头首地址, 相对地址，内存块索引
         */
        uint32_t                _iHead;

        /**
        * 主key头首地址，绝对地址
        */
        void                    *_pHead;

    };

    ///////////////////////////////////////////////////////////////////////////////////
    /**
    * @brief 联合主key及数据块
    */
    class Block
    {
    public:

        /**
        * @brief block头部中bitwise位的意义
        */
        enum BITWISE
        {
            NEXTCHUNK_BIT = 0,        /**是否有下一个chunk*/
            DIRTY_BIT,                /**是否为脏数据位*/
            ONLYKEY_BIT,            /**是否为OnlyKey*/
        };

        /**
         * @brief block数据头
         */
        struct tagBlockHead
        {
            uint32_t        _iSize;         /**block的容量大小*/
            uint32_t        _iIndex;        /**hash的索引*/
            uint32_t        _iUKBlockNext;  /**联合主键block链的下一个Block, 没有则为0*/
            uint32_t        _iUKBlockPrev;  /**联合主键block链的上一个Block, 没有则为0*/
            uint32_t        _iMKBlockNext;  /**主key block链的下一个Block, 没有则为0*/
            uint32_t        _iMKBlockPrev;  /**主key block链的上一个Block, 没有则为0*/
            uint32_t        _iSetNext;      /**Set链上的上一个Block, 没有则为0*/
            uint32_t        _iSetPrev;      /**Set链上的上一个Block, 没有则为0*/
            uint32_t        _iMainKey;        /**指向所属主key头*/
            time_t            _iSyncTime;     /**上次缓写时间*/
            uint8_t            _iVersion;        /**数据版本，1为初始版本，0为保留*/
            uint8_t            _iBitset;        /**8个bit，用于标识不同的bool值，各bit的含义见BITWISE枚举定义*/
            union
            {
                uint32_t    _iNextChunk;    /**下一个Chunk块, _iBitwise中的NEXTCHUNK_BIT为1时有效*/
                uint32_t    _iDataLen;      /**当前数据块中使用了的长度, _iBitwise中的NEXTCHUNK_BIT为0时有效*/
            };
            char            _cData[0];      /**数据开始部分*/

            tagBlockHead()
            :_iSize(0)
            ,_iIndex(0)
            ,_iUKBlockNext(0)
            ,_iUKBlockPrev(0)
            ,_iMKBlockNext(0)
            ,_iMKBlockPrev(0)
            ,_iSetNext(0)
            ,_iSetPrev(0)
            ,_iMainKey(0)
            ,_iSyncTime(0)
            ,_iVersion(1)
            ,_iBitset(0)
            ,_iDataLen(0)
            {
                _cData[0] = 0;
            }

        }__attribute__((packed));

        /**
         * @brief 
         *        一个chunk放不下所有数据，后面挂接其它chunk,其它chunk头部
         */
        struct tagChunkHead
        {
            uint32_t    _iSize;          /**当前chunk的容量大小*/
            bool        _bNextChunk;     /**是否还有下一个chunk*/
            union
            {
                uint32_t  _iNextChunk;     /**下一个chunk, _bNextChunk=true时有效*/
                uint32_t  _iDataLen;       /**当前chunk中使用了的长度, _bNextChunk=false时有效*/
            };
            char        _cData[0];      /**数据开始部分*/

            tagChunkHead()
            :_iSize(0)
            ,_bNextChunk(false)
            ,_iDataLen(0)
            {
                _cData[0] = 0;
            }

        }__attribute__((packed));

        /**
         * @brief 构造函数
         * @param Map
         * @param Block的地址
         * @param pAdd
         */
        Block(TC_Multi_HashMap *pMap, uint32_t iAddr)
        : _pMap(pMap)
        , _iHead(iAddr)
        {
            // 构造时保存绝对地址，避免每次都计算
            _pHead = _pMap->getAbsolute(iAddr);
        }

        /**
         * @brief 拷贝构造函数
         * @param mb
         */
        Block(const Block &mb)
        : _pMap(mb._pMap)
        , _iHead(mb._iHead)
        , _pHead(mb._pHead)
        {
        }

        /**
         * @brief  赋值运算符
         * @param mb
         *
         * @return Block&
         */
        Block& operator=(const Block &mb)
        {
            _iHead = mb._iHead;
            _pMap  = mb._pMap;
            _pHead = mb._pHead;
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
         * @brief 获取Block头指针
         *
         * @return tagBlockHead*
         */
        tagBlockHead *getBlockHead() {return (tagBlockHead*)_pHead; }

        /**
         * @brief 根据头地址获取Block头指针
         * @param iAddr, block头地址
         * @return tagBlockHead*
         */
        tagBlockHead *getBlockHead(uint32_t iAddr) { return ((tagBlockHead*)_pMap->getAbsolute(iAddr)); }

        /**
         * @brief 获取头部地址
         *
         * @return size_t
         */
        uint32_t getHead() { return _iHead;}

        /**
         * @brief 获取当前桶链表最后一个block的头部地址
         * @param bUKList, 是联合主键链还是主key链
         * @return uint32_t
         */
        uint32_t getLastBlockHead(bool bUKList);

        /**
         * @brief 获取回写时间
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
        * @brief 获取数据版本
        */
        uint8_t getVersion() { return getBlockHead()->_iVersion; }

        /**
        * @brief 设置数据版本
        */
        void setVersion(uint8_t iVersion) { getBlockHead()->_iVersion = iVersion; }

        /**
         * @brief 获取Block中的数据
         * @param data 要获取数据的Block
         * @return int
         *          TC_Multi_HashMap::RT_OK, 正常, 其他异常
         *          TC_Multi_HashMap::RT_ONLY_KEY, 只有Key
         *          其他异常
         */
        int getBlockData(TC_Multi_HashMap::BlockData &data);

        /**
         * @brief 获取原始数据
         * @param pData
         * @param iDatalen
         * @return int,
         *          TC_Multi_HashMap::RT_OK, 正常
         *          其他异常
         */
        int get(void *pData, uint32_t &iDataLen);

        /**
         * @brief 获取原始数据
         * @param s
         * @return int
         *          TC_Multi_HashMap::RT_OK, 正常
         *          其他异常
         */
        int get(string &s);

        /**
         * @brief 设置数据
         * @param pData
         * @param iDatalen
         * @param bOnlyKey
         * @param iVersion, 数据版本，应该根据get出的数据版本写回，为0表示不关心数据版本
         * @param vtData, 淘汰的数据
         * @return int
         *                RT_OK, 设置成功
         *                RT_DATA_VER_MISMATCH, 要设置的数据版本与当前版本不符，应该重新get后再set
         *                其它为失败
         */
        int set(const void *pData, uint32_t iDataLen, bool bOnlyKey, uint8_t iVersion, vector<TC_Multi_HashMap::Value> &vtData);

        /**
         * @brief 是否是脏数据
         *
         * @return bool
         */
        bool isDirty()      { return ISDIRTY(); }

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
        bool isOnlyKey()    { return ISONLYKEY(); }

        /**
         * @brief 当前元素移动到联合主键block链的下一个block
         * @return true, 移到下一个block了, false, 没有下一个block
         *
         */
        bool nextBlock();

        /**
         * @brief 当前元素移动到联合主键block链的上一个block
         * @return true, 移到上一个block了, false, 没有上一个block
         *
         */
        bool prevBlock();

        /**
         * @brief 释放block的所有空间
         */
        void deallocate();

        /**
         * @brief 新block时调用该函数
         * 初始化新block的一些信息
         * @param iMainKeyAddr, 所属主key地址
         * @param uIndex, 联合主键hash索引
         * @param iAllocSize, 内存大小
         * @param bHead, 插入到主key链上的顺序，前序或后序
         */
        void makeNew(uint32_t iMainKeyAddr, uint32_t uIndex, uint32_t iAllocSize, bool bHead);

        /**
         * @brief 从Block链表中删除当前Block
         * @return
         */
        void erase();

        /**
         * @brief 刷新set链表, 将当前block放在Set链表头部
         */
        void refreshSetList();

    protected:

        /**
         * @brief 根据chunk头地址获取chunk头指针
         * @param iAddr
         *
         * @return tagChunkHead*
         */
        tagChunkHead *getChunkHead(uint32_t iAddr) { return ((tagChunkHead*)_pMap->getAbsolute(iAddr)); }

        /**
         * @brief 如果数据容量不够, 则新增加chunk, 不影响原有数据
         * 使新增加的总容量大于iDataLen，释放多余的chunk 
         * @param iDataLen
         * @param vtData, 淘汰的数据
         *
         * @return int,
         */
        int allocate(uint32_t iDataLen, vector<TC_Multi_HashMap::Value> &vtData);

        /**
         * @brief 挂接chunk, 如果core则挂接失败, 保证内存块还可以用
         * @param pChunk, 第一个chunk指针
         * @param chunks, 所有的chunk地址
         *
         * @return int
         */
        int joinChunk(tagChunkHead *pChunk, const vector<uint32_t>& chunks);

        /**
         * @brief 分配指定大小的内存空间, 可能会有多个chunk
         * @param fn, 分配的空间大小
         * @param chunks, 分配成功返回的chunks地址列表
         * @param vtData, 淘汰的数据
         * @return int
         */
        int allocateChunk(uint32_t fn, vector<uint32_t> &chunks, vector<TC_Multi_HashMap::Value> &vtData);

        /**
         * @brief 释放指定chunk之后的所有chunk
         * @param iChunk 释放地址
         */
        void deallocate(uint32_t iChunk);

        /**
         * @brief 获取数据长度
         *
         * @return size_t
         */
        uint32_t getDataLen();

        bool HASNEXTCHUNK()                        { return TC_Multi_HashMap::ISSET(getBlockHead()->_iBitset, NEXTCHUNK_BIT); }
        bool ISDIRTY()                            { return TC_Multi_HashMap::ISSET(getBlockHead()->_iBitset, DIRTY_BIT); }
        bool ISONLYKEY()                        { return TC_Multi_HashMap::ISSET(getBlockHead()->_iBitset, ONLYKEY_BIT); }

        void SETNEXTCHUNK(bool b)                { if(b) TC_Multi_HashMap::SET(getBlockHead()->_iBitset, NEXTCHUNK_BIT); else TC_Multi_HashMap::UNSET(getBlockHead()->_iBitset, NEXTCHUNK_BIT); }
        void SETDIRTY(bool b)                    { if(b) TC_Multi_HashMap::SET(getBlockHead()->_iBitset, DIRTY_BIT); else TC_Multi_HashMap::UNSET(getBlockHead()->_iBitset, DIRTY_BIT); }
        void SETONLYKEY(bool b)                    { if(b) TC_Multi_HashMap::SET(getBlockHead()->_iBitset, ONLYKEY_BIT); else TC_Multi_HashMap::UNSET(getBlockHead()->_iBitset, ONLYKEY_BIT); }

    private:

        /**
         * Map
         */
        TC_Multi_HashMap         *_pMap;

        /**
         * block区块首地址, 相对地址，内存块索引
         */
        uint32_t                _iHead;

        /**
        * Block首地址，绝对地址
        */
        void                    *_pHead;

    };

    ////////////////////////////////////////////////////////////////////////
    /**
    * @brief 内存数据块分配器，可同时为数据区和主key区分配内存
    *
    */
    class BlockAllocator
    {
    public:

        /**
         * @brief 构造函数
         */
        BlockAllocator(TC_Multi_HashMap *pMap)
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
         * @param fFactor, 数据块增长因子
         */
        void create(void *pHeadAddr, size_t iSize, size_t iMinBlockSize, size_t iMaxBlockSize, float fFactor)
        {
            _pChunkAllocator->create(pHeadAddr, iSize, iMinBlockSize, iMaxBlockSize, fFactor);
        }

        /**
         * @brief 连接到已经结构化的内存(如共享内存)
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
         * @brief 重建内存结构
         */
        void rebuild()
        {
            _pChunkAllocator->rebuild();
        }

        /**
         * @brief 获取每种数据块头部信息
         *
         * @return vector<TC_MemChunk::tagChunkHead>
         */
        vector<TC_MemChunk::tagChunkHead> getBlockDetail() const  { return _pChunkAllocator->getBlockDetail(); }

        /**
         * @brief 总内存大小
         *
         * @return size_t
         */
        size_t getMemSize() const       { return _pChunkAllocator->getMemSize(); }

        /**
         * @brief 实际的数据容量
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
         * @brief 在内存中分配一个新的Block，实际上只分配一个chunk， 
         *        并初始化Block头
         * @param iMainKeyAddr, 新block所属主key地址
         * @param index, block hash索引
         * @param bHead, 新块插入到主key链上的顺序，前序或后序
         * @param iAllocSize: in/需要分配的大小, out/分配的块大小
         * @param vtData, 返回淘汰的数据
         * @return size_t, 内存块地址索引, 0表示没有空间可以分配
         */
        uint32_t allocateMemBlock(uint32_t iMainKeyAddr, uint32_t index, bool bHead, uint32_t &iAllocSize, vector<TC_Multi_HashMap::Value> &vtData);

        /**
        * @brief 在内存中分配一个主key头，只需要一个chunk即可
        * @param index, 主key hash索引
        * @param iAllocSize: in/需要分配的大小, out/分配的块大小
        * @param vtData, 返回释放的内存块数据
        * @return size_t, 主key头首地址,0表示没有空间可以分配
        */
        uint32_t allocateMainKeyHead(uint32_t index, vector<TC_Multi_HashMap::Value> &vtData);

        /**
         * @brief 为地址为iAddr的Block分配一个chunk         *
         * @param iAddr,分配的Block的地址
         * @param iAllocSize, in/需要分配的大小, out/分配的块大小
         * @param vtData 返回释放的内存块数据
         * @return size_t, 相对地址,0表示没有空间可以分配
         */
        uint32_t allocateChunk(uint32_t iAddr, uint32_t &iAllocSize, vector<TC_Multi_HashMap::Value> &vtData);

        /**
         * @brief 释放Block
         * @param v, 需要释放的chunk的地址列表
         */
        void deallocateMemChunk(const vector<uint32_t> &v);

        /**
         * @brief 释放Block
         * @param iChunk, 需要释放的chunk地址
         */
        void deallocateMemChunk(uint32_t iChunk);

    protected:
        /**
         * @brief 不允许copy构造
         */
        BlockAllocator(const BlockAllocator &);
        /**
         * @brief 不允许赋值
         */
        BlockAllocator& operator=(const BlockAllocator &);
        bool operator==(const BlockAllocator &mba) const;
        bool operator!=(const BlockAllocator &mba) const;

    public:
        /**
         * map
         */
        TC_Multi_HashMap                *_pMap;

        /**
         * chunk分配器
         */
        TC_MemMultiChunkAllocator        *_pChunkAllocator;
    };

    ////////////////////////////////////////////////////////////////
    /** 
      * @brief 构造map的数据项
      */
    class HashMapLockItem
    {
    public:

        /**
         *
         * @param pMap, 
         * @param iAddr, 与LockItem对应的Block首地址
         */
        HashMapLockItem(TC_Multi_HashMap *pMap, uint32_t iAddr);

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
         * @return time_t
         */
        time_t getSyncTime();

        /**
         * @brief 获取键与值
         * @param v
         * @return int
         *          RT_OK:数据获取OK
         *          RT_ONLY_KEY: key有效, v无效为空
         *          其他值, 异常
         *
         */
        int get(TC_Multi_HashMap::Value &v);

        /**
         * @brief 仅获取key
         * @param mk, 主key
         * @param uk, 联合主key(除主key外的联合主键)
         * @return int
         *          RT_OK:数据获取OK
         *          其他值, 异常
         */
        int get(string &mk, string &uk);

        /**
         * @brief 获取对应block的相对地址
         *
         * @return size_t
         */
        uint32_t getAddr() const { return _iAddr; }

    protected:

        /**
         * @brief 设置数据
         * @param mk, 主key
         * @param uk, 除主key外的联合主键
         * @param v, 数据值
         * @param iVersion, 数据版本(1-255), 0表示不关注版本
         * @param vtData, 淘汰的数据
         * @return int
         */
        int set(const string &mk, const string &uk, const string& v, uint8_t iVersion, vector<TC_Multi_HashMap::Value> &vtData);

        /**
         * @brief 设置Key, 无数据(Only Key)
         * @param mk, 主key
         * @param uk, 除主key外的联合主键
         * @param vtData, 淘汰的数据
         *
         * @return int
         */
        int set(const string &mk, const string &uk, vector<TC_Multi_HashMap::Value> &vtData);

        /**
         * @brief 判断当前item是否是指定key的item, 如果是还返回value
         * @param pKey
         * @param iKeyLen
         *
         * @return bool
         */
        bool equal(const string &mk, const string &uk, TC_Multi_HashMap::Value &v, int &ret);

        /**
         * @brief 判断当前item是否是指定key的item
         * @param pKey
         * @param iKeyLen
         *
         * @return bool
         */
        bool equal(const string &mk, const string &uk, int &ret);

        /**
         * @brief 将当前item移动到下一个item
         *
         * @return HashMapLockItem
         */
        void nextItem(int iType);

        /**
         * @brief 将当前item移动到上一个item
         * @param iType
         */
        void prevItem(int iType);

        friend class TC_Multi_HashMap;
        friend struct TC_Multi_HashMap::HashMapLockIterator;

    private:
        /**
         * map
         */
        TC_Multi_HashMap *_pMap;

        /**
         * 对应的block的地址
         */
        uint32_t      _iAddr;
    };

    /////////////////////////////////////////////////////////////////////////
    /** 
      * @brief  定义迭代器 
       */ 
    struct HashMapLockIterator
    {
    public:

        /** 
          * @brief 定义遍历方式
          */
        enum
        {
            IT_BLOCK    = 0,        /**普通的顺序*/
            IT_SET      = 1,        /**Set时间顺序*/
            IT_GET      = 2,        /**Get时间顺序*/
            IT_MKEY        = 3,        /**同一主key下的block遍历*/
            IT_UKEY        = 4,        /**同一联合主键下的block遍历*/
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
         * @param pMap,
         * @param iAddr, 对应的block地址
         * @param iType, 遍历类型
         * @param iOrder, 遍历的顺序
         */
        HashMapLockIterator(TC_Multi_HashMap *pMap, uint32_t iAddr, int iType, int iOrder);

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
        TC_Multi_HashMap  *_pMap;

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
     *  @brief map的HashItem项, 一个HashItem对应多个数据项
     */
    class HashMapItem
    {
    public:

        /**
         *
         * @param pMap
         * @param iIndex, Hash索引
         */
        HashMapItem(TC_Multi_HashMap *pMap, uint32_t iIndex);

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
         * @brief 获取当前hash桶的所有数据, 注意只获取有key/value的数据
         * 对于只有key的数据, 不获取
         * @param vtData
         * @return
         */
        void get(vector<TC_Multi_HashMap::Value> &vtData);

        /**
         * @brief 获取当前item的hash索引
         * 
         * @return int
         */
        uint32_t getIndex() const { return _iIndex; }

        /**
         * @brief 将当前item移动为下一个item
         *
         */
        void nextItem();

        friend class TC_Multi_HashMap;
        friend struct TC_Multi_HashMap::HashMapIterator;

    private:
        /**
         * map
         */
        TC_Multi_HashMap *_pMap;

        /**
         * 对应的数据块索引
         */
        uint32_t      _iIndex;
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
         * @param iIndex, hash索引
         * @param type
         */
        HashMapIterator(TC_Multi_HashMap *pMap, uint32_t iIndex);

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
        TC_Multi_HashMap  *_pMap;

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
        char        _cMaxVersion;        /**大版本*/
        char        _cMinVersion;        /**小版本*/
        bool        _bReadOnly;          /**是否只读*/
        bool        _bAutoErase;         /**是否可以自动淘汰*/
        char        _cEraseMode;         /**淘汰方式:0x00:按照Get链淘汰, 0x01:按照Set链淘汰*/
        size_t        _iMemSize;           /**内存大小*/
        size_t        _iMinDataSize;       /**最小数据块大小*/
        size_t        _iMaxDataSize;       /**最大数据块大小*/
        float        _fFactor;            /**因子*/
        float        _fHashRatio;         /**chunks个数/hash个数*/
        float        _fMainKeyRatio;         /**chunks个数/主key hash个数*/
        size_t        _iElementCount;      /**总元素个数*/
        size_t        _iEraseCount;        /**每次淘汰个数*/
        size_t        _iDirtyCount;        /**脏数据个数*/
        uint32_t    _iSetHead;           /**Set时间链表头部*/
        uint32_t    _iSetTail;           /**Set时间链表尾部*/
        uint32_t    _iGetHead;           /**Get时间链表头部*/
        uint32_t    _iGetTail;           /**Get时间链表尾部*/
        uint32_t    _iDirtyTail;         /**脏数据链尾部*/
        uint32_t    _iBackupTail;        /**热备指针*/
        uint32_t    _iSyncTail;          /**回写链表*/
        time_t        _iSyncTime;          /**回写时间*/
        size_t        _iUsedChunk;         /**已经使用的内存块*/
        size_t        _iGetCount;          /**get次数*/
        size_t        _iHitCount;          /**命中次数*/
        size_t        _iMKOnlyKeyCount;     /**主key的onlykey个数*/
        size_t        _iOnlyKeyCount;         /**主键的OnlyKey个数, 这个数通常为0*/
        size_t        _iMaxBlockCount;     /**主key链下最大的记录数，这个数值要监控，不能太大，否则会导致查询变慢*/
        size_t        _iReserve[4];        /**保留*/
    }__attribute__((packed));

    /**
     * @brief 需要修改的地址
     */
    struct tagModifyData
    {
        size_t        _iModifyAddr;       /**修改的地址*/
        char        _cBytes;            /**字节数*/
        size_t        _iModifyValue;      /**值*/
    }__attribute__((packed));

    /**
     * @brief 修改数据块头部
     */
    struct tagModifyHead
    {
        char            _cModifyStatus;         /**修改状态: 0:目前没有人修改, 1: 开始准备修改, 2:修改完毕, 没有copy到内存中*/
        size_t          _iNowIndex;             /**更新到目前的索引, 不能操作1000个*/
        tagModifyData   _stModifyData[1000];     /**一次最多1000次修改*/
    }__attribute__((packed));

    /**
     * @brief HashItem
     */
    struct tagHashItem
    {
        uint32_t    _iBlockAddr;        /**指向数据项的内存地址索引*/
        uint32_t    _iListCount;        /**链表个数*/
    }__attribute__((packed));

    /**
    * @brief 主key HashItem
    */
    struct tagMainKeyHashItem
    {
        uint32_t    _iMainKeyAddr;        /**主key数据项的偏移地址*/
        uint32_t    _iListCount;        /**相同主key hash索引下主key个数*/
    }__attribute__((packed));

    /**64位操作系统用基数版本号, 32位操作系统用偶数版本号*/
#if __WORDSIZE == 64

    /**
    * @brief 定义版本号
    */
    enum
    {
        MAX_VERSION         = 1,        /**当前map的大版本号*/
        MIN_VERSION         = 1,        /**当前map的小版本号*/
    };

#else
    /**
    * @brief 定义版本号
    */
    enum
    {
        MAX_VERSION         = 1,        /**当前map的大版本号*/
        MIN_VERSION         = 0,        /**当前map的小版本号*/
    };

#endif

    /**
    * @brief 定义淘汰方式
    */
    enum
    {
        ERASEBYGET          = 0x00,        /**按照Get链表淘汰*/
        ERASEBYSET          = 0x01,        /**按照Set链表淘汰*/
    };

    /**
    * @brief 定义设置数据时的选项
    */
    enum DATATYPE
    {
        PART_DATA            = 0,        /**不完整数据*/
        FULL_DATA            = 1,        /**完整数据*/
        AUTO_DATA            = 2,        /**根据内部的数据完整性状态来决定*/
    };

    /**
     * @brief  get, set等int返回值
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
        RT_DATA_VER_MISMATCH    = 11,    /**写入数据版本不匹配*/
        RT_PART_DATA            = 12,    /**主key数据不完整*/
        RT_DECODE_ERR           = -1,   /**解析错误*/
        RT_EXCEPTION_ERR        = -2,   /**异常*/
        RT_LOAD_DATA_ERR        = -3,   /**加载数据异常*/
        RT_VERSION_MISMATCH_ERR = -4,   /**版本不一致*/
        RT_DUMP_FILE_ERR        = -5,   /**dump到文件失败*/
        RT_LOAD_FILE_ERR        = -6,   /**load文件到内存失败*/
        RT_NOTALL_ERR           = -7,   /**没有复制完全*/
    };

    /**定义迭代器*/
    typedef HashMapIterator     hash_iterator;
    typedef HashMapLockIterator lock_iterator;

    /**定义hash处理器*/
    using hash_functor = std::function<uint32_t (const string& )>;

    //////////////////////////////////////////////////////////////////////////////////////////////
    //map的接口定义

    /**
     * @brief 构造函数
     */
    TC_Multi_HashMap()
    : _iMinDataSize(0)
    , _iMaxDataSize(0)
    , _fFactor(1.0)
    , _fHashRatio(2.0)
    , _fMainKeyRatio(1.0)
    , _pDataAllocator(new BlockAllocator(this))
    , _lock_end(this, 0, 0, 0)
    , _end(this, (uint32_t)(-1))
    , _hashf(magic_string_hash())
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
    void initDataBlockSize(size_t iMinDataSize, size_t iMaxDataSize, float fFactor);

    /**
     * @brief 始化chunk数据块/hash项比值, 默认是2,
     * 有需要更改必须在create之前调用
     *
     * @param fRatio
     */
    void initHashRatio(float fRatio)                { _fHashRatio = fRatio;}

    /**
     * @brief 初始化chunk个数/主key hash个数, 默认是1, 
     * 含义是一个主key下面大概有多个条数据 有需要更改必须在create之前调用 
     *
     * @param fRatio
     */
    void initMainKeyHashRatio(float fRatio)         { _fMainKeyRatio = fRatio;}

    /**
     * @brief 初始化, 之前需要调用:initDataAvgSize和initHashRatio
     * @param pAddr 外部分配好的存储的绝对地址
     * @param iSize 存储空间大小
     * @return 失败则抛出异常
     */
    void create(void *pAddr, size_t iSize);

    /**
     * @brief 链接到已经格式化的内存块
     * @param pAddr, 内存地址
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
     * @brief 获取数据区hash桶的个数
     *
     * @return size_t
     */
    size_t getHashCount()                           { return _hash.size(); }

    /**
    * @brief 获取主key hash桶个数
    */
    size_t getMainKeyHashCount()                    { return _hashMainKey.size(); }

    /**
     * @brief 获取元素的个数
     *
     * @return size_t
     */
    size_t size()                                   { return _pHead->_iElementCount; }

    /**
     * @brief 脏数据元素个数
     *
     * @return size_t
     */
    size_t dirtyCount()                             { return _pHead->_iDirtyCount;}

    /**
     * @brief 主键OnlyKey数据元素个数
     *
     * @return size_t
     */
    size_t onlyKeyCount()                           { return _pHead->_iOnlyKeyCount;}

    /**
     * @brief 主key OnlyKey数据元素个数
     *
     * @return size_t
     */
    size_t onlyKeyCountM()                          { return _pHead->_iMKOnlyKeyCount;}

    /**
     * @brief 设置每次淘汰数量
     * @param n
     */
    void setEraseCount(size_t n)                    { _pHead->_iEraseCount = n; }

    /**
     * @brief 获取每次淘汰数量
     *
     * @return size_t
     */
    size_t getEraseCount()                          { return _pHead->_iEraseCount; }

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
     * TC_Multi_HashMap::ERASEBYGET
     * TC_Multi_HashMap::ERASEBYSET
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
     * @brief 设置回写时间间隔(秒)
     * @param iSyncTime
     */
    void setSyncTime(time_t iSyncTime)              { _pHead->_iSyncTime = iSyncTime; }

    /**
     * @brief 获取回写时间
     *
     * @return time_t
     */
    time_t getSyncTime()                            { return _pHead->_iSyncTime; }

    /**
     * @brief 获取头部数据信息
     * 
     * @return tagMapHead&
     */
    tagMapHead& getMapHead()                        { return *_pHead; }

    /**
     * @brief 设置联合主键hash方式
     * @param hashf
     */
    void setHashFunctor(hash_functor hashf)         { _hashf = hashf; }

    /**
    * @brief 设置主key的hash方式
    * @param hashf
    */
    void setHashFunctorM(hash_functor hashf)            { _mhashf = hashf; } 

    /**
     * @brief 返回hash处理器
     * 
     * @return hash_functor&
     */
    hash_functor &getHashFunctor()                  { return _hashf; }
    hash_functor &getHashFunctorM()                    { return _mhashf; }

    /**
     * @brief 获取指定索引的hash item
     * @param index, hash索引
     *
     * @return tagHashItem&
     */
    tagHashItem *item(size_t iIndex)                { return &_hash[iIndex]; }

    /**
    * @brief 根据主key hash索引取主key item
    * @param iIndex, 主key的hash索引
    */
    tagMainKeyHashItem* itemMainKey(size_t iIndex)    { return &_hashMainKey[iIndex]; }

    /**
     * @brief dump到文件
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
     * @brief 清空hashmap
     * 所有map的数据恢复到初始状态
     */
    void clear();

    /**
    * @brief 检查主key是否存在
    * @param mk, 主key
    *
    * @return int
    *        TC_Multi_HashMap::RT_OK, 主key存在，且有数据
    *        TC_Multi_HashMap::RT_ONLY_KEY, 主key存在，没有数据
    *        TC_Multi_HashMap::RT_PART_DATA, 主key存在，里面的数据可能不完整
    *        TC_Multi_HashMap::RT_NO_DATA, 主key不存在
    */
    int checkMainKey(const string &mk);

    /**
    * @brief 设置主key下数据的完整性
    * @param mk, 主key
    * @param bFull, true为完整数据，false为不完整数据
    *
    * @return
    *          RT_READONLY: 只读
    *          RT_NO_DATA: 没有当前数据
    *          RT_OK: 设置成功
    *          其他返回值: 错误
    */
    int setFullData(const string &mk, bool bFull);

    /**
     * @brief 检查数据干净状态
     * @param mk, 主key
     * @param uk, 除主key外的联合主键
     *
     * @return int
     *          RT_NO_DATA: 没有当前数据
     *          RT_ONLY_KEY:只有Key
     *          RT_DIRTY_DATA: 是脏数据
     *          RT_OK: 是干净数据
     *          其他返回值: 错误
     */
    int checkDirty(const string &mk, const string &uk);

    /**
    * @brief 
    *        检查主key下数据的干净状态，只要主key下面有一条脏数据，则返回脏
    * @param mk, 主key
    * @return int
    *          RT_NO_DATA: 没有当前数据
    *          RT_ONLY_KEY:只有Key
    *          RT_DIRTY_DATA: 是脏数据
    *          RT_OK: 是干净数据
    *          其他返回值: 错误
    */
    int checkDirty(const string &mk);

    /**
     * @brief 设置为脏数据, 修改SET时间链, 会导致数据回写
     * @param mk, 主key
     * @param uk, 除主key外的联合主键
     *
     * @return int
     *          RT_READONLY: 只读
     *          RT_NO_DATA: 没有当前数据
     *          RT_ONLY_KEY:只有Key
     *          RT_OK: 设置脏数据成功
     *          其他返回值: 错误
     */
    int setDirty(const string &mk, const string &uk);

    /**
     * @brief 设置为干净数据, 修改SET链, 导致数据不回写
     * @param mk, 主key
     * @param uk, 除主key外的联合主键
     *
     * @return int
     *          RT_READONLY: 只读
     *          RT_NO_DATA: 没有当前数据
     *          RT_ONLY_KEY:只有Key
     *          RT_OK: 设置成功
     *          其他返回值: 错误
     */
    int setClean(const string &mk, const string &uk);

    /**
    * @brief 更新数据的回写时间
    * @param mk,
    * @param uk,
    * @param iSynctime
    *
    * @return int
    *          RT_READONLY: 只读
    *          RT_NO_DATA: 没有当前数据
    *          RT_ONLY_KEY:只有Key
    *          RT_OK: 设置脏数据成功
    *          其他返回值: 错误
    */
    int setSyncTime(const string &mk, const string &uk, time_t iSyncTime);

    /**
     * @brief 获取数据, 修改GET时间链
     * @param mk, 主key
     * @param uk, 除主key外的联合主键
     @ @param v, 返回的数据
     *
     * @return int:
     *          RT_NO_DATA: 没有数据
     *          RT_ONLY_KEY:只有Key
     *          RT_OK:获取数据成功
     *          其他返回值: 错误
     */
    int get(const string &mk, const string &uk, Value &v);

    /**
     * @brief 获取主key下的所有数据, 修改GET时间链
     * @param mk, 主key
     * @param vs, 返回的数据集
     *
     * @return int:
     *          RT_NO_DATA: 没有数据
     *          RT_ONLY_KEY: 只有Key
     *          RT_PART_DATA: 数据不全，只有部分数据
     *          RT_OK: 获取数据成功
     *          其他返回值: 错误
     */
    int get(const string &mk, vector<Value> &vs);

    /**
     * @brief 获取主key hash下的所有数据 
     *        , 不修改GET时间链，主要用于迁移
     * @param mh, 主key hash值
     * @param vs, 返回的数据集，以主key进行分组的map
     *
     * @return int:
     *          RT_OK: 获取数据成功
     *          其他返回值: 错误
     */
    int get(uint32_t &mh, map<string, vector<Value> > &vs);

    /**
     * @brief 设置数据, 修改时间链, 内存不够时会自动淘汰老的数据
     * @param mk: 主key
     * @param uk: 除主key外的联合主键
     * @param v: 数据值
     * @param iVersion: 数据版本, 应该根据get出的数据版本写回，为0表示不关心数据版本
     * @param bDirty: 是否是脏数据
     * @param eType: set的数据类型，PART_DATA-不完事的数据，FULL_DATA-完整的数据，AUTO_DATA-根据已有数据类型决定
     * @param bHead: 插入到主key链的顺序，前序或后序
     * @param vtData: 被淘汰的记录
     * @return int:
     *          RT_READONLY: map只读
     *          RT_NO_MEMORY: 没有空间(不淘汰数据情况下会出现)
     *            RT_DATA_VER_MISMATCH, 要设置的数据版本与当前版本不符，应该重新get后再set
     *          RT_OK: 设置成功
     *          其他返回值: 错误
     */
    int set(const string &mk, const string &uk, const string &v, uint8_t iVersion, 
        bool bDirty, DATATYPE eType, bool bHead,vector<Value> &vtData);

    /**
     * @brief 设置key, 但无数据(only key), 
     *        内存不够时会自动淘汰老的数据
     * @param mk: 主key
     * @param uk: 除主key外的联合主键
     * @param eType: set的数据类型，PART_DATA-不完事的数据，FULL_DATA-完整的数据，AUTO_DATA-根据已有数据类型决定
     * @param bHead: 插入到主key链的顺序，前序或后序
     * @param vtData: 被淘汰的数据
     *
     * @return int
     *          RT_READONLY: map只读
     *          RT_NO_MEMORY: 没有空间(不淘汰数据情况下会出现)
     *          RT_OK: 设置成功
     *          其他返回值: 错误
     */
    int set(const string &mk, const string &uk, DATATYPE eType, bool bHead, vector<Value> &vtData);

    /**
     * @brief 仅设置主key, 无联合key及数据 
     *        , 内存不够时会自动淘汰老的数据
     * @param mk: 主key
     * @param vtData: 被淘汰的数据
     *
     * @return int
     *          RT_READONLY: map只读
     *          RT_NO_MEMORY: 没有空间(不淘汰数据情况下会出现)
     *          RT_OK: 设置成功
     *          其他返回值: 错误
     */
    int set(const string &mk, vector<Value> &vtData);

    /**
    * @brief 批量设置数据, 内存不够时会自动淘汰老的数据
    * @param vtSet: 需要设置的数据
    * @param eType: set的数据类型，PART_DATA-不完事的数据，FULL_DATA-完整的数据，AUTO_DATA-根据已有数据类型决定
    * @param bHead: 插入到主key链的顺序，前序或后序
    * @param bForce, 是否强制插入数据，为false则表示如果数据已经存在则不更新
    * @param vtErased: 内存不足时被淘汰的数据
    *
    * @return 
    *          RT_READONLY: map只读
    *          RT_NO_MEMORY: 没有空间(不淘汰数据情况下会出现)
    *          RT_OK: 设置成功
    *          其他返回值: 错误
    */
    int set(const vector<Value> &vtSet, DATATYPE eType, bool bHead, bool bForce, vector<Value> &vtErased);

    /**
     * @brief 
     *        删除数据，除非强制删除某条数据，否则应该调用下面的删除主key下所有数据的函数
     * @param mk: 主key
     * @param uk: 除主key外的联合主键
     * @param data: 被删除的记录
     * @return int:
     *          RT_READONLY: map只读
     *          RT_NO_DATA: 没有当前数据
     *          RT_ONLY_KEY:只有Key, 删除成功
     *          RT_OK: 删除数据成功
     *         其他返回值: 错误
     */
    int del(const string &mk, const string &uk, Value &data);

    /**
     * @brief 删除主key下所有数据
     * @param mk: 主key
     * @param data: 被删除的记录
     * @return int:
     *          RT_READONLY: map只读
     *          RT_NO_DATA: 没有当前数据
     *          RT_ONLY_KEY:只有Key, 删除成功
     *          RT_OK: 删除数据成功
     *         其他返回值: 错误
     */
    int del(const string &mk, vector<Value> &data);

    /**
     * @brief 淘汰数据, 每次删除一条, 根据Get时间淘汰
     * 外部循环调用该接口淘汰数据
     * 直到: 元素个数/chunks * 100 < Ratio, bCheckDirty 为true时，遇到脏数据则淘汰结束
     * @param ratio: 共享内存chunks使用比例 0< Ratio < 100
     * @param data: 被删除的数据集
     * @param bCheckDirty: 是否检查脏数据
     * @return int:
     *          RT_READONLY: map只读
     *          RT_OK: 不用再继续淘汰了
     *          RT_ONLY_KEY:只有Key, 删除成功
     *          RT_DIRTY_DATA:数据是脏数据，当bCheckDirty=true时会有可能产生这种返回值
     *          RT_ERASE_OK:淘汰当前数据成功, 继续淘汰
     *          其他返回值: 错误, 通常忽略, 继续调用erase淘汰
     */
    int erase(int ratio, vector<Value> &vtData, bool bCheckDirty = false);

    /**
     * @brief 回写, 每次返回需要回写的一条
     * 数据回写时间与当前时间超过_pHead->_iSyncTime则需要回写
     * _pHead->_iSyncTime由setSyncTime函数设定, 默认10分钟

     * 外部循环调用该函数进行回写
     * map只读时仍然可以回写
     * @param iNowTime: 当前时间
     *                  回写时间与当前时间相差_pHead->_iSyncTime都需要回写
     * @param data : 返回需要回写的数据
     * @return int:
     *          RT_OK: 到脏数据链表头部了, 可以sleep一下再尝试
     *          RT_ONLY_KEY:只有Key, 删除成功, 当前数据不要缓写,继续调用sync回写
     *          RT_NEED_SYNC:当前返回的data数据需要回写
     *          RT_NONEED_SYNC:当前返回的data数据不需要回写
     *          其他返回值: 错误, 通常忽略, 继续调用sync回写
     */
    int sync(time_t iNowTime, Value &data);

    /**
     * @brief 开始回写, 调整回写指针
     */
    void sync();

    /**
     * @brief 开始备份之前调用该函数
     *
     * @param bForceFromBegin: 是否强制从头开始备份
     * @return void
     */
    void backup(bool bForceFromBegin = false);

    /**
     * @brief 开始备份数据 
     *        , 每次返回需要备份的数据(一个主key下的所有数据)
     * @param data
     *
     * @return int
     *          RT_OK: 备份完毕
     *          RT_NEED_BACKUP:当前返回的data数据需要备份
     *          RT_ONLY_KEY:只有Key, 当前数据不要备份
     *          其他返回值: 错误, 通常忽略, 继续调用backup
     */
    int backup(vector<Value> &vtData);

    /////////////////////////////////////////////////////////////////////////////////////////
    // 以下是遍历map函数, 需要对map加锁

    /**
     * @brief 结束
     *
     * @return
     */
    lock_iterator end() { return _lock_end; }

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
     * @brief 获取脏链表尾部迭代器(最长时间没有操作的脏数据)
     *
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
     *@brief  结束
     *
     * @return
     */
    hash_iterator hashEnd() { return _end; }

    /**
     * @brief 根据Key查找数据
     * @param mk: 主key
     * @param uk: 除主key外的联合主键
     * @return lock_iterator
     */
    lock_iterator find(const string &mk, const string &uk);

    /**
    * @brief 获取主key链上block的数量
    * @param mk: 主key
    * @return size_t
    */
    size_t count(const string &mk);

    /**
    * @brief 根据主key查找第一个block位置. 
    *  
    * 与count配合可以遍历某个主key下的所有数据 也可以直接使用迭代器，直到end
    * @param mk: 主key
    * @return lock_iterator
    */
    lock_iterator find(const string &mk);

    /**
     * @brief 描述
     *
     * @return string
     */
    string desc();

    /**
    * @brief 对可能的坏block进行检查，并可进行修复
    * @param iHash, hash索引
    * @param bRepaire, 是否进行修复
    *
    * @return size_t, 返回坏数据个数
    */
    size_t checkBadBlock(uint32_t iHash, bool bRepair);

protected:

    friend class Block;
    friend class BlockAllocator;
    friend class HashMapIterator;
    friend class HashMapItem;
    friend class HashMapLockIterator;
    friend class HashMapLockItem;

    /**
    *  @brief 禁止copy构造
    */
    TC_Multi_HashMap(const TC_Multi_HashMap &mcm);
       /**
    *  @brief 禁止复制
    */
    TC_Multi_HashMap &operator=(const TC_Multi_HashMap &mcm);

    /**                      
    *  @brief 用于数据更新过程失败的自动恢复，
    *  在所有可能进行关键数据更新的函数的最开始构造
    */
    struct FailureRecover
    {
        FailureRecover(TC_Multi_HashMap *pMap) : _pMap(pMap)
        {
            // 构造时恢复可能损坏的数据
            _pMap->doRecover();
            assert(_iRefCount ++ == 0);
        }
        
        ~FailureRecover()
        {
            // 析构时清理已经成功更新的数据
            _pMap->doUpdate();
            assert(_iRefCount-- == 1);
        }
        
    protected:
        TC_Multi_HashMap   *_pMap;
        // 避免嵌套调用
        static int            _iRefCount;
    };


    /**
     * @brief 初始化 
     * @param pAddr, 外部分配好的存储地址
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
     * @brief 增加主键下OnlyKey数据个数
     */
    void incOnlyKeyCount()    { saveValue(&_pHead->_iOnlyKeyCount, _pHead->_iOnlyKeyCount+1); }

    /**
     * @brief 减少主键下OnlyKey数据个数
     */
    void delOnlyKeyCount()    { saveValue(&_pHead->_iOnlyKeyCount, _pHead->_iOnlyKeyCount-1); }

    /**
     * @brief 增加主key下OnlyKey数据个数
     */
    void incOnlyKeyCountM()    { saveValue(&_pHead->_iMKOnlyKeyCount, _pHead->_iMKOnlyKeyCount+1); }

    /**
     * @brief 减少主key下OnlyKey数据个数
     */
    void delOnlyKeyCountM()    { saveValue(&_pHead->_iMKOnlyKeyCount, _pHead->_iMKOnlyKeyCount-1); }

    /**
     * @brief 增加Chunk数
     */
    void incChunkCount()    { saveValue(&_pHead->_iUsedChunk, _pHead->_iUsedChunk + 1); }

    /**
     * @brief 减少Chunk数
     */
    void delChunkCount()    { saveValue(&_pHead->_iUsedChunk, _pHead->_iUsedChunk - 1); }

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
    * @brief 某hash值主key链上主key个数+1
    */
    void incMainKeyListCount(uint32_t index) { saveValue(&itemMainKey(index)->_iListCount, itemMainKey(index)->_iListCount+1); }

    /**
     * @brief 某hash链表数据个数-1
     * @param index
     */
    void delListCount(uint32_t index) { saveValue(&item(index)->_iListCount, item(index)->_iListCount-1); }

    /**
    * @brief 某hash值主key链上主key个数-1
    */
    void delMainKeyListCount(uint32_t index) { saveValue(&itemMainKey(index)->_iListCount, itemMainKey(index)->_iListCount-1); }

    /**
    * @brief 某hash值主key链上blockdata个数+/-1
    * @param mk, 主key
    * @param bInc, 是增加还是减少
    */
    void incMainKeyBlockCount(const string &mk, bool bInc = true);

    /**
    * @brief 更新主key下面最大记录数信息
    */
    void updateMaxMainKeyBlockCount(size_t iCount);

    /**
     * @brief 相对地址换成绝对地址
     * @param iAddr
     *
     * @return void*
     */
    void *getAbsolute(uint32_t iAddr) { return _pDataAllocator->_pChunkAllocator->getAbsolute(iAddr); }

    /**
     * @brief 绝对地址换成相对地址
     *
     * @return size_t
     */
    uint32_t getRelative(void *pAddr) { return (uint32_t)_pDataAllocator->_pChunkAllocator->getRelative(pAddr); }

    /**
     * @brief 淘汰iNowAddr之外的数据(根据淘汰策略淘汰)
     * @param iNowAddr, 当前主key正在分配内存，是不能被淘汰的
     *                  0表示做直接根据淘汰策略淘汰
     * @param vector<Value>, 被淘汰的数据
     * @return size_t, 淘汰的数据个数
     */
    size_t eraseExcept(uint32_t iNowAddr, vector<Value> &vtData);

    /**
     * @brief 根据Key计算hash值
     * @param mk: 主key
     * @param uk: 除主key外的联合主键
     *
     * @return uint32_t
     */
    uint32_t hashIndex(const string &mk, const string &uk);

    /**
     * @brief 根据Key计算hash值
     * @param k: key
     *
     * @return uint32_t
     */
    uint32_t hashIndex(const string &k);

    /**
    * @brief 根据主key计算主key的hash
    * @param mk: 主key
    * @return uint32_t
    */
    uint32_t mhashIndex(const string &mk); 

    /**
     * @brief 根据hash索引查找指定key(mk+uk)的数据的位置, 并返回数据
     * @param mk: 主key
     * @param uk: 除主key外的联合主键
     * @param index: 联合主键的hash索引
     * @param v: 如果存在数据，则返回数据值
     * @param ret: 具体的返回值
     * @return lock_iterator: 返回找到的数据的位置，不存在则返回end()
     *
     */
    lock_iterator find(const string &mk, const string &uk, uint32_t index, Value &v, int &ret);

    /**
     * @brief 根据hash索引查找指定key(mk+uk)的数据的位置
     * @param mk: 主key
     * @param uk: 除主key外的联合主键
     * @param index: 联合主键的hash索引
     * @param ret: 具体的返回值
     * @return lock_iterator: 返回找到的数据的位置，不存在则返回end()
     *
     */
    lock_iterator find(const string &mk, const string &uk, uint32_t index, int &ret);

    /**
    * @brief 根据主key hash索引查找主key的地址，找不到返回0
    * @param mk: 主key
    * @param index: 主key hash索引
    * @param ret: 具体返回值
    * @return uint32_t: 返回找到的主key的首地址，找不到返回0
    */
    uint32_t find(const string &mk, uint32_t index, int &ret);

    /**
     * @brief 分析主键hash的数据
     * @param iMaxHash: 最大的block hash桶上元素个数
     * @param iMinHash: 最小的block hash桶上元素个数
     * @param fAvgHash: 平均元素个数
     */
    void analyseHash(uint32_t &iMaxHash, uint32_t &iMinHash, float &fAvgHash);

    /**
     * @brief 分析主key hash的数据
     * @param iMaxHash: 最大的主key hash桶上元素个数
     * @param iMinHash: 最小的主key hash桶上元素个数
     * @param fAvgHash: 平均元素个数
     */
    void analyseHashM(uint32_t &iMaxHash, uint32_t &iMinHash, float &fAvgHash);

    /**
    * @brief 修改具体的值
    * @param pModifyAddr
    * @param iModifyValue
    */
    template<typename T>
    void saveValue(void* pModifyAddr, T iModifyValue)
    {
        //获取原始值
        T tmp = *(T*)pModifyAddr;
        
        //保存原始值
        _pstModifyHead->_stModifyData[_pstModifyHead->_iNowIndex]._iModifyAddr  = (char*)pModifyAddr - (char*)_pHead;
        _pstModifyHead->_stModifyData[_pstModifyHead->_iNowIndex]._iModifyValue = tmp;
        _pstModifyHead->_stModifyData[_pstModifyHead->_iNowIndex]._cBytes       = sizeof(iModifyValue);
        _pstModifyHead->_iNowIndex++;
        
        _pstModifyHead->_cModifyStatus = 1;

        //修改具体值
        *(T*)pModifyAddr = iModifyValue;
        
        assert(_pstModifyHead->_iNowIndex < sizeof(_pstModifyHead->_stModifyData) / sizeof(tagModifyData));
    }

    /**
    * @brief 对某个值的某位进行更新
    * @param pModifyAddr, 待修改的(整型数)内存地址
    * @param bit, 需要修改的整型数的位
    * @param b, 需要修改的整型数位的值
    */
    template<typename T>
    void saveValue(T *pModifyAddr, uint8_t bit, bool b)
    {
        T tmp = *pModifyAddr;    // 取出原值
        if(b)
        {
            tmp |= 0x01 << bit;
        }
        else
        {
            tmp &= T(-1) ^ (0x01 << bit);
        }
        saveValue(pModifyAddr, tmp);
    }
    
    /**
    * @brief 恢复数据
    */
    void doRecover();
    
    /**
    * @brief 确认处理完毕
    */
    void doUpdate();

    /**
     * @brief 获取大于n且离n最近的素数
     * @param n
     *
     * @return size_t
     */
    size_t getMinPrimeNumber(size_t n);

protected:

    /**
     * 头部指针
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
     * 数据块增长因子
     */
    float                       _fFactor;

    /**
     * 设置chunk数据块/hash项比值
     */
    float                       _fHashRatio;

    /**
    * 主key hash个数/联合hash个数
    */
    float                        _fMainKeyRatio;

    /**
     * 联合主键hash索引区
     */
    TC_MemVector<tagHashItem>   _hash;

    /**
    * 主key hash索引区
    */
    TC_MemVector<tagMainKeyHashItem>    _hashMainKey;

    /**
     * 修改数据块
     */
    tagModifyHead               *_pstModifyHead;

    /**
     * block分配器对象，包括为数据区和主key区分配内存
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
     * 联合主键hash值计算公式
     */
    hash_functor                _hashf;

    /**
    * 主key的hash计算函数, 如果不提供，将使用上面的_hashf
    */
    hash_functor                _mhashf;
};

}

#endif

