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
#include "AdminReapThread.h"

using namespace tars;

/**
 *  AdminRegistryServer Server
 */
class AdminRegistryServer : public Application
{
protected:
    /**
     * 初始化, 只会进程调用一次
     */
    virtual void initialize();

    /**
     * 析构, 每个进程都会调用一次
     */
    virtual void destroyApp();

public:
    /**
     * 获取registry对象的端口信息
     */
    map<string, string> getServantEndpoint() { return _mapServantEndpoint; }

private:
    int loadServantEndpoint();

protected:

    //用于执行定时操作的线程对象
    AdminReapThread     _reapThread;

    //对象-适配器 列表
    map<string, string> _mapServantEndpoint;
};

extern AdminRegistryServer g_app;
extern const string SERVER_VERSION;
