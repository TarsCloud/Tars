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

#ifndef __TC_RBTREE_H
#define __TC_RBTREE_H

#include <iostream>
#include <string>
#include <cassert>
#include <functional>
#include "util/tc_ex.h"
#include "util/tc_pack.h"
#include "util/tc_mem_chunk.h"

using namespace std;

namespace tars
{
/////////////////////////////////////////////////
/** 
* @file tc_rbtree.h 
* @brief rbtree map类
* 
*/           
/////////////////////////////////////////////////
/**
* @brief RBTree map异常类
*/
struct TC_RBTree_Exception : public TC_Exception
{
    TC_RBTree_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_RBTree_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_RBTree_Exception() throw(){};
};
 /**
 * @brief 内存rbtree，不要直接使用该类，通过jmem组件来使用 
 *  
 *  该红黑树通过TC_MemMutilChunkAllocator来分配空间，支持不同大小的内存块的分配,
 *  
 *  分配器分配的是内存索引，减少自身消耗的空间（尤其是64位OS下面）；
 *  
 * 支持内存和共享内存,对接口的所有操作都需要加锁； 
 *  
 *  内部有脏数据链，支持数据缓写；
 *  
 *  当数据超过一个数据块时，则会拼接多个数据块；
 *  
 *  Set时当数据块用完，自动淘汰最长时间没有访问的数据，也可以不淘汰，直接返回错误；
 *  
 *  支持dump到文件，或从文件load；
 */
class TC_RBTree
{
public:
    struct RBTreeLockIterator;
    struct RBTreeIterator;

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
            uint16_t    _iSize;         /**block的容量大小*/
            char        _iColor;        /**颜色*/
            uint32_t    _iParent;       /**父节点*/
            uint32_t    _iLeftChild;    /**左子树*/
            uint32_t    _iRightChild;   /**右子树*/
            uint32_t    _iSetNext;      /**Set链上的上一个Block*/
            uint32_t    _iSetPrev;      /**Set链上的上一个Block*/
            uint32_t    _iGetNext;      /**Get链上的上一个Block*/
            uint32_t    _iGetPrev;      /**Get链上的上一个Block*/
            time_t      _iSyncTime;     /**上次缓写时间*/
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
        Block(TC_RBTree *pMap, uint32_t iAddr)
        : _pMap(pMap)
        , _iHead(iAddr)
        {
            _pHead = getBlockHead(_iHead);
        }

        /**
         * @brief copy
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
        tagBlockHead *getBlockHead()    { return _pHead; }

        /**
         * @brief 是否有左子节点
         * 
         * @return bool
         */
        bool hasLeft();

        /**
         * 当前元素移动到左子节点
         * @return true, 移到下一个block了, false, 没有下一个block
         *
         */
        bool moveToLeft();

        /**
         * @brief 是否有右子节点
         * 
         * @return bool
         */
        bool hasRight();

        /**
         * @brief 当前元素移动右子节点
         * @return true, 移到下一个block了, false, 没有下一个block
         *
         */
        bool moveToRight();

        /**
         * @brief 是否有父节点
         * 
         * @return bool
         */
        bool hasParent();

        /**
         * @brief 当前元素移动父
         * @return true, 移到下一个block了, false, 没有下一个block
         *
         */
        bool moveToParent();

        /**
         * @brief 头部
         *
         * @return uint32_t
         */
        uint32_t getHead()  { return _iHead;}

        /**
         * 最新Get时间
         *
         * @return time_t
         */
        time_t getSyncTime() { return getBlockHead()->_iSyncTime; }

        /**
         * 设置回写时间
         * @param iSyncTime
         */
        void setSyncTime(time_t iSyncTime) { getBlockHead()->_iSyncTime = iSyncTime; }

        /**
         * 获取Block中的数据
         *
         * @return int
         *          TC_RBTree::RT_OK, 正常, 其他异常
         *          TC_RBTree::RT_ONLY_KEY, 只有Key
         *          其他异常
         */
        int getBlockData(TC_RBTree::BlockData &data);

        /**
         * 获取数据
         * @param pData
         * @param iDatalen
         * @return int,
         *          TC_RBTree::RT_OK, 正常
         *          其他异常
         */
        int get(void *pData, uint32_t &iDataLen);

        /**
         * 获取数据
         * @param s
         * @return int
         *          TC_RBTree::RT_OK, 正常
         *          其他异常
         */
        int get(string &s);

        /**
         * 设置数据
         * @param pData
         * @param iDatalen
         * @param vtData, 淘汰的数据
         */
        int set(const string& k, const string& v, bool bNewBlock, bool bOnlyKey, vector<TC_RBTree::BlockData> &vtData);

        /**
         * 是否是脏数据
         *
         * @return bool
         */
        bool isDirty()      { return getBlockHead()->_bDirty; }

        /**
         * 设置数据
         * @param b
         */
        void setDirty(bool b);

