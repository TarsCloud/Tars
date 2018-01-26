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

#ifndef __KEEP_ALIVE_THREAD_H_
#define __KEEP_ALIVE_THREAD_H_
#include "Node.h"
#include <unistd.h>
#include "ServerFactory.h"
#include "util/tc_monitor.h"
#include "util/tc_thread.h"
#include "PlatformInfo.h"

using namespace tars;
using namespace std;

class KeepAliveThread : public TC_Thread//,public TC_ThreadLock
{
public:
    /**
     * 构造函数
     */
    KeepAliveThread();

    /**
     * 析构函数
     */
    ~KeepAliveThread();

    /**
     * 结束线程
     */
    void terminate();

protected:

    virtual void run();
    
    /**
     * 往registry注册node信息
     */
    bool registerNode();
    
    /**
     * 获取本node在registry配置服务信息
     */
    bool loadAllServers();

    /**
    * 上报registry node当前状态
    * @return int  非0失败
    */
    int reportAlive();

    /**
     * 检查node上server状态
     */
    void checkAlive();

    /**
     * 与registry 同步node上所属服务状态
     */
    int synStat();

    /**
     * 等待
     */
    bool timedWait(int millsecond);

protected:

    NodeInfo            _nodeInfo;
    PlatformInfo        _platformInfo;
    
    bool                _terminate; 
    time_t              _runTime;              //node运行时间
    int                 _heartTimeout;         //业务心跳超时时间(s)
    int                 _monitorInterval;      //监控server状态的间隔时间(s)
    int                 _monitorIntervalMs;    //新的监控状态间隔，改成毫秒
    int                 _synInterval;          //同步与regisrty server状态的间隔时间(s)
    string              _synStatBatch;         //批量同步

    RegistryPrx         _registryPrx;

private:

    vector<ServerStateInfo>     _stat;         //服务状态列表
    TC_ThreadLock                _lock;
};


#endif

