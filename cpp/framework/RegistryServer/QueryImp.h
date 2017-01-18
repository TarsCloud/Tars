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

#ifndef __QUERY_IMP_H__
#define __QUERY_IMP_H__

#include "QueryF.h"
#include "DbHandle.h"

using namespace tars;

//////////////////////////////////////////////////////

enum FUNID
{
    FUNID_findObjectById              = 0,
    FUNID_findObjectById4Any          = 1,
    FUNID_findObjectById4All          = 2,
    FUNID_findObjectByIdInSameGroup   = 3,
    FUNID_findObjectByIdInSameStation = 4,
    FUNID_findObjectByIdInSameSet     = 5
};

//////////////////////////////////////////////////////
/**
 * 对象查询接口类
 */
class QueryImp: public QueryF
{
public:
    /**
     * 构造函数
     */
    QueryImp(){};

    /**
     * 初始化
     */
    virtual void initialize();

    /**
     ** 退出
     */
    virtual void destroy() {};

    /** 
     * 根据id获取所有该对象的活动endpoint列表
     */
    virtual vector<EndpointF> findObjectById(const string & id, tars::TarsCurrentPtr current);

    /**
     * 根据id获取所有对象,包括活动和非活动对象
     */
    virtual tars::Int32 findObjectById4Any(const std::string & id, vector<tars::EndpointF> &activeEp, vector<tars::EndpointF> &inactiveEp, tars::TarsCurrentPtr current);

    /** 
     * 根据id获取对象所有endpoint列表
     */
    Int32 findObjectById4All(const std::string & id, vector<tars::EndpointF> &activeEp, vector<tars::EndpointF> &inactiveEp, tars::TarsCurrentPtr current);

    /** 
     * 根据id获取对象同组endpoint列表
     */
    Int32 findObjectByIdInSameGroup(const std::string & id, vector<tars::EndpointF> &activeEp, vector<tars::EndpointF> &inactiveEp, tars::TarsCurrentPtr current);

    /** 
     * 根据id获取对象指定归属地的endpoint列表
     */
    Int32 findObjectByIdInSameStation(const std::string & id, const std::string & sStation, vector<tars::EndpointF> &activeEp, vector<tars::EndpointF> &inactiveEp, tars::TarsCurrentPtr current);

    /** 
     * 根据id获取对象同set endpoint列表
     */
    Int32 findObjectByIdInSameSet(const std::string & id,const std::string & setId,vector<tars::EndpointF> &activeEp,vector<tars::EndpointF> &inactiveEp, tars::TarsCurrentPtr current);

private:
    /**
     * 打印按天日志
     */
    void doDaylog(const FUNID eFnId,const string& id,const vector<tars::EndpointF> &activeEp, const vector<tars::EndpointF> &inactiveEp, const tars::TarsCurrentPtr& current,const std::ostringstream& os,const string& sSetid="");

    /**
     * 转化成字符串
     */
    string eFunTostr(const FUNID eFnId);

protected:

    //数据库操作
    CDbHandle      _db;

};

#endif