        /**
         * 是否只有key
         *
         * @return bool
         */
        bool isOnlyKey()    { return getBlockHead()->_bOnlyKey; }

        /**
         * 释放block的所有空间
         */
        void deallocate();

        /**
         * 新block时调用该函数
         * 分配一个新的block
         * @param iAllocSize, 内存大小
         */
        void makeNew(uint32_t iAllocSize);

        /**
         * 插入新节点
         * 
         * @param i 
         * @param k 
         */
        void insertRBTree(tagBlockHead *i, const string &k);

        /**
         * 从Block链表中删除当前Block
         * 只对Block有效, 对Chunk是无效的
         * @return
         */
        void erase();

        /**
         * 刷新set链表, 放在Set链表头部
         */
        void refreshSetList();

        /**
         * 刷新get链表, 放在Get链表头部
         */
        void refreshGetList();

    protected:

        Block& operator=(const Block &mb);
        bool operator==(const Block &mb) const;
        bool operator!=(const Block &mb) const;

        /**
         * 获取Chunk头绝对地址
         *
         * @return tagChunkHead*
         */
        tagChunkHead *getChunkHead()                {return getChunkHead(_iHead);}

        /**
         * 获取chunk头绝对地址
         * @param iAddr
         *
         * @return tagChunkHead*
         */
        tagChunkHead *getChunkHead(uint32_t iAddr)  { return ((tagChunkHead*)_pMap->getAbsolute(iAddr)); }

        /**
         * 从当前的chunk开始释放
         * @param iChunk 释放地址
         */
        void deallocate(uint32_t iChunk);

        /**
         * 如果数据容量不够, 则新增加chunk, 不影响原有数据
         * 使新增加的总容量大于iDataLen
         * 释放多余的chunk
         * @param iDataLen
         *
         * @return int,
         */
        int allocate(uint32_t iDataLen, vector<TC_RBTree::BlockData> &vtData);

        /**
         * 挂接chunk, 如果core则挂接失败, 保证内存块还可以用
         * @param pChunk
         * @param chunks
         *
         * @return int
         */
        int joinChunk(tagChunkHead *pChunk, const vector<uint32_t> chunks);

        /**
         * 分配n个chunk地址, 注意释放内存的时候不能释放正在分配的对象
         * @param fn, 分配空间大小
         * @param chunks, 分配成功返回的chunks地址列表
         * @param vtData, 淘汰的数据
         * @return int
         */
        int allocateChunk(uint32_t fn, vector<uint32_t> &chunks, vector<TC_RBTree::BlockData> &vtData);

        /**
         * 获取数据长度
         *
         * @return uint32_t
         */
        uint32_t getDataLen();

        /**
         * 左旋
         * 
         * @param i
         */
        void rotateLeft(tagBlockHead *i, uint32_t iAddr);

        /**
         * 右旋
         * 
         * @param i
         */
        void rotateRight(tagBlockHead *i, uint32_t iAddr);

        /**
         * 删除后调整
         * 
         * @param i 
         */
        void eraseFixUp(tagBlockHead *i, uint32_t iAddr, tagBlockHead *p, uint32_t iPAddr);

        /**
         * 删除
         * 
         * @param i 
         */
        void erase(tagBlockHead *i, uint32_t iAddr);

        /**
         * 插入后调整
         * 
         * @param i 
         */
        void insertFixUp(tagBlockHead *i, uint32_t iAddr);

        /**
         * 插入到get/set链表中
         */
        void insertGetSetList(TC_RBTree::Block::tagBlockHead *i);

    private:

        /**
         * Map
         */
        TC_RBTree         *_pMap;

        /**
         * block区块首地址, 相对地址
         */
        uint32_t          _iHead;

        /**
         * 头区块指针
         */
        tagBlockHead *    _pHead;
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
         * 构造函数
         */
        BlockAllocator(TC_RBTree *pMap)
        : _pMap(pMap)
        , _pChunkAllocator(new TC_MemMultiChunkAllocator())
        {
        }

        /**
         * 析够函数
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
         * 初始化
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
         * 连接上
         * @param pAddr, 地址, 换到应用程序的绝对地址
         */
        void connect(void *pHeadAddr)
        {
            _pChunkAllocator->connect(pHeadAddr);
        }

        /**
         * 扩展空间
         * @param pAddr
         * @param iSize
         */
        void append(void *pAddr, size_t iSize)
        {
            _pChunkAllocator->append(pAddr, iSize);
        }

        /**
         * 重建
         */
        void rebuild()
        {
            _pChunkAllocator->rebuild();
        }

        /**
         * 获取每种数据块头部信息
         *
         * @return TC_MemChunk::tagChunkHead
         */
        vector<TC_MemChunk::tagChunkHead> getBlockDetail() const  { return _pChunkAllocator->getBlockDetail(); }

        /**
         * 内存大小
         *
         * @return size_t
         */
        size_t getMemSize() const       { return _pChunkAllocator->getMemSize(); }

