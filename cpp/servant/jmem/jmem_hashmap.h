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

#ifndef _JMEM_HASHMAP_H
#define _JMEM_HASHMAP_H

#include "util/tc_hashmap.h"
#include "util/tc_autoptr.h"
#include "jmem/jmem_policy.h"
#include "tup/Tars.h"

namespace tars
{

/************************************************************************
 基本说明如下:
 基于Tars协议的内存hashmap
 编解码出错则抛出TarsDecodeException和TarsEncodeException
 可以对锁策略和存储策略进行组合, 例如:
 基于信号量锁, 文件存储的hashmap:
 TarsHashMap<Test::QueueElement, SemLockPolicy, FileStorePolicy>
 基于信号量锁, 共享内存存储的hashmap
 TarsHashMap<Test::QueueElement, SemLockPolicy, ShmStorePolicy>
 基于线程锁, 内存存储的hashmap
 TarsHashMap<Test::QueueElement, ThreadLockPolicy, MemStorePolicy>
 
 使用上, 不同的组合, 初始化函数不完全一样
 初始化函数有:
 SemLockPolicy::initLock(key_t)
 ShmStorePolicy::initStore(key_t, size_t)
 FileStorePolicy::initStore(const char *file, size_t)
 等, 具体参见jmem_policy.h
 
 ***********************************************************************

 基本特性说明:
 > 内存数据的map, 根据最后Get时间的顺序淘汰数据;
 > 支持缓写/dump到文件/在线备份;
 > 支持不同大小内存块的配置, 提供内存的使用率;
 > 支持回收到指定空闲比率的空间;
 > 支持仅设置Key的操作, 即数据无value, 只有Key, 类似与stl::set;
 > 支持自定义hash算法;
 > hash数可以根据内存块比率设置, 并优化有素数, 提高hash的散列性;
 > 支持几种方式的遍历, 通常遍历时需要对map加锁; 
 > 对于hash方式的遍历, 遍历时可以不需要对map加锁, 推荐使用;
 > 支持自定义操作对象设置, 可以非常快速实现相关的接口;
 > 支持自动编解码, Key和Value的结构都通过tars2cpp生成;
 > tars协议支持自动扩展字段, 因此该hashmap支持自动扩展字段(Key和Value都必须是通过tars编码的);
 > map支持只读模式, 只读模式下set/erase/del等修改数据的操作不能使用, get/回写/在线备份正常使用
 > 支持自动淘汰, set时, 内存满则自动淘汰, 在非自动淘汰时, 内存满直接返回RT_READONLY
 > 对于mmap文件, 支持自动扩展文件, 即内存不够了, 可以自动扩展文件大小(注意hash的数量不变, 因此开始就需要考虑hash的数量), 而且不能跨JHashmap对象（即两个hashmap对象访问同一块文件，通知一个hashmap扩展以后，另外一个对象并不知道扩展了）

 ***********************************************************************

 hashmap链说明:
 hashmap链一共包括了如下几种链表:
 > Set时间链: 任何Set操作都会修改该链表, Set后数据被设置为脏数据, 且移动到Set链表头部;
 > Get时间链: 任何Get操作都会修改该链表, 除非链表只读, 注意Set链同时也会修改Get链
 > Dirty时间链: dirty链是Set链的一部分, 用于回写数据用
 > Backup链:备份链是Get链的一部分, 当备份数据时, 顺着Get链从尾部到头部开始备份;
 
 ***********************************************************************

 相关操作说明:
 > 可以设置map只读, 则所有写操作返回RT_READONLY, 此时Get操作不修改链表
 > 可以设置知否自动淘汰, 默认是自动淘汰的.如果不自动淘汰,则set时,无内存空间返回:RT_NO_MEMORY
 > 可以更改hash的算法, 调用setHashFunctor即可
 > 可以将某条数据设置为干净, 此时移出到Dirty链表指Dirty尾部的下一个元素;
 > 可以将某条数据设置为脏, 此时会移动到Set链表头部;
 > 每个数据都有一个上次回写时间(SyncTime), 如果启动回写操作, 则在一定时间内会回写;
 > 可以dump到文件或者从文件中load, 这个时候会对map加锁
 > 可以调用erase批量淘汰数据直到内存空闲率到一定比例
 > 可以调用sync进行数据回写, 保证一定时间内没有回写的数据会回写, map回写时间通过setSyncTime设置, 默认10分钟
 > 可以setToDoFunctor设置操作类, 以下是操作触发的情况:
  
 ***********************************************************************
 
 ToDoFunctor的函数说明:
 > 通常继承ToDoFunctor, 实现相关函数就可以了, 可以实现以下功能:Get数据, 淘汰数据, 删除数据, 回写数据, 备份数据
 > ToDoFunctor::erase, 当调用map.erase时, 该函数会被调用
 > ToDoFunctor::del, 当调用map.del时, 该函数会被调用, 注意删除时数据可能都不在cache中;
 > ToDoFunctor::sync, 当调用map.sync时, 会触发每条需要回写的数据该函数都被调用一次, 在该函数中处理回写请求;
 > ToDoFunctor::backup, 当调用map.backup时, 会触发需要备份的数据该函数会被调用一次, 在该函数中处理备份请求;
 > ToDoFunctor::get, 当调用map.get时, 如果map中无数据, 则该函数被调用, 该函数从db中获取数据, 并返回RT_OK, 如果db无数据则返回RT_NO_DATA;
 > ToDoFunctor所有接口被调用时, 都不会对map加锁, 因此可以操作map

 ***********************************************************************

 map的重要函数说明:
 > set, 设置数据到map中, 会更新set链表
        如果满了, 且可以自动淘汰, 则根据Get链淘汰数据, 此时ToDoFunctor的sync会被调用
        如果满了, 且可以不能自动淘汰, 则返回RT_NO_MEMORY

 > get, 从map获取数据, 如果有数据, 则直接从map获取数据并返回RT_OK;
        如果没有数据, 则调用ToDoFunctor::get函数, 此时get函数需要返回RT_OK, 同时会设置到map中, 并返回数据;
        如果没有数据, 则ToDoFunctor::get函数也无数据, 则需要返回RT_NO_DATA, 此时只会把Key设置到map中, 并返回RT_ONLY_KEY;
        在上面情况下, 如果再有get请求, 则不再调用ToDoFunctor::get, 直接返回RT_ONLY_KEY;

 > del, 删除数据, 无论cache是否有数据, ToDoFunctor::del都会被调用;
        如果只有Key, 则该数据也会被删除;

 > erase, 淘汰数据, 只有cache存在数据, ToDoFunctor::erase才会被调用
          如果只有Key, 则该数据也会被淘汰, 但是ToDoFunctor::erase不会被调用;

 > erase(int radio), 批量淘汰数据, 直到空闲块比率到达radio;
                     ToDoFunctor::erase会被调用;
                     只有Key的记录也会被淘汰, 但是ToDoFunctor::erase不会被调用;

 > sync: 缓写数据, 超时没有回写且是脏数据需要回写, 回写完毕后, 数据会自动设置为干净数据;
         可以多个线程或进程同时缓写;
         ToDoFunctor::sync会被调用;
         只有Key的记录, ToDoFunctor::sync不会被调用;

 > backup: 备份数据, 顺着顺着Get链从尾部到头部开始备份;
           ToDoFunctor::backup会被调用;
           只有Key的记录, ToDoFunctor::backup不会被调用;
           由于备份游标只有一个, 因此多个进程同时备份的时候数据可能会每个进程有一部分
           如果备份程序备份到一半down了, 则下次启动备份时会接着上次的备份进行, 除非将backup(true)调用备份

 ***********************************************************************

 返回值说明: 
 > 注意函数所有int的返回值, 如无特别说明, 请参见TC_HashMap::RT_
 
 ***********************************************************************

 遍历说明:
 > 可以用lock_iterator对map进行以下几种遍历, 在遍历过程中其实对map加锁处理了
 > end(): 迭代器尾部
 > begin(): 按照block区块遍历
 > rbegin():按照block区块逆序遍历
 > beginSetTime(): 按照Set时间顺序遍历
 > rbeginSetTime(): 按照Set时间顺序遍历
 > beginGetTime(): 按照Get时间顺序遍历
 > rbeginGetTime(): 按照Get时间逆序遍历
 > beginDirty(): 按时间逆序遍历脏数据链(如果setClean, 则也可能在脏链表上)
 > 其实回写数据链是脏数据量的子集
 > 注意:lock_iterator一旦获取, 就对map加锁了, 直到lock_iterator析够为止
 >
 > 可以用hash_iterator对map进行遍历, 遍历过程中对map没有加锁, 推荐使用
 > hashBegin(): 获取hash遍历迭代器
 > hashEnd(): hash遍历尾部迭代器
 > 注意:hash_iterator对应的其实是一个hash桶链, 每次获取数据其实会获取桶链上面的所有数据
*/

template<typename K,
         typename V,
         typename LockPolicy,
         template<class, class> class StorePolicy>
class TarsHashMap : public StorePolicy<TC_HashMap, LockPolicy>
{
public:
    /**
    * 定义数据操作基类
    * 获取,遍历,删除,淘汰时都可以使用该操作类
    */
    class ToDoFunctor
    {
    public:
        /**
         * 数据记录
         */
        struct DataRecord
        {
            K       _key;
            V       _value;
            bool    _dirty;
            time_t  _iSyncTime;

