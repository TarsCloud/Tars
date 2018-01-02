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

#include "DbProxy.h"
#include <time.h>

///////////////////////////////////////////////////////////
string tFlagInc(const string& stflag);

string dateInc(const string& sDate);

void selectLastMinTime(const string& sUid, int iThread , const string& tbname, const TC_DBConf& tcDbInfo, string & ret, QueryParam &queryParam);

void query(int iThread, const TC_DBConf & conf, map<string,string>& mSqlPart, map<string, vector<Int64> > &result, string &sRes, QueryParam &queryParam);

DbProxy::DbProxy()
{
}

DbProxy::~DbProxy()
{
}

int DbProxy::createRespHead(const vector<string> &res, const string& sLasttime, string& result, bool bDbCountFlag)
{
    int iRet = 0;
    string sRes;

    //检查查询返回值，如果一个线程失败，就返回失败。
    for(size_t i=0; i< res.size(); i++)
    {
        sRes += res[i] ;
        if ( res[i][4] != '0' && iRet == 0)
        {
            iRet = -1;
        }
    }

    //int total = bDbCountFlag ? g_app.getDbNumber() : g_app.getDbNumber();
    int total = g_app.getDbNumber();
    result = "Ret:";
    result += TC_Common::tostr(iRet);
    result += "\nlasttime:";
    result += sLasttime;
    result += "\nActiveDb:";
    result += TC_Common::tostr(res.size());
    result += "\nTotalDb:";
    result += TC_Common::tostr(total);
    result += "\n";
    result += sRes;

    return iRet;
}
int DbProxy::createRespData(const string& sUid, const map<string,string>& mSqlPart, const vector<map<string, vector<Int64> > >& vDataList, const string& sHead,  string &result)
{
    // 组合多线程结果
    //map  first由goupby生成
    //map second 由index生成
    int64_t tStart = TNOWMS;
    vector<map<string, vector<Int64> > >::const_iterator dataItr = vDataList.begin();
    map<string, vector<Int64> > mStatData;
    map<string, vector<Int64> >::iterator _it;

    for(size_t i = 0; dataItr != vDataList.end(); dataItr++, i++)
    {
        TLOGINFO(sUid << "sum["<<i<<"].size"<< ":" << dataItr->size() << endl);
        for(map<string, vector<Int64> >::const_iterator it = dataItr->begin(); it != dataItr->end(); it++)
        {
            _it = mStatData.find(it->first);
            if (_it != mStatData.end())
            {
                const vector<Int64> &number1 = it->second;
                vector<Int64> &number2 = _it->second;
                // 相同key的值 求和,number1和number1的大小是一样的
                for (size_t j=0; j<number1.size(); j++)
                {
                    number2[j] +=  number1[j];
                }
            }
            else
            {
                mStatData[it->first] = it->second;
            }
        }
    }

    int iIndex = -1;
    size_t iGroupFieldSize = 0;
    bool bTars  = false;
    string sIsTars("");
    string groupField("");

    map<string,string>::const_iterator const_it = mSqlPart.find("istars");
    if(const_it != mSqlPart.end())
    {
        TLOGDEBUG("DbProxy::createRespData istars:" << const_it->second << endl);

        bTars = true;

        sIsTars = const_it->second;

        map<string,string>::const_iterator const_it_inner = mSqlPart.find("groupField");
        if(const_it_inner != mSqlPart.end())
        {
            groupField = const_it_inner->second;

            vector<string> vGroupField = TC_Common::sepstr<string>(groupField, ", ");

            iGroupFieldSize = vGroupField.size();
        
            for(size_t i = 0; i < vGroupField.size(); ++i)
            {
                if(vGroupField[i] == "slave_name")
                {
                    iIndex = i;
                    break;
                }
            }
        }

        if(iIndex < 0)
        {
            bTars = false;
        }
    }
    /*else
    {
        TLOGDEBUG("DbProxy::createRespData no set istars." << endl);
    }*/

    string sTemp("");
    int iLineNum = 0;

    //把 查询结果转换成一行一行的串
    /*
     * input :groupby, f_date, f_tflag
     * input : index, succ_count, timeout_count
     * all map <string, vector<Int64> >
     * string =>>  f_date, f_tflag
     * vector<Int64>  =>> succ_count, timeout_count
     */
    _it = mStatData.begin();
    while(_it != mStatData.end())
    {
        bool bFilter = false;
        if(bTars)
        {
            vector<string> vGroupField = TC_Common::sepstr<string>(_it->first, ",", true);

            if(sIsTars == "1")
            {
                set<string>::iterator it;
                it =  g_app.getNotTarsSlaveName().find(vGroupField[iIndex]);
                if(it != g_app.getNotTarsSlaveName().end())
                {
                    bFilter = true;
                }
            }
            else
            {
                if(vGroupField.size() != iGroupFieldSize)
                {
                    TLOGERROR("DbProxy::createRespData vGroupField.size:" << vGroupField.size() << "|iGroupFieldSize:" << iGroupFieldSize << "|key:" << _it->first << "|keyFiled:" << groupField << endl);
                }
                else if(vGroupField[iIndex] == "" || vGroupField[iIndex].size() == 0)
                {
                    TLOGERROR("DbProxy::createRespData iIndex:" << iIndex << "|vGroupField[iIndex]:" << vGroupField[iIndex] << endl);
                }

                set<string>::iterator it;
                it =  g_app.getNotTarsSlaveName().find(vGroupField[iIndex]);
                if(it == g_app.getNotTarsSlaveName().end())
                {
                    bFilter = true;
                }
            }

            if(!bFilter)
            {
                ++iLineNum;

                string valueBuffer = "";
                vector<Int64>::iterator valueIt = _it->second.begin();
                while(valueIt != _it->second.end()) // value is vector int, need transfer to string;
                {
                    valueBuffer += TC_Common::tostr(*valueIt);
                    valueBuffer += ",";
                    valueIt++;
                }

                sTemp += _it->first;
                sTemp += ",";
                sTemp += valueBuffer;
                sTemp += "\n";
            }
        }
        else
        {
            ++iLineNum;

            string valueBuffer = "";
            vector<Int64>::iterator valueIt = _it->second.begin();
            while(valueIt != _it->second.end()) // value is vector int, need transfer to string;
            {
                valueBuffer += TC_Common::tostr(*valueIt);
                valueBuffer += ",";
                valueIt++;
            }

            sTemp += _it->first;
            sTemp += ",";
            sTemp += valueBuffer;
            sTemp += "\n";
        }

        _it++;
    }

    result += sHead;
    result += "linecount:";
    result += TC_Common::tostr(iLineNum);
    result += "\n";
    result += sTemp;

    TLOGINFO("DbProxy::createRespData result:"<<result<<endl);

    int64_t tEnd = TNOWMS;

    TLOGDEBUG("DbProxy::createRespData "<< sUid << "createRespData size:"<< result.length() << "|timecost(ms):" << (tEnd-tStart) << endl);
    return 0;
}

