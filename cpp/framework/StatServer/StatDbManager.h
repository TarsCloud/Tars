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

#ifndef __DB_MANAGER_H_
#define __DB_MANAGER_H_

#include "util/tc_common.h"
#include "util/tc_thread.h"
#include "util/tc_option.h"
#include "util/tc_file.h"
#include "util/tc_mysql.h"
#include "util/tc_config.h"
#include "servant/TarsLogger.h"
#include "jmem/jmem_hashmap.h"
#include "servant/StatF.h"
#include "StatHashMap.h"

using namespace tars;

class StatDbManager : public TC_Singleton<StatDbManager>,public TC_ThreadMutex 
{
public:
    enum CutType
    {
        CUT_BY_DAY      = 0,
        CUT_BY_HOUR     = 1,
        CUT_BY_MINUTE   = 2,
    };

public:
    /**
     *
     */
    StatDbManager();

    ~StatDbManager();

public:

    int creatTb(const string &strTbName,TC_Mysql *pMysql = NULL);

    int creatTable(const string &sTbName, TC_Mysql *_pstMql);

    int creatEscTb(const string &sTbName, const string& sSql , TC_Mysql *pMysql);

    int insert2Db(const StatMsg &statmsg,const string &sDate,const string &sFlag,const string &sTbNamePre = "",TC_Mysql *pMysql = NULL);

    int updateEcsStatus(const string &sLastTime,const string &sTbNamePre = "",TC_Mysql *pMysql = NULL);

    int checkLastTime(const string &sLastTime,const string &sTbNamePre,TC_Mysql *pMysql);

    int insert2MultiDbs(int iIndex, const StatMsg &vStatmsg, const string &sDate, const string &sFlag);

    int sendAlarmSMS(const string &sMsg);

    int    getDbNumber();

    int genRandOrder();

     /**
      * 获取各个db的权重值，并返回所有权重的最大公约数值和最大权重
      * @param iGcd
      * @param iMaxW
      * @param vDbWeighted
      */
    void getDbWeighted(int& iGcd,int& iMaxW,vector<int>& vDbWeighted);

    bool IsEnableWeighted();

    bool IsdbTableExist(const string& sTbName,TC_Mysql *pMysql);

    size_t getDbToIpIndex(size_t iIndex);

    size_t getDbIpNum() { return _dbIpNum; }

    string getIpAndPort(size_t iDbIndex);

    map<string, vector<size_t> >& getIpHasDbInfo() { return _mIpHasDbInfo; }

    void setITerminateFlag(bool bFlag) { _terminate = bFlag; }

private:

    int getGcd (int a, int b);

private:

    //入库时，停止的控制开关
    bool                _terminate;

    //上次创建数据表的时间
    string              _lastDayTimeTable;

    //创建表
    string                _sql;        

    //创建表t_ecstatus
    string              _sqlStatus;

    //表前缀
    string              _tbNamePre;

    //一次最大插入条数
    int                 _maxInsertCount;

    //分表类型
    CutType             _eCutType;

    //插入数据的mysql db信息
    vector<TC_Mysql *>  _vMysql;

    //数据表前缀
    vector<string>      _vsTbNamePre;

    //db个数
    int                 _dbNumber;

    //随机值
    vector<int>         _vRandOrder;

    //各个db的权重值
    vector<int>        _vDbWeighted;
    bool               _enableWeighted;

    //db的ip个数
    size_t                _dbIpNum;

    //设置每个db实例使用写db的线程下标，即每个写db线程负责写哪些db实例的数据
    map<size_t, size_t> _mDbToIp;

    //
    map<string, vector<size_t> > _mIpHasDbInfo;

    //打印日志使用
    vector<string>      _vIpAndPort;

    
};

#endif