            DataRecord() : _dirty(true), _iSyncTime(0)
            {
            }
        };

        /**
         * 析够
         */
        virtual ~ToDoFunctor(){};

        /**
         * 淘汰数据
         * @param stDataRecord: 被淘汰的数据
         */
        virtual void erase(const DataRecord &stDataRecord){};

        /**
         * 删除数据
         * @param bExists: 是否存在数据
         * @param stDataRecord: 数据, bExists==true时有效, 否则只有key有效
         */
        virtual void del(bool bExists, const DataRecord &stDataRecord){};

        /**
         * 回写数据
         * @param stDataRecord: 数据
         */
        virtual void sync(const DataRecord &stDataRecord){};

        /**
         * 备份数据
         * @param stDataRecord: 数据
         */
        virtual void backup(const DataRecord &stDataRecord){};

        /**
         * 获取数据, 默认返回RT_NO_GET
         * stDataRecord中_key有效, 其他数据需要返回
         * @param stDataRecord: 需要获取的数据
         *
         * @return int, 获取到数据, 返回:TC_HashMap::RT_OK
         *              没有数据,返回:TC_HashMap::RT_NO_DATA,
         *              系统默认GET,返回:TC_HashMap::RT_NO_GET
         *              其他,则返回:TC_HashMap::RT_LOAD_DATA_ERR
         */
        virtual int get(DataRecord &stDataRecord)
        {
            return TC_HashMap::RT_NO_GET;
        }
    };

    typedef typename ToDoFunctor::DataRecord DataRecord;

    ///////////////////////////////////////////////////////////////////
    /**
     * 自动锁, 用于迭代器
     */
    class JhmAutoLock : public TC_HandleBase
    {
    public:
        /**
         * 构造
         * @param mutex
         */
        JhmAutoLock(typename LockPolicy::Mutex &mutex) : _lock(mutex)
        {
        }

    protected:
        //不实现
        JhmAutoLock(const JhmAutoLock &al);
        JhmAutoLock &operator=(const JhmAutoLock &al);

    protected:
        /**
         * 锁
         */
        TC_LockT<typename LockPolicy::Mutex>   _lock;
    };

    typedef TC_AutoPtr<JhmAutoLock> JhmAutoLockPtr;

    ///////////////////////////////////////////////////////////////////
    /**
     * 数据项
     */
    class JhmLockItem
    {
    public:

        /**
         * 构造函数
         * @param item
         */
        JhmLockItem(const TC_HashMap::HashMapLockItem &item)
        : _item(item)
        {
        }

        /**
         * 拷贝构造
         * @param it
         */
        JhmLockItem(const JhmLockItem &item)
        : _item(item._item)
        {
        }

        /**
         * 复制
         * @param it
         *
         * @return JhmLockItem&
         */
        JhmLockItem& operator=(const JhmLockItem &item)
        {
            if(this != &item)
            {
                _item     = item._item;
            }

            return (*this);
        }

        /**
         *
         * @param item
         *
         * @return bool
         */
        bool operator==(const JhmLockItem& item)
        {
            return (_item == item._item);
        }

        /**
         *
         * @param item
         *
         * @return bool
         */
        bool operator!=(const JhmLockItem& item)
        {
            return !((*this) == item);
        }

        /**
         * 是否是脏数据
         *
         * @return bool
         */
        bool isDirty()          { return _item.isDirty(); }

        /**
         * 是否只有Key
         *
         * @return bool
         */
        bool isOnlyKey()        { return _item.isOnlyKey(); }

        /**
         * 最后回写时间
         *
         * @return time_t
         */
        time_t getSyncTime()    { return _item.getSyncTime(); }

        /**
         * 获取值
         * @return int
         *          TC_HashMap::RT_OK:数据获取OK
         *          其他值, 异常
         */
        int get(K& k)
        {
            string sk;
            int ret = _item.get(sk);
            if(ret != TC_HashMap::RT_OK)
            {
                return ret;
            }

            tars::TarsInputStream<BufferReader> is;
            is.setBuffer(sk.c_str(), sk.length());
            k.readFrom(is);

            return ret;
        }

        /**
         * 获取值
         * @return int
         *          TC_HashMap::RT_OK:数据获取OK
         *          TC_HashMap::RT_ONLY_KEY: key有效, v无效为空
         *          其他值, 异常
         */
        int get(K& k, V& v)
        {
            string sk;
            string sv;
            int ret = _item.get(sk, sv);
            if(ret != TC_HashMap::RT_OK && ret != TC_HashMap::RT_ONLY_KEY)
            {
                return ret;
            }

            tars::TarsInputStream<BufferReader> is;
            is.setBuffer(sk.c_str(), sk.length());
            k.readFrom(is);

            if(ret != TC_HashMap::RT_ONLY_KEY)
            {
                is.setBuffer(sv.c_str(), sv.length());
                v.readFrom(is);
            }

            return ret;
        }

    protected:
        TC_HashMap::HashMapLockItem _item;
    };

    ///////////////////////////////////////////////////////////////////
    /**
     * 迭代器
     */
    struct JhmLockIterator
    {
    public:

        /**
         * 构造
         * @param it
         * @param lock
         */
        JhmLockIterator(const TC_HashMap::lock_iterator it, const JhmAutoLockPtr &lock)
        : _it(it), _item(it._iItem), _lock(lock)
        {
        }

        /**
         * 拷贝构造
         * @param it
         */
        JhmLockIterator(const JhmLockIterator &it)
        : _it(it._it), _item(it._item), _lock(it._lock)
        {
        }

