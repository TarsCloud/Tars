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

#ifndef __REGISTRY_IMP_H__
#define __REGISTRY_IMP_H__

#include "util/tc_common.h"
#include "util/tc_config.h"

#include "Registry.h"
#include "DbHandle.h"

using namespace tars;

//////////////////////////////////////////////////////
/*
 * 提供给node调用的接口类
 */
class RegistryImp: public Registry
{
public:
    /**
     * 构造函数
     */
    RegistryImp(){};

    /**
     * 初始化
     */
    virtual void initialize();

    /**
     ** 退出
     */
    virtual void destroy() {};

    /**
     * 获取数据
     * @param k
     * @param v
     *
     * @return int
     */
    int get(int &i, tars::TarsCurrentPtr current);

    /**
     * node启动的时候往registry注册一个session
     *
     * @param name node名称
     * @param ni   node详细信息
     * @param li    node机器负载信息
     *
     * @return 注册是否成功
     */
    virtual int registerNode(const string & name, const NodeInfo & ni, const LoadInfo & li, tars::TarsCurrentPtr current);

    /**
     * node上报心跳负载
     *
     * @param name node名称
     * @param li    node机器负载信息
     *
     * @return 心跳接收状态
     */
    virtual int keepAlive(const string& name, const LoadInfo & li, tars::TarsCurrentPtr current);

    /**
     * 获取在该node部署的server列表
     *
     * @param name    node名称
     *
     * @return server名称列表
     */
    virtual vector<tars::ServerDescriptor> getServers(const std::string & app, const std::string & serverName, const std::string & nodeName, tars::TarsCurrentPtr current);

    /**
     * 更新server状态
     *
     * @param nodeName : node id
     * @param app:       应用
     * @param serverName: server 名
     * @param state :  server状态
     *
     * @return server信息列表
     */
    virtual int updateServer(const string & nodeName, const string & app, const string & serverName, const tars::ServerStateInfo & stateInfo, tars::TarsCurrentPtr current);

    /**
     * 量批更新server状态
     *
     * @param vecStateInfo : 批量server状态
     *
     * @return server信息列表
     */
    virtual int updateServerBatch(const std::vector<tars::ServerStateInfo> & vecStateInfo, tars::TarsCurrentPtr current);

    /**
     * node停止，释放node的会话
     *
     * @param name    node名称
     */
    virtual int destroy(const string & name, tars::TarsCurrentPtr current);

    /**
     * 上报server的tars库版本
     * @param app:       应用
     * @param serverName: server 名
     * @param nodeName : node id
     * @param version:  server基于的tars版本号
     *
     * @return 0-成功 others-失败
     */
    virtual int reportVersion(const string & app, const string & serverName, const string & nodeName, const string & version, tars::TarsCurrentPtr current);
    
    /**
     * 获取node的模板配置
     * @param nodeName: node名称
     * @param out profileTemplate:  对应模板内容
     *
     * @return 0-成功 others-失败
     */
    virtual tars::Int32 getNodeTemplate(const std::string & nodeName, std::string &profileTemplate, tars::TarsCurrentPtr current);

    /**
     * node通过接口获取连接上主控的node ip
     * @param sNodeIp:  node 的ip
     *
     * @return 0-成功 others-失败
     */
    virtual tars::Int32 getClientIp(std::string &sClientIp, tars::TarsCurrentPtr current);
    
    /**
     * 发布任务完成后，UPDATE版本号和发布人 
     * @param PatchResult: 发布结果 
     *  
     * @return 0-成功 othres-失败 
     */
    virtual tars::Int32 updatePatchResult(const PatchResult & result, tars::TarsCurrentPtr current);

protected:

    /*
     * 数据库操作
     */
    CDbHandle      _db;

};


#endif
