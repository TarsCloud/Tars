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

#ifndef __TARS__COMMUNICATOR_EPOLL_H_
#define __TARS__COMMUNICATOR_EPOLL_H_

#include "util/tc_thread.h"
#include "util/tc_thread_mutex.h"
#include "util/tc_epoller.h"
#include "util/tc_loop_queue.h"
#include "servant/Message.h"
#include <set>

namespace tars
{

class Communicator;
class ObjectProxy;
class ObjectProxyFactory;
class StatReport;
class PropertyReport;

////////////////////////////////////////////////////////////////////////
/**
 * 监听FD事件并触发注册的handle
 */
struct FDInfo
{
    enum
    {
        ET_C_NOTIFY = 1,
        ET_C_NET    = 2,
    };

    /**
     * 构造函数
     */
    FDInfo()
    : iSeq(0)
    , fd(-1)
    , iType(ET_C_NOTIFY)
    , p(NULL)
    {
    }

    /**
     * 析构函数
     */
    ~FDInfo()
    {
    }

    size_t iSeq;

    int    fd;

    int    iType;

    void * p;
};

////////////////////////////////////////////////////////////////////////
/**
 * 客户端网络处理的线程类
 */
class CommunicatorEpoll : public TC_Thread ,public TC_ThreadRecMutex
{
public:
    struct NotifyInfo
    {
        /**
         * 构造函数
         */
        NotifyInfo()
        : eventFd(-1)
        , bValid(false)
        {
        }

        /**
         * 析构函数
         */
        ~NotifyInfo()
        {
        }

        
        FDInfo    stFDInfo;   //通知FD信息
        
        TC_Socket notify;     //通知fd

        int       eventFd;    //eventfd,目前未使用
        
        bool      bValid;     //是否有效
    };

    /**
     * 构造函数
     */
    CommunicatorEpoll(Communicator * pCommunicator, size_t _netThreadSeq);

    /**
     * 析构函数
     */
    virtual ~CommunicatorEpoll();

    /**
     * 获取 Communicator
     */
    inline Communicator * getCommunicator()
    {
        return _communicator;
    }

    /**
     * 获取 ObjectProxyFactory
     */
    inline ObjectProxyFactory * getObjectProxyFactory()
    {
        return _objectProxyFactory;
    }

    /**
     * 获取 网络线程id
     */
    inline size_t getCommunicatorEpollId()
    {
        return _netThreadSeq;
    }

    /**
     * 获取 节点队列未发送请求的大小限制
     */
    inline size_t getNoSendQueueLimit()
    {
        return _noSendQueueLimit;
    }

    /*
     * 判断是否是第一个网络线程 主控写缓存的时候用到
     */
    inline bool isFirstNetThread()
    {
        return (_netThreadSeq == 0);
    }

    /*
     * 获取本epoll的代理对象
     */
    ObjectProxy * getObjectProxy(const string & sObjectProxyName,const string& setName="");

    /**
     * 循环监听网络事件
     */
    virtual void run();

    /**
     * 中止监听
     */
    void terminate();

    /**
     * 注册fd对应的处理handle
     * @param fd
     * @param info 事件指针
     * @param event
     * @param handle
     */
    void addFd(int fd,FDInfo * info, uint32_t events);

    /**
     * 取消已注册的handle
     * @param fd
     * @param info 事件指针
     * @param event
     * @param handle
     */
    void delFd(int fd,FDInfo * info, uint32_t events);

    /**
     * 通知事件过来
     * @param fd
     */
    void notify(size_t iSeq,ReqInfoQueue * pReqQueue);

    /**
     * 通知删除事件过来
     * @param iSeq
     */
    void notifyDel(size_t iSeq);

    /**
     * 数据加入到异步线程队列里面
     * @return
     */
    void pushAsyncThreadQueue(ReqMessage * msg);

protected:
    /**
     * 处理函数
     *
     * @param pFDInfo
     * @param events
     */
    void handle(FDInfo * pFDInfo, uint32_t events);

    /**
     * 输入事件
     * @param pi
     */
    void handleInputImp(Transceiver * pTransceiver);

    /**
     * 输出事件
     * @param pi
     */
    void handleOutputImp(Transceiver * pTransceiver);

    /**
     * 处理超时
     * @param pi
     */
    void doTimeout();

    /**
     * 处理stat
     * @param pi
     */
    void doStat();

protected:
    /*
     * 通信器
     */
    Communicator *         _communicator;

    /*
     * 请求事件通知数组
     */
    NotifyInfo             _notify[MAX_CLIENT_NOTIFYEVENT_NUM];

    /*
     * 关闭线程请求的事件通知
     */
    TC_Socket              _shutdown;

    /*
     * 线程是否终止
     */
    bool                   _terminate;

    /*
     * epoll
     */
    TC_Epoller             _ep;

    /*
     * 下次检查超时的时间
     */
    int64_t                _nextTime;

    /*
     * 下次上报数据的时间
     */
    int64_t                _nextStatTime;

    /*
     * ObjectProxy的工厂类
     */
    ObjectProxyFactory *   _objectProxyFactory;

    /*
     * 异步线程数组
     */
    AsyncProcThread *      _asyncThread[MAX_CLIENT_ASYNCTHREAD_NUM];

    /*
     * 异步线程数目
     */
    size_t                 _asyncThreadNum;

    /*
     * 分发给异步线程的索引seq
     */
    size_t                 _asyncSeq;

    /*
     * 网络线程的id号
     */
    size_t                 _netThreadSeq;

    /*
     * 异步队列的统计上报的对象
     */
    PropertyReport *       _reportAsyncQueue;

    /*
     * 节点ip队列未发送请求的大小限制
     */
    size_t                 _noSendQueueLimit;

    /*
     * epoll wait的超时时间
     */
    int64_t                _waitTimeout;

    /*
     * 超时的检查时间间隔
     */
    int64_t                _timeoutCheckInterval;
};

/////////////////////////////////////////////////////////////////////////////////////
}

#endif
