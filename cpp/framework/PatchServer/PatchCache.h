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

#ifndef __PATCH_CACHE_H_
#define __PATCH_CACHE_H_

#include <string>
#include <map>
#include <vector>
#include "util/tc_monitor.h"

const size_t SIZE_BUMEM_MIN =   1 * 1024 * 1024;
const size_t SIZE_BUMEM_MAX = 100 * 1024 * 1024;

class PatchCache
{
private:
    enum FileLoad 
    {
        EM_NLOAD    = 0,
        EM_LOADING  = 1,
        EM_RELOAD   = 2,
        EM_LOADED   = 3   
    };

    struct MemState
    {
        string      FileName;       //文件路径
        size_t      FileSize;       //文件大小
        size_t      FileInode;      //Inode索引号
        time_t      FileTime;       //文件创建时间

        char *      MemBuf;         //该块内存的起始地址
        size_t      MemSize;        //该块内存的大小
        size_t      MemCount;       //指向该块内存的数目
        time_t      MemTime;        //该块内存最后操纵时间
        FileLoad    MemLoad;        //该块内存已经加载成功
    };
public:
    void setMemOption(const size_t MemMax, const size_t MemMin, const size_t MemNum)
    {
        _MemMax     = MemMax > SIZE_BUMEM_MAX ? SIZE_BUMEM_MAX : MemMax;
        _MemMin     = MemMin < SIZE_BUMEM_MIN ? SIZE_BUMEM_MIN : MemMin;
        _MemNum     = MemNum;
    }

    int load(const std::string & sFile, std::pair<char *, size_t> & mem);

    int release(const std::string & sFile);

private:
    int __loadFile(const std::string & sFile, char * szBuff, size_t sizeLen);

    int __getMem  (const std::string & sFile, struct MemState *& cur);

private:
    tars::TC_ThreadLock _mutex;

    std::vector<MemState *>             _vecMems;

    std::map<std::string, MemState *>   _mapFiles;

    size_t _MemMax;

    size_t _MemMin;

    size_t _MemNum;
};

#endif

