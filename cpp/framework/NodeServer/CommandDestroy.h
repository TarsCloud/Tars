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

#ifndef __DESTROY_COMMAND_H_
#define __DESTROY_COMMAND_H_

#include "ServerCommand.h"
#include "NodeRollLogger.h"

/**
 * 销毁服务
 *
 */
extern RemoveLogManager * g_RemoveLogThread;

class CommandDestroy : public ServerCommand
{
public:
    CommandDestroy(const ServerObjectPtr &pServerObjectPtr,bool bByNode = false);
    ExeStatus canExecute(string &sResult);
    int execute(string &sResult);

private:
    bool    _byNode;
    ServerDescriptor    _desc;
    ServerObjectPtr     _serverObjectPtr;
};

//////////////////////////////////////////////////////////////
//
inline CommandDestroy::CommandDestroy(const ServerObjectPtr &pServerObjectPtr,bool bByNode)
: _byNode(bByNode)
, _serverObjectPtr(pServerObjectPtr)
{
    _desc      = _serverObjectPtr->getServerDescriptor();
}
//////////////////////////////////////////////////////////////
//
inline ServerCommand::ExeStatus CommandDestroy::canExecute(string &sResult)
{
    TC_ThreadRecLock::Lock lock( *_serverObjectPtr );

    NODE_LOG("destroyServer")->debug()<<FILE_FUN << _desc.application<< "." << _desc.serverName << " beging destroyed------|"<< endl;

    ServerObject::InternalServerState eState = _serverObjectPtr->getInternalState();

    if (eState != ServerObject::Inactive)
    {
        sResult = FILE_FUN_STR+"server state is not Inactive. the curent state is " + _serverObjectPtr->toStringState( eState );
        NODE_LOG("destroyServer")->debug()<<FILE_FUN << sResult << endl;
        return DIS_EXECUTABLE;
    }

    return EXECUTABLE;
}
//////////////////////////////////////////////////////////////
//
inline int CommandDestroy::execute(string &sResult)
{
    try
    {
        string sServerDir  = _serverObjectPtr->getServerDir();
        string sServerId   = _serverObjectPtr->getServerId();

        //删除服务日志目录
        string sLogPath = _serverObjectPtr->getLogPath();
        if (sLogPath.empty())
        {//使用默认路径
            sLogPath = "/usr/local/app/tars/app_log/";
        }

        vector<string> vtServerNames = TC_Common::sepstr<string>(sServerId,".");
        if (vtServerNames.size() != 2)
        {
            NODE_LOG("destroyServer")->error() <<FILE_FUN<<"parse serverid error:" << sServerId << "|" << vtServerNames.size()  << endl;
            sResult = FILE_FUN_STR+"failed to remove log path, server name error:" + sServerId;
            //return -1;
        }
        else
        {
            sLogPath += vtServerNames[0] + "/" + vtServerNames[1];
            sLogPath = TC_File::simplifyDirectory(sLogPath);

            g_RemoveLogThread->push_back(sLogPath);

            //异步删除data下文件或者子目录
            if(TC_File::isFileExistEx(sServerDir, S_IFDIR))
            {
                g_RemoveLogThread->push_back(sServerDir);
                NODE_LOG("destroyServer")->error()<<FILE_FUN <<"asyn remove:"<<sServerDir<<endl;
            }
            else
            {
                NODE_LOG("destroyServer")->error()<<FILE_FUN << "[" << sServerId << "]'s server path doesnot exist:" << sServerDir << endl;
            }
        }
    }
    catch (exception& e)
    {
        sResult =  FILE_FUN_STR+"Exception:"  + string(e.what());
        NODE_LOG("destroyServer")->error()<<FILE_FUN <<sResult <<endl;
        return -1;
    }
    catch(...)
    {
        sResult =  FILE_FUN_STR+"Exception: unknown";
        NODE_LOG("destroyServer")->error()<<FILE_FUN <<sResult <<endl;
    }

    return 0;
}

#endif
