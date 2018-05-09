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

#include "NotifyServer.h"
#include "NotifyImp.h"
#include "jmem/jmem_hashmap.h"

TarsHashMap<NotifyKey, NotifyInfo, ThreadLockPolicy, MemStorePolicy> * g_notifyHash;

void NotifyServer::initialize()
{
    //增加对象
    addServant<NotifyImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".NotifyObj");

    //初始化hash
    g_notifyHash = new TarsHashMap<NotifyKey, NotifyInfo, ThreadLockPolicy, MemStorePolicy>();

    g_notifyHash->initDataBlockSize(TC_Common::strto<size_t>((*g_pconf)["/tars/hash<min_block>"]),
            TC_Common::strto<size_t>((*g_pconf)["/tars/hash<max_block>"]),
            TC_Common::strto<float>((*g_pconf)["/tars/hash<factor>"]));

    size_t iSize = TC_Common::toSize((*g_pconf)["/tars/hash<file_size>"], 1024 * 1024 * 10);
    if (iSize > 1024 * 1024 * 100)
        iSize = 1024 * 1024 * 100;
    char* data = new char[iSize];
    g_notifyHash->create(data, iSize);

    _loadDbThread = new LoadDbThread();
    _loadDbThread->init();
    _loadDbThread->start();
}

void NotifyServer::destroyApp()
{
    if(_loadDbThread != NULL)
    {
        delete _loadDbThread;
        _loadDbThread = NULL;
    }

    TLOGDEBUG("NotifyServer::destroyApp ok" << endl);
}
