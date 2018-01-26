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

#ifndef __TARS_ENDPOINT_MANAGER_H_
#define __TARS_ENDPOINT_MANAGER_H_

#include "util/tc_consistent_hash_new.h"
#include "servant/EndpointInfo.h"
#include "servant/EndpointF.h"
#include "servant/QueryF.h"
#include "servant/AppProtocol.h"

namespace tars
{
////////////////////////////////////////////////////////////////////////
/*
 * 获取路由的方式
 */
enum  GetEndpointType
{
    E_DEFAULT = 0,
    E_ALL     = 1,
    E_SET     = 2,
    E_STATION = 3
};

/*
 * 权重类型
 */
enum  EndpointWeightType
{
    E_LOOP          = 0,
    E_STATIC_WEIGHT = 1,
};

////////////////////////////////////////////////////////////////////////
/*
 * 路由请求与回调的实现类
 */
class QueryEpBase : public QueryFPrxCallback
{
public:
    /*
     * 构造函数
     */
    QueryEpBase(Communicator * pComm, bool bFirstNetThread, bool bInterfaceReq);

    /*
     * 析构函数
     */
    virtual ~QueryEpBase(){}

    /*
     * 初始化
     */
    bool init(const string & sObjName, const string & sLocator, const string& setName = "");

    /*
     * 获取所有节点信息的回调处理
     */
    void callback_findObjectById4All(tars::Int32 ret, const vector<tars::EndpointF>& activeEp, const vector<tars::EndpointF>& inactiveEp);

    /*
     * 获取所有节点信息的异常回调处理
     */
    void callback_findObjectById4All_exception(tars::Int32 ret);

    /*
     * 获取所有节点信息的回调处理
     */
    void callback_findObjectById4Any(tars::Int32 ret, const vector<tars::EndpointF>& activeEp, const vector<tars::EndpointF>& inactiveEp);

    /*
     * 获取所有节点信息的异常回调处理
     */
    void callback_findObjectById4Any_exception(tars::Int32 ret);

    /*
     * 按idc获取的节点信息的回调处理
     */
    void callback_findObjectByIdInSameGroup(tars::Int32 ret, const vector<tars::EndpointF>& activeEp, const vector<tars::EndpointF>& inactiveEp);

    /*
     * 按idc分组获取的节点信息的异常回调处理
     */
    void callback_findObjectByIdInSameGroup_exception(tars::Int32 ret);

    /*
     * 按set获取的节点信息的回调处理
     */
    void callback_findObjectByIdInSameSet(tars::Int32 ret, const vector<tars::EndpointF>& activeEp, const vector<tars::EndpointF>& inactiveEp);

    /*
     * 按set获取的节点信息的异常回调处理
     */
    void callback_findObjectByIdInSameSet_exception(tars::Int32 ret);

    /*
     * 按地区获取的节点信息的回调处理
     */
    void callback_findObjectByIdInSameStation(tars::Int32 ret, const vector<tars::EndpointF>& activeEp, const vector<tars::EndpointF>& inactiveEp);

    /*
     * 按地区获取的节点信息的异常回调处理
     */
    void callback_findObjectByIdInSameStation_exception(tars::Int32 ret);

    /*
     * 从主控请求到数据了 通知更新ip列表信息
     */
    virtual void notifyEndpoints(const set<EndpointInfo> & active,const set<EndpointInfo> & inactive,bool bSync) = 0;

    /*
     * 从主控请求到数据了 最开始调用主控 要通知
     */
    virtual void doNotify() = 0;

    /*
     * 设置主控的代理
     */
    int  setLocatorPrx(QueryFPrx prx);

    /*
     * 是否直连后端
     */
    inline bool getDirectProxy() { return _direct; }

protected:

    /*
     * 刷新主控
     */
    void refreshReg(GetEndpointType type,const string & sName);

private:
    
    /*
     * 设置obj名字
     * 如果是直接连接，则从obj名字中提取ip列表信息
     * 如果是间接连接，则设置主控代理，并从缓存中加载相应的列表
     */
    void setObjName(const string & sObjName);
    
    /*
     * 从sEndpoints提取ip列表信息
     */
    void setEndpoints(const string & sEndpoints, set<EndpointInfo> & setEndpoints);

