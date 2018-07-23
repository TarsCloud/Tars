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

#ifndef __AMIN_REGISTRY_H__
#define __AMIN_REGISTRY_H__

#include "AdminReg.h"
#include "Patch.h"
#include "EndpointF.h"
#include "DbProxy.h"

using namespace tars;

/**
 * 管理控制接口类
 */
class AdminRegistryImp: public AdminReg
{
public:
    /**
     * 构造函数
     */
    AdminRegistryImp(){};

    /**
     * 初始化
     */
    virtual void initialize();

    /**
     ** 退出
     */
    virtual void destroy() {};

public:

    /**
     * 
     * @param taskList 
     * 
     * @return string, TaskNo
     */
   virtual int addTaskReq(const TaskReq &taskReq, tars::TarsCurrentPtr current);

    /**
     * 获取任务状态
     *
     * @param taskNo : 任务列表id
     *
     * @return 任务状态
     */
    virtual int getTaskRsp(const string &taskNo, TaskRsp &taskRsp, tars::TarsCurrentPtr current);

    /**
     * 获取TaskRsp信息
     * 
     * @param application 
     * @param serverName 
     * @param command 
     * @param current 
     * 
     * @return vector<TaskRsp> 
     */
    virtual int getTaskHistory(const string & application, const string & serverName, const string & command, vector<TaskRsp> &taskRsp, tars::TarsCurrentPtr current);

    /**
     * 设置任务状态
     * 
     * @param itemNo 
     * @param startTime 
     * @param endTime 
     * @param status 
     * @param log 
     * @param current 
     * 
     * @return int 
     */
    virtual int setTaskItemInfo(const string & itemNo, const map<string, string> &info, tars::TarsCurrentPtr current);

    /***********application****************/
    /**
     * 卸载服务
     * 
     * @param application 
     * @param serverName 
     * @param nodeName 
     * @param current 
     * 
     * @return int 
     */
    virtual int undeploy(const string & application, const string & serverName, const string & nodeName, const string &user, string &log, tars::TarsCurrentPtr current);

    /**
     * 获取application列表
     *
     * @param null
     * @param out result : 结果描述
     *
     * @return application列表
     */
    virtual vector<string> getAllApplicationNames(string &result, tars::TarsCurrentPtr current);


    /***********node****************/

    /**
     * 获取node列表
     *
     * @param null
     * @param out result : 结果描述
     *
     * @return node 列表
     */
    virtual vector<string> getAllNodeNames(string &result, tars::TarsCurrentPtr current);

    /**
     * 获取node版本
     * @param name   node名称
     * @param version   node版本
     * @param out result 结果描述
     * @return  0-成功 others-失败
     */
    virtual int getNodeVesion(const string &nodeName, string &version, string & result, tars::TarsCurrentPtr current);

    /**
     * ping node
     *
     * @param name: node id
     * @param out result : 结果描述
     *
     * @return : true-ping通；false-不通
     */
    virtual bool pingNode(const string & name, string &result, tars::TarsCurrentPtr current);

    /**
     * 停止 node
     *
     * @param name: node id
     * @param out result : 结果描述
     *
     * @return : 0-成功 others-失败
     */
    virtual int shutdownNode(const string & name, string &result, tars::TarsCurrentPtr current);

    /**
     * 获取server列表
     *
     * @param name: null
     * @param out result : 结果描述
     *
     * @return: server列表及相关信息
     */
    virtual vector<vector<string> > getAllServerIds(string &result, tars::TarsCurrentPtr current);

    /**
     * 获取特定server状态
     *
     * @param application: 应用
     * @param serverName : server名
     * @param nodeNmae   : node id
     * @param out state  : 状态
     * @param out result : 结果描述
     *
     * @return : 处理结果
     */
    virtual int getServerState(const string & application, const string & serverName, const string & nodeName, ServerStateDesc &state, string &result, tars::TarsCurrentPtr current);

