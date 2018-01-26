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
#include "servant/PropertyF.h"
#include "PropertyHashMap.h"

using namespace tars;

class PropertyDbManager : public TC_Singleton<PropertyDbManager>,public TC_ThreadMutex 
{
public:
    enum CutType
    {
        CUT_BY_DAY      = 0,
        CUT_BY_HOUR     = 1,
        CUT_BY_MINUTE   = 2,
    };

public:
    PropertyDbManager();

    ~PropertyDbManager();

public:

    int creatTb(const string &strTbName,TC_Mysql *pMysql = NULL);

    int creatTable(const string &sTbName, TC_Mysql *_pstMql);

    int creatEscTb(const string &sTbName, const string& sSql , TC_Mysql *pMysql);

    int insert2Db(const PropertyMsg &mPropMsg,const string &sDate,const string &sFlag, int iOldWriteNum, int &iNowWriteNum,const string &sTbNamePre = "",TC_Mysql *pMysql = NULL);

    int updateEcsStatus(const string &sLastTime,const string &sTbNamePre = "",TC_Mysql *pMysql = NULL);

    int checkLastTime(const string &sLastTime,const string &sTbNamePre,TC_Mysql *pMysql);

    int insert2MultiDbs(int iIndex, const PropertyMsg &propertymsg, const string &sDate, const string &sFlag);

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

    bool hasEnableWeighted();

    bool hasdbTableExist(const string& sTbName,TC_Mysql *pMysql);

    size_t getDbToIpIndex(size_t iIndex);

    size_t getDbIpNum() { return _dbIpNum; }

    size_t getInsertDbThreadNum() { return _dbIpNum * _oneDbHasThreadNum; }

    string getIpAndPort(size_t iDbIndex);

    map<string, vector<size_t> >& getIpHasDbInfo() { return _ipHasDbInfo; }

    void setTerminateFlag(bool bFlag) { _terminate = bFlag; }

private:

    /**
     *Greatest Common Divisor
     *最大公约数
     */
    int getGcd (int a, int b);

private:

    
    bool                _terminate;        //入库时，停止的控制开关
    string                _sql;               //创建表
    string              _sqlStatus;        //创建表t_ecstatus
    string              _tbNamePre;        //表前缀
    int                 _maxInsertCount;   //一次最大插入条数
    CutType             _cutType;          //分表类型
    int                    _tableInterval;
    map<string,string>  _reapSql;          //定时执行sql

    vector<TC_Mysql *> _ssdThreadMysql;    // reapthread中使用。
    vector<string>     _sTbNamePre;
    int                _dbNumber;
    vector<int>        _randOrder;

    //各个db的权重值
    vector<int> _dbWeighted;
    bool _enableWeighted;

    //多线程入库使用
    size_t                _dbIpNum;
    size_t                _oneDbHasThreadNum;
    map<size_t, size_t> _dbToIp;
    map<string, vector<size_t> > _ipHasDbInfo;
    //打印日志使用
    vector<string>      _ipAndPort;

    
};

#endif