        /**
         * 真正的数据容量
         *
         * @return size_t
         */
        size_t getCapacity() const      { return _pChunkAllocator->getCapacity(); }

        /**
         * 每种block中的chunk个数(每种block中的chunk个数相同)
         *
         * @return vector<size_t>
         */
        vector<size_t> singleBlockChunkCount() const { return _pChunkAllocator->singleBlockChunkCount(); }

        /**
         * 所有block的chunk个数
         *
         * @return size_t
         */
        size_t allBlockChunkCount() const    { return _pChunkAllocator->allBlockChunkCount(); }

        /**
         * 在内存中分配一个新的Block
         *
         * @param iAllocSize: in/需要分配的大小, out/分配的块大小
         * @param vtData, 返回释放的内存块数据
         * @return uint32_t, 相对地址,0表示没有空间可以分配
         */
        uint32_t allocateMemBlock(uint32_t &iAllocSize, vector<TC_RBTree::BlockData> &vtData);

        /**
         * 为地址为iAddr的Block分配一个chunk
         *
         * @param iAddr,分配的Block的地址
         * @param iAllocSize, in/需要分配的大小, out/分配的块大小
         * @param vtData 返回释放的内存块数据
         * @return uint32_t, 相对地址,0表示没有空间可以分配
         */
        uint32_t allocateChunk(uint32_t iAddr, uint32_t &iAllocSize, vector<TC_RBTree::BlockData> &vtData);

        /**
         * 释放Block
         * @param v
         */
        void deallocateMemBlock(const vector<uint32_t> &v);

        /**
         * 释放Block
         * @param v
         */
        void deallocateMemBlock(uint32_t v);

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
        TC_RBTree                  *_pMap;

        /**
         * chunk分配器
         */
        TC_MemMultiChunkAllocator   *_pChunkAllocator;
    };

    ////////////////////////////////////////////////////////////////
    // map的数据项
    class RBTreeLockItem
    {
    public:

        /**
         *
         * @param pMap
         * @param iAddr
         */
        RBTreeLockItem(TC_RBTree *pMap, uint32_t iAddr);

        /**
         *
         * @param mcmdi
         */
        RBTreeLockItem(const RBTreeLockItem &mcmdi);

        /**
         * 
         */
        RBTreeLockItem(){};

        /**
         *
         * @param mcmdi
         *
         * @return RBTreeLockItem&
         */
        RBTreeLockItem &operator=(const RBTreeLockItem &mcmdi);

        /**
         *
         * @param mcmdi
         *
         * @return bool
         */
        bool operator==(const RBTreeLockItem &mcmdi);

        /**
         *
         * @param mcmdi
         *
         * @return bool
         */
        bool operator!=(const RBTreeLockItem &mcmdi);

        /**
         * 是否是脏数据
         *
         * @return bool
         */
        bool isDirty();

        /**
         * 是否只有Key
         *
         * @return bool
         */
        bool isOnlyKey();

        /**
         * 最后Sync时间
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
         * 获取值
         * @return int
         *          RT_OK:数据获取OK
         *          其他值, 异常
         */
        int get(string& k);

        /**
         * 数据块相对地址
         *
         * @return uint32_t
         */
        uint32_t getAddr() const { return _iAddr; }

    protected:

        /**
         * 设置数据
         * @param k
         * @param v
         * @param vtData, 淘汰的数据
         * @return int
         */
        int set(const string& k, const string& v, bool bNewBlock, vector<TC_RBTree::BlockData> &vtData);

        /**
         * 设置Key, 无数据
         * @param k
         * @param vtData
         *
         * @return int
         */
        int set(const string& k, bool bNewBlock, vector<TC_RBTree::BlockData> &vtData);

        /**
         *
         * @param pKey
         * @param iKeyLen
         *
         * @return bool
         */
        bool equal(const string &k, string &k1, string &v, int &ret);

        /**
         *
         * @param pKey
         * @param iKeyLen
         *
         * @return bool
         */
        bool equal(const string& k, string &k1, int &ret);

        /**
         * 下一个item
         *
         * @return RBTreeLockItem
         */
        void nextItem(int iType);

        /**
         * 上一个item
         * @param iType
         */
        void prevItem(int iType);

        friend class TC_RBTree;
        friend struct TC_RBTree::RBTreeLockIterator;

    private:
        /**
         * map
         */
        TC_RBTree *_pMap;

        /**
         * block的地址
         */
        uint32_t  _iAddr;
    };

    /////////////////////////////////////////////////////////////////////////
    // 定义迭代器
    struct RBTreeLockIterator
    {
    public:

        //定义遍历方式
        enum
        {
            IT_RBTREE       = 1,        //rbtree大小遍历
            IT_SET          = 2,        //Set时间顺序
            IT_GET          = 3,        //Get时间顺序
        };

        //遍历顺序
        enum
        {
            IT_NEXT = 0,
            IT_PREV = 1,
        };

