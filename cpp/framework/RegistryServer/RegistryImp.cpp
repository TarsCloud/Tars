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

#include <iostream>

#include "RegistryImp.h"
#include "RegistryProcThread.h"
#include "RegistryServer.h"

//初始化配置db连接
extern TC_Config * g_pconf;
extern RegistryServer g_app;

void RegistryImp::initialize()
{
    TLOGDEBUG("begin RegistryImp init"<<endl);

    _db.init(g_pconf);

    TLOGDEBUG("RegistryImp init ok."<<endl);

}

int RegistryImp::registerNode(const string & name, const NodeInfo & ni, const LoadInfo & li, tars::TarsCurrentPtr current)
{
    return _db.registerNode(name, ni, li);
}

int RegistryImp::keepAlive(const string & name, const LoadInfo & ni, tars::TarsCurrentPtr current)
{
    RegistryProcInfo procInfo;
    procInfo.nodeName = name;
    procInfo.loadinfo = ni;
    procInfo.cmd      = EM_NODE_KEEPALIVE;

    //放入异步处理线程中
    g_app.getRegProcThread()->put(procInfo);

    return 0;
}

vector<tars::ServerDescriptor> RegistryImp::getServers(const std::string & app,const std::string & serverName,const std::string & nodeName,tars::TarsCurrentPtr current)
{
    return  _db.getServers(app, serverName, nodeName);
}

int RegistryImp::updateServer(const string & nodeName, const string & app, const string & serverName, const tars::ServerStateInfo & stateInfo, tars::TarsCurrentPtr current)
{
    return _db.updateServerState(app, serverName, nodeName, "present_state", stateInfo.serverState, stateInfo.processId);
}

int RegistryImp::updateServerBatch(const std::vector<tars::ServerStateInfo> & vecStateInfo, tars::TarsCurrentPtr current)
{
    return _db.updateServerStateBatch(vecStateInfo);
}

int RegistryImp::destroy(const string & name, tars::TarsCurrentPtr current)
{
    return _db.destroyNode(name);
}

int RegistryImp::reportVersion(const string & app, const string & serverName, const string & nodeName, const string & version, tars::TarsCurrentPtr current)
{
    RegistryProcInfo procInfo;
    procInfo.appName    = app;
    procInfo.serverName = serverName;
    procInfo.nodeName   = nodeName;
    procInfo.tarsVersion = version;
    procInfo.cmd        = EM_REPORTVERSION;

    //放入异步处理线程中
    g_app.getRegProcThread()->put(procInfo);

    return 0;
}

int RegistryImp::getNodeTemplate(const std::string & nodeName,std::string &profileTemplate,tars::TarsCurrentPtr current)
{
    string sTemplateName;
    int iRet = _db.getNodeTemplateName(nodeName, sTemplateName);
    if(iRet != 0 || sTemplateName == "")
    {
        //默认模板配置
        sTemplateName = (*g_pconf)["/tars/nodeinfo<defaultTemplate>"];
    }

    string sDesc;
    profileTemplate = _db.getProfileTemplate(sTemplateName, sDesc);

    TLOGDEBUG(nodeName << " get sTemplateName:" << sTemplateName << " result:" << sDesc << endl);

    return 0;
}

int RegistryImp::getClientIp(std::string &sClientIp,tars::TarsCurrentPtr current)
{
    sClientIp = current->getIp();
    TLOGDEBUG("RegistryImp::getClientIp ip: " << sClientIp <<  endl);

    return 0;
}

int RegistryImp::updatePatchResult(const PatchResult & result, tars::TarsCurrentPtr current)
{
    TLOGDEBUG( "RegistryImp::updatePatchResult " << result.sApplication + "." + result.sServerName + "_" + result.sNodeName << "|V:" << result.sVersion << "|U:" <<  result.sUserName << endl);

    return _db.setPatchInfo(result.sApplication, result.sServerName, result.sNodeName, result.sVersion, result.sUserName);
}

