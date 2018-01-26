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

#ifndef _JMEM_MULTI_HASHMAP_H
#define _JMEM_MULTI_HASHMAP_H

#include "util/tc_multi_hashmap.h"
#include "util/tc_autoptr.h"
#include "jmem/jmem_policy.h"
#include "tup/Tars.h"

namespace tars
{

/************************************************************************
 基本说明如下:
 基于Tars协议的支持多key的内存hashmap
 编解码出错则抛出TarsDecodeException和TarsEncodeException
 可以对锁策略和存储策略进行组合, 例如:
 基于信号量锁, 文件存储的多key hashmap:
 TarsMultiHashMap<Test::QueueElement, SemLockPolicy, FileStorePolicy>
 基于信号量锁, 共享内存存储的多key hashmap
 TarsMultiHashMap<Test::QueueElement, SemLockPolicy, ShmStorePolicy>
 基于线程锁, 内存存储的多key hashmap
 TarsMultiHashMap<Test::QueueElement, ThreadLockPolicy, MemStorePolicy>
 
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

 > erase(int ratio), 批量淘汰数据, 直到空闲块比率到达ratio;
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
 > 注意函数所有int的返回值, 如无特别说明, 请参见TC_Multi_HashMap::RT_
 
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

template<typename MK,
         typename UK,
         typename V,
         typename LockPolicy,
         template<class, class> class StorePolicy>
class TarsMultiHashMap : public StorePolicy<TC_Multi_HashMap, LockPolicy>
{
public:
    /**
    * 返回数据结构
    */
    struct Value
    {
        MK                _mkey;
        UK                _ukey;
        V                _value;
        bool            _dirty;
        uint8_t            _iVersion;    // 取值范围为1-255，0为特殊值，表示不检查版本。循环使用
        time_t            _iSyncTime;
        Value() : _dirty(true), _iVersion(1), _iSyncTime(0)
        {
        }
    };
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
        typedef Value DataRecord;    // 兼容老版本的名字

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
         * @return int, 获取到数据, 返回:TC_Multi_HashMap::RT_OK
         *              没有数据,返回:TC_Multi_HashMap::RT_NO_DATA,
         *              系统默认GET,返回:TC_Multi_HashMap::RT_NO_GET
         *              其他,则返回:TC_Multi_HashMap::RT_LOAD_DATA_ERR
         */
        virtual int get(DataRecord &stDataRecord)
        {
            return TC_Multi_HashMap::RT_NO_GET;
        }