/**
 * 通过线程池进行并发查询
 */
void DbProxy::queryData(map<string, string> &mSqlPart, string &sResult, bool bDbCountFlag)
{
    typedef TC_Functor<void, TL::TLMaker<int, const TC_DBConf &, map<string,string>&, map<string,vector<Int64> >&, string&, QueryParam &>::Result> QueryFun;
    typedef QueryFun::wrapper_type QueryFunWrapper;

    try
    {
        string sUid = mSqlPart.find("uid")->second;

        vector<TC_DBConf> vActive;

        if(bDbCountFlag)
        {
            vActive = g_app.getActiveDbInfo();
        }
        else
        {
            //vActive = g_app.getActiveDbInfo();
        }

        int iThreads = vActive.size();

        //int iThreads = bDbCountFlag ? g_app.getDbNumber() : g_app.getDbNumber();
        if(iThreads > 0)
        {
            vector<string> res(iThreads);

            vector<map<string, vector<Int64> > >  vDataList(iThreads);

            QueryFun qeryCMD(query);

            _queryParam._run_times = iThreads;

            //TLOGDEBUG("DbProxy::queryData sUid:" << sUid << "all thread query data begin." << endl);

            int64_t tStart    = TC_TimeProvider::getInstance()->getNowMs();

            for(int i=0; i < iThreads; i++)
            {
                QueryFunWrapper fwrapper(qeryCMD, i, vActive[i], mSqlPart, vDataList[i], res[i], _queryParam);

                if(bDbCountFlag)
                {
                    g_app.getThreadPoolDb().exec(fwrapper);
                }
                else
                {
                    //g_app.getThreadPoolDb().exec(fwrapper);                
                }
            }

            //等待线程结束
            TLOGDEBUG("DbProxy::queryData sUid:" << sUid << "wait for all thread query data done." << endl);

            bool rc = true;
            int ifail = 0;
            while(_queryParam._atomic.get() != _queryParam._run_times)
            {
                {
                    TC_ThreadLock::Lock lock(_queryParam._monitor);
                    rc = _queryParam._monitor.timedWait(6000);
                }

                ++ifail;

                if(!rc)
                {
                    if(ifail >= 10)
                    {
                        break;
                    }
                }
            }

            if(ifail >= 10)
            {
                TLOGDEBUG("DbProxy::queryData sUid:" << sUid << "wait for all thread query data timeout." << endl);
                while(_queryParam._atomic.get() != _queryParam._run_times)
                {
                    {
                        TC_ThreadLock::Lock lock(_queryParam._monitor);
                        _queryParam._monitor.timedWait(1000);
                    }
                }
            }

            if(_queryParam._atomic.get() == _queryParam._run_times)
                rc = true;
            /*bool rc = false;
            {
                TC_ThreadLock::Lock lock(_queryParam._monitor);
                rc = _queryParam._monitor.timedWait(60000);
            }*/
            
            int64_t tEnd = TC_TimeProvider::getInstance()->getNowMs();

            if(rc)
            {
                _queryParam._run_times = 0;
                _queryParam._run_result = 0;
                _queryParam._atomic = 0;

                TLOGDEBUG("DbProxy::queryData sUid:" << sUid << "all thread done return:" << _queryParam._run_result <<"|timecost(ms):" << (tEnd - tStart) << endl);

                // 返回ret code
                string sHead;

                string sLasttime = getLastTime(mSqlPart);

                if(createRespHead(res, sLasttime, sHead, bDbCountFlag) != 0)
                {
                    _queryParam._run_times = 0;
                    _queryParam._run_result = 0;
                    _queryParam._atomic = 0;

                    sResult = sHead;
                    TLOGERROR("DbProxy::queryData query error:" << sHead << endl);
                    return;
                }

                createRespData(sUid, mSqlPart, vDataList, sHead, sResult);
            }
            else
            {
                sResult ="Ret:-1\nquery timeout\n";

                TLOGDEBUG("DbProxy::queryData sUid:" << sUid << "Ret:-1|query timeout." << endl);
            }
        }
        else
        {
            sResult ="Ret:-1\nno active db\n";
        }
    }
    catch (exception &ex)
    {
        TLOGERROR("DbProxy::queryData exception:" << ex.what() << endl);
        sResult ="Ret:-1\n" + string(ex.what()) + "\n";        
    }

    _queryParam._run_times = 0;
    _queryParam._run_result = 0;
    _queryParam._atomic = 0;
}

