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

#include "PatchServer.h"
#include "PatchImp.h"
#include "PatchCache.h"

PatchCache  g_PatchCache;

void PatchServer::initialize()
{
    //增加对象
    addServant<PatchImp>(ServerConfig::Application + "." + ServerConfig::ServerName +".PatchObj");
    
    size_t memMax   = TC_Common::toSize(g_conf->get("/tars<MemMax>", "100M"), 1024*1024);
    size_t memMin   = TC_Common::toSize(g_conf->get("/tars<MemMin>", "1M"), 1024*1024);
    size_t memNum   = TC_Common::strto<size_t>(g_conf->get("/tars<MemNum>", "10"));

    g_PatchCache.setMemOption(memMax, memMin, memNum);

    int _expireTime = TC_Common::strto<int>(g_conf->get("/tars<ExpireTime>", "30"));

    TLOGDEBUG("PatchServer::initialize memMax:" << memMax << "|memMin:" << memMin << "|memNum:" << memNum << "|expireTime:" << _expireTime << endl);
}

void PatchServer::destroyApp()
{
    TLOGDEBUG("PatchServer::destroyApp ok" << endl);
}

int PatchServer::getExpireTime() const
{
    return _expireTime;
}
