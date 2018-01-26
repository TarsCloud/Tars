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

#ifndef __TARS_NOTIFY_H_
#define __TARS_NOTIFY_H_

#include "servant/NotifyF.h"
#include "servant/Global.h"
#include "util/tc_singleton.h"

using namespace std;

namespace tars
{

/**
 * 上报信息给Notify服务
 * 异步上报给notify服务
 */
class TarsRemoteNotify : public TC_Singleton<TarsRemoteNotify>
{
public:
    /**
     * 初始化
     * @param comm, 通信器
     * @param obj, 对象名称
     * @param notifyPrx
     * @param app
     * @param serverName
     *
     * @return int
     */
    int setNotifyInfo(const CommunicatorPtr &comm, const string &obj, const string & app, const string &serverName, const string &sSetName="");

    /**
     * 通知, 一定是异步上报的
     * @param message
     */
    void notify(NOTIFYLEVEL level, const string &sMesage);

    /**
     * 上报
     * @param sResult
     * @param bSync
     */
    void report(const string &sResult, bool bSync = false);

protected:
    /**
     * 通信器
     */
    CommunicatorPtr _comm;

    /**
     * 通知代理
     */
    NotifyPrx       _notifyPrx;

    /**
     * 应用
     */
    string          _app;

    /**
     * 服务名称
     */
    string          _serverName;

    /*
    *set 名字
    */
    string            _setName;
};

}

#endif
