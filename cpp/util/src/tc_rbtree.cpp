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

#include "util/tc_rbtree.h"
#include "util/tc_pack.h"
#include "util/tc_common.h"
#include <string.h>

namespace tars
{

bool TC_RBTree::Block::hasLeft()
{
    return _pHead->_iLeftChild != 0;
}

bool TC_RBTree::Block::moveToLeft()
{
    _iHead = getBlockHead()->_iLeftChild;
    _pHead = getBlockHead(_iHead);
    return _iHead != 0;
}

bool TC_RBTree::Block::hasRight()
{
    return _pHead->_iRightChild != 0;
}

bool TC_RBTree::Block::moveToRight()
{
    _iHead = getBlockHead()->_iRightChild;
    _pHead = getBlockHead(_iHead);
    return _iHead != 0;
}

bool TC_RBTree::Block::hasParent()
{
    return _pHead->_iParent != 0;
}

bool TC_RBTree::Block::moveToParent()
{
    _iHead = getBlockHead()->_iParent;
    _pHead = getBlockHead(_iHead);
    return _iHead != 0;
}

/*-----------------------------------------------------------
|     i            right
|   / \    ==>     /  \
|   a  right      i    y
|       / \           / \
|       b  y         a   b
 -----------------------------------------------------------*/

void TC_RBTree::Block::rotateLeft(TC_RBTree::Block::tagBlockHead *i, uint32_t iAddr)
{
    assert(i != NULL);

//    assert(iAddr == _pMap->getRelative(i));

    tagBlockHead *r = getBlockHead(i->_iRightChild);
    uint32_t iRAddr = i->_iRightChild; 

    tagBlockHead *p = getBlockHead(i->_iParent);

    assert(r != NULL);

    _pMap->saveValue(&i->_iRightChild, r->_iLeftChild);
    if(r->_iLeftChild != 0)
    {
        _pMap->saveValue(&getBlockHead(r->_iLeftChild)->_iParent, iAddr);
    }

    _pMap->saveValue(&r->_iLeftChild, iAddr);
    _pMap->saveValue(&r->_iParent, i->_iParent);

    if(p != NULL)
    {
        if(iAddr == p->_iLeftChild)
        {
            _pMap->saveValue(&p->_iLeftChild, iRAddr);
        }
        else
        {
            _pMap->saveValue(&p->_iRightChild, iRAddr);
        }
    }
    else
    {
        _pMap->saveValue(&_pMap->_pHead->_iRootAddr, iRAddr);
    }

    _pMap->saveValue(&i->_iParent, iRAddr);
}

/*-----------------------------------------------------------
|         i           left
|       / \            / \
|    left  y   ==>    a   node
|   / \               / \
|  a   b             b   y
-----------------------------------------------------------*/
void TC_RBTree::Block::rotateRight(TC_RBTree::Block::tagBlockHead *i, uint32_t iAddr)
{
    assert(i != NULL);
//    assert(iAddr == _pMap->getRelative(i));

    tagBlockHead *l = getBlockHead(i->_iLeftChild);
    uint32_t iLAddr = i->_iLeftChild; 

    tagBlockHead *p = getBlockHead(i->_iParent);

    assert(l != NULL);

    _pMap->saveValue(&i->_iLeftChild, l->_iRightChild);
    if(l->_iRightChild != 0)
    {
        _pMap->saveValue(&getBlockHead(l->_iRightChild)->_iParent, iAddr);
    }

    _pMap->saveValue(&l->_iRightChild, iAddr);
    _pMap->saveValue(&l->_iParent, i->_iParent);
    
    if(p != NULL)
    {
        if(iAddr == p->_iRightChild)
        {   
            _pMap->saveValue(&p->_iRightChild, iLAddr);
        }
        else
        {
            _pMap->saveValue(&p->_iLeftChild, iLAddr);
        }
    }
    else
    {
        _pMap->saveValue(&_pMap->_pHead->_iRootAddr, iLAddr);
    }

    _pMap->saveValue(&i->_iParent, iLAddr);
}

void TC_RBTree::Block::eraseFixUp(TC_RBTree::Block::tagBlockHead *i, uint32_t iAddr, TC_RBTree::Block::tagBlockHead *p, uint32_t iPAddr)
{
//    assert(iAddr == _pMap->getRelative(i));
//    assert(iPAddr == _pMap->getRelative(p));

    tagBlockHead *o;
    uint32_t iOAddr;

    while(((!i || i->_iColor == BLACK)) && i != getBlockHead(_pMap->_pHead->_iRootAddr))
    {
        assert(p != NULL);

        if(p->_iLeftChild == iAddr)//_pMap->getRelative(i))
        {
            o       = getBlockHead(p->_iRightChild);
            iOAddr  = p->_iRightChild;

            if(o->_iColor == RED)
            {
                _pMap->saveValue(&o->_iColor, BLACK);
                _pMap->saveValue(&p->_iColor, RED);

                rotateLeft(p, iPAddr);

                o       = getBlockHead(p->_iRightChild);
                iOAddr  = p->_iRightChild;
            }

            if((o->_iLeftChild == 0 || getBlockHead(o->_iLeftChild)->_iColor == BLACK)
                && (o->_iRightChild == 0 || getBlockHead(o->_iRightChild)->_iColor == BLACK))
            {
                _pMap->saveValue(&o->_iColor, RED);

                i       = p;
                iAddr   = iPAddr;

                p       = getBlockHead(i->_iParent);
                iPAddr  = i->_iParent;
            }
            else
            {
                if(o->_iRightChild == 0 || getBlockHead(o->_iRightChild)->_iColor == BLACK)
                {
                    _pMap->saveValue(&getBlockHead(o->_iLeftChild)->_iColor, BLACK);

                    _pMap->saveValue(&o->_iColor, RED);

                    rotateRight(o, iOAddr);

                    o       = getBlockHead(p->_iRightChild);
                    iOAddr  = p->_iRightChild;
                }

                _pMap->saveValue(&o->_iColor, p->_iColor);

                _pMap->saveValue(&p->_iColor, BLACK);

                _pMap->saveValue(&getBlockHead(o->_iRightChild)->_iColor, BLACK);
                
                rotateLeft(p, iPAddr);
                
                i       = getBlockHead(_pMap->_pHead->_iRootAddr);
                iAddr   = _pMap->_pHead->_iRootAddr;

                break;
            }
        }
        else
        {
            o       = getBlockHead(p->_iLeftChild);
            iOAddr  = p->_iLeftChild;

            if(o->_iColor == RED)
            {
                _pMap->saveValue(&o->_iColor, BLACK);

                _pMap->saveValue(&p->_iColor, RED);

                rotateRight(p, iPAddr);

                o       = getBlockHead(p->_iLeftChild);
                iOAddr  = p->_iLeftChild;
            }

            if((o->_iLeftChild == 0 || getBlockHead(o->_iLeftChild)->_iColor == BLACK)
                && (o->_iRightChild == 0 || getBlockHead(o->_iRightChild)->_iColor == BLACK))
            {
                _pMap->saveValue(&o->_iColor, RED);

                i       = p;
                iAddr   = iPAddr;

                p       = getBlockHead(i->_iParent);
                iPAddr  = i->_iParent;
            }
            else
            {
                if(o->_iLeftChild == 0 || getBlockHead(o->_iLeftChild)->_iColor == BLACK)
                {
                    _pMap->saveValue(&getBlockHead(o->_iRightChild)->_iColor, BLACK);

                    _pMap->saveValue(&o->_iColor, RED);

                    rotateLeft(o, iOAddr);

                    o       = getBlockHead(p->_iLeftChild);
                    iOAddr  = p->_iLeftChild;
                }

                _pMap->saveValue(&o->_iColor, p->_iColor);

                _pMap->saveValue(&p->_iColor, BLACK);

                _pMap->saveValue(&getBlockHead(o->_iLeftChild)->_iColor, BLACK);

                rotateRight(p, iPAddr);

                i       = getBlockHead(_pMap->_pHead->_iRootAddr);
                iAddr   = _pMap->_pHead->_iRootAddr;

                break;
            }
        }
    }

    if(i)
    {
        _pMap->saveValue(&i->_iColor, BLACK);
    }
}

void TC_RBTree::Block::erase(TC_RBTree::Block::tagBlockHead *i, uint32_t iAddr)
{
    assert(i != NULL);
//    assert(iAddr == _pMap->getRelative(i));

    tagBlockHead *c = NULL;
    uint32_t iCAddr = 0;
    tagBlockHead *p = NULL;
    uint32_t iPAddr = 0;

    char color;

    if(i->_iLeftChild == 0)
    {
        c = getBlockHead(i->_iRightChild);
        iCAddr = i->_iRightChild;
    }
    else if(i->_iRightChild == 0)
    {
        c = getBlockHead(i->_iLeftChild);
        iCAddr = i->_iLeftChild;
    }
    else
    {                    
        tagBlockHead *old;
        uint32_t iOldAddr;
        tagBlockHead *left;

        old         = i;
        iOldAddr    = iAddr;

        //获取右子树的最左节点
        iAddr   = i->_iRightChild;
        i       = getBlockHead(i->_iRightChild);

        while((left = getBlockHead(i->_iLeftChild)) != NULL)
        {
            iAddr = i->_iLeftChild;
            i     = left;
        }

        c       = getBlockHead(i->_iRightChild);
        iCAddr  = i->_iRightChild;

        p       = getBlockHead(i->_iParent);
        iPAddr  = i->_iParent;

        color   = i->_iColor;

        if(c != NULL)
        {
            _pMap->saveValue(&c->_iParent, iPAddr);
        }

        if(p != NULL)
        {
            if(p->_iLeftChild == iAddr)
            {
                _pMap->saveValue(&p->_iLeftChild, iCAddr);
            }
            else
            {
                _pMap->saveValue(&p->_iRightChild, iCAddr);
            }
        }
        else
        {
            _pMap->saveValue(&_pMap->_pHead->_iRootAddr, iCAddr);
        }

        if(i->_iParent == iOldAddr)
        {
            p       = i;
            iPAddr  = iAddr;
        }

        _pMap->saveValue(&i->_iParent, old->_iParent);
        _pMap->saveValue(&i->_iColor, old->_iColor);
        _pMap->saveValue(&i->_iRightChild, old->_iRightChild);
        _pMap->saveValue(&i->_iLeftChild, old->_iLeftChild);

        if(old->_iParent != 0)
        {
            tagBlockHead *op = getBlockHead(old->_iParent);
            if( op->_iLeftChild == iOldAddr)
            {
                _pMap->saveValue(&op->_iLeftChild, iAddr);
            }
            else
            {
                _pMap->saveValue(&op->_iRightChild, iAddr);
            }
        }
        else
        {
            _pMap->saveValue(&_pMap->_pHead->_iRootAddr, iAddr);
        }

        _pMap->saveValue(&getBlockHead(old->_iLeftChild)->_iParent, iAddr);
        if(old->_iRightChild)
        {
            _pMap->saveValue(&getBlockHead(old->_iRightChild)->_iParent, iAddr);
        }

        goto COLOR;
    }

    p       = getBlockHead(i->_iParent);
    iPAddr  = i->_iParent;
    
    color   = i->_iColor;

    if(c != NULL)
    {
        _pMap->saveValue(&c->_iParent, iPAddr);
    }

    if(p != NULL)
    {
        if(p->_iLeftChild == iAddr)
        {
            _pMap->saveValue(&p->_iLeftChild, iCAddr);
        }
        else
        {
            _pMap->saveValue(&p->_iRightChild, iCAddr);
        }
    }
    else
    {
        _pMap->saveValue(&_pMap->_pHead->_iRootAddr, iCAddr);
    }
    
COLOR:
    if(color == BLACK)
    {
        eraseFixUp(c, iCAddr, p, iPAddr);
    }
}

void TC_RBTree::Block::insertFixUp(TC_RBTree::Block::tagBlockHead *i, uint32_t iAddr)
{
    tagBlockHead *p = NULL;
    uint32_t iPAddr = 0;
    tagBlockHead *g = NULL;
    uint32_t iGAddr = 0;
//    assert(iAddr == _pMap->getRelative(i));

    while((p = getBlockHead(i->_iParent)) != NULL && p->_iColor == RED)
    {
        iPAddr  = i->_iParent;
        g       = getBlockHead(p->_iParent);
        iGAddr  = p->_iParent;

        if(i->_iParent == g->_iLeftChild)
        {
            {
                tagBlockHead *u = getBlockHead(g->_iRightChild);
                if(u != NULL && u->_iColor == RED)
                {
                    _pMap->saveValue(&u->_iColor, BLACK);

                    _pMap->saveValue(&p->_iColor, BLACK);

                    _pMap->saveValue(&g->_iColor, RED);

                    i       = g;
                    iAddr   = iGAddr;

                    continue;
                }
            }

            if(p->_iRightChild == iAddr)
            {
                rotateLeft(p, iPAddr);

                tagBlockHead *t;
                uint32_t iTAddr;

                t       = p;
                iTAddr  = iPAddr;

                p       = i;
                iPAddr  = iAddr;

                i       = t;
                iAddr   = iTAddr;
            }

            _pMap->saveValue(&p->_iColor, BLACK);

            _pMap->saveValue(&g->_iColor, RED);

            rotateRight(g, iGAddr);
        }
        else
        {
            {
                tagBlockHead *u = getBlockHead(g->_iLeftChild);
                if(u != NULL && u->_iColor == RED)
                {
                    _pMap->saveValue(&u->_iColor, BLACK);

                    _pMap->saveValue(&p->_iColor, BLACK);

                    _pMap->saveValue(&g->_iColor, RED);

                    i       = g;
                    iAddr   = iGAddr;
                    continue;
                }
            }

            if(p->_iLeftChild == iAddr)
            {
                rotateRight(p, iPAddr);

                tagBlockHead *t;
                uint32_t iTAddr;

                t       = p;
                iTAddr  = iPAddr;

                p       = i;
                iPAddr  = iAddr;

                i       = t;
                iAddr   = iTAddr;
            }

            _pMap->saveValue(&p->_iColor, BLACK);

            _pMap->saveValue(&g->_iColor, RED);

            rotateLeft(g, iGAddr);
        }
    }

    _pMap->saveValue(&getBlockHead(_pMap->_pHead->_iRootAddr)->_iColor, BLACK);
}

void TC_RBTree::Block::insertGetSetList(TC_RBTree::Block::tagBlockHead *i)
{
    //挂在Set链表的头部
    if(_pMap->_pHead->_iSetHead == 0)
    {
        assert(_pMap->_pHead->_iSetTail == 0);
        _pMap->saveValue(&_pMap->_pHead->_iSetHead , _iHead);
        _pMap->saveValue(&_pMap->_pHead->_iSetTail , _iHead);
    }
    else
    {

        assert(_pMap->_pHead->_iSetTail != 0);
        _pMap->saveValue(&i->_iSetNext, _pMap->_pHead->_iSetHead);
        _pMap->saveValue(&getBlockHead(_pMap->_pHead->_iSetHead)->_iSetPrev, _iHead);
        _pMap->saveValue(&_pMap->_pHead->_iSetHead, _iHead);
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
        _pMap->saveValue(&i->_iGetNext, _pMap->_pHead->_iGetHead);
        _pMap->saveValue(&getBlockHead(_pMap->_pHead->_iGetHead)->_iGetPrev, _iHead);
        _pMap->saveValue(&_pMap->_pHead->_iGetHead, _iHead);
    }
}

void TC_RBTree::Block::insertRBTree(tagBlockHead *i, const string &k)
{
    if(_pMap->_pHead->_iRootAddr == 0)
    {
        _pMap->saveValue(&_pMap->_pHead->_iRootAddr, _iHead);
    }
    else
    {
        Block last      = _pMap->getLastBlock(k);

        assert(last.getBlockHead()->_iLeftChild == 0 || last.getBlockHead()->_iRightChild == 0);

        _pMap->saveValue(&i->_iParent, last.getHead());

        BlockData data;
        
        int iRet = last.getBlockData(data);

        assert(iRet == TC_RBTree::RT_OK || iRet == TC_RBTree::RT_ONLY_KEY);

        if(_pMap->_lessf(k, data._key))
        {
            _pMap->saveValue(&last.getBlockHead()->_iLeftChild , _iHead);
        }
        else if(_pMap->_lessf(data._key, k))
        {
            _pMap->saveValue(&last.getBlockHead()->_iRightChild , _iHead);
        }
        else
        {
            assert(true);
        }
    }

    //插入新节点后, 需要调整
    insertFixUp(i, _iHead);
}

int TC_RBTree::Block::set(const string& k, const string& v, bool bNewBlock, bool bOnlyKey, vector<TC_RBTree::BlockData> &vtData)
{
    TC_PackIn pi;
    pi << k;

    if(!bOnlyKey)
    {
        pi << v;
    }

    const char *pData = pi.topacket().c_str();

    uint32_t iDataLen   = pi.topacket().length();

    //首先分配刚刚够的长度, 不能多一个chunk, 也不能少一个chunk
    int ret = allocate(iDataLen, vtData);
    if(ret != TC_RBTree::RT_OK)
    {
        return ret;
    }

    tagBlockHead *i = getBlockHead();

    uint32_t iUseSize = i->_iSize - sizeof(tagBlockHead);
    //没有下一个chunk, 一个chunk就可以装下数据了
    if(!i->_bNextChunk)
    {
        memcpy(i->_cData, (char*)pData, iDataLen);
        //先copy数据, 再复制数据长度
        i->_iDataLen   = iDataLen;
    }
    else
    {
        //copy到当前的block中
        memcpy(i->_cData, (char*)pData, iUseSize);
        //剩余程度
        uint32_t iLeftLen = iDataLen - iUseSize;
        uint32_t iCopyLen = iUseSize;

        tagChunkHead *pChunk    = getChunkHead(i->_iNextChunk);
        while(true)
        {
            //计算chunk的可用大小
            iUseSize                = pChunk->_iSize - sizeof(tagChunkHead);

            if(!pChunk->_bNextChunk)
            {
                assert(iUseSize >= iLeftLen);
                //copy到当前的chunk中
                memcpy(pChunk->_cData, (char*)pData + iCopyLen, iLeftLen);
                //最后一个chunk, 才有数据长度, 先copy数据再赋值长度
                pChunk->_iDataLen = iLeftLen;
                iCopyLen += iLeftLen;
                iLeftLen -= iLeftLen;
                break;
            }
            else
            {
                //copy到当前的chunk中
                memcpy(pChunk->_cData, (char*)pData + iCopyLen, iUseSize);
                iCopyLen += iUseSize;
                iLeftLen -= iUseSize;

                pChunk   =  getChunkHead(pChunk->_iNextChunk);
            }
        }
        assert(iLeftLen == 0);
    }

    //是新块, 需要插入到树中
    if(bNewBlock)
    {
        //插入到二叉树中
        insertRBTree(i, k);

        //插入到get/set链表中
        insertGetSetList(i);

        _pMap->incDirtyCount();

        _pMap->incElementCount();
    }

    if(bOnlyKey)
    {
        //原始数据是脏数据
        if(i->_bDirty)
        {
            _pMap->delDirtyCount();
        }

        //数据被修改, 设置为脏数据
        i->_bDirty     = false;

        //原始数据不是OnlyKey数据
        if(!i->_bOnlyKey)
        {
            _pMap->incOnlyKeyCount();
        }
    }
    else
    {
        //原始数据不是脏数据
        if(!i->_bDirty)
        {
            _pMap->incDirtyCount();
        }

        //数据被修改, 设置为脏数据
        i->_bDirty     = true;

        //原始数据是OnlyKey数据
        if(i->_bOnlyKey)
        {
            _pMap->delOnlyKeyCount();
        }
    }

    //设置是否只有Key
    i->_bOnlyKey   = bOnlyKey;

    return TC_RBTree::RT_OK;
}

int TC_RBTree::Block::getBlockData(TC_RBTree::BlockData &data)
{
    data._dirty = isDirty();
    data._synct = getSyncTime();

    string s;
    int ret   = get(s);

    if(ret != TC_RBTree::RT_OK)
    {
        return ret;
    }

    try
    {
        TC_PackOut po(s.c_str(), s.length());
        po >> data._key;

        //如果不是只有Key
        if(!isOnlyKey())
        {
            po >> data._value;
        }
        else
        {
            return TC_RBTree::RT_ONLY_KEY;
        }
    }
    catch(exception &ex)
    {
        assert(false);
        ret = TC_RBTree::RT_DECODE_ERR;
    }

    return ret;
}

int TC_RBTree::Block::get(void *pData, uint32_t &iDataLen)
{
    tagBlockHead *i = getBlockHead();

    //没有下一个chunk, 一个chunk就可以装下数据了
    if(!i->_bNextChunk)
    {
        memcpy(pData, i->_cData, min(i->_iDataLen, iDataLen));
        iDataLen = i->_iDataLen;
        return TC_RBTree::RT_OK;
    }
    else
    {
        uint32_t iUseSize = i->_iSize - sizeof(tagBlockHead);
        uint32_t iCopyLen = min(iUseSize, iDataLen);

        //copy到当前的block中
        memcpy(pData, i->_cData, iCopyLen);
        if (iDataLen < iUseSize)
        {
            return TC_RBTree::RT_NOTALL_ERR;   //copy数据不完全
        }

        //已经copy长度
        uint32_t iHasLen  = iCopyLen;
        //最大剩余长度
        uint32_t iLeftLen = iDataLen - iCopyLen;

        tagChunkHead *pChunk    = getChunkHead(i->_iNextChunk);
        while(iHasLen < iDataLen)
        {
            iUseSize        = pChunk->_iSize - sizeof(tagChunkHead);
            if(!pChunk->_bNextChunk)
            {
                //copy到当前的chunk中
                uint32_t iCopyLen = min(pChunk->_iDataLen, iLeftLen);
                memcpy((char*)pData + iHasLen, pChunk->_cData, iCopyLen);
                iDataLen = iHasLen + iCopyLen;

                if(iLeftLen < pChunk->_iDataLen)
                {
                    return TC_RBTree::RT_NOTALL_ERR;       //copy不完全
                }

                return TC_RBTree::RT_OK;
            }
            else
            {
                uint32_t iCopyLen = min(iUseSize, iLeftLen);
                //copy当前的chunk
                memcpy((char*)pData + iHasLen, pChunk->_cData, iCopyLen);
                if (iLeftLen <= iUseSize)
                {
                    iDataLen = iHasLen + iCopyLen;
                    return TC_RBTree::RT_NOTALL_ERR;   //copy不完全
                }

                //copy当前chunk完全
                iHasLen  += iCopyLen;
                iLeftLen -= iCopyLen;

                pChunk   =  getChunkHead(pChunk->_iNextChunk);
            }
        }
    }

    return TC_RBTree::RT_OK;
}

int TC_RBTree::Block::get(string &s)
{
    uint32_t iLen = getDataLen();

    char *cData = new char[iLen];
    uint32_t iGetLen = iLen;
    int ret = get(cData, iGetLen);
    if(ret == TC_RBTree::RT_OK)
    {
        s.assign(cData, iGetLen);
    }

    delete[] cData;

    return ret;
}

void TC_RBTree::Block::setDirty(bool b)
{
    tagBlockHead *i = getBlockHead();

    if(i->_bDirty != b)
    {
        if (b)
        {
            _pMap->incDirtyCount();
        }
        else
        {
            _pMap->delDirtyCount();
        }

        _pMap->saveValue(&i->_bDirty, b);
    }
}

void TC_RBTree::Block::deallocate()
{
    tagBlockHead *i = getBlockHead();

    vector<uint32_t> v;
    v.push_back(_iHead);

    if(i->_bNextChunk)
    {
        deallocate(i->_iNextChunk);
    }

    _pMap->_pDataAllocator->deallocateMemBlock(v);
}

void TC_RBTree::Block::makeNew(uint32_t iAllocSize)
{
    tagBlockHead *i    = getBlockHead();

    i->_iSize          = iAllocSize;
    i->_iColor         = TC_RBTree::RED;
    i->_iParent        = 0;
    i->_iLeftChild     = 0;
    i->_iRightChild    = 0;
    i->_iSetNext       = 0;
    i->_iSetPrev       = 0;
    i->_iGetNext       = 0;
    i->_iGetPrev       = 0;
    i->_iSyncTime      = 0;
    i->_bNextChunk     = false;
    i->_iDataLen       = 0;
    i->_bDirty         = true;
    i->_bOnlyKey       = false;
}

void TC_RBTree::Block::erase()
{
    tagBlockHead *i = getBlockHead();

    ////////////////////从树删除节点/////////////////////
    erase(i, _iHead);

    //////////////////修改脏数据链表/////////////
    if(_pMap->_pHead->_iDirtyTail == _iHead)
    {
        _pMap->saveValue(&_pMap->_pHead->_iDirtyTail, i->_iSetPrev);
    }

    //////////////////修改回写数据链表/////////////
    if(_pMap->_pHead->_iSyncTail == _iHead)
    {
        _pMap->saveValue(&_pMap->_pHead->_iSyncTail, i->_iSetPrev);
    }

    //////////////////修改备份数据链表/////////////
    if(_pMap->_pHead->_iBackupTail == _iHead)
    {
        _pMap->saveValue(&_pMap->_pHead->_iBackupTail, i->_iGetPrev);
    }
    
    ////////////////////修改Set链表的数据//////////
    {
        bool bHead = (_pMap->_pHead->_iSetHead == _iHead);
        bool bTail = (_pMap->_pHead->_iSetTail == _iHead);

        if(!bHead)
        {
            if(bTail)
            {
                assert(i->_iSetNext == 0);
                //是尾部, 尾部指针指向上一个元素
                _pMap->saveValue(&_pMap->_pHead->_iSetTail, i->_iSetPrev);
                _pMap->saveValue(&getBlockHead(i->_iSetPrev)->_iSetNext, (uint32_t)0);                 
            }
            else
            {
                //不是头部也不是尾部
                assert(i->_iSetNext != 0);
                _pMap->saveValue(&getBlockHead(i->_iSetPrev)->_iSetNext, i->_iSetNext);
                _pMap->saveValue(&getBlockHead(i->_iSetNext)->_iSetPrev, i->_iSetPrev);
            }
        }
        else
        {
            if(bTail)
            {
                assert(i->_iSetNext == 0);
                assert(i->_iSetPrev == 0);
                //头部也是尾部, 指针都设置为0
                _pMap->saveValue(&_pMap->_pHead->_iSetHead, (uint32_t)0);
                _pMap->saveValue(&_pMap->_pHead->_iSetTail, (uint32_t)0);
            }
            else
            {
                //头部不是尾部, 头部指针指向下一个元素
                assert(i->_iSetNext != 0);
                _pMap->saveValue(&_pMap->_pHead->_iSetHead, i->_iSetNext);
                //下一个元素上指针为0
                _pMap->saveValue(&getBlockHead(i->_iSetNext)->_iSetPrev, (uint32_t)0);
            }
        }
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
                assert(i->_iGetNext == 0);
                //是尾部, 尾部指针指向上一个元素
                _pMap->saveValue(&_pMap->_pHead->_iGetTail, i->_iGetPrev);
                _pMap->saveValue(&getBlockHead(getBlockHead()->_iGetPrev)->_iGetNext, (uint32_t)0);
            }
            else
            {
                //不是头部也不是尾部
                assert(getBlockHead()->_iGetNext != 0);
                _pMap->saveValue(&getBlockHead(i->_iGetPrev)->_iGetNext, i->_iGetNext);
                _pMap->saveValue(&getBlockHead(i->_iGetNext)->_iGetPrev, i->_iGetPrev);
            }
        }
        else
        {
            if(bTail)
            {
                assert(i->_iGetNext == 0);
                assert(i->_iGetPrev == 0);
                //头部也是尾部, 指针都设置为0
                _pMap->saveValue(&_pMap->_pHead->_iGetHead, (uint32_t)0);
                _pMap->saveValue(&_pMap->_pHead->_iGetTail, (uint32_t)0);
            }
            else
            {
                //头部不是尾部, 头部指针指向下一个元素
                assert(i->_iGetNext != 0);
                _pMap->saveValue(&_pMap->_pHead->_iGetHead, i->_iGetNext);
                //下一个元素上指针为0
                _pMap->saveValue(&getBlockHead(i->_iGetNext)->_iGetPrev, (uint32_t)0);
            }
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

    //保留头部指针的现场
    _pMap->saveValue(&i->_iSize, 0, false);
    _pMap->saveValue(&i->_iColor, TC_RBTree::RED, false);
    _pMap->saveValue(&i->_iParent, 0, false);
    _pMap->saveValue(&i->_iLeftChild, 0, false);
    _pMap->saveValue(&i->_iRightChild, 0, false);
    _pMap->saveValue(&i->_iSetNext, 0, false);
    _pMap->saveValue(&i->_iSetPrev, 0, false);
    _pMap->saveValue(&i->_iGetNext, 0, false);
    _pMap->saveValue(&i->_iGetPrev, 0, false);
    _pMap->saveValue(&i->_iSyncTime, 0, false);
    _pMap->saveValue(&i->_bNextChunk, 0, false);
    _pMap->saveValue(&i->_iDataLen, 0, false);
    _pMap->saveValue(&i->_bDirty, 0, false);
    _pMap->saveValue(&i->_bOnlyKey, 0, false);

    //使修改生效, 在释放内存之前生效, 即使归还内存失败也只是丢掉了内存块
    _pMap->doUpdate();

    //归还到内存中
    deallocate();
}

void TC_RBTree::Block::refreshSetList()
{
    tagBlockHead *i = getBlockHead();

    assert(_pMap->_pHead->_iSetHead != 0);
    assert(_pMap->_pHead->_iSetTail != 0);

    //修改同步链表
    if(_pMap->_pHead->_iSyncTail == _iHead && _pMap->_pHead->_iSetHead != _iHead)
    {
        _pMap->saveValue(&_pMap->_pHead->_iSyncTail, i->_iSetPrev);
    }

    //修改脏数据尾部链表指针, 不仅一个元素
    if(_pMap->_pHead->_iDirtyTail == _iHead && _pMap->_pHead->_iSetHead != _iHead)
    {
        //脏数据尾部位置前移
        _pMap->saveValue(&_pMap->_pHead->_iDirtyTail, i->_iSetPrev);
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
        refreshGetList();
        return;
    }

    uint32_t iPrev = i->_iSetPrev;
    uint32_t iNext = i->_iSetNext;

    assert(iPrev != 0);

    //挂在链表头部
    _pMap->saveValue(&i->_iSetNext, _pMap->_pHead->_iSetHead);
    _pMap->saveValue(&getBlockHead(_pMap->_pHead->_iSetHead)->_iSetPrev, _iHead);
    _pMap->saveValue(&_pMap->_pHead->_iSetHead, _iHead);
    _pMap->saveValue(&i->_iSetPrev, (uint32_t)0);

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
    refreshGetList();
}

void TC_RBTree::Block::refreshGetList()
{
    assert(_pMap->_pHead->_iGetHead != 0);
    assert(_pMap->_pHead->_iGetTail != 0);

    //是头部数据
    if(_pMap->_pHead->_iGetHead == _iHead)
    {
        return;
    }

    tagBlockHead *i = getBlockHead();

    uint32_t iPrev = i->_iGetPrev;
    uint32_t iNext = i->_iGetNext;

    assert(iPrev != 0);

    //是正在备份的数据
    if(_pMap->_pHead->_iBackupTail == _iHead)
    {
        _pMap->saveValue(&_pMap->_pHead->_iBackupTail, iPrev);
    }

    //挂在链表头部
    _pMap->saveValue(&i->_iGetNext, _pMap->_pHead->_iGetHead);
    _pMap->saveValue(&getBlockHead(_pMap->_pHead->_iGetHead)->_iGetPrev, _iHead);
    _pMap->saveValue(&_pMap->_pHead->_iGetHead, _iHead);
    _pMap->saveValue(&i->_iGetPrev, (uint32_t)0);

    //上一个元素的Next指针指向下一个元素
    _pMap->saveValue(&getBlockHead(iPrev)->_iGetNext, iNext);

    //下一个元素的Prev指向上一个元素
    if (iNext != 0)
    {
        _pMap->saveValue(&getBlockHead(iNext)->_iGetPrev, iPrev);
    }
    else
    {
        //改变尾部指针
        assert(_pMap->_pHead->_iGetTail == _iHead);
        _pMap->saveValue(&_pMap->_pHead->_iGetTail, iPrev);
    }
}

void TC_RBTree::Block::deallocate(uint32_t iChunk)
{
    tagChunkHead *pChunk        = getChunkHead(iChunk);
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
    _pMap->_pDataAllocator->deallocateMemBlock(v);
}

int TC_RBTree::Block::allocate(uint32_t iDataLen, vector<TC_RBTree::BlockData> &vtData)
{
    uint32_t fn   = 0;

    tagBlockHead *i = getBlockHead();

    //一个块的真正的数据容量
    fn = getBlockHead()->_iSize - sizeof(tagBlockHead);
    if(fn >= iDataLen)
    {
        //一个block就可以了, 后续的chunk都要释放掉
        if(getBlockHead()->_bNextChunk)
        {
            uint32_t iNextChunk = i->_iNextChunk;
            //先修改自己的区块
            _pMap->saveValue(&i->_bNextChunk, false);
            _pMap->saveValue(&i->_iDataLen, (uint32_t)0);
            //修改成功后再释放区块, 从而保证, 不会core的时候导致整个内存块不可用
            deallocate(iNextChunk);
        }
        return TC_RBTree::RT_OK;
    }

    //计算还需要多少长度
    fn = iDataLen - fn;

    if(i->_bNextChunk)
    {
        tagChunkHead *pChunk    = getChunkHead(i->_iNextChunk);

        while(true)
        {
            if(fn <= (pChunk->_iSize - sizeof(tagChunkHead)))
            {
                //已经不需要chunk了, 释放多余的chunk
                if(pChunk->_bNextChunk)
                {
                    uint32_t iNextChunk = pChunk->_iNextChunk;
                    _pMap->saveValue(&pChunk->_bNextChunk, false);
                    //一旦异常core, 最坏的情况就是少了可用的区块, 但是整个内存结构还是可用的
                    deallocate(iNextChunk);
                }
                return TC_RBTree::RT_OK ;
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
                if(ret != TC_RBTree::RT_OK)
                {
                    //没有内存可以分配
                    return ret;
                }

                _pMap->saveValue(&pChunk->_bNextChunk, true);
                _pMap->saveValue(&pChunk->_iNextChunk, chunks[0]);
                //chunk指向分配的第一个chunk
                pChunk  =  getChunkHead(chunks[0]);
                //修改第一个chunk的属性, 保证异常core的时候, chunk链表不会有问题
                _pMap->saveValue(&pChunk->_bNextChunk, false);
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
        if(ret != TC_RBTree::RT_OK)
        {
            //没有内存可以分配
            return ret;
        }

        _pMap->saveValue(&i->_bNextChunk, true);
        _pMap->saveValue(&i->_iNextChunk, chunks[0]);
        //chunk指向分配的第一个chunk
        tagChunkHead *pChunk = getChunkHead(chunks[0]);
        //修改第一个chunk的属性, 保证异常core的时候, chunk链表不会有问题
        _pMap->saveValue(&pChunk->_bNextChunk, false);
        _pMap->saveValue(&pChunk->_iDataLen, (uint32_t)0);

        //连接每个chunk
        return joinChunk(pChunk, chunks);
    }

    return TC_RBTree::RT_OK;
}

int TC_RBTree::Block::joinChunk(tagChunkHead *pChunk, const vector<uint32_t> chunks)
{
    for (size_t i = 0; i < chunks.size(); ++i)
    {
        if (i == chunks.size() - 1)
        {
            _pMap->saveValue(&pChunk->_bNextChunk, false);
            return TC_RBTree::RT_OK;
        }
        else
        {
            _pMap->saveValue(&pChunk->_bNextChunk, true);
            _pMap->saveValue(&pChunk->_iNextChunk, chunks[i+1]);
            pChunk  =  getChunkHead(chunks[i+1]);
            _pMap->saveValue(&pChunk->_bNextChunk, false);
            _pMap->saveValue(&pChunk->_iDataLen, (uint32_t)0);
        }
    }

    return TC_RBTree::RT_OK;
}

int TC_RBTree::Block::allocateChunk(uint32_t fn, vector<uint32_t> &chunks, vector<TC_RBTree::BlockData> &vtData)
{
    assert(fn > 0);

    while(true)
    {
        uint32_t iAllocSize = fn;
        //分配空间
        uint32_t t = _pMap->_pDataAllocator->allocateChunk(_iHead, iAllocSize, vtData);
        if (t == 0)
        {
            //没有内存可以分配了, 先把分配的归还
            _pMap->_pDataAllocator->deallocateMemBlock(chunks);
            chunks.clear();
            return TC_RBTree::RT_NO_MEMORY;
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

    return TC_RBTree::RT_OK;
}

uint32_t TC_RBTree::Block::getDataLen()
{
    tagBlockHead *i = getBlockHead();

    uint32_t n = 0;
    if(!i->_bNextChunk)
    {
        n += i->_iDataLen;
        return n;
    }

    //当前块的大小
    n += i->_iSize - sizeof(tagBlockHead);
    tagChunkHead *pChunk    = getChunkHead(i->_iNextChunk);

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
// 
uint32_t TC_RBTree::BlockAllocator::allocateMemBlock(uint32_t &iAllocSize, vector<TC_RBTree::BlockData> &vtData)
{
begin:
    size_t iIndex;
    size_t iTmp1 = (size_t)iAllocSize;
    _pChunkAllocator->allocate2(iTmp1, iTmp1, iIndex);
    iAllocSize = (uint32_t)iTmp1;

    if(iIndex == 0)
    {
        uint32_t ret = _pMap->eraseExcept(0, vtData);
        if(ret == 0)
            return 0;     //没有空间可以释放了
        goto begin;
    }

    //分配的新的MemBlock, 初始化一下
    Block block(_pMap, (uint32_t)iIndex);
    block.makeNew(iAllocSize);

    _pMap->incChunkCount();

    return (uint32_t)iIndex;
}                     

uint32_t TC_RBTree::BlockAllocator::allocateChunk(uint32_t iAddr, uint32_t &iAllocSize, vector<TC_RBTree::BlockData> &vtData)
{
begin:
    size_t iIndex;
    size_t iTmp1 = (size_t)iAllocSize;
    _pChunkAllocator->allocate2(iTmp1, iTmp1, iIndex);
    iAllocSize = (uint32_t)iTmp1;

    if(iIndex == 0)
    {
        uint32_t ret = _pMap->eraseExcept(iAddr, vtData);
        if(ret == 0)
            return 0;     //没有空间可以释放了
        goto begin;
    }

    _pMap->incChunkCount();

    return (uint32_t)iIndex;
}

void TC_RBTree::BlockAllocator::deallocateMemBlock(const vector<uint32_t> &v)
{
    for(size_t i = 0; i < v.size(); i++)
    {
        _pChunkAllocator->deallocate2(v[i]);
        _pMap->delChunkCount();
    }
}

void TC_RBTree::BlockAllocator::deallocateMemBlock(uint32_t v)
{
    _pChunkAllocator->deallocate2(v);
    _pMap->delChunkCount();
}

///////////////////////////////////////////////////////////////////

TC_RBTree::RBTreeLockItem::RBTreeLockItem(TC_RBTree *pMap, uint32_t iAddr)
: _pMap(pMap)
, _iAddr(iAddr)
{
}

TC_RBTree::RBTreeLockItem::RBTreeLockItem(const RBTreeLockItem &mcmdi)
: _pMap(mcmdi._pMap)
, _iAddr(mcmdi._iAddr)
{
}

TC_RBTree::RBTreeLockItem &TC_RBTree::RBTreeLockItem::operator=(const TC_RBTree::RBTreeLockItem &mcmdi)
{
    if(this != &mcmdi)
    {
        _pMap   = mcmdi._pMap;
        _iAddr  = mcmdi._iAddr;
    }
    return (*this);
}

bool TC_RBTree::RBTreeLockItem::operator==(const TC_RBTree::RBTreeLockItem &mcmdi)
{
    return _pMap == mcmdi._pMap && _iAddr == mcmdi._iAddr;
}

bool TC_RBTree::RBTreeLockItem::operator!=(const TC_RBTree::RBTreeLockItem &mcmdi)
{
    return _pMap != mcmdi._pMap || _iAddr != mcmdi._iAddr;
}

bool TC_RBTree::RBTreeLockItem::isDirty()
{
    Block block(_pMap, _iAddr);
    return block.isDirty();
}

bool TC_RBTree::RBTreeLockItem::isOnlyKey()
{
    Block block(_pMap, _iAddr);
    return block.isOnlyKey();
}

time_t TC_RBTree::RBTreeLockItem::getSyncTime()
{
    Block block(_pMap, _iAddr);
    return block.getSyncTime();
}

int TC_RBTree::RBTreeLockItem::get(string& k, string& v)
{
    string s;

    Block block(_pMap, _iAddr);

    int ret = block.get(s);
    if(ret != TC_RBTree::RT_OK)
    {
        return ret;
    }

    try
    {
        TC_PackOut po(s.c_str(), s.length());
        po >> k;
        if(!block.isOnlyKey())
        {
            po >> v;
        }
        else
        {
            v = "";
            return TC_RBTree::RT_ONLY_KEY;
        }
    }
    catch ( exception &ex )
    {
        assert(false);
        return TC_RBTree::RT_EXCEPTION_ERR;
    }

    return TC_RBTree::RT_OK;
}

int TC_RBTree::RBTreeLockItem::get(string& k)
{
    string s;

    Block block(_pMap, _iAddr);

    int ret = block.get(s);
    if(ret != TC_RBTree::RT_OK)
    {
        return ret;
    }

    try
    {
        TC_PackOut po(s.c_str(), s.length());
        po >> k;
    }
    catch ( exception &ex )
    {
        assert(false);
        return TC_RBTree::RT_EXCEPTION_ERR;
    }

    return TC_RBTree::RT_OK;
}

int TC_RBTree::RBTreeLockItem::set(const string& k, const string& v, bool bNewBlock, vector<TC_RBTree::BlockData> &vtData)
{
    Block block(_pMap, _iAddr);

    return block.set(k, v, bNewBlock, false, vtData);
}

int TC_RBTree::RBTreeLockItem::set(const string& k, bool bNewBlock, vector<TC_RBTree::BlockData> &vtData)
{
    Block block(_pMap, _iAddr);

    return block.set(k, "", bNewBlock, true, vtData);
}

bool TC_RBTree::RBTreeLockItem::equal(const string &k, string &k1, string &v, int &ret)
{
    ret = get(k1, v);

    if ((ret == TC_RBTree::RT_OK || ret == TC_RBTree::RT_ONLY_KEY) && k == k1)
    {
        return true;
    }

    return false;
}

bool TC_RBTree::RBTreeLockItem::equal(const string& k, string &k1, int &ret)
{
    ret = get(k1);

    if (ret == TC_RBTree::RT_OK && k == k1)
    {
        return true;
    }

    return false;
}

void TC_RBTree::RBTreeLockItem::nextItem(int iType)
{
    Block block(_pMap, _iAddr);

    if(iType == RBTreeLockIterator::IT_SET)
    {
        _iAddr = block.getBlockHead()->_iSetNext;
    }
    else if(iType == RBTreeLockIterator::IT_GET)
    {
        _iAddr = block.getBlockHead()->_iGetNext;
    }
    else if(iType == RBTreeLockIterator::IT_RBTREE)
    {
        if(block.hasRight())
        {
            //移到右子树的最左节点
            block.moveToRight();
            while(block.hasLeft())
                block.moveToLeft();
        }
        else
        {
            while(block.hasParent() && block.getHead() == block.getBlockHead(block.getBlockHead()->_iParent)->_iRightChild)
                block.moveToParent();

            block.moveToParent();
        }
        _iAddr = block.getHead();
    }
}

void TC_RBTree::RBTreeLockItem::prevItem(int iType)
{
    Block block(_pMap, _iAddr);

    if(iType == RBTreeLockIterator::IT_SET)
    {
        _iAddr = block.getBlockHead()->_iSetPrev;
    }
    else if(iType == RBTreeLockIterator::IT_GET)
    {
        _iAddr = block.getBlockHead()->_iGetPrev;
    }
    else if(iType == RBTreeLockIterator::IT_RBTREE)
    {
        if(block.hasLeft())
        {
            //移到右子树的最左节：点
            block.moveToLeft();
            while(block.hasRight())
                block.moveToRight();
        }
        else
        {
            while(block.hasParent() && block.getHead() == block.getBlockHead(block.getBlockHead()->_iParent)->_iLeftChild)
                block.moveToParent();

            block.moveToParent();
        }
        _iAddr = block.getHead();
    }
}

///////////////////////////////////////////////////////////////////

TC_RBTree::RBTreeLockIterator::RBTreeLockIterator()
: _pMap(NULL), _iItem(NULL, 0),_iType(IT_RBTREE),_iOrder(IT_NEXT)
{
}

TC_RBTree::RBTreeLockIterator::RBTreeLockIterator(TC_RBTree *pMap, uint32_t iAddr, int iType, int iOrder)
: _pMap(pMap), _iItem(_pMap, iAddr), _iType(iType), _iOrder(iOrder)
{
}

TC_RBTree::RBTreeLockIterator::RBTreeLockIterator(const RBTreeLockIterator &it)
: _pMap(it._pMap),_iItem(it._iItem), _iType(it._iType), _iOrder(it._iOrder)
{
}

TC_RBTree::RBTreeLockIterator& TC_RBTree::RBTreeLockIterator::operator=(const RBTreeLockIterator &it)
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

bool TC_RBTree::RBTreeLockIterator::operator==(const RBTreeLockIterator& mcmi)
{
    if (_iItem.getAddr() != 0 || mcmi._iItem.getAddr() != 0)
    {
        return _iItem == mcmi._iItem
            && _iType == mcmi._iType
            && _iOrder == mcmi._iOrder;
    }

    return true;
}

bool TC_RBTree::RBTreeLockIterator::operator!=(const RBTreeLockIterator& mcmi)
{
    if (_iItem.getAddr() != 0 || mcmi._iItem.getAddr() != 0 )
    {
        return _iItem != mcmi._iItem
            || _iType != mcmi._iType
            || _iOrder != mcmi._iOrder;
    }

    return false;
}

TC_RBTree::RBTreeLockIterator& TC_RBTree::RBTreeLockIterator::operator++()
{
    if(_iOrder == IT_NEXT)
    {
        _iItem.nextItem(_iType);
    }
    else
    {
        _iItem.prevItem(_iType);
    }
    return (*this);

}

TC_RBTree::RBTreeLockIterator TC_RBTree::RBTreeLockIterator::operator++(int)
{
    RBTreeLockIterator it(*this);

    if(_iOrder == IT_NEXT)
    {
        _iItem.nextItem(_iType);
    }
    else
    {
        _iItem.prevItem(_iType);
    }

    return it;
}

///////////////////////////////////////////////////////////////////

TC_RBTree::RBTreeItem::RBTreeItem(TC_RBTree *pMap, const string &key, bool bEnd)
: _pMap(pMap)
, _key(key)
, _bEnd(bEnd)
{
}

TC_RBTree::RBTreeItem::RBTreeItem(const TC_RBTree::RBTreeItem &mcmdi)
: _pMap(mcmdi._pMap)
, _key(mcmdi._key)
, _bEnd(mcmdi._bEnd)
{
}

TC_RBTree::RBTreeItem &TC_RBTree::RBTreeItem::operator=(const TC_RBTree::RBTreeItem &mcmdi)
{
    if(this != &mcmdi)
    {
        _pMap   = mcmdi._pMap;
        _key    = mcmdi._key;
        _bEnd   = mcmdi._bEnd;
    }
    return (*this);
}

bool TC_RBTree::RBTreeItem::operator==(const TC_RBTree::RBTreeItem &mcmdi)
{
    if(_bEnd == mcmdi._bEnd)
    {
        return true;
    }
    return _pMap == mcmdi._pMap && _key == mcmdi._key;
}

bool TC_RBTree::RBTreeItem::operator!=(const TC_RBTree::RBTreeItem &mcmdi)
{
    if(_bEnd == mcmdi._bEnd)
    {
        return false;
    }
    return _pMap != mcmdi._pMap || _key != mcmdi._key;
}

int TC_RBTree::RBTreeItem::get(TC_RBTree::BlockData &stData)
{
    if(_bEnd)
    {
        return TC_RBTree::RT_NO_DATA;
    }

    TC_RBTree::lock_iterator it = _pMap->find(_key);
    if(it != _pMap->end())
    {
        Block block(_pMap, it->getAddr());
        return block.getBlockData(stData);
    }

    return TC_RBTree::RT_NO_DATA;
}

void TC_RBTree::RBTreeItem::nextItem()
{
    if(_bEnd)
    {
        return;
    }

    string k;
    int ret;
    TC_RBTree::lock_iterator it = _pMap->upper_bound(_key);

    while(it != _pMap->end())
    {
        ret = it->get(k);

        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
        {
            //数据有错
            ++it;
            continue;
        }

        _key  = k;
        _bEnd = false;
        return;
    }

    _bEnd = true;
}

void TC_RBTree::RBTreeItem::prevItem()
{
    if(_bEnd)
    {
        return;
    }

    string k;
    int ret;
    TC_RBTree::lock_iterator it1 = _pMap->lower_bound(_key);
    TC_RBTree::lock_iterator it(_pMap, it1->getAddr(), TC_RBTree::lock_iterator::IT_RBTREE, TC_RBTree::lock_iterator::IT_PREV);

    while(it != _pMap->end())
    {
        ret = it->get(k);

        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
        {
            //数据有错
            ++it;
            continue;
        }

        //判断就是当前的key, 取下一个元素
        if(k == _key)
        {
            ++it;
            continue;
        }

        _key  = k;
        _bEnd = false;
        return;
    }

    _bEnd = true;
}

///////////////////////////////////////////////////////////////////

TC_RBTree::RBTreeIterator::RBTreeIterator()
: _pMap(NULL), _iItem(NULL, "", true),_iOrder(IT_NEXT)
{
}

TC_RBTree::RBTreeIterator::RBTreeIterator(TC_RBTree *pMap, const string &key, bool bEnd, int iOrder)
: _pMap(pMap), _iItem(_pMap, key, bEnd), _iOrder(iOrder)
{
}

TC_RBTree::RBTreeIterator::RBTreeIterator(const RBTreeIterator &it)
: _pMap(it._pMap),_iItem(it._iItem), _iOrder(it._iOrder)
{
}

TC_RBTree::RBTreeIterator& TC_RBTree::RBTreeIterator::operator=(const RBTreeIterator &it)
{
    if(this != &it)
    {
        _pMap       = it._pMap;
        _iItem      = it._iItem;
        _iOrder     = it._iOrder;
    }

    return (*this);
}

bool TC_RBTree::RBTreeIterator::operator==(const RBTreeIterator& mcmi)
{
    if (!_iItem.isEnd() || !mcmi._iItem.isEnd())
    {
        return _iItem == mcmi._iItem && _iOrder == mcmi._iOrder; 
    }

    return true;
}

bool TC_RBTree::RBTreeIterator::operator!=(const RBTreeIterator& mcmi)
{
    if (!_iItem.isEnd() || !mcmi._iItem.isEnd())
    {
        return _iItem != mcmi._iItem || _iOrder != mcmi._iOrder;
    }

    return false;
}

TC_RBTree::RBTreeIterator& TC_RBTree::RBTreeIterator::operator++()
{
    if(_iOrder == IT_NEXT)
    {
        _iItem.nextItem();
    }
    else
    {
        _iItem.prevItem();
    }
    return (*this);

}

TC_RBTree::RBTreeIterator TC_RBTree::RBTreeIterator::operator++(int)
{
    RBTreeIterator it(*this);

    if(_iOrder == IT_NEXT)
    {
        _iItem.nextItem();
    }
    else
    {
        _iItem.prevItem();
    }

    return it;

}

///////////////////////////////////////////////////////////////////

void TC_RBTree::init(void *pAddr)
{
    _pHead          = static_cast<tagMapHead*>(pAddr);
    _pstModifyHead  = static_cast<tagModifyHead*>((void*)((char*)pAddr + sizeof(tagMapHead)));
}

void TC_RBTree::initDataBlockSize(uint32_t iMinDataSize, uint32_t iMaxDataSize, float fFactor)
{
    _iMinDataSize   = iMinDataSize;
    _iMaxDataSize   = iMaxDataSize;
    _fFactor        = fFactor;
}

void TC_RBTree::create(void *pAddr, size_t iSize)
{
    if(sizeof(tagMapHead)
       + sizeof(tagModifyHead)
       + sizeof(TC_MemMultiChunkAllocator::tagChunkAllocatorHead)
       + 10 > iSize)
    {
        throw TC_RBTree_Exception("[TC_RBTree::create] mem size not enougth.");
    }

    if(_iMinDataSize == 0 || _iMaxDataSize == 0 || _fFactor < 1.0)
    {
        throw TC_RBTree_Exception("[TC_RBTree::create] init data size error:" + TC_Common::tostr(_iMinDataSize) + "|" + TC_Common::tostr(_iMaxDataSize) + "|" + TC_Common::tostr(_fFactor));
    }

    init(pAddr);

    //block size用2个字节存储的
    if(sizeof(Block::tagBlockHead) + _pHead->_iMaxDataSize > (uint16_t)(-1))
    {
        throw TC_RBTree_Exception("[TC_RBTree::create] init block size error, block size must be less then " + TC_Common::tostr((uint16_t)(-1) - sizeof(Block::tagBlockHead)));
    }

    _pHead->_cMaxVersion    = MAX_VERSION;
    _pHead->_cMinVersion    = MIN_VERSION;
    _pHead->_bReadOnly      = false;
    _pHead->_bAutoErase     = true;
    _pHead->_cEraseMode     = ERASEBYGET;
    _pHead->_iMemSize       = iSize;
    _pHead->_iMinDataSize   = _iMinDataSize;
    _pHead->_iMaxDataSize   = _iMaxDataSize;
    _pHead->_fFactor        = _fFactor;
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
    _pHead->_iRootAddr      = 0;

    void *pDataAddr     = (char*)_pHead + sizeof(tagMapHead) + sizeof(tagModifyHead);

    _pDataAllocator->create(pDataAddr, iSize - ((char*)pDataAddr - (char*)_pHead), sizeof(Block::tagBlockHead) + _pHead->_iMinDataSize, sizeof(Block::tagBlockHead) + _pHead->_iMaxDataSize, _pHead->_fFactor);
}

void TC_RBTree::connect(void *pAddr, size_t iSize)
{
    init(pAddr);

    if(_pHead->_cMaxVersion != MAX_VERSION || _pHead->_cMinVersion != MIN_VERSION)
    {
        ostringstream os;
        os << (int)_pHead->_cMaxVersion << "." << (int)_pHead->_cMinVersion << " != " << ((int)MAX_VERSION) << "." << ((int)MIN_VERSION);
        throw TC_RBTree_Exception("[TC_RBTree::connect] rbtree map version not equal:" + os.str() + " (data != code)");
    }

    if(_pHead->_iMemSize != iSize)
    {
        throw TC_RBTree_Exception("[TC_RBTree::connect] rbtree map size not equal:" + TC_Common::tostr(_pHead->_iMemSize) + "!=" + TC_Common::tostr(iSize));
    }

    void *pDataAddr = (char*)_pHead + sizeof(tagMapHead) + sizeof(tagModifyHead);

    _pDataAllocator->connect(pDataAddr);

    _iMinDataSize   = _pHead->_iMinDataSize;
    _iMaxDataSize   = _pHead->_iMaxDataSize;
    _fFactor        = _pHead->_fFactor;
}

int TC_RBTree::append(void *pAddr, size_t iSize)
{
    if(iSize <= _pHead->_iMemSize)
    {
        return -1;
    }

    init(pAddr);

    if(_pHead->_cMaxVersion != MAX_VERSION || _pHead->_cMinVersion != MIN_VERSION)
    {
        ostringstream os;
        os << (int)_pHead->_cMaxVersion << "." << (int)_pHead->_cMinVersion << " != " << ((int)MAX_VERSION) << "." << ((int)MIN_VERSION);
        throw TC_RBTree_Exception("[TC_RBTree::append] rbtree map version not equal:" + os.str() + " (data != code)");
    }
    
    _pHead->_iMemSize = iSize;

    void *pDataAddr = (char*)_pHead + sizeof(tagMapHead) + sizeof(tagModifyHead);

    _pDataAllocator->append(pDataAddr, iSize - ((size_t)pDataAddr - (size_t)pAddr));
    
    _iMinDataSize   = _pHead->_iMinDataSize;
    _iMaxDataSize   = _pHead->_iMaxDataSize;
    _fFactor        = _pHead->_fFactor;

    return 0;
}

void TC_RBTree::clear()
{
    FailureRecover check(this);

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
    _pHead->_iRootAddr      = 0;

    _pDataAllocator->rebuild();
}

int TC_RBTree::dump2file(const string &sFile)
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

int TC_RBTree::load5file(const string &sFile)
{
    FILE *fp = fopen(sFile.c_str(), "rb");
    if(fp == NULL)
    {
        return RT_LOAL_FILE_ERR;
    }
    fseek(fp, 0L, SEEK_END);
    size_t fs = ftell(fp);
    if(fs != _pHead->_iMemSize)
    {
        fclose(fp);
        return RT_LOAL_FILE_ERR;
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
                return RT_LOAL_FILE_ERR;
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
        {
            break;
        }
    }
    fclose(fp);
    delete[] pBuffer;
    if(iLen == _pHead->_iMemSize)
    {
        return RT_OK;
    }

    return RT_LOAL_FILE_ERR;
}

int TC_RBTree::checkDirty(const string &k)
{
    FailureRecover check(this);

    incGetCount();

    int ret         = TC_RBTree::RT_OK;
    lock_iterator it= find(_pHead->_iRootAddr, k, ret);
    if(ret != TC_RBTree::RT_OK)
    {
        return ret;
    }

    //没有数据
    if(it == end())
    {
        return TC_RBTree::RT_NO_DATA;
    }

    //只有Key
    if(it->isOnlyKey())
    {
        return TC_RBTree::RT_ONLY_KEY;
    }

    Block block(this, it->getAddr());
    if (block.isDirty())
    {
        return TC_RBTree::RT_DIRTY_DATA;
    }

    return TC_RBTree::RT_OK;
}

int TC_RBTree::setDirty(const string& k)
{
    FailureRecover check(this);

    if(_pHead->_bReadOnly) return RT_READONLY;

    incGetCount();

    int ret         = TC_RBTree::RT_OK;
    lock_iterator it= find(_pHead->_iRootAddr, k, ret);
    if(ret != TC_RBTree::RT_OK)
    {
        return ret;
    }

    //没有数据或只有Key
    if(it == end())
    {
        return TC_RBTree::RT_NO_DATA;
    }

    //只有Key
    if(it->isOnlyKey())
    {
        return TC_RBTree::RT_ONLY_KEY;
    }

    Block block(this, it->getAddr());
    block.setDirty(true);
    block.refreshSetList();

    return TC_RBTree::RT_OK;
}

int TC_RBTree::setClean(const string& k)
{
    FailureRecover check(this);

    if(_pHead->_bReadOnly) return RT_READONLY;

    incGetCount();

    int ret         = TC_RBTree::RT_OK;
    lock_iterator it= find(_pHead->_iRootAddr, k, ret);
    if(ret != TC_RBTree::RT_OK)
    {
        return ret;
    }

    //没有数据或只有Key
    if(it == end())
    {
        return TC_RBTree::RT_NO_DATA;
    }

    //只有Key
    if(it->isOnlyKey())
    {
        return TC_RBTree::RT_ONLY_KEY;
    }

    Block block(this, it->getAddr());
    block.setDirty(false);
    block.refreshSetList();

    return TC_RBTree::RT_OK;
}

int TC_RBTree::get(const string& k, string &v, time_t &iSyncTime)
{
    FailureRecover check(this);

    incGetCount();

    int ret             = TC_RBTree::RT_OK;

    lock_iterator it    = find(_pHead->_iRootAddr, k, v, ret);

    if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
    {
        return ret;
    }

    //没有数据
    if(it == end())
    {
        return TC_RBTree::RT_NO_DATA;
    }

    //只有Key
    if(it->isOnlyKey())
    {
        return TC_RBTree::RT_ONLY_KEY;
    }

    Block block(this, it->getAddr());
    iSyncTime = block.getSyncTime();

    //如果只读, 则不刷新get链表
    if(!_pHead->_bReadOnly)
    {
        block.refreshGetList();
    }

    return TC_RBTree::RT_OK;
}

int TC_RBTree::get(const string& k, string &v)
{
    time_t iSyncTime;
    return get(k, v, iSyncTime);
}

int TC_RBTree::set(const string& k, const string& v, bool bDirty, vector<BlockData> &vtData)
{
    FailureRecover check(this);
    incGetCount();

    if(_pHead->_bReadOnly) return RT_READONLY;

    int ret             = TC_RBTree::RT_OK;
    lock_iterator it    = find(_pHead->_iRootAddr, k, ret);
    bool bNewBlock      = false;

    if(ret != TC_RBTree::RT_OK)
    {
        return ret;
    }

    if(it == end())
    {
        TC_PackIn pi;
        pi << k;
        pi << v;
        uint32_t iAllocSize     = sizeof(Block::tagBlockHead) + pi.length();

        //先分配空间, 并获得淘汰的数据
        uint32_t iAddr         = _pDataAllocator->allocateMemBlock(iAllocSize, vtData);
        if(iAddr == 0)
        {
            return TC_RBTree::RT_NO_MEMORY;
        }

        it = RBTreeLockIterator(this, iAddr, RBTreeLockIterator::IT_GET, RBTreeLockIterator::IT_NEXT);

        bNewBlock = true;
    }

    ret = it->set(k, v, bNewBlock, vtData);
    if(ret != TC_RBTree::RT_OK)
    {
        //新记录, 写失败了, 要删除该块
        if(bNewBlock)
        {
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
    block.refreshSetList();

    return TC_RBTree::RT_OK;
}

int TC_RBTree::set(const string& k, vector<BlockData> &vtData)
{
    FailureRecover check(this);

    incGetCount();

    if(_pHead->_bReadOnly) return RT_READONLY;

    int ret             = TC_RBTree::RT_OK;
    lock_iterator it    = find(_pHead->_iRootAddr, k, ret);
    bool bNewBlock      = false;

    if(ret != TC_RBTree::RT_OK)
    {
        return ret;
    }

    if(it == end())
    {
        TC_PackIn pi;
        pi << k;
        uint32_t iAllocSize = sizeof(Block::tagBlockHead) + pi.length();

        //先分配空间, 并获得淘汰的数据
        uint32_t iAddr = _pDataAllocator->allocateMemBlock(iAllocSize, vtData);
        if(iAddr == 0)
        {
            return TC_RBTree::RT_NO_MEMORY;
        }

        it = RBTreeLockIterator(this, iAddr, RBTreeLockIterator::IT_GET, RBTreeLockIterator::IT_NEXT);

        bNewBlock = true;
    }

    ret = it->set(k, bNewBlock, vtData);
    if(ret != TC_RBTree::RT_OK)
    {
        //新记录, 写失败了, 要删除该块
        if(bNewBlock)
        {
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
    block.refreshSetList();
    return TC_RBTree::RT_OK;
}

int TC_RBTree::del(const string& k, BlockData &data)
{
    FailureRecover check(this);

    incGetCount();

    if(_pHead->_bReadOnly) return RT_READONLY;

    int    ret      = TC_RBTree::RT_OK;

    data._key       = k;

    lock_iterator it     = find(_pHead->_iRootAddr, k, data._value, ret);
    if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
    {
        return ret;
    }

    if(it == end())
    {
        return TC_RBTree::RT_NO_DATA;
    }

    Block block(this, it->getAddr());
    ret = block.getBlockData(data);
    block.erase();

    return ret;
}

int TC_RBTree::erase(int radio, BlockData &data, bool bCheckDirty)
{
    FailureRecover check(this);

    if(_pHead->_bReadOnly) return RT_READONLY;

    if(radio <= 0)   radio = 1;
    if(radio >= 100) radio = 100;

    uint32_t iAddr    = _pHead->_iGetTail;
    //到链表头部
    if(iAddr == 0)
    {
        return RT_OK;
    }

    //删除到指定比率了
    if((_pHead->_iUsedChunk * 100. / allBlockChunkCount()) < radio)
    {
        return RT_OK;
    }

    Block block(this, iAddr);
    if(bCheckDirty)
    {
        if(block.isDirty())
        {
            if(_pHead->_iDirtyTail == 0)
            {
                _pHead->_iDirtyTail = iAddr;
            }
            return RT_OK;
        }
    }
    int ret = block.getBlockData(data);
    block.erase();
    if(ret == TC_RBTree::RT_OK)
    {
        return TC_RBTree::RT_ERASE_OK;
    }

    return ret;
}

void TC_RBTree::sync()
{
    FailureRecover check(this);
    
    _pHead->_iSyncTail = _pHead->_iDirtyTail;
}

int TC_RBTree::sync(time_t iNowTime, BlockData &data)
{
    FailureRecover check(this);

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

    int ret = block.getBlockData(data);
    if(ret != TC_RBTree::RT_OK)
    {
        //没有获取完整的记录
        if(_pHead->_iDirtyTail == iAddr)
        {
            _pHead->_iDirtyTail = block.getBlockHead()->_iSetPrev;
        }
        return ret;
    }

    //脏数据且超过_pHead->_iSyncTime没有回写, 需要回写
    if(_pHead->_iSyncTime + data._synct < iNowTime && block.isDirty())
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

void TC_RBTree::backup(bool bForceFromBegin)
{
    FailureRecover check(this);
    if(bForceFromBegin || _pHead->_iBackupTail == 0)
    {
        //移动备份指针到Get链尾部, 准备开始备份数据
        _pHead->_iBackupTail = _pHead->_iGetTail;
    }
}

int TC_RBTree::backup(BlockData &data)
{
    FailureRecover check(this);

    uint32_t iAddr    = _pHead->_iBackupTail;

    //到链表头部了, 返回RT_OK
    if(iAddr == 0)
    {
        return RT_OK;
    }

    Block block(this, iAddr);
    int ret = block.getBlockData(data);

    //迁移一次
    _pHead->_iBackupTail = block.getBlockHead()->_iGetPrev;

    if(ret == TC_RBTree::RT_OK)
    {
        return TC_RBTree::RT_NEED_BACKUP;
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////////////

TC_RBTree::nolock_iterator TC_RBTree::nolock_begin()
{
    FailureRecover check(this);

    if(_pHead->_iRootAddr == 0)
    {
        return nolock_end();
    }

    Block root(this, _pHead->_iRootAddr);
    //最左节点
    while(root.hasLeft())
    {
        root.moveToLeft();
    }

    string key;
    lock_iterator it(this, root.getHead(), lock_iterator::IT_RBTREE, lock_iterator::IT_NEXT);

    while(true)
    {
        RBTreeLockItem item(this, it->getAddr());
        
        int ret = item.get(key);
        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
        {
            //当前元素有问题, 取下一个元素
            ++it;
            continue;
        }
        else
        {
            break;
        }
    }

    return nolock_iterator(this, key, false, lock_iterator::IT_NEXT);
}

TC_RBTree::nolock_iterator TC_RBTree::nolock_rbegin()
{
    FailureRecover check(this);

    if(_pHead->_iRootAddr == 0)
    {
        return nolock_end();
    }

    Block root(this, _pHead->_iRootAddr);
    //最右节点
    while(root.hasRight())
    {
        root.moveToRight();
    }

    string key;
    lock_iterator it(this, root.getHead(), lock_iterator::IT_RBTREE, lock_iterator::IT_PREV);

    while(true)
    {
        RBTreeLockItem item(this, it->getAddr());
        
        int ret = item.get(key);
        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
        {
            ++it;
            continue;
        }
        else
        {
            break;
        }
    }

    return nolock_iterator(this, key, false, lock_iterator::IT_PREV);
}

////////////////////////////////////////////////////////////////////////////////
///

TC_RBTree::lock_iterator TC_RBTree::begin()
{
    FailureRecover check(this);

    if(_pHead->_iRootAddr == 0)
    {
        return end();
    }

    Block root(this, _pHead->_iRootAddr);
    //最左节点
    
    while(root.hasLeft())
    {
        root.moveToLeft();
    }

    return lock_iterator(this, root.getHead(), lock_iterator::IT_RBTREE, lock_iterator::IT_NEXT);
}

TC_RBTree::lock_iterator TC_RBTree::rbegin()
{
    FailureRecover check(this);

    if(_pHead->_iRootAddr == 0)
    {
        return end();
    }

    Block root(this, _pHead->_iRootAddr);
    //最左节点
    
    while(root.hasRight())
    {
        root.moveToRight();
    }

    return lock_iterator(this, root.getHead(), lock_iterator::IT_RBTREE, lock_iterator::IT_PREV);
}

TC_RBTree::lock_iterator TC_RBTree::find(const string& k)
{
    FailureRecover check(this);
    int ret;
    return find(_pHead->_iRootAddr, k, ret);
}

TC_RBTree::lock_iterator TC_RBTree::rfind(const string& k)
{
    FailureRecover check(this);
    int ret;
    return find(_pHead->_iRootAddr, k, ret, false);
}

TC_RBTree::lock_iterator TC_RBTree::lower_bound(const string &k)
{
    FailureRecover check(this);

    if(_pHead->_iRootAddr == 0)
    {
        return end();
    }

    Block last = getLastBlock(k);
    RBTreeLockItem item(this, last.getHead());
    string k1;
    item.get(k1);

    //k 大于 map中的数据
    if(_lessf(k1, k))
    {
        lock_iterator it(this, last.getHead(), lock_iterator::IT_RBTREE, lock_iterator::IT_NEXT);
        ++it;
        return it;  
    }
    return lock_iterator(this, last.getHead(), lock_iterator::IT_RBTREE, lock_iterator::IT_NEXT);
}

TC_RBTree::lock_iterator TC_RBTree::upper_bound(const string &k)
{
    FailureRecover check(this);

    if(_pHead->_iRootAddr == 0)
    {
        return end();
    }

    Block last = getLastBlock(k);
    RBTreeLockItem item(this, last.getHead());
    string k1;
    item.get(k1);

    //k < map中的数据
    if(_lessf(k, k1))
    {
        lock_iterator it(this, last.getHead(), lock_iterator::IT_RBTREE, lock_iterator::IT_NEXT);
        return it;
    }

    //k >= map中的数据
    lock_iterator it(this, last.getHead(), lock_iterator::IT_RBTREE, lock_iterator::IT_NEXT);
    ++it;
    return it;
}

pair<TC_RBTree::lock_iterator, TC_RBTree::lock_iterator> TC_RBTree::equal_range(const string& k1, const string &k2)
{
    FailureRecover check(this);

    pair<lock_iterator, lock_iterator> pit;

    if(_lessf(k2, k1))
    {
        pit.first = end();
        pit.second = end();
        return pit;
    }
    pit.first   = lower_bound(k1);
    pit.second  = upper_bound(k2);

    return pit;
}

///////////////////////////////////////////////////////////////////////////////////////////
///
TC_RBTree::lock_iterator TC_RBTree::beginSetTime()
{
    FailureRecover check(this);
    return lock_iterator(this, _pHead->_iSetHead, lock_iterator::IT_SET, lock_iterator::IT_NEXT);
}

TC_RBTree::lock_iterator TC_RBTree::rbeginSetTime()
{
    FailureRecover check(this);
    return lock_iterator(this, _pHead->_iSetTail, lock_iterator::IT_SET, lock_iterator::IT_PREV);
}

TC_RBTree::lock_iterator TC_RBTree::beginGetTime()
{
    FailureRecover check(this);
    return lock_iterator(this, _pHead->_iGetHead, lock_iterator::IT_GET, lock_iterator::IT_NEXT);
}

TC_RBTree::lock_iterator TC_RBTree::rbeginGetTime()
{
    FailureRecover check(this);
    return lock_iterator(this, _pHead->_iGetTail, lock_iterator::IT_GET, lock_iterator::IT_PREV);
}

TC_RBTree::lock_iterator TC_RBTree::beginDirty()
{
    FailureRecover check(this);
    return lock_iterator(this, _pHead->_iDirtyTail, lock_iterator::IT_SET, lock_iterator::IT_PREV);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

string TC_RBTree::desc()
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
        s << "[ElementCount     = "   << _pHead->_iElementCount     << "]" << endl;
        s << "[EraseCount       = "   << _pHead->_iEraseCount     << "]" << endl;
        s << "[RootAddr         = "   << _pHead->_iRootAddr       << "]" << endl;
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
        s << "[ModifyStatus     = "   << (int)_pstModifyHead->_cModifyStatus     << "]" << endl;
        s << "[ModifyIndex      = "   << _pstModifyHead->_iNowIndex << "]" << endl;
        s << "[OnlyKeyCount     = "      << _pHead->_iOnlyKeyCount        << "]" << endl;
    }

    vector<TC_MemChunk::tagChunkHead> vChunkHead = _pDataAllocator->getBlockDetail();

    s << "*************************************************************************" << endl;
    s << "[DiffBlockCount   = "   << vChunkHead.size()  << "]" << endl;
    for(size_t i = 0; i < vChunkHead.size(); i++)
    {
        s << "[BlockSize        = "   << vChunkHead[i]._iBlockSize      << "]";
        s << "[BlockCount       = "   << vChunkHead[i]._iBlockCount     << "]";
        s << "[BlockAvailable   = "   << vChunkHead[i]._blockAvailable  << "]" << endl;
    }

    return s.str();
}

uint32_t TC_RBTree::eraseExcept(uint32_t iNowAddr, vector<BlockData> &vtData)
{
    //在真正数据淘汰前使修改生效
    doUpdate();

    //不能被淘汰
    if(!_pHead->_bAutoErase) return 0;

    uint32_t n = _pHead->_iEraseCount;
    if(n == 0) n = 10;
    uint32_t d = n;

    while (d != 0)
    {
        uint32_t iAddr;

        //判断按照哪种方式淘汰
        if(_pHead->_cEraseMode == TC_RBTree::ERASEBYSET)
        {
            iAddr = _pHead->_iSetTail;
        }
        else
        {
            iAddr = _pHead->_iGetTail;
        }

        if (iAddr == 0)
        {
            break;
        }

        Block block(this, iAddr);

        //当前block正在分配空间, 不能删除, 移到上一个数据
        if (iNowAddr == iAddr)
        {
            if(_pHead->_cEraseMode == TC_RBTree::ERASEBYSET)
            {
                iAddr = block.getBlockHead()->_iSetPrev;
            }
            else
            {
                iAddr = block.getBlockHead()->_iGetPrev;
            }
        }

        if(iAddr == 0)
        {
            break;
        }

        Block block1(this, iAddr);
        BlockData data;
        int ret = block1.getBlockData(data);
        if(ret == TC_RBTree::RT_OK)
        {
            vtData.push_back(data);
            d--;
        }
        else if(ret == TC_RBTree::RT_NO_DATA)
        {
            d--;
        }

        //做现场保护
        FailureRecover check(this);
        //删除数据
        block1.erase();
    }

    return n-d;
}

TC_RBTree::Block TC_RBTree::getLastBlock(const string &k)
{
    assert(_pHead->_iRootAddr != 0);

    uint32_t iNowAddr = _pHead->_iRootAddr;

    Block block(this, iNowAddr);

    while(iNowAddr != 0)
    {
        BlockData data;

        int iRet = block.getBlockData(data);
        if(iRet != TC_RBTree::RT_OK && iRet != TC_RBTree::RT_ONLY_KEY)
        {
            block.erase();
            //数据块有问题,删除后,自恢复
            iNowAddr = _pHead->_iRootAddr;
            continue;
        }

        iNowAddr = block.getHead();

        if(_lessf(k, data._key))
        {
            if(!block.moveToLeft())
                break;
        }
        else if(_lessf(data._key, k))
        {
            if(!block.moveToRight())
                break;
        }
        else
        {
            break;
        }
    }

    return Block(this, iNowAddr);
}

TC_RBTree::lock_iterator TC_RBTree::find(uint32_t iAddr, const string& k, int &ret, bool bOrder)
{
    if(iAddr == 0)
        return end();

    ret = TC_RBTree::RT_OK;

    string k1;
    Block block(this, iAddr);

    RBTreeLockItem mcmdi(this, block.getHead());
    if(mcmdi.equal(k, k1, ret))
    {
        incHitCount();
        if(bOrder)
        {
            return lock_iterator(this, block.getHead(), lock_iterator::IT_RBTREE, lock_iterator::IT_NEXT);
        }
        else
        {
            return lock_iterator(this, block.getHead(), lock_iterator::IT_RBTREE, lock_iterator::IT_PREV);
        }
    }

    if(_lessf(k, k1))
    {
        if(block.moveToLeft())
        {
            return find(block.getHead(), k, ret);
        }
    }
    else
    {
        if(block.moveToRight())
        {
            return find(block.getHead(), k, ret);
        }
    }

    return end();
}

TC_RBTree::lock_iterator TC_RBTree::find(uint32_t iAddr, const string& k, string &v, int &ret)
{
    if(iAddr == 0)
        return end();

    ret = TC_RBTree::RT_OK;

    string k1;
    Block block(this, iAddr);

    RBTreeLockItem mcmdi(this, block.getHead());
    if(mcmdi.equal(k, k1, v, ret))
    {
        incHitCount();
        return lock_iterator(this, block.getHead(), lock_iterator::IT_RBTREE, lock_iterator::IT_NEXT);
    }

    if(_lessf(k, k1))
    {
        if(block.moveToLeft())
        {
            return find(block.getHead(), k, v, ret);
        }
    }
    else
    {
        if(block.moveToRight())
        {
            return find(block.getHead(), k, v, ret);
        }
    }

    return end();
}

void TC_RBTree::doRecover()
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
            else if(_pstModifyHead->_stModifyData[i]._cBytes == sizeof(uint16_t))
            {
                *(uint16_t*)((char*)_pHead + _pstModifyHead->_stModifyData[i]._iModifyAddr) = (uint16_t)_pstModifyHead->_stModifyData[i]._iModifyValue;
            }
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

void TC_RBTree::doUpdate()
{
    //==2,已经修改成功, 清除
    if(_pstModifyHead->_cModifyStatus == 1)
    {
        _pstModifyHead->_iNowIndex        = 0;
        _pstModifyHead->_cModifyStatus    = 0;
    }
}

}

