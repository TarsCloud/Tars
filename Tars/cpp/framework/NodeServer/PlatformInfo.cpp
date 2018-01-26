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

#include "PlatformInfo.h"
#include "util/tc_clientsocket.h"
#include "NodeServer.h"

NodeInfo PlatformInfo::getNodeInfo() const
{
    NodeInfo tNodeInfo;
    tNodeInfo.nodeName      = getNodeName();
    tNodeInfo.dataDir       = getDataDir();
    TC_Endpoint tEndPoint   = g_app.getAdapterEndpoint("NodeAdapter");
    tNodeInfo.nodeObj       = ServerConfig::Application + "." + ServerConfig::ServerName + ".NodeObj@" + tEndPoint.toString();
    tNodeInfo.endpointIp    = tEndPoint.getHost();
    tNodeInfo.endpointPort  = tEndPoint.getPort();
    tNodeInfo.timeOut       = tEndPoint.getTimeout();
    tNodeInfo.version       = TARS_VERSION+string("_")+NODE_VERSION;

    return tNodeInfo;
}

LoadInfo PlatformInfo::getLoadInfo() const
{
    LoadInfo info;
    info.avg1   = -1.0f;
    info.avg5   = -1.0f;
    info.avg15  = -1.0f;

    double loadAvg[3];
    if ( getloadavg( loadAvg, 3 ) != -1 )
    {
        info.avg1   = static_cast<float>( loadAvg[0] );
        info.avg5   = static_cast<float>( loadAvg[1] );
        info.avg15  = static_cast<float>( loadAvg[2] );
    }

    return  info;
}

string PlatformInfo::getNodeName() const
{
    return ServerConfig::LocalIp;
}

string PlatformInfo::getDataDir() const
{
    string sDataDir;
    sDataDir = ServerConfig::DataPath;

    if ( TC_File::isAbsolute(sDataDir) == false)
    {
        char cwd[PATH_MAX];
        if ( getcwd( cwd, PATH_MAX ) == NULL )
        {
            TLOGERROR("PlatformInfo::getDataDir cannot get the current directory:\n" << endl);
            exit( 0 );
        }
        sDataDir = string(cwd) + '/' + sDataDir;
    }

    sDataDir = TC_File::simplifyDirectory(sDataDir);
    if ( sDataDir[sDataDir.length() - 1] == '/' )
    {
        sDataDir = sDataDir.substr( 0, sDataDir.length() - 1 );
    }

    return sDataDir;
}

string PlatformInfo::getDownLoadDir() const
{
    string sDownLoadDir = "";
    try
    {
        sDownLoadDir = g_app.getConfig().get("/tars/node<downloadpath>","");
        if(sDownLoadDir == "")
        {
            string sDataDir       = getDataDir();   
            string::size_type pos =  sDataDir.find_last_of("/");
            if(pos != string::npos)
            {
                sDownLoadDir    = sDataDir.substr(0,pos)+"/tmp/download/";
            }
        }

        sDownLoadDir = TC_File::simplifyDirectory(sDownLoadDir);
        if(!TC_File::makeDirRecursive( sDownLoadDir ))
        {
            TLOGERROR("getDownLoadDir property `tars/node<downloadpath>' is not set and cannot create dir:"<<sDownLoadDir<<endl);
            exit(-1);
        }
    }
    catch(exception &e)
    {
        TLOGERROR("PlatformInfo::getDownLoadDir "<< e.what() << endl);
        exit(-1);
    }

    return sDownLoadDir;
}


