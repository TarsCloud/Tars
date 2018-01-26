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

#include "util/tc_mem_chunk.h"
#include <cassert>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <string.h>

namespace tars
{

TC_MemChunk::TC_MemChunk()
: _pHead(NULL)
, _pData(NULL)
{
}

size_t TC_MemChunk::calcMemSize(size_t iBlockSize, size_t iBlockCount)
{
    return iBlockSize * iBlockCount + sizeof(tagChunkHead);
}

size_t TC_MemChunk::calcBlockCount(size_t iMemSize, size_t iBlockSize)
{
    if(iMemSize <= sizeof(tagChunkHead))
    {
        return 0;
    }

    return min(((iMemSize - sizeof(tagChunkHead)) / iBlockSize), (size_t)(-1));
}

void TC_MemChunk::create(void *pAddr, size_t iBlockSize, size_t iBlockCount)
{
    assert(iBlockSize > sizeof(size_t));

    init(pAddr);

    _pHead->_iBlockSize             = iBlockSize;
    _pHead->_iBlockCount            = iBlockCount;
    _pHead->_firstAvailableBlock    = 0;
    _pHead->_blockAvailable         = iBlockCount;

    memset(_pData, 0x00, iBlockCount*iBlockSize);

    unsigned char *pt               = _pData;
    for(size_t i = 0; i != iBlockCount; pt+= iBlockSize)
    {
        ++i;
        memcpy(pt, &i, sizeof(size_t)); //每块第一个字节直接指向下一个可用的block编号, 变成一个链表
    }
}

void TC_MemChunk::connect(void *pAddr)
{
    init(pAddr);
}

void TC_MemChunk::init(void *pAddr)
{
    _pHead = static_cast<tagChunkHead*>(pAddr);
    _pData = (unsigned char*)((char*)_pHead + sizeof(tagChunkHead));
}

void* TC_MemChunk::allocate()
{
    if(!isBlockAvailable()) return NULL;

    unsigned char *result        = _pData + (_pHead->_firstAvailableBlock * _pHead->_iBlockSize);

    --_pHead->_blockAvailable;

    _pHead->_firstAvailableBlock = *((size_t *)result);

    memset(result, 0x00, sizeof(_pHead->_iBlockSize));

    return result;
}

void* TC_MemChunk::allocate2(size_t &iIndex)
{
    iIndex = _pHead->_firstAvailableBlock + 1;

    void *pAddr = allocate();

    if(pAddr == NULL)
    {
        iIndex = 0;
        return NULL;
    }

    return pAddr;
}

void TC_MemChunk::deallocate(void *pAddr)
{
    assert(pAddr >= _pData);

    unsigned char* prelease = static_cast<unsigned char*>(pAddr);

    assert((prelease - _pData) % _pHead->_iBlockSize == 0);

    memset(pAddr, 0x00, _pHead->_iBlockSize);

    *((size_t *)prelease) = _pHead->_firstAvailableBlock;

    _pHead->_firstAvailableBlock = static_cast<size_t>((prelease - _pData)/_pHead->_iBlockSize);

    assert(_pHead->_firstAvailableBlock == (prelease - _pData)/_pHead->_iBlockSize);

    ++_pHead->_blockAvailable;
}

void TC_MemChunk::deallocate2(size_t iIndex)
{
    assert(iIndex > 0 && iIndex <= _pHead->_iBlockCount);

    void *pAddr = _pData + (iIndex - 1) * _pHead->_iBlockSize;

    deallocate(pAddr);
}

void* TC_MemChunk::getAbsolute(size_t iIndex)
{
    assert(iIndex > 0 && iIndex <= _pHead->_iBlockCount);

    void* pAddr = _pData + (iIndex - 1) * _pHead->_iBlockSize;

    return pAddr;
}

size_t TC_MemChunk::getRelative(void *pAddr)
{
    assert((char*)pAddr >= (char*)_pData && ((char*)pAddr <= (char*)_pData + _pHead->_iBlockSize * _pHead->_iBlockCount));
    assert(((char*)pAddr - (char*)_pData) % _pHead->_iBlockSize == 0);

    return 1 + ((char*)pAddr - (char*)_pData) / _pHead->_iBlockSize;
}

void TC_MemChunk::rebuild()
{
    assert(_pHead);

    _pHead->_firstAvailableBlock    = 0;
    _pHead->_blockAvailable         = _pHead->_iBlockCount;

    memset(_pData, 0x00, _pHead->_iBlockCount*_pHead->_iBlockSize);

    unsigned char *pt               = _pData;
    for(size_t i = 0; i != _pHead->_iBlockCount; pt+= _pHead->_iBlockSize)
    {
        ++i;
        memcpy(pt, &i, sizeof(size_t)); //每块第一个直接指向下一个可用的block编号, 变成一个链表
    }
}

TC_MemChunk::tagChunkHead TC_MemChunk::getChunkHead() const
{
    return *_pHead;
}

////////////////////////////////////////////////////////////////////
//

TC_MemChunkAllocator::TC_MemChunkAllocator()
: _pHead(NULL),_pChunk(NULL)
{
}

void TC_MemChunkAllocator::init(void *pAddr)
{
    _pHead  = static_cast<tagChunkAllocatorHead*>(pAddr);
    _pChunk = (char*)_pHead + sizeof(tagChunkAllocatorHead);
}

void TC_MemChunkAllocator::initChunk()
{
    assert(_pHead->_iSize > sizeof(tagChunkAllocatorHead));

    size_t iChunkCapacity = _pHead->_iSize - sizeof(tagChunkAllocatorHead);

    assert(iChunkCapacity > TC_MemChunk::getHeadSize());

    size_t  iBlockCount   = (iChunkCapacity - TC_MemChunk::getHeadSize()) / _pHead->_iBlockSize;

    assert(iBlockCount > 0);

    _chunk.create((void*)((char *)_pChunk), _pHead->_iBlockSize, iBlockCount);
}

void TC_MemChunkAllocator::create(void *pAddr, size_t iSize, size_t iBlockSize)
{
    init(pAddr);

    _pHead->_iSize       = iSize;
    _pHead->_iBlockSize  = iBlockSize;

    initChunk();
}

void TC_MemChunkAllocator::connectChunk()
{
    assert(_pHead->_iSize > sizeof(tagChunkAllocatorHead));

    size_t iChunkCapacity = _pHead->_iSize - sizeof(tagChunkAllocatorHead);

    assert(iChunkCapacity > TC_MemChunk::getHeadSize());

    _chunk.connect((void*)((char *)_pChunk));
}

void TC_MemChunkAllocator::connect(void *pAddr)
{
    init(pAddr);

    connectChunk();
}

void TC_MemChunkAllocator::rebuild()
{
    _chunk.rebuild();
}

void* TC_MemChunkAllocator::allocate()
{
    return _chunk.allocate();
}

void* TC_MemChunkAllocator::allocate2(size_t &iIndex)
{
    return _chunk.allocate2(iIndex);
}

void TC_MemChunkAllocator::deallocate(void *pAddr)
{
    assert(pAddr >= _pChunk);

    _chunk.deallocate(pAddr);
}

void TC_MemChunkAllocator::deallocate2(size_t iIndex)
{
    _chunk.deallocate2(iIndex);
}

TC_MemChunk::tagChunkHead TC_MemChunkAllocator::getBlockDetail() const
{
    return _chunk.getChunkHead();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
TC_MemMultiChunkAllocator::TC_MemMultiChunkAllocator()
: _pHead(NULL), _pChunk(NULL),_iBlockCount(0),_iAllIndex(0), _nallocator(NULL)
{
}

TC_MemMultiChunkAllocator::~TC_MemMultiChunkAllocator()
{
    clear();
}

void TC_MemMultiChunkAllocator::clear()
{
    for(size_t i = 0; i < _allocator.size(); i++)
    {
        delete _allocator[i];
    }
    _allocator.clear();

    if(_nallocator)
    {
        _nallocator->clear();
        delete _nallocator;
        _nallocator = NULL;
    }

    _vBlockSize.clear();

    _pHead = NULL;
    _pChunk = NULL;
    _iBlockCount = 0;
    _iAllIndex = 0;
}

vector<size_t> TC_MemMultiChunkAllocator::getBlockSize()  const    
{ 
    vector<size_t> v = _vBlockSize;
    if(_nallocator)
    {
        vector<size_t> vNext = _nallocator->getBlockSize();
        copy(vNext.begin(), vNext.end(), inserter(v, v.end()));
    }

    return v; 
}

size_t TC_MemMultiChunkAllocator::getCapacity() const
{
    size_t iCapacity = 0;
    for(size_t i = 0; i < _allocator.size(); i++)
    {
        iCapacity += _allocator[i]->getCapacity();
    }

    if(_nallocator)
    {
        iCapacity += _nallocator->getCapacity();
    }

    return iCapacity;
}

vector<TC_MemChunk::tagChunkHead> TC_MemMultiChunkAllocator::getBlockDetail() const
{
    vector<TC_MemChunk::tagChunkHead> vt;
    for(size_t i = 0; i < _allocator.size(); i++)
    {
        vt.push_back(_allocator[i]->getBlockDetail());
    }

    if(_nallocator)
    {
        vector<TC_MemChunk::tagChunkHead> v = _nallocator->getBlockDetail();

        copy(v.begin(), v.end(), inserter(vt, vt.end()));
    }

    return vt;
}

vector<size_t> TC_MemMultiChunkAllocator::singleBlockChunkCount() const                
{ 
    vector<size_t> vv;
    vv.push_back(_iBlockCount);

    if(_nallocator)
    {
        vector<size_t> v = _nallocator->singleBlockChunkCount();
        copy(v.begin(), v.end(), inserter(vv, vv.end()));
    }

    return vv;
}

size_t TC_MemMultiChunkAllocator::allBlockChunkCount() const                   
{ 
    size_t n = _iBlockCount * _vBlockSize.size(); 

    if(_nallocator)
    {
        n += _nallocator->allBlockChunkCount();
    }

    return n;
}

void TC_MemMultiChunkAllocator::init(void *pAddr)
{
    _pHead  = static_cast<tagChunkAllocatorHead*>(pAddr);
    _pChunk = (char*)_pHead + sizeof(tagChunkAllocatorHead);
}

void TC_MemMultiChunkAllocator::calc()
{
    _vBlockSize.clear();

    //每种block大小总和
    size_t sum = 0;
    for(size_t n = _pHead->_iMinBlockSize; n < _pHead->_iMaxBlockSize; )
    {
        sum += n;
        _vBlockSize.push_back(n);

        if(_pHead->_iMaxBlockSize > _pHead->_iMinBlockSize)
        {
            n = max((size_t)(n*_pHead->_fFactor), n+1);    //至少内存块大小要+1
        }
    }

    sum += _pHead->_iMaxBlockSize;
    _vBlockSize.push_back(_pHead->_iMaxBlockSize);

    assert(_pHead->_iSize > (TC_MemMultiChunkAllocator::getHeadSize() + TC_MemChunkAllocator::getHeadSize()*_vBlockSize.size() + TC_MemChunk::getHeadSize()*_vBlockSize.size()));

    //计算块的个数
    _iBlockCount = (_pHead->_iSize
                    - TC_MemMultiChunkAllocator::getHeadSize()
                    - TC_MemChunkAllocator::getHeadSize() * _vBlockSize.size()
                    - TC_MemChunk::getHeadSize() * _vBlockSize.size())/sum;

    assert(_iBlockCount >= 1);
}

void TC_MemMultiChunkAllocator::create(void *pAddr, size_t iSize, size_t iMinBlockSize, size_t iMaxBlockSize, float fFactor)
{
    assert(iMaxBlockSize >= iMinBlockSize);
    assert(fFactor >= 1.0);

    init(pAddr);

    _pHead->_iSize          = iSize;
    _pHead->_iTotalSize     = iSize;
    _pHead->_iMinBlockSize  = iMinBlockSize;
    _pHead->_iMaxBlockSize  = iMaxBlockSize;
    _pHead->_fFactor        = fFactor;
    _pHead->_iNext          = 0;

    calc();

    //初始化每种快的分配器
    char *pChunkBegin = (char*)_pChunk;
    for(size_t i = 0; i < _vBlockSize.size(); i++)
    {
        TC_MemChunkAllocator *p = new TC_MemChunkAllocator;
        size_t iAllocSize = TC_MemChunkAllocator::getHeadSize() + TC_MemChunk::calcMemSize(_vBlockSize[i], _iBlockCount);
        p->create(pChunkBegin, iAllocSize, _vBlockSize[i]);
        pChunkBegin += iAllocSize;
        _allocator.push_back(p);
    }

    _iAllIndex = _allocator.size() * getBlockCount();
}

void TC_MemMultiChunkAllocator::connect(void *pAddr)
{
    clear();

    init(pAddr);

    calc();

    //初始化每种块的分配器
    char *pChunkBegin = (char*)_pChunk;
    for(size_t i = 0; i < _vBlockSize.size(); i++)
    {
        TC_MemChunkAllocator *p = new TC_MemChunkAllocator;

        p->connect(pChunkBegin);
        pChunkBegin += TC_MemChunkAllocator::getHeadSize() + TC_MemChunk::calcMemSize(_vBlockSize[i], _iBlockCount);
        _allocator.push_back(p);
    }

    _iAllIndex = _allocator.size() * getBlockCount();

    //没有后续的空间了
    if(_pHead->_iNext == 0)
    {
        return;
    }

    assert(_pHead->_iNext == _pHead->_iSize);
    assert(_nallocator == NULL);

    //下一块地址, 注意这里是嵌套的, 扩展分配空间的时候注意 
    tagChunkAllocatorHead   *pNextHead = (tagChunkAllocatorHead   *)((char*)_pHead + _pHead->_iNext);
    _nallocator = new TC_MemMultiChunkAllocator();
    _nallocator->connect(pNextHead);
}

void TC_MemMultiChunkAllocator::rebuild()
{
    for(size_t i = 0; i < _allocator.size(); i++)
    {
        _allocator[i]->rebuild();
    }

    if(_nallocator)
    {
        _nallocator->rebuild();
    }
}

TC_MemMultiChunkAllocator *TC_MemMultiChunkAllocator::lastAlloc()
{
    if(_nallocator == NULL)
        return NULL;

    TC_MemMultiChunkAllocator *p = _nallocator;

    while(p && p->_nallocator)
    {
        p = p->_nallocator;
    }
    
    return p;
}

void TC_MemMultiChunkAllocator::append(void *pAddr, size_t iSize)
{
    connect(pAddr);

    //扩展后的空间地址一定需要>开始的空间
    assert(iSize > _pHead->_iTotalSize);

    //扩展空间部分的真实起始地址
    void *pAppendAddr = (char*)pAddr + _pHead->_iTotalSize;

    //扩展的部分初始化, 注意这里p不用delete, 最后系统的时候会循环delete所有分配器
    TC_MemMultiChunkAllocator *p = new TC_MemMultiChunkAllocator();
    p->create(pAppendAddr, iSize - _pHead->_iTotalSize, _pHead->_iMinBlockSize, _pHead->_iMaxBlockSize, _pHead->_fFactor);

    //扩展部分连接到最后一个分配块最后
    TC_MemMultiChunkAllocator *palloc = lastAlloc();
    if(palloc)
    {
        palloc->_pHead->_iNext  = (char*)pAppendAddr - (char*)palloc->_pHead;
        palloc->_nallocator     = p;
    }
    else
    {
        _pHead->_iNext  = (char*)pAppendAddr - (char*)_pHead;
        _nallocator     = p;
    }

    assert(_pHead->_iNext == _pHead->_iSize);

    //总计大小
    _pHead->_iTotalSize = iSize;
}

void* TC_MemMultiChunkAllocator::allocate(size_t iNeedSize, size_t &iAllocSize)
{
    size_t iIndex;
    return allocate2(iNeedSize, iAllocSize, iIndex);
}

void *TC_MemMultiChunkAllocator::allocate2(size_t iNeedSize, size_t &iAllocSize, size_t &iIndex)
{
    void *p = NULL;
    iIndex  = 0;

    for(size_t i = 0; i < _allocator.size(); i++)
    {
        //首先分配大小刚刚超过的
        if(_allocator[i]->getBlockSize() >= iNeedSize)
        {
            size_t n;
            p = _allocator[i]->allocate2(n);
            if(p != NULL)
            {
                iAllocSize = _vBlockSize[i];
                iIndex     += i * getBlockCount() + n;
                return p;
            }
        }
    }

    //没有比数据更大的数据块了
    for(size_t i = _allocator.size(); i != 0 ; i--)
    {
        //首先分配大小刚刚小于的
        if(_allocator[i-1]->getBlockSize() < iNeedSize)
        {
            size_t n;
            p = _allocator[i-1]->allocate2(n);
            if(p != NULL)
            {
                iAllocSize = _vBlockSize[i-1];
                iIndex     += (i - 1) * getBlockCount() + n;
                return p;
            }
        }
    }

    //后续的扩展块分配空间, 注意对象是递归的, 因此只需要分配一个块就可以了
    if(_nallocator)
    {
        p = _nallocator->allocate2(iNeedSize, iAllocSize, iIndex);
        if(p != NULL)
        {
            iIndex     += _iAllIndex;
            return p;
        }
    }

    return NULL;
}

void TC_MemMultiChunkAllocator::deallocate(void *pAddr)
{
    if(pAddr < (void*)((char*)_pHead + _pHead->_iSize))
    {
        char *pChunkBegin = (char*)_pChunk;

        for(size_t i = 0; i < _vBlockSize.size(); i++)
        {
            pChunkBegin += _allocator[i]->getMemSize();
//            pChunkBegin += TC_MemChunkAllocator::getHeadSize() + TC_MemChunk::calcMemSize(_vBlockSize[i], _iBlockCount);

            if((char*)pAddr < pChunkBegin)
            {
                _allocator[i]->deallocate(pAddr);
                return;
            }
        }

        assert(false);
    }
    else
    {
        if(_nallocator)
        {
            _nallocator->deallocate(pAddr);
            return;
        }
    }

    assert(false);
}

void TC_MemMultiChunkAllocator::deallocate2(size_t iIndex)
{
    for(size_t i = 0; i < _allocator.size(); i++)
    {
        if(iIndex <= getBlockCount())
        {
            _allocator[i]->deallocate2(iIndex);
            return;
        }
        iIndex -= getBlockCount();
    }
    
    if(_nallocator)
    {
        _nallocator->deallocate2(iIndex);
        return;
    }
    
    assert(false);
}

void* TC_MemMultiChunkAllocator::getAbsolute(size_t iIndex)
{
    if(iIndex == 0)
    {
        return NULL;
    }

    size_t n = _iAllIndex;
    if(iIndex <= n)
    {
        size_t i = (iIndex - 1) / getBlockCount();
        iIndex   -= i * getBlockCount();
        return _allocator[i]->getAbsolute(iIndex);
    }
    else
    {
        iIndex -= n;
    }

    if(_nallocator)
    {
        return _nallocator->getAbsolute(iIndex);
    }

    assert(false);

    return NULL;
}

size_t TC_MemMultiChunkAllocator::getRelative(void *pAddr)
{
    if(pAddr == NULL)
    {
        return 0;
    }

    if(pAddr < (char*)_pHead + _pHead->_iSize)
    {
        for(size_t i = 0; i < _vBlockSize.size(); i++)
        {
            if((char*)pAddr < ((char*)_allocator[i]->getHead() + _allocator[i]->getMemSize()))
            {
                //注意:索引从1开始计数
                return i * getBlockCount() + _allocator[i]->getRelative(pAddr);
            }
        }
        assert(false);
    }
    else
    {
        if(_nallocator)
        {
            return _iAllIndex + _nallocator->getRelative((char*)pAddr);
        }
    }
    
    return 0;
}

}
