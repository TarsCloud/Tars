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

#include "util/tc_bitmap.h"
#include "util/tc_common.h"
#include <cassert>
#include <string.h>
#include <iostream>

namespace tars
{

const int TC_BitMap::BitMap::_magic_bits[8]={0x80,0x40,0x20,0x10,0x8,0x4,0x2,0x1};

size_t TC_BitMap::BitMap::calcMemSize(size_t iElementCount)
{
    assert(iElementCount > 0);

    iElementCount--;
    size_t iMemSize = iElementCount/8+1;
    
    iMemSize += sizeof(tagBitMapHead);
    return iMemSize;
}

void TC_BitMap::BitMap::create(void *pAddr, size_t iSize)
{
    memset((char*)pAddr, 0, iSize);

    _pHead = static_cast<tagBitMapHead*>(pAddr);
    _pHead->_cVersion   = BM_VERSION;
    _pHead->_iMemSize   = iSize;

    _pData = (unsigned char*)pAddr + sizeof(tagBitMapHead);
}

int TC_BitMap::BitMap::connect(void *pAddr, size_t iSize)
{
    _pHead = static_cast<tagBitMapHead*>(pAddr);
    if(_pHead->_cVersion != BM_VERSION)
    {
        return -1;
    }
    if(iSize != _pHead->_iMemSize)
    {
        return -2;
    }

    _pData = (unsigned char*)pAddr + sizeof(tagBitMapHead);
    return 0;
}

int TC_BitMap::BitMap::get(size_t i)
{
    if(i/8 >= (_pHead->_iMemSize-sizeof(tagBitMapHead)))
    {
        return -1;
    }
    unsigned char* p =_pData + i/8;
    return _get_bit(*p, i%8)>0?1:0;
}

int TC_BitMap::BitMap::set(size_t i)
{
    if(i/8 >= (_pHead->_iMemSize-sizeof(tagBitMapHead)))
    {
        return -1;
    }
    
    unsigned char* p=(unsigned char*)_pData + i/8;
    *p = _set_bit(*p, i%8);
    return (int)(*p)>0?1:0;
}

int TC_BitMap::BitMap::clear(size_t i)
{
    if(i/8 >= (_pHead->_iMemSize-sizeof(tagBitMapHead)))
    {
        return -1;
    }

    unsigned char* p = (unsigned char*)_pData + i/8;
    *p = _clear_bit(*p, i%8);
    return (int)(*p)>0?1:0;
}

int TC_BitMap::BitMap::clear4all()
{
    memset(_pData, 0, _pHead->_iMemSize-sizeof(tagBitMapHead));

    return 0;
}

int TC_BitMap::BitMap::dump2file(const string &sFile)
{
    FILE *fp = fopen(sFile.c_str(), "wb");
    if(fp == NULL)
    {
        return -1;
    }

    size_t ret = fwrite((void*)_pHead, 1, _pHead->_iMemSize, fp);
    fclose(fp);

    if(ret == _pHead->_iMemSize)
    {
        return 0;
    }
   return -1;
}

int TC_BitMap::BitMap::load5file(const string &sFile)
{
    FILE *fp = fopen(sFile.c_str(), "rb");
    if(fp == NULL)
    {
        return -1;
    }
    fseek(fp, 0L, SEEK_END);   
    size_t fs = ftell(fp);
    if(fs != _pHead->_iMemSize)
    {
        fclose(fp);
        return -2;
    }

    fseek(fp, 0L, SEEK_SET); 

    size_t iSize    = 1024*1024*10;
    size_t iLen     = 0;
    char *pBuffer  = new char[iSize];
    while(true)
    {
        int ret = fread(pBuffer, 1, iSize, fp); 
        if(ret == 0)
        {
            break;
        }
        //检查版本
        if(iLen == 0)
        {
            tagBitMapHead *tmp = (tagBitMapHead*)pBuffer;
            if(tmp->_cVersion != BM_VERSION)
            {
                fclose(fp);
                delete[] pBuffer;
                return -3;
            }
            if(tmp->_iMemSize != _pHead->_iMemSize)
            {
                fclose(fp);
                delete[] pBuffer;
                return -2;
            }
        }

        memcpy((char*)_pHead + iLen, pBuffer, ret);
        iLen += ret;
    }
    fclose(fp);
    delete[] pBuffer;

    if(iLen != _pHead->_iMemSize)
    {
        return -2;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////


size_t TC_BitMap::calcMemSize(size_t iElementCount, unsigned iBitCount)
{
    size_t n = BitMap::calcMemSize(iElementCount);
    if(n * iBitCount < n)
    {
        throw TC_BitMap_Exception("[TC_BitMap::calcMemSize] memory to much error"); 
    }
    return n * iBitCount;
}

void TC_BitMap::create(void *pAddr, size_t iSize, unsigned iBitCount)
{
    assert(iBitCount != 0);
    assert(iSize % iBitCount == 0);

    BitMap bitmap;
    size_t n = iSize/iBitCount;
    for(unsigned i = 0; i < iBitCount; i++)
    {
        bitmap.create((char*)pAddr + i*n, n);
        _bitmaps.push_back(bitmap);
    }
}

int TC_BitMap::connect(void *pAddr, size_t iSize, unsigned iBitCount)
{
    assert(iBitCount != 0);
    assert(iSize % iBitCount == 0);

    BitMap bitmap;
    size_t n = iSize/iBitCount;
    for(unsigned i = 0; i < iBitCount; i++)
    {
        int ret = bitmap.connect((char*)pAddr + i*n, n);
        if(ret != 0)
        {
            return ret;
        }
        _bitmaps.push_back(bitmap);
    }

    return 0;
}

int TC_BitMap::get(size_t i, unsigned iBit)
{
    assert(iBit != 0);

    if(iBit > _bitmaps.size())
    {
        throw TC_BitMap_Exception("[TC_BitMap::get] bit beyond range:"+TC_Common::tostr(iBit)+">"+TC_Common::tostr(_bitmaps.size())); 
    }

    return _bitmaps[iBit-1].get(i);
}

int TC_BitMap::set(size_t i, unsigned iBit)
{
    assert(iBit != 0);

    if(iBit > _bitmaps.size())
    {
        throw TC_BitMap_Exception("[TC_BitMap::get] bit beyond range:"+TC_Common::tostr(iBit)+">"+TC_Common::tostr(_bitmaps.size())); 
    }

    return _bitmaps[iBit-1].set(i);
}

int TC_BitMap::clear(size_t i, unsigned iBit)
{
    assert(iBit != 0);

    if(iBit > _bitmaps.size())
    {
        throw TC_BitMap_Exception("[TC_BitMap::get] bit beyond range:"+TC_Common::tostr(iBit)+">"+TC_Common::tostr(_bitmaps.size())); 
    }

    return _bitmaps[iBit-1].clear(i);
}

int TC_BitMap::clear4all(unsigned iBit)
{
    assert(iBit != 0);

    if (iBit != (unsigned)(-1) && iBit > _bitmaps.size())
    {
        throw TC_BitMap_Exception("[TC_BitMap::get] bit beyond range:"+TC_Common::tostr(iBit)+">"+TC_Common::tostr(_bitmaps.size())); 
    }

    for (vector<BitMap>::size_type i = 0; i < _bitmaps.size(); i++)
    {
        if (iBit == (unsigned)(-1) || iBit == i + 1)
        {
            _bitmaps[i].clear4all();
        }
    }

    return 0;
}

int TC_BitMap::dump2file(const string &sFile)
{
    FILE *fp = fopen(sFile.c_str(), "wb");
    if(fp == NULL)
    {
        return -1;
    }

    for(unsigned i = 0; i < _bitmaps.size(); i++)
    {
        size_t ret = fwrite((void*)_bitmaps[i].getAddr(), 1, _bitmaps[i].getMemSize(), fp);
        if(ret != _bitmaps[i].getMemSize())
        {
            fclose(fp);
            return -1;
        }
    }
    fclose(fp);
    return 0;
}

int TC_BitMap::load5file(const string &sFile)
{
    FILE *fp = fopen(sFile.c_str(), "rb");
    if(fp == NULL)
    {
        return -1;
    }

    //总内存大小
    size_t iAllSize = 0;
    for(unsigned i = 0; i < _bitmaps.size(); i++)
    {
        iAllSize += _bitmaps[i].getMemSize();
    }

    fseek(fp, 0L, SEEK_END);   
    size_t fs = ftell(fp);
    if(fs != iAllSize)
    {
        fclose(fp);
        return -2;
    }

    fseek(fp, 0L, SEEK_SET); 

    size_t iSize    = 1024*1024*10;
    size_t iLen     = 0;
    char *pBuffer  = new char[iSize];
    while(true)
    {
        int ret = fread(pBuffer, 1, iSize, fp); 
        if(ret == 0)
        {
            break;
        }
        //检查版本
        if(iLen == 0)
        {
            BitMap::tagBitMapHead *tmp = (BitMap::tagBitMapHead*)pBuffer;
            if(tmp->_cVersion != BM_VERSION)
            {
                fclose(fp);
                delete[] pBuffer;
                return -3;
            }
            if(tmp->_iMemSize != _bitmaps[0].getMemSize())
            {
                fclose(fp);
                delete[] pBuffer;
                return -2;
            }
        }

        memcpy((char*)_bitmaps[0].getAddr() + iLen, pBuffer, ret);
        iLen += ret;
    }
    fclose(fp);
    delete[] pBuffer;

    if(iLen != iAllSize)
    {
        return -2;
    }
    return 0;
}

}