        /**
        * 根据主key获取数据，默认返回RT_NO_GET
        * mk，需要获取数据的主key
        * vtRecords，返回的数据
        */
        virtual int get(MK mk, vector<DataRecord>& vtRecords)
        {
            return TC_Multi_HashMap::RT_NO_GET;
        }
    };

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
        JhmLockItem(const TC_Multi_HashMap::HashMapLockItem &item)
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
         * 获取当前item的key
         * @param mk, 主key
         * @param uk, 除主key外的联合主键
         * @return int
         *          TC_Multi_HashMap::RT_OK:数据获取OK
         *          其他值, 异常
         */
        int get(MK &mk, UK &uk)
        {
            string smk, suk;
            int ret = _item.get(smk, suk);
            if(ret != TC_Multi_HashMap::RT_OK)
            {
                return ret;
            }

            tars::TarsInputStream<BufferReader> is;
            is.setBuffer(smk.c_str(), smk.length());
            mk.readFrom(is);
            is.setBuffer(suk.c_str(), suk.length());
            uk.readFrom(is);

            return ret;
        }

        /**
         * 获取值当前item的value(含key)
         * @param v
         * @return int
         *          TC_Multi_HashMap::RT_OK:数据获取OK
         *          TC_Multi_HashMap::RT_ONLY_KEY: key有效, v无效为空
         *          其他值, 异常
         */
        int get(Value &v)
        {
            TC_Multi_HashMap::Value hv;
            int ret = _item.get(hv);
            if(ret != TC_Multi_HashMap::RT_OK && ret != TC_Multi_HashMap::RT_ONLY_KEY)
            {
                return ret;
            }

            tars::TarsInputStream<BufferReader> is;
            is.setBuffer(hv._mkey.c_str(), hv._mkey.length());
            v._mkey.readFrom(is);
            is.setBuffer(hv._data._key.c_str(), hv._data._key.length());
            v._ukey.readFrom(is);
            if(ret != TC_Multi_HashMap::RT_ONLY_KEY)
            {
                is.setBuffer(hv._data._value.c_str(), hv._data._value.length());
                v._value.readFrom(is);
            }

            v._dirty = hv._data._dirty;
            v._iVersion = hv._data._iVersion;
            v._iSyncTime = hv._data._synct;

            return ret;
        }

    protected:
        TC_Multi_HashMap::HashMapLockItem _item;
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
        JhmLockIterator(const TC_Multi_HashMap::lock_iterator it, const JhmAutoLockPtr &lock)
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
        TC_Multi_HashMap::lock_iterator  _it;

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
        JhmItem(const TC_Multi_HashMap::HashMapItem &item, const JhmLockPtr &lock)
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
         * 获取当前hash桶的所有数据, 注意只获取有key/value的数据
         * 对于只有key的数据, 不获取
         * 如果协议解码有问题也不获取
         * @param vs,
         */
        void get(vector<TarsMultiHashMap::Value> &vs)
        {
            vector<TC_Multi_HashMap::Value> vtData;

            {
                TC_LockT<typename LockPolicy::Mutex> lock(_lock->mutex());
                _item.get(vtData);
            }

            for(size_t i = 0; i < vtData.size(); i++)
            {

                try
                {
                    Value v;

                    tars::TarsInputStream<BufferReader> is;
                    is.setBuffer(vtData[i]._mkey.c_str(), vtData[i]._mkey.length());
                    v._mkey.readFrom(is);

                    is.setBuffer(vtData[i]._data._key.c_str(), vtData[i]._data._key.length());
                    v._ukey.readFrom(is);

                    is.setBuffer(vtData[i]._data._value.c_str(), vtData[i]._data._value.length());
                    v._value.readFrom(is);

                    v._iVersion = vtData[i]._data._iVersion;
                    v._dirty = vtData[i]._data._dirty;
                    v._iSyncTime = vtData[i]._data._synct;

                    vs.push_back(v);
                }
                catch(exception &ex)
                {
                }
            }
        }

    protected:
        TC_Multi_HashMap::HashMapItem _item;
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
        JhmIterator(const TC_Multi_HashMap::hash_iterator &it, const JhmLockPtr &lock)
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
        TC_Multi_HashMap::hash_iterator  _it;

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
    TarsMultiHashMap()
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
     * @param fratio
     */
    void initHashRatio(float fratio)                       { this->_t.initHashRatio(fratio);}

    /**
     * 初始化chunk个数/主key hash个数, 默认是1, 含义是一个主key下面大概有多个条数据
     * 有需要更改必须在create之前调用
     *
     * @param fratio
     */
    void initMainKeyHashRatio(float fratio)                { this->_t.initMainKeyHashRatio(fratio);}
    
    /**
     * 设置hash方式，这个hash函数将作为联合主键的hash函数
     * @param hash_of
     */
    void setHashFunctor(TC_Multi_HashMap::hash_functor hashf)     
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.setHashFunctor(hashf); 
    }

    /**
     * 设置主key的hash方式，如果不设，主key将使用上面的联合主键的hash函数
     * @param hash_of
     */
    void setHashFunctorM(TC_Multi_HashMap::hash_functor hashf)
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.setHashFunctorM(hashf); 
    }

    /**
     * 获取hash方式
     * 
     * @return TC_Multi_HashMap::hash_functor&
     */
    TC_Multi_HashMap::hash_functor &getHashFunctor()              { return this->_t.getHashFunctor(); }

    /* 获取主key hash方式
     * 
     * @return TC_Multi_HashMap::hash_functor&
     */
    TC_Multi_HashMap::hash_functor &getHashFunctorM()              { return this->_t.getHashFunctorM(); }

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
    * 获取主key hash桶个数
    */
    size_t getMainKeyHashCount()
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.getMainKeyHashCount();
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
     * 主键下Only key数据元素个数
     *
     * @return size_t
     */
    size_t onlyKeyCount()                                     
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.onlyKeyCount();
    }

    /**
     * 主key下Only key数据元素个数
     *
     * @return size_t
     */
    size_t onlyKeyCountM()                                     
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.onlyKeyCountM();
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
     * TC_Multi_HashMap::ERASEBYGET
     * TC_Multi_HashMap::ERASEBYSET
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
     * @return TC_Multi_HashMap::tagMapHead
     */
    TC_Multi_HashMap::tagMapHead& getMapHead()                    { return this->_t.getMapHead(); }

    /**
     * 设置回写时间间隔(秒)
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
     *          TC_Multi_HashMap::RT_DUMP_FILE_ERR: dump到文件出错
     *          TC_Multi_HashMap::RT_OK: dump到文件成功
     */
    int dump2file(const string &sFile, bool bDoClear = false)
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        int ret = this->_t.dump2file(sFile);
        if(ret != TC_Multi_HashMap::RT_OK)
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
     *          TC_Multi_HashMap::RT_LOAL_FILE_ERR: load出错
     *          TC_Multi_HashMap::RT_VERSION_MISMATCH_ERR: 版本不一致
     *          TC_Multi_HashMap::RT_OK: load成功
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
     * 检查数据脏状态
     * @param mk, 主key
     * @param uk, 除主key外的联合主键
     *
     * @return int
     *          TC_Multi_HashMap::RT_NO_DATA: 没有当前数据
     *          TC_Multi_HashMap::RT_ONLY_KEY:只有Key
     *          TC_Multi_HashMap::RT_DIRTY_DATA: 是脏数据
     *          TC_Multi_HashMap::RT_OK: 是干净数据
     *          其他返回值: 错误
     */
    int checkDirty(const MK &mk, const UK &uk)
    {
        tars::TarsOutputStream<BufferWriter> mosk;
        mk.writeTo(mosk);
        string smk(mosk.getBuffer(), mosk.getLength());

        tars::TarsOutputStream<BufferWriter> uosk;
        uk.writeTo(uosk);
        string suk(uosk.getBuffer(), uosk.getLength());

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.checkDirty(smk, suk);
    }

    /**
     * 检查主key下的数据脏状态，只要主key下任何一条记录是脏数据就返回脏
     * @param mk, 主key
     *
     * @return int
     *          TC_Multi_HashMap::RT_NO_DATA: 没有当前数据
     *          TC_Multi_HashMap::RT_ONLY_KEY:只有Key
     *          TC_Multi_HashMap::RT_DIRTY_DATA: 是脏数据
     *          TC_Multi_HashMap::RT_OK: 是干净数据
     *          其他返回值: 错误
     */
    int checkDirty(const MK &mk)
    {
        tars::TarsOutputStream<BufferWriter> mosk;
        mk.writeTo(mosk);
        string smk(mosk.getBuffer(), mosk.getLength());

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.checkDirty(smk);
    }

    /**
     * 设置为干净数据i, 修改SET/GET时间链, 会导致数据不回写
     * @param k
     *
     * @return int
     *          TC_Multi_HashMap::RT_READONLY: 只读
     *          TC_Multi_HashMap::RT_NO_DATA: 没有当前数据
     *          TC_Multi_HashMap::RT_ONLY_KEY:只有Key
     *          TC_Multi_HashMap::RT_OK: 设置成功
     *          其他返回值: 错误
     */
    int setClean(const MK &mk, const UK &uk)
    {
        tars::TarsOutputStream<BufferWriter> mosk;
        mk.writeTo(mosk);
        string smk(mosk.getBuffer(), mosk.getLength());

        tars::TarsOutputStream<BufferWriter> uosk;
        uk.writeTo(uosk);
        string suk(uosk.getBuffer(), uosk.getLength());

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.setClean(smk, suk);
    }

    /**
     * 设置为脏数据, 修改SET/GET时间链, 会导致数据回写
     * @param mk
     * @param uk
     * @return int
     *          TC_Multi_HashMap::RT_READONLY: 只读
     *          TC_Multi_HashMap::RT_NO_DATA: 没有当前数据
     *          TC_Multi_HashMap::RT_ONLY_KEY:只有Key
     *          TC_Multi_HashMap::RT_OK: 设置脏数据成功
     *          其他返回值: 错误
     */
    int setDirty(const MK &mk, const UK &uk)
    {
        tars::TarsOutputStream<BufferWriter> mosk;
        mk.writeTo(mosk);
        string smk(mosk.getBuffer(), mosk.getLength());

        tars::TarsOutputStream<BufferWriter> uosk;
        uk.writeTo(uosk);
        string suk(uosk.getBuffer(), uosk.getLength());

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.setDirty(smk, suk);
    }

    /**
     * 设置数据回写时间
     * @param mk
     * @param uk
     * @param iSyncTime
     * @return int
     *          TC_Multi_HashMap::RT_READONLY: 只读
     *          TC_Multi_HashMap::RT_NO_DATA: 没有当前数据
     *          TC_Multi_HashMap::RT_ONLY_KEY:只有Key
     *          TC_Multi_HashMap::RT_OK: 设置脏数据成功
     *          其他返回值: 错误
     */
    int setSyncTime(const MK &mk, const UK &uk, time_t iSyncTime)
    {
        tars::TarsOutputStream<BufferWriter> mosk;
        mk.writeTo(mosk);
        string smk(mosk.getBuffer(), mosk.getLength());

        tars::TarsOutputStream<BufferWriter> uosk;
        uk.writeTo(uosk);
        string suk(uosk.getBuffer(), uosk.getLength());

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.setSyncTime(smk, suk, iSyncTime);
    }

    /**
     * 获取数据, 修改GET时间链
     * (如果没设置自定义Get函数,没有数据时返回:RT_NO_DATA)
     * @param mk
     * @param uk
     * @param v
     *
     * @return int:
     *          TC_Multi_HashMap::RT_NO_DATA: 没有数据
     *          TC_Multi_HashMap::RT_READONLY: 只读模式
     *          TC_Multi_HashMap::RT_ONLY_KEY:只有Key
     *          TC_Multi_HashMap::RT_OK:获取数据成功
     *          TC_Multi_HashMap::RT_LOAD_DATA_ERR: load数据失败
     *          其他返回值: 错误
     */
    int get(const MK &mk, const UK &uk, Value &v)
    {
        int ret = TC_Multi_HashMap::RT_OK;

        tars::TarsOutputStream<BufferWriter> mosk;
        mk.writeTo(mosk);
        string smk(mosk.getBuffer(), mosk.getLength());

        tars::TarsOutputStream<BufferWriter> uosk;
        uk.writeTo(uosk);
        string suk(uosk.getBuffer(), uosk.getLength());

        TC_Multi_HashMap::Value tv;

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.get(smk, suk, tv);
        }

        // 读取到数据了, 解包
        if(ret == TC_Multi_HashMap::RT_OK)
        {
            v._mkey = mk;
            v._ukey = uk;
            v._dirty = tv._data._dirty;
            v._iSyncTime = tv._data._synct;
            v._iVersion = tv._data._iVersion;

            tars::TarsInputStream<BufferReader> is;
            is.setBuffer(tv._data._value.c_str(), tv._data._value.length());
            v._value.readFrom(is);

            return ret;
        }

        if(ret != TC_Multi_HashMap::RT_NO_DATA || _todo_of == NULL)
        {
            return ret;
        }

        //只读模式
        if(isReadOnly())
        {
            return TC_Multi_HashMap::RT_READONLY;
        }

        // Hashmap中没有数据，从外部获取函数获取数据
        typename ToDoFunctor::DataRecord stDataRecord;
        stDataRecord._mkey   = mk;
        stDataRecord._ukey   = uk;
        ret = _todo_of->get(stDataRecord);
        if(ret == TC_Multi_HashMap::RT_OK)
        {
            v = stDataRecord;
            // 设置到hashmap中
            return this->set(stDataRecord._mkey, stDataRecord._ukey, stDataRecord._value, 0, stDataRecord._dirty);
        }
        else if(ret == TC_Multi_HashMap::RT_NO_GET)
        {
            return TC_Multi_HashMap::RT_NO_DATA;
        }
        else if(ret == TC_Multi_HashMap::RT_NO_DATA)
        {
            // 没有数据，以only key的形式设置到hashmap中
            ret = this->set(stDataRecord._mkey, stDataRecord._ukey);
            if(ret == TC_Multi_HashMap::RT_OK)
            {
                return TC_Multi_HashMap::RT_ONLY_KEY;
            }
            return ret;
        }

        return TC_Multi_HashMap::RT_LOAD_DATA_ERR;
    }

    /**
    * 仅根据主key获取所有的数据
    * @param mk
    * @param vs
    * 
    * @return int:
    *          RT_NO_DATA: 没有数据
    *          RT_ONLY_KEY: 只有Key
    *          RT_PART_DATA: 数据不全，只有部分数据
    *          RT_OK: 获取数据成功
    *          其他返回值: 错误
    */
    int get(const MK& mk, vector<Value> &vs)
    {
        tars::TarsOutputStream<BufferWriter> os;
        mk.writeTo(os);
        string smk(os.getBuffer(), os.getLength());

        int ret = TC_Multi_HashMap::RT_OK;
        vector<TC_Multi_HashMap::Value> hvs;
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.get(smk, hvs);
        }

        if(ret == TC_Multi_HashMap::RT_OK)
        {
            for(size_t i = 0; i < hvs.size(); i ++)
            {
                Value v;
                v._mkey = mk;
                
                tars::TarsInputStream<BufferReader> is;
                is.setBuffer(hvs[i]._data._key.c_str(), hvs[i]._data._key.length());
                v._ukey.readFrom(is);
                
                is.setBuffer(hvs[i]._data._value.c_str(), hvs[i]._data._value.length());
                v._value.readFrom(is);
                
                v._iVersion = hvs[i]._data._iVersion;
                v._dirty = hvs[i]._data._dirty;
                v._iSyncTime = hvs[i]._data._synct;
                vs.push_back(v);
            }
            return ret;
        }

        if(ret != TC_Multi_HashMap::RT_NO_DATA || _todo_of == NULL)
        {
            return ret;
        }

        //只读模式
        if(isReadOnly())
        {
            return TC_Multi_HashMap::RT_READONLY;
        }

        // Hashmap中没有数据，从外部获取函数获取数据
        ret = _todo_of->get(mk, vs);
        if(ret == TC_Multi_HashMap::RT_OK)
        {
            // 设置到hashmap中
            for(size_t i = 0; i < vs.size(); i ++)
            {
                ret = this->set(vs[i]._mkey, vs[i]._ukey, vs[i]._value, 0, vs[i]._dirty);
                if(ret != TC_Multi_HashMap::RT_OK)
                {
                    // 把设置进去的全部删除
                    vector<TC_Multi_HashMap::Value> vtErased;
                    {
                        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
                        this->_t.del(smk, vtErased);
                    }
                    return ret;
                }
            }
        }
        else if(ret == TC_Multi_HashMap::RT_NO_GET)
        {
            return TC_Multi_HashMap::RT_NO_DATA;
        }
        else if(ret == TC_Multi_HashMap::RT_NO_DATA)
        {
            // 没有数据，以only key的形式设置到hashmap中
            ret = this->set(mk);
            if(ret == TC_Multi_HashMap::RT_OK)
            {
                return TC_Multi_HashMap::RT_ONLY_KEY;
            }
            return ret;
        }
        
        return TC_Multi_HashMap::RT_LOAD_DATA_ERR;
    }

    /**
     * 根据hash值获取相同hash值的所有数据
     * 注意:c匹配对象操作中, map是加锁的, 需要注意
     * @param h, h为联合主键(MK+UK)的hash
     * @param vv
     * @param c, 匹配仿函数: bool operator()(MK, UK);
     * 
     * @return int, RT_OK
     */
    template<typename C>
    int getHash(uint32_t h, vector<Value> &vv, C c)
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());

        size_t index = h % this->_t.getHashCount();
        size_t iAddr = this->_t.item(index)->_iBlockAddr;
        TC_Multi_HashMap::lock_iterator it(&this->_t, iAddr, TC_Multi_HashMap::lock_iterator::IT_UKEY, TC_Multi_HashMap::lock_iterator::IT_NEXT);

        while(it != this->_t.end())
        {
            Value v;
            TC_Multi_HashMap::Value hv;
            int ret = it->get(hv);
            if(ret == TC_Multi_HashMap::RT_OK)
            {
                tars::TarsInputStream<BufferReader> is;
                is.setBuffer(hv._mkey.c_str(), hv._mkey.length());
                v._mkey.readFrom(is);

                is.setBuffer(hv._data._key.c_str(), hv._data._key.length());
                v._ukey.readFrom(is);
                
                if(c(v._mkey, v._ukey))
                {
                    is.setBuffer(hv._data._value.c_str(), hv._data._value.length());
                    v._value.readFrom(is);
                    
                    v._iVersion = hv._data._iVersion;
                    v._dirty = hv._data._dirty;
                    v._iSyncTime = hv._data._synct;
                    vv.push_back(v);
                }
            }
            it ++;
        }

        return TC_Multi_HashMap::RT_OK;
    }

     /**
     * 根据主key hash值获取相同主key hash值的所有数据
     * 注意:c匹配对象操作中, map是加锁的, 需要注意
     * @param h, h为主key(MK)的hash
     * @param mv, 返回结果集，以主key分组
     * @param c, 匹配仿函数: bool operator()(MK);
     * 
     * @return int, RT_OK
     */
    template<typename C>
    int getHashM(uint32_t h, map<MK, vector<Value> > &mv, C c)
    {
        int ret = TC_Multi_HashMap::RT_OK;
        map<string, vector<TC_Multi_HashMap::Value> > hmv;
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.get(h, hmv);
        }
        
        if(ret == TC_Multi_HashMap::RT_OK)
        {
            map<string, vector<TC_Multi_HashMap::Value> >::iterator it = hmv.begin();
            while(it != hmv.end())
            {
                MK mk;
                tars::TarsInputStream<BufferReader> is;
                is.setBuffer(it->first.c_str(), it->first.length());
                mk.readFrom(is);

                if(c(mk))
                {
                    vector<Value> vs;
                    vector<TC_Multi_HashMap::Value> &hvs = it->second;
                    for(size_t i = 0; i < hvs.size(); i ++)
                    {
                        Value v;
                        v._mkey = mk;
                        
                        is.setBuffer(hvs[i]._data._key.c_str(), hvs[i]._data._key.length());
                        v._ukey.readFrom(is);
                        
                        is.setBuffer(hvs[i]._data._value.c_str(), hvs[i]._data._value.length());
                        v._value.readFrom(is);
                        
                        v._iVersion = hvs[i]._data._iVersion;
                        v._dirty = hvs[i]._data._dirty;
                        v._iSyncTime = hvs[i]._data._synct;
                        vs.push_back(v);
                    }                    
                    mv[mk] = vs;
                }
                it ++;
            }
        }
        return ret;
    }

    /**
     * 设置数据, 修改时间链, 内存不够时会自动淘汰老的数据
     * @param mk: 主key
     * @param uk: 除主key外的联合主键
     * @param v: 值
     * @param iVersion: 设置的数据版本，应该根据get的数据版本来设置，0表示不检查版本
     * @param bDirty: 是否是脏数据
     * @param bHead: 数据插入到主key链的头部还是尾部
     * @param eType: 插入的数据的类型
     *            PART_DATA: 插入的数据是不完整的数据
     *            FULL_DATA: 插入的数据是完整数据
     *            AUTO_DATA: 根据Cache已有数据类型决定最终数据类型，如果已有数据是不完整的，最终的数据也是不完整的，如果已有数据是完整的，最终数据也是完整的，如果Cache中没有数据，最终数据是不完整的
     * @return int:
     *          TC_Multi_HashMap::RT_READONLY: map只读
     *          TC_Multi_HashMap::RT_NO_MEMORY: 没有空间(不淘汰数据情况下会出现)
     *          TC_Multi_HashMap::RT_OK: 设置成功
     *          其他返回值: 错误
     */
    int set(const MK &mk, const UK &uk, const V &v, uint8_t iVersion, 
        bool bDirty = true, TC_Multi_HashMap::DATATYPE eType = TC_Multi_HashMap::AUTO_DATA, bool bHead = true)
    {
        tars::TarsOutputStream<BufferWriter> mos;
        mk.writeTo(mos);
        string smk(mos.getBuffer(), mos.getLength());

        tars::TarsOutputStream<BufferWriter> uos;
        uk.writeTo(uos);
        string suk(uos.getBuffer(), uos.getLength());

        tars::TarsOutputStream<BufferWriter> vos;
        v.writeTo(vos);
        string sv(vos.getBuffer(), vos.getLength());

        int ret = TC_Multi_HashMap::RT_OK;
        vector<TC_Multi_HashMap::Value> vtErased;

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.set(smk, suk, sv, iVersion, bDirty, eType, bHead, vtErased);
        }

        //操作淘汰数据
        if(_todo_of)
        {
            for(size_t i = 0; i < vtErased.size(); i++)
            {
                MK emk;
                UK euk;
                V tv;
                try
                {
                    tars::TarsInputStream<BufferReader> is;
                    is.setBuffer(vtErased[i]._mkey.c_str(), vtErased[i]._mkey.length());
                    emk.readFrom(is);

                    is.setBuffer(vtErased[i]._data._key.c_str(), vtErased[i]._data._key.length());
                    euk.readFrom(is);

                    is.setBuffer(vtErased[i]._data._value.c_str(), vtErased[i]._data._value.length());
                    tv.readFrom(is);

                    typename ToDoFunctor::DataRecord stDataRecord;
                    stDataRecord._mkey      = emk;
                    stDataRecord._ukey      = euk;
                    stDataRecord._value     = tv;
                    stDataRecord._iVersion    = vtErased[i]._data._iVersion;
                    stDataRecord._dirty     = vtErased[i]._data._dirty;
                    stDataRecord._iSyncTime = vtErased[i]._data._synct;

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
     * @param mk: 主key
     * @param uk: 除主key外的联合主键
     * @param bHead: 数据插入到主key链的头部还是尾部
     * @param eType: 插入的数据的类型
     *            PART_DATA: 插入的数据是不完整的数据
     *            FULL_DATA: 插入的数据是完整数据
     *            AUTO_DATA: 根据Cache已有数据类型决定最终数据类型，如果已有数据是不完整的，最终的数据也是不完整的，如果已有数据是完整的，最终数据也是完整的，如果Cache中没有数据，最终数据是不完整的
     * @return int:
     *          TC_Multi_HashMap::RT_READONLY: map只读
     *          TC_Multi_HashMap::RT_NO_MEMORY: 没有空间(不淘汰数据情况下会出现)
     *          TC_Multi_HashMap::RT_OK: 设置成功
     *          其他返回值: 错误
     */
    int set(const MK &mk, const UK &uk, TC_Multi_HashMap::DATATYPE eType = TC_Multi_HashMap::AUTO_DATA, bool bHead = true)
    {
        tars::TarsOutputStream<BufferWriter> mos;
        mk.writeTo(mos);
        string smk(mos.getBuffer(), mos.getLength());
        
        tars::TarsOutputStream<BufferWriter> uos;
        uk.writeTo(uos);
        string suk(uos.getBuffer(), uos.getLength());

        int ret = TC_Multi_HashMap::RT_OK;
        vector<TC_Multi_HashMap::Value> vtErased;

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.set(smk, suk, eType, bHead, vtErased);
        }

        //操作淘汰数据
        if(_todo_of)
        {
            for(size_t i = 0; i < vtErased.size(); i++)
            {
                MK emk;
                UK euk;
                V tv;

                try
                {
                    tars::TarsInputStream<BufferReader> is;
                    is.setBuffer(vtErased[i]._mkey.c_str(), vtErased[i]._mkey.length());
                    emk.readFrom(is);

                    is.setBuffer(vtErased[i]._data._key.c_str(), vtErased[i]._data._key.length());
                    euk.readFrom(is);

                    is.setBuffer(vtErased[i]._data._value.c_str(), vtErased[i]._data._value.length());
                    tv.readFrom(is);

                    typename ToDoFunctor::DataRecord stDataRecord;
                    stDataRecord._mkey      = emk;
                    stDataRecord._ukey      = euk;
                    stDataRecord._value     = tv;
                    stDataRecord._iVersion    = vtErased[i]._data._iVersion;
                    stDataRecord._dirty     = vtErased[i]._data._dirty;
                    stDataRecord._iSyncTime = vtErased[i]._data._synct;

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
     * 仅设置主Key, 内存不够时会自动淘汰老的数据
     * @param mk: 主key
     * @return int:
     *          TC_Multi_HashMap::RT_READONLY: map只读
     *          TC_Multi_HashMap::RT_NO_MEMORY: 没有空间(不淘汰数据情况下会出现)
     *          TC_Multi_HashMap::RT_OK: 设置成功
     *          其他返回值: 错误
     */
    int set(const MK &mk)
    {
        tars::TarsOutputStream<BufferWriter> mos;
        mk.writeTo(mos);
        string smk(mos.getBuffer(), mos.getLength());
        
        int ret = TC_Multi_HashMap::RT_OK;
        vector<TC_Multi_HashMap::Value> vtErased;

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.set(smk, vtErased);
        }

        //操作淘汰数据
        if(_todo_of)
        {
            for(size_t i = 0; i < vtErased.size(); i++)
            {
                MK emk;
                UK euk;
                V tv;

                try
                {
                    tars::TarsInputStream<BufferReader> is;
                    is.setBuffer(vtErased[i]._mkey.c_str(), vtErased[i]._mkey.length());
                    emk.readFrom(is);

                    is.setBuffer(vtErased[i]._data._key.c_str(), vtErased[i]._data._key.length());
                    euk.readFrom(is);

                    is.setBuffer(vtErased[i]._data._value.c_str(), vtErased[i]._data._value.length());
                    tv.readFrom(is);

                    typename ToDoFunctor::DataRecord stDataRecord;
                    stDataRecord._mkey      = emk;
                    stDataRecord._ukey      = euk;
                    stDataRecord._value     = tv;
                    stDataRecord._iVersion    = vtErased[i]._data._iVersion;
                    stDataRecord._dirty     = vtErased[i]._data._dirty;
                    stDataRecord._iSyncTime = vtErased[i]._data._synct;

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
    * 批量设置数据，内存不够时会自动淘汰
    * 注意，此接口通常用于从数据库中取出相同主key下的一批数据并同步到Cache
    * 不能滥用此接口来批量插入数据，要注意保证Cache和数据库数据的一致
    * @param vs, 批量数据集
    * @param eType: 插入的数据的类型
    *            PART_DATA: 插入的数据是不完整的数据
    *            FULL_DATA: 插入的数据是完整数据
    *            AUTO_DATA: 根据Cache已有数据类型决定最终数据类型，如果已有数据是不完整的，最终的数据也是不完整的，如果已有数据是完整的，最终数据也是完整的，如果Cache中没有数据，最终数据是不完整的
    * @param bHead, 数据插入到主key链的头部还是尾部
    * @param bForce, 是否强制插入数据，为false则表示如果数据已经存在则不更新
    *
    * @return int:
    *          TC_Multi_HashMap::RT_READONLY: map只读
    *          TC_Multi_HashMap::RT_NO_MEMORY: 没有空间(不淘汰数据情况下会出现)
    *          TC_Multi_HashMap::RT_OK: 设置成功
    *          其他返回值: 错误
    */
    int set(const vector<Value> &vs, TC_Multi_HashMap::DATATYPE eType = TC_Multi_HashMap::AUTO_DATA, bool bHead = true, bool bForce = true)
    {
        int ret = TC_Multi_HashMap::RT_OK;
        vector<TC_Multi_HashMap::Value> vtSet, vtErased;

        for(size_t i = 0; i < vs.size(); i ++)
        {
            TC_Multi_HashMap::Value v;

            tars::TarsOutputStream<BufferWriter> mos;
            vs[i]._mkey.writeTo(mos);
            v._mkey.assign(mos.getBuffer(), mos.getLength());
            
            tars::TarsOutputStream<BufferWriter> uos;
            vs[i]._ukey.writeTo(uos);
            v._data._key.assign(uos.getBuffer(), uos.getLength());

            tars::TarsOutputStream<BufferWriter> vos;
            vs[i]._value.writeTo(vos);
            v._data._value.assign(vos.getBuffer(), vos.getLength());

            v._data._dirty = vs[i]._dirty;
            v._data._iVersion = vs[i]._iVersion;
            v._data._synct = vs[i]._iSyncTime;

            vtSet.push_back(v);
        }
        
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.set(vtSet, eType, bHead, bForce, vtErased);
        }

        //操作淘汰数据
        if(_todo_of)
        {
            for(size_t i = 0; i < vtErased.size(); i++)
            {
                MK emk;
                UK euk;
                V tv;
                
                try
                {
                    tars::TarsInputStream<BufferReader> is;
                    is.setBuffer(vtErased[i]._mkey.c_str(), vtErased[i]._mkey.length());
                    emk.readFrom(is);
                    
                    is.setBuffer(vtErased[i]._data._key.c_str(), vtErased[i]._data._key.length());
                    euk.readFrom(is);
                    
                    is.setBuffer(vtErased[i]._data._value.c_str(), vtErased[i]._data._value.length());
                    tv.readFrom(is);
                    
                    typename ToDoFunctor::DataRecord stDataRecord;
                    stDataRecord._mkey      = emk;
                    stDataRecord._ukey      = euk;
                    stDataRecord._value     = tv;
                    stDataRecord._iVersion    = vtErased[i]._data._iVersion;
                    stDataRecord._dirty     = vtErased[i]._data._dirty;
                    stDataRecord._iSyncTime = vtErased[i]._data._synct;
                    
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
     * @param mk, 主key
     * @param uk, 除主key外的联合主键
     *
     * @return int:
     *          TC_Multi_HashMap::RT_READONLY: map只读
     *          TC_Multi_HashMap::RT_NO_DATA: 没有当前数据
     *          TC_Multi_HashMap::RT_ONLY_KEY:只有Key, 也删除了
     *          TC_Multi_HashMap::RT_OK: 删除数据成功
     *          其他返回值: 错误
     */
    int del(const MK &mk, const UK &uk)
    {
        int ret = TC_Multi_HashMap::RT_OK;

        TC_Multi_HashMap::Value data;

        tars::TarsOutputStream<BufferWriter> mos;
        mk.writeTo(mos);
        string smk(mos.getBuffer(), mos.getLength());

        tars::TarsOutputStream<BufferWriter> uos;
        uk.writeTo(uos);
        string suk(uos.getBuffer(), uos.getLength());

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(smk, suk, data);
        }

        if(ret != TC_Multi_HashMap::RT_OK && ret != TC_Multi_HashMap::RT_ONLY_KEY && ret != TC_Multi_HashMap::RT_NO_DATA)
        {
            return ret;
        }

        if(_todo_of)
        {
            typename ToDoFunctor::DataRecord stDataRecord;

            stDataRecord._mkey = mk;
            stDataRecord._ukey = uk;

            if(ret == TC_Multi_HashMap::RT_OK)
            {
                V v;
                tars::TarsInputStream<BufferReader> is;
                is.setBuffer(data._data._value.c_str(), data._data._value.length());
                v.readFrom(is);

                stDataRecord._value     = v;
                stDataRecord._iVersion    = data._data._iVersion;
                stDataRecord._dirty     = data._data._dirty;
                stDataRecord._iSyncTime = data._data._synct;
            }

            _todo_of->del((ret == TC_Multi_HashMap::RT_OK), stDataRecord);
        }
        return ret;
    }

    /**
     * 删除主key下的所有数据
     * cache有数据,todo的erase被调用
     *
     * @param mk, 主key
     *
     * @return int:
     *          TC_Multi_HashMap::RT_READONLY: map只读
     *          TC_Multi_HashMap::RT_NO_DATA: 没有当前数据
     *          TC_Multi_HashMap::RT_ONLY_KEY:只有Key, 也删除了
     *          TC_Multi_HashMap::RT_OK: 删除数据成功
     *          其他返回值: 错误
     */
    int erase(const MK &mk)
    {
        int ret = TC_Multi_HashMap::RT_OK;

        vector<TC_Multi_HashMap::Value> vtErased;

        tars::TarsOutputStream<BufferWriter> os;
        mk.writeTo(os);
        string smk(os.getBuffer(), os.getLength());

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(smk, vtErased);
        }

        if(ret != TC_Multi_HashMap::RT_OK)
        {
            return ret;
        }

        if(_todo_of)
        {
            for(size_t i = 0; i < vtErased.size(); i ++)
            {
                MK emk;
                UK euk;
                V tv;
                
                try
                {
                    tars::TarsInputStream<BufferReader> is;
                    is.setBuffer(vtErased[i]._mkey.c_str(), vtErased[i]._mkey.length());
                    emk.readFrom(is);
                    
                    is.setBuffer(vtErased[i]._data._key.c_str(), vtErased[i]._data._key.length());
                    euk.readFrom(is);
                    
                    is.setBuffer(vtErased[i]._data._value.c_str(), vtErased[i]._data._value.length());
                    tv.readFrom(is);
                    
                    typename ToDoFunctor::DataRecord stDataRecord;
                    stDataRecord._mkey      = emk;
                    stDataRecord._ukey      = euk;
                    stDataRecord._value     = tv;
                    stDataRecord._iVersion    = vtErased[i]._data._iVersion;
                    stDataRecord._dirty     = vtErased[i]._data._dirty;
                    stDataRecord._iSyncTime = vtErased[i]._data._synct;
                    
                    _todo_of->erase(stDataRecord);
                }
                catch(exception &ex)
                {
                }
            }
        }
        return ret;
    }

    /**
     * 删除指定数据
     * cache有数据,todo的erase被调用
     *
     * @param mk, 主key
     * @param uk, 除主key外的联合主键
     *
     * @return int:
     *          TC_Multi_HashMap::RT_READONLY: map只读
     *          TC_Multi_HashMap::RT_NO_DATA: 没有当前数据
     *          TC_Multi_HashMap::RT_ONLY_KEY:只有Key, 也删除了
     *          TC_Multi_HashMap::RT_OK: 删除数据成功
     *          其他返回值: 错误
     */
    int erase(const MK &mk, const UK &uk)
    {
        int ret = TC_Multi_HashMap::RT_OK;
        
        TC_Multi_HashMap::Value data;
        
        tars::TarsOutputStream<BufferWriter> mos;
        mk.writeTo(mos);
        string smk(mos.getBuffer(), mos.getLength());
        
        tars::TarsOutputStream<BufferWriter> uos;
        uk.writeTo(uos);
        string suk(uos.getBuffer(), uos.getLength());
        
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(smk, suk, data);
        }
        
        if(ret != TC_Multi_HashMap::RT_OK)
        {
            return ret;
        }
        if(_todo_of)
        {
            MK emk;
            UK euk;
            V tv;
            
            try
            {
                tars::TarsInputStream<BufferReader> is;
                is.setBuffer(data._mkey.c_str(), data._mkey.length());
                emk.readFrom(is);
                
                is.setBuffer(data._data._key.c_str(), data._data._key.length());
                euk.readFrom(is);
                
                is.setBuffer(data._data._value.c_str(), data._data._value.length());
                tv.readFrom(is);
                
                typename ToDoFunctor::DataRecord stDataRecord;
                stDataRecord._mkey      = emk;
                stDataRecord._ukey      = euk;
                stDataRecord._value     = tv;
                stDataRecord._iVersion    = data._data._iVersion;
                stDataRecord._dirty     = data._data._dirty;
                stDataRecord._iSyncTime = data._data._synct;
                
                _todo_of->erase(stDataRecord);
            }
            catch(exception &ex)
            {
            }
        }
        return ret;
    }

    /**
     * 强制删除数据,不调用todo的erase被调用
     *
     * @param mk, 主key
     * @param uk, 除主key外的联合主键
     *
     * @return int:
     *          TC_Multi_HashMap::RT_READONLY: map只读
     *          TC_Multi_HashMap::RT_NO_DATA: 没有当前数据
     *          TC_Multi_HashMap::RT_ONLY_KEY:只有Key, 也删除了
     *          TC_Multi_HashMap::RT_OK: 删除数据成功
     *          其他返回值: 错误
     */
    int eraseByForce(const MK &mk, const UK &uk)
    {
        int ret = TC_Multi_HashMap::RT_OK;

        TC_Multi_HashMap::Value data;

        tars::TarsOutputStream<BufferWriter> mos;
        mk.writeTo(mos);
        string smk(mos.getBuffer(), mos.getLength());

        tars::TarsOutputStream<BufferWriter> uos;
        uk.writeTo(uos);
        string suk(uos.getBuffer(), uos.getLength());

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(smk, suk, data);
        }

        if(ret != TC_Multi_HashMap::RT_OK)
        {
            return ret;
        }

        return ret;
    }

    /**
     * 强制删除主key下的所有数据,不调用todo的erase被调用
     *
     * @param mk, 主key
     * @return int:
     *          TC_Multi_HashMap::RT_READONLY: map只读
     *          TC_Multi_HashMap::RT_NO_DATA: 没有当前数据
     *          TC_Multi_HashMap::RT_ONLY_KEY:只有Key, 也删除了
     *          TC_Multi_HashMap::RT_OK: 删除数据成功
     *          其他返回值: 错误
     */
    int eraseByForce(const MK &mk)
    {
        int ret = TC_Multi_HashMap::RT_OK;
        
        vector<TC_Multi_HashMap::Value> data;
        
        tars::TarsOutputStream<BufferWriter> os;
        mk.writeTo(os);
        string smk(os.getBuffer(), os.getLength());
        
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(smk, data);
        }
        
        if(ret != TC_Multi_HashMap::RT_OK)
        {
            return ret;
        }
        
        return ret;
    }

    /**
     * 淘汰数据, 根据Get时间淘汰
     * 直到: 元素个数/chunks * 100 < ratio，bCheckDirty 为true时，遇到脏数据则淘汰结束
     * @param ratio: 共享内存chunks使用比例 0< ratio < 100
     * @param bCheckDirty: 是否检查数据脏状态，如果检查则遇到脏数据不淘汰

     * @return int:
     *          TC_Multi_HashMap::RT_READONLY: map只读
     *          TC_Multi_HashMap::RT_OK:淘汰完毕
     */
    int erase(int ratio, bool bCheckDirty = false)
    {
        while(true)
        {
            int ret;
            vector<TC_Multi_HashMap::Value> vtErased;

            {
                TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
                ret = this->_t.erase(ratio, vtErased, bCheckDirty);
                if(ret == TC_Multi_HashMap::RT_OK || ret == TC_Multi_HashMap::RT_READONLY)
                {
                    return ret;
                }

                if(ret != TC_Multi_HashMap::RT_ERASE_OK)
                {
                    continue;
                }
            }

            if(_todo_of)
            {
                for(size_t i = 0; i < vtErased.size(); i++)
                {
                    MK emk;
                    UK euk;
                    V tv;
                    try
                    {
                        tars::TarsInputStream<BufferReader> is;
                        is.setBuffer(vtErased[i]._mkey.c_str(), vtErased[i]._mkey.length());
                        emk.readFrom(is);
                        
                        is.setBuffer(vtErased[i]._data._key.c_str(), vtErased[i]._data._key.length());
                        euk.readFrom(is);
                        
                        is.setBuffer(vtErased[i]._data._value.c_str(), vtErased[i]._data._value.length());
                        tv.readFrom(is);
                        
                        typename ToDoFunctor::DataRecord stDataRecord;
                        stDataRecord._mkey      = emk;
                        stDataRecord._ukey      = euk;
                        stDataRecord._value     = tv;
                        stDataRecord._iVersion    = vtErased[i]._data._iVersion;
                        stDataRecord._dirty     = vtErased[i]._data._dirty;
                        stDataRecord._iSyncTime = vtErased[i]._data._synct;
                        
                        _todo_of->erase(stDataRecord);
                    }
                    catch(exception &ex)
                    {
                    }
                }
             }
        }
        return TC_Multi_HashMap::RT_OK;
    }

    /**
     * 回写单条记录, 如果记录不存在, 则不做任何处理
     * @param mk
     * @param uk
     * 
     * @return int
     *          TC_Multi_HashMap::RT_NO_DATA: 没有数据
     *          TC_Multi_HashMap::RT_ONLY_KEY:只有Key
     *          TC_Multi_HashMap::RT_OK:获取数据成功
     *          TC_Multi_HashMap::RT_LOAD_DATA_ERR: load数据失败
     *          其他返回值: 错误
     */
    int sync(const MK &mk, const UK &uk)
    {
        Value v;
        int ret = get(mk, uk, v);

        if(ret == TC_Multi_HashMap::RT_OK)
        {
            if(_todo_of)
            {
                _todo_of->sync(v);
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
     *      TC_Multi_HashMap::RT_OK: 回写完毕了
     */
    int sync(time_t iNowTime)
    {
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            this->_t.sync();
        }

        while(true)
        {
            TC_Multi_HashMap::Value data;

            int ret;
            {
                TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
                ret = this->_t.sync(iNowTime, data);
                if(ret == TC_Multi_HashMap::RT_OK)
                {
                    return ret;
                }

                if(ret != TC_Multi_HashMap::RT_NEED_SYNC)
                {
                    continue;
                }
            }

            if(_todo_of)
            {
                MK mk;
                UK uk;
                V tv;

                tars::TarsInputStream<BufferReader> is;
                is.setBuffer(data._mkey.c_str(), data._mkey.length());
                mk.readFrom(is);

                is.setBuffer(data._data._key.c_str(), data._data._key.length());
                uk.readFrom(is);

                is.setBuffer(data._data._value.c_str(), data._data._value.length());
                tv.readFrom(is);

                typename ToDoFunctor::DataRecord stDataRecord;
                stDataRecord._mkey      = mk;
                stDataRecord._ukey      = uk;
                stDataRecord._value     = tv;
                stDataRecord._iVersion    = data._data._iVersion;
                stDataRecord._dirty     = data._data._dirty;
                stDataRecord._iSyncTime = data._data._synct;

                _todo_of->sync(stDataRecord);
            }
        }

        return TC_Multi_HashMap::RT_OK;
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
     * 将脏数据且一定时间没有回写的数据回写,只回写一个脏数据，目的是替代int sync(time_t iNowTime)
     * 方法，把由业务控制每次回写数据量，使用时应该先调用void sync()
     * 
     * 数据回写时间与当前时间超过_pHead->_iSyncTime(setSyncTime)则需要回写

     * map只读时仍然可以回写
     * 
     * @param iNowTime: 回写到什么时间, 通常是当前时间
     * @return int:
     *      TC_Multi_HashMap::RT_OK: 回写完毕了
     * 
     * 示例：
     *      p->sync();
     *      while(true) {
     *          int iRet = pthis->SyncOnce(tNow);
     *          if( iRet == TC_Multi_HashMap::RT_OK )
     *                break;
     *        }
     */
    int syncOnce(time_t iNowTime)
    {
        TC_Multi_HashMap::Value data;

        int ret;
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.sync(iNowTime, data);
            if(ret == TC_Multi_HashMap::RT_OK)
            {
                return ret;
            }

            if(ret != TC_Multi_HashMap::RT_NEED_SYNC)
            {
                return ret;
            }
        }

        if(_todo_of)
        {
            MK mk;
            UK uk;
            V tv;

            tars::TarsInputStream<BufferReader> is;
            is.setBuffer(data._mkey.c_str(), data._mkey.length());
            mk.readFrom(is);

            is.setBuffer(data._data._key.c_str(), data._data._key.length());
            uk.readFrom(is);

            is.setBuffer(data._data._value.c_str(), data._data._value.length());
            tv.readFrom(is);

            typename ToDoFunctor::DataRecord stDataRecord;
            stDataRecord._mkey      = mk;
            stDataRecord._ukey      = uk;
            stDataRecord._value     = tv;
            stDataRecord._iVersion    = data._data._iVersion;
            stDataRecord._dirty     = data._data._dirty;
            stDataRecord._iSyncTime = data._data._synct;

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
     *      TC_Multi_HashMap::RT_OK: 备份OK了
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
            TC_Multi_HashMap::Value data;

            int ret;
            {
                TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
                ret = this->_t.backup(data);
                if(ret == TC_Multi_HashMap::RT_OK)
                {
                    return ret;
                }

                if(ret != TC_Multi_HashMap::RT_NEED_BACKUP)
                {
                    continue;
                }
            }

            if(_todo_of)
            {
                MK mk;
                UK uk;
                V tv;

                tars::TarsInputStream<BufferReader> is;
                is.setBuffer(data._mkey.c_str(), data._mkey.length());
                mk.readFrom(is);

                is.setBuffer(data._data._key.c_str(), data._data._key.length());
                uk.readFrom(is);

                is.setBuffer(data._data._value.c_str(), data._data._value.length());
                tv.readFrom(is);

                typename ToDoFunctor::DataRecord stDataRecord;
                stDataRecord._mkey      = mk;
                stDataRecord._ukey        = uk;
                stDataRecord._value     = tv;
                stDataRecord._iVersion    = data._data._iVersion;
                stDataRecord._dirty     = data._data._dirty;
                stDataRecord._iSyncTime = data._data._synct;

                _todo_of->backup(stDataRecord);
            }
        }

        return TC_Multi_HashMap::RT_OK;
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
     * 根据联合主键(MK+UK)查找数据
     * @param mk
     * @param uk
     * @return lock_interator
     *        返回end()表示没有查到
     */
    lock_iterator find(const MK &mk, const UK &uk)
    {
        tars::TarsOutputStream<BufferWriter> mos;
        mk.writeTo(mos);
        string smk(mos.getBuffer(), mos.getLength());

        tars::TarsOutputStream<BufferWriter> uos;
        uk.writeTo(uos);
        string suk(uos.getBuffer(), uos.getLength());

        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.find(smk, suk), jlock);
    }

    /**
    * 查找主key下所有数据数量
    * @param mk, 主key
    *
    * @return size_t, 主key下的记录数
    */
    size_t count(const MK &mk)
    {
        tars::TarsOutputStream<BufferWriter> os;
        mk.writeTo(os);
        string smk(os.getBuffer(), os.getLength());

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.count(smk);
    }

    /**
    * 根据主key查找第一个数据的位置
    * 与上面的count函数组合可以遍历所有主key下的数据
    * 也可以直接使用迭代器，直到end
    * @param mk, 主key
    * @return lock_iterator, 返回end()表示没有数据
    */
    lock_iterator find(const MK& mk)
    {
        tars::TarsOutputStream<BufferWriter> os;
        mk.writeTo(os);
        string smk(os.getBuffer(), os.getLength());
        
        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.find(smk), jlock);
    }

    /**
    * 判断主key是否存在
    * @param mk, 主key
    *
    * @return int
    *        TC_Multi_HashMap::RT_OK, 主key存在，且有数据
    *        TC_Multi_HashMap::RT_ONLY_KEY, 主key存在，没有数据
    *        TC_Multi_HashMap::RT_PART_DATA, 主key存在，里面的数据可能不完整
    *        TC_Multi_HashMap::RT_NO_DATA, 主key不存在
    */
    int checkMainKey(const MK& mk)
    {
        tars::TarsOutputStream<BufferWriter> os;
        mk.writeTo(os);
        string smk(os.getBuffer(), os.getLength());
        
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.checkMainKey(smk);
    }

    /**
    * 设置主key下数据的完整性
    * @param mk, 主key
    * @param bFull, true为完整数据，false为不完整数据
    *
    * @return
    *          RT_READONLY: 只读
    *          RT_NO_DATA: 没有当前数据
    *          RT_OK: 设置成功
    *          其他返回值: 错误
    */
    int setFullData(const MK &mk, bool bFull)
    {
        tars::TarsOutputStream<BufferWriter> os;
        mk.writeTo(os);
        string smk(os.getBuffer(), os.getLength());
        
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.setFullData(smk, bFull);
    }

    /**
    * 检查坏block，并可进行修复
    * @param bRepaire, 是否进行修复
    *
    * @return size_t, 返回坏数据个数
    */
    size_t checkBadBlock(bool bRepair)
    {
        size_t c     = this->_t.getHashCount();
        size_t e     = 0;
        for(size_t i = 0; i < c; i++)
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());

            e += this->_t.checkBadBlock(i, bRepair);
        }

        return e;
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

protected:

    /**
     * 删除数据的函数对象
     */
    ToDoFunctor                 *_todo_of;
};

}

#endif
