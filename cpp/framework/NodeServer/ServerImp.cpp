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

#include "ServerImp.h"
#include "util.h"

int ServerImp::keepAlive( const tars::ServerInfo& serverInfo, tars::TarsCurrentPtr current )
{
    try
    {   
        string sApp     = serverInfo.application;
        string sName    = serverInfo.serverName;

        ServerObjectPtr pServerObjectPtr = ServerFactory::getInstance()->getServer( sApp, sName );
        if(pServerObjectPtr)
        {
            TLOGDEBUG("ServerImp::keepAlive server " << serverInfo.application << "." << serverInfo.serverName << " keep alive"<< endl);

            pServerObjectPtr->keepAlive(serverInfo.pid,serverInfo.adapter);

            return 0;
        }

        TLOGDEBUG("ServerImp::keepAlive server " << serverInfo.application << "." << serverInfo.serverName << " is not exist"<< endl);
    }
    catch ( exception& e )
    {
        TLOGERROR( "ServerImp::keepAlive catch exception :" << e.what() << endl);
    }
    catch ( ... )
    {
        TLOGERROR("ServerImp::keepAlive unkown exception catched" << endl);
    }

    return -1;
}

int ServerImp::reportVersion( const string &app,const string &serverName,const string &version,tars::TarsCurrentPtr current)
{
    try
    {
        TLOGDEBUG("ServerImp::reportVersion|server|" << app << "." << serverName << "|version|" << version<< endl);

        ServerObjectPtr pServerObjectPtr = ServerFactory::getInstance()->getServer( app, serverName );
        if(pServerObjectPtr)
        {
            pServerObjectPtr->setVersion(version);

            return 0;
        }

        TLOGDEBUG("ServerImp::reportVersion server " << app << "." << serverName << " is not exist"<< endl);
    }
    catch ( exception& e )
    {
        TLOGERROR("ServerImp::reportVersion catch exception :" << e.what() << endl);
    }
    catch ( ... )
    {
        TLOGERROR("ServerImp::reportVersion unkown exception catched" << endl);
    }

    return -1;
}


