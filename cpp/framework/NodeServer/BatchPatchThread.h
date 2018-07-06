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

#ifndef __BATCH_PATCH_THREAD_H_
#define __BATCH_PATCH_THREAD_H_
#include "servant/Application.h"
#include "ServerObject.h"
#include "util/tc_thread_queue.h"

class BatchPatchThread;

class BatchPatch //: public tars::TC_ThreadLock
{
public:
    /**
    * 构造函数
    */
    BatchPatch();

    /**
    * 析构函数
    */
    ~BatchPatch();

    /**
     * 结束线程
     */
    void terminate();

    /**
     * 线程运行
     */
    void start(int iThreadNum);

    void timedWait(int millsecond);
public:
    /**
     * 插入发布请求
     */
    void push_back(const tars::PatchRequest & request, ServerObjectPtr servePtr);

    /**
     * 从发布队列中获取发布请求
     */
    bool pop_front(pair<tars::PatchRequest,ServerObjectPtr>& item);

    /**
     * 设置下载暂存目录
     */
    void setPath(const std::string & sDownloadPath)
    {
        _downloadPath = sDownloadPath;
    }
private:
    tars::TC_ThreadLock                  _queueMutex;

    TC_ThreadQueue<pair<tars::PatchRequest,ServerObjectPtr> > _patchQueue;

    std::set<std::string>               _patchIng;

    std::vector<BatchPatchThread *>     _runners;

    std::string                         _downloadPath;       
};


class BatchPatchThread : public TC_Thread
{
public:
    BatchPatchThread(BatchPatch * patch);

    ~BatchPatchThread();

    virtual void run();

    void terminate();

public:
    /**
     * 设置下载暂存目录
     */
    void setPath(const std::string & sDownloadPath)
    {
        _downloadPath = sDownloadPath;
    }

    /**
     * 执行发布单个请求
     */
    void doPatchRequest(const tars::PatchRequest & request, ServerObjectPtr server);

protected:
    BatchPatch *    _batchPatch;

    std::string     _downloadPath; //文件下载目录

protected:
    bool            _shutDown; 
};

#endif