        /**
         *
         */
        RBTreeLockIterator();

        /**
         * 构造函数
         * @param iAddr, 地址
         * @param type
         */
        RBTreeLockIterator(TC_RBTree *pMap, uint32_t iAddr, int iType, int iOrder);

        /**
         * copy
         * @param it
         */
        RBTreeLockIterator(const RBTreeLockIterator &it);

        /**
         * 复制
         * @param it
         *
         * @return RBTreeLockIterator&
         */
        RBTreeLockIterator& operator=(const RBTreeLockIterator &it);

        /**
         *
         * @param mcmi
         *
         * @return bool
         */
        bool operator==(const RBTreeLockIterator& mcmi);

        /**
         *
         * @param mv
         *
         * @return bool
         */
        bool operator!=(const RBTreeLockIterator& mcmi);

        /**
         * 前置++
         *
         * @return RBTreeLockIterator&
         */
        RBTreeLockIterator& operator++();

        /**
         * 后置++
         *
         * @return RBTreeLockIterator&
         */
        RBTreeLockIterator operator++(int);

        /**
         *
         *
         * @return RBTreeLockItem&i
         */
        RBTreeLockItem& operator*() { return _iItem; }

        /**
         *
         *
         * @return RBTreeLockItem*
         */
        RBTreeLockItem* operator->() { return &_iItem; }

        /**
         * 设置顺序
         * @param iOrder
         */
        void setOrder(int iOrder) { _iOrder = iOrder; }

    public:
        /**
         *
         */
        TC_RBTree  *_pMap;

        /**
         *
         */
        RBTreeLockItem _iItem;

        /**
         * 迭代器的方式
         */
        int         _iType;

        /**
         * 顺序
         */
        int         _iOrder;
    };

    ////////////////////////////////////////////////////////////////
    // map的数据项
    class RBTreeItem
    {
    public:

        /**
         *
         * @param pMap
         * @param key
         */
        RBTreeItem(TC_RBTree *pMap, const string &key, bool bEnd);

        /**
         *
         * @param mcmdi
         */
        RBTreeItem(const RBTreeItem &mcmdi);

        /**
         * 
         */
        RBTreeItem() : _bEnd(true)
        {
        }

        /**
         *
         * @param mcmdi
         *
         * @return RBTreeLockItem&
         */
        RBTreeItem &operator=(const RBTreeItem &mcmdi);

        /**
         *
         * @param mcmdi
         *
         * @return bool
         */
        bool operator==(const RBTreeItem &mcmdi);

        /**
         *
         * @param mcmdi
         *
         * @return bool
         */
        bool operator!=(const RBTreeItem &mcmdi);

        /**
         * 获取当前数据
         * 
         * @return RT_OK, 获取成功
         *         RT_ONLY_KEY, 是onlykey
         *         RT_NO_DATA, 无数据
         *         RT_EXCEPTION_ERR, 异常
         */
        int get(TC_RBTree::BlockData &stData);

    protected:

        /**
         * 获取key
         * 
         * @return string
         */
        string getKey() const { return _key; }

        /**
         * 空数据
         * 
         * @return bool
         */
        bool isEnd() const { return _bEnd; }

        /**
         * 下一个item
         *
         * @return 
         */
        void nextItem();

        /**
         * 上一个item
         */
        void prevItem();

        friend class TC_RBTree;
        friend struct TC_RBTree::RBTreeIterator;

    private:
        /**
         * map
         */
        TC_RBTree *_pMap;

        /**
         * block的地址
         */
        string    _key;

        /**
         * 是否是结尾
         */
        bool      _bEnd;
    };

    /////////////////////////////////////////////////////////////////////////
    // 定义迭代器
    struct RBTreeIterator
    {
    public:

        //遍历顺序
        enum
        {
            IT_NEXT = 0,
            IT_PREV = 1,
        };

        /**
         *
         */
        RBTreeIterator();

        /**
         * 构造函数
         * @param iAddr, 地址
         * @param type
         */
        RBTreeIterator(TC_RBTree *pMap, const string &key, bool bEnd, int iOrder);

        /**
         * copy
         * @param it
         */
        RBTreeIterator(const RBTreeIterator &it);

        /**
         * 复制
         * @param it
         *
         * @return RBTreeIterator&
         */
        RBTreeIterator& operator=(const RBTreeIterator &it);

        /**
         *
         * @param mcmi
         *
         * @return bool
         */
        bool operator==(const RBTreeIterator& mcmi);

        /**
         *
         * @param mv
         *
         * @return bool
         */
        bool operator!=(const RBTreeIterator& mcmi);

        /**
         * 前置++
         *
         * @return RBTreeIterator&
         */
        RBTreeIterator& operator++();

        /**
         * 后置++
         *
         * @return RBTreeIterator&
         */
        RBTreeIterator operator++(int);

