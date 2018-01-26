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

#ifndef __TARS_SERVANT_H_
#define __TARS_SERVANT_H_

#include "util/tc_autoptr.h"
#include "util/tc_epoll_server.h"
#include "servant/ServantProxy.h"
#include "servant/TarsCurrent.h"
#include "servant/BaseNotify.h"

namespace tars
{

class BaseNotify;
////////////////////////////////////////////////////////////////////
/**
 * 每个对象的基类
 */
class Servant : public BaseNotify
{
public:
    /**
     * 构造函数
     */
    Servant();

    /**
     * 析构函数
     */
    ~Servant();

    /**
     * 设置名称
     * @param name
     */
    void setName(const string &name);

    /**
     * 名称
     * @return string
     */
    string getName() const;

    /**
     * 设置所属的Handle
     * @param handle 
     */
    void setHandle(TC_EpollServer::Handle* handle);

    /**
     * 获取所属的Handle
     * @return HandlePtr& 
     */
    TC_EpollServer::Handle* getHandle();

    /**
     * 初始化
     * @return int
     */
    virtual void initialize() = 0;

    /**
     * 退出
     */
    virtual void destroy() = 0;

public:
    /**
     * 分发收到的请求
     * @param sRecv
     * @param sFuncName
     * @param sSend
     * @return int
     */
    virtual int dispatch(TarsCurrentPtr current, vector<char> &buffer);

    /**
     * tars协议，分发并处理请求
     * @param sRecv
     * @param sFuncName
     * @param sSend
     * @return int
     */
    virtual int onDispatch(TarsCurrentPtr current, vector<char> &buffer) { return -1; }

public:
    /**
     * 普通协议的请求，没有方法名，不需要Dispatch
     * @param current 
     * @param buffer 
     * @return int 
     */
    virtual int doRequest(TarsCurrentPtr current, vector<char> &buffer) { return -1; }

    /**
     * 作为客户端访问其他server时，成功返回的响应接口
     * @param resp 
     * @return int 
     */
    virtual int doResponse(ReqMessagePtr resp) { return -1; }

    /**
     * 作为客户端访问其他server时，返回其他异常的响应接口
     * @param resp 
     * @return int 
     */
    virtual int doResponseException(ReqMessagePtr resp) { return -1; }

    /**
     * 作为客户端访问其他server时，如果resp没有找到request,则响应该接口
     * 例如:push消息或者超时后服务端迟到的响应
     * @param resp 
     * @return int 
     */
    virtual int doResponseNoRequest(ReqMessagePtr resp) { return -1; }

    /**
     * 每次handle被唤醒后都会调用，业务可以通过在其他线程中调用handle的notify
     * 实现在主线程中处理自有数据的功能，比如定时器任务或自有网络的异步响应;
     * [一般都需要配合业务自有的队列使用，队列可以封装在ServantImp对象中] 
     *  
     * 关于参数bExpectIdle： 
     * 一般业务不用关注该值，可忽略。 
     *  
     * bExpectIdle为true时，在循环Adapter时调用的 
     * bExpectIdle为false时，在处理Adapter上的请求时调用的 
     *
     * doCustomMessage() 不带参数的是为了兼容老版本。尽量用带参数的函数
     * 现在不带参数和带参数的都会调用
     */
    virtual int doCustomMessage(bool bExpectIdle) { return -1; }
    virtual int doCustomMessage() { return -1; }


     /**
     * 客户端关闭连接时的处理
     * @param current 
     * @return int  
     */
    virtual int doClose(TarsCurrentPtr current){ return -1; }

    /**
     * 获得响应的数据队列
     * @return TC_ThreadQueue<ReqMessagePtr>& 
     */
    TC_ThreadQueue<ReqMessagePtr>& getResponseQueue();

protected:
    /**
     * 名字
     */
    string _name;

    /**
     * 所属的Handle
     */
    TC_EpollServer::Handle* _handle;

    /**
     * 异步响应队列
     */
    TC_ThreadQueue<ReqMessagePtr> _asyncResponseQueue;
};

typedef TC_AutoPtr<Servant> ServantPtr;

//////////////////////////////////////////////////////////////////////
/**
 * 单线程全异步中的callback对象，业务自有的callback需要从这里继承
 */
class ServantCallback : public ServantProxyCallback
{
public:
    /**
     * 构造函数，type用来区分同一链路上的多种cb对象类型
     * @param type 
     * @param servant 
     * @param current 
     */
    ServantCallback(const string& type, const ServantPtr& servant, const TarsCurrentPtr& current);

    /**
     * callback的响应接口 
     * @param msg 
     * @return int 
     */
    virtual int onDispatch(ReqMessagePtr msg);

    /**
     * 获得生成时所属的servant
     * @return const ServantPtr& 
     */
    const ServantPtr& getServant();

    /**
     * 获得网络上下文
     * @return const TarsCurrentPtr& 
     */
    const TarsCurrentPtr& getCurrent();

protected:
    /*
     * Servant
     */
    ServantPtr _servant;

    /*
     * TarsCurrent
     */
    TarsCurrentPtr _current;
};

//////////////////////////////////////////////////////////////////////

//线程私有数据
class CallbackThreadData : public TC_ThreadPool::ThreadData
{
public:
    static TC_ThreadMutex _mutex;  //全局的互斥锁
    static pthread_key_t _key;   //私有线程数据key

    /**
     * 构造函数
     */
    CallbackThreadData();

    /**
     * 析构函数
     */
    ~CallbackThreadData() {}

    /**
     * 数据资源释放
     * @param p
     */
    static void destructor(void* p);

    /**
     * 获取线程数据，没有的话会自动创建
     * @return CallbackThreadData*
     */
    static CallbackThreadData * getData();

public:
    /**
     * 设置返回的额外内容
     * @param context
     */
    void setResponseContext(const map<std::string, std::string> & context);

    /**
     * 获取返回的额外内容是否有效
     * @param context
     */
    bool getContextValid(){return _contextValid;}

    /**
     * 获取返回的额外内容
     * @param context
     */
    map<std::string, std::string> & getResponseContext();

    /**
     * 清除返回的额外内容是否有效
     * @param context
     */
    void delResponseContext();

private:
    /*
     * 返回的context是否合法
     */
    bool                            _contextValid;

    /*
     * 返回的context
     */
    map<std::string, std::string>    _responseContext;
};

//////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////
#endif
