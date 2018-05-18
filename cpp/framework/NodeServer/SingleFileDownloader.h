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

#ifndef __SINGLE_FILE_DOWNLOADER_H_
#define __SINGLE_FILE_DOWNLOADER_H_

#include "Patch.h"
#include <set>
#include "util/tc_monitor.h"
#include "util/tc_hash_fun.h"

class DownloadEvent : public TC_HandleBase
{
public:
    virtual void onDownloading(const FileInfo &fi, int pos) = 0;
};

typedef TC_AutoPtr<DownloadEvent> DownloadEventPtr;


//下载任务
struct DownloadTask
{
    string sLocalTgzFile;
};

inline bool operator<(const DownloadTask&l, const DownloadTask&r)
{
    if(l.sLocalTgzFile != r.sLocalTgzFile)  
    {
        return (l.sLocalTgzFile < r.sLocalTgzFile);
    }

    return false;
}

#define POOLSIZE 10

class DownloadTaskFactory
{
    /**定义hash处理器*/
    using hash_functor = std::function<uint32_t (const string &)>;

public:

    tars::TC_ThreadRecLock* getRecLock(const DownloadTask& task) 
    {
        size_t hashcode = _hashf(task.sLocalTgzFile);
        size_t index = hashcode % POOLSIZE;
        return &_lockPool[index];
    }

    static DownloadTaskFactory& getInstance()
    {
        return *_instance;
    }

private:
    DownloadTaskFactory()
    : _hashf(tars::hash<string>()) 
    {
    }

private:
    tars::TC_ThreadRecLock       _lockPool[POOLSIZE];
    hash_functor                _hashf;
    static DownloadTaskFactory* _instance;
};

//从patch上下载单个文件
class SingleFileDownloader
{
public:
    static int download(const PatchPrx &patchPrx, const string &remoteFile, const string &localFile, const DownloadEventPtr &pPtr, std::string & sResult);

private:
};
//////////////////////////////////////////////////////////////
#endif