void query(int iThread, const TC_DBConf & conf, map<string,string>& mSqlPart, map<string, vector<Int64> > &result, string &sRes, QueryParam &queryParam)
{
    string sUid = mSqlPart.find("uid")->second;

    TLOGDEBUG("queryData " << sUid << "thread iIndex:"  << iThread << endl);

    int64_t tStart = TNOWMS;
    try
    {
        //dateFrom =>> 20111120
          string dateFrom  = mSqlPart["date1"];
          string dateTo    = mSqlPart["date2"];

        //tflagFrom =>> 2360
          string tflagFrom = mSqlPart["tflag1"];
          string tflagTo   = mSqlPart["tflag2"];

        // 输入tflag 条件检查
        if (dateFrom.length() != 8 || dateTo.length() != 8  || tflagFrom.length()  != 4 ||
            tflagTo.length()              != 4     ||
            TC_Common::isdigit(tflagFrom) == false ||
            TC_Common::isdigit(tflagTo)   == false)
        {
            sRes += "ret:-1|iDb:" + TC_Common::tostr(iThread) + "|wrong tflag:" + tflagFrom + "-" + tflagTo +  "\n";

            TLOGERROR("query sUid:" << sUid << sRes << endl);

            queryParam._run_result = -1;
            queryParam._atomic.inc();

            if(queryParam._atomic.get() == queryParam._run_times)
            {
                TC_ThreadLock::Lock lock(queryParam._monitor);
                queryParam._monitor.notifyAll();
            }
            
            return ;
        }

        //groupCond =>> "where slave_name like 'MTTsh2.BrokerServer' and f_tflag >='0000'  and f_tflag <='2360'  and f_date = '20111120'"
          string whereCond = mSqlPart["whereCond"];

        //groupCond =>> "group by f_date, f_tflag"
          string groupCond = mSqlPart["groupCond"];

        //sumField    =>> "succ_count, timeout_count";
          string sumField  = mSqlPart["sumField"];

        //groupField  =>> "f_date, f_tflag"
          string groupField = mSqlPart["groupField"];

        //selectCond =>> "succ_count, timeout_count, f_date, f_tflag"
        string selectCond = sumField +"," + groupField;

        //日期格式20111019
        string::size_type pos = string::npos;
        string tmpDate = "f_date";
        if ((pos = selectCond.find(tmpDate, 0)) != string::npos)
        {
            selectCond.replace(pos, tmpDate.length(), "DATE_FORMAT( f_date, '%Y%m%d') as f_date");
        }

        string sDbName = mSqlPart["dataid"];
        string ignoreKey("");

        //不使用主键
        if(sDbName == "tars" || sDbName == "db_tarsstat")
        {
            ignoreKey = " IGNORE INDEX ( PRIMARY ) ";
        }

        vector<string> vGroupField = TC_Common::sepstr<string>(groupField, ", ");
        vector<string> vSumField = TC_Common::sepstr<string>(sumField, ", ");

        TC_Mysql tcMysql;

        TC_DBConf tcDbConf = conf;
        /*if(bFlag)
        {
            tcDbConf = g_app.getDbInfo(iThread);
        }
        else
        {
            tcDbConf = g_app.getDbInfo(iThread);
        }*/

        tcDbConf._database = sDbName;

        tcMysql.init(tcDbConf);

        string sTbNamePre = tcDbConf._database + "_";

        string sTbName("");
        string sSql("");
        //select range by f_date and f_tflag
        for(string day = dateFrom; day <= dateTo; day = dateInc(day))
        {
            for(string tflag = tflagFrom; tflag <= tflagTo && (tflag.substr(0,2) < "24"); tflag = tFlagInc(tflag))
            {
                //table name:tars_2012060723
                sTbName = sTbNamePre + day + tflag.substr(0,2);

                sSql = "select " + selectCond + " from " + sTbName + " " + ignoreKey  + whereCond  + groupCond + " order by null;";

                tars::TC_Mysql::MysqlData res = tcMysql.queryRecord(sSql);

                TLOGINFO(sUid << "res.size:" << res.size() << "|sSql:" << sSql << endl);

                // result is key:value pair;
                //sKey 由groupby生成
                //value由index生成
                //int64_t t2Start = TNOWMS;
                for(size_t    iRow = 0; iRow < res.size(); iRow++)
                {
                    string sKey = "";
                    for(size_t j = 0; j < vGroupField.size(); j++)
                    {
                        sKey += sKey.empty()?"":",";
                        sKey += res[iRow][vGroupField[j]];
                    }

                    map<string,vector<Int64> >::iterator itResult = result.find(sKey);
                    if (itResult != result.end())
                    {
                        vector<Int64>& data = itResult->second;
                        for (size_t j = 0; j < vSumField.size() && j < data.size(); j++)
                        {
                            data[j] += TC_Common::strto<Int64>(res[iRow][vSumField[j]]);// 相同key的值 求和
                        }
                    }
                    else
                    {
                        vector<Int64>& vRes = result[sKey];
                        for(size_t j = 0; j < vSumField.size(); j++)
                        {
                            vRes.push_back( TC_Common::strto<Int64>(res[iRow][vSumField[j]]));;
                        }
                    }
                    TLOGINFO("query iDb:" << iThread <<" {"<< sKey << ":" << TC_Common::tostr(result[sKey]) << "}" << endl);
                }

                TLOGINFO("query iDb :" << iThread << " day:" << day <<" tflag:" << tflag << endl);
            }
        }  //day

        sRes =  "ret:0 iDb:" + TC_Common::tostr(iThread)  + "\n";

        //queryParam._atomic.inc();
    }
    catch(TC_Mysql_Exception & ex)
    {
        sRes = "ret:-1|iDb:" + TC_Common::tostr(iThread) + string("|exception:") + ex.what() + "\n";
        TLOGERROR("query sUid:" << sUid << "query:" << sRes << endl);

        queryParam._run_result = -1;
        //queryParam._atomic.inc();
    }
    catch(exception & ex)
    {
        sRes = "ret:-1|iDb:" + TC_Common::tostr(iThread) + string("|exception:") + ex.what() + "\n";
        TLOGERROR("query sUid:" << sUid << "query:" << sRes << endl);

        queryParam._run_result = -1;
        //queryParam._atomic.inc();
    }
    int64_t tEnd = TNOWMS;

    TLOGDEBUG("query sUid:" << sUid << "exit query iDb:" << iThread <<"|timecost(ms):" << (tEnd - tStart) << "|res:" << sRes << endl);

    queryParam._atomic.inc();

    if(queryParam._atomic.get() == queryParam._run_times)
    {
        {
            TC_ThreadLock::Lock lock(queryParam._monitor);
            queryParam._monitor.notifyAll();
        }

        TLOGDEBUG("query sUid:" << sUid << "notify query finish." << endl);
    }

    
}