        /**
         *
         *
         * @return RBTreeItem&i
         */
        RBTreeItem& operator*() { return _iItem; }

        /**
         *
         *
         * @return RBTreeItem*
         */
        RBTreeItem* operator->() { return &_iItem; }

    public:
        /**
         *
         */
        TC_RBTree   *_pMap;

        /**
         *
         */
        RBTreeItem  _iItem;

        /**
         * 顺序
         */
        int         _iOrder;
    };

public:
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // map定义
    //
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
        uint32_t    _iElementCount;      //总元素个数
        uint32_t    _iEraseCount;        //每次删除个数
        uint32_t    _iDirtyCount;        //脏数据个数
        uint32_t    _iSetHead;           //Set时间链表头部
        uint32_t    _iSetTail;           //Set时间链表尾部
        uint32_t    _iGetHead;           //Get时间链表头部
        uint32_t    _iGetTail;           //Get时间链表尾部
        uint32_t    _iDirtyTail;         //脏数据链尾部
        time_t      _iSyncTime;          //回写时间
        uint32_t    _iUsedChunk;         //已经使用的内存块
        uint32_t    _iGetCount;          //get次数
        uint32_t    _iHitCount;          //命中次数
        uint32_t    _iBackupTail;        //热备指针
        uint32_t    _iSyncTail;          //回写链表
        uint32_t    _iOnlyKeyCount;        // OnlyKey个数
        uint32_t    _iRootAddr;          //根元素地址
        uint32_t    _iReserve[4];       //保留
    }__attribute__((packed));

    /**
     * 需要修改的地址
     */
    struct tagModifyData
    {
        size_t  _iModifyAddr;       //修改的地址
        char    _cBytes;            //字节数
        size_t  _iModifyValue;      //值
    }__attribute__((packed));

    /**
     * 修改数据块头部
     */
    struct tagModifyHead
    {
        char            _cModifyStatus;         //修改状态: 0:目前没有人修改, 1: 开始准备修改, 2:修改完毕, 没有copy到内存中
        uint32_t        _iNowIndex;             //更新到目前的索引, 不能操作1000个
        tagModifyData   _stModifyData[1000];    //一次最多1000次修改
    }__attribute__((packed));

    //64位操作系统用基数小版本号, 32位操作系统用偶数小版本
    //注意与hashmap的版本差别
#if __WORDSIZE == 64

    //定义版本号
    enum
    {
        MAX_VERSION         = 2,    //当前map的大版本号
        MIN_VERSION         = 1,    //当前map的小版本号
    };

#else
    //定义版本号
    enum
    {
        MAX_VERSION         = 2,    //当前map的大版本号
        MIN_VERSION         = 0,    //当前map的小版本号
    };