     /**
     * 获取特定ip所属group
     *
     * @param sting: ip
     * @param out int  : group id
     * @param out result : 结果描述
     *
     * @return : 处理结果
     */

    virtual int getGroupId(const string & ip,int &groupId, string &result, tars::TarsCurrentPtr current);

    /**
     * 启动特定server
     *
     * @param application: 应用
     * @param serverName : server名
     * @param nodeName   : node id
     * @param out result : 结果描述
     *
     * @return : 0-成功 others-失败
     */
    virtual int startServer(const string & application, const string & serverName, const string & nodeName, string &result, tars::TarsCurrentPtr current);

    /**
     * 停止特定server
     *
     * @param application: 应用
     * @param serverName : server名
     * @param nodeName   : node id
     * @param out result : 结果描述
     *
     * @return : 0-成功 others-失败
     */
    virtual int stopServer(const string & application, const string & serverName, const string & nodeName, string &result, tars::TarsCurrentPtr current);

    /**
     * 重启特定server
     *
     * @param application: 应用
     * @param serverName : server名
     * @param nodeName   : node id
     * @param out result : 结果描述
     *
     * @return : 0-成功 others-失败
     */
    virtual int restartServer(const string & application, const string & serverName, const string & nodeName, string &result, tars::TarsCurrentPtr current);

    /**
     * 通知服务
     * @param application
     * @param serverName
     * @param nodeName
     * @param command
     * @param result
     * @param current
     *
     * @return int
     */
    virtual int notifyServer(const string & application, const string & serverName, const string & nodeName, const string &command, string &result, tars::TarsCurrentPtr current);

    /**
     * 批量发布
     *
     * @param PatchRequest : 发布请求
     * @param out result   : 结果描述
     *
     * @return : 0-成功 others-失败
     */
    virtual int batchPatch(const tars::PatchRequest & req, string &result, tars::TarsCurrentPtr current);

    /**
     * 发布成功
     * 
     * @param req 
     * @param result 
     * @param current 
     * 
     * @return int 
     */
    virtual int updatePatchLog(const string &application, const string & serverName, const string & nodeName, const string & patchId, const string & user, const string &patchType, bool succ, tars::TarsCurrentPtr current);

    /**
    * 获取服务发布进度
    * @param application  服务所属应用名
    * @param serverName  服务名
    * @param nodeName   :node id
    * @out tPatchInfo  :发布百分比
    * @return :0-成功 others-失败
    */
    virtual int getPatchPercent(const string &application, const string &serverName,const string & nodeName, PatchInfo &tPatchInfo, tars::TarsCurrentPtr current);

    /**
     * 加载特定server
     *
     * @param application: 应用
     * @param serverName : server名
     * @param nodeName   : node id
     * @param out result : 结果描述
     *
     * @return : 0-成功 others-失败
     */
    virtual int loadServer(const string & application, const string & serverName, const string & nodeName, string &result, tars::TarsCurrentPtr current);

    /**
     * 获取相应模板
     *
     * @param profileName: 模板名称
     * @param out profileTemplate: 模板内容
     * @param out resultDesc: 结果描述
     *
     * @return : 0-成功 others-失败
     */
    virtual int getProfileTemplate(const std::string & profileName,std::string &profileTemplate, std::string & resultDesc, tars::TarsCurrentPtr current);

    /**
     * 获取务服相应模板
     *
     * @param application: 应用
     * @param serverName : server名
     * @param nodeName   : node id
     * @param out profileTemplate: 模板内容
     * @param out resultDesc: 结果描述
     *
     * @return : 0-成功 others-失败
     */
    virtual int getServerProfileTemplate(const string & application, const string & serverName, const string & nodeName,std::string &profileTemplate, std::string & resultDesc, tars::TarsCurrentPtr current);

    /**
     * node通过接口获取连接上主控的node ip
     * @param sNodeIp:  node 的ip
     *
     * @return 0-成功 others-失败
     */
    virtual int getClientIp(std::string &sClientIp,tars::TarsCurrentPtr current);

