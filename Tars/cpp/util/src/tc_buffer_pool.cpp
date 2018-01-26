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

#include "util/tc_buffer_pool.h"

#include <cassert>

#include <sstream>
#include <iomanip>

inline static std::size_t RoundUp2Power(std::size_t size)
{
    if (size == 0)
        return 0;

    size_t roundUp = 1;
    while (roundUp < size)
        roundUp *= 2;

    return roundUp;
}


namespace tars
{

TC_Slice::TC_Slice(void* d, size_t dl, size_t l) :
    data(d),
    dataLen(dl),
    len(l)
{
}

TC_BufferPool::TC_BufferPool(size_t minBlock, size_t maxBlock) :
    _minBlock(RoundUp2Power(minBlock)),
    _maxBlock(RoundUp2Power(maxBlock)),
    _maxBytes(1024 * 1024), // pool所分配的内存不能超过它，否则直接new和delete，避免占用过高内存
    _totalBytes(0) // pool中的内存
{
    /*
     * minBlock表示该pool所负责的最小内存分配，向上取整，比如20字节，取整为32
     * maxBlock表示该pool所负责的最大内存分配，向上取整，比如1000字节，取整为1024
     * listCount是计算buffer链表的数量，比如minBlock是32，maxBlock是64，那么只需要两个链表
     */
    size_t listCount = 0;
    size_t testVal = _minBlock;
    while (testVal <= _maxBlock)
    {
        testVal *= 2;
        ++ listCount;
    }

    assert (listCount > 0);

    _buffers.resize(listCount);
}

TC_BufferPool::~TC_BufferPool()
{
    std::vector<BufferList>::iterator it(_buffers.begin());
    for (; it != _buffers.end(); ++ it)
    {
        BufferList& blist = *it;
        BufferList::iterator bit(blist.begin());
        for ( ; bit != blist.end(); ++ bit)
        {
            //delete[] (*bit);
            delete[] reinterpret_cast<char*>(*bit);
        }
    }
}

TC_Slice TC_BufferPool::Allocate(size_t size)
{
    TC_Slice s;
    size = RoundUp2Power(size);
    if (size == 0)
        return s;

    if (size < _minBlock || size > _maxBlock) 
    {
        // 不归pool管理，直接new
        s.data = new char[size];
        s.len = size;
    }
    else
    {
        // 定位到具体的buffer链表
        BufferList& blist = _GetBufferList(size); 
        s = _Allocate(size, blist);
    }

    return s;
}

void TC_BufferPool::Deallocate(TC_Slice s)
{
    if (s.len < _minBlock || s.len > _maxBlock) 
    {
        // 不归pool管理，直接delete
        delete[] reinterpret_cast<char*>(s.data);
    }
    else if (_totalBytes >= _maxBytes)
    {
        // 占用内存过多，就不还给pool
        delete[] reinterpret_cast<char*>(s.data);
    }
    else
    {
        // 还给pool
        BufferList& blist = _GetBufferList(s.len);
        blist.push_back(s.data);
        _totalBytes += s.len;
    }
}

void TC_BufferPool::SetMaxBytes(size_t bytes)
{
    _maxBytes = bytes;
}

size_t TC_BufferPool::GetMaxBytes() const
{
    return _maxBytes;
}

std::string TC_BufferPool::DebugPrint() const
{
    std::ostringstream oss;

    oss << "\n===============================================================\n";
    oss << "============  BucketCount " << std::setiosflags(std::ios::left) << std::setw(4) << _buffers.size() << " ================================" << std::endl;
    oss << "============  PoolBytes " << std::setw(10) << _totalBytes << " ============================" << std::endl;

    int bucket = 0;
    size_t size = _minBlock;
    std::vector<BufferList>::const_iterator it(_buffers.begin());
    for (; it != _buffers.end(); ++ it)
    {
        const BufferList& blist = *it;
        oss << "== Bucket " << std::setw(3) << bucket
            << ": BlockSize " << std::setw(8) << size
            << " Remain blocks " << std::setw(6) << blist.size()
            << " ======== \n";

        ++ bucket;
        size *= 2;
    }

    return oss.str();
}


TC_Slice TC_BufferPool::_Allocate(size_t size, BufferList& blist)
{
    assert ((size & (size - 1)) == 0);

    TC_Slice s;
    s.len = size;

    if (blist.empty())
    {
        s.data = new char[size];
    }
    else
    {
        // 直接从链表中取出buffer
        s.data = *blist.begin();
        blist.pop_front();
        _totalBytes -= s.len;
    }

    return s;
}

TC_BufferPool::BufferList& TC_BufferPool::_GetBufferList(size_t s)
{
    const BufferList& blist = const_cast<const TC_BufferPool& >(*this)._GetBufferList(s);
    return const_cast<BufferList& >(blist);
}

const TC_BufferPool::BufferList& TC_BufferPool::_GetBufferList(size_t s) const
{
    assert ((s & (s - 1)) == 0);
    assert (s >= _minBlock && s <= _maxBlock);

    size_t index = _buffers.size();
    size_t testVal = s;
    while (testVal <= _maxBlock)
    {
        testVal *= 2;
        index --;
    }

    return _buffers[index];
}

} // end namespace tars