    /*
     * 主控的请求的响应到了,做相应的处理
     */
    void doEndpoints(const vector<tars::EndpointF>& activeEp, const vector<tars::EndpointF>& inactiveEp, int iRet, bool bSync = false);

    /*
     * 请求主控异常,做相应的处理
     */
    void doEndpointsExp(int iRet);

    /*
     * 刷新ip列表信息到缓存文件
     */
    void setEndPointToCache(bool bInactive);

protected:

    /*
     * 通信器
     */
    Communicator *            _communicator;

    /*
     * 是否第一个客户端网络线程
     * 若是，会对ip列表信息的写缓存
     */
    bool                      _firstNetThread;

    /*
     * 是否主动请求ip列表信息的接口的请求
     * 比如按idc获取某个obj的ip列表信息
     */
    bool                      _interfaceReq;

    /*
     * 是否直连后端服务
     */
    bool                      _direct;

    /*
     * 请求的后端服务的Obj对象名称
     */
    string                    _objName;

    /*
     * 指定set调用的setid,默认为空
     * 如果有值，则说明是指定set调用
     */
    string                    _invokeSetId;

    /*
     * 框架的主控地址
     */
    string                    _locator;

    /*
     * 主控的路由代理
     */
    QueryFPrx                 _queryFPrx;

    /*
     * 数据是否有效,初始化的时候是无效的数据
     * 只有请求过主控或者从文件缓存加载的数据才是有效数据
     */
    bool                      _valid;

    /*
     * 权重类型
     */
    EndpointWeightType        _weightType;
    
    /*
     * 活跃节点列表
     */
    set<EndpointInfo>         _activeEndpoints;

    /*
     * 不活跃节点列表
     */
    set<EndpointInfo>         _inactiveEndpoints;

    
private:

    /////////以下是请求主控的策略信息/////////////////

    /*
     * 是否正在向请求主控服务的ip列表信息
     */
    bool                      _requestRegistry;

    /*
     * 请求主控的超时时间(绝对时间)，单位毫秒
     * 防止请求超时或者失败，一直处理请求状态
     */
    int64_t                   _requestTimeout;

    /*
     * 请求主控的超时间隔，默认5s
     */
    int                       _timeoutInterval;

    /*
     * 下次请求主控的时间(绝对时间),单位毫秒
     */
    int64_t                   _refreshTime;

    /*
     * 正常请求主控的频率(有ip列表信息)，单位毫秒
     * 默认60s一次
     */
    int                       _refreshInterval;

    /*
     * 主控返回的活跃ip列表为空情况下
     * 请求主控的频率,默认10s一次
     */
    int                       _activeEmptyInterval;

    /*
     * 请求主控失败的时间频率
     * 默认2s一次
     */
    int                       _failInterval;

    /*
     * 请求主控连续失败达到一定的次数后
     * 请求主控的时间频率，默认30s一次
     */
    int                       _manyFailInterval;

    /*
     * 连续请求失败的次数限制，默认3次
     */
    int                       _failTimesLimit;

    /*
     * 连续失败的次数
     */
    int                       _failTimes;

    
};

////////////////////////////////////////////////////////////////////////
/*
 * 框架内部的路由管理的实现类
 */
class EndpointManager : public QueryEpBase
{
public:
    static const size_t iMinWeightLimit = 10;
    static const size_t iMaxWeightLimit = 100;

public:
    /*
     * 构造函数
     */
    EndpointManager(ObjectProxy* pObjectProxy, Communicator* pComm, const string & sObjName, bool bFirstNetThread, const string& setName = "");

    /*
     * 析构函数
     */
    virtual ~EndpointManager();

    /*
     * 重写基类的实现
     */
    void notifyEndpoints(const set<EndpointInfo> & active, const set<EndpointInfo> & inactive, bool bSync = false);

    /*
     * 重写基类的实现
     */
    void doNotify();

    /**
     * 根据请求策略从可用的服务列表选择一个服务节点
     */
    bool selectAdapterProxy(ReqMessage * msg, AdapterProxy * & pAdapterProxy);

    /**
     * 获取所有的服务节点
     */
    const vector<AdapterProxy*> & getAdapters()
    {
        return _vAllProxys;
    }

private:

    /*
     * 轮询选取一个结点
     */
    AdapterProxy * getNextValidProxy();

