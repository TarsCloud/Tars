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

#include "servant/Application.h"
#include "RegistryImp.h"
#include "QueryImp.h"
#include "ReapThread.h"
#include "CheckNodeThread.h"
#include "CheckSettingState.h"
#include "RegistryProcThread.h"

using namespace tars;

//////////////////////////////////////////////////////
/**
 *  主控服务
 */
class RegistryServer : public Application
{
public:
    /**
     * 初始化, 服务启动时会调用一次
     */
    virtual void initialize();

    /**
     * 析构, 服务退出时会调用一次
     */
    virtual void destroyApp();

    /**
     * 获取registry对象的端口信息
     */
    map<string, string> getServantEndpoint() { return _mapServantEndpoint; }

    /**
     * 加载registry对象的端口信息
     */
    int loadServantEndpoint();

    /**
     * 获取异步处理线程的对象指针
     */
    RegistryProcThread * getRegProcThread();

    /**
     * 根据name获取对应的ip、端口等信息
     */
    TC_Endpoint getAdapterEndpoint(const string& name ) const;

protected:

    /*
     * 用于执行定时操作的线程对象
     */
    ReapThread             _reapThread;             //全量和增量加载路由信息的线程

    CheckNodeThread        _checkNodeThread;        //监控tarsnode超时的线程

    CheckSettingState      _checksetingThread;      //监控所有服务状态的线程

    RegistryProcThreadPtr  _registryProcThread;     //处理心跳、上报等的异步线程

    /*
     * 对象-适配器 列表
     */
    map<string, string>    _mapServantEndpoint;

};


