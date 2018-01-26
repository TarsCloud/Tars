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

#include "StatDbManager.h"
#include "util/tc_config.h"
#include "StatServer.h"
#include "StatHashMap.h"

///////////////////////////////////////////////////////////
StatDbManager::StatDbManager()
: _terminate(false)
, _lastDayTimeTable("")
{
    TLOGDEBUG("begin StatDbManager init" << endl);

    _sql               = (*g_pconf)["/tars<sql>"];
    _sqlStatus         = g_pconf->get("/tars<sqlStatus>", "");
    _tbNamePre         = g_pconf->get("/tars/db<tbnamePre>","t_stat_realtime_");
    _maxInsertCount    = TC_Common::strto<int>(g_pconf->get("/tars/reapSql<maxInsertCount>","1000"));

    //默认不使用权重
    _enableWeighted    = TC_Common::strto<bool>(g_pconf->get("/tars/<enWeighted>","0"));

    size_t iInsertDbThreaad = TC_Common::strto<int>(g_pconf->get("/tars/reapSql<insertDbThreadNum>","4"));

    if (_sqlStatus == "")
    {
        _sqlStatus = "CREATE TABLE `t_ecstatus` ( "
        "  `id` int(11) NOT NULL auto_increment, "
        "  `appname` varchar(64) NOT NULL default '', "
        "  `action` tinyint(4) NOT NULL default '0', "
        "  `checkint` smallint(6) NOT NULL default '10', "
        "  `lasttime` varchar(16) NOT NULL default '', "
        "   PRIMARY KEY  (`appname`,`action`), "
        "   UNIQUE KEY `id` (`id`) "
        " ) ENGINE=HEAP DEFAULT CHARSET=utf8";
    }

    string sCutType     = g_pconf->get("/tars/reapSql<CutType>","hour");
    if (sCutType == "day")
    {
        _eCutType = CUT_BY_DAY;
    }
    else if (sCutType == "minute")
    {
        _eCutType = CUT_BY_MINUTE;
    }
    else
    {
        _eCutType = CUT_BY_HOUR;
    }

    vector<string> vDb =g_pconf->getDomainVector("/tars/multidb");

    _dbNumber = vDb.size();

    TLOGDEBUG("StatDbManager init multidb size:" <<_dbNumber  << endl);

    map<string, size_t> mIp;
    vector<string> vIp;
    string sIp("");
    string sIpAndPort("");

    for (int i=0; i< _dbNumber; i++)
    {
        TC_DBConf tConf;
        tConf.loadFromMap(g_pconf->getDomainMap("/tars/multidb/" + vDb[i]));

        _vsTbNamePre.push_back(g_pconf->get("/tars/multidb/" + vDb[i] + "<tbname>", "t_stat_0" + TC_Common::tostr(i) + "_"));

        sIp = tConf._host;

        sIpAndPort = "ip:";
        sIpAndPort += tConf._host;
        sIpAndPort += "|port:";
        sIpAndPort += TC_Common::tostr(tConf._port);

        _vIpAndPort.push_back(sIpAndPort);

        map<string, size_t>::const_iterator iter = mIp.find(sIp);
        if(iter == mIp.end())
        {
            mIp.insert(map<string, size_t>::value_type(sIp, 0));

            _dbIpNum++;

            vector<size_t> vIndex;
            vIndex.push_back(i);

            _mIpHasDbInfo.insert(map<string, vector<size_t> >::value_type(sIp, vIndex));
        }
        else
        {
            _mIpHasDbInfo[sIp].push_back(i);
        }

        vIp.push_back(sIp);

        //默认值为1
        _vDbWeighted.push_back(TC_Common::strto<int>(g_pconf->get("/tars/multidb/" + vDb[i] + "<weighted>","1")));

         TC_Mysql *pMysql = new TC_Mysql();
         pMysql->init(tConf);

        _vMysql.push_back(pMysql);
    }

    //如果写db的线程小于db ip的个数，则设置db ip的个数为写db的线程数
    if(iInsertDbThreaad < _dbIpNum)
    {
        _dbIpNum = iInsertDbThreaad;
    }

    TLOGDEBUG("StatDbManager init insert DB threadnum:" << _dbIpNum  << endl);

    //设置每个db ip使用写db的线程下标，即每个写db线程负责写哪些ip的db数据
    size_t iIndex = 0;
    map<string, size_t>::iterator map_it = mIp.begin();
    while(map_it != mIp.end())
    {
        map_it->second = iIndex;
        ++iIndex;

        if(iIndex == _dbIpNum)
        {
            iIndex = 0;
        }

        ++map_it;
    }

    //设置每个db实例使用写db的线程下标，即每个写db线程负责写哪些db实例的数据
    for(size_t i = 0; i < vIp.size(); ++i)
    {
        map<string, size_t>::const_iterator iter = mIp.find(vIp[i]);
        if(iter != mIp.end())
        {
            _mDbToIp.insert(map<size_t, size_t>::value_type(i, iter->second));
        }
        else
        {
            TLOGERROR("DbManager init DbToIp error ip:" << vIp[i] << endl);
        }
    }

    map<string, vector<size_t> >::iterator m_iter = _mIpHasDbInfo.begin();
    while(m_iter != _mIpHasDbInfo.end())
    {
        vector<size_t> &vDb = m_iter->second;

        FDLOG("CountStat") << "stat ip:" << ServerConfig::LocalIp << "|ip:" << m_iter->first << "|DbNum:" << vDb.size() << endl;

        ++m_iter;
    }


    TLOGDEBUG("StatDbManager init ok." << endl);
}
///////////////////////////////////////////////////////////
StatDbManager::~StatDbManager()
{
    vector<TC_Mysql*>::iterator it = _vMysql.begin();
    while(it != _vMysql.end())
    {
        if(*it)
        {
            delete *it;
        }
        ++it;
    }
}
///////////////////////////////////////////////////////////
size_t StatDbManager::getDbToIpIndex(size_t iIndex)
{
    map<size_t, size_t>::const_iterator iter = _mDbToIp.find(iIndex);
    if(iter != _mDbToIp.end())
    {
        return iter->second;
    }
    return -1;
}
///////////////////////////////////////////////////////////
bool StatDbManager::IsEnableWeighted()
{
    return _enableWeighted;
}
///////////////////////////////////////////////////////////
string StatDbManager::getIpAndPort(size_t iDbIndex) 
{ 
    assert(iDbIndex < _vIpAndPort.size());

    return _vIpAndPort[iDbIndex];
}
///////////////////////////////////////////////////////////
void StatDbManager::getDbWeighted(int& iGcd,int& iMaxW,vector<int>& vDbWeighted)
{
    vDbWeighted = _vDbWeighted;

    int gcd = 0;
    int iMax = 0;
    for(size_t i = 0;i < vDbWeighted.size(); i++)
    {
        gcd = getGcd(gcd,vDbWeighted[i]);

        if(vDbWeighted[i] > iMax)
        {
            iMax = vDbWeighted[i];
        }
    }

    iGcd = gcd;

    iMaxW = iMax;
}
///////////////////////////////////////////////////////////
int StatDbManager::getGcd (int a, int b)
{
    int c;
    while(a != 0)
    {
         c = a;
         a = b%a;
         b = c;
    }

    return b;
}
///////////////////////////////////////////////////////////
int StatDbManager::genRandOrder()
{
    ostringstream os;
    _vRandOrder.clear();
    srand((unsigned)time(NULL));
    int iValue = 0;
    size_t i = 0;
    while(1)
    {
        int iFound = 0;
        iValue =  rand()%(_dbNumber );
        for (i=0; i<_vRandOrder.size(); i++)
        {
            if (iValue == _vRandOrder[i])
            {
                iFound    = 1;
                break;
            }
        }
        if ( iFound == 0)
        {
            _vRandOrder.push_back(iValue);
        }
        else
        {
            continue;
        }
        if (_vRandOrder.size() ==    (size_t)_dbNumber  )
        {
            break;
        }
    }

    vector<int>::iterator it = _vRandOrder.begin();
    while(it != _vRandOrder.end() )
    {
        os << *it++ << " " ;
    }
    TLOGDEBUG("randorder: " << os.str() << endl);
    return 0;

}
///////////////////////////////////////////////////////////
int StatDbManager::insert2Db(const StatMsg &statmsg, const string &sDate, const string &sFlag, const string &sTbNamePre, TC_Mysql *pMysql)
{
    int iCount  = 0;
    string sSql;
    string strValues;

    string sTbName  = (sTbNamePre != "" ? sTbNamePre : _tbNamePre);
    sTbName += TC_Common::replace(sDate, "-", "");
    sTbName += sFlag.substr(0,_eCutType * 2);

    try
    {
        string sEscapeString = pMysql->escapeString(ServerConfig::LocalIp);

        creatTable(sTbName,pMysql);

        for (StatMsg::const_iterator it = statmsg.begin(); it != statmsg.end(); it++ )
        {
            if(_terminate)
            {
                return -1;
            }

            const StatMicMsgHead& head      = it->first;
            const StatMicMsgBody& body      = it->second;

            string strIntervCount;
            int iTemp = 0;
            for (map<int,int>::const_iterator it = body.intervalCount.begin();it != body.intervalCount.end();it++)
            {
                if (iTemp != 0)
                {
                    strIntervCount += ",";
                }
                strIntervCount = strIntervCount;
                strIntervCount += TC_Common::tostr(it->first);
                strIntervCount += "|";
                strIntervCount += TC_Common::tostr(it->second);
                iTemp++;
            }

            int iAveTime = 0;
            if ( body.count != 0 )
            {
                iAveTime = body.totalRspTime/body.count;
                iAveTime == 0?iAveTime=1:iAveTime=iAveTime;
            }

            //组织sql语句
            strValues = " ('";
            strValues += sEscapeString;
            strValues += "','";
            strValues += sDate;
            strValues += "','";
            strValues += sFlag;
            strValues += "','";
            strValues += pMysql->escapeString(head.masterName);
            strValues += "','";
            strValues += pMysql->escapeString(head.slaveName);
            strValues += "','";
            strValues += pMysql->escapeString(head.interfaceName);
            strValues += "','";
            strValues += pMysql->escapeString(head.tarsVersion);
            strValues += "','";
            strValues += pMysql->escapeString(head.masterIp);
            strValues += "','";
            strValues += pMysql->escapeString(head.slaveIp);
            strValues += "',";
            strValues += TC_Common::tostr<tars::Int32>( head.slavePort);
            strValues += ",";
            strValues += TC_Common::tostr<tars::Int32>( head.returnValue );
            strValues += ",";
            strValues += TC_Common::tostr<tars::Int32>(body.count);
            strValues += ",";
            strValues += TC_Common::tostr<tars::Int32>(body.timeoutCount);
            strValues += ",";
            strValues += TC_Common::tostr<tars::Int32>(body.execCount);
            strValues += ",";
            strValues += TC_Common::tostr<tars::Int64>(body.totalRspTime);
            strValues += ",'";
            strValues += strIntervCount;
            strValues += "',";
            strValues += TC_Common::tostr<tars::Int32>(iAveTime);
            strValues += ",";
            strValues += TC_Common::tostr<tars::Int32>(body.maxRspTime);
            strValues += ",";
            strValues += TC_Common::tostr<tars::Int32>(body.minRspTime ) + ") ";

            if( iCount == 0 )
            {
                sSql = "insert ignore into ";
                sSql += sTbName;
                sSql += " (source_id,f_date,f_tflag,master_name,slave_name,interface_name,tars_version,master_ip,slave_ip,slave_port,return_value, succ_count,timeout_count,exce_count,total_time,interv_count,ave_time,maxrsp_time, minrsp_time) values ";
                sSql += strValues;
            }
            else
            {
                sSql += ",";
                sSql += strValues;
            }

            iCount ++;
            if ( iCount >= _maxInsertCount )
            {
                usleep(100);
                pMysql->execute(sSql);
                TLOGDEBUG("insert " << sTbName << " affected:" << iCount  << endl);

                FDLOG("CountStat") << "stat ip:" << ServerConfig::LocalIp << "|insert " << sTbName << "|insert affected:" << iCount << "|mysql affected:" << pMysql->getAffectedRows() << endl;
                iCount = 0;
            }
        }

        if ( iCount != 0 )
        {
            pMysql->execute(sSql);
            TLOGDEBUG("insert " << sTbName << " affected:" << iCount << endl);
            FDLOG("CountStat") << "stat ip:" << ServerConfig::LocalIp << "|insert " << sTbName << "|insert affected:" << iCount << "|mysql affected:" << pMysql->getAffectedRows() << endl;
        }
    }
    catch (TC_Mysql_Exception& ex)
    {
        string err = string (ex.what());
        if (std::string::npos == err.find( "Duplicate"))
        {
            creatTable(sTbName,pMysql);
        }
        //因为会输出1千条记录，这里做截取
        TLOGERROR("insert2Db exception: " << err.substr(0,64) << endl);
        return 1;
    }
    catch (exception& ex)
    {
        TLOGERROR("insert2Db exception: " << ex.what() << endl);
        return 1;
    }
    return 0;
}
///////////////////////////////////////////////////////////
int StatDbManager::creatTable(const string &sTbName, TC_Mysql *_pstMql)
{
    string daytime = sTbName.substr(0, sTbName.length()- 2); // 去掉小时

    if (_lastDayTimeTable == daytime)
    {
        TLOGDEBUG("StatDbManager::creatTable table " << sTbName << " exist." << endl);
        return 0;
    }

    int iRet = 0;
    int hour = 0;

    for (hour=0; hour < 24; hour++)
    {
        char buf[3];
        snprintf(buf,sizeof(buf),"%.2d",hour);
        string sName =   daytime + string(buf);
        iRet |= creatTb(sName,  _pstMql);
    }

    _lastDayTimeTable = daytime;

    return iRet;
}
///////////////////////////////////////////////////////////
bool StatDbManager::IsdbTableExist(const string& sTbName,TC_Mysql *pMysql)
{
    try
    {
        TC_Mysql::MysqlData tTotalRecord = pMysql->queryRecord("show tables like '%"+sTbName+"%'");
        TLOGINFO(__FUNCTION__<<"|show tables like '%"+sTbName+"%|affected:"<<tTotalRecord.size()<<endl);
        if (tTotalRecord.size() > 0)
        {
            return true;
        }
        else
        {
            return false;
        }

    }catch(TC_Mysql_Exception& ex)
    {
        TLOGERROR("StatDbManager::IsdbTableExist exception: " << ex.what() << endl);
        return false;
    }
}
///////////////////////////////////////////////////////////
int StatDbManager::creatTb(const string &sTbName,TC_Mysql *pMysql)
{
    try
    {
        if (!IsdbTableExist(sTbName,pMysql))
        {
            string sSql = TC_Common::replace(_sql, "${TABLE}",sTbName);
            TLOGINFO(sSql << endl);
            pMysql->execute(sSql);
        }
        return 0;
    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR("StatDbManager::creatTb exception: " << ex.what() << endl);
        return 1;
    }
}
///////////////////////////////////////////////////////////
int StatDbManager::creatEscTb(const string &sTbName, const string& sSql , TC_Mysql *pMysql)
{
    try
    {
        if (!IsdbTableExist(sTbName,pMysql))
        {
            TLOGDEBUG(sSql << endl);
            pMysql->execute(sSql);

        }
        return 0;
    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR("exception: " << ex.what() << endl);
        return 1;
    }
}
///////////////////////////////////////////////////////////
int StatDbManager::updateEcsStatus(const string &sLastTime,const string &sTbNamePre,TC_Mysql *pMysql)
{
    try
    {
        string sAppName  = sTbNamePre != "" ? sTbNamePre : _tbNamePre;
        string sCondition = "where  appname='";
        sCondition += sAppName;
        sCondition += "' and action=0";

        TC_Mysql::RECORD_DATA rd;
        rd["lasttime"]      = make_pair(TC_Mysql::DB_STR, sLastTime);

        int iRet = pMysql->updateRecord("t_ecstatus", rd,sCondition);

        TLOGDEBUG("StatDbManager::updateEcsStatus iRet: " <<iRet <<" "<<pMysql->getLastSQL()<< endl);

        if (iRet == 0 )
        {
            rd["appname"]       = make_pair(TC_Mysql::DB_STR, sAppName);
            rd["checkint"]      = make_pair(TC_Mysql::DB_INT, TC_Common::tostr(g_app.getInserInterv()));
            rd["lasttime"]      = make_pair(TC_Mysql::DB_STR, sLastTime);
            iRet = pMysql->replaceRecord("t_ecstatus", rd);
        }

        if (iRet != 1)
        {
            TLOGERROR("StatDbManager::updateEcsStatus erro: ret:" << iRet<<" "<<pMysql->getLastSQL()<< endl);
        }
    }
    catch (TC_Mysql_Exception& ex)
    {
        string sql = _sqlStatus;
        TLOGERROR("StatDbManager::updateEcsStatus exception: " << ex.what() << endl);
        creatEscTb("t_ecstatus", sql, pMysql);
        return 1;
    }
    return 0;
}
///////////////////////////////////////////////////////////
int StatDbManager::insert2MultiDbs(int iIndex, const StatMsg &statmsg, const string &sDate, const string &sFlag)
{
    try
    {
        string sIp = ServerConfig::LocalIp;

        string sLastTime  = sDate + " " + sFlag;

        string sTbNamePre = "";
        TC_Mysql * pMysql = NULL;

        sTbNamePre = _vsTbNamePre[iIndex];

        pMysql = _vMysql[iIndex];

        if (checkLastTime(sLastTime,  sTbNamePre + sIp, pMysql) == 0)  //=0,没有记录表示: 数据库记录的时间，比当前当前时间小
        {
            TLOGDEBUG("begin insert to db " << getIpAndPort(iIndex) << endl);

            int64_t iBegin = tars::TC_TimeProvider::getInstance()->getNowMs();

            if(insert2Db(statmsg, sDate, sFlag, sTbNamePre, pMysql) != 0)
            {
                if(_terminate)
                {
                    return -1;
                }

                if(insert2Db(statmsg ,sDate, sFlag, sTbNamePre, pMysql) != 0 )
                {
                    if(_terminate)
                    {
                        return -1;
                    }

                    string sMsg("insert2Db_");
                    sMsg += getIpAndPort(iIndex);

                    sendAlarmSMS(sMsg);
                }
                else
                {
                    if(updateEcsStatus(sLastTime, sTbNamePre + sIp, pMysql) != 0)
                     {
                        string sMsg("updateEcsStatus_");
                        sMsg += getIpAndPort(iIndex);

                        sendAlarmSMS(sMsg);
                     }
                }
            }
            else
            {
                 if(updateEcsStatus(sLastTime, sTbNamePre + sIp, pMysql) != 0)
                 {
                    string sMsg("updateEcsStatus_");
                    sMsg += getIpAndPort(iIndex);

                    sendAlarmSMS(sMsg);
                 }
            }

            int64_t iEnd = tars::TC_TimeProvider::getInstance()->getNowMs();

            TLOGDEBUG("insert|" << iIndex << "|" << getIpAndPort(iIndex) << "|" << sDate << "|" << sFlag << "|" << statmsg.size() << "|" << (iEnd - iBegin) << endl);
            FDLOG("CountStat") << "stat ip:" << ServerConfig::LocalIp  << "|insert|dbIndex:" << iIndex << "|" << getIpAndPort(iIndex) << "|date:" << sDate << "|tflag:" << sFlag 
                << "|records:" << statmsg.size() << "|timecost(ms):" << (iEnd - iBegin) << "|iBegin(ms):" << iBegin << "|iEnd(ms):" << iEnd << endl;
        }
    }
    catch(TC_Mysql_Exception& ex)
    {
        TLOGERROR("StatDbManager::insert2MultiDbs TC_Mysql_Exception: " << ex.what() << endl);
    }
    catch(exception& ex)
    {
        TLOGERROR("StatDbManager::insert2MultiDbs exception: " << ex.what() << endl);
    }

    return 0;
}

