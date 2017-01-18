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

#include <sys/stat.h>
#include <unistd.h>
#include "PatchCache.h"
#include "PatchServer.h"

using namespace tars;

int PatchCache::load(const std::string & sFile, std::pair<char *, size_t> & mem)
{
    TLOGDEBUG("PatchCache::load sFile:" << sFile << endl);

    struct MemState * cur = NULL;

    TC_ThreadLock::Lock lock(_mutex);
    
    try
    {
        struct stat st;
        memset(&st, 0, sizeof(struct stat));

        if (lstat(sFile.c_str(), &st) != 0)
        {
            TLOGERROR("PatchCache::load sFile:" << sFile << "|lstat file error:" << strerror(errno) << endl);
            return -1;
        }
        
        if ((size_t)st.st_size > _MemMax || (size_t)st.st_size < _MemMin)
        {
            TLOGERROR("PatchCache::load sFile:" << sFile << "|invalid file size must(" << _MemMax << ">" << st.st_size << "<" << _MemMin << ")" << endl);
            return -1;
        }

        //查看是否已经加载入内存
        std::map<std::string, struct MemState *>::iterator it = _mapFiles.find(sFile);
        if (it != _mapFiles.end())
        {
            cur = it->second;
            
            if ((cur->MemLoad  == EM_LOADING || cur->MemLoad == EM_RELOAD))//错误都为1状态
            {
                TLOGERROR("PatchCache::load sFile:" << sFile << "|loading file now|MemLoad:" << cur->MemLoad << "|MemCount:" << cur->MemCount << "|FileName:" << cur->FileName << endl);

                //这里是由于内存分配不足，导致出现loading的状态
                _mapFiles.erase(sFile);

                return -1;
            }
            
            if (cur->FileSize == (size_t)st.st_size && cur->FileTime == st.st_mtime && cur->FileInode == st.st_ino)
            {
                mem.first       = cur->MemBuf;
                mem.second      = cur->FileSize;
                cur->MemTime    = time(NULL);
                cur->MemCount++;

                TLOGDEBUG("PatchCache::load sFile:" << sFile << "|reuse file in mem" << endl);
                return 0;
            }
            else
            {
                cur->FileSize   = st.st_size;
                cur->FileInode  = st.st_ino;
                cur->FileTime   = st.st_mtime;
                cur->MemLoad    = EM_RELOAD;
                
                mem.first       = cur->MemBuf;
                mem.second      = st.st_size;
                
                TLOGDEBUG("PatchCache::load sFile:" << sFile << "|need reload file" << endl);
            }
        }
        else
        {
            if (__getMem(sFile, cur) == -1)
            {
                TLOGERROR("PatchCache::load sFile:" << sFile << "|get free mem fault" << endl);
                return -1;
            }

            cur->FileName   = sFile;
            cur->FileSize   = st.st_size;
            cur->FileInode  = st.st_ino;
            cur->FileTime   = st.st_mtime;

            mem.first       = cur->MemBuf;
            mem.second      = cur->FileSize;

            if (_mapFiles.find(sFile) == _mapFiles.end())
            {
                _mapFiles.insert(make_pair(sFile,cur));
            }
        }
    }
    catch (std::exception & ex)
    {
        TLOGERROR("PatchCache::load sFile:" << sFile << "|exception:" << ex.what() << endl);
        return -1;
    }
    catch (...)
    {
        TLOGERROR("PatchCache::load sFile:" << sFile << "|unknown Exception" << endl);
        return -1;
    }
    
    if (__loadFile(sFile, cur->MemBuf, cur->FileSize) == 0)
    {
        cur->MemCount++;
        cur->MemTime = time(NULL);
        cur->MemLoad = EM_LOADED;

        return 0;
    }
    else
    {
        cur->MemTime = 0;
        cur->MemLoad = EM_NLOAD;
        return -1;
    }

    return -1;
}