    /*
     * 根据hash值选取一个结点
     */
    AdapterProxy* getHashProxy(int64_t hashCode, bool bConsistentHash = false);

    /*
     * 根据hash值按取模方式，从正常节点中选取一个结点
     */
    AdapterProxy* getHashProxyForNormal(int64_t hashCode);

    /*
     * 根据hash值按一致性hash方式，从正常节点中选取一个结点
     */
    AdapterProxy* getConHashProxyForNormal(int64_t hashCode);

    /*
     * 根据hash值按取模方式，从静态权重节点中选取一个结点
     */
    AdapterProxy* getHashProxyForWeight(int64_t hashCode, bool bStatic, vector<size_t> &vRouterCache);

    /*
     * 根据hash值按一致性hash方式，从静态权重节点中选取一个结点
     */
    AdapterProxy* getConHashProxyForWeight(int64_t hashCode, bool bStatic);

    /*
     * 判断静态权重节点是否有变化
     */
    bool checkHashStaticWeightChange(bool bStatic);

    /*
     * 判断静态权重节点是否有变化
     */
    bool checkConHashChange(bool bStatic, const vector<AdapterProxy*> &vLastConHashProxys);

    /*
     * 更新取模hash方法的静态权重节点信息
     */
    void updateHashProxyWeighted(bool bStatic);

    /*
     * 更新一致性hash方法的静态权重节点信息
     */
    void updateConHashProxyWeighted(bool bStatic, vector<AdapterProxy*> &vLastConHashProxys, TC_ConsistentHashNew &conHash);

    /*
     * 根据后端服务的权重值选取一个结点
     */
    AdapterProxy* getWeightedProxy(bool bStaticWeighted);

    /*
     * 根据后端服务的权重值选取一个结点
     */
    AdapterProxy* getWeightedForNormal(bool bStaticWeighted);

    /*
     * 根据各个节点的权重值，建立各个节点的调用数
     */
    void updateProxyWeighted();

    /*
     * 根据各个节点的静态权重值，建立各个节点的静态权重
     */
    void updateStaticWeighted();

    /*
     * 建立静态权重节点信息的缓存
     */
    void dispatchEndpointCache(const vector<int> &vWeight);

private:

    /*
     * ObjectProxy
     */
    ObjectProxy *                 _objectProxy;

    /*
     * 活跃的结点
     */
    vector<AdapterProxy*>         _activeProxys;
    
    /*
     * 部署的结点 包括活跃的和不活跃的
     */
    map<string,AdapterProxy*>     _regProxys;
    vector<AdapterProxy*>         _vRegProxys;

    /*
     * 所有曾经create的结点
     */
    map<string,AdapterProxy*>     _allProxys;
    vector<AdapterProxy*>         _vAllProxys;

    /*
     * 轮训访问_activeProxys的偏移
     */
    size_t                        _lastRoundPosition;

    /*
     * 节点信息是否有更新
     */
    bool                          _update;

    /*
     * 是否是第一次建立权重信息
     */
    bool                          _first;

    /**
     * 上次重新建立权重信息表的时间
     */
    int64_t                          _lastBuildWeightTime;

    /**
     * 负载值更新频率,单位毫秒
     */
    int32_t                          _updateWeightInterval;

    /**
     * 静态时，对应的节点路由选择
     */
    size_t                          _lastSWeightPosition;

    /**
     * 静态权重对应的节点路由缓存
     */
    vector<size_t>                  _staticRouterCache;

    /*
     * 静态权重的活跃节点
     */
    vector<AdapterProxy*>          _activeWeightProxy;

    /*
     * hash静态权重的路由缓存
     */
    vector<size_t>                  _hashStaticRouterCache;

    /*
     * hash静态权重的缓存节点
     */
    vector<AdapterProxy*>         _lastHashStaticProxys;

    /*
     * 一致性hash静态权重时使用
     */
    vector<AdapterProxy*>         _lastConHashWeightProxys;

    /*
     * 一致性hash静态权重时使用
     */
    TC_ConsistentHashNew          _consistentHashWeight;

    /*
     * 一致性hash普通使用
     */
    vector<AdapterProxy*>         _lastConHashProxys;