///////////////////////////////////////////////////////////
int StatDbManager::sendAlarmSMS(const string &sMsg)
{
    string errInfo = " ERROR:" + ServerConfig::LocalIp + "|" + sMsg +  ":统计入库失败，请及时处理!";
    TARS_NOTIFY_ERROR(errInfo);

    TLOGERROR("TARS_NOTIFY_ERROR " << errInfo << endl);

    return 0;
}
///////////////////////////////////////////////////////////
int StatDbManager::getDbNumber()
{
    return _dbNumber;
}
///////////////////////////////////////////////////////////
int StatDbManager::checkLastTime(const string &sLastTime,const string &sTbNamePre,TC_Mysql *pMysql)
{
    int iRet = 0;
    try
    {
        string sAppName  = sTbNamePre != "" ? sTbNamePre : _tbNamePre;
        string sCondition = "where  appname='";
        sCondition += sAppName;
        sCondition += "' and lasttime >= '";
        sCondition += sLastTime;
        sCondition += "'" ;

        iRet = pMysql->getRecordCount("t_ecstatus", sCondition);

        TLOGDEBUG("StatDbManager::checkLastTime iRet: " <<iRet <<" "<<pMysql->getLastSQL()<< endl);
    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR("StatDbManager::checkLastTime exception: " << ex.what() << endl);
        creatEscTb("t_ecstatus", _sqlStatus, pMysql);
        return 0;
    }

    return iRet;
}