int PatchCache::release(const std::string & sFile)
{
    TLOGDEBUG("PatchCache::release sFile:" << sFile << endl);

    try
    {
        TC_ThreadLock::Lock lock(_mutex);
        
        std::map<std::string, MemState *>::iterator it = _mapFiles.find(sFile);
        if (it == _mapFiles.end())
        {
            TLOGERROR("PatchCache::release Find '" << sFile << "' fault" << endl);
            return -1;
        }

        it->second->MemCount--;
        
        TLOGDEBUG("PatchCache::release sFile:" << sFile << "|count:" << it->second->MemCount << endl);
        return 0;
    }
    catch (std::exception & ex)
    {
        TLOGERROR("PatchCache::release sFile:" << sFile  << "|exception:" << ex.what() << endl);
    }
    catch (...)
    {
        TLOGERROR("PatchCache::release sFile:" << sFile  << "|unknown Exception" << endl);
    }
    
    return -1;
}

int PatchCache::__getMem(const std::string & sFile, struct MemState *& cur)
{
    TLOGDEBUG("PatchCache::__getMem sFile:" << sFile << endl);

    cur = NULL;
    
    for (size_t i = 0; i < _vecMems.size(); i++)
    {
        struct MemState * st = _vecMems[i];

        TLOGDEBUG("PatchCache::__getMem sFile:" << sFile << "|MemLoad:"<<st->MemLoad << "|MemCount:" << st->MemCount << "|MemTime:" << st->MemTime << "|FileName:" << st->FileName << endl);

        if ((st->MemLoad == EM_NLOAD || st->MemLoad == EM_LOADED) && st->MemCount == 0 && st->MemTime + g_app.getExpireTime() <= time(NULL))
        {
            _mapFiles.erase(_vecMems[i]->FileName);

            cur = _vecMems[i];
            break;
        }
    }
    
    if (cur == NULL && _vecMems.size() < _MemNum)
    {
        cur = new MemState();
        cur->MemBuf = (char *)malloc(_MemMax);//从这里开始新建

        _vecMems.push_back(cur);

        _mapFiles.insert(std::make_pair(sFile, cur));
    }

    if (cur != NULL)
    {
        cur->MemCount   = 0;
        cur->MemSize    = _MemMax;
        cur->MemLoad    = EM_LOADING;

        if (cur->MemBuf == NULL)//有可能内存分配失败
        {
            //从vector删除新的分配内存失败的元素
            _vecMems.erase(remove(_vecMems.begin(),_vecMems.end(),cur));

            TLOGERROR("PatchCache::__getMem sFile:" << sFile << "|cur->MemBuf NULL" << endl);

            return -1;
        }

        return 0;
    }

    TLOGERROR("PatchCache::__getMem sFile:" << sFile << "|not enough mem" << endl);

    return -1;
}

int PatchCache::__loadFile(const std::string & sFile, char * szBuff, size_t sizeLen)
{
    TLOGDEBUG("PatchCache::__loadFile sFile:" << sFile << "|sizeLen:" << sizeLen << endl);
    
    if(szBuff == NULL)
    {
        TLOGERROR("PatchCache::__loadFile sFile:" << sFile << "|sizeLen:" << sizeLen << "|szBuff NULL"<< endl);

        return -4;
    }

    FILE * fp = fopen(sFile.c_str(), "r");
    if (fp == NULL)
    {
        TLOGERROR("PatchCache::__loadFile sFile:" << sFile << "|sizeLen:" << sizeLen << "|open file error:" << strerror(errno) << endl);
        return -1;
    }
    
    //从指定位置开始读数据
    if (fseek(fp, 0, SEEK_SET) == -1)
    {
        fclose(fp);

        TLOGERROR("PatchCache::__loadFile sFile:" << sFile << "|sizeLen:" << sizeLen << "|fseek error:" << strerror(errno) << endl);
        return -2;
    }

    //开始读取文件
    size_t r = fread(szBuff, 1, sizeLen, fp);
    if (r > 0)
    {
        //成功读取r字节数据
        fclose(fp);
        TLOGDEBUG("PatchCache::__loadFile sFile:" << sFile << "|sizeLen:" << sizeLen << "|r:" << r << endl);
        return 0;
    }
    else
    {
        //到文件末尾了
        if (feof(fp))
        {
            fclose(fp);
            TLOGDEBUG("PatchCache::__loadFile sFile:" << sFile << "|sizeLen:" << sizeLen << "|to tail ok" << endl);
            return 1;
        }

        //读取文件出错了
        TLOGERROR("PatchCache::__loadFile sFile:" << sFile << "|sizeLen:" << sizeLen << "|r:" << r << "|error:" << strerror(errno) << endl);
        fclose(fp);
        return -3;
    }

}