        /**
         * 复制
         * @param it
         *
         * @return JhmLockIterator&
         */
        JhmLockIterator& operator=(const JhmLockIterator &it)
        {
            if(this != &it)
            {
                _it     = it._it;
                _item   = it._item;
                _lock   = it._lock;
            }

            return (*this);
        }

        /**
         *
         * @param it
         *
         * @return bool
         */
        bool operator==(const JhmLockIterator& it)
        {
            return (_it == it._it && _item == it._item);
        }

        /**
         *
         * @param mv
         *
         * @return bool
         */
        bool operator!=(const JhmLockIterator& it)
        {
            return !((*this) == it);
        }

        /**
         * 前置++
         *
         * @return JhmLockIterator&
         */
        JhmLockIterator& operator++()
        {
            ++_it;
            _item = JhmLockItem(_it._iItem);
            return (*this);
        }

        /**
         * 后置++
         *
         * @return JhmLockIterator&
         */
        JhmLockIterator operator++(int)
        {
            JhmLockIterator jit(_it, _lock);
            ++_it;
            _item = JhmLockItem(_it._iItem);
            return jit;
        }

        /**
         * 获取数据项
         *
         * @return JhmLockItem&
         */
        JhmLockItem& operator*()     { return _item; }

        /**
         * 获取数据项
         *
         * @return JhmLockItem*
         */
        JhmLockItem* operator->()   { return &_item; }

    protected:

        /**
         * 迭代器
         */
        TC_HashMap::lock_iterator  _it;

        /**
         * 数据项
         */
        JhmLockItem                     _item;

        /**
         * 锁
         */
        JhmAutoLockPtr              _lock;
    };

    typedef JhmLockIterator lock_iterator ;

    ///////////////////////////////////////////////////////////////////
    /**
     * 锁, 用于非锁迭代器
     * 
     */
    class JhmLock : public TC_HandleBase
    {
    public:
        /**
         * 构造
         * @param mutex
         */
        JhmLock(typename LockPolicy::Mutex &mutex) : _mutex(mutex)
        {
        }

        /**
         * 获取锁
         * 
         * @return typename LockPolicy::Mutex
         */
        typename LockPolicy::Mutex& mutex()
        {
            return _mutex;
        }
    protected:
        //不实现
        JhmLock(const JhmLock &al);
        JhmLock &operator=(const JhmLock &al);

    protected:
        /**
         * 锁
         */
        typename LockPolicy::Mutex &_mutex;
    };

    typedef TC_AutoPtr<JhmLock> JhmLockPtr;

    ///////////////////////////////////////////////////////////////////
    /**
     * 数据项
     */
    class JhmItem
    {
    public:

        /**
         * 构造函数
         * @param item
         */
        JhmItem(const TC_HashMap::HashMapItem &item, const JhmLockPtr &lock)
        : _item(item), _lock(lock)
        {
        }

        /**
         * 拷贝构造
         * @param it
         */
        JhmItem(const JhmItem &item)
        : _item(item._item), _lock(item._lock)
        {
        }

        /**
         * 复制
         * @param it
         *
         * @return JhmItem&
         */
        JhmItem& operator=(const JhmItem &item)
        {
            if(this != &item)
            {
                _item     = item._item;
                _lock     = item._lock;
            }

            return (*this);
        }

        /**
         *
         * @param item
         *
         * @return bool
         */
        bool operator==(const JhmItem& item)
        {
            return (_item == item._item);
        }

        /**
         *
         * @param item
         *
         * @return bool
         */
        bool operator!=(const JhmItem& item)
        {
            return !((*this) == item);
        }

        /**
         * 获取当前hash桶的所有数量, 注意只获取有key/value的数据
         * 对于只有key的数据, 不获取
         * 如果协议解码有问题也不获取
         * @param
         */
        void get(vector<pair<K, V> > &v)
        {
            vector<TC_HashMap::BlockData> vtData;

            {
                TC_LockT<typename LockPolicy::Mutex> lock(_lock->mutex());
                _item.get(vtData);
            }

            for(size_t i = 0; i < vtData.size(); i++)
            {
                pair<K, V> pk;

                try
                {
                    tars::TarsInputStream<BufferReader> is;
                    is.setBuffer(vtData[i]._key.c_str(), vtData[i]._key.length());
                    pk.first.readFrom(is);

                    is.setBuffer(vtData[i]._value.c_str(), vtData[i]._value.length());
                    pk.second.readFrom(is);

                    v.push_back(pk);
                }
                catch(exception &ex)
                {
                }
            }
        }

        /**
         * 设置当前桶下的所有数据为脏数据
         * 
         * @param
         * 
         * @return int
         */
        int setDirty()
        {
            {
                TC_LockT<typename LockPolicy::Mutex> lock(_lock->mutex());
                return _item.setDirty();
            }
        }

    protected:
        TC_HashMap::HashMapItem _item;
        JhmLockPtr              _lock;
    };


    ///////////////////////////////////////////////////////////////////
    /**
     * 迭代器
     */
    struct JhmIterator
    {
    public:

        /**
         * 构造
         * @param it
         * @param lock
         */
        JhmIterator(const TC_HashMap::hash_iterator &it, const JhmLockPtr &lock)
        : _it(it), _item(it._iItem, lock), _lock(lock)
        {
        }

        /**
         * 拷贝构造
         * @param it
         */
        JhmIterator(const JhmIterator &it)
        : _it(it._it), _item(it._item), _lock(it._lock)
        {
        }

        /**
         * 复制
         * @param it
         *
         * @return JhmIterator&
         */
        JhmIterator& operator=(const JhmIterator &it)
        {
            if(this != &it)
            {
                _it     = it._it;
                _item   = it._item;
            }

            return (*this);
        }

        /**
         *
         * @param it
         *
         * @return bool
         */
        bool operator==(const JhmIterator& it)
        {
            return (_it == it._it && _item == it._item);
        }

        /**
         *
         * @param mv
         *
         * @return bool
         */
        bool operator!=(const JhmIterator& it)
        {
            return !((*this) == it);
        }

        /**
         * 前置++
         *
         * @return JhmIterator&
         */
        JhmIterator& operator++()
        {
            TC_LockT<typename LockPolicy::Mutex> lock(_lock->mutex());
            ++_it;
            _item = JhmItem(_it._iItem, _lock);
            return (*this);
        }

        /**
         * 后置++
         *
         * @return JhmIterator&
         */
        JhmIterator operator++(int)
        {
            TC_LockT<typename LockPolicy::Mutex> lock(_lock->mutex());
            JhmIterator jit(_it, _lock);
            ++_it;
            _item = JhmItem(_it._iItem, _lock);
            return jit;
        }

        /**
         * 获取数据项
         *
         * @return JhmItem&
         */
        JhmItem& operator*()     { return _item; }

        /**
         * 获取数据项
         *
         * @return JhmItem*
         */
        JhmItem* operator->()   { return &_item; }

    protected:

        /**
         * 迭代器
         */
        TC_HashMap::hash_iterator  _it;

        /**
         * 数据项
         */
        JhmItem               _item;

        /**
         * 锁
         */
        JhmLockPtr            _lock;
    };

    typedef JhmIterator hash_iterator ;

    ////////////////////////////////////////////////////////////////////////////
    //
    /**
     * 构造函数
     */
    TarsHashMap()
    {
        _todo_of = NULL;
    }