///////////////////////////////////////////////////////////////////////////////
string tFlagInc(const string& stflag)
{
    int h = TC_Common::strto<int>(stflag.substr(0,2));
    int m = TC_Common::strto<int>(stflag.substr(2,2));

    h += 1;
    char buf[5];
    snprintf(buf,sizeof(buf),"%.2d%.2d",h,m);

    return string(buf);
}
///////////////////////////////////////////////////////////////////////////////
string dateInc(const string& sDate)
{
    string ret("20991231"); // 返回大数
    
    try
    {
        int year = TC_Common::strto<int>(sDate.substr(0, 4));
        int mon  = TC_Common::strto<int>(sDate.substr(4, 2));
        int day  = TC_Common::strto<int>(sDate.substr(6, 2));

        struct tm *p = NULL;
        time_t timep;
        struct tm tt = {0};

        time(&timep);
        p=localtime_r(&timep, &tt);
        p->tm_mon  = mon -1;
        p->tm_mday = day +1;
        p->tm_year = year -1900 ;

        timep = mktime(p);
        ret = TC_Common::tm2str(timep, "%Y%m%d");
    }
    catch(exception & ex)
    {
        TLOGERROR("DbProxy::dateInc exception:" << ex.what() << endl);
    }
    return ret;
}
///////////////////////////////////////////////////////////////////////////////
void selectLastMinTime(const string& sUid, int iThread , const string& tbname, const TC_DBConf& tcDbInfo, string & ret, QueryParam &queryParam)
{
    string sId = sUid;
    try
    {
        TC_Mysql tcMysql;

        //TC_DBConf tcDbConf = tcDbInfo;

        //tcDbConf._database = TC_Common::trimright(tbname, "_");
        //tcDbConf._database = tbname;
        string sTbNamePre = tbname + ".t_ecstatus";

        //TLOGDEBUG("selectLastMinTime database name:" << tcDbConf._database << "|tbname:" << tbname << endl);

        tcMysql.init(tcDbInfo);

        int interval      = g_app.getInsertInterval();
        time_t now    = TC_TimeProvider::getInstance()->getNow();

        string sDate,sFlag;
        // 排除历史过期数据
        string sTime,sHour,sMinute;
        time_t t    = (now - interval * 60 * 2);
        interval    = g_app.getInsertInterval();
        t           = (t/(interval*60))*interval*60; //要求必须为loadIntev整数倍
        t           = (t%3600 == 0?t-60:t);                           //要求将9点写作0860
        sTime       = TC_Common::tm2str(t,"%Y%m%d%H%M");
        sDate       = sTime.substr(0,8);
        sHour       = sTime.substr(8,2);
        sMinute     = sTime.substr(10,2);
        sFlag       = sHour +  (sMinute=="59"?"60":sMinute);    //要求将9点写作0860

        string sLast = sDate + " " + sFlag;

        string sSql = "select min(lasttime) as lasttime  from "+ sTbNamePre+" where appname like '" +"%' and lasttime > '" + sLast + "'" ;

        tars::TC_Mysql::MysqlData res = tcMysql.queryRecord(sSql);

        if (res.size() > 0)
        {
            TLOGINFO("selectLastTime" << sId << "sSql:" << sSql << "|lasttime:" <<  res[0]["lasttime"] << endl);

            ret = res[0]["lasttime"];
        }
        else
        {
            ret = "";
        }
        //queryParam._atomic.inc();
    }
    catch(TC_Mysql_Exception & ex)
    {
        TLOGERROR("selectLastTime sUid="<< sId <<"exception:"<< ex.what() << endl);
        ret = "";
        queryParam._run_result = -1;
        //queryParam._atomic.inc();
    }
    catch(exception& e)
    {
        TLOGERROR("selectLastTime sUid="<< sId <<"exception:"<< e.what() << endl);
        ret = "";
        queryParam._run_result = -1;
        //queryParam._atomic.inc();
    }

    queryParam._atomic.inc();
    if(queryParam._atomic.get() == queryParam._run_times)
    {
        TC_ThreadLock::Lock lock(queryParam._monitor);
        queryParam._monitor.notifyAll();
        TLOGDEBUG("query sUid:" << sId << "notify checktime finish." << endl);
    }
}
///////////////////////////////////////////////////////////////////////////////
string DbProxy::getLastTime(const map<string,string>& mSqlPart)
{
    string sUid    = mSqlPart.find("uid")->second;

    string min = "99999999999"; // 求最小的，初始使用很大的数据
    //TLOGDEBUG("mSqlPart"<< mSqlPart.find("dataid")->second <<endl);
    try
    {
        typedef TC_Functor<void, TL::TLMaker<const string &, int, const string &, const TC_DBConf &, string &, QueryParam &>::Result> CheckTimeFun;
        typedef CheckTimeFun::wrapper_type CheckTimeFunWrapper;

        vector<TC_DBConf> vDbInfo = g_app.getAllActiveDbInfo();

        int iThreads = vDbInfo.size();

        if(iThreads > 0)
        {
            vector<string> res(iThreads);

            //vector<string> vsTbNamePre(iThreads);

            CheckTimeFun qeryCMD(selectLastMinTime);

            _queryParam._run_times = iThreads;

            int64_t tStart    = TC_TimeProvider::getInstance()->getNowMs();

            for (int i=0; i< iThreads; i++)
            {
                //vsTbNamePre[i] = vDbInfo[i]._database + "_";
                //vsTbNamePre[i] = vDbInfo[i]._database;

            //    TLOGDEBUG("mSqlPart"<< mSqlPart["dataid"] <<endl);
            //    CheckTimeFunWrapper fwrapper(qeryCMD, sUid, i, mSqlPart["dataid"], vDbInfo[i], res[i], _queryParam);
                CheckTimeFunWrapper fwrapper(qeryCMD, sUid, i,  mSqlPart.find("dataid")->second, vDbInfo[i], res[i], _queryParam);

                g_app.getThreadPoolTimeCheck().exec(fwrapper);
            }

            TLOGDEBUG("DbProxy::getLastTime sUid:" << sUid << "wait for getLastTime done." << endl);
            
            bool rc = true;
            int ifail = 0;
            while(_queryParam._atomic.get() != _queryParam._run_times)
            {
                {
                    TC_ThreadLock::Lock lock(_queryParam._monitor);
                    rc = _queryParam._monitor.timedWait(300);
                }

                ++ifail;

                if(!rc)
                {
                    if(ifail >= 10)
                    {
                        break;
                    }
                }
            }

            if(ifail >= 10)
            {
                TLOGDEBUG("DbProxy::getLastTime sUid:" << sUid << "wait for getLastTime timeout." << endl);
                while(_queryParam._atomic.get() != _queryParam._run_times)
                {
                    {
                        TC_ThreadLock::Lock lock(_queryParam._monitor);
                        _queryParam._monitor.timedWait(1000);
                    }
                }
            }

            if(_queryParam._atomic.get() == _queryParam._run_times)
                rc = true;
            /*bool rc = false;
            {
                TC_ThreadLock::Lock lock(_queryParam._monitor);
                rc = _queryParam._monitor.timedWait(3000);
            }*/

            int64_t tEnd = TC_TimeProvider::getInstance()->getNowMs();


            if(rc)
            {
                TLOGDEBUG("DbProxy::getLastTime sUid:" << sUid << "getLastTime all done|return:" << _queryParam._run_result <<"|timecost(ms):" << (tEnd-tStart) << endl);

                for(int i = 0; i < iThreads; ++i)
                {
                    if(res[i] < min)
                    {
                        min = res[i];
                    }
                }
            }
            else
            {
                min = "";
                TLOGDEBUG("DbProxy::getLastTime sUid:" << sUid << "getLastTime timeout." << endl);
            }
        }
        else
        {
            min = "";
        }

        TLOGDEBUG("DbProxy::getLastTime sUid:" << sUid << "final lasttime:" << min << endl);
    }
    catch (exception &ex)
    {
        TLOGERROR("DbProxy::getLastTime exception:" << ex.what() << endl);
        min = "";    
    }

    _queryParam._run_times = 0;
    _queryParam._run_result = 0;
    _queryParam._atomic = 0;

    return min;
}


string DbProxy::makeResult(int iRet, const string& sRes)
{
    size_t act    = g_app.getActiveDbInfo().size();
    int total     = g_app.getDbInfo().size();
    string result = "Ret:" + TC_Common::tostr(iRet) + "\n"
           + "ActiveDb:" + TC_Common::tostr(act) + "\n"
           + "TotalDb:" + TC_Common::tostr(total) + "\n"
           + sRes;
    return result;
}
