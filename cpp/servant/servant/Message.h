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

#ifndef __TARS_MESSAGE_H_
#define __TARS_MESSAGE_H_

#include "servant/Global.h"
#include "util/tc_autoptr.h"
#include "util/tc_monitor.h"
#include "util/tc_loop_queue.h"
#include "servant/CoroutineScheduler.h"

namespace tars
{

/////////////////////////////////////////////////////////////////////////
/**
 * 超时一定比率后进行节点屏蔽
 * 设置超时检查参数
 * 计算到某台服务器的超时情况, 如果连续超时次数或者超时比例超过阀值
 * 默认60s内, 超时调用次数>=2, 超时比率0.5，或者5s内连续超时次数>5
 * 或者连接异常连续超过5次
 * 则失效，进行屏蔽
 * 服务屏蔽后, 请求将尽可能的切换到其他可能的服务器, 并每隔tryTimeInterval尝试一次, 如果成功则认为恢复
 * 如果其他服务器都屏蔽, 则随机选择一台尝试
 */
struct CheckTimeoutInfo
{
    /*
     * 构造函数
     */
    CheckTimeoutInfo() 
    : minTimeoutInvoke(2)
    , checkTimeoutInterval(60)
    , frequenceFailInvoke(5)
    , minFrequenceFailTime(5)
    , radio(0.5)
    , tryTimeInterval(30)
    , maxConnectExc(5)
    {
    }

    /*
     * 判断超时屏蔽时，计算的最小的超时次数
     */
    uint16_t minTimeoutInvoke;

    /*
     * 判断超时屏蔽时，时间间隔
     */
    uint32_t checkTimeoutInterval;       

    /*
     * 判断因连续失败而进行屏蔽时，连续失败的次数
     */
    uint32_t frequenceFailInvoke;

    /*
     * 判断因连续失败而进行屏蔽时，最小的时间间隔
     */
    uint32_t minFrequenceFailTime;

    /*
     * 超时的比率
     */
    float    radio;

    /*
     * 重试间隔
     */
    uint32_t tryTimeInterval;

    /*
     * 最大连接异常次数
     */
    uint32_t maxConnectExc;
};

/////////////////////////////////////////////////////////////////////////
/*
 * stat采样信息(用于调用链时序分析)
 */
struct SampleKey
{
    /*
     * 构造函数
     */
    SampleKey()
    : _root(true)
    , _unid("")
    , _depth(0)
    , _width(0)
    , _parentWidth(0)
    {}

    /*
     * 初始化
     */
    void init()
    {
        _root        = true;
        _unid.clear();
        _depth       = 0;
        _width       = 0;
        _parentWidth = 0;
    }

    /*
     * 是否根节点 在根节点产生唯一id
     */
    bool _root;

    /*
     * 唯一id
     */
    string _unid;

    /*
     * 深度
     */
    int _depth;

    /*
     * 广度
     */
    int _width;

    /*
     * 父节点广度值
     */
    int _parentWidth;
};

/////////////////////////////////////////////////////////////////////////
/**
 * 用于同步调用时的条件变量
 */
struct ReqMonitor : public TC_ThreadLock
{
};

/////////////////////////////////////////////////////////////////////////

#define IS_MSG_TYPE(m, t) ((m & t) != 0)
#define SET_MSG_TYPE(m, t) do { (m |= t); } while (0);
#define CLR_MSG_TYPE(m, t) do { (m &=~t); } while (0);

struct ReqMessage : public TC_HandleBase
{
    //调用类型
    enum CallType
    {
        SYNC_CALL = 1, //同步
        ASYNC_CALL,    //异步
        ONE_WAY,       //单向
        THREAD_EXIT    //线程退出的标识
    };

    //请求状态
    enum ReqStatus
    {
        REQ_REQ = 0,    //状态正常,正在请求中
        REQ_RSP,        //请求已经发出去
        REQ_TIME,       //请求超时
        REQ_BUSY,       //请求队列满了
        REQ_EXC         //客户端请求异常
    };

    /*
     * 构造函数
     */
    ReqMessage()
    : eStatus(ReqMessage::REQ_REQ)
    , eType(SYNC_CALL)
    , bFromRpc(false)
    , callback(NULL)
    , proxy(NULL)
    , pObjectProxy(NULL)
    , pMonitor(NULL)
    , bMonitorFin(false)
    , iBeginTime(0)
    , iEndTime(0)
    , bHash(false)
    , bConHash(false)
    , iHashCode(0)
    , adapter(NULL)
    , bDyeing(false)
    , bPush(false)
    , bCoroFlag(false)
    , sched(NULL)
    , iCoroId(0)
    {
    }

    /*
     * 析构函数
     */
    ~ReqMessage()
    {
        if(pMonitor != NULL)
        {
            delete pMonitor;
            pMonitor = NULL;
        }
    }

    /*
     * 初始化
     */
    void init(CallType eCallType, ObjectProxy * pObj, const string & sFuncName)
    {
        eStatus        = ReqMessage::REQ_REQ;
        eType          = eCallType;
        bFromRpc       = false;

        callback       = NULL;
        proxy          = NULL;
        pObjectProxy   = pObj;

        sReqData.clear();
        pMonitor       = NULL;
        bMonitorFin    = false;

        iBeginTime     = 0;
        iEndTime       = 0;
        bHash          = false;
        bConHash       = false;
        iHashCode      = 0;
        adapter        = NULL;

        bDyeing        = false;

        bPush          = false;

        bCoroFlag      = false;
        sched          = NULL;
        iCoroId        = 0;
    }


    ReqStatus                   eStatus;        //调用的状态
    CallType                    eType;          //调用类型
    bool                        bFromRpc;       //是否是第三方协议的rcp_call，缺省为false

    ServantProxyCallbackPtr     callback;       //异步调用时的回调对象

    ServantProxy *              proxy;          //调用的ServantProxy对象
    ObjectProxy *               pObjectProxy;   //调用端的proxy对象

    RequestPacket               request;        //请求消息体
    ResponsePacket              response;       //响应消息体

    string                      sReqData;       //请求消息体

    ReqMonitor *                pMonitor;        //用于同步的monitor
    bool                        bMonitorFin;    //同步请求timewait是否结束

    int64_t                     iBeginTime;     //请求时间
    int64_t                     iEndTime;       //完成时间

    bool                        bHash;          //是否hash调用
    bool                        bConHash;       //是否一致性hash调用
    int64_t                     iHashCode;      //hash值，用户保证一个用户的请求发送到同一个server(不严格保证)

    AdapterProxy *              adapter;        //调用的adapter

    bool                        bDyeing;        //是否需要染色
    string                      sDyeingKey;     //染色key

    bool                        bPush;          //push back 消息

    bool                        bCoroFlag;      //是否是协程里发出的请求
    CoroutineScheduler*            sched;          //协程调度器
    uint32_t                    iCoroId;        //协程的id

    SampleKey                   sampleKey;      //采样信息

};

typedef TC_AutoPtr<ReqMessage>          ReqMessagePtr;
typedef TC_LoopQueue<ReqMessage*,1000>  ReqInfoQueue;
    
#define HTTP2 "http2"

}

#endif