    /**
     * 初始化数据块平均大小
     * 表示内存分配的时候，会分配n个最小块， n个（最小快*增长因子）, n个（最小快*增长因子*增长因子）..., 直到n个最大块
     * n是hashmap自己计算出来的
     * 这种分配策略通常是你数据快记录变长比较多的使用， 便于节约内存，如果数据记录基本不是变长的， 那最小块=最大快，增长因子=1就可以了
     * @param iMinDataSize: 最小数据块大小
     * @param iMaxDataSize: 最大数据块大小
     * @param fFactor: 增长因子 >= 1.0
     */
    void initDataBlockSize(size_t iMinDataSize, size_t iMaxDataSize, float fFactor)
    {
        this->_t.initDataBlockSize(iMinDataSize, iMaxDataSize, fFactor);
    }

    /**
     * 设置hash比率(设置chunk数据块/hash项比值, 默认是2)
     * 有需要更改必须在create之前调用
     *
     * @param fRadio
     */
    void initHashRadio(float fRadio)                       { this->_t.initHashRadio(fRadio);}
    
    /**
     * 设置hash方式
     * @param hash_of
     */
    void setHashFunctor(TC_HashMap::hash_functor hashf)     
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.setHashFunctor(hashf); 
    }

    /**
     * 获取hash方式
     * 
     * @return TC_HashMap::hash_functor&
     */
    TC_HashMap::hash_functor &getHashFunctor()              { return this->_t.getHashFunctor(); }

    /**
     * 设置淘汰操作类
     * @param erase_of
     */
    void setToDoFunctor(ToDoFunctor *todo_of)               { this->_todo_of = todo_of; }

