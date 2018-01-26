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

#ifndef NOTIFY_I_H
#define NOTIFY_I_H

#include "NotifyF.h"
#include "util/tc_common.h"
#include "util/tc_config.h"
#include "util/tc_mysql.h"
#include "servant/TarsLogger.h"
#include "LoadDbThread.h"

using namespace tars;

class NotifyImp : public Notify
{
public:
    /**
     * 初始化
     *
     * @return int
     */
    virtual void initialize();

    /**
     * 退出
     */
    virtual void destroy() {};

    /**
     * report
     * @param sServerName
     * @param sThreadId
     * @param sResult
     * @param current
     */
    virtual void reportServer(const string& sServerName, const string& sThreadId, const string& sResult, tars::TarsCurrentPtr current);

    /**
     * notify
     * @param sServerName
     * @param sThreadId
     * @param sCommand
     * @param sResult
     * @param current
     */
    virtual void notifyServer(const string& sServerName, NOTIFYLEVEL level, const string& sMessage, tars::TarsCurrentPtr current);

    /**
     * get notify info
     */
    virtual tars::Int32 getNotifyInfo(const tars::NotifyKey & stKey,tars::NotifyInfo &stInfo,tars::TarsCurrentPtr current);

    /*
     *reportNotifyInfo
     *@param info
     */
    virtual void reportNotifyInfo(const tars::ReportInfo & info, tars::TarsCurrentPtr current);

protected:

    void loadconf();

private:

    bool IsdbTableExist(const string& sTbName);

    void creatTb(const string &sTbName);

    bool IsNeedFilte(const string& sServerName,const string& sResult);

protected:
    TC_Mysql _mysqlConfig;
    string     _sql;                //创建表
    size_t   _maxPageSize;
    size_t   _maxPageNum;
   /*
     * 按模块配置过滤规则
     * key=default 表示全局规则
     * key=app.server具体模块规则
     * 模块规则匹配失败才匹配全局规则
       <tars>
         <filter>
            default=r1|r2
            MTT.HelloServer=r3|r4
         </filter> 
       </tars>
     */
   map<string,set<string> > _setFilter;
};

#endif
