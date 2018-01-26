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

#ifndef __SERVERLIMITRESOURCE_H_
#define __SERVERLIMITRESOURCE_H_

#include <unistd.h>
#include <iostream>
#include "util/tc_monitor.h"
#include "util/tc_autoptr.h"

using namespace tars;
using namespace std;

class ServerLimitResource : public TC_ThreadLock, public TC_HandleBase
{
public:

    ServerLimitResource(int iMaxCount=3,int iTimeInterval=5/*分钟*/,int iExpiredTime=30/*分钟*/,const string& app="",const string& servername="");

    ~ServerLimitResource(){}

public:
    void setLimitCheckInfo(int iMaxCount,int iTimeInterval,int iExpiredTime);
    /**
     * @brief 服务异常重启记录
     *
     *1 如果在_coreLimitTimeInterval时间段内，则增加异常个数
     *2 判断异常个数，如果在时间段内达到core限制_maxExcStopCount，则屏蔽core属性_closeCore
     *  并记录当前时间戳_enableCoreLimitTime
     */
    void addExcStopRecord();
    /**
     *@brief 检查是否需要关闭或者打开core
     *
     *1 如果服务core被关闭，则先检查被关闭的时间戳，如果过期，且本服务没有配置manual关闭，则打开core属性
     *2 检查core开关，如果true则关闭core，则异步通知服务关闭，否则打开开关
     *
     */
    int IsCoreLimitNeedClose(bool& bClose);

    //重置内部的运行时数据
    void resetRunTimeData();

private:

    //字段分为两类，运行时数据和配置数据
    bool    _closeCore;              //屏蔽core开关, 运行时
    int     _curExcStopCount;        //当前服务异常停止个数, 运行时
    int     _maxExcStopCount;        //最大服务异常停止个数，配置
    time_t  _excStopRecordTime;      //第一次异常停止时间, 运行时
    int32_t _coreLimitTimeInterval;  //时间间隔内core的限制，单位是分钟，配置
    time_t  _enableCoreLimitTime;    //上一次关闭core的时间, 运行时
    int32_t _coreLimitExpiredTime;   //core关闭的持续时间,单位为分钟，配置

    string  _appName;                //应用名
    string  _serverName;             //服务名
};

typedef TC_AutoPtr<ServerLimitResource> ServerLimitResourcePtr;

#endif