#endif

    //定义淘汰方式
    enum
    {
        ERASEBYGET          = 0x00, //按照Get链表淘汰
        ERASEBYSET          = 0x01, //按照Set链表淘汰
    };

    const static char RED       = 0;    //红色节点
    const static char BLACK     = 1;    //黑色节点

    /**
     * get, set等int返回值
     */
    enum
    {
        RT_OK                   = 0,    //成功
        RT_DIRTY_DATA           = 1,    //脏数据
        RT_NO_DATA              = 2,    //没有数据
        RT_NEED_SYNC            = 3,    //需要回写
        RT_NONEED_SYNC          = 4,    //不需要回写
        RT_ERASE_OK             = 5,    //淘汰数据成功
        RT_READONLY             = 6,    //map只读
        RT_NO_MEMORY            = 7,    //内存不够
        RT_ONLY_KEY             = 8,    //只有Key, 没有Value
        RT_NEED_BACKUP          = 9,    //需要备份
        RT_NO_GET               = 10,   //没有GET过
        RT_DECODE_ERR           = -1,   //解析错误
        RT_EXCEPTION_ERR        = -2,   //异常
        RT_LOAD_DATA_ERR        = -3,   //加载数据异常
        RT_VERSION_MISMATCH_ERR = -4,   //版本不一致
        RT_DUMP_FILE_ERR        = -5,   //dump到文件失败
        RT_LOAL_FILE_ERR        = -6,   //load文件到内存失败
        RT_NOTALL_ERR           = -7,   //没有复制完全
    };

    //定义迭代器
    typedef RBTreeLockIterator lock_iterator;
    typedef RBTreeIterator nolock_iterator;

    //定义key比较处理器
    using less_functor = std::function<bool (const string& , const string& )>;

    /**
     * 缺省的小写比较符号
     */
    struct default_less
    {
        bool operator()(const string &k1, const string &k2)
        {
            return k1 < k2;
        }
    };

    //////////////////////////////////////////////////////////////////////////////////////////////
    //map的接口定义
    /**
     * 构造函数
     */
    TC_RBTree()
    : _iMinDataSize(0)
    , _iMaxDataSize(0)
    , _fFactor(1.0)
    , _lock_end(this, 0, 0, 0)
    , _nolock_end(this, "", true, 0)
    , _pDataAllocator(new BlockAllocator(this))
    , _lessf(default_less())
    {
    }

    /**
     * 初始化数据块平均大小
     * 表示内存分配的时候，会分配n个最小块， n个（最小快*增长因子）, n个（最小快*增长因子*增长因子）..., 直到n个最大块
     * n是hashmap自己计算出来的
     * 这种分配策略通常是数据块记录变长比较多的使用， 便于节约内存，如果数据记录基本不是变长的， 那最小块=最大快，增长因子=1就可以了
     * @param iMinDataSize: 最小数据块大小
     * @param iMaxDataSize: 最大数据块大小
     * @param fFactor: 增长因子
     */
    void initDataBlockSize(uint32_t iMinDataSize, uint32_t iMaxDataSize, float fFactor);

    /**
     * 初始化, 之前需要调用:initDataAvgSize和initHashRadio
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
     * 原来的数据块基础上扩展内存, 注意通常只能对mmap文件生效
     * (如果iSize比本来的内存就小,则返回-1)
     * @param pAddr, 扩展后的空间
     * @param iSize
     * @return 0:成功, -1:失败
     */
    int append(void *pAddr, size_t iSize);

    /**
     * 获取每种大小内存块的头部信息
     *
     * @return vector<TC_MemChunk::tagChunkHead>: 不同大小内存块头部信息
     */
    vector<TC_MemChunk::tagChunkHead> getBlockDetail() { return _pDataAllocator->getBlockDetail(); }

    /**
     * 所有block中chunk的个数
     *
     * @return size_t
     */
    size_t allBlockChunkCount()                     { return _pDataAllocator->allBlockChunkCount(); }

    /**
     * 每种block中chunk的个数(不同大小内存块的个数相同)
     *
     * @return vector<size_t>
     */
    vector<size_t> singleBlockChunkCount()          { return _pDataAllocator->singleBlockChunkCount(); }

    /**
     * 元素的个数
     *
     * @return size_t
     */
    uint32_t size()                                   { return _pHead->_iElementCount; }

    /**
     * 脏数据元素个数
     *
     * @return size_t
     */
    uint32_t dirtyCount()                             { return _pHead->_iDirtyCount;}

    /**
     * OnlyKey数据元素个数
     *
     * @return uint32_t
     */
    uint32_t onlyKeyCount()                             { return _pHead->_iOnlyKeyCount;}

    /**
     * 设置每次淘汰数量
     * @param n
     */
    void setEraseCount(uint32_t n)                    { _pHead->_iEraseCount = n; }

    /**
     * 获取每次淘汰数量
     *
     * @return uint32_t
     */
    uint32_t getEraseCount()                          { return _pHead->_iEraseCount; }

    /**
     * 设置只读
     * @param bReadOnly
     */
    void setReadOnly(bool bReadOnly)                { _pHead->_bReadOnly = bReadOnly; }

    /**
     * 是否只读
     *
     * @return bool
     */
    bool isReadOnly()                               { return _pHead->_bReadOnly; }

    /**
     * 设置是否可以自动淘汰
     * @param bAutoErase
     */
    void setAutoErase(bool bAutoErase)              { _pHead->_bAutoErase = bAutoErase; }

    /**
     * 是否可以自动淘汰
     *
     * @return bool
     */
    bool isAutoErase()                              { return _pHead->_bAutoErase; }

    /**
     * 设置淘汰方式
     * TC_RBTree::ERASEBYGET
     * TC_RBTree::ERASEBYSET
     * @param cEraseMode
     */
    void setEraseMode(char cEraseMode)              { _pHead->_cEraseMode = cEraseMode; }

    /**
     * 获取淘汰方式
     *
     * @return bool
     */
    char getEraseMode()                             { return _pHead->_cEraseMode; }

    /**
     * 设置回写时间(秒)
     * @param iSyncTime
     */
    void setSyncTime(time_t iSyncTime)              { _pHead->_iSyncTime = iSyncTime; }

    /**
     * 获取回写时间
     *
     * @return time_t
     */
    time_t getSyncTime()                            { return _pHead->_iSyncTime; }

    /**
     * 获取头部数据信息
     * 
     * @return tagMapHead&
     */
    tagMapHead& getMapHead()                        { return *_pHead; }

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
     * 从文件load
     * @param sFile
     *
     * @return int
     *          RT_LOAL_FILE_ERR: load出错
     *          RT_VERSION_MISMATCH_ERR: 版本不一致
     *          RT_OK: load成功
     */
    int load5file(const string &sFile);

    /**
     * 清空hashmap
     * 所有map的数据恢复到初始状态
     */
    void clear();

    /**
     * 检查数据干净状态
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
     * 设置为脏数据, 修改SET时间链, 会导致数据回写
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
     * 设置为干净数据, 修改SET链, 导致数据不回写
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
     * 获取数据, 修改GET时间链
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
     * 获取数据, 修改GET时间链
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
     * 设置数据, 修改时间链, 内存不够时会自动淘汰老的数据
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
     * 设置key, 但无数据
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
     * 删除数据
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
     * 淘汰数据, 每次删除一条, 根据Get时间淘汰
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
     * 回写, 每次返回需要回写的一条
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
     * 开始回写, 调整回写指针
     */
    void sync();

    /**
     * 开始备份之前调用该函数
     *
     * @param bForceFromBegin: 是否强制重头开始备份
     * @return void
     */
    void backup(bool bForceFromBegin = false);

    /**
     * 开始备份数据, 每次返回需要备份的一条数据
     * @param data
     *
     * @return int
     *          RT_OK: 备份完毕
     *          RT_NEED_BACKUP:当前返回的data数据需要备份
     *          RT_ONLY_KEY:只有Key, 当前数据不要备份
     *          其他返回值: 错误, 通常忽略, 继续调用backup
     */
    int backup(BlockData &data);

    /**
     * 设置比较方式
     * @param lessf
     */
    void setLessFunctor(less_functor lessf)         { _lessf = lessf; }

    /**
     * 获取比较方式
     * 
     * @return less_functor& 
     */
    less_functor &getLessFunctor()                  { return _lessf; }

    /////////////////////////////////////////////////////////////////////////////////////////
    // 以下是遍历map函数, 无需要对map加大面积锁, 但是遍历效率有一定影响
    // (只在get以及迭代器++的时候加锁)
    // 获取的迭代器和数据不保证实时有效,可能已经被删除了,获取数据时需要判断数据的合法性
    
    /**
     * 结束
     *
     * @return
     */
    nolock_iterator nolock_end() { return _nolock_end; }

    /**
     * 按从小到大排序
     * 
     * @return nolock_iterator
     */
    nolock_iterator nolock_begin();

    /**
     * 按从大到小排序
     * 
     * @return nolock_iterator
     */
    nolock_iterator nolock_rbegin();

    /////////////////////////////////////////////////////////////////////////////////////////
    // 以下是遍历map函数, 需要对map加大面积锁(及迭代器存在有效范围内全部都加锁)
    // 获取的数据以及迭代器都是实时有效

    /**
     * 结束
     *
     * @return
     */
    lock_iterator end() { return _lock_end; }

    /**
     * 按从小到大排序(sort顺序)
     * 
     * @return lock_iterator 
     */
    lock_iterator begin();

    /**
     * 按从大到小序(sort逆序)
     * 
     * @return lock_iterator 
     */
    lock_iterator rbegin();

    /**
     * 查找(++表示顺序)
     * 
     * @param k 
     * 
     * @return lock_iterator 
     */
    lock_iterator find(const string& k);

    /**
     * 查找(++表示逆序)
     * 
     * @param k 
     * 
     * @return lock_iterator 
     */
    lock_iterator rfind(const string& k);

    /**
     * 返回查找关键字的下界(返回键值>=给定元素的第一个位置)
     * map中已经插入了1,2,3,4的话，如果lower_bound(2)的话，返回的2，而upper-bound(2)的话，返回的就是3
     * @param k
     * 
     * @return lock_iterator
     */
    lock_iterator lower_bound(const string &k);

    /**
     * 返回查找关键字的上界(返回键值>给定元素的第一个位置)
     * map中已经插入了1,2,3,4的话，如果lower_bound(2)的话，返回的2，而upper-bound(2)的话，返回的就是3
     * @param k
     * 
     * @return lock_iterator
     */
    lock_iterator upper_bound(const string &k);

    /**
     * 查找
     * @param k1
     * @param k2
     * 
     * @return pair<lock_iterator,lock_iterator>
     */
    pair<lock_iterator, lock_iterator> equal_range(const string& k1, const string &k2);

    //////////////////////////////////////////////////////////////////////////////////////
    // 
    /**
     * 以Set时间排序的迭代器
     *
     * @return lock_iterator
     */
    lock_iterator beginSetTime();

    /**
     * Set链逆序的迭代器
     *
     * @return lock_iterator
     */
    lock_iterator rbeginSetTime();

    /**
     * 以Get时间排序的迭代器
     *
     * @return lock_iterator
     */
    lock_iterator beginGetTime();

    /**
     * Get链逆序的迭代器
     *
     * @return lock_iterator
     */
    lock_iterator rbeginGetTime();

    /**
     * 获取脏链表尾部迭代器(最长时间没有操作的脏数据)
     *
     * 返回的迭代器++表示按照时间顺序==>(最短时间没有操作的脏数据)
     *
     * @return lock_iterator
     */
    lock_iterator beginDirty();

    ///////////////////////////////////////////////////////////////////////////
    /**
     * 描述
     *
     * @return string
     */
    string desc();

