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

#ifndef __NOTIFY_COMMAND_H_
#define __NOTIFY_COMMAND_H_

#include "ServerCommand.h"
#include "NodeRollLogger.h"

/**
 * 通知服务
 * 
 */
class CommandNotify : public ServerCommand
{
public:
    CommandNotify(const ServerObjectPtr &pServerObjectPtr,const string &sMsg,bool bByNode = false);

    ExeStatus canExecute(string &sResult);

    int execute(string &sResult);
    
private:
    bool                _byNode;
    string              _msg;
    ServerDescriptor    _desc;
    ServerObjectPtr     _serverObjectPtr;     
};

//////////////////////////////////////////////////////////////
//
inline CommandNotify::CommandNotify(const ServerObjectPtr &pServerObjectPtr,const string &sMsg,bool bByNode)
:_byNode(bByNode)
,_msg(sMsg)
,_serverObjectPtr(pServerObjectPtr)
{ 
    _desc  = _serverObjectPtr->getServerDescriptor();
}
//////////////////////////////////////////////////////////////
//
inline ServerCommand::ExeStatus CommandNotify::canExecute(string &sResult)
{    
    NODE_LOG("notify")->debug()<<FILE_FUN<<"notify begining "<<  _desc.application + "." + _desc.serverName<<" " <<_msg<<endl;
    
    ServerObject::InternalServerState eState = _serverObjectPtr->getInternalState();
    
    if ( eState == ServerObject::Inactive)
    {
        sResult = "server state is Inactive. ";
        NODE_LOG("notify")->debug()<<FILE_FUN<<sResult<<endl;
        return DIS_EXECUTABLE;
    }

    if ( eState == ServerObject::Destroying)
    {
        sResult = "server state is Destroying. ";
        NODE_LOG("notify")->debug()<<FILE_FUN<<sResult<< endl;
        return DIS_EXECUTABLE;
    }

    return EXECUTABLE;
    
}
//////////////////////////////////////////////////////////////
//
inline int CommandNotify::execute(string &sResult)
{   
    try
    {
        TC_Endpoint ep;
        AdminFPrx pAdminPrx;    //服务管理代理

        string  sAdminPrx = "AdminObj@"+_serverObjectPtr->getLocalEndpoint().toString();
        pAdminPrx         = Application::getCommunicator()->stringToProxy<AdminFPrx>(sAdminPrx);
        sResult           = pAdminPrx->notify(_msg);
    }
    catch (exception& e)
    {
        NODE_LOG("notify")->debug()<<FILE_FUN<<"CommandNotify::execute notify "<<_desc.application + "." + _desc.serverName << " error:" << e.what() << endl;
        sResult = "error" + string(e.what());
        return -1;
    }

    return 0;
}

#endif 
