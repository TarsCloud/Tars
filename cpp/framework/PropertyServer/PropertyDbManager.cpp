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

#include "PropertyDbManager.h"
#include "PropertyReapThread.h"
#include "util/tc_config.h"
#include "PropertyServer.h"
#include "PropertyHashMap.h"
///////////////////////////////////////////////////////////
//

PropertyDbManager::PropertyDbManager()
: _terminate(false)
{
    TLOGDEBUG("begin PropertyDbManager init" << endl);

    _sql               = (*g_pconf)["/tars<sql>"];
    _sqlStatus         = g_pconf->get("/tars<sqlStatus>", "");
    _reapSql           = g_pconf->getDomainMap("/tars/reapSql");
    _tbNamePre         = g_pconf->get("/tars/db<tbnamePre>","t_property_realtime_");
    _maxInsertCount    = TC_Common::strto<int>(g_pconf->get("/tars/reapSql<maxInsertCount>","1000"));
    _enableWeighted    = TC_Common::strto<bool>(g_pconf->get("/tars/<enWeighted>","0"));

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
        " ) ENGINE=HEAP DEFAULT CHARSET=gbk";
    }

    string sCutType     = g_pconf->get("/tars/reapSql<CutType>","hour");
    if (sCutType == "day")
    {
        _cutType = CUT_BY_DAY;
    }
    else if (sCutType == "minute")
    {
        _cutType = CUT_BY_MINUTE;
    }
    else
    {
        _cutType = CUT_BY_HOUR;
    }

    size_t iInsertThreadByMachine = TC_Common::strto<int>(g_pconf->get("/tars/reapSql<insertThreadByMachine>","4"));
    size_t iInsertThreadByDB      = TC_Common::strto<int>(g_pconf->get("/tars/reapSql<insertThreadByDB>","2"));

    TLOGDEBUG("PropertyDbManager init iInsertThreadByMachine:" << iInsertThreadByMachine << "|iInsertThreadByDB:" << iInsertThreadByDB << endl);

    vector<string> vDb =g_pconf->getDomainVector("/tars/multidb");

    _dbNumber = vDb.size();

    TLOGDEBUG("PropertyDbManager init multidb size:" <<_dbNumber  << endl);

    //map<string, size_t> mIp;
    map<string, pair<size_t, size_t> > mIp;
    vector<string> vIp;
    string sIp("");
    string sIpAndPort("");

    for (int i=0; i< _dbNumber; i++)
    {
        TC_DBConf tConf;
        tConf.loadFromMap(g_pconf->getDomainMap("/tars/multidb/" + vDb[i]));
        _sTbNamePre.push_back(g_pconf->get("/tars/multidb/" + vDb[i] + "<tbname>",
                                              "t_propert_0" + TC_Common::tostr(i) + "_"));

        sIp = tConf._host;

        sIpAndPort  = "ip:";
        sIpAndPort += tConf._host;
        sIpAndPort += "|port:";
        sIpAndPort += TC_Common::tostr(tConf._port);

        _ipAndPort.push_back(sIpAndPort);

        map<string, pair<size_t, size_t> >::const_iterator iter = mIp.find(sIp);

        if(iter == mIp.end())
        {
            pair<size_t, size_t> tempPair;
            tempPair.first  = 0;
            tempPair.second = 0;

            //mIp.insert(map<string, size_t>::value_type(sIp, 0));
            mIp.insert(map<string, pair<size_t, size_t> >::value_type(sIp, tempPair));

            _dbIpNum++;

            vector<size_t> vIndex;
            vIndex.push_back(i);

            _ipHasDbInfo.insert(map<string, vector<size_t> >::value_type(sIp, vIndex));
        }
        else
        {
            _ipHasDbInfo[sIp].push_back(i);
        }

        vIp.push_back(sIp);

        //默认值为1
        _dbWeighted.push_back(TC_Common::strto<int>(g_pconf->get("/tars/multidb/" + vDb[i] + "<weighted>","1")));

         TC_Mysql *pMysql = new TC_Mysql();
         pMysql->init(tConf);
        _ssdThreadMysql.push_back(pMysql);
    }

    if(iInsertThreadByMachine < _dbIpNum)
    {
        _dbIpNum = iInsertThreadByMachine;
    }

    TLOGDEBUG("PropertyDbManager init insert DB threadnum:" << _dbIpNum  << endl);

    size_t iDbOneMachine = 999999;
    map<string, vector<size_t> >::iterator m_iter = _ipHasDbInfo.begin();
    while(m_iter != _ipHasDbInfo.end())
    {
        vector<size_t> &vDb = m_iter->second;

        if(vDb.size() < iDbOneMachine)
        {
            iDbOneMachine = vDb.size();
        }
        ++m_iter;
    }

    TLOGDEBUG("PropertyDbManager init iDbOneMachine:" << iDbOneMachine  << endl);

    if(iInsertThreadByDB > iDbOneMachine)
    {
        iInsertThreadByDB = iDbOneMachine;
    }

    _oneDbHasThreadNum = iInsertThreadByDB;

    TLOGDEBUG("PropertyDbManager init iInsertThreadByDB:" << iInsertThreadByDB  << endl);

    size_t iIndex = 0;
    map<string, pair<size_t, size_t> >::iterator map_it = mIp.begin();

    while(map_it != mIp.end())
    {
        map_it->second.first  = iIndex;
        map_it->second.second = 0;
        ++iIndex;

        if(iIndex == _dbIpNum)
        {
            iIndex = 0;
        }

        ++map_it;
    }

    for(size_t i = 0; i < vIp.size(); ++i)
    {
        map<string, pair<size_t, size_t> >::iterator iter = mIp.find(vIp[i]);
        if(iter != mIp.end())
        {
            _dbToIp.insert(map<size_t, size_t>::value_type(i, (iter->second.first * iInsertThreadByDB + iter->second.second)));

            iter->second.second += 1;

            if(iter->second.second == iInsertThreadByDB)
            {
                iter->second.second = 0;
            }
        }
        else
        {
            TLOGERROR("PropertyDbManager init DbToIp error ip:" << vIp[i] << endl);
        }
    }

    map<size_t, size_t>::iterator mDbToIpIter = _dbToIp.begin();
    while(mDbToIpIter != _dbToIp.end())
    {
        TLOGDEBUG("PropertyDbManager init index:" << mDbToIpIter->first << "|thread index:" << mDbToIpIter->second  << endl);
        ++mDbToIpIter;
    }

    map<string, vector<size_t> >::iterator m_iter1 = _ipHasDbInfo.begin();
    while(m_iter1 != _ipHasDbInfo.end())
    {
        vector<size_t> &vDb = m_iter1->second;

        FDLOG("PropertyPool") << "statsec ip:" << ServerConfig::LocalIp << "|ip:" << m_iter1->first << "|DbNum:" << vDb.size() << endl;

        ++m_iter1;
    }

    TLOGDEBUG("PropertyDbManager init ok." << endl);
}
///////////////////////////////////////////////////////////
PropertyDbManager::~PropertyDbManager()
{
    vector<TC_Mysql*>::iterator it = _ssdThreadMysql.begin();
    while(it != _ssdThreadMysql.end())
    {
        if(*it)
        {
            delete *it;
        }
        ++it;
    }
}
///////////////////////////////////////////////////////////
size_t PropertyDbManager::getDbToIpIndex(size_t iIndex)
{
    map<size_t, size_t>::const_iterator iter = _dbToIp.find(iIndex);
    if(iter != _dbToIp.end())
    {
        return iter->second;
    }
    return -1;
}
///////////////////////////////////////////////////////////
bool PropertyDbManager::hasEnableWeighted()
{
    return _enableWeighted;
}
///////////////////////////////////////////////////////////
string PropertyDbManager::getIpAndPort(size_t iDbIndex) 
{ 
    assert(iDbIndex < _ipAndPort.size());

    return _ipAndPort[iDbIndex];
}
///////////////////////////////////////////////////////////
void PropertyDbManager::getDbWeighted(int& iGcd,int& iMaxW,vector<int>& vDbWeighted)
{
    vDbWeighted = _dbWeighted;

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

    iGcd  = gcd;

    iMaxW = iMax;
}
///////////////////////////////////////////////////////////
int PropertyDbManager::getGcd (int a, int b)
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
int PropertyDbManager::insert2Db(const PropertyMsg &mPropMsg, const string &sDate, const string &sFlag, int iOldWriteNum, int &iNowWriteNum, const string &sTbNamePre, TC_Mysql *pMysql)
{
    int iCount = 0;
    int iHasWriteNum = iOldWriteNum;
    ostringstream osSql;
    string strValues;

    iNowWriteNum = 0;

    string strTbName   = (sTbNamePre !=""?sTbNamePre:_tbNamePre) + TC_Common::replace(sDate,"-","") + sFlag.substr(0,_cutType*2);
    try
    {
        creatTable(strTbName,pMysql);

        for(map<PropHead,PropBody>::const_iterator it = mPropMsg.begin(); it != mPropMsg.end(); it++ )
        {
            if(_terminate)
            {
                return -1;
            }

            if(iHasWriteNum > 0)
            {
                --iHasWriteNum;
                continue;
            }

            const PropHead& head = it->first;
            const PropBody& body = it->second;
            //组织sql语句
            for(size_t i = 0; i < body.vInfo.size(); i++)
            {
                strValues = " ('";
                strValues += sDate;
                strValues += "','";
                strValues += sFlag;
                strValues += "','";
                strValues += pMysql->escapeString(head.moduleName);
                strValues += "','";
                strValues += pMysql->escapeString(head.ip);
                strValues += "','";
                strValues += pMysql->escapeString(head.propertyName);
                strValues += "','";
                strValues += pMysql->escapeString(head.setName);
                strValues += "','";
                strValues += pMysql->escapeString(head.setArea);
                strValues += "','";
                strValues += pMysql->escapeString(head.setID);
                strValues += "','";
                strValues += pMysql->escapeString(body.vInfo[i].policy);
                strValues += "','";

                //如果是平均值
                if(body.vInfo[i].policy == "Avg")
                {
                    vector<string> sTmp = TC_Common::sepstr<string>(body.vInfo[i].value,"=");
                    double avg = 0;
                    if(2 == sTmp.size() && TC_Common::strto<long>(sTmp[1]) != 0)
                    {
                        avg = TC_Common::strto<double>(sTmp[0])/TC_Common::strto<long>(sTmp[1]);
                    }
                    else
                    {
                        avg = TC_Common::strto<double>(sTmp[0]);
                    }

                    
                    strValues += pMysql->escapeString(TC_Common::tostr(avg));
                    strValues += "') ";
                }
                else
                {
                    strValues += pMysql->escapeString(body.vInfo[i].value);
                    strValues += "') ";
                }

                if ( iCount == 0 )
                {
                    osSql.str("");
                    osSql << "insert ignore into " + strTbName + "  (f_date,f_tflag,master_name,master_ip,property_name,set_name,set_area,set_id,policy,value) values ";
                    osSql << strValues;
                }
                else
                {
                    osSql<< "," + strValues;
                }
                iNowWriteNum++;
                iCount ++;
            }
            if ( iCount >= _maxInsertCount )
            {
                usleep(100);
                pMysql->execute(osSql.str());    

                TLOGDEBUG("insert " << strTbName << " affected:" << iCount  << endl);

                FDLOG("PropertyPool") << "propertypool ip:" << ServerConfig::LocalIp << "|insert " << strTbName << "|insert affected:" << iCount << "|mysql affected:" << pMysql->getAffectedRows() << endl;

                iCount = 0;
            }
        }

        if ( iCount != 0 )
        {
            pMysql->execute(osSql.str());
            TLOGDEBUG("insert " << strTbName << " affected:" << iCount << endl);
            FDLOG("PropertyPool") << "statsec ip:" << ServerConfig::LocalIp << "|insert " << strTbName << "|insert affected:" << iCount << "|mysql affected:" << pMysql->getAffectedRows() << endl;
        }
    }
    catch (TC_Mysql_Exception& ex)
    {
        string err = string (ex.what());
        if (std::string::npos == err.find( "Duplicate"))
        {
            creatTable(strTbName,pMysql);
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
int PropertyDbManager::creatTable(const string &sTbName, TC_Mysql *_pstMql)
{
    string daytime = sTbName.substr(0, sTbName.length() -2 ); // 去掉小时
    int iRet = 0;
    int hour = 0;

    for (hour=0; hour < 24; hour++)
    {
        char buf[3];
        snprintf(buf,sizeof(buf),"%.2d",hour);
        string sName =     daytime + string(buf);
        iRet |= creatTb(sName,    _pstMql);
    }

    return iRet;
}
///////////////////////////////////////////////////////////
bool PropertyDbManager::hasdbTableExist(const string& sTbName,TC_Mysql *pMysql)
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

    }
    catch(TC_Mysql_Exception& ex)
    {
        TLOGERROR("exception: " << ex.what() << endl);
        return false;
    }
}
///////////////////////////////////////////////////////////
int PropertyDbManager::creatTb(const string &sTbName,TC_Mysql *pMysql)
{
    try
    {

        if (!hasdbTableExist(sTbName,pMysql))
        {
            string sSql = TC_Common::replace(_sql, "${TABLE}",sTbName);
            TLOGINFO(sSql << endl);
            pMysql->execute(sSql);
        }

        return 0;
    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR("PropertyDbManager::creatTb exception: " << ex.what() << endl);
        return 1;
    }
}
///////////////////////////////////////////////////////////
int PropertyDbManager::creatEscTb(const string &sTbName, const string& sSql , TC_Mysql *pMysql)
{
    try
    {
        if (!hasdbTableExist(sTbName,pMysql))
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
int PropertyDbManager::updateEcsStatus(const string &sLastTime,const string &sTbNamePre,TC_Mysql *pMysql)
{
    try
    {
        string sAppName   = sTbNamePre != ""?sTbNamePre:_tbNamePre;
        string sCondition = "where  appname='"+sAppName+"' and action =0";

        TC_Mysql::RECORD_DATA rd;
        rd["lasttime"] = make_pair(TC_Mysql::DB_STR, sLastTime);

        int iRet = pMysql->updateRecord("t_ecstatus", rd,sCondition);

        TLOGDEBUG("PropertyDbManager::updateEcsStatus iRet: " <<iRet <<"|"<<pMysql->getLastSQL()<< endl);
        if (iRet == 0 )
        {
            rd["appname"]       = make_pair(TC_Mysql::DB_STR, sAppName);
            rd["checkint"]      = make_pair(TC_Mysql::DB_INT, TC_Common::tostr(g_app.getInserInterv()));
            rd["lasttime"]      = make_pair(TC_Mysql::DB_STR, sLastTime);
            iRet = pMysql->replaceRecord("t_ecstatus", rd);
        }
        if (iRet != 1)
        {
            TLOGDEBUG("PropertyDbManager::updateEcsStatus erro: ret:" << iRet<<"\n"<<pMysql->getLastSQL()<< endl);
        }
    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR("PropertyDbManager::updateEcsStatus exception: " << ex.what() << endl);
        creatEscTb("t_ecstatus", _sqlStatus, pMysql);
        return 1;
    }
    return 0;
}
///////////////////////////////////////////////////////////
int PropertyDbManager::insert2MultiDbs(int iIndex, const PropertyMsg &propertymsg, const string &sDate, const string &sFlag)
{
    try
    {
        string sIp = ServerConfig::LocalIp;

        string sLastTime  = sDate + " " + sFlag;

        string sTbNamePre = "";
        TC_Mysql * pMysql = NULL;

        sTbNamePre = _sTbNamePre[iIndex];

        pMysql = _ssdThreadMysql[iIndex];

        if (checkLastTime(sLastTime,  sTbNamePre + sIp, pMysql) == 0)  //=0,没有记录表示: 数据库记录的时间，比当前当前时间小
        {
            TLOGDEBUG("begin insert to db " << getIpAndPort(iIndex) << endl);

            int iOldWrite = 0;
            int iNowWrite = 0;

            int64_t iBegin = tars::TC_TimeProvider::getInstance()->getNowMs();

            if(insert2Db(propertymsg, sDate, sFlag, iOldWrite, iNowWrite, sTbNamePre, pMysql) != 0)
            {
                if(_terminate)
                {
                    return -1;
                }

                iOldWrite = iNowWrite;
                iNowWrite = 0;

                if(insert2Db(propertymsg ,sDate, sFlag, iOldWrite, iNowWrite, sTbNamePre, pMysql) != 0 )
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

            TLOGDEBUG("insert|" << iIndex << "|" << getIpAndPort(iIndex) << "|" << sDate << "|" << sFlag << "|" << propertymsg.size() << "|" << (iEnd - iBegin) << endl);
            FDLOG("PropertyPool") << "propertypool ip:" << ServerConfig::LocalIp  << "|insert|dbIndex:" << iIndex << "|" << getIpAndPort(iIndex) << "|date:" << sDate << "|tflag:" << sFlag 
                << "|records:" << propertymsg.size() << "|timecost(ms):" << (iEnd - iBegin) << "|iBegin(ms):" << iBegin << "|iEnd(ms):" << iEnd << endl;
        }
    }
    catch(TC_Mysql_Exception& ex)
    {
        TLOGERROR("PropertyDbManager::insert2MultiDbs TC_Mysql_Exception: " << ex.what() << endl);
    }
    catch(exception& ex)
    {
        TLOGERROR("PropertyDbManager::insert2MultiDbs exception: " << ex.what() << endl);
    }

    return 0;
}
///////////////////////////////////////////////////////////
int PropertyDbManager::sendAlarmSMS(const string &sMsg)
{
    string errInfo = " ERROR:" + ServerConfig::LocalIp + "|" + sMsg +  ":统计入库失败，请及时处理!";
    TARS_NOTIFY_ERROR(errInfo);

    TLOGERROR("TARS_NOTIFY_ERROR " << errInfo << endl);

    return 0;
}
///////////////////////////////////////////////////////////
int PropertyDbManager::getDbNumber()
{
    return _dbNumber;
}
///////////////////////////////////////////////////////////
int PropertyDbManager::checkLastTime(const string &sLastTime,const string &sTbNamePre,TC_Mysql *pMysql)
{
    int iRet = 0;
    try
    {
        string sAppName   = sTbNamePre != "" ? sTbNamePre : _tbNamePre;
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
