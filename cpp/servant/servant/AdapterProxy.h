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

#ifndef __TARS_ADAPTER_PROXY_H_
#define __TARS_ADAPTER_PROXY_H_

#include "util/tc_timeout_queue_new.h"
#include "util/tc_timeout_queue_map.h"
#include "servant/Global.h"
#include "servant/EndpointInfo.h"
#include "servant/ObjectProxy.h"
#include "servant/Transceiver.h"
#include "servant/Message.h"
#include "servant/StatReport.h"
#include <queue>

namespace tars
{
////////////////////////////////////////////////////////////////////////
/**
 * 每个Adapter对应一个Endpoint，也就是一个服务端口
 */
class AdapterProxy
{
public:
    /**
     * 构造函数
     * @param ep
     * @param op
     */
    AdapterProxy(ObjectProxy * pObjectProxy, const EndpointInfo &ep, Communicator* pCom);

    /**
     * 析构函数
     */
    ~AdapterProxy();

    /**
     * 调用server端对象方法
     */
    int invoke(ReqMessage * msg);

    /**
     * 发送请求
     * 发送挤压的数据
     */
    void doInvoke();

    /**
     * server端的响应包返回
     */
    void finishInvoke(ResponsePacket &rsp);

    /**
     * 端口是否有效,当连接全部失效时返回false
     * @param bForceConnect : 是否强制发起连接,为true时不对状态进行判断就发起连接
     * @return bool
     */
    bool checkActive(bool bForceConnect = false);

    /**
     * 记录连接是否异常
     */
    void addConnExc(bool bExc);

    /**
     * 处理超时
     */
    void doTimeout();

    /**
     * 处理stat
     */
    void doStat(map<StatMicMsgHead, StatMicMsgBody> & mStatMicMsg);

    /**
     * 处理采样
     */
    void sample(ReqMessage * msg);

    /**
     * 获取ObjectProxy
     */
    inline ObjectProxy * getObjProxy() { return _objectProxy; }

    /**
     * 获取端口信息
     * @return const EndpointInfo&
     */
    inline const EndpointInfo & endpoint() const { return _endpoint; }

    /**
     * 连接超时的时间
     */
    int getConTimeout();

    /**
     * 连接是否超时
     */
    inline bool isConnTimeout() { return _connTimeout; }

    /**
     * 设置连接是否超时
     */
    void   setConTimeout(bool bConTimeout);

    /**
     * 连接是否异常
     */
    inline bool isConnExc() { return _connExc; }

    /**
     * 连接异常的次数
     */
    inline int ConnExcCnt() const { return _connExcCnt; }

    /**
     * 服务在主控的状态是否为active
     */
    inline bool isActiveInReg() { return _activeStateInReg; }

    /**
     * 服务在主控的状态是否为active
     */
    inline void setActiveInReg(bool bActive) { _activeStateInReg = bActive; }

    /**
     * 获取连接
     *
     * @return Transceiver*
     */
    inline Transceiver* trans() { return _trans.get(); }

    /**
     * 设置节点的静态权重值
     */
    inline void setWeight(int iWeight) { _staticWeight = iWeight; }

    /**
     * 获取节点的静态权重值
     */
    inline int getWeight() { return _staticWeight; }

private:

    /**
     * 屏蔽结点
     */
    void setInactive();

private:

    /**
     * 请求的响应处理
     */
    void finishInvoke(ReqMessage * msg);

    /**
     * 超时统计处理
     */
    void finishInvoke(bool bTimeout);

    /**
     * 初始化模块间调用信息的头部
     */
    void initStatHead();

    /**
     * 模块间调用信息的统计
     */
    void stat(ReqMessage * msg);

    /**
     * 模块间调用信息的合并
     */
    void merge(const StatMicMsgBody& inBody, StatMicMsgBody& outBody);

    /**
     * 获取被调名
     */
    string getSlaveName(const string& sSlaveName);

private:

    /*
     * 通信器
     */
    Communicator*                           _communicator;

    /*
     * ObjectProxy
     */
    ObjectProxy*                           _objectProxy;

    /*
     * 节点信息
     */
    EndpointInfo                           _endpoint;

    /*
     * 收发包处理
     */
    std::unique_ptr<Transceiver>           _trans;

    /*
     * 超时队列
     */
    std::unique_ptr<TC_TimeoutQueueNew<ReqMessage*>> _timeoutQueue;

    /*
     * 节点在主控的存活状态
     */
    bool                                   _activeStateInReg;

    /*
     * 节点当前的存活状态
     */
    bool                                   _activeStatus;

    /*
     * 单位时间内的总调用次数
     */
    uint32_t                               _totalInvoke;

    /*
     * 单位时间内的调用超时次数
     */
    uint32_t                               _timeoutInvoke;

    /*
     * 下一次进行超时比率检测的时间
     */
    time_t                                 _nextFinishInvokeTime;

    /*
     * 连续超时的次数
     */
    uint32_t                               _frequenceFailInvoke;

    /*
     * 连续失败的检查时间点
     */
    time_t                                 _frequenceFailTime;

    /*
     * 下一次重试的时间
     */
    time_t                                 _nextRetryTime;

    /*
     * 是否连接超时
     */
    bool                                   _connTimeout;

    /*
     * 是否连接异常
     */
    bool                                   _connExc;

    /*
     * 连接异常的次数
     */
    uint32_t                               _connExcCnt;

    /*
     * 静态权重值
     */
    int                                       _staticWeight;

    /*
     * 超时请求的回包回来后，是否打印超时的日志信息
     */
    bool                                   _timeoutLogFlag;

    /*
     * 非发送队列的大小限制，用于发送过载判断
     */
    size_t                                   _noSendQueueLimit;

    /*
     * 模块间调用统计信息的head信息
     */
    StatMicMsgHead                           _statHead;

    /*
     * 模块间调用统计信息的body信息
     */
    map<string,StatMicMsgBody>               _statBody;

    /*
     * 最大采样次数
     */
    uint32_t                               _maxSampleCount;

    /*
     * 采样比率
     */
    int                                    _sampleRate;

    /*
     * 采样信息
     */
    map<string,vector<StatSampleMsg> >     _sample;
};
////////////////////////////////////////////////////////////////////
}
#endif