    /*
     * 一致性hash普通使用
     */
    TC_ConsistentHashNew          _consistentHash;
};

////////////////////////////////////////////////////////////////////////
/*
 * 对外按类型获取路由的实现类
 */
class EndpointThread : public QueryEpBase
{
public:
    /*
     * 构造函数
     */
    EndpointThread(Communicator* pComm, const string & sObjName, GetEndpointType type, const string & sSetName, bool bFirstNetThread = false);

    /*
     * 析构函数
     */
    ~EndpointThread(){};

    /*
     * 用EndpointInfo存在可用与不可用的节点信息
     */
    void getEndpoints(vector<EndpointInfo> &activeEndPoint, vector<EndpointInfo> &inactiveEndPoint);

    /*
     * 用TC_Endpoint存在可用与不可用的节点信息
     */
    void getTCEndpoints(vector<TC_Endpoint> &activeEndPoint, vector<TC_Endpoint> &inactiveEndPoint);

    /*
     * 重写基类的实现
     */
    void notifyEndpoints(const set<EndpointInfo> & active, const set<EndpointInfo> & inactive, bool bSync);

    /*
     * 重写基类的实现
     */
    void doNotify()
    {
    }

private:

    /*
     * 更新缓存的ip列表信息
     */
    void update(const set<EndpointInfo> & active, const set<EndpointInfo> & inactive);

private:

    /*
     * 类型
     */
    GetEndpointType          _type;

    /*
     * Obj名称
     */
    string                   _name;

    /*
     * 锁
     */
    TC_ThreadLock            _lock;


    /*
     * 活跃的结点
     */
    vector<EndpointInfo>     _activeEndPoint;
    vector<TC_Endpoint>      _activeTCEndPoint;

    /*
     * 不活跃的结点
     */
    vector<EndpointInfo>     _inactiveEndPoint;
    vector<TC_Endpoint>      _inactiveTCEndPoint;
    
};

////////////////////////////////////////////////////////////////////////
/*
 * 对外获取路由请求的封装类
 */
class EndpointManagerThread
{
public:
    /*
     * 构造函数
     */
    EndpointManagerThread(Communicator *pComm, const string &sObjName);

    /*
     * 析构函数
     */
    ~EndpointManagerThread();

    /*
     * 按idc获取可用与不可用的结点
     */
    void getEndpoint(vector<EndpointInfo> &activeEndPoint, vector<EndpointInfo> &inactiveEndPoint);

    /*
     * 获取所有可用与不可用的结点
     */
    void getEndpointByAll(vector<EndpointInfo> &activeEndPoint, vector<EndpointInfo> &inactiveEndPoint);

    /*
     * 根据set获取可用与不可用的结点
     */
    void getEndpointBySet(const string sName, vector<EndpointInfo> &activeEndPoint, vector<EndpointInfo> &inactiveEndPoint);

    /*
     * 根据地区获取可用与不可用的结点
     */
    void getEndpointByStation(const string sName, vector<EndpointInfo> &activeEndPoint, vector<EndpointInfo> &inactiveEndPoint);

    /*
     * 按idc获取可用与不可用的结点
     */
    void getTCEndpoint( vector<TC_Endpoint> &activeEndPoint, vector<TC_Endpoint> &inactiveEndPoint);

    /*
     * 获取所有可用与不可用的结点
     */
    void getTCEndpointByAll(vector<TC_Endpoint> &activeEndPoint, vector<TC_Endpoint> &inactiveEndPoint);

    /*
     * 根据set获取可用与不可用的结点
     */
    void getTCEndpointBySet(const string sName, vector<TC_Endpoint> &activeEndPoint, vector<TC_Endpoint> &inactiveEndPoint);

    /*
     * 根据地区获取可用与不可用的结点
     */
    void getTCEndpointByStation(const string sName, vector<TC_Endpoint> &activeEndPoint, vector<TC_Endpoint> &inactiveEndPoint);

protected:

    /*
     * 根据type创建相应的EndpointThread
     */
    EndpointThread * getEndpointThread(GetEndpointType type, const string & sName);

private:
    /*
     * 通信器
     */
    Communicator *                 _communicator;

    /*
     * Obj名称
     */
    string                         _objName;

    /*
     * 锁
     */
    TC_ThreadLock                  _lock;

    /*
     * 保存对象的map
     */
    map<string,EndpointThread*>    _info;
};

////////////////////////////////////////////////////////////////////////
}
#endif
