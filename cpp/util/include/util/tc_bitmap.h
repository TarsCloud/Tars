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

#ifndef __TC_BIT_MAP_H__
#define __TC_BIT_MAP_H__

#include <iostream>
#include <string>
#include <vector>
#include "util/tc_ex.h"

using namespace std;

namespace tars
{
/////////////////////////////////////////////////
/** 
 * @file  tc_bitmap.h 
 * @brief  多位bitmap类. 
 */             
/////////////////////////////////////////////////
/**
 * @brief 异常
 */
struct TC_BitMap_Exception : public TC_Exception
{
    TC_BitMap_Exception(const string &buffer) : TC_Exception(buffer){};
    ~TC_BitMap_Exception() throw(){};
};


/**
 * @brief 内存bitmap，每个整数1位，可以支持多位，即几个整数多位.
 * 
 *  操作过程不加锁，如果有需要在外面调用的时候加，通常采用群锁策略.
 * 
 *  注意群锁策略应该/8，然后按照尾号分群锁
 */
class TC_BitMap
{
public:
    /**
     * @brief 内存的bitmap，每个整数保持1位 
     *
     */
    class BitMap
    {
    public:
        
        static const int _magic_bits[8];

        #define _set_bit(n,m)   (n|_magic_bits[m])
        #define _clear_bit(n,m) (n&(~_magic_bits[m]))
        #define _get_bit(n,m)   (n&_magic_bits[m])

        /**共享内存版本*/
        #define BM_VERSION      1

        /**
         * @brief 根据元素个数计算需要内存的大小
         * @param iElementCount, 需要保存的元素个数(元素从0开始记)
         * 
         * @return size_t
         */
        static size_t calcMemSize(size_t iElementCount);

        /**
         * @brief 初始化
         * @param pAddr 绝对地址
         * @param iSize 大小, 采用(calcMemSize)计算出来
         * @return      0: 成功, -1:内存不够
         */
        void create(void *pAddr, size_t iSize);

        /**
         * @brief 链接到内存块
         * @param pAddr  地址, 采用(calcMemSize)计算出来
         * @return       0, 成功, -1,版本不对, -2:大小不对
         */
        int connect(void *pAddr, size_t iSize);

        /**
         * @brief 是否有标识
         * @param i 
         * @return int, >0:有标识, =0:无标识, <0:超过范围
         */
        int get(size_t i);

        /**
         * @brief 设置标识
         * @param i 
         * @return int, >0:有标识, =0:无标识, <0:超过范围
         */
        int set(size_t i);

        /**
         * @brief 清除标识
         * @param i
         * 
         * @return int, >0:有标识, =0:无标识, <0:超过范围
         */
        int clear(size_t i);

        /**
         * @brief 清除所有的数据
         * 
         * @return int 
         */
        int clear4all();

        /**
         * @brief dump到文件
         * @param sFile
         * 
         * @return int
         */
        int dump2file(const string &sFile);

        /**
         * @brief 从文件load
         * @param sFile
         * 
         * @return int
         */
        int load5file(const string &sFile);

        /**共享内存头部*/
        struct tagBitMapHead
        {
           char     _cVersion;          /**版本, 当前版本为1*/
           size_t   _iMemSize;          /**共享内存大小*/
        }__attribute__((packed));

        /**
         * @brief 获取头部地址 
         * @return tagBitMapHead* 共享内存头部
         */
        BitMap::tagBitMapHead *getAddr() const   { return _pHead; }

        /**
         * @brief 获取内存大小 
         * @return 内存大小
         */
        size_t getMemSize() const                   { return _pHead->_iMemSize; }

    protected:

        /**
         * 共享内存头部
         */
        tagBitMapHead               *_pHead;

        /**
         * 数据块指针
         */
        unsigned char *             _pData;
    };

    /**
     * @brief 根据元素个数计算需要内存的大小
     * @param iElementCount  需要保存的元素个数(元素从0开始记)
     * @param iBitCount     每个元素支持几位(默认1位) (位数>=1) 
     * @return              所需内存的大小
     */
    static size_t calcMemSize(size_t iElementCount, unsigned iBitCount = 1);

    /**
     * @brief 初始化
     * @param pAddr 绝对地址
     * @param iSize 大小, 采用(calcMemSize)计算出来
     * @return      0: 成功, -1:内存不够
     */
    void create(void *pAddr, size_t iSize, unsigned iBitCount = 1);

    /**
     * @brief 链接到内存块
     * @param pAddr 地址，采用(calcMemSize)计算出来
     * @return      0：成功, -1：版本不对, -2:大小不对
     */
    int connect(void *pAddr, size_t iSize, unsigned iBitCount = 1);

    /**
     * @brief 是否有标识
     * @param i      元素值
     * @param iBit   第几位 
     * @return       int, >0:有标识, =0:无标识, <0:超过范围
     */
    int get(size_t i, unsigned iBit = 1);

    /**
     * @brief 设置标识
     * @param i     元素值
     * @param iBit  第几位 
     * @return      int, >0:有标识, =0:无标识, <0:超过范围
     */
    int set(size_t i, unsigned iBit = 1);

    /**
     * @brief 清除标识
     * @param i     元素值
     * @param iBit  第几位 
     * @return      int, >0:有标识, =0:无标识, <0:超过范围
     */
    int clear(size_t i, unsigned iBit = 1);

    /**
     * @brief 清除所有的标识
     * 
     * @param iBit  第几位
     * @return int 
     */
    int clear4all(unsigned iBit = (unsigned)(-1));

    /**
     * @brief dump到文件
     * @param sFile 
     * 
     * @return int
     */
    int dump2file(const string &sFile);

    /**
     * @brief 从文件load
     * @param sFile
     * 
     * @return int
     */
    int load5file(const string &sFile);

protected:
    vector<BitMap>   _bitmaps;
};

}

#endif