    virtual int gridPatchServer(const vector<ServerGridDesc> &gridDescList, vector<ServerGridDesc> &gridFailDescList, std::string & resultDesc, tars::TarsCurrentPtr current);
protected:

    //数据库操作类对象
    DbProxy _db;

    PatchPrx _patchPrx;
};

class PatchProCallbackImp: public NodePrxCallback
{
public:
    PatchProCallbackImp(const tars::PatchRequest& req, const NodePrx& nodePrx, int defaultTime, tars::TarsCurrentPtr current)
    : _reqPro(req)
    , _nodePrx(nodePrx)
    , _defaultTime(defaultTime)
    , _current(current)
    {
    }

    virtual void callback_patchPro(tars::Int32 ret,  const std::string& result);
    virtual void callback_patchPro_exception(tars::Int32 ret);

private:

    tars::PatchRequest _reqPro;
    NodePrx _nodePrx;
    int _defaultTime;
    tars::TarsCurrentPtr _current;
};


class StartServerCallbackImp: public NodePrxCallback
{
public:
    StartServerCallbackImp(string application, string serverName, string nodeName, tars::TarsCurrentPtr current)
    : _application(application)
    , _serverName(serverName)
    , _nodeName(nodeName)
    , _current(current)
    {
    }

    virtual void callback_startServer(tars::Int32 ret,  const std::string& result);
    virtual void callback_startServer_exception(tars::Int32 ret);

private:
    string _application;
    string _serverName;
    string _nodeName;
    tars::TarsCurrentPtr _current;
};

class StopServerCallbackImp: public NodePrxCallback
{
public:
    StopServerCallbackImp(string application, string serverName, string nodeName, tars::TarsCurrentPtr current)
    : _application(application)
    , _serverName(serverName)
    , _nodeName(nodeName)
    , _current(current)
    {
    }

    virtual void callback_stopServer(tars::Int32 ret,  const std::string& result);
    virtual void callback_stopServer_exception(tars::Int32 ret);

private:
    string _application;
    string _serverName;
    string _nodeName;
    tars::TarsCurrentPtr _current;
};

class NotifyServerCallbackImp: public NodePrxCallback
{
public:
    NotifyServerCallbackImp(tars::TarsCurrentPtr current)
    : _current(current)
    {
    }

    virtual void callback_notifyServer(tars::Int32 ret,  const std::string& result);
    virtual void callback_notifyServer_exception(tars::Int32 ret);

private:
    tars::TarsCurrentPtr _current;
};

class GetServerStateCallbackImp: public NodePrxCallback
{
public:
    GetServerStateCallbackImp(const NodePrx& nodePrx, string application, string serverName, string nodeName, const ServerStateDesc& state, tars::TarsCurrentPtr current)
    : _nodePrx(nodePrx)
    , _application(application)
    , _serverName(serverName)
    , _nodeName(nodeName)
    , _state(state)
    , _current(current)
    {
    }

    virtual void callback_getStateInfo(tars::Int32 ret,  const tars::ServerStateInfo& info,  const std::string& result);
    virtual void callback_getStateInfo_exception(tars::Int32 ret);
private:
    NodePrx _nodePrx;
    string  _application;
    string  _serverName;
    string  _nodeName;
    ServerStateDesc _state;
    tars::TarsCurrentPtr _current;
};


class GetPatchPercentCallbackImp: public NodePrxCallback
{
public:
    GetPatchPercentCallbackImp(string application, string serverName, string nodeName, tars::TarsCurrentPtr current)
    : _application(application)
    , _serverName(serverName)
    , _nodeName(nodeName)
    , _current(current)
    {
    }

    virtual void callback_getPatchPercent(tars::Int32 ret,  const tars::PatchInfo& tPatchInfo);
    virtual void callback_getPatchPercent_exception(tars::Int32 ret);

private:
    string _application;
    string _serverName;
    string _nodeName;
    tars::TarsCurrentPtr _current;
};


#endif
