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

#ifndef __TARS_NODEF_H__
#define __TARS_NODEF_H__

#include "servant/NodeF.h"
#include "servant/Global.h"
#include "util/tc_singleton.h"

namespace tars
{

/**
 * 给node发送心跳
 * 调用keepAlive异步发送心跳给node
 */
class TarsNodeFHelper : public TC_Singleton<TarsNodeFHelper>,public TC_ThreadMutex 
{
public:

    /**
     * 设置node信息
     * @param comm, 通信器
     * @param obj,
     * @param app
     * @param server
     */
    void setNodeInfo(const CommunicatorPtr &comm, const string &obj, const string &app, const string &server);

    /**
     * keepAlive
     */
    void keepAlive(const string &adapter = "");

    /**
     * 上报TARS的编译版本
     * @param version
     */
    void reportVersion(const string &version);

protected:
    /**
     * 通信器
     */
    CommunicatorPtr _comm;

    /**
     * Node
     */
    ServerFPrx      _nodePrx;

    /**
     * 信息
     */
    ServerInfo      _si;

    set<string>     _adapterSet;

};

}

#endif