    /**
     * 获取每种大小内存块的头部信息
     *
     * @return vector<TC_MemChunk::tagChunkHead>: 不同大小内存块头部信息
     */
    vector<TC_MemChunk::tagChunkHead> getBlockDetail()      
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.getBlockDetail(); 
    }

    /**
     * 所有block中chunk的个数
     *
     * @return size_t
     */
    size_t allBlockChunkCount()                             
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.allBlockChunkCount(); 
    }

    /**
     * 每种block中chunk的个数(不同大小内存块的个数相同)
     *
     * @return size_t
     */
    vector<size_t> singleBlockChunkCount()                  
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.singleBlockChunkCount(); 
    }

    /**
     * 获取hash桶的个数
     *
     * @return size_t
     */
    size_t getHashCount()                                   
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.getHashCount(); 
    }

    /**
     * 元素的个数
     *
     * @return size_t
     */
    size_t size()                                           
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.size(); 
    }

    /**
     * 脏数据元素个数
     *
     * @return size_t
     */
    size_t dirtyCount()                                     
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.dirtyCount();
    }

    /**
     * Only数据元素个数
     *
     * @return size_t
     */
    size_t onlyKeyCount()                                     
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.onlyKeyCount();
    }

    /**
     * 设置每次淘汰数量
     * @param n
     */
    void setEraseCount(size_t n)                            
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.setEraseCount(n); 
    }

    /**
     * 获取每次淘汰数量
     *
     * @return size_t
     */
    size_t getEraseCount()                                  
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.getEraseCount(); 
    }

    /**
     * 设置只读
     * @param bReadOnly
     */
    void setReadOnly(bool bReadOnly)                        
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.setReadOnly(bReadOnly); 
    }

    /**
     * 是否只读
     *
     * @return bool
     */
    bool isReadOnly()                                       
    {   
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.isReadOnly(); 
    }

    /**
     * 设置是否可以自动淘汰
     * @param bAutoErase
     */
    void setAutoErase(bool bAutoErase)                      
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.setAutoErase(bAutoErase); 
    }

    /**
     * 是否可以自动淘汰
     *
     * @return bool
     */
    bool isAutoErase()                                      
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.isAutoErase(); 
    }

    /**
     * 设置淘汰方式
     * TC_HashMap::ERASEBYGET
     * TC_HashMap::ERASEBYSET
     * @param cEraseMode
     */
    void setEraseMode(char cEraseMode)                      
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.setEraseMode(cEraseMode); 
    }

    /**
     * 获取淘汰方式
     *
     * @return bool
     */
    char getEraseMode()                                     
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.getEraseMode(); 
    }

    /**
     * 头部信息
     * 
     * @return TC_HashMap::tagMapHead
     */
    TC_HashMap::tagMapHead& getMapHead()                    { return this->_t.getMapHead(); }

    /**
     * 设置回写时间(秒)
     * @param iSyncTime
     */
    void setSyncTime(time_t iSyncTime)                      
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.setSyncTime(iSyncTime); 
    }

    /**
     * 获取回写时间
     *
     * @return time_t
     */
    time_t getSyncTime()                                    
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.getSyncTime(); 
    }

    /**
     * dump到文件
     * @param sFile
     * @param bDoClear: 是否清空
     * @return int
     *          TC_HashMap::RT_DUMP_FILE_ERR: dump到文件出错
     *          TC_HashMap::RT_OK: dump到文件成功
     */
    int dump2file(const string &sFile, bool bDoClear = false)
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        int ret = this->_t.dump2file(sFile);
        if(ret != TC_HashMap::RT_OK)
        {
            return ret;
        }

        if(bDoClear)
               this->_t.clear();

        return ret;
    }

    /**
     * 从文件load
     * @param sFile
     *
     * @return int
     *          TC_HashMap::RT_LOAL_FILE_ERR: load出错
     *          TC_HashMap::RT_VERSION_MISMATCH_ERR: 版本不一致
     *          TC_HashMap::RT_OK: load成功
     */
    int load5file(const string &sFile)
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.load5file(sFile);
    }

    /**
     * 清空hash map
     * 所有map中的数据都被清空
     */
    void clear()
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.clear();
    }

    /**
     * 检查数据状态
     * @param k
     *
     * @return int
     *          TC_HashMap::RT_NO_DATA: 没有当前数据
     *          TC_HashMap::RT_ONLY_KEY:只有Key
     *          TC_HashMap::RT_DIRTY_DATA: 是脏数据
     *          TC_HashMap::RT_OK: 是干净数据
     *          其他返回值: 错误
     */
    int checkDirty(const K &k)
    {
        tars::TarsOutputStream<BufferWriter> osk;
        k.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.checkDirty(sk);
    }

    /**
     * 设置为干净数据i, 修改SET/GET时间链, 会导致数据不回写
     * @param k
     *
     * @return int
     *          TC_HashMap::RT_READONLY: 只读
     *          TC_HashMap::RT_NO_DATA: 没有当前数据
     *          TC_HashMap::RT_ONLY_KEY:只有Key
     *          TC_HashMap::RT_OK: 设置成功
     *          其他返回值: 错误
     */
    int setClean(const K& k)
    {
        tars::TarsOutputStream<BufferWriter> osk;
        k.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.setClean(sk);
    }

    /**
     * 设置为脏数据, 修改SET/GET时间链, 会导致数据回写
     * @param k
     * @return int
     *          TC_HashMap::RT_READONLY: 只读
     *          TC_HashMap::RT_NO_DATA: 没有当前数据
     *          TC_HashMap::RT_ONLY_KEY:只有Key
     *          TC_HashMap::RT_OK: 设置脏数据成功
     *          其他返回值: 错误
     */
    int setDirty(const K& k)
    {
        tars::TarsOutputStream<BufferWriter> osk;
        k.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.setDirty(sk);
    }

    /**
     * 获取数据, 修改GET时间链
     * (如果没设置自定义Get函数,没有数据时返回:RT_NO_DATA)
     * @param k
     * @param v
     * @param iSyncTime:数据上次回写的时间, 没有缓写则为0
     *
     * @return int:
     *          TC_HashMap::RT_NO_DATA: 没有数据
     *          TC_HashMap::RT_READONLY: 只读模式
     *          TC_HashMap::RT_ONLY_KEY:只有Key
     *          TC_HashMap::RT_OK:获取数据成功
     *          TC_HashMap::RT_LOAD_DATA_ERR: load数据失败
     *          其他返回值: 错误
     */
    int get(const K& k, V &v, time_t &iSyncTime)
    {
        iSyncTime   = 0;
        int ret = TC_HashMap::RT_OK;

        tars::TarsOutputStream<BufferWriter> osk;
        k.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());
        string sv;

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.get(sk, sv, iSyncTime);
        }

        //读取到数据了, 解包
        if(ret == TC_HashMap::RT_OK)
        {
            tars::TarsInputStream<BufferReader> is;
            is.setBuffer(sv.c_str(), sv.length());
            v.readFrom(is);

            return ret;
        }

        if(ret != TC_HashMap::RT_NO_DATA || _todo_of == NULL)
        {
            return ret;
        }

        //只读模式
        if(isReadOnly())
        {
            return TC_HashMap::RT_READONLY;
        }

        //获取函数
        typename ToDoFunctor::DataRecord stDataRecord;
        stDataRecord._key   = k;
        ret = _todo_of->get(stDataRecord);
        if(ret == TC_HashMap::RT_OK)
        {
            v = stDataRecord._value;
            return this->set(stDataRecord._key, stDataRecord._value, stDataRecord._dirty);
        }
        else if(ret == TC_HashMap::RT_NO_GET)
        {
            return TC_HashMap::RT_NO_DATA;
        }
        else if(ret == TC_HashMap::RT_NO_DATA)
        {
            ret = this->set(stDataRecord._key);
            if(ret == TC_HashMap::RT_OK)
            {
                return TC_HashMap::RT_ONLY_KEY;
            }
            return ret;
        }

        return TC_HashMap::RT_LOAD_DATA_ERR;
    }

    /**
     * 获取数据, 修改GET时间链
     * @param k
     * @param v
     *
     * @return int:
     *          TC_HashMap::RT_NO_DATA: 没有数据
     *          TC_HashMap::RT_ONLY_KEY:只有Key
     *          TC_HashMap::RT_OK:获取数据成功
     *          TC_HashMap::RT_LOAD_DATA_ERR: load数据失败
     *          其他返回值: 错误
     */
    int get(const K& k, V &v)
    {
        time_t iSyncTime;
        return get(k, v, iSyncTime);
    }

    /**
     * 根据key, 获取相同hash值的所有数据
     * 注意:c匹配对象操作中, map是加锁的, 需要注意
     * @param h
     * @param vv
     * @param c, 匹配仿函数: bool operator()(K v);
     * 
     * @return int, RT_OK
     */
    template<typename C>
    int getHash(size_t h, vector<pair<K, V> > &vv, C c)
    {
        int ret = TC_HashMap::RT_OK;

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());

        this->_t.doUpdate();
        size_t index = h % this->_t.getHashCount();
        size_t iAddr = this->_t.item(index)->_iBlockAddr;

        TC_HashMap::Block block(&this->_t, iAddr);

        while(block.getHead() != 0)
        {
            TC_HashMap::BlockData data;
            ret = block.getBlockData(data);
            if(ret == TC_HashMap::RT_OK)
            {
                K tk;
                V tv;

                try
                {
                    tars::TarsInputStream<BufferReader> is;
                    is.setBuffer(data._key.c_str(), data._key.length());
                    tk.readFrom(is);

                    if(c(tk))
                    {
                        is.setBuffer(data._value.c_str(), data._value.length());
                        tv.readFrom(is);
                        vv.push_back(make_pair(tk, tv));
                    }
                }
                catch(exception &ex)
                {
                }
            }
            if(!block.nextBlock())
            {
                break;
            }
        }

        return TC_HashMap::RT_OK;
    }

    /**
     * 根据key, 获取相同hash值的所有数据
     * 注意:c匹配对象操作中, map是加锁的, 需要注意
     * @param h
     * @param vv
     * @param c, 匹配仿函数: bool operator()(K v);
     * 
     * @return int, RT_OK
     */
    template<typename C>
    int getHash(size_t h, vector<DataRecord> &vv, C c)
    {
        int ret = TC_HashMap::RT_OK;

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());

        this->_t.doUpdate();
        size_t index = h % this->_t.getHashCount();
        size_t iAddr = this->_t.item(index)->_iBlockAddr;

        TC_HashMap::Block block(&this->_t, iAddr);

        while(block.getHead() != 0)
        {
            TC_HashMap::BlockData data;
            ret = block.getBlockData(data);
            if(ret == TC_HashMap::RT_OK)
            {
                K tk;
                V tv;

                try
                {
                    tars::TarsInputStream<BufferReader> is;
                    is.setBuffer(data._key.c_str(), data._key.length());
                    tk.readFrom(is);

                    if(c(tk))
                    {
                        is.setBuffer(data._value.c_str(), data._value.length());
                        tv.readFrom(is);

                        DataRecord stDataRecord;
                        stDataRecord._key = tk;
                        stDataRecord._value = tv;
                        stDataRecord._dirty = data._dirty;
                        stDataRecord._iSyncTime = data._synct;

                        vv.push_back(stDataRecord);
                    }
                }
                catch(exception &ex)
                {
                }
            }
            if(!block.nextBlock())
            {
                break;
            }
        }

        return TC_HashMap::RT_OK;
    }

    /**
     * 恢复数据
     * 对于block记录无法读取的数据自动删除
     * @param bRepair: 是否修复
     * @return 返回删除的记录数
     */
    size_t recover(bool bRepair)
    {
        size_t c     = this->_t.getHashCount();
        size_t e     = 0;
        for(size_t i = 0; i < c; i++)
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());

            e += this->_t.recover(i, bRepair);
        }

        return e;
    }

    /**
     * 设置数据, 修改时间链, 内存不够时会自动淘汰老的数据
     * @param k: 关键字
     * @param v: 值
     * @param bDirty: 是否是脏数据
     * @return int:
     *          TC_HashMap::RT_READONLY: map只读
     *          TC_HashMap::RT_NO_MEMORY: 没有空间(不淘汰数据情况下会出现)
     *          TC_HashMap::RT_OK: 设置成功
     *          其他返回值: 错误
     */
    int set(const K& k, const V& v, bool bDirty = true)
    {
        tars::TarsOutputStream<BufferWriter> osk;
        k.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());

        tars::TarsOutputStream<BufferWriter> osv;
        v.writeTo(osv);
        string sv(osv.getBuffer(), osv.getLength());

        int ret = TC_HashMap::RT_OK;
        vector<TC_HashMap::BlockData> vtData;

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.set(sk, sv, bDirty, vtData);
        }

        //操作淘汰数据
        if(_todo_of)
        {
            for(size_t i = 0; i < vtData.size(); i++)
            {
                K tk;
                V tv;
                try
                {
                    tars::TarsInputStream<BufferReader> is;
                    is.setBuffer(vtData[i]._key.c_str(), vtData[i]._key.length());
                    tk.readFrom(is);

                    is.setBuffer(vtData[i]._value.c_str(), vtData[i]._value.length());
                    tv.readFrom(is);

                    typename ToDoFunctor::DataRecord stDataRecord;
                    stDataRecord._key       = tk;
                    stDataRecord._value     = tv;
                    stDataRecord._dirty     = vtData[i]._dirty;
                    stDataRecord._iSyncTime = vtData[i]._synct;

                    _todo_of->sync(stDataRecord);
                }
                catch(exception &ex)
                {
                }
            }
        }
        return ret;
    }

    /**
     * 仅设置Key, 内存不够时会自动淘汰老的数据
     * @param k: 关键字
     * @return int:
     *          TC_HashMap::RT_READONLY: map只读
     *          TC_HashMap::RT_NO_MEMORY: 没有空间(不淘汰数据情况下会出现)
     *          TC_HashMap::RT_OK: 设置成功
     *          其他返回值: 错误
     */
    int set(const K& k)
    {
        tars::TarsOutputStream<BufferWriter> osk;
        k.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());

        int ret = TC_HashMap::RT_OK;
        vector<TC_HashMap::BlockData> vtData;

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.set(sk, vtData);
        }

        //操作淘汰数据
        if(_todo_of)
        {
            for(size_t i = 0; i < vtData.size(); i++)
            {
                K tk;
                V tv;

                try
                {
                    tars::TarsInputStream<BufferReader> is;
                    is.setBuffer(vtData[i]._key.c_str(), vtData[i]._key.length());
                    tk.readFrom(is);

                    is.setBuffer(vtData[i]._value.c_str(), vtData[i]._value.length());
                    tv.readFrom(is);

                    typename ToDoFunctor::DataRecord stDataRecord;
                    stDataRecord._key       = tk;
                    stDataRecord._value     = tv;
                    stDataRecord._dirty     = vtData[i]._dirty;
                    stDataRecord._iSyncTime = vtData[i]._synct;

                    _todo_of->sync(stDataRecord);
                }
                catch(exception &ex)
                {
                }
            }
        }
        return ret;
    }

    /**
     * 删除数据
     * 无论cache是否有数据,todo的del都被调用
     *
     * @param k, 关键字
     * @return int:
     *          TC_HashMap::RT_READONLY: map只读
     *          TC_HashMap::RT_NO_DATA: 没有当前数据
     *          TC_HashMap::RT_ONLY_KEY:只有Key, 也删除了
     *          TC_HashMap::RT_OK: 删除数据成功
     *          其他返回值: 错误
     */
    int del(const K& k)
    {
        int ret = TC_HashMap::RT_OK;

        TC_HashMap::BlockData data;

        tars::TarsOutputStream<BufferWriter> os;
        k.writeTo(os);
        string sk(os.getBuffer(), os.getLength());

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(sk, data);
        }

        if(ret != TC_HashMap::RT_OK && ret != TC_HashMap::RT_ONLY_KEY && ret != TC_HashMap::RT_NO_DATA)
        {
            return ret;
        }

        if(_todo_of)
        {
            typename ToDoFunctor::DataRecord stDataRecord;
            stDataRecord._key       = k;

            if(ret == TC_HashMap::RT_OK)
            {
                V v;
                tars::TarsInputStream<BufferReader> is;
                is.setBuffer(data._value.c_str(), data._value.length());
                v.readFrom(is);

                stDataRecord._value     = v;
                stDataRecord._dirty     = data._dirty;
                stDataRecord._iSyncTime = data._synct;
            }

            _todo_of->del((ret == TC_HashMap::RT_OK), stDataRecord);
        }
        return ret;
    }

    /**
     * 删除数据
     * cache有数据,todo的erase被调用
     *
     * @param k, 关键字
     * @return int:
     *          TC_HashMap::RT_READONLY: map只读
     *          TC_HashMap::RT_NO_DATA: 没有当前数据
     *          TC_HashMap::RT_ONLY_KEY:只有Key, 也删除了
     *          TC_HashMap::RT_OK: 删除数据成功
     *          其他返回值: 错误
     */
    int erase(const K& k)
    {
        int ret = TC_HashMap::RT_OK;

        TC_HashMap::BlockData data;

        tars::TarsOutputStream<BufferWriter> os;
        k.writeTo(os);
        string sk(os.getBuffer(), os.getLength());

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(sk, data);
        }

        if(ret != TC_HashMap::RT_OK)
        {
            return ret;
        }

        if(_todo_of)
        {
            V v;
            tars::TarsInputStream<BufferReader> is;
            is.setBuffer(data._value.c_str(), data._value.length());
            v.readFrom(is);

            typename ToDoFunctor::DataRecord stDataRecord;
            stDataRecord._key       = k;
            stDataRecord._value     = v;
            stDataRecord._dirty     = data._dirty;
            stDataRecord._iSyncTime = data._synct;

            _todo_of->erase(stDataRecord);
        }
        return ret;
    }

    /**
     * 强制删除数据,不调用todo的erase被调用
     *
     * @param k, 关键字
     * @return int:
     *          TC_HashMap::RT_READONLY: map只读
     *          TC_HashMap::RT_NO_DATA: 没有当前数据
     *          TC_HashMap::RT_ONLY_KEY:只有Key, 也删除了
     *          TC_HashMap::RT_OK: 删除数据成功
     *          其他返回值: 错误
     */
    int eraseByForce(const K& k)
    {
        int ret = TC_HashMap::RT_OK;

        TC_HashMap::BlockData data;

        tars::TarsOutputStream<BufferWriter> os;
        k.writeTo(os);
        string sk(os.getBuffer(), os.getLength());

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(sk, data);
        }

        if(ret != TC_HashMap::RT_OK)
        {
            return ret;
        }

        return ret;
    }

    /**
     * 根据key, 获取相同hash值的所有数据
     * 注意:c匹配对象操作中, map是加锁的, 需要注意
     * @param h
     * @param vv
     * @param c, 匹配仿函数: bool operator()(K v);
     * 
     * @return int, RT_OK
     */

    /**
     * 根据hash，强制删除相同hash值的所有数据,不调用todo的erase被调用
     * 注意:c匹配对象操作中, map是加锁的, 需要注意
     * @param h
     * @param c, 匹配仿函数: bool operator()(K v);
     * @return int:
     *          TC_HashMap::RT_READONLY: map只读
     *          TC_HashMap::RT_NO_DATA: 没有当前数据
     *          TC_HashMap::RT_ONLY_KEY:只有Key, 也删除了
     *          TC_HashMap::RT_OK: 删除数据成功
     *          其他返回值: 错误
     */
    template<typename C>
    int eraseHashByForce(size_t h, C c)
    {
        int ret = TC_HashMap::RT_OK;

        vector<string> vDelKey;

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());

        this->_t.doUpdate();
        size_t index = h % this->_t.getHashCount();
        size_t iAddr = this->_t.item(index)->_iBlockAddr;

        TC_HashMap::Block block(&this->_t, iAddr);
        TC_HashMap::BlockData data;

        while(block.getHead() != 0)
        {
            ret = block.getBlockData(data);
            if(ret == TC_HashMap::RT_OK)
            {
                K tk;
                V tv;

                try
                {
                    tars::TarsInputStream<BufferReader> is;
                    is.setBuffer(data._key.c_str(), data._key.length());
                    tk.readFrom(is);

                    if(c(tk))
                        vDelKey.push_back(data._key);
                }
                catch(exception &ex)
                {
                }
            }
            if(!block.nextBlock())
            {
                break;
            }
        }

        for(size_t i=0; i<vDelKey.size(); ++i)
        {
            ret = this->_t.del(vDelKey[i], data);
            if(ret != TC_HashMap::RT_OK)
            {
                return ret;
            }
        }

        return TC_HashMap::RT_OK;
    }

    /**
     * 根据hash，强制删除相同hash值的所有数据,不调用todo的erase被调用
     * 注意:c匹配对象操作中, map是加锁的, 需要注意
     * @param h
     * @param c, 匹配仿函数: bool operator()(K v);
     * @return int:
     *          TC_HashMap::RT_READONLY: map只读
     *          TC_HashMap::RT_NO_DATA: 没有当前数据
     *          TC_HashMap::RT_ONLY_KEY:只有Key, 也删除了
     *          TC_HashMap::RT_OK: 删除数据成功
     *          其他返回值: 错误
     */
    template<typename C>
    int eraseHashByForce(size_t h, C c, vector<K>& vDelK)
    {
        int ret = TC_HashMap::RT_OK;

        vector<string> vDelKey;

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());

        this->_t.doUpdate();
        size_t index = h % this->_t.getHashCount();
        size_t iAddr = this->_t.item(index)->_iBlockAddr;

        TC_HashMap::Block block(&this->_t, iAddr);
        TC_HashMap::BlockData data;

        while(block.getHead() != 0)
        {
            ret = block.getBlockData(data);
            if(ret == TC_HashMap::RT_OK)
            {
                K tk;

                try
                {
                    tars::TarsInputStream<BufferReader> is;
                    is.setBuffer(data._key.c_str(), data._key.length());
                    tk.readFrom(is);

                    if(c(tk))
                    {    
                        vDelKey.push_back(data._key);
                        vDelK.push_back(tk);
                    }
                }
                catch(exception &ex)
                {
                }
            }
            if(!block.nextBlock())
            {
                break;
            }
        }

        for(size_t i=0; i<vDelKey.size(); ++i)
        {
            ret = this->_t.del(vDelKey[i], data);
            if(ret != TC_HashMap::RT_OK)
            {
                vDelK.resize(i);
                return ret;
            }
        }

        return TC_HashMap::RT_OK;
    }

    /**
     * 淘汰数据, 根据Get时间淘汰
     * 直到: 元素个数/chunks * 100 < radio，bCheckDirty 为true时，遇到脏数据则淘汰结束
     * @param radio: 共享内存chunks使用比例 0< radio < 100
     * @return int:
     *          TC_HashMap::RT_READONLY: map只读
     *          TC_HashMap::RT_OK:淘汰完毕
     */
    int erase(int radio, bool bCheckDirty = false)
    {
        while(true)
        {
            int ret;
            TC_HashMap::BlockData data;

            {
                TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
                ret = this->_t.erase(radio, data, bCheckDirty);
                if(ret == TC_HashMap::RT_OK || ret == TC_HashMap::RT_READONLY)
                {
                    return ret;
                }

                if(ret != TC_HashMap::RT_ERASE_OK)
                {
                    continue;
                }
            }

            if(_todo_of)
            {
                K tk;
                V tv;

                tars::TarsInputStream<BufferReader> is;
                is.setBuffer(data._key.c_str(), data._key.length());
                tk.readFrom(is);

                is.setBuffer(data._value.c_str(), data._value.length());
                tv.readFrom(is);

                typename ToDoFunctor::DataRecord stDataRecord;
                stDataRecord._key       = tk;
                stDataRecord._value     = tv;
                stDataRecord._dirty     = data._dirty;
                stDataRecord._iSyncTime = data._synct;

                _todo_of->erase(stDataRecord);
            }
        }
        return TC_HashMap::RT_OK;
    }

    /**
     * 回写单条记录, 如果记录不存在, 则不做任何处理
     * @param k
     * 
     * @return int
     *          TC_HashMap::RT_NO_DATA: 没有数据
     *          TC_HashMap::RT_ONLY_KEY:只有Key
     *          TC_HashMap::RT_OK:获取数据成功
     *          TC_HashMap::RT_LOAD_DATA_ERR: load数据失败
     *          其他返回值: 错误
     */
    int sync(const K& k)
    {
        V v;
        time_t iSyncTime;
        int ret = get(k, v, iSyncTime);

        if(ret == TC_HashMap::RT_OK)
        {
            bool bDirty = (checkDirty(k) == TC_HashMap::RT_DIRTY_DATA);

            if(_todo_of)
            {
                typename ToDoFunctor::DataRecord stDataRecord;
                stDataRecord._key       = k;
                stDataRecord._value     = v;
                stDataRecord._dirty     = bDirty;
                stDataRecord._iSyncTime = iSyncTime;

                _todo_of->sync(stDataRecord);
            }
        }

        return ret;
    }

    /**
     * 将脏数据且一定时间没有回写的数据全部回写
     * 数据回写时间与当前时间超过_pHead->_iSyncTime(setSyncTime)则需要回写
     * 
     * map只读时仍然可以回写
     * 
     * @param iNowTime: 回写到什么时间, 通常是当前时间
     * @return int:
     *      TC_HashMap::RT_OK: 回写完毕了
     */
    int sync(time_t iNowTime)
    {
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            this->_t.sync();
        }

        while(true)
        {
            TC_HashMap::BlockData data;

            int ret;
            {
                TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
                ret = this->_t.sync(iNowTime, data);
                if(ret == TC_HashMap::RT_OK)
                {
                    return ret;
                }

                if(ret != TC_HashMap::RT_NEED_SYNC)
                {
                    continue;
                }
            }

            if(_todo_of)
            {
                K tk;
                V tv;

                tars::TarsInputStream<BufferReader> is;
                is.setBuffer(data._key.c_str(), data._key.length());
                tk.readFrom(is);

                is.setBuffer(data._value.c_str(), data._value.length());
                tv.readFrom(is);

                typename ToDoFunctor::DataRecord stDataRecord;
                stDataRecord._key       = tk;
                stDataRecord._value     = tv;
                stDataRecord._dirty     = data._dirty;
                stDataRecord._iSyncTime = data._synct;

                _todo_of->sync(stDataRecord);
            }
        }

        return TC_HashMap::RT_OK;
    }

    /**
    *将脏数据尾指针赋给回写尾指针
    */
    void sync()
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.sync();
    }

    /**
     * 将脏数据且一定时间没有回写的数据回写,只回写一个脏数据，目的是替代int sync(uint32_t iNowTime)
     * 方法，把由业务控制每次回写数据量，使用时应该先调用void sync()
     * 注意:c条件对象操作中, map是加锁的, 需要注意
     * 
     * map只读时仍然可以回写
     * 
     * @param iNowTime: 回写到什么时间, 通常是当前时间
     * @return int:
     *      TC_HashMapCompact::RT_OK: 回写完毕了
     * 
     * 示例：
     *      p->sync();
     *      while(true) {
     *          int iRet = pthis->SyncOnce(tNow);
     *          if( iRet == TC_HashMapCompact::RT_OK )
     *                break;
     *        }
     */
    template<typename C>
    int syncOnce(uint32_t iNowTime, C c)
    {
        TC_HashMap::BlockData data;

        K tk;
        tars::TarsInputStream<BufferReader> is;

        int ret;
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.sync(iNowTime, data);
            if(ret == TC_HashMap::RT_OK)
            {
                return ret;
            }

            if(ret != TC_HashMap::RT_NEED_SYNC)
            {
                return ret;
            }

            if(_todo_of)
            {
                is.setBuffer(data._key.c_str(), data._key.length());
                tk.readFrom(is);

                if(!c(tk))
                {    
                    this->_t.setDirtyAfterSync(data._key);
                    return ret;
                }
            }
            else
            {
                return ret;
            }
        }

        V tv;
        is.setBuffer(data._value.c_str(), data._value.length());
        tv.readFrom(is);

        typename ToDoFunctor::DataRecord stDataRecord;
        stDataRecord._key       = tk;
        stDataRecord._value     = tv;
        stDataRecord._dirty     = data._dirty;
        stDataRecord._iSyncTime = data._synct;

        _todo_of->sync(stDataRecord);

        return ret;
    }

    /**
     * 将脏数据且一定时间没有回写的数据回写,只回写一个脏数据，目的是替代int sync(time_t iNowTime)
     * 方法，把由业务控制每次回写数据量，使用时应该先调用void sync()
     * 
     * 数据回写时间与当前时间超过_pHead->_iSyncTime(setSyncTime)则需要回写

     * map只读时仍然可以回写
     * 
     * @param iNowTime: 回写到什么时间, 通常是当前时间
     * @return int:
     *      TC_HashMap::RT_OK: 回写完毕了
     * 
     * 示例：
     *      p->sync();
     *      while(true) {
     *          int iRet = pthis->SyncOnce(tNow);
     *          if( iRet == TC_HashMap::RT_OK )
     *                break;
     *        }
     */
    int syncOnce(time_t iNowTime)
    {
        

        TC_HashMap::BlockData data;

        int ret;
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.sync(iNowTime, data);
            if(ret == TC_HashMap::RT_OK)
            {
                return ret;
            }

            if(ret != TC_HashMap::RT_NEED_SYNC)
            {
                return ret;
            }
        }

        if(_todo_of)
        {
            K tk;
            V tv;

            tars::TarsInputStream<BufferReader> is;
            is.setBuffer(data._key.c_str(), data._key.length());
            tk.readFrom(is);

            is.setBuffer(data._value.c_str(), data._value.length());
            tv.readFrom(is);

            typename ToDoFunctor::DataRecord stDataRecord;
            stDataRecord._key       = tk;
            stDataRecord._value     = tv;
            stDataRecord._dirty     = data._dirty;
            stDataRecord._iSyncTime = data._synct;

            _todo_of->sync(stDataRecord);
        }

        return ret;
    }
    /**
     * 备份数据
     * map只读时仍然可以备份
     * 可以多个线程/进程备份数据,同时备份时bForceFromBegin设置为false效率更高
     *
     * @param bForceFromBegin: 是否强制重头开始备份, 通常为false
     * @return int:
     *      TC_HashMap::RT_OK: 备份OK了
     */
    int backup(bool bForceFromBegin = false)
    {
        {
            //开始准备备份
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            this->_t.backup(bForceFromBegin);
        }

        while(true)
        {
            TC_HashMap::BlockData data;

            int ret;
            {
                TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
                ret = this->_t.backup(data);
                if(ret == TC_HashMap::RT_OK)
                {
                    return ret;
                }

                if(ret != TC_HashMap::RT_NEED_BACKUP)
                {
                    continue;
                }
            }

            if(_todo_of)
            {
                K tk;
                V tv;

                tars::TarsInputStream<BufferReader> is;
                is.setBuffer(data._key.c_str(), data._key.length());
                tk.readFrom(is);

                is.setBuffer(data._value.c_str(), data._value.length());
                tv.readFrom(is);

                typename ToDoFunctor::DataRecord stDataRecord;
                stDataRecord._key       = tk;
                stDataRecord._value     = tv;
                stDataRecord._dirty     = data._dirty;
                stDataRecord._iSyncTime = data._synct;

                _todo_of->backup(stDataRecord);
            }
        }

        return TC_HashMap::RT_OK;
    }

    /**
     * 描述
     *
     * @return string
     */
    string desc() { return this->_t.desc(); }

    ///////////////////////////////////////////////////////////////////////////////
    /**
     * 尾部
     *
     * @return lock_iterator
     */
    lock_iterator end()
    {
        JhmAutoLockPtr jlock;
        return JhmLockIterator(this->_t.end(), jlock);
    }

    /**
     * 根据Key查找数据
     * @param k
     */
    lock_iterator find(const K& k)
    {
        tars::TarsOutputStream<BufferWriter> os;
        k.writeTo(os);
        string sk(os.getBuffer(), os.getLength());

        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.find(sk), jlock);
    }

    /**
     * block正序
     *
     * @return lock_iterator
     */
    lock_iterator begin()
    {
        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.begin(), jlock);
    }

    /**
     * block逆序
     *
     * @return lock_iterator
     */
    lock_iterator rbegin()
    {
        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.rbegin(), jlock);
    }

    /**
     * 以Set时间排序的迭代器
     * 返回的迭代器++表示按照时间顺序:最近Set-->最久Set
     *
     * @return lock_iterator
     */
    lock_iterator beginSetTime()
    {
        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.beginSetTime(), jlock);
    }

    /**
     * Set时间链逆序的迭代器
     *
     * 返回的迭代器++表示按照时间顺序:最久Set-->最近Set
     *
     * @return lock_iterator
     */
    lock_iterator rbeginSetTime()
    {
        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.rbeginSetTime(), jlock);
    }

    /**
     * 以Get时间排序的迭代器
     * 返回的迭代器++表示按照时间顺序:最近Get-->最久Get
     *
     * @return lock_iterator
     */
    lock_iterator beginGetTime()
    {
        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.beginGetTime(), jlock);
    }

    /**
     * Get时间链逆序的迭代器
     *
     * 返回的迭代器++表示按照时间顺序:最久Get-->最近Get
     *
     * @return lock_iterator
     */
    lock_iterator rbeginGetTime()
    {
        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.rbeginGetTime(), jlock);
    }

    /**
     * 获取脏链表尾部迭代器(最长时间没有Set的脏数据)
     *
     * 返回的迭代器++表示按照时间顺序:最近Set-->最久Set
     * 可能存在干净数据
     *
     * @return lock_iterator
     */
    lock_iterator beginDirty()
    {
        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.beginDirty(), jlock);
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    // 以下是遍历map函数, 不需要对map加锁

    /**
     * 根据hash桶遍历
     * 
     * @return hash_iterator
     */
    hash_iterator hashBegin()
    {
        JhmLockPtr jlock(new JhmLock(this->mutex()));
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return JhmIterator(this->_t.hashBegin(), jlock);
    }

    /**
     * 结束
     *
     * @return
     */
    hash_iterator hashEnd() 
    { 
        JhmLockPtr jlock;
        return JhmIterator(this->_t.hashEnd(), jlock); 
    }

    /**
     * 获取指定下标的hash_iterator
     * @param iIndex
     * 
     * @return hash_iterator
     */
    hash_iterator hashIndex(size_t iIndex)
    {
        JhmLockPtr jlock(new JhmLock(this->mutex()));
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return JhmIterator(this->_t.hashIndex(iIndex), jlock);
    }

protected:

    /**
     * 删除数据的函数对象
     */
    ToDoFunctor                 *_todo_of;
};

}

#endif
