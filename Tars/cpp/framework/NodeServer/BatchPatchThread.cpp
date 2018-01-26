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

#include "util/tc_md5.h"
#include "BatchPatchThread.h"
#include "ServerFactory.h"
#include "CommandPatch.h"
#include "NodeRollLogger.h"
#include "util.h"

using namespace tars;

BatchPatch::BatchPatch()
{
    
}

BatchPatch::~BatchPatch()
{
    terminate();
}

void BatchPatch::start(int iNum)
{
    for (int i = 0; i < iNum; i++)
    {
        BatchPatchThread * t = new BatchPatchThread(this);
        t->setPath(_downloadPath);
        t->start();

        _runners.push_back(t);
    }
}

void BatchPatch::terminate()
{
    for (size_t i = 0; i < _runners.size(); ++i)
    {
        if(_runners[i]->isAlive())
        {
            _runners[i]->terminate();
        }
    }

    for (size_t i = 0; i < _runners.size(); ++i)
    {
        if(_runners[i]->isAlive())
        {
            _runners[i]->getThreadControl().join();
        }
    }

}

void BatchPatch::push_back(const tars::PatchRequest & request, ServerObjectPtr serverPtr)
{
    {
        TC_ThreadLock::Lock LockQueue(_queueMutex);

        if (_patchIng.count(request.appname + request.servername) == 1)
        {
            std::string sException = "reduplicate patch request:" + request.version + "," + request.user;

            NODE_LOG("patchPro")->debug() << FILE_FUN<< request.appname + "." + request.servername << "|" << sException << endl;
            throw TC_Exception(sException);
        }

        if (!serverPtr)
        {
            NODE_LOG("patchPro")->error() << FILE_FUN<< "server null " <<endl;
            throw TC_Exception("sever null");
        }

        _patchQueue.push_back(make_pair(request,serverPtr));

        _patchIng.insert(request.appname + request.servername);
    }

}

bool BatchPatch::pop_front(pair<tars::PatchRequest,ServerObjectPtr>& item)
{
    TC_ThreadLock::Lock LockQueue(_queueMutex);

    bool bRet=false;

    bRet=_patchQueue.pop_front(item);

    if(bRet)
    {
        _patchIng.erase(item.first.appname+item.first.servername);
    }

    return bRet;
}

void BatchPatch::timedWait(int millsecond)
{
    TC_ThreadLock::Lock lock(_queueMutex);
    
    _queueMutex.timedWait(millsecond);
}

BatchPatchThread::BatchPatchThread(BatchPatch * patch) : _batchPatch(patch)
{
    _shutDown  = false;
}

BatchPatchThread::~BatchPatchThread()
{
    terminate();
}

void BatchPatchThread::terminate()
{
    _shutDown = true;

    if (isAlive())
    {
        getThreadControl().join();
    }
}

void BatchPatchThread::doPatchRequest(const tars::PatchRequest & request, ServerObjectPtr server)
{
    NODE_LOG("patchPro")->debug() << FILE_FUN
    << request.appname + "." + request.servername + "_" + request.nodename << "|"
    << request.groupname   << "|"
    << request.version     << "|"
    << request.user        << "|"
    << request.servertype  << "|"
    << request.patchobj    << "|"
    << request.md5         << "|"
    << request.ostype       << endl;


    //设置发布状态
    try
    {
        std::string sError;

        if (!server)
        {
            NODE_LOG("patchPro")->error() <<FILE_FUN<< request.appname + "." + request.servername << "|" << request.md5 << "|get server object fault:" << sError << endl;
            return ;        
        }

        //查看本地硬盘是否已经存在该文件
        NODE_LOG("patchPro")->debug() <<FILE_FUN<< request.appname + "." + request.servername << "|" << request.md5 << "|patch begin" << endl;

        CommandPatch command(server, _downloadPath, request);
        if (command.canExecute(sError) != ServerCommand::EXECUTABLE)
        {
            NODE_LOG("patchPro")->error() << FILE_FUN<< request.appname + "." + request.servername << "|" << request.md5 << "|canExecute error:" << sError << endl;
            return ;
        }

        if (command.execute(sError) == 0)
        {
            NODE_LOG("patchPro")->debug() <<FILE_FUN<< request.appname + "." + request.servername << "|" << request.md5 << "|patch succ" << endl;
        }
        else
        {
            NODE_LOG("patchPro")->error() <<FILE_FUN<< request.appname + "." + request.servername << "|" << request.md5 << "|patch fault:" << sError << endl;
        }
    }
    catch (exception & e)
    {
        NODE_LOG("patchPro")->error() << FILE_FUN<< request.appname + "." + request.servername << "|" << request.md5 << "|Exception:" << e.what() << endl;
    }
    catch (...)
    {
        NODE_LOG("patchPro")->error() <<FILE_FUN<< request.appname + "." + request.servername << "|" << request.md5 << "|Unknown Exception" << endl;
    }
}

void BatchPatchThread::run()
{
    while (!_shutDown)
    {
        try
        {
    
            pair<tars::PatchRequest,ServerObjectPtr> item;
            if (_batchPatch->pop_front(item))
            {
                if (!(item.second))
                {
                    NODE_LOG("patchPro")->error() <<FILE_FUN<< " server error" << endl;
                    throw TC_Exception("server error");
                }
                doPatchRequest(item.first,item.second);
            }
            else
            {
                _batchPatch->timedWait(2000);
            }
        }
        catch (exception& e)
        {
            NODE_LOG("patchPro")->error()<<FILE_FUN<<"catch exception|"<<e.what()<<endl;
        }
        catch (...)
        {
            NODE_LOG("patchPro")->error()<<FILE_FUN<<"catch unkown exception"<<endl;
        }
    }
}