protected:

    friend class Block;
    friend class BlockAllocator;
    friend class RBTreeLockIterator;
    friend class RBTreeLockItem;

    //禁止copy构造
    TC_RBTree(const TC_RBTree &mcm);
    //禁止复制
    TC_RBTree &operator=(const TC_RBTree &mcm);

    struct FailureRecover
    {
        FailureRecover(TC_RBTree *pMap) : _pMap(pMap)
        {
            _pMap->doRecover();
        }

        ~FailureRecover()
        {
            _pMap->doUpdate();
        }
       
    protected:
        TC_RBTree   *_pMap;
    };

    /**
     * 初始化
     * @param pAddr
     */
    void init(void *pAddr);

    /**
     * 增加脏数据个数
     */
    void incDirtyCount()    { saveValue(&_pHead->_iDirtyCount, _pHead->_iDirtyCount+1); }

    /**
     * 减少脏数据个数
     */
    void delDirtyCount()    { saveValue(&_pHead->_iDirtyCount, _pHead->_iDirtyCount-1); }

    /**
     * 增加数据个数
     */
    void incElementCount()  { saveValue(&_pHead->_iElementCount, _pHead->_iElementCount+1); }

    /**
     * 减少数据个数
     */
    void delElementCount()  { saveValue(&_pHead->_iElementCount, _pHead->_iElementCount-1); }

    /**
     * 增加OnlyKey数据个数
     */
    void incOnlyKeyCount()    { saveValue(&_pHead->_iOnlyKeyCount, _pHead->_iOnlyKeyCount+1); }

    /**
     * 减少OnlyKey数据个数
     */
    void delOnlyKeyCount()    { saveValue(&_pHead->_iOnlyKeyCount, _pHead->_iOnlyKeyCount-1); }

    /**
     * 增加Chunk数
     * 直接更新, 因为有可能一次分配的chunk个数
     * 多余更新区块的内存空间, 导致越界错误
     */
    void incChunkCount()    { saveValue(&_pHead->_iUsedChunk, _pHead->_iUsedChunk+1); }

    /**
     * 减少Chunk数
     * 直接更新, 因为有可能一次释放的chunk个数
     * 多余更新区块的内存空间, 导致越界错误
     */
    void delChunkCount()    { saveValue(&_pHead->_iUsedChunk, _pHead->_iUsedChunk-1); }

    /**
     * 增加hit次数
     */
    void incGetCount()      { saveValue(&_pHead->_iGetCount, _pHead->_iGetCount+1); }

    /**
     * 增加命中次数
     */
    void incHitCount()      { saveValue(&_pHead->_iHitCount, _pHead->_iHitCount+1); }

    /**
     * 相对地址换成绝对地址
     * @param iAddr
     *
     * @return void*
     */
    void *getAbsolute(uint32_t iAddr) { if(iAddr == 0) return NULL; return _pDataAllocator->_pChunkAllocator->getAbsolute(iAddr); }

    /**
     * 淘汰iNowAddr之外的数据(根据淘汰策略淘汰)
     * @param iNowAddr, 当前Block不能正在分配空间, 不能被淘汰
     *                  0表示做直接根据淘汰策略淘汰
     * @param vector<BlockData>, 被淘汰的数据
     * @return uint32_t,淘汰的数据个数
     */
    uint32_t eraseExcept(uint32_t iNowAddr, vector<BlockData> &vtData);

    /**
     * 根据key获得最后一个大于当前key的block
     * @param k
     * 
     * @return size_t
     */
    Block getLastBlock(const string &k);

    /**
     * 从某个地址开始超找
     * 
     * @param iAddr 
     * @param k 
     * @param ret 
     * 
     * @return lock_iterator 
     */
    lock_iterator find(uint32_t iAddr, const string& k, int &ret, bool bOrder = true);

    /**
     * 从某个地址开始查找
     * 
     * @param iAddr 
     * @param k 
     * @param v 
     * @param ret 
     * 
     * @return lock_iterator 
     */
    lock_iterator find(uint32_t iAddr, const string& k, string &v, int &ret);

    /**
     * 修改具体的值
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
        _pstModifyHead->_iNowIndex++;

        _pstModifyHead->_cModifyStatus = 1;

        if(bModify)
        {
            //修改具体值
            *(T*)iModifyAddr = iModifyValue;
        }

        assert(_pstModifyHead->_iNowIndex < sizeof(_pstModifyHead->_stModifyData) / sizeof(tagModifyData));
    }

    /**
     * 恢复数据
     */
    void doRecover();

    /**
     * 确认处理完毕
     */
    void doUpdate();

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
     * 尾部
     */
    lock_iterator               _lock_end;

    /**
     * 尾部
     */
    nolock_iterator             _nolock_end;

    /**
     * 修改数据块
     */
    tagModifyHead               *_pstModifyHead;

    /**
     * block分配器对象
     */
    BlockAllocator              *_pDataAllocator;

    /**
     * 比较公式
     */
    less_functor                _lessf;

};

}

#endif
