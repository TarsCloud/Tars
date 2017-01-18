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

#include "util/tc_multi_hashmap.h"
#include "util/tc_pack.h"
#include "util/tc_common.h"

namespace tars
{

int TC_Multi_HashMap::FailureRecover::_iRefCount = 0;

bool TC_Multi_HashMap::MainKey::next()
{
    _iHead = getHeadPtr()->_iNext;
    _pHead = _pMap->getAbsolute(_iHead);
        
    return _iHead != 0;
}
    
bool TC_Multi_HashMap::MainKey::prev()
{
    _iHead = getHeadPtr()->_iPrev;
    _pHead = _pMap->getAbsolute(_iHead);
    
    return _iHead != 0;
}

void TC_Multi_HashMap::MainKey::deallocate()
{
    if(HASNEXTCHUNK())
    {
        // 释放所有chunk
        deallocate(getHeadPtr()->_iNextChunk);
    }
    
    // 释放主key头
    vector<uint32_t> v;
    v.push_back(_iHead);
    _pMap->_pDataAllocator->deallocateMemChunk(v);
}

void TC_Multi_HashMap::MainKey::deallocate(uint32_t iChunk)
{
    vector<uint32_t> v;
    v.push_back(iChunk);

    tagChunkHead *pChunk        = getChunkHead(iChunk);
    
    //获取所有后续的chunk地址
    while(true)
    {
        if(pChunk->_bNextChunk)
        {
            v.push_back(pChunk->_iNextChunk);
            pChunk = getChunkHead(pChunk->_iNextChunk);
        }
        else
        {
            break;
        }
    }
    
    // 空间全部释放掉
    _pMap->_pDataAllocator->deallocateMemChunk(v);
}

void TC_Multi_HashMap::MainKey::makeNew(uint32_t iIndex, uint32_t iAllocSize)
{
    getHeadPtr()->_iSize        = iAllocSize;
    getHeadPtr()->_iIndex        = iIndex;
    getHeadPtr()->_iAddr        = 0;
    getHeadPtr()->_iNext        = 0;
    getHeadPtr()->_iPrev        = 0;
    getHeadPtr()->_iGetNext        = 0;
    getHeadPtr()->_iGetPrev        = 0;
    getHeadPtr()->_iDataLen     = 0;
    SETNEXTCHUNK(false);
    SETFULLDATA(false);

    _pMap->incMainKeyListCount(iIndex);
    
    // 挂到主key链上
    if(_pMap->itemMainKey(iIndex)->_iMainKeyAddr == 0)
    {
        //当前hash桶没有元素
        _pMap->saveValue(&_pMap->itemMainKey(iIndex)->_iMainKeyAddr, _iHead);
        _pMap->saveValue(&getHeadPtr()->_iNext, (uint32_t)0);
        _pMap->saveValue(&getHeadPtr()->_iPrev, (uint32_t)0);
    }
    else
    {
        //当前hash桶有元素, 挂在桶开头
        _pMap->saveValue(&getHeadPtr(_pMap->itemMainKey(iIndex)->_iMainKeyAddr)->_iPrev, _iHead);
        _pMap->saveValue(&getHeadPtr()->_iNext, _pMap->itemMainKey(iIndex)->_iMainKeyAddr);
        _pMap->saveValue(&_pMap->itemMainKey(iIndex)->_iMainKeyAddr, _iHead);
        _pMap->saveValue(&getHeadPtr()->_iPrev, (uint32_t)0);
    }
    
    //挂在Get链表头部
    if(_pMap->_pHead->_iGetHead == 0)
    {
        assert(_pMap->_pHead->_iGetTail == 0);
        _pMap->saveValue(&_pMap->_pHead->_iGetHead, _iHead);
        _pMap->saveValue(&_pMap->_pHead->_iGetTail, _iHead);
    }
    else
    {
        assert(_pMap->_pHead->_iGetTail != 0);
        _pMap->saveValue(&getHeadPtr()->_iGetNext, _pMap->_pHead->_iGetHead);
        _pMap->saveValue(&getHeadPtr(_pMap->_pHead->_iGetHead)->_iGetPrev, _iHead);
        _pMap->saveValue(&_pMap->_pHead->_iGetHead, _iHead);
    }
}

int TC_Multi_HashMap::MainKey::erase(vector<Value> &vtData)
{
    //////////////////修改备份数据链表/////////////
    if(_pMap->_pHead->_iBackupTail == _iHead)
    {
        _pMap->saveValue(&_pMap->_pHead->_iBackupTail, getHeadPtr()->_iGetPrev);
    }

    ////////////////////修改Get链表的数据//////////
    //
    {
        bool bHead = (_pMap->_pHead->_iGetHead == _iHead);
        bool bTail = (_pMap->_pHead->_iGetTail == _iHead);

        if(!bHead)
        {
            if(bTail)
            {
                assert(getHeadPtr()->_iGetNext == 0);
                //是尾部, 尾部指针指向上一个元素
                _pMap->saveValue(&_pMap->_pHead->_iGetTail, getHeadPtr()->_iGetPrev);
                _pMap->saveValue(&getHeadPtr(getHeadPtr()->_iGetPrev)->_iGetNext, (uint32_t)0);
            }
            else
            {
                //不是头部也不是尾部
                assert(getHeadPtr()->_iGetNext != 0);
                _pMap->saveValue(&getHeadPtr(getHeadPtr()->_iGetPrev)->_iGetNext, getHeadPtr()->_iGetNext);
                _pMap->saveValue(&getHeadPtr(getHeadPtr()->_iGetNext)->_iGetPrev, getHeadPtr()->_iGetPrev);
            }
        }
        else
        {
            if(bTail)
            {
                assert(getHeadPtr()->_iGetNext == 0);
                assert(getHeadPtr()->_iGetPrev == 0);
                //头部也是尾部, 指针都设置为0
                _pMap->saveValue(&_pMap->_pHead->_iGetHead, (uint32_t)0);
                _pMap->saveValue(&_pMap->_pHead->_iGetTail, (uint32_t)0);
            }
            else
            {
                //头部不是尾部, 头部指针指向下一个元素
                assert(getHeadPtr()->_iGetNext != 0);
                _pMap->saveValue(&_pMap->_pHead->_iGetHead, getHeadPtr()->_iGetNext);
                //下一个元素上指针为0
                _pMap->saveValue(&getHeadPtr(getHeadPtr()->_iGetNext)->_iGetPrev, (uint32_t)0);
            }
        }
    }

    ///////////////////删除主key下的所有block/////////////////////////////
    string mk;
    int ret = get(mk);
    if(ret != TC_Multi_HashMap::RT_OK)
    {
        // todo, 这里出错会有大问题，前面的修改都应该取消
        return ret;
    }

    while(getHeadPtr()->_iAddr != 0)
    {
        Block block(_pMap, getHeadPtr()->_iAddr);
        Value value;
        value._mkey = mk;
        int ret = block.getBlockData(value._data);
        if(ret == TC_Multi_HashMap::RT_OK)
        {
            vtData.push_back(value);
        }
        block.erase();
    }

    ///////////////////从主key链表中去掉///////////
    //
    //上一个主key指向下一个主key
    if(getHeadPtr()->_iPrev != 0)
    {
        _pMap->saveValue(&getHeadPtr(getHeadPtr()->_iPrev)->_iNext, getHeadPtr()->_iNext);
    }

    //下一个主key指向上一个
    if(getHeadPtr()->_iNext != 0)
    {
        _pMap->saveValue(&getHeadPtr(getHeadPtr()->_iNext)->_iPrev, getHeadPtr()->_iPrev);
    }

    //////////////////如果是hash头部, 需要修改hash索引数据指针//////
    //
    _pMap->delMainKeyListCount(getHeadPtr()->_iIndex);
    if(getHeadPtr()->_iPrev == 0)
    {
        //如果是hash桶的头部, 则还需要处理
        TC_Multi_HashMap::tagMainKeyHashItem *pItem  = _pMap->itemMainKey(getHeadPtr()->_iIndex);
        assert(pItem->_iMainKeyAddr == _iHead);
        if(pItem->_iMainKeyAddr == _iHead)
        {
            _pMap->saveValue(&pItem->_iMainKeyAddr, getHeadPtr()->_iNext);
        }
    }

    // 手工置空一些参数，使在数据恢复区建立记录，以便crash后恢复
    _pMap->saveValue(&getHeadPtr()->_iSize, (uint32_t)0);
    _pMap->saveValue(&getHeadPtr()->_iIndex, (uint32_t)0);
    _pMap->saveValue(&getHeadPtr()->_iBitset, (uint8_t)0);
    _pMap->saveValue(&getHeadPtr()->_iDataLen, (uint32_t)0);

    // 归还内存前先确认上述修改，因为内存被释放后是不能恢复的
    // 即使后面的内存释放失败也不会造成数据的破坏
    _pMap->doUpdate();

    //归还到内存中
    deallocate();

    return RT_OK;
}

void TC_Multi_HashMap::MainKey::refreshGetList()
{
    assert(_pMap->_pHead->_iGetHead != 0);
    assert(_pMap->_pHead->_iGetTail != 0);

    // 本身已经在头部，不需要处理
    if(_pMap->_pHead->_iGetHead == _iHead)
    {
        return;
    }
    
    uint32_t iPrev = getHeadPtr()->_iGetPrev;
    uint32_t iNext = getHeadPtr()->_iGetNext;
    
    assert(iPrev != 0);
    
    //是正在备份的数据
    if(_pMap->_pHead->_iBackupTail == _iHead)
    {
        _pMap->saveValue(&_pMap->_pHead->_iBackupTail, iPrev);
    }
    
    //挂在链表头部
    _pMap->saveValue(&getHeadPtr()->_iGetNext, _pMap->_pHead->_iGetHead);
    _pMap->saveValue(&getHeadPtr(_pMap->_pHead->_iGetHead)->_iGetPrev, _iHead);
    _pMap->saveValue(&_pMap->_pHead->_iGetHead, _iHead);
    _pMap->saveValue(&getHeadPtr()->_iGetPrev, (uint32_t)0);
    
    //上一个元素的Next指针指向下一个元素
    _pMap->saveValue(&getHeadPtr(iPrev)->_iGetNext, iNext);
    
    //下一个元素的Prev指向上一个元素
    if (iNext != 0)
    {
        _pMap->saveValue(&getHeadPtr(iNext)->_iGetPrev, iPrev);
    }
    else
    {
        //改变尾部指针
        assert(_pMap->_pHead->_iGetTail == _iHead);
        _pMap->saveValue(&_pMap->_pHead->_iGetTail, iPrev);
    }
}

int TC_Multi_HashMap::MainKey::allocate(uint32_t iDataLen, vector<TC_Multi_HashMap::Value> &vtData)
{
    uint32_t fn   = 0;
    //一个块的真正的数据容量
    fn = getHeadPtr()->_iSize - sizeof(tagMainKeyHead);
    if(fn >= iDataLen)
    {
        //一个chunk就可以了, 后续的chunk都要释放掉
        if(HASNEXTCHUNK())
        {
            uint32_t iNextChunk = getHeadPtr()->_iNextChunk;
            //先修改自己的区块
            _pMap->saveValue(&getHeadPtr()->_iBitset, NEXTCHUNK_BIT, false);
            _pMap->saveValue(&getHeadPtr()->_iDataLen, (uint32_t)0);
            //修改成功后再释放区块, 从而保证, 不会core的时候导致整个内存块不可用
            deallocate(iNextChunk);
        }
        return TC_Multi_HashMap::RT_OK;
    }

    //计算还需要多少长度
    fn = iDataLen - fn;

    if(HASNEXTCHUNK())
    {
        tagChunkHead *pChunk    = getChunkHead(getHeadPtr()->_iNextChunk);

        while(true)
        {
            if(fn <= (pChunk->_iSize - sizeof(tagChunkHead)))
            {
                //已经不需要chunk了, 释放多余的chunk
                if(pChunk->_bNextChunk)
                {
                    uint32_t iNextChunk = pChunk->_iNextChunk;
                    _pMap->saveValue(&pChunk->_bNextChunk, (uint8_t)false);
                    //一旦异常core, 最坏的情况就是少了可用的区块, 但是整个内存结构还是可用的
                    deallocate(iNextChunk);
                }
                return TC_Multi_HashMap::RT_OK;
            }

            //计算, 还需要多少长度
            fn -= pChunk->_iSize - sizeof(tagChunkHead);

            if(pChunk->_bNextChunk)
            {
                pChunk  =  getChunkHead(pChunk->_iNextChunk);
            }
            else
            {
                //没有后续chunk了, 需要新分配fn的空间
                vector<uint32_t> chunks;
                int ret = allocateChunk(fn, chunks, vtData);
                if(ret != TC_Multi_HashMap::RT_OK)
                {
                    //没有内存可以分配
                    return ret;
                }

                _pMap->saveValue(&pChunk->_bNextChunk, (uint8_t)true);
                _pMap->saveValue(&pChunk->_iNextChunk, chunks[0]);
                //chunk指向分配的第一个chunk
                pChunk  =  getChunkHead(chunks[0]);
                //修改第一个chunk的属性, 保证异常core的时候, chunk链表不会有问题
                _pMap->saveValue(&pChunk->_bNextChunk, (uint8_t)false);
                _pMap->saveValue(&pChunk->_iDataLen, (uint32_t)0);

                //连接每个chunk
                return joinChunk(pChunk, chunks);
            }
        }
    }
    else
    {
        //没有后续chunk了, 需要新分配fn空间
        vector<uint32_t> chunks;
        int ret = allocateChunk(fn, chunks, vtData);
        if(ret != TC_Multi_HashMap::RT_OK)
        {
            //没有内存可以分配
            return ret;
        }

        _pMap->saveValue(&getHeadPtr()->_iBitset, NEXTCHUNK_BIT, true);
        _pMap->saveValue(&getHeadPtr()->_iNextChunk, chunks[0]);
        //chunk指向分配的第一个chunk
        tagChunkHead *pChunk = getChunkHead(chunks[0]);
        //修改第一个chunk的属性, 保证异常core的时候, chunk链表不会有问题
        _pMap->saveValue(&pChunk->_bNextChunk, (uint8_t)false);
        _pMap->saveValue(&pChunk->_iDataLen, (uint32_t)0);

        //连接每个chunk
        return joinChunk(pChunk, chunks);
    }

    return TC_Multi_HashMap::RT_OK;
}

int TC_Multi_HashMap::MainKey::joinChunk(tagChunkHead *pChunk, const vector<uint32_t>& chunks)
{
    for (size_t i = 0; i < chunks.size(); ++i)
    {
        if (i == chunks.size() - 1)
        {
            _pMap->saveValue(&pChunk->_bNextChunk, (uint8_t)false);
            return TC_Multi_HashMap::RT_OK;
        }
        else
        {
            _pMap->saveValue(&pChunk->_bNextChunk, (uint8_t)true);
            _pMap->saveValue(&pChunk->_iNextChunk, chunks[i+1]);
            pChunk  =  getChunkHead(chunks[i+1]);
            _pMap->saveValue(&pChunk->_bNextChunk, (uint8_t)false);
            _pMap->saveValue(&pChunk->_iDataLen, (uint32_t)0);
        }
    }

    return TC_Multi_HashMap::RT_OK;
}

int TC_Multi_HashMap::MainKey::allocateChunk(uint32_t fn, vector<uint32_t> &chunks, vector<TC_Multi_HashMap::Value> &vtData)
{
    assert(fn > 0);

    while(true)
    {
        uint32_t iAllocSize = fn;
        // 分配空间
        // 分配过程中可能会淘汰数据，第一个参数是当前的主key头地址，不能被淘汰
        uint32_t t = _pMap->_pDataAllocator->allocateChunk(_iHead, iAllocSize, vtData);
        if (t == 0)
        {
            //没有内存可以分配了, 先把分配的归还
            _pMap->_pDataAllocator->deallocateMemChunk(chunks);
            chunks.clear();
            return TC_Multi_HashMap::RT_NO_MEMORY;
        }

        //设置分配的数据块的大小
        getChunkHead(t)->_iSize = iAllocSize;

        chunks.push_back(t);

        //分配够了
        if(fn <= (iAllocSize - sizeof(tagChunkHead)))
        {
            break;
        }

        //还需要多少空间
        fn -= iAllocSize - sizeof(tagChunkHead);
    }

    return TC_Multi_HashMap::RT_OK;
}

uint32_t TC_Multi_HashMap::MainKey::getDataLen()
{
    uint32_t n = 0;
    if(!HASNEXTCHUNK())
    {
        // 只有一个chunk
        n += getHeadPtr()->_iDataLen;
        return n;
    }
    
    //当前块的大小
    n += getHeadPtr()->_iSize - sizeof(tagMainKeyHead);
    tagChunkHead *pChunk = getChunkHead(getHeadPtr()->_iNextChunk);
    
    while (true)
    {
        if(pChunk->_bNextChunk)
        {
            //当前块的大小
            n += pChunk->_iSize - sizeof(tagChunkHead);
            pChunk   =  getChunkHead(pChunk->_iNextChunk);
        }
        else
        {
            // 最后一个chunk
            n += pChunk->_iDataLen;
            break;
        }
    }
    
    return n;
}

int TC_Multi_HashMap::MainKey::get(void *pData, uint32_t &iDataLen)
{
    if(!HASNEXTCHUNK())
    {
        // 没有下一个chunk, 一个chunk就可以装下数据了
        iDataLen = min(getHeadPtr()->_iDataLen, iDataLen);
        memcpy(pData, getHeadPtr()->_cData, iDataLen);
        return TC_Multi_HashMap::RT_OK;
    }
    else
    {
        uint32_t iUseSize = getHeadPtr()->_iSize - sizeof(tagMainKeyHead);
        uint32_t iCopyLen = min(iUseSize, iDataLen);
        
        // 先copy第一个chunk的数据
        memcpy(pData, getHeadPtr()->_cData, iCopyLen);
        if (iDataLen < iUseSize)
        {
            return TC_Multi_HashMap::RT_NOTALL_ERR;   // copy数据不完全
        }
        
        // 已经copy长度
        uint32_t iHasLen  = iCopyLen;
        // 最大剩余长度
        uint32_t iLeftLen = iDataLen - iCopyLen;
        
        
        // copy后面所有的chunk中的数据
        tagChunkHead *pChunk    = getChunkHead(getHeadPtr()->_iNextChunk);
        while(iHasLen < iDataLen)
        {
            iUseSize        = pChunk->_iSize - sizeof(tagChunkHead);
            if(!pChunk->_bNextChunk)
            {
                uint32_t iCopyLen = min(pChunk->_iDataLen, iLeftLen);
                memcpy((char*)pData + iHasLen, pChunk->_cData, iCopyLen);
                iDataLen = iHasLen + iCopyLen;
                
                if(iLeftLen < pChunk->_iDataLen)
                {
                    return TC_Multi_HashMap::RT_NOTALL_ERR;       // copy不完全
                }
                
                return TC_Multi_HashMap::RT_OK;
            }
            else
            {
                uint32_t iCopyLen = min(iUseSize, iLeftLen);
                memcpy((char*)pData + iHasLen, pChunk->_cData, iCopyLen);
                if (iLeftLen <= iUseSize)
                {
                    iDataLen = iHasLen + iCopyLen;
                    return TC_Multi_HashMap::RT_NOTALL_ERR;   // copy不完全
                }
                
                iHasLen  += iCopyLen;
                iLeftLen -= iCopyLen;
                
                pChunk   =  getChunkHead(pChunk->_iNextChunk);
            }
        }
    }
    
    return TC_Multi_HashMap::RT_OK;
}

int TC_Multi_HashMap::MainKey::get(string &mk)
{
    // 获取数据长度
    uint32_t iLen = getDataLen();
    
    char *cData = new char[iLen];
    uint32_t iGetLen = iLen;
    int ret = get(cData, iGetLen);
    if(ret == TC_Multi_HashMap::RT_OK)
    {
        // 解码成string
        mk.assign(cData, iGetLen);
    }
    
    delete[] cData;
    
    return ret;
}

int TC_Multi_HashMap::MainKey::set(const void *pData, uint32_t iDataLen, vector<TC_Multi_HashMap::Value> &vtData)
{
    // 首先分配刚刚够的长度, 不能多一个chunk, 也不能少一个chunk
    // allocate会判断当前chunk的长度是否满足iDataLen，少了就加chunk，多了就释放chunk
    int ret = allocate(iDataLen, vtData);
    if(ret != TC_Multi_HashMap::RT_OK)
    {
        return ret;
    }

    // 第一个chunk的有效长度
    uint32_t iUseSize = getHeadPtr()->_iSize - sizeof(tagMainKeyHead);
    if(!HASNEXTCHUNK())
    {
        // 没有下一个chunk, 一个chunk就可以装下数据了
        // 先copy数据, 再复制数据长度
        memcpy(getHeadPtr()->_cData, (char*)pData, iDataLen);
        getHeadPtr()->_iDataLen   = iDataLen;
    }
    else
    {
        // copy到当前的chunk中
        memcpy(getHeadPtr()->_cData, (char*)pData, iUseSize);
        // 剩余程度
        uint32_t iLeftLen = iDataLen - iUseSize;
        uint32_t iCopyLen = iUseSize;

        tagChunkHead *pChunk    = getChunkHead(getHeadPtr()->_iNextChunk);
        while(true)
        {
            // 计算chunk的可用大小
            iUseSize                = pChunk->_iSize - sizeof(tagChunkHead);

            if(!pChunk->_bNextChunk)
            {
                assert(iUseSize >= iLeftLen);
                // copy到当前的chunk中
                memcpy(pChunk->_cData, (char*)pData + iCopyLen, iLeftLen);
                // 最后一个chunk, 才有数据长度, 先copy数据再赋值长度
                pChunk->_iDataLen = iLeftLen;
                iCopyLen += iLeftLen;
                iLeftLen -= iLeftLen;
                break;
            }
            else
            {
                // copy到当前的chunk中
                memcpy(pChunk->_cData, (char*)pData + iCopyLen, iUseSize);
                iCopyLen += iUseSize;
                iLeftLen -= iUseSize;

                pChunk   =  getChunkHead(pChunk->_iNextChunk);
            }
        }
        assert(iLeftLen == 0);
    }

    return TC_Multi_HashMap::RT_OK;
}

int TC_Multi_HashMap::Block::getBlockData(TC_Multi_HashMap::BlockData &data)
{
    data._dirty = isDirty();
    data._synct = getSyncTime();
    data._iVersion = getVersion();

    string s;
    int ret   = get(s);

    if(ret != TC_Multi_HashMap::RT_OK)
    {
        return ret;
    }

    try
    {
        // block保存的数据中，第一部分为除主key外的联合主键
        TC_PackOut po(s.c_str(), s.length());
        po >> data._key;

        // 如果不是只有Key
        if(!isOnlyKey())
        {
            // 第二部分为数据值
            po >> data._value;
        }
        else
        {
            return TC_Multi_HashMap::RT_ONLY_KEY;
        }
    }
    catch(exception &ex)
    {
        ret = TC_Multi_HashMap::RT_DECODE_ERR;
    }

    return ret;
}

uint32_t TC_Multi_HashMap::Block::getLastBlockHead(bool bUKList)
{
    uint32_t iHead = _iHead;

    if(bUKList)
    {
        // 获取当前联合主键block链上的最后一个block
        while(getBlockHead(iHead)->_iUKBlockNext != 0)
        {
            iHead = getBlockHead(iHead)->_iUKBlockNext;
        }
    }
    else
    {
        // 获取当前主key block链上的最后一个block
        while(getBlockHead(iHead)->_iMKBlockNext != 0)
        {
            iHead = getBlockHead(iHead)->_iMKBlockNext;
        }
    }

    return iHead;
}

int TC_Multi_HashMap::Block::get(void *pData, uint32_t &iDataLen)
{
    if(!HASNEXTCHUNK())
    {
        // 没有下一个chunk, 一个chunk就可以装下数据了
        memcpy(pData, getBlockHead()->_cData, min(getBlockHead()->_iDataLen, iDataLen));
        iDataLen = getBlockHead()->_iDataLen;
        return TC_Multi_HashMap::RT_OK;
    }
    else
    {
        // 第一个chunk的有效空间长度
        uint32_t iUseSize = getBlockHead()->_iSize - sizeof(tagBlockHead);
        uint32_t iCopyLen = min(iUseSize, iDataLen);

        // copy第一个chunk中的数据
        memcpy(pData, getBlockHead()->_cData, iCopyLen);
        if (iDataLen < iUseSize)
        {
            // 外部提供的缓冲区长度不够
            return TC_Multi_HashMap::RT_NOTALL_ERR;   //copy数据不完全
        }

        // 已经copy长度
        uint32_t iHasLen  = iCopyLen;
        // 最大剩余长度
        uint32_t iLeftLen = iDataLen - iCopyLen;

        tagChunkHead *pChunk    = getChunkHead(getBlockHead()->_iNextChunk);
        while(iHasLen < iDataLen)
        {
            iUseSize        = pChunk->_iSize - sizeof(tagChunkHead);
            if(!pChunk->_bNextChunk)
            {
                // 最后一个chunk
                uint32_t iCopyLen = min(pChunk->_iDataLen, iLeftLen);
                memcpy((char*)pData + iHasLen, pChunk->_cData, iCopyLen);
                iDataLen = iHasLen + iCopyLen;

                if(iLeftLen < pChunk->_iDataLen)
                {
                    return TC_Multi_HashMap::RT_NOTALL_ERR;       // copy不完全
                }

                return TC_Multi_HashMap::RT_OK;
            }
            else
            {
                uint32_t iCopyLen = min(iUseSize, iLeftLen);
                // copy当前的chunk
                memcpy((char*)pData + iHasLen, pChunk->_cData, iCopyLen);
                if (iLeftLen <= iUseSize)
                {
                    iDataLen = iHasLen + iCopyLen;
                    return TC_Multi_HashMap::RT_NOTALL_ERR;   // copy不完全
                }

                // copy当前chunk完全
                iHasLen  += iCopyLen;
                iLeftLen -= iCopyLen;

                pChunk   =  getChunkHead(pChunk->_iNextChunk);
            }
        }
    }

    return TC_Multi_HashMap::RT_OK;
}

int TC_Multi_HashMap::Block::get(string &s)
{
    uint32_t iLen = getDataLen();

    char *cData = new char[iLen];
    uint32_t iGetLen = iLen;
    int ret = get(cData, iGetLen);
    if(ret == TC_Multi_HashMap::RT_OK)
    {
        s.assign(cData, iGetLen);
    }

    delete[] cData;

    return ret;
}

int TC_Multi_HashMap::Block::set(const void *pData, uint32_t iDataLen, bool bOnlyKey, 
                                 uint8_t iVersion, vector<TC_Multi_HashMap::Value> &vtData)
{
    // version为0表示外部不关心数据版本，onlykey时也不需要关注数据版本
    if(iVersion != 0 && !isOnlyKey() && getBlockHead()->_iVersion != iVersion)
    {
        // 数据版本不匹配
        return TC_Multi_HashMap::RT_DATA_VER_MISMATCH;
    }

    // 首先分配刚刚够的长度, 不能多一个chunk, 也不能少一个chunk
    // allocate会判断当前chunk的长度是否满足iDataLen，少了就加chunk，多了就释放chunk
    int ret = allocate(iDataLen, vtData);
    if(ret != TC_Multi_HashMap::RT_OK)
    {
        return ret;
    }

    if(bOnlyKey)
    {
        // 原始数据是脏数据
        if(isDirty())
        {
            _pMap->delDirtyCount();
        }

        // 数据被修改, 设置为脏数据
        SETDIRTY(false);

        // 原始数据不是OnlyKey数据
        if(!isOnlyKey())
        {
            _pMap->incOnlyKeyCount();
        }
    }
    else
    {
        //原始数据不是脏数据
        if(!isDirty())
        {
            _pMap->incDirtyCount();
        }

        //数据被修改, 设置为脏数据
        SETDIRTY(true);

        //原始数据是OnlyKey数据
        if(isOnlyKey())
        {
            _pMap->delOnlyKeyCount();
        }

        // 递增数据版本
        iVersion = getBlockHead()->_iVersion;
        iVersion ++;
        if(iVersion == 0)
        {
            // 0是保留版本，有效版本范围是1-255
            iVersion = 1;
        }
        getBlockHead()->_iVersion = iVersion;
    }

    //设置是否只有Key
    SETONLYKEY(bOnlyKey);

    // 第一个chunk的有效空间大小
    uint32_t iUseSize = getBlockHead()->_iSize - sizeof(tagBlockHead);
    if(!HASNEXTCHUNK())
    {
        // 没有下一个chunk, 一个chunk就可以装下数据了
        memcpy(getBlockHead()->_cData, (char*)pData, iDataLen);
        getBlockHead()->_iDataLen   = iDataLen;
    }
    else
    {
        // copy到第一个chunk中
        memcpy(getBlockHead()->_cData, (char*)pData, iUseSize);
        // 剩余程度
        uint32_t iLeftLen = iDataLen - iUseSize;
        uint32_t iCopyLen = iUseSize;

        tagChunkHead *pChunk    = getChunkHead(getBlockHead()->_iNextChunk);
        while(true)
        {
            // 计算chunk的可用大小
            iUseSize                = pChunk->_iSize - sizeof(tagChunkHead);

            if(!pChunk->_bNextChunk)
            {
                assert(iUseSize >= iLeftLen);
                // copy到当前的chunk中
                memcpy(pChunk->_cData, (char*)pData + iCopyLen, iLeftLen);
                // 最后一个chunk, 才有数据长度, 先copy数据再赋值长度
                pChunk->_iDataLen = iLeftLen;
                iCopyLen += iLeftLen;
                iLeftLen -= iLeftLen;
                break;
            }
            else
            {
                // copy到当前的chunk中
                memcpy(pChunk->_cData, (char*)pData + iCopyLen, iUseSize);
                iCopyLen += iUseSize;
                iLeftLen -= iUseSize;

                pChunk   =  getChunkHead(pChunk->_iNextChunk);
            }
        }
        assert(iLeftLen == 0);
    }

    return TC_Multi_HashMap::RT_OK;
}

void TC_Multi_HashMap::Block::setDirty(bool b)
{
    if (b)
    {
        if(!isDirty())
        {
            SETDIRTY(b);
            _pMap->incDirtyCount();
        }
    }
    else
    {
        if(isDirty())
        {
            SETDIRTY(b);
            _pMap->delDirtyCount();
        }
    }
}

bool TC_Multi_HashMap::Block::nextBlock()
{
    _iHead = getBlockHead()->_iUKBlockNext;
    _pHead = _pMap->getAbsolute(_iHead);

    return _iHead != 0;
}

bool TC_Multi_HashMap::Block::prevBlock()
{
    _iHead = getBlockHead()->_iUKBlockPrev;
    _pHead = _pMap->getAbsolute(_iHead);

    return _iHead != 0;
}

void TC_Multi_HashMap::Block::deallocate()
{
    // 先释放所有的后续chunk
    if(HASNEXTCHUNK())
    {
        deallocate(getBlockHead()->_iNextChunk);
    }

    // 再释放第一个chunk
    vector<uint32_t> v;
    v.push_back(_iHead);
    _pMap->_pDataAllocator->deallocateMemChunk(v);
}

void TC_Multi_HashMap::Block::makeNew(uint32_t iMainKeyAddr, uint32_t uIndex, uint32_t iAllocSize, bool bHead)
{
    getBlockHead()->_iSize          = iAllocSize;
    getBlockHead()->_iIndex         = uIndex;
    getBlockHead()->_iUKBlockNext   = 0;
    getBlockHead()->_iUKBlockPrev   = 0;
    getBlockHead()->_iMKBlockNext   = 0;
    getBlockHead()->_iMKBlockPrev   = 0;
    getBlockHead()->_iSetNext       = 0;
    getBlockHead()->_iSetPrev       = 0;
    getBlockHead()->_iMainKey       = iMainKeyAddr;
    getBlockHead()->_iSyncTime      = 0;
    getBlockHead()->_iDataLen       = 0;
    getBlockHead()->_iVersion        = 1;        // 有效版本范围1-255
    SETONLYKEY(false);
    SETDIRTY(true);
    SETNEXTCHUNK(false);

    _pMap->incDirtyCount();
    _pMap->incElementCount();
    _pMap->incListCount(uIndex);

    // 增加主key下面的block数量
    MainKey mainKey(_pMap, iMainKeyAddr);
    _pMap->saveValue(&mainKey.getHeadPtr()->_iBlockCount, mainKey.getHeadPtr()->_iBlockCount + 1);
    _pMap->updateMaxMainKeyBlockCount(mainKey.getHeadPtr()->_iBlockCount);

    // 挂在block链表上
    if(_pMap->item(uIndex)->_iBlockAddr == 0)
    {
        // 当前hash桶没有元素
        _pMap->saveValue(&_pMap->item(uIndex)->_iBlockAddr, _iHead);
        _pMap->saveValue(&getBlockHead()->_iUKBlockNext, (uint32_t)0);
        _pMap->saveValue(&getBlockHead()->_iUKBlockPrev, (uint32_t)0);
    }
    else
    {
        // 当前hash桶有元素, 挂在桶开头
        _pMap->saveValue(&getBlockHead(_pMap->item(uIndex)->_iBlockAddr)->_iUKBlockPrev, _iHead);
        _pMap->saveValue(&getBlockHead()->_iUKBlockNext, _pMap->item(uIndex)->_iBlockAddr);
        _pMap->saveValue(&_pMap->item(uIndex)->_iBlockAddr, _iHead);
        _pMap->saveValue(&getBlockHead()->_iUKBlockPrev, (uint32_t)0);
    }

    // 挂在主key链上
    if(mainKey.getHeadPtr()->_iAddr == 0)
    {
        // 当前主key链上还没有元素
        _pMap->saveValue(&mainKey.getHeadPtr()->_iAddr, _iHead);
        _pMap->saveValue(&getBlockHead()->_iMKBlockNext, (uint32_t)0);
        _pMap->saveValue(&getBlockHead()->_iMKBlockPrev, (uint32_t)0);
    }
    else
    {
        // 当前主key链上已经有元素
        if(bHead)
        {
            // 挂在最前面
            _pMap->saveValue(&getBlockHead(mainKey.getHeadPtr()->_iAddr)->_iMKBlockPrev, _iHead);
            _pMap->saveValue(&getBlockHead()->_iMKBlockNext, mainKey.getHeadPtr()->_iAddr);
            _pMap->saveValue(&mainKey.getHeadPtr()->_iAddr, _iHead);
            _pMap->saveValue(&getBlockHead()->_iMKBlockPrev, (uint32_t)0);
        }
        else
        {
            // 挂到最后面
            Block blkHead(_pMap, mainKey.getHeadPtr()->_iAddr);
            uint32_t iLast = blkHead.getLastBlockHead(false);
            _pMap->saveValue(&getBlockHead(iLast)->_iMKBlockNext, _iHead);
            _pMap->saveValue(&getBlockHead()->_iMKBlockNext, (uint32_t)0);
            _pMap->saveValue(&getBlockHead()->_iMKBlockPrev, iLast);
        }
    }

    // 挂在Set链表的头部
    if(_pMap->_pHead->_iSetHead == 0)
    {
        assert(_pMap->_pHead->_iSetTail == 0);
        _pMap->saveValue(&_pMap->_pHead->_iSetHead, _iHead);
        _pMap->saveValue(&_pMap->_pHead->_iSetTail, _iHead);
    }
    else
    {
        assert(_pMap->_pHead->_iSetTail != 0);
        _pMap->saveValue(&getBlockHead()->_iSetNext, _pMap->_pHead->_iSetHead);
        _pMap->saveValue(&getBlockHead(_pMap->_pHead->_iSetHead)->_iSetPrev, _iHead);
        _pMap->saveValue(&_pMap->_pHead->_iSetHead, _iHead);
    }
}

void TC_Multi_HashMap::Block::erase()
{
    //////////////////修改脏数据链表/////////////
    if(_pMap->_pHead->_iDirtyTail == _iHead)
    {
        _pMap->saveValue(&_pMap->_pHead->_iDirtyTail, getBlockHead()->_iSetPrev);
    }

    //////////////////修改回写数据链表/////////////
    if(_pMap->_pHead->_iSyncTail == _iHead)
    {
        _pMap->saveValue(&_pMap->_pHead->_iSyncTail, getBlockHead()->_iSetPrev);
    }

    ////////////////////修改Set链表的数据//////////
    {
        bool bHead = (_pMap->_pHead->_iSetHead == _iHead);
        bool bTail = (_pMap->_pHead->_iSetTail == _iHead);

        if(!bHead)
        {
            if(bTail)
            {
                assert(getBlockHead()->_iSetNext == 0);
                // 是尾部, 尾部指针指向上一个元素
                _pMap->saveValue(&_pMap->_pHead->_iSetTail, getBlockHead()->_iSetPrev);
                _pMap->saveValue(&getBlockHead(getBlockHead()->_iSetPrev)->_iSetNext, (uint32_t)0);
            }
            else
            {
                // 不是头部也不是尾部
                assert(getBlockHead()->_iSetNext != 0);
                _pMap->saveValue(&getBlockHead(getBlockHead()->_iSetPrev)->_iSetNext, getBlockHead()->_iSetNext);
                _pMap->saveValue(&getBlockHead(getBlockHead()->_iSetNext)->_iSetPrev, getBlockHead()->_iSetPrev);
            }
        }
        else
        {
            if(bTail)
            {
                assert(getBlockHead()->_iSetNext == 0);
                assert(getBlockHead()->_iSetPrev == 0);
                // 头部也是尾部, 指针都设置为0
                _pMap->saveValue(&_pMap->_pHead->_iSetHead, (uint32_t)0);
                _pMap->saveValue(&_pMap->_pHead->_iSetTail, (uint32_t)0);
            }
            else
            {
                // 头部不是尾部, 头部指针指向下一个元素
                assert(getBlockHead()->_iSetNext != 0);
                _pMap->saveValue(&_pMap->_pHead->_iSetHead, getBlockHead()->_iSetNext);
                // 下一个元素上指针为0
                _pMap->saveValue(&getBlockHead(getBlockHead()->_iSetNext)->_iSetPrev, (uint32_t)0);
            }
        }
    }

    ////////////////////修改主key链表的数据//////////
    //
    {
        MainKey mainKey(_pMap, getBlockHead()->_iMainKey);
        if(getBlockHead()->_iMKBlockPrev != 0)
        {
            // 将上一下block指向下一个
            _pMap->saveValue(&getBlockHead(getBlockHead()->_iMKBlockPrev)->_iMKBlockNext, getBlockHead()->_iMKBlockNext);
        }
        else
        {
            // 主key链上的第一个
            _pMap->saveValue(&mainKey.getHeadPtr()->_iAddr, getBlockHead()->_iMKBlockNext);
        }
        if(getBlockHead()->_iMKBlockNext != 0)
        {
            // 将下一个block指向上一个
            _pMap->saveValue(&getBlockHead(getBlockHead()->_iMKBlockNext)->_iMKBlockPrev, getBlockHead()->_iMKBlockPrev);
        }
    }

    ///////////////////从block链表中去掉///////////
    //
    //上一个block指向下一个block
    if(getBlockHead()->_iUKBlockPrev != 0)
    {
        _pMap->saveValue(&getBlockHead(getBlockHead()->_iUKBlockPrev)->_iUKBlockNext, getBlockHead()->_iUKBlockNext);
    }

    //下一个block指向上一个
    if(getBlockHead()->_iUKBlockNext != 0)
    {
        _pMap->saveValue(&getBlockHead(getBlockHead()->_iUKBlockNext)->_iUKBlockPrev, getBlockHead()->_iUKBlockPrev);
    }

    //////////////////如果是hash头部, 需要修改hash索引数据指针//////
    //
    _pMap->delListCount(getBlockHead()->_iIndex);
    if(getBlockHead()->_iUKBlockPrev == 0)
    {
        //如果是hash桶的头部, 则还需要处理
        TC_Multi_HashMap::tagHashItem *pItem  = _pMap->item(getBlockHead()->_iIndex);
        assert(pItem->_iBlockAddr == _iHead);
        if(pItem->_iBlockAddr == _iHead)
        {
            _pMap->saveValue(&pItem->_iBlockAddr, getBlockHead()->_iUKBlockNext);
        }
    }

    //////////////////脏数据///////////////////
    //
    if (isDirty())
    {
        _pMap->delDirtyCount();
    }

    if(isOnlyKey())
    {
        _pMap->delOnlyKeyCount();
    }

    //元素个数减少
    _pMap->delElementCount();

    // 减少主key下数据个数
    MainKey mainKey(_pMap, getBlockHead()->_iMainKey);
    _pMap->saveValue(&mainKey.getHeadPtr()->_iBlockCount, mainKey.getHeadPtr()->_iBlockCount - 1);
    _pMap->updateMaxMainKeyBlockCount(mainKey.getHeadPtr()->_iBlockCount);

    // 手工置空一些参数，使在数据恢复区建立记录，以便crash后恢复
    _pMap->saveValue(&getBlockHead()->_iSize, (uint32_t)0);
    _pMap->saveValue(&getBlockHead()->_iIndex, (uint32_t)0);
    _pMap->saveValue(&getBlockHead()->_iSyncTime, (time_t)0);
    _pMap->saveValue(&getBlockHead()->_iBitset, (uint8_t)0);
    _pMap->saveValue(&getBlockHead()->_iDataLen, (uint32_t)0);
    // 特别地，这里一定要手工置block指向的mainkey为空，否则crash后的恢复将无法建立起block与mainkey的联系
    _pMap->saveValue(&getBlockHead()->_iMainKey, (uint32_t)0);

    // 归还内存前先确认上述修改，因为内存被释放后是不能恢复的
    // 即使后面的内存释放失败也不会造成数据的破坏，只会造成一个数据的丢失
    _pMap->doUpdate();

    //归还到内存中
    deallocate();
}

void TC_Multi_HashMap::Block::refreshSetList()
{
    assert(_pMap->_pHead->_iSetHead != 0);
    assert(_pMap->_pHead->_iSetTail != 0);

    //修改同步链表
    if(_pMap->_pHead->_iSyncTail == _iHead && _pMap->_pHead->_iSetHead != _iHead)
    {
        _pMap->saveValue(&_pMap->_pHead->_iSyncTail, getBlockHead()->_iSetPrev);
    }

    //修改脏数据尾部链表指针, 不仅一个元素
    if(_pMap->_pHead->_iDirtyTail == _iHead && _pMap->_pHead->_iSetHead != _iHead)
    {
        //脏数据尾部位置前移
        _pMap->saveValue(&_pMap->_pHead->_iDirtyTail, getBlockHead()->_iSetPrev);
    }
    else if (_pMap->_pHead->_iDirtyTail == 0)
    {
        //原来没有脏数据
        _pMap->saveValue(&_pMap->_pHead->_iDirtyTail, _iHead);
    }

    //是头部数据或者set新数据时走到这个分支
    if(_pMap->_pHead->_iSetHead == _iHead)
    {
        //刷新Get链
        MainKey mainKey(_pMap, getBlockHead()->_iMainKey);
        mainKey.refreshGetList();
        return;
    }

    uint32_t iPrev = getBlockHead()->_iSetPrev;
    uint32_t iNext = getBlockHead()->_iSetNext;

    assert(iPrev != 0);

    //挂在链表头部
    _pMap->saveValue(&getBlockHead()->_iSetNext, _pMap->_pHead->_iSetHead);
    _pMap->saveValue(&getBlockHead(_pMap->_pHead->_iSetHead)->_iSetPrev, _iHead);
    _pMap->saveValue(&_pMap->_pHead->_iSetHead, _iHead);
    _pMap->saveValue(&getBlockHead()->_iSetPrev, (uint32_t)0);

    //上一个元素的Next指针指向下一个元素
    _pMap->saveValue(&getBlockHead(iPrev)->_iSetNext, iNext);

    //下一个元素的Prev指向上一个元素
    if (iNext != 0)
    {
        _pMap->saveValue(&getBlockHead(iNext)->_iSetPrev, iPrev);
    }
    else
    {
        //改变尾部指针
        assert(_pMap->_pHead->_iSetTail == _iHead);
        _pMap->saveValue(&_pMap->_pHead->_iSetTail, iPrev);
    }

    //刷新Get链
    MainKey mainKey(_pMap, getBlockHead()->_iMainKey);
    mainKey.refreshGetList();
}


void TC_Multi_HashMap::Block::deallocate(uint32_t iChunk)
{
    tagChunkHead *pChunk = getChunkHead(iChunk);
    vector<uint32_t> v;
    v.push_back(iChunk);

    //获取所有后续的chunk地址
    while(true)
    {
        if(pChunk->_bNextChunk)
        {
            v.push_back(pChunk->_iNextChunk);
            pChunk = getChunkHead(pChunk->_iNextChunk);
        }
        else
        {
            break;
        }
    }

    //空间全部释放掉
    _pMap->_pDataAllocator->deallocateMemChunk(v);
}

int TC_Multi_HashMap::Block::allocate(uint32_t iDataLen, vector<TC_Multi_HashMap::Value> &vtData)
{
    uint32_t fn   = 0;

    // 第一个chunk的有效数据容量
    fn = getBlockHead()->_iSize - sizeof(tagBlockHead);
    if(fn >= iDataLen)
    {
        // 一个chunk就可以了, 后续的chunk都要释放掉
        if(HASNEXTCHUNK())
        {
            uint32_t iNextChunk = getBlockHead()->_iNextChunk;
            // 先修改自己的区块
            _pMap->saveValue(&getBlockHead()->_iBitset, NEXTCHUNK_BIT, false);
            _pMap->saveValue(&getBlockHead()->_iDataLen, (uint32_t)0);
            // 修改成功后再释放区块, 从而保证, 不会core的时候导致整个内存块不可用
            deallocate(iNextChunk);
        }
        return TC_Multi_HashMap::RT_OK;
    }

    // 计算还需要多少长度
    fn = iDataLen - fn;

    if(HASNEXTCHUNK())
    {
        tagChunkHead *pChunk    = getChunkHead(getBlockHead()->_iNextChunk);

        while(true)
        {
            if(fn <= (pChunk->_iSize - sizeof(tagChunkHead)))
            {
                //已经不需要chunk了, 释放多余的chunk
                if(pChunk->_bNextChunk)
                {
                    uint32_t iNextChunk = pChunk->_iNextChunk;
                    _pMap->saveValue(&pChunk->_bNextChunk, (uint8_t)false);
                    //一旦异常core, 最坏的情况就是少了可用的区块, 但是整个内存结构还是可用的
                    deallocate(iNextChunk);
                }
                return TC_Multi_HashMap::RT_OK ;
            }

            //计算, 还需要多少长度
            fn -= pChunk->_iSize - sizeof(tagChunkHead);

            if(pChunk->_bNextChunk)
            {
                pChunk  =  getChunkHead(pChunk->_iNextChunk);
            }
            else
            {
                //没有后续chunk了, 需要新分配fn的空间
                vector<uint32_t> chunks;
                int ret = allocateChunk(fn, chunks, vtData);
                if(ret != TC_Multi_HashMap::RT_OK)
                {
                    //没有内存可以分配
                    return ret;
                }

                _pMap->saveValue(&pChunk->_bNextChunk, (uint8_t)true);
                _pMap->saveValue(&pChunk->_iNextChunk, chunks[0]);
                //chunk指向分配的第一个chunk
                pChunk  =  getChunkHead(chunks[0]);
                //修改第一个chunk的属性, 保证异常core的时候, chunk链表不会有问题
                _pMap->saveValue(&pChunk->_bNextChunk, (uint8_t)false);
                _pMap->saveValue(&pChunk->_iDataLen, (uint32_t)0);

                //连接每个chunk
                return joinChunk(pChunk, chunks);
            }
        }
    }
    else
    {
        //没有后续chunk了, 需要新分配fn空间
        vector<uint32_t> chunks;
        int ret = allocateChunk(fn, chunks, vtData);
        if(ret != TC_Multi_HashMap::RT_OK)
        {
            //没有内存可以分配
            return ret;
        }

        _pMap->saveValue(&getBlockHead()->_iBitset, NEXTCHUNK_BIT, true);
        _pMap->saveValue(&getBlockHead()->_iNextChunk, chunks[0]);
        //chunk指向分配的第一个chunk
        tagChunkHead *pChunk = getChunkHead(chunks[0]);
        //修改第一个chunk的属性, 保证异常core的时候, chunk链表不会有问题
        _pMap->saveValue(&pChunk->_bNextChunk, (uint8_t)false);
        _pMap->saveValue(&pChunk->_iDataLen, (uint32_t)0);

        //连接每个chunk
        return joinChunk(pChunk, chunks);
    }

    return TC_Multi_HashMap::RT_OK;
}

int TC_Multi_HashMap::Block::joinChunk(tagChunkHead *pChunk, const vector<uint32_t>& chunks)
{
    for (size_t i = 0; i < chunks.size(); ++i)
    {
        if (i == chunks.size() - 1)
        {
            _pMap->saveValue(&pChunk->_bNextChunk, (uint8_t)false);
            return TC_Multi_HashMap::RT_OK;
        }
        else
        {
            _pMap->saveValue(&pChunk->_bNextChunk, (uint8_t)true);
            _pMap->saveValue(&pChunk->_iNextChunk, chunks[i+1]);
            pChunk  =  getChunkHead(chunks[i+1]);
            _pMap->saveValue(&pChunk->_bNextChunk, (uint8_t)false);
            _pMap->saveValue(&pChunk->_iDataLen, (uint32_t)0);
        }
    }

    return TC_Multi_HashMap::RT_OK;
}

int TC_Multi_HashMap::Block::allocateChunk(uint32_t fn, vector<uint32_t> &chunks, vector<TC_Multi_HashMap::Value> &vtData)
{
    assert(fn > 0);

    while(true)
    {
        uint32_t iAllocSize = fn;
        // 分配空间
        // 正在分配的block所属的主key是不能被淘汰的
        uint32_t t = _pMap->_pDataAllocator->allocateChunk(getBlockHead()->_iMainKey, iAllocSize, vtData);
        if (t == 0)
        {
            //没有内存可以分配了, 先把分配的归还
            _pMap->_pDataAllocator->deallocateMemChunk(chunks);
            chunks.clear();
            return TC_Multi_HashMap::RT_NO_MEMORY;
        }

        //设置分配的数据块的大小
        getChunkHead(t)->_iSize = iAllocSize;

        chunks.push_back(t);

        //分配够了
        if(fn <= (iAllocSize - sizeof(tagChunkHead)))
        {
            break;
        }

        //还需要多少空间
        fn -= iAllocSize - sizeof(tagChunkHead);
    }

    return TC_Multi_HashMap::RT_OK;
}

uint32_t TC_Multi_HashMap::Block::getDataLen()
{
    uint32_t n = 0;
    if(!HASNEXTCHUNK())
    {
        n += getBlockHead()->_iDataLen;
        return n;
    }

    //当前块的大小
    n += getBlockHead()->_iSize - sizeof(tagBlockHead);
    tagChunkHead *pChunk    = getChunkHead(getBlockHead()->_iNextChunk);

    while (true)
    {
        if(pChunk->_bNextChunk)
        {
            //当前块的大小
            n += pChunk->_iSize - sizeof(tagChunkHead);
            pChunk   =  getChunkHead(pChunk->_iNextChunk);
        }
        else
        {
            n += pChunk->_iDataLen;
            break;
        }
    }

    return n;
}

////////////////////////////////////////////////////////

uint32_t TC_Multi_HashMap::BlockAllocator::allocateMemBlock(uint32_t iMainKeyAddr, uint32_t index, bool bHead, 
                                                          uint32_t &iAllocSize, vector<TC_Multi_HashMap::Value> &vtData)
{
begin:
    size_t iAddr = 0;
    size_t bigSize = iAllocSize;
    _pChunkAllocator->allocate2(bigSize, bigSize, iAddr);
    iAllocSize = (uint32_t)bigSize;
    if(iAddr == 0)
    {
        size_t ret = _pMap->eraseExcept(iMainKeyAddr, vtData);
        if(ret == 0)
        {
            return 0;     //没有空间可以释放了
        }
        goto begin;
    }

    // 初始化block头部信息
    Block block(_pMap, (uint32_t)iAddr);
    block.makeNew(iMainKeyAddr, index, iAllocSize, bHead);

    _pMap->incChunkCount();

    return (uint32_t)iAddr;
}

uint32_t TC_Multi_HashMap::BlockAllocator::allocateMainKeyHead(uint32_t index, vector<TC_Multi_HashMap::Value> &vtData)
{
    size_t iAllocSize = sizeof(MainKey::tagMainKeyHead);
begin:
    size_t iAddr = 0;
    _pChunkAllocator->allocate2(iAllocSize, iAllocSize, iAddr);
    if(iAddr == 0)
    {
        size_t ret = _pMap->eraseExcept(0, vtData);
        if(ret == 0)
        {
            return 0;     //没有空间可以释放了
        }
        goto begin;
    }

    // 分配的新的MemBlock, 初始化一下
    MainKey mainKey(_pMap, (uint32_t)iAddr);
    mainKey.makeNew(index, (uint32_t)iAllocSize);

    _pMap->incChunkCount();

    return (uint32_t)iAddr;
}


uint32_t TC_Multi_HashMap::BlockAllocator::allocateChunk(uint32_t iAddr, uint32_t &iAllocSize, vector<TC_Multi_HashMap::Value> &vtData)
{
begin:
    size_t iChunkAddr = 0;
    size_t bigSize = iAllocSize;
    _pChunkAllocator->allocate2(bigSize, bigSize, iChunkAddr);
    iAllocSize = (uint32_t)bigSize;
    if(iChunkAddr == 0)
    {
        size_t ret = _pMap->eraseExcept(iAddr, vtData);
        if(ret == 0)
        {
            return 0;     //没有空间可以释放了
        }
        goto begin;
    }

    _pMap->incChunkCount();

    return iChunkAddr;
}

void TC_Multi_HashMap::BlockAllocator::deallocateMemChunk(const vector<uint32_t> &v)
{
    for(size_t i = 0; i < v.size(); i++)
    {
        _pChunkAllocator->deallocate2(v[i]);
        _pMap->delChunkCount();
    }
}

void TC_Multi_HashMap::BlockAllocator::deallocateMemChunk(uint32_t iChunk)
{
    _pChunkAllocator->deallocate2(iChunk);
    _pMap->delChunkCount();
}

///////////////////////////////////////////////////////////////////

TC_Multi_HashMap::HashMapLockItem::HashMapLockItem(TC_Multi_HashMap *pMap, uint32_t iAddr)
: _pMap(pMap)
, _iAddr(iAddr)
{
}

TC_Multi_HashMap::HashMapLockItem::HashMapLockItem(const HashMapLockItem &mcmdi)
: _pMap(mcmdi._pMap)
, _iAddr(mcmdi._iAddr)
{
}

TC_Multi_HashMap::HashMapLockItem &TC_Multi_HashMap::HashMapLockItem::operator=(const TC_Multi_HashMap::HashMapLockItem &mcmdi)
{
    if(this != &mcmdi)
    {
        _pMap   = mcmdi._pMap;
        _iAddr  = mcmdi._iAddr;
    }
    return (*this);
}

bool TC_Multi_HashMap::HashMapLockItem::operator==(const TC_Multi_HashMap::HashMapLockItem &mcmdi)
{
    return _pMap == mcmdi._pMap && _iAddr == mcmdi._iAddr;
}

bool TC_Multi_HashMap::HashMapLockItem::operator!=(const TC_Multi_HashMap::HashMapLockItem &mcmdi)
{
    return _pMap != mcmdi._pMap || _iAddr != mcmdi._iAddr;
}

bool TC_Multi_HashMap::HashMapLockItem::isDirty()
{
    Block block(_pMap, _iAddr);
    return block.isDirty();
}

bool TC_Multi_HashMap::HashMapLockItem::isOnlyKey()
{
    Block block(_pMap, _iAddr);
    return block.isOnlyKey();
}

time_t TC_Multi_HashMap::HashMapLockItem::getSyncTime()
{
    Block block(_pMap, _iAddr);
    return block.getSyncTime();
}

int TC_Multi_HashMap::HashMapLockItem::get(TC_Multi_HashMap::Value &v)
{
    Block block(_pMap, _iAddr);
    MainKey mainKey(_pMap, block.getBlockHead()->_iMainKey);

    // 先获取主key
    int ret = mainKey.get(v._mkey);
    if(ret != TC_Multi_HashMap::RT_OK)
    {
        return ret;
    }

    // 获取数据
    ret = block.getBlockData(v._data);
    if(ret != TC_Multi_HashMap::RT_OK)
    {
        return ret;
    }

    return TC_Multi_HashMap::RT_OK;
}

int TC_Multi_HashMap::HashMapLockItem::get(string &mk, string &uk)
{
    Block block(_pMap, _iAddr);
    MainKey mainKey(_pMap, block.getBlockHead()->_iMainKey);

    // 获取主key
    int ret = mainKey.get(mk);
    if(ret != TC_Multi_HashMap::RT_OK)
    {
        return ret;
    }

    // 获取数据区
    string s;
    ret = block.get(s);
    if(ret != TC_Multi_HashMap::RT_OK)
    {
        return ret;
    }

    try
    {
        // 提取除主key外的联合主键
        TC_PackOut po(s.c_str(), s.length());
        po >> uk;
    }
    catch ( exception &ex )
    {
        return TC_Multi_HashMap::RT_EXCEPTION_ERR;
    }

    return TC_Multi_HashMap::RT_OK;
}

int TC_Multi_HashMap::HashMapLockItem::set(const string &mk, const string &uk, 
                                           const string &v, uint8_t iVersion, vector<TC_Multi_HashMap::Value> &vtData)
{
    Block block(_pMap, _iAddr);

    TC_PackIn pi;
    pi << uk;        // 数据区只存放uk，不存mk，节省空间
    pi << v;

    return block.set(pi.topacket().c_str(), pi.topacket().length(), false, iVersion, vtData);
}

int TC_Multi_HashMap::HashMapLockItem::set(const string &mk, const string &uk, vector<TC_Multi_HashMap::Value> &vtData)
{
    Block block(_pMap, _iAddr);

    TC_PackIn pi;
    pi << uk;        // 数据区只存放uk

    return block.set(pi.topacket().c_str(), pi.topacket().length(), true, 0, vtData);
}

bool TC_Multi_HashMap::HashMapLockItem::equal(const string &mk, const string &uk, TC_Multi_HashMap::Value &v, int &ret)
{
    ret = get(v);

    if ((ret == TC_Multi_HashMap::RT_OK || ret == TC_Multi_HashMap::RT_ONLY_KEY) 
        && mk == v._mkey && uk == v._data._key)
    {
        return true;
    }

    return false;
}

bool TC_Multi_HashMap::HashMapLockItem::equal(const string &mk, const string &uk, int &ret)
{
    string mk1, uk1;
    ret = get(mk1, uk1);

    if (ret == TC_Multi_HashMap::RT_OK && mk == mk1 && uk == uk1)
    {
        return true;
    }

    return false;
}

void TC_Multi_HashMap::HashMapLockItem::nextItem(int iType)
{
    if(_iAddr == 0)
    {
        // 到头了
        return;
    }

    Block block(_pMap, _iAddr);

    if(iType == HashMapLockIterator::IT_BLOCK)
    {
        // 按联合主键索引下的block链遍历
        uint32_t index = block.getBlockHead()->_iIndex;

        //当前block链表有元素
        if(block.nextBlock())
        {
            _iAddr = block.getHead();
            return;
        }

        index += 1;

        while(index < _pMap->_hash.size())
        {
            //当前的hash桶也没有数据
            if (_pMap->item(index)->_iBlockAddr == 0)
            {
                ++index;
                continue;
            }

            _iAddr = _pMap->item(index)->_iBlockAddr;
            return;
        }

        _iAddr = 0;  //到尾部了
    }
    else if(iType == HashMapLockIterator::IT_SET)
    {
        // 按set链遍历
        _iAddr = block.getBlockHead()->_iSetNext;
    }
    else if(iType == HashMapLockIterator::IT_GET)
    {
        // 按get(同一主key下的block)链遍历，即获取下一个get链上的数据
        _iAddr = block.getBlockHead()->_iMKBlockNext;
        if(_iAddr == 0)
        {
            // 当前主key下的block链已经没有数据了，移动GET链上的下一个主key链
            MainKey mainKey(_pMap, block.getBlockHead()->_iMainKey);
            while(mainKey.getHeadPtr()->_iGetNext)
            {
                mainKey = MainKey(_pMap, mainKey.getHeadPtr()->_iGetNext);
                _iAddr = mainKey.getHeadPtr()->_iAddr;
                if(_iAddr != 0)
                {
                    break;
                }
            }
        }
    }
    else if(iType == HashMapLockIterator::IT_MKEY)
    {
        // 按同一主key下block链遍历
        _iAddr = block.getBlockHead()->_iMKBlockNext;
    }
    else if(iType == HashMapLockIterator::IT_UKEY)
    {
        // 按同一联合主键索引的block链遍历
        _iAddr = block.getBlockHead()->_iUKBlockNext;
    }
}

void TC_Multi_HashMap::HashMapLockItem::prevItem(int iType)
{
    if(_iAddr == 0)
    {
        // 到头了
        return;
    }

    Block block(_pMap, _iAddr);

    if(iType == HashMapLockIterator::IT_BLOCK)
    {
        // 按联合主键索引下的block链遍历
        uint32_t index = block.getBlockHead()->_iIndex;
        if(block.prevBlock())
        {
            _iAddr = block.getHead();
            return;
        }

        while(index > 0)
        {
            //当前的hash桶也没有数据
            if (_pMap->item(index-1)->_iBlockAddr == 0)
            {
                --index;
                continue;
            }

            //需要到这个桶的末尾
            Block tmp(_pMap, _pMap->item(index-1)->_iBlockAddr);
            _iAddr = tmp.getLastBlockHead(true);

            return;
        }

        _iAddr = 0;  //到尾部了
    }
    else if(iType == HashMapLockIterator::IT_SET)
    {
        // 按set链遍历
        _iAddr = block.getBlockHead()->_iSetPrev;
    }
    else if(iType == HashMapLockIterator::IT_GET)
    {
        // 按get(同一主key下的block)链遍历，即获取下一个get链上的数据
        _iAddr = block.getBlockHead()->_iMKBlockPrev;
        if(_iAddr == 0)
        {
            // 当前主key下的block链已经没有数据了，移动get链上的上一个主key链
            MainKey mainKey(_pMap, block.getBlockHead()->_iMainKey);
            while(mainKey.getHeadPtr()->_iGetPrev)
            {
                mainKey = MainKey(_pMap, mainKey.getHeadPtr()->_iGetPrev);
                _iAddr = mainKey.getHeadPtr()->_iAddr;
                if(_iAddr != 0)
                {
                    // 移动到主key链上的最后一个block
                    Block tmp(_pMap, _iAddr);
                    while(tmp.getBlockHead()->_iMKBlockNext)
                    {
                        _iAddr = tmp.getBlockHead()->_iMKBlockNext;
                        tmp = Block(_pMap, _iAddr);
                    }
                    break;
                }
            }
        }
    }
    else if(iType == HashMapLockIterator::IT_MKEY)
    {
        // 按同一主key下block链遍历
        _iAddr = block.getBlockHead()->_iMKBlockPrev;
    }
    else if(iType == HashMapLockIterator::IT_UKEY)
    {
        // 按同一联合主键索引的block链遍历
        _iAddr = block.getBlockHead()->_iUKBlockPrev;
    }
}

///////////////////////////////////////////////////////////////////

TC_Multi_HashMap::HashMapLockIterator::HashMapLockIterator()
: _pMap(NULL), _iItem(NULL, 0), _iType(IT_BLOCK), _iOrder(IT_NEXT)
{
}

TC_Multi_HashMap::HashMapLockIterator::HashMapLockIterator(TC_Multi_HashMap *pMap, uint32_t iAddr, int iType, int iOrder)
: _pMap(pMap), _iItem(_pMap, iAddr), _iType(iType), _iOrder(iOrder)
{
}

TC_Multi_HashMap::HashMapLockIterator::HashMapLockIterator(const HashMapLockIterator &it)
: _pMap(it._pMap),_iItem(it._iItem), _iType(it._iType), _iOrder(it._iOrder)
{
}

TC_Multi_HashMap::HashMapLockIterator& TC_Multi_HashMap::HashMapLockIterator::operator=(const HashMapLockIterator &it)
{
    if(this != &it)
    {
        _pMap       = it._pMap;
        _iItem      = it._iItem;
        _iType      = it._iType;
        _iOrder     = it._iOrder;
    }

    return (*this);
}

bool TC_Multi_HashMap::HashMapLockIterator::operator==(const HashMapLockIterator& mcmi)
{
    if (_iItem.getAddr() != 0 || mcmi._iItem.getAddr() != 0)
    {
        return _pMap == mcmi._pMap
            && _iItem == mcmi._iItem
            && _iType == mcmi._iType
            && _iOrder == mcmi._iOrder;
    }

    return _pMap == mcmi._pMap;
}

bool TC_Multi_HashMap::HashMapLockIterator::operator!=(const HashMapLockIterator& mcmi)
{
    if (_iItem.getAddr() != 0 || mcmi._iItem.getAddr() != 0 )
    {
        return _pMap != mcmi._pMap
            || _iItem != mcmi._iItem
            || _iType != mcmi._iType
            || _iOrder != mcmi._iOrder;
    }

    return _pMap != mcmi._pMap;
}

TC_Multi_HashMap::HashMapLockIterator& TC_Multi_HashMap::HashMapLockIterator::operator++()
{
    if(_iOrder == IT_NEXT)
    {
        // 顺序遍历
        _iItem.nextItem(_iType);
    }
    else
    {
        // 逆序遍历
        _iItem.prevItem(_iType);
    }
    return (*this);

}

TC_Multi_HashMap::HashMapLockIterator TC_Multi_HashMap::HashMapLockIterator::operator++(int)
{
    HashMapLockIterator it(*this);

    if(_iOrder == IT_NEXT)
    {
        // 顺序遍历
        _iItem.nextItem(_iType);
    }
    else
    {
        // 逆序遍历
        _iItem.prevItem(_iType);
    }

    return it;

}

///////////////////////////////////////////////////////////////////

TC_Multi_HashMap::HashMapItem::HashMapItem(TC_Multi_HashMap *pMap, uint32_t iIndex)
: _pMap(pMap)
, _iIndex(iIndex)
{
}

TC_Multi_HashMap::HashMapItem::HashMapItem(const HashMapItem &mcmdi)
: _pMap(mcmdi._pMap)
, _iIndex(mcmdi._iIndex)
{
}

TC_Multi_HashMap::HashMapItem &TC_Multi_HashMap::HashMapItem::operator=(const TC_Multi_HashMap::HashMapItem &mcmdi)
{
    if(this != &mcmdi)
    {
        _pMap   = mcmdi._pMap;
        _iIndex  = mcmdi._iIndex;
    }
    return (*this);
}

bool TC_Multi_HashMap::HashMapItem::operator==(const TC_Multi_HashMap::HashMapItem &mcmdi)
{
    return _pMap == mcmdi._pMap && _iIndex == mcmdi._iIndex;
}

bool TC_Multi_HashMap::HashMapItem::operator!=(const TC_Multi_HashMap::HashMapItem &mcmdi)
{
    return _pMap != mcmdi._pMap || _iIndex != mcmdi._iIndex;
}

void TC_Multi_HashMap::HashMapItem::get(vector<TC_Multi_HashMap::Value> &vtData)
{
    uint32_t iAddr = _pMap->item(_iIndex)->_iBlockAddr;

    while(iAddr != 0)
    {
        Block block(_pMap, iAddr);
        MainKey mainKey(_pMap, block.getBlockHead()->_iMainKey);
        TC_Multi_HashMap::Value data;

        // 获取主key
        int ret = mainKey.get(data._mkey);
        if(ret == TC_Multi_HashMap::RT_OK)
        {
            // 获取联合主键及数据
            ret = block.getBlockData(data._data);
            if(ret == TC_Multi_HashMap::RT_OK)
            {
                vtData.push_back(data);
            }
        }

        iAddr = block.getBlockHead()->_iUKBlockNext;
    }
}

void TC_Multi_HashMap::HashMapItem::nextItem()
{
    if(_iIndex == (uint32_t)(-1))
    {
        return;
    }

    if(_iIndex >= _pMap->getHashCount() - 1)
    {
        _iIndex = (uint32_t)(-1);
        return;
    }
    _iIndex++;
}

///////////////////////////////////////////////////////////////////

TC_Multi_HashMap::HashMapIterator::HashMapIterator()
: _pMap(NULL), _iItem(NULL, (uint32_t)-1)
{
}

TC_Multi_HashMap::HashMapIterator::HashMapIterator(TC_Multi_HashMap *pMap, uint32_t iIndex)
: _pMap(pMap), _iItem(_pMap, iIndex)
{
}

TC_Multi_HashMap::HashMapIterator::HashMapIterator(const HashMapIterator &it)
: _pMap(it._pMap),_iItem(it._iItem)
{
}

TC_Multi_HashMap::HashMapIterator& TC_Multi_HashMap::HashMapIterator::operator=(const HashMapIterator &it)
{
    if(this != &it)
    {
        _pMap       = it._pMap;
        _iItem      = it._iItem;
    }

    return (*this);
}

bool TC_Multi_HashMap::HashMapIterator::operator==(const HashMapIterator& mcmi)
{
    if (_iItem.getIndex() != (uint32_t)-1 || mcmi._iItem.getIndex() != (uint32_t)-1)
    {
        return _pMap == mcmi._pMap && _iItem == mcmi._iItem;
    }

    return _pMap == mcmi._pMap;
}

bool TC_Multi_HashMap::HashMapIterator::operator!=(const HashMapIterator& mcmi)
{
    if (_iItem.getIndex() != (uint32_t)-1 || mcmi._iItem.getIndex() != (uint32_t)-1 )
    {
        return _pMap != mcmi._pMap || _iItem != mcmi._iItem;
    }

    return _pMap != mcmi._pMap;
}

TC_Multi_HashMap::HashMapIterator& TC_Multi_HashMap::HashMapIterator::operator++()
{
    _iItem.nextItem();
    return (*this);
}

TC_Multi_HashMap::HashMapIterator TC_Multi_HashMap::HashMapIterator::operator++(int)
{
    HashMapIterator it(*this);
    _iItem.nextItem();
    return it;
}

//////////////////////////////////////////////////////////////////////////////////

void TC_Multi_HashMap::init(void *pAddr)
{
    _pHead          = static_cast<tagMapHead*>(pAddr);
    _pstModifyHead  = static_cast<tagModifyHead*>((void*)((char*)pAddr + sizeof(tagMapHead)));
}

void TC_Multi_HashMap::initDataBlockSize(size_t iMinDataSize, size_t iMaxDataSize, float fFactor)
{
    _iMinDataSize   = iMinDataSize;
    _iMaxDataSize   = iMaxDataSize;
    _fFactor        = fFactor;
}

void TC_Multi_HashMap::create(void *pAddr, size_t iSize)
{
    if(sizeof(tagHashItem)
       + sizeof(tagMainKeyHashItem)
       + sizeof(tagMapHead)
       + sizeof(tagModifyHead)
       + sizeof(TC_MemMultiChunkAllocator::tagChunkAllocatorHead)
       + 10 > iSize)
    {
        throw TC_Multi_HashMap_Exception("[TC_Multi_HashMap::create] mem size not enougth.");
    }

    if(_iMinDataSize == 0 || _iMaxDataSize == 0 || _fFactor < 1.0)
    {
        throw TC_Multi_HashMap_Exception("[TC_Multi_HashMap::create] init data size error:" + TC_Common::tostr(_iMinDataSize) + "|" + TC_Common::tostr(_iMaxDataSize) + "|" + TC_Common::tostr(_fFactor));
    }

    init(pAddr);

    _pHead->_cMaxVersion    = MAX_VERSION;
    _pHead->_cMinVersion    = MIN_VERSION;
    _pHead->_bReadOnly      = false;
    _pHead->_bAutoErase     = true;
    _pHead->_cEraseMode     = ERASEBYGET;
    _pHead->_iMemSize       = iSize;
    _pHead->_iMinDataSize   = _iMinDataSize;
    _pHead->_iMaxDataSize   = _iMaxDataSize;
    _pHead->_fFactor        = _fFactor;
    _pHead->_fHashRatio     = _fHashRatio;
    _pHead->_fMainKeyRatio  = _fMainKeyRatio;
    _pHead->_iElementCount  = 0;
    _pHead->_iEraseCount    = 10;
    _pHead->_iSetHead       = 0;
    _pHead->_iSetTail       = 0;
    _pHead->_iGetHead       = 0;
    _pHead->_iGetTail       = 0;
    _pHead->_iDirtyCount    = 0;
    _pHead->_iDirtyTail     = 0;
    _pHead->_iSyncTime      = 60 * 10;  //缺省十分钟回写一次
    _pHead->_iUsedChunk     = 0;
    _pHead->_iGetCount      = 0;
    _pHead->_iHitCount      = 0;
    _pHead->_iBackupTail    = 0;
    _pHead->_iSyncTail      = 0;
    _pHead->_iMKOnlyKeyCount= 0;
    _pHead->_iOnlyKeyCount    = 0;
    _pHead->_iMaxBlockCount = 0;
    memset(_pHead->_iReserve, 0, sizeof(_pHead->_iReserve));

    size_t iMaxHeadSize = sizeof(Block::tagBlockHead) > sizeof(MainKey::tagMainKeyHead) ? 
                            sizeof(Block::tagBlockHead) : sizeof(MainKey::tagMainKeyHead);

    // 计算平均block大小
    size_t iBlockSize   = (_pHead->_iMinDataSize + _pHead->_iMaxDataSize)/2 + iMaxHeadSize;

    // 计算近似主key Hash个数
    size_t iMHashCount   = (iSize - sizeof(tagMapHead) - sizeof(tagModifyHead) - sizeof(TC_MemChunkAllocator::tagChunkAllocatorHead)) / 
                            ((size_t)(iBlockSize*_fHashRatio*_fMainKeyRatio) + 
                            (size_t)(sizeof(tagHashItem)*_fMainKeyRatio) + sizeof(tagMainKeyHashItem));
    // 采用最近的素数作为hash值
    iMHashCount          = getMinPrimeNumber(iMHashCount);
    // 联合hash个数
    size_t iUHashCount     = getMinPrimeNumber(size_t(iMHashCount * _fMainKeyRatio));

    void *pHashAddr     = (char*)_pHead + sizeof(tagMapHead) + sizeof(tagModifyHead);

    // 主key hash索引区
    size_t iHashMemSize = TC_MemVector<tagMainKeyHashItem>::calcMemSize(iMHashCount);
    if((char*)pHashAddr - (char*)_pHead + iHashMemSize > iSize)
    {
        throw TC_Multi_HashMap_Exception("[TC_Multi_HashMap::create] mem size not enougth.");
    }
    _hashMainKey.create(pHashAddr, iHashMemSize);
    pHashAddr = (char*)pHashAddr + _hashMainKey.getMemSize();

    // 联合hash索引区
    iHashMemSize = TC_MemVector<tagHashItem>::calcMemSize(iUHashCount);
    if((char*)pHashAddr - (char*)_pHead + iHashMemSize > iSize)
    {
        throw TC_Multi_HashMap_Exception("[TC_Multi_HashMap::create] mem size not enougth.");
    }
    _hash.create(pHashAddr, iHashMemSize);

    // chunk数据区
    void *pDataAddr     = (char*)pHashAddr + _hash.getMemSize();
    if((char*)pDataAddr - (char*)_pHead + iMaxHeadSize + _pHead->_iMinDataSize > iSize)
    {
        throw TC_Multi_HashMap_Exception("[TC_Multi_HashMap::create] mem size not enougth.");
    }
    _pDataAllocator->create(pDataAddr, iSize - ((char*)pDataAddr - (char*)_pHead), 
        iMaxHeadSize + _pHead->_iMinDataSize, iMaxHeadSize + _pHead->_iMaxDataSize, _pHead->_fFactor);
}

void TC_Multi_HashMap::connect(void *pAddr, size_t iSize)
{
    init(pAddr);

    if(_pHead->_cMaxVersion != MAX_VERSION || _pHead->_cMinVersion != MIN_VERSION)
    {
        // 版本不匹配
        ostringstream os;
        os << (int)_pHead->_cMaxVersion << "." << (int)_pHead->_cMinVersion << " != " << ((int)MAX_VERSION) << "." << ((int)MIN_VERSION);
        throw TC_Multi_HashMap_Exception("[TC_Multi_HashMap::connect] hash map version not equal:" + os.str() + " (data != code)");
    }

    if(_pHead->_iMemSize != iSize)
    {
        // 内存大小不匹配
        throw TC_Multi_HashMap_Exception("[TC_Multi_HashMap::connect] hash map size not equal:" + TC_Common::tostr(_pHead->_iMemSize) + "!=" + TC_Common::tostr(iSize));
    }

    // 连接主key hash区
    void *pHashAddr = (char*)_pHead + sizeof(tagMapHead) + sizeof(tagModifyHead);
    _hashMainKey.connect(pHashAddr);

    // 连接联合hash区
    pHashAddr = (char*)pHashAddr + _hashMainKey.getMemSize();
    _hash.connect(pHashAddr);

    // 连接chunk区
    void *pDataAddr = (char*)pHashAddr + _hash.getMemSize();
    _pDataAllocator->connect(pDataAddr);

    _iMinDataSize   = _pHead->_iMinDataSize;
    _iMaxDataSize   = _pHead->_iMaxDataSize;
    _fFactor        = _pHead->_fFactor;
    _fHashRatio     = _pHead->_fHashRatio;
    _fMainKeyRatio  = _pHead->_fMainKeyRatio;

    // 恢复可能的错误
    doRecover();
}

int TC_Multi_HashMap::append(void *pAddr, size_t iSize)
{
    if(iSize <= _pHead->_iMemSize)
    {
        return -1;
    }

    init(pAddr);

    if(_pHead->_cMaxVersion != MAX_VERSION || _pHead->_cMinVersion != MIN_VERSION)
    {
        // 版本不匹配
        ostringstream os;
        os << (int)_pHead->_cMaxVersion << "." << (int)_pHead->_cMinVersion << " != " << ((int)MAX_VERSION) << "." << ((int)MIN_VERSION);
        throw TC_Multi_HashMap_Exception("[TC_Multi_HashMap::append] hash map version not equal:" + os.str() + " (data != code)");
    }
    
    _pHead->_iMemSize = iSize;

    // 连接主key hash区
    void *pHashAddr = (char*)_pHead + sizeof(tagMapHead) + sizeof(tagModifyHead);
    _hashMainKey.connect(pHashAddr);

    // 连接联合hash区
    pHashAddr = (char*)pHashAddr + _hashMainKey.getMemSize();
    _hash.connect(pHashAddr);

    // 扩展chunk区
    void *pDataAddr = (char*)pHashAddr + _hash.getMemSize();
    _pDataAllocator->append(pDataAddr, iSize - ((size_t)pDataAddr - (size_t)pAddr));
    
    _iMinDataSize   = _pHead->_iMinDataSize;
    _iMaxDataSize   = _pHead->_iMaxDataSize;
    _fFactor        = _pHead->_fFactor;
    _fHashRatio     = _pHead->_fHashRatio;
    _fMainKeyRatio  = _pHead->_fMainKeyRatio;

    return 0;
}

void TC_Multi_HashMap::clear()
{
    assert(_pHead);

    _pHead->_iElementCount  = 0;
    _pHead->_iSetHead       = 0;
    _pHead->_iSetTail       = 0;
    _pHead->_iGetHead       = 0;
    _pHead->_iGetTail       = 0;
    _pHead->_iDirtyCount    = 0;
    _pHead->_iDirtyTail     = 0;
    _pHead->_iUsedChunk     = 0;
    _pHead->_iGetCount      = 0;
    _pHead->_iHitCount      = 0;
    _pHead->_iBackupTail    = 0;
    _pHead->_iSyncTail      = 0;

    _hashMainKey.clear();
    _hash.clear();

    // 清除错误
    doUpdate();

    _pDataAllocator->rebuild();
}

int TC_Multi_HashMap::dump2file(const string &sFile)
{
    FILE *fp = fopen(sFile.c_str(), "wb");
    if(fp == NULL)
    {
        return RT_DUMP_FILE_ERR;
    }

    size_t ret = fwrite((void*)_pHead, 1, _pHead->_iMemSize, fp);
    fclose(fp);

    if(ret == _pHead->_iMemSize)
    {
        return RT_OK;
    }
    return RT_DUMP_FILE_ERR;
}

int TC_Multi_HashMap::load5file(const string &sFile)
{
    FILE *fp = fopen(sFile.c_str(), "rb");
    if(fp == NULL)
    {
        return RT_LOAD_FILE_ERR;
    }
    fseek(fp, 0L, SEEK_END);
    size_t fs = ftell(fp);
    if(fs != _pHead->_iMemSize)
    {
        fclose(fp);
        return RT_LOAD_FILE_ERR;
    }

    fseek(fp, 0L, SEEK_SET);

    size_t iSize    = 1024*1024*10;
    size_t iLen     = 0;
    char *pBuffer = new char[iSize];
    bool bEof = false;
    while(true)
    {
        int ret = fread(pBuffer, 1, iSize, fp);
        if(ret != (int)iSize)
        {
            if(feof(fp))
            {
                bEof = true;
            }
            else
            {
                fclose(fp);
                delete[] pBuffer;
                return RT_LOAD_FILE_ERR;
            }

        }
        //检查版本
        if(iLen == 0)
        {
            if(pBuffer[0] != MAX_VERSION || pBuffer[1] != MIN_VERSION)
            {
                fclose(fp);
                delete[] pBuffer;
                return RT_VERSION_MISMATCH_ERR;
            }
        }

        memcpy((char*)_pHead + iLen, pBuffer, ret);
        iLen += ret;
        if(bEof)
            break;
    }
    fclose(fp);
    delete[] pBuffer;
    if(iLen == _pHead->_iMemSize)
    {
        return RT_OK;
    }

    return RT_LOAD_FILE_ERR;
}

int TC_Multi_HashMap::checkMainKey(const string &mk)
{
    TC_Multi_HashMap::FailureRecover recover(this);
    
    incGetCount();
    
    int ret          = RT_OK;
    uint32_t index   = mhashIndex(mk);
    if(itemMainKey(index)->_iMainKeyAddr == 0)
    {
        return RT_NO_DATA;
    }
    
    MainKey mainKey(this, itemMainKey(index)->_iMainKeyAddr);
    do 
    {
        string s;
        ret = mainKey.get(s);
        if(ret != RT_OK)
        {
            return ret;
        }
        if(s == mk)
        {
            incHitCount();
            // 找到了
            if(mainKey.getHeadPtr()->_iAddr == 0)
            {
                return RT_ONLY_KEY;
            }
            if(!mainKey.ISFULLDATA())
            {
                return RT_PART_DATA;
            }
            return RT_OK;
        }
        
    } while (mainKey.next());
    
    return RT_NO_DATA;
}

int TC_Multi_HashMap::setFullData(const string &mk, bool bFull)
{
    if(_pHead->_bReadOnly) return RT_READONLY;

    TC_Multi_HashMap::FailureRecover recover(this);
    
    incGetCount();

    int ret          = RT_OK;
    uint32_t index   = mhashIndex(mk);
    if(itemMainKey(index)->_iMainKeyAddr == 0)
    {
        return RT_NO_DATA;
    }
    
    MainKey mainKey(this, itemMainKey(index)->_iMainKeyAddr);
    do 
    {
        string s;
        ret = mainKey.get(s);
        if(ret != RT_OK)
        {
            return ret;
        }
        if(s == mk)
        {
            incHitCount();
            // 找到了
            mainKey.SETFULLDATA(bFull);
            return RT_OK;
        }
        
    } while (mainKey.next());
    
    return RT_NO_DATA;
}

int TC_Multi_HashMap::checkDirty(const string &mk, const string &uk)
{
    TC_Multi_HashMap::FailureRecover recover(this);

    incGetCount();

    int ret          = TC_Multi_HashMap::RT_OK;
    uint32_t index   = hashIndex(mk, uk);
    lock_iterator it = find(mk, uk, index, ret);
    if(ret != TC_Multi_HashMap::RT_OK)
    {
        return ret;
    }

    //没有数据
    if(it == end())
    {
        return TC_Multi_HashMap::RT_NO_DATA;
    }

    incHitCount();

    //只有Key
    if(it->isOnlyKey())
    {
        return TC_Multi_HashMap::RT_ONLY_KEY;
    }

    Block block(this, it->getAddr());
    if (block.isDirty())
    {
        return TC_Multi_HashMap::RT_DIRTY_DATA;
    }

    return TC_Multi_HashMap::RT_OK;
}

int TC_Multi_HashMap::checkDirty(const string &mk)
{
    TC_Multi_HashMap::FailureRecover recover(this);

    incGetCount();

    int ret          = RT_OK;
    uint32_t index   = mhashIndex(mk);
    if(itemMainKey(index)->_iMainKeyAddr == 0)
    {
        return RT_NO_DATA;
    }

    MainKey mainKey(this, itemMainKey(index)->_iMainKeyAddr);
    do 
    {
        string s;
        ret = mainKey.get(s);
        if(ret != RT_OK)
        {
            return ret;
        }
        if(s == mk)
        {
            incHitCount();
            // 找到了
            if(mainKey.getHeadPtr()->_iAddr == 0)
            {
                ret = RT_ONLY_KEY;
            }
            lock_iterator it(this, mainKey.getHeadPtr()->_iAddr, lock_iterator::IT_MKEY, lock_iterator::IT_NEXT);
            while(it != end())
            {
                if(it->isDirty())
                {
                    if(_pHead->_iDirtyTail == 0)
                    {
                        _pHead->_iDirtyTail = mainKey.getHeadPtr()->_iAddr;
                    }
                    return RT_DIRTY_DATA;
                }
                it ++;
            }
            return RT_OK;
        }
        
    } while (mainKey.next());

    return RT_NO_DATA;
}

int TC_Multi_HashMap::setDirty(const string &mk, const string &uk)
{
    TC_Multi_HashMap::FailureRecover recover(this);

    if(_pHead->_bReadOnly) return RT_READONLY;

    incGetCount();

    int ret         = TC_Multi_HashMap::RT_OK;
    uint32_t index    = hashIndex(mk, uk);
    lock_iterator it= find(mk, uk, index, ret);
    if(ret != TC_Multi_HashMap::RT_OK)
    {
        return ret;
    }

    //没有数据或只有Key
    if(it == end())
    {
        return TC_Multi_HashMap::RT_NO_DATA;
    }

    incHitCount();

    //只有Key
    if(it->isOnlyKey())
    {
        return TC_Multi_HashMap::RT_ONLY_KEY;
    }

    Block block(this, it->getAddr());
    block.setDirty(true);
    block.refreshSetList();

    return TC_Multi_HashMap::RT_OK;
}

int TC_Multi_HashMap::setClean(const string &mk, const string &uk)
{
    TC_Multi_HashMap::FailureRecover recover(this);

    if(_pHead->_bReadOnly) return RT_READONLY;

    incGetCount();

    int ret          = TC_Multi_HashMap::RT_OK;
    uint32_t index     = hashIndex(mk, uk);
    lock_iterator it = find(mk, uk, index, ret);
    if(ret != TC_Multi_HashMap::RT_OK)
    {
        return ret;
    }

    //没有数据或只有Key
    if(it == end())
    {
        return TC_Multi_HashMap::RT_NO_DATA;
    }

    incHitCount();

    //只有Key
    if(it->isOnlyKey())
    {
        return TC_Multi_HashMap::RT_ONLY_KEY;
    }

    Block block(this, it->getAddr());
    block.setDirty(false);
    block.refreshSetList();

    return TC_Multi_HashMap::RT_OK;
}

int TC_Multi_HashMap::setSyncTime(const string &mk, const string &uk, time_t iSyncTime)
{
    TC_Multi_HashMap::FailureRecover recover(this);
    
    if(_pHead->_bReadOnly) return RT_READONLY;
    
    incGetCount();
    
    int ret          = TC_Multi_HashMap::RT_OK;
    uint32_t index   = hashIndex(mk, uk);
    lock_iterator it = find(mk, uk, index, ret);
    if(ret != TC_Multi_HashMap::RT_OK)
    {
        return ret;
    }
    
    //没有数据或只有Key
    if(it == end())
    {
        return TC_Multi_HashMap::RT_NO_DATA;
    }
    
    incHitCount();
    
    //只有Key
    if(it->isOnlyKey())
    {
        return TC_Multi_HashMap::RT_ONLY_KEY;
    }
    
    Block block(this, it->getAddr());
    block.setSyncTime(iSyncTime);
    
    return TC_Multi_HashMap::RT_OK;
}

void TC_Multi_HashMap::incMainKeyBlockCount(const string &mk, bool bInc)
{
    uint32_t index = mhashIndex(mk);

    // 找到真正的主key头
    if(itemMainKey(index)->_iMainKeyAddr != 0)
    {
        MainKey mainKey(this, itemMainKey(index)->_iMainKeyAddr);
        do
        {
            string sKey;
            mainKey.get(sKey);
            if(mk == sKey)
            {
                // 找到了，增加计数
                if(bInc)
                {
                    saveValue(&mainKey.getHeadPtr()->_iBlockCount, mainKey.getHeadPtr()->_iBlockCount + 1);
                    updateMaxMainKeyBlockCount(mainKey.getHeadPtr()->_iBlockCount + 1);
                }
                else
                {
                    saveValue(&mainKey.getHeadPtr()->_iBlockCount, mainKey.getHeadPtr()->_iBlockCount - 1);
                    updateMaxMainKeyBlockCount(mainKey.getHeadPtr()->_iBlockCount - 1);
                }
                break;
            }
        }while(mainKey.next());
    }
}

void TC_Multi_HashMap::updateMaxMainKeyBlockCount(size_t iCount)
{
    if(iCount > _pHead->_iMaxBlockCount)
    {
        saveValue(&_pHead->_iMaxBlockCount, iCount);
    }
}

int TC_Multi_HashMap::get(const string &mk, const string &uk, Value &v)
{
    TC_Multi_HashMap::FailureRecover recover(this);

    incGetCount();

    int ret             = RT_OK;

    uint32_t index      = hashIndex(mk, uk);
    lock_iterator it    = find(mk, uk, index, v, ret);

    if(ret != RT_OK && ret != RT_ONLY_KEY)
    {
        return ret;
    }

    if(it == end())
    {
        // 没有数据
        // 查询主key信息
        uint32_t mIndex = mhashIndex(mk);
        uint32_t iAddr = find(mk, mIndex, ret);
        if(ret != RT_OK)
        {
            // 这里有可能返回RT_ONLY_KEY
            return ret;
        }
        if(iAddr != 0)
        {
            MainKey mainKey(this, iAddr);
            if(mainKey.ISFULLDATA())
            {
                // 主key存在，由于cache中的数据必须与数据源(如数据库)保持一致
                // 说明数据源必定没有此记录，返回only key即可，可以使得调用者不必
                // 从数据源再取数据
                return RT_ONLY_KEY;
            }
            // 如果主key存在，但数据不全，则有可能会在数据库中有，应该返回RT_NO_DATA
        }
        return RT_NO_DATA;
    }

    incHitCount();

    // 只有Key
    if(it->isOnlyKey())
    {
        return RT_ONLY_KEY;
    }

    Block block(this, it->getAddr());
    // 如果只读, 则不刷新get链表
    if(!_pHead->_bReadOnly)
    {
        MainKey mainKey(this, block.getBlockHead()->_iMainKey);
        mainKey.refreshGetList();
    }
    return RT_OK;
}

int TC_Multi_HashMap::get(const string &mk, vector<Value> &vs)
{
    TC_Multi_HashMap::FailureRecover recover(this);

    incGetCount();

    int ret = TC_Multi_HashMap::RT_OK;
    
    uint32_t index = mhashIndex(mk);
    uint32_t iMainKeyAddr = find(mk, index, ret);
    if(ret != RT_OK)
    {
        return ret;
    }
    if(iMainKeyAddr == 0)
    {
        // 主key不存在
        return RT_NO_DATA;
    }

    incHitCount();

    MainKey mainKey(this, iMainKeyAddr);
    if(mainKey.getHeadPtr()->_iAddr == 0)
    {
        return RT_ONLY_KEY;
    }
    uint32_t iAddr = mainKey.getHeadPtr()->_iAddr;
    while(iAddr != 0)
    {
        Block block(this, iAddr);
        Value v;
        v._mkey = mk;
        ret = block.getBlockData(v._data);
        if(ret != RT_OK && ret != RT_ONLY_KEY)
        {
            return ret;
        }
        if(ret == RT_OK)
        {
            // 仅取非only key的数据
            vs.push_back(v);
        }
        iAddr = block.getBlockHead()->_iMKBlockNext;
    }

    if(!mainKey.ISFULLDATA())
    {
        return RT_PART_DATA;
    }

    return RT_OK;
}

int TC_Multi_HashMap::get(uint32_t &mh, map<string, vector<Value> > &vs)
{
    int ret = TC_Multi_HashMap::RT_OK;
    
    uint32_t index = mh % _hashMainKey.size();
    uint32_t iMainKeyAddr = itemMainKey(index)->_iMainKeyAddr;
    if(iMainKeyAddr == 0)
    {
        return RT_OK;
    }
    
    MainKey mainKey(this, iMainKeyAddr);
    do
    {
        if(mainKey.getHeadPtr()->_iAddr != 0)
        {
            lock_iterator it(this, mainKey.getHeadPtr()->_iAddr, lock_iterator::IT_MKEY, lock_iterator::IT_NEXT);
            while(it != end())
            {
                Value v;
                ret = it->get(v);
                if(ret != RT_OK && ret != RT_ONLY_KEY)
                {
                    return ret;
                }
                if(ret == RT_OK)
                {
                    // 仅取非only key的数据
                    vs[v._mkey].push_back(v);
                }
                it ++;
            }
        }
    }while(mainKey.next());
    
    return RT_OK;
}

int TC_Multi_HashMap::set(const string &mk, const string &uk, const string &v, 
                          uint8_t iVersion, bool bDirty, DATATYPE eType, bool bHead, vector<Value> &vtData)
{
    TC_Multi_HashMap::FailureRecover recover(this);

    incGetCount();

    if(_pHead->_bReadOnly) return RT_READONLY;

    int ret             = TC_Multi_HashMap::RT_OK;
    uint32_t index      = hashIndex(mk, uk);
    lock_iterator it    = find(mk, uk, index, ret);
    bool bNewBlock = false;

    if(ret != TC_Multi_HashMap::RT_OK)
    {
        return ret;
    }

    incHitCount();

    if(it == end())
    {
        // 数据尚不存在

        // 查找是否已经存在于主key链
        uint32_t mIndex = mhashIndex(mk);        // 主key索引
        uint32_t iMainKeyAddr = find(mk, mIndex, ret);
        if(ret != RT_OK && ret != RT_ONLY_KEY)
        {
            return ret;
        }
        if(iMainKeyAddr == 0)
        {
            // 主key头尚不存在，新建一个
            iMainKeyAddr = _pDataAllocator->allocateMainKeyHead(mIndex, vtData);
            if(iMainKeyAddr == 0)
            {
                return TC_Multi_HashMap::RT_NO_MEMORY;
            }
            // 将主key设置到主key头中
            MainKey mainKey(this, iMainKeyAddr);
            ret = mainKey.set(mk.c_str(), mk.length(), vtData);
            if(ret != TC_Multi_HashMap::RT_OK)
            {
                return ret;
            }
        }

        TC_PackIn pi;
        pi << uk;
        pi << v;
        uint32_t iAllocSize = (uint32_t)pi.length();

        // 先分配Blcok空间, 并获得淘汰的数据
        uint32_t iAddr = _pDataAllocator->allocateMemBlock(iMainKeyAddr, index, bHead, iAllocSize, vtData);
        if(iAddr == 0)
        {
            return TC_Multi_HashMap::RT_NO_MEMORY;
        }

        it = HashMapLockIterator(this, iAddr, HashMapLockIterator::IT_BLOCK, HashMapLockIterator::IT_NEXT);
        bNewBlock = true;
    }

    ret = it->set(mk, uk, v, iVersion, vtData);
    if(ret != TC_Multi_HashMap::RT_OK)
    {
        // set数据失败，可能的情况是空间不够了
        if(bNewBlock)
        {
            // 如果是新set的数据，此时要注意删除前面分配并挂接的block头
            // 否则这个block将永远无法访问，也不能删除，因为block里是一个空的block
            Block block(this, it->getAddr());
            block.erase();
        }
        return ret;
    }

    Block block(this, it->getAddr());
    if(bNewBlock)
    {
        block.setSyncTime(time(NULL));
    }
    block.setDirty(bDirty);

    if(eType != AUTO_DATA)
    {
        MainKey mainKey(this, block.getBlockHead()->_iMainKey);
        mainKey.SETFULLDATA(eType == FULL_DATA);
    }

    block.refreshSetList();

    return TC_Multi_HashMap::RT_OK;
}

int TC_Multi_HashMap::set(const string &mk, const string &uk, DATATYPE eType, bool bHead, vector<Value> &vtData)
{
    TC_Multi_HashMap::FailureRecover recover(this);

    incGetCount();

    if(_pHead->_bReadOnly) return RT_READONLY;

    int ret             = TC_Multi_HashMap::RT_OK;
    uint32_t index        = hashIndex(mk, uk);
    lock_iterator it    = find(mk, uk, index, ret);
    bool bNewBlock = false;

    if(ret != TC_Multi_HashMap::RT_OK)
    {
        return ret;
    }

    incHitCount();

    if(it == end())
    {
        // 查找是否已经存在于主key链
        uint32_t mIndex = mhashIndex(mk);        // 主key索引
        uint32_t iMainKeyAddr = find(mk, mIndex, ret);
        if(ret != RT_OK && ret != RT_ONLY_KEY)
        {
            return ret;
        }
        if(iMainKeyAddr == 0)
        {
            // 主key头尚不存在，新建一个
            iMainKeyAddr = _pDataAllocator->allocateMainKeyHead(mIndex, vtData);
            if(iMainKeyAddr == 0)
            {
                return TC_Multi_HashMap::RT_NO_MEMORY;
            }
            // 将主key设置到主key头中
            MainKey mainKey(this, iMainKeyAddr);
            ret = mainKey.set(mk.c_str(), mk.length(), vtData);
            if(ret != TC_Multi_HashMap::RT_OK)
            {
                return ret;
            }
        }

        TC_PackIn pi;
        pi << uk;
        uint32_t iAllocSize = (uint32_t)pi.length();

        //先分配空间, 并获得淘汰的数据
        uint32_t iAddr = _pDataAllocator->allocateMemBlock(iMainKeyAddr, index, bHead, iAllocSize, vtData);
        if(iAddr == 0)
        {
            return TC_Multi_HashMap::RT_NO_MEMORY;
        }

        it = HashMapLockIterator(this, iAddr, HashMapLockIterator::IT_BLOCK, HashMapLockIterator::IT_NEXT);
        bNewBlock = true;
    }

    ret = it->set(mk, uk, vtData);
    if(ret != TC_Multi_HashMap::RT_OK)
    {
        // set数据失败，可能的情况是空间不够了
        if(bNewBlock)
        {
            // 如果是新set的数据，此时要注意删除前面分配并挂接的block头
            // 否则这个block将永远无法访问，也不能删除，因为block里是一个空的block
            Block block(this, it->getAddr());
            block.erase();
        }
        return ret;
    }

    Block block(this, it->getAddr());
    if(bNewBlock)
    {
        block.setSyncTime(time(NULL));
    }

    if(eType != AUTO_DATA)
    {
        MainKey mainKey(this, block.getBlockHead()->_iMainKey);
        mainKey.SETFULLDATA(eType == FULL_DATA);
    }

    block.refreshSetList();

    return TC_Multi_HashMap::RT_OK;
}

int TC_Multi_HashMap::set(const string &mk, vector<Value> &vtData)
{
    TC_Multi_HashMap::FailureRecover recover(this);

    incGetCount();
    
    if(_pHead->_bReadOnly) return RT_READONLY;
    
    int ret             = TC_Multi_HashMap::RT_OK;

    // 查找是否已经存在该主key
    uint32_t mIndex = mhashIndex(mk);        // 主key索引
    uint32_t iMainKeyAddr = find(mk, mIndex, ret);
    if(ret != RT_OK && ret != RT_ONLY_KEY)
    {
        return ret;
    }

    incHitCount();

    if(iMainKeyAddr == 0)
    {
        // 主key头尚不存在，新建一个
        iMainKeyAddr = _pDataAllocator->allocateMainKeyHead(mIndex, vtData);
        if(iMainKeyAddr == 0)
        {
            return TC_Multi_HashMap::RT_NO_MEMORY;
        }
    }

    MainKey mainKey(this, iMainKeyAddr);

    while(mainKey.getHeadPtr()->_iAddr != 0)
    {
        // 下面还有数据，需要清理这些数据
        Block block(this, mainKey.getHeadPtr()->_iAddr);
        Value value;
        value._mkey = mk;
        int ret = block.getBlockData(value._data);
        if(ret == TC_Multi_HashMap::RT_OK)
        {
            vtData.push_back(value);
        }
        block.erase();
    }
    
    // 将主key设置到主key头中
    ret = mainKey.set(mk.c_str(), mk.length(), vtData);
    if(ret != TC_Multi_HashMap::RT_OK)
    {
        return ret;
    }
    mainKey.refreshGetList();

    return TC_Multi_HashMap::RT_OK;
}

int TC_Multi_HashMap::set(const vector<Value> &vtSet, DATATYPE eType, bool bHead, bool bForce, vector<Value> &vtErased)
{
    // 这里不需要加恢复代码
    //TC_Multi_HashMap::FailureRecover recover(this);

    int ret = RT_OK;

    for(size_t i = 0; i < vtSet.size(); i ++)
    {
        if(bForce || checkMainKey(vtSet[i]._mkey) == RT_NO_DATA)
        {
            // 强制更新或主key下没有数据
            if(vtSet[i]._data._key.empty())
            {
                // 没有联合key，当作主key的only key设置
                ret = set(vtSet[i]._mkey, vtErased);
            }
            else if(vtSet[i]._data._value.empty())
            {
                // 没有value，当作联合主键的only key设置
                ret = set(vtSet[i]._mkey, vtSet[i]._data._key, eType, bHead, vtErased);
            }
            else
            {
                ret = set(vtSet[i]._mkey, vtSet[i]._data._key, vtSet[i]._data._value, 
                    vtSet[i]._data._iVersion, vtSet[i]._data._dirty, eType, bHead, vtErased);
            }
            if(ret != RT_OK)
            {
                return ret;
            }
        }
    }

    return RT_OK;
}

int TC_Multi_HashMap::del(const string &mk, const string &uk, Value &data)
{
    TC_Multi_HashMap::FailureRecover recover(this);

    incGetCount();

    if(_pHead->_bReadOnly) return RT_READONLY;

    int    ret      = RT_OK;
    uint32_t index    = hashIndex(mk, uk);
    lock_iterator it     = find(mk, uk, index, data, ret);
    if(ret != TC_Multi_HashMap::RT_OK && ret != TC_Multi_HashMap::RT_ONLY_KEY)
    {
        return ret;
    }

    if(it == end())
    {
        return TC_Multi_HashMap::RT_NO_DATA;
    }

    incHitCount();

    Block block(this, it->getAddr());
    block.erase();

    return ret;
}

int TC_Multi_HashMap::del(const string &mk, vector<Value> &data)
{
    if(_pHead->_bReadOnly) return RT_READONLY;

    TC_Multi_HashMap::FailureRecover recover(this);

    incGetCount();

    uint32_t iIndex = mhashIndex(mk);
    if(itemMainKey(iIndex)->_iMainKeyAddr != 0)
    {
        incHitCount();

        MainKey mainKey(this, itemMainKey(iIndex)->_iMainKeyAddr);
        do
        {
            string sKey;
            mainKey.get(sKey);
            if(mk == sKey)
            {
                // 找到主key头了
                return mainKey.erase(data);
            }
        }while(mainKey.next());
    }

    return TC_Multi_HashMap::RT_NO_DATA;         
}

int TC_Multi_HashMap::erase(int ratio, vector<Value> &vtData, bool bCheckDirty)
{
    if(_pHead->_bReadOnly) return RT_READONLY;

    TC_Multi_HashMap::FailureRecover recover(this);

    if(ratio <= 0)   ratio = 1;
    if(ratio >= 100) ratio = 100;

    uint32_t iAddr    = _pHead->_iGetTail;
    //到链表头部
    if(iAddr == 0)
    {
        return RT_OK;
    }

    //删除到指定比率了
    if((_pHead->_iUsedChunk * 100. / allBlockChunkCount()) < ratio)
    {
        return RT_OK;
    }

    // 删除是针对主key的
    MainKey mainKey(this, iAddr);
    if(bCheckDirty)
    {
        // 检查脏数据，脏数据不淘汰
        // 只要主key下有一条脏数据就不淘汰
        lock_iterator it(this, mainKey.getHeadPtr()->_iAddr, lock_iterator::IT_MKEY, lock_iterator::IT_NEXT);
        while(it != end())
        {
            if(it->isDirty())
            {
                if(_pHead->_iDirtyTail == 0)
                {
                    _pHead->_iDirtyTail = mainKey.getHeadPtr()->_iAddr;
                }
                return RT_OK;
            }
            it ++;
        }
    }

    // 淘汰主key下的所有数据
    mainKey.erase(vtData);

    return RT_ERASE_OK;
}

void TC_Multi_HashMap::sync()
{
    TC_Multi_HashMap::FailureRecover recover(this);
    
    _pHead->_iSyncTail = _pHead->_iDirtyTail;
}

int TC_Multi_HashMap::sync(time_t iNowTime, Value &data)
{
    TC_Multi_HashMap::FailureRecover recover(this);

    uint32_t iAddr    = _pHead->_iSyncTail;

    //到链表头部了, 返回RT_OK
    if(iAddr == 0)
    {
        return RT_OK;
    }

    Block block(this, iAddr);

    _pHead->_iSyncTail = block.getBlockHead()->_iSetPrev;

    //当前数据是干净数据
    if( !block.isDirty())
    {
        if(_pHead->_iDirtyTail == iAddr)
        {
            _pHead->_iDirtyTail = block.getBlockHead()->_iSetPrev;
        }
        return RT_NONEED_SYNC;
    }

    // 取出主key
    MainKey mainKey(this, block.getBlockHead()->_iMainKey);
    mainKey.get(data._mkey);

    int ret = block.getBlockData(data._data);
    if(ret != TC_Multi_HashMap::RT_OK)
    {
        //没有获取完整的记录
        if(_pHead->_iDirtyTail == iAddr)
        {
            _pHead->_iDirtyTail = block.getBlockHead()->_iSetPrev;
        }
        return ret;
    }

    //脏数据且超过_pHead->_iSyncTime没有回写, 需要回写
    if(_pHead->_iSyncTime + data._data._synct < iNowTime && block.isDirty())
    {
        block.setDirty(false);
        block.setSyncTime(iNowTime);

        if(_pHead->_iDirtyTail == iAddr)
        {
            _pHead->_iDirtyTail = block.getBlockHead()->_iSetPrev;
        }

        return RT_NEED_SYNC;
    }

    //脏数据, 但是不需要回写, 脏链表不往前推
    return RT_NONEED_SYNC;
}

void TC_Multi_HashMap::backup(bool bForceFromBegin)
{
    TC_Multi_HashMap::FailureRecover recover(this);

    if(bForceFromBegin || _pHead->_iBackupTail == 0)
    {
        //移动备份指针到Get链尾部, 准备开始备份数据
        _pHead->_iBackupTail = _pHead->_iGetTail;
    }
}

int TC_Multi_HashMap::backup(vector<Value> &vtData)
{
    TC_Multi_HashMap::FailureRecover recover(this);

    uint32_t iAddr    = _pHead->_iBackupTail;

    //到链表头部了, 返回RT_OK
    if(iAddr == 0)
    {
        return RT_OK;
    }

    // 取出主key
    MainKey mainKey(this, iAddr);
    string mk;
    mainKey.get(mk);

    // 一次备份整个主key
    int ret = RT_OK;
    lock_iterator it(this, mainKey.getHeadPtr()->_iAddr, lock_iterator::IT_MKEY, lock_iterator::IT_NEXT);
    while(it != end())
    {
        Block block(this, it->_iAddr);
        Value value;
        value._mkey = mk;
        ret = block.getBlockData(value._data);
        if(ret != RT_OK)
        {
            break;
        }
        vtData.push_back(value);
        
        it++;
    }

    //迁移一次
    _pHead->_iBackupTail = mainKey.getHeadPtr()->_iGetPrev;

    if(ret == RT_OK)
    {
        return RT_NEED_BACKUP;
    }

    return ret;
}

TC_Multi_HashMap::lock_iterator TC_Multi_HashMap::begin()
{
    TC_Multi_HashMap::FailureRecover recover(this);

    for(size_t i = 0; i < _hash.size(); i++)
    {
        tagHashItem &hashItem = _hash[i];
        if(hashItem._iBlockAddr != 0)
        {
            return lock_iterator(this, hashItem._iBlockAddr, lock_iterator::IT_BLOCK, lock_iterator::IT_NEXT);
        }
    }

    return end();
}

TC_Multi_HashMap::lock_iterator TC_Multi_HashMap::rbegin()
{
    TC_Multi_HashMap::FailureRecover recover(this);

    for(size_t i = _hash.size(); i > 0; i--)
    {
        tagHashItem &hashItem = _hash[i-1];
        if(hashItem._iBlockAddr != 0)
        {
            Block block(this, hashItem._iBlockAddr);
            return lock_iterator(this, block.getLastBlockHead(true), lock_iterator::IT_BLOCK, lock_iterator::IT_PREV);
        }
    }

    return end();
}

TC_Multi_HashMap::lock_iterator TC_Multi_HashMap::beginSetTime()
{
    TC_Multi_HashMap::FailureRecover recover(this);
    return lock_iterator(this, _pHead->_iSetHead, lock_iterator::IT_SET, lock_iterator::IT_NEXT);
}

TC_Multi_HashMap::lock_iterator TC_Multi_HashMap::rbeginSetTime()
{
    TC_Multi_HashMap::FailureRecover recover(this);
    return lock_iterator(this, _pHead->_iSetTail, lock_iterator::IT_SET, lock_iterator::IT_PREV);
}

TC_Multi_HashMap::lock_iterator TC_Multi_HashMap::beginGetTime()
{
    TC_Multi_HashMap::FailureRecover recover(this);
    if(_pHead->_iGetHead != 0)
    {
        MainKey mainKey(this, _pHead->_iGetHead);
        return lock_iterator(this, mainKey.getHeadPtr()->_iAddr, lock_iterator::IT_GET, lock_iterator::IT_NEXT);
    }
    else
    {
        return lock_iterator(this, 0, lock_iterator::IT_GET, lock_iterator::IT_NEXT);
    }
}

TC_Multi_HashMap::lock_iterator TC_Multi_HashMap::rbeginGetTime()
{
    TC_Multi_HashMap::FailureRecover recover(this);
    if(_pHead->_iGetTail != 0)
    {
        MainKey mainKey(this, _pHead->_iGetTail);
        // 找到主key链上的最后一个数据
        uint32_t iAddr = mainKey.getHeadPtr()->_iAddr;
        if(iAddr)
        {
            Block block(this, iAddr);
            while(block.getBlockHead()->_iMKBlockNext)
            {
                iAddr = block.getBlockHead()->_iMKBlockNext;
                block = Block(this, iAddr);
            }
        }
        return lock_iterator(this, iAddr, lock_iterator::IT_GET, lock_iterator::IT_PREV);
    }
    else
    {
        return lock_iterator(this, 0, lock_iterator::IT_GET, lock_iterator::IT_PREV);
    }
}

TC_Multi_HashMap::lock_iterator TC_Multi_HashMap::beginDirty()
{
    TC_Multi_HashMap::FailureRecover recover(this);
    return lock_iterator(this, _pHead->_iDirtyTail, lock_iterator::IT_SET, lock_iterator::IT_PREV);
}

TC_Multi_HashMap::hash_iterator TC_Multi_HashMap::hashBegin()
{
    TC_Multi_HashMap::FailureRecover recover(this);
    return hash_iterator(this, 0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

string TC_Multi_HashMap::desc()
{
    ostringstream s;
    {
        s << "[Version          = "   << (int)_pHead->_cMaxVersion << "." << (int)_pHead->_cMinVersion << "]" << endl;
        s << "[ReadOnly         = "   << _pHead->_bReadOnly    << "]" << endl;
        s << "[AutoErase        = "   << _pHead->_bAutoErase   << "]" << endl;
        s << "[MemSize          = "   << _pHead->_iMemSize          << "]" << endl;
        s << "[Capacity         = "   << _pDataAllocator->getCapacity()  << "]" << endl;
        s << "[SingleBlockCount = "   << TC_Common::tostr(singleBlockChunkCount())         << "]" << endl;
        s << "[AllBlockChunk    = "   << allBlockChunkCount()       << "]" << endl;
        s << "[UsedChunk        = "   << _pHead->_iUsedChunk        << "]" << endl;
        s << "[FreeChunk        = "   << allBlockChunkCount() - _pHead->_iUsedChunk        << "]" << endl;
        s << "[MinDataSize      = "   << _pHead->_iMinDataSize      << "]" << endl;
        s << "[MaxDataSize      = "   << _pHead->_iMaxDataSize      << "]" << endl;
        s << "[HashCount        = "   << _hash.size()               << "]" << endl;
        s << "[MainKeyHashCount = "   << _hashMainKey.size()        << "]" << endl;
        s << "[HashRatio        = "   << _pHead->_fHashRatio            << "]" << endl;
        s << "[MainKeyRatio     = "   << _pHead->_fMainKeyRatio         << "]" << endl;
        s << "[ElementCount     = "   << _pHead->_iElementCount     << "]" << endl;
        s << "[SetHead          = "   << _pHead->_iSetHead       << "]" << endl;
        s << "[SetTail          = "   << _pHead->_iSetTail       << "]" << endl;
        s << "[GetHead          = "   << _pHead->_iGetHead       << "]" << endl;
        s << "[GetTail          = "   << _pHead->_iGetTail       << "]" << endl;
        s << "[DirtyTail        = "   << _pHead->_iDirtyTail        << "]" << endl;
        s << "[SyncTail         = "   << _pHead->_iSyncTail        << "]" << endl;
        s << "[SyncTime         = "   << _pHead->_iSyncTime        << "]" << endl;
        s << "[BackupTail       = "   << _pHead->_iBackupTail      << "]" << endl;
        s << "[DirtyCount       = "   << _pHead->_iDirtyCount       << "]" << endl;
        s << "[GetCount         = "   << _pHead->_iGetCount         << "]" << endl;
        s << "[HitCount         = "   << _pHead->_iHitCount         << "]" << endl;
        s << "[HitRatio         = "   << (_pHead->_iGetCount == 0 ? 0 :_pHead->_iHitCount * 100 / _pHead->_iGetCount) << "]" << endl;
        s << "[OnlyKeyCount     = "      << _pHead->_iOnlyKeyCount        << "]" << endl;
        s << "[MKOnlyKeyCount   = "      << _pHead->_iMKOnlyKeyCount    << "]" << endl;
        s << "[MaxMKBlockCount  = "      << _pHead->_iMaxBlockCount    << "]" << endl;
        s << "[ModifyStatus     = "   << (int)_pstModifyHead->_cModifyStatus     << "]" << endl;
        s << "[ModifyIndex      = "   << _pstModifyHead->_iNowIndex << "]" << endl;
    }

    uint32_t iMaxHash;
    uint32_t iMinHash;
    float fAvgHash;

    analyseHash(iMaxHash, iMinHash, fAvgHash);
    {
        s << "[MaxHash          = "   << iMaxHash      << "]" << endl;
        s << "[MinHash          = "   << iMinHash     << "]" << endl;
        s << "[AvgHash          = "   << fAvgHash     << "]" << endl;
    }

    analyseHashM(iMaxHash, iMinHash, fAvgHash);
    {
        s << "[MaxMainkeyHash   = "   << iMaxHash      << "]" << endl;
        s << "[MinMainKeyHash   = "   << iMinHash     << "]" << endl;
        s << "[AvgMainKeyHash   = "   << fAvgHash     << "]" << endl;
    }

    vector<TC_MemChunk::tagChunkHead> vChunkHead = _pDataAllocator->getBlockDetail();

    s << "*************************************************************************" << endl;
    s << "[DiffBlockCount   = "   << vChunkHead.size()  << "]" << endl;
    for(size_t i = 0; i < vChunkHead.size(); i++)
    {
        s << "[BlockSize        = "   << vChunkHead[i]._iBlockSize      << "] ";
        s << "[BlockCount       = "   << vChunkHead[i]._iBlockCount     << "] ";
        s << "[BlockAvailable   = "   << vChunkHead[i]._blockAvailable  << "]" << endl;
    }

    return s.str();
}

size_t TC_Multi_HashMap::eraseExcept(uint32_t iNowAddr, vector<Value> &vtData)
{
    //不能被淘汰
    if(!_pHead->_bAutoErase) return 0;

    size_t n = _pHead->_iEraseCount;
    if(n == 0) n = 10;
    size_t d = n;

    while (d != 0)
    {
        uint32_t iAddr;

        //判断按照哪种方式淘汰
        if(_pHead->_cEraseMode == TC_Multi_HashMap::ERASEBYSET)
        {
            // 按set链淘汰
            iAddr = _pHead->_iSetTail;
            if (iAddr == 0)
            {
                break;
            }
            // set链是block链，换算成主key头的地址
            Block block(this, iAddr);
            iAddr = block.getBlockHead()->_iMainKey;
        }
        else
        {
            // 按get链淘汰
            iAddr = _pHead->_iGetTail;
            if (iAddr == 0)
            {
                break;
            }
        }
        
        // 不管是按get链淘汰还是按set链淘汰，都是要淘汰主key下所有数据
        MainKey mainKey(this, iAddr);
        if(iNowAddr == iAddr)
        {
            // 当前主key链中有正在分配的block，跳过
            iAddr = mainKey.getHeadPtr()->_iGetPrev;
        }
        if(iAddr == 0)
        {
            break;
        }

        uint32_t iBlockCount = mainKey.getHeadPtr()->_iBlockCount;
        if(d >= iBlockCount)
        {
            // 淘汰主key链下的所有数据
            int ret = mainKey.erase(vtData);
            if(ret == RT_OK)
            {
                d -= iBlockCount;
            }
        }
        else
        {
            if(d == n)
            {
                // 设置的要删除的数量太小，没有满足条件的主key，直接删除
                int ret = mainKey.erase(vtData);
                if(ret == RT_OK)
                {
                    n = d + iBlockCount;    // 使返回正确的数据量
                }
            }
            break;
        }
    }

    return n-d;
}

uint32_t TC_Multi_HashMap::hashIndex(const string &mk, const string &uk)
{
    // mk是主key，uk是除主key外的辅key，二者加起来作为联合主键
    return hashIndex(mk + uk);
}

uint32_t TC_Multi_HashMap::hashIndex(const string& k)
{
    return _hashf(k) % _hash.size();
}

uint32_t TC_Multi_HashMap::mhashIndex(const string &mk)
{
    if(_mhashf)
    {
        return _mhashf(mk) % _hashMainKey.size();
    }
    else
    {
        // 如果没有单独指定主key hash函数，则使用联合主键的hash函数
        return _hashf(mk) % _hashMainKey.size();
    }
}

TC_Multi_HashMap::lock_iterator TC_Multi_HashMap::find(const string &mk, const string &uk)
{
    TC_Multi_HashMap::FailureRecover recover(this);

    incGetCount();

    uint32_t index = hashIndex(mk, uk);
    int ret = TC_Multi_HashMap::RT_OK;
    
    if(item(index)->_iBlockAddr == 0)
    {
        return end();
    }
    
    Block mb(this, item(index)->_iBlockAddr);
    while(true)
    {
        HashMapLockItem mcmdi(this, mb.getHead());
        if(mcmdi.equal(mk, uk, ret))
        {
            // 找到了，不仅索引相同，key也相同
            incHitCount();
            return lock_iterator(this, mb.getHead(), lock_iterator::IT_BLOCK, lock_iterator::IT_NEXT);
        }
        
        if (!mb.nextBlock())
        {
            return end();
        }
    }
    
    return end();
}

TC_Multi_HashMap::lock_iterator TC_Multi_HashMap::find(const string &mk, const string &uk, uint32_t index, Value &v, int &ret)
{
    ret = TC_Multi_HashMap::RT_OK;

    if(item(index)->_iBlockAddr == 0)
    {
        return end();
    }

    Block mb(this, item(index)->_iBlockAddr);
    while(true)
    {
        HashMapLockItem mcmdi(this, mb.getHead());
        if(mcmdi.equal(mk, uk, v, ret))
        {
            // 找到了，不仅索引相同，key也相同
            return lock_iterator(this, mb.getHead(), lock_iterator::IT_BLOCK, lock_iterator::IT_NEXT);
        }

        if (!mb.nextBlock())
        {
            return end();
        }
    }

    return end();
}

TC_Multi_HashMap::lock_iterator TC_Multi_HashMap::find(const string &mk, const string &uk, uint32_t index, int &ret)
{
    ret = TC_Multi_HashMap::RT_OK;

    if(item(index)->_iBlockAddr == 0)
    {
        return end();
    }

    Block mb(this, item(index)->_iBlockAddr);
    while(true)
    {
        HashMapLockItem mcmdi(this, mb.getHead());
        if(mcmdi.equal(mk, uk, ret))
        {
            // 找到了，不仅索引相同，key也相同
            return lock_iterator(this, mb.getHead(), lock_iterator::IT_BLOCK, lock_iterator::IT_NEXT);
        }

        if (!mb.nextBlock())
        {
            return end();
        }
    }

    return end();
}

uint32_t TC_Multi_HashMap::find(const string &mk, uint32_t index, int &ret)
{
    ret = TC_Multi_HashMap::RT_OK;

    if(itemMainKey(index)->_iMainKeyAddr == 0)
    {
        return 0;
    }

    MainKey mainKey(this, itemMainKey(index)->_iMainKeyAddr);
    do 
    {
        string s;
        ret = mainKey.get(s);
        if(ret != RT_OK)
        {
            return 0;
        }
        if(s == mk)
        {
            // 找到了
            if(mainKey.getHeadPtr()->_iAddr == 0)
            {
                ret = RT_ONLY_KEY;
            }
            return mainKey.getHead();
        }

    } while (mainKey.next());

    return 0;
}

size_t TC_Multi_HashMap::count(const string &mk)
{
    TC_Multi_HashMap::FailureRecover recover(this);

    incGetCount();

    uint32_t iIndex = mhashIndex(mk);
    if(itemMainKey(iIndex)->_iMainKeyAddr != 0)
    {
        MainKey mainKey(this, itemMainKey(iIndex)->_iMainKeyAddr);
        do
        {
            string sKey;
            mainKey.get(sKey);
            if(mk == sKey)
            {
                // 找到对应的主key了
                incHitCount();
                return mainKey.getHeadPtr()->_iBlockCount;
            }
        }while(mainKey.next());
    }
    return 0;
}

TC_Multi_HashMap::lock_iterator TC_Multi_HashMap::find(const string &mk)
{
    TC_Multi_HashMap::FailureRecover recover(this);

    incGetCount();

    uint32_t iIndex = mhashIndex(mk);
    if(itemMainKey(iIndex)->_iMainKeyAddr != 0)
    {
        MainKey mainKey(this, itemMainKey(iIndex)->_iMainKeyAddr);
        do
        {
            string sKey;
            mainKey.get(sKey);
            if(mk == sKey)
            {
                // 找到对应的主key了
                incHitCount();
                if(mainKey.getHeadPtr()->_iAddr != 0)
                {
                    // 主key下面有数据
                    return lock_iterator(this, mainKey.getHeadPtr()->_iAddr, lock_iterator::IT_MKEY, lock_iterator::IT_NEXT);
                }
                break;
            }
        }while(mainKey.next());
    }
    return end();
}

void TC_Multi_HashMap::analyseHash(uint32_t &iMaxHash, uint32_t &iMinHash, float &fAvgHash)
{
    iMaxHash = 0;
    iMinHash = (uint32_t)-1;

    fAvgHash = 0;

    uint32_t n = 0;
    for(uint32_t i = 0; i < _hash.size(); i++)
    {
        iMaxHash = max(_hash[i]._iListCount, iMaxHash);
        iMinHash = min(_hash[i]._iListCount, iMinHash);
        //平均值只统计非0的
        if(_hash[i]._iListCount != 0)
        {
            n++;
            fAvgHash  += _hash[i]._iListCount;
        }
    }

    if(n != 0)
    {
        fAvgHash = fAvgHash / n;
    }
}

void TC_Multi_HashMap::analyseHashM(uint32_t &iMaxHash, uint32_t &iMinHash, float &fAvgHash)
{
    iMaxHash = 0;
    iMinHash = (uint32_t)-1;
    
    fAvgHash = 0;
    
    uint32_t n = 0;
    for(uint32_t i = 0; i < _hashMainKey.size(); i++)
    {
        iMaxHash = max(_hashMainKey[i]._iListCount, iMaxHash);
        iMinHash = min(_hashMainKey[i]._iListCount, iMinHash);
        //平均值只统计非0的
        if(_hashMainKey[i]._iListCount != 0)
        {
            n++;
            fAvgHash  += _hashMainKey[i]._iListCount;
        }
    }
    
    if(n != 0)
    {
        fAvgHash = fAvgHash / n;
    }
}

void TC_Multi_HashMap::doRecover()
{
    //==1 copy过程中, 程序失败, 恢复原数据
    if(_pstModifyHead->_cModifyStatus == 1)
    {
        for(int i = _pstModifyHead->_iNowIndex-1; i >=0; i--)
        {
            if(_pstModifyHead->_stModifyData[i]._cBytes == sizeof(uint64_t))
            {
                *(uint64_t*)((char*)_pHead + _pstModifyHead->_stModifyData[i]._iModifyAddr) = _pstModifyHead->_stModifyData[i]._iModifyValue;
            }
            //#if __WORDSIZE == 64
            else if(_pstModifyHead->_stModifyData[i]._cBytes == sizeof(uint32_t))
            {
                *(uint32_t*)((char*)_pHead + _pstModifyHead->_stModifyData[i]._iModifyAddr) = (uint32_t)_pstModifyHead->_stModifyData[i]._iModifyValue;
            }
            //#endif
            else if(_pstModifyHead->_stModifyData[i]._cBytes == sizeof(uint8_t))
            {
                *(uint8_t*)((char*)_pHead + _pstModifyHead->_stModifyData[i]._iModifyAddr) = (bool)_pstModifyHead->_stModifyData[i]._iModifyValue;
            }
            else
            {
                assert(true);
            }
        }
        _pstModifyHead->_iNowIndex        = 0;
        _pstModifyHead->_cModifyStatus    = 0;
    }
}

void TC_Multi_HashMap::doUpdate()
{
    //==2,已经修改成功, 清除
    if(_pstModifyHead->_cModifyStatus == 1)
    {
        _pstModifyHead->_iNowIndex        = 0;
        _pstModifyHead->_cModifyStatus    = 0;
    }
}

size_t TC_Multi_HashMap::getMinPrimeNumber(size_t n)
{
    while(true)
    {
        if(TC_Common::isPrimeNumber(n))
        {
            return n;
        }
        ++n;
    }
    return 3;
}

size_t TC_Multi_HashMap::checkBadBlock(uint32_t iHash, bool bRepair)
{
    size_t iCount = 0;
    if(iHash >= _hash.size())
    {
        return 0;
    }

    TC_Multi_HashMap::FailureRecover recover(this);

check:
    uint32_t iAddr = item(iHash)->_iBlockAddr;
    Block block(this, iAddr);
    while(block.getHead() != 0)
    {
        BlockData blkData;
        int ret = block.getBlockData(blkData);
        if(ret != RT_OK && ret != RT_ONLY_KEY)
        {
            iCount ++;
            if(bRepair)
            {
                // 删除这个block
                block.erase();
                goto check;
            }
        }
        if(!block.nextBlock())
        {
            break;
        }
    }

    return iCount;
}

}

