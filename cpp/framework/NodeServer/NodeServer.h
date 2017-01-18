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

#ifndef __NODE_SERVER_H_
#define __NODE_SERVER_H_

#include "servant/Application.h"
#include "KeepAliveThread.h"
#include "ReportMemThread.h"
#include "QueryF.h"
#include "BatchPatchThread.h"
#include "RemoveLogThread.h"
#include "util.h"

using namespace tars;

const string NODE_VERSION="B001";

class NodeServer;

extern NodeServer g_app;

class NodeServer : public Application
{
public:
     /**
     * 获取Adapter Endpoint
     */
    TC_Endpoint getAdapterEndpoint(const string& name ) const;

      /**
     * 添加Config
     * @param filename
     */
    bool addConfig(const string &filename){return Application::addConfig(filename);}

    /*
    *   权限控制
    */
    bool isValid(const string& ip);

     /*
    *   上报string至notify
    */
    void reportServer(const string &sServerId,const string &sResult);

public:
    /*
    *   管理指令
    */
    bool cmdViewServerDesc(const string& command, const string& params, string& result);
    /**
     *@brief  加载服务配置文件,并使之生效
     *
     */
    bool cmdReLoadConfig(const string& command, const string& params, string& result);

protected:

    /**
     * 初始化, 只会进程调用一次
     */
    virtual void initialize();

    /**
     * 析够, 只会进程调用一次
     */
    virtual void destroyApp();

    /**
     * 初始化hashmap
     */
    void initHashMap();
private:
    /**
     *初始化主控obj名字
     */
    void initRegistryObj();
private:
    KeepAliveThread *   _keepAliveThread;
    ReportMemThread *    _reportMemThread;

    BatchPatch *        _batchPatchThread;
    RemoveLogManager *    _removeLogThread;

    static string       g_sNodeIp;
};
#endif


