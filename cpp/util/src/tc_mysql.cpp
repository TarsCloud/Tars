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

#include "util/tc_mysql.h"
#include "errmsg.h"
#include <sstream>
#include <string.h>

namespace tars
{

TC_Mysql::TC_Mysql()
:_bConnected(false)
{
    _pstMql = mysql_init(NULL);
}

TC_Mysql::TC_Mysql(const string& sHost, const string& sUser, const string& sPasswd, const string& sDatabase, const string &sCharSet, int port, int iFlag)
:_bConnected(false)
{
    init(sHost, sUser, sPasswd, sDatabase, sCharSet, port, iFlag);
    
    _pstMql = mysql_init(NULL);
}

TC_Mysql::TC_Mysql(const TC_DBConf& tcDBConf)
:_bConnected(false)
{
    _dbConf = tcDBConf;
    
    _pstMql = mysql_init(NULL);    
}

TC_Mysql::~TC_Mysql()
{
    if (_pstMql != NULL)
    {
        mysql_close(_pstMql);
        _pstMql = NULL;
    }
}

void TC_Mysql::init(const string& sHost, const string& sUser, const string& sPasswd, const string& sDatabase, const string &sCharSet, int port, int iFlag)
{
    _dbConf._host = sHost;
    _dbConf._user = sUser;
    _dbConf._password = sPasswd;
    _dbConf._database = sDatabase;
    _dbConf._charset  = sCharSet;
    _dbConf._port = port;
    _dbConf._flag = iFlag;
}

void TC_Mysql::init(const TC_DBConf& tcDBConf)
{
    _dbConf = tcDBConf;
}

void TC_Mysql::connect()
{
    disconnect();

    if( _pstMql == NULL)
    {
        _pstMql = mysql_init(NULL);
    }

    //建立连接后, 自动调用设置字符集语句
    if(!_dbConf._charset.empty())  {
        if (mysql_options(_pstMql, MYSQL_SET_CHARSET_NAME, _dbConf._charset.c_str())) {
            throw TC_Mysql_Exception(string("TC_Mysql::connect: mysql_options MYSQL_SET_CHARSET_NAME ") + _dbConf._charset + ":" + string(mysql_error(_pstMql)));
        }
    }

    if (mysql_real_connect(_pstMql, _dbConf._host.c_str(), _dbConf._user.c_str(), _dbConf._password.c_str(), _dbConf._database.c_str(), _dbConf._port, NULL, _dbConf._flag) == NULL) 
    {
        throw TC_Mysql_Exception("[TC_Mysql::connect]: mysql_real_connect: " + string(mysql_error(_pstMql)));
    }
    
    _bConnected = true;
}

void TC_Mysql::disconnect()
{
    if (_pstMql != NULL)
    {
        mysql_close(_pstMql);
        _pstMql = mysql_init(NULL);
    }
    
    _bConnected = false;    
}

string TC_Mysql::escapeString(const string& sFrom)
{
    if(!_bConnected)
    {
        connect();
    }

    string sTo;
    string::size_type iLen = sFrom.length() * 2 + 1;
    char *pTo = (char *)malloc(iLen);

    memset(pTo, 0x00, iLen);
    
    mysql_real_escape_string(_pstMql, pTo, sFrom.c_str(), sFrom.length());

    sTo = pTo;

    free(pTo);

    return sTo;
}

MYSQL *TC_Mysql::getMysql(void)
{
    return _pstMql;
}

string TC_Mysql::buildInsertSQL(const string &sTableName, const RECORD_DATA &mpColumns)
{
    ostringstream sColumnNames;
    ostringstream sColumnValues;
    
    map<string, pair<FT, string> >::const_iterator itEnd = mpColumns.end();

    for(map<string, pair<FT, string> >::const_iterator it = mpColumns.begin(); it != itEnd; ++it)
    {
        if (it == mpColumns.begin())
        {
            sColumnNames << "`" << it->first << "`";
            if(it->second.first == DB_INT)
            {
                sColumnValues << it->second.second;
            }
            else
            {
                sColumnValues << "'" << escapeString(it->second.second) << "'";
            }
        }
        else
        {
            sColumnNames << ",`" << it->first << "`";
            if(it->second.first == DB_INT)
            {
                sColumnValues << "," + it->second.second;
            }
            else
            {
                sColumnValues << ",'" + escapeString(it->second.second) << "'";
            }
        }
    }

    ostringstream os;
    os << "insert into " << sTableName << " (" << sColumnNames.str() << ") values (" << sColumnValues.str() << ")";
    return os.str();
}

string TC_Mysql::buildReplaceSQL(const string &sTableName, const RECORD_DATA &mpColumns)
{
    ostringstream sColumnNames;
    ostringstream sColumnValues;

    map<string, pair<FT, string> >::const_iterator itEnd = mpColumns.end();
    for(map<string, pair<FT, string> >::const_iterator it = mpColumns.begin(); it != itEnd; ++it)
    {
        if (it == mpColumns.begin())
        {
            sColumnNames << "`" << it->first << "`";
            if(it->second.first == DB_INT)
            {
                sColumnValues << it->second.second;
            }
            else
            {
                sColumnValues << "'" << escapeString(it->second.second) << "'";
            }
        }
        else
        {
            sColumnNames << ",`" << it->first << "`";
            if(it->second.first == DB_INT)
            {
                sColumnValues << "," + it->second.second;
            }
            else
            {
                sColumnValues << ",'" << escapeString(it->second.second) << "'";
            }
        }
    }

    ostringstream os;
    os << "replace into " << sTableName << " (" << sColumnNames.str() << ") values (" << sColumnValues.str() << ")";
    return os.str();
}

string TC_Mysql::buildUpdateSQL(const string &sTableName,const RECORD_DATA &mpColumns, const string &sWhereFilter)
{
    ostringstream sColumnNameValueSet;

    map<string, pair<FT, string> >::const_iterator itEnd = mpColumns.end();

    for(map<string, pair<FT, string> >::const_iterator it = mpColumns.begin(); it != itEnd; ++it)
    {
        if (it == mpColumns.begin())
        {
            sColumnNameValueSet << "`" << it->first << "`";
        }
        else
        {
            sColumnNameValueSet << ",`" << it->first << "`";
        }

        if(it->second.first == DB_INT)
        {
            sColumnNameValueSet << "= " << it->second.second;
        }
        else
        {
            sColumnNameValueSet << "= '" << escapeString(it->second.second) << "'";
        }
    }

    ostringstream os;
    os << "update " << sTableName << " set " << sColumnNameValueSet.str() << " " << sWhereFilter;

    return os.str();
}

string TC_Mysql::getVariables(const string &sName)
{
    string sql = "SHOW VARIABLES LIKE '" + sName + "'";

    MysqlData data = queryRecord(sql);
    if(data.size() == 0)
    {
        return "";
    }

    if(sName == data[0]["Variable_name"])
    {
        return data[0]["Value"];
    }

    return "";
}

void TC_Mysql::execute(const string& sSql)
{
    /**
    没有连上, 连接数据库
    */
    if(!_bConnected)
    {
        connect();
    }

    _sLastSql = sSql;

    int iRet = mysql_real_query(_pstMql, sSql.c_str(), sSql.length());
    if(iRet != 0)
    {
        /**
        自动重新连接
        */
        int iErrno = mysql_errno(_pstMql);
        if (iErrno == 2013 || iErrno == 2006)
        {
            connect();
            iRet = mysql_real_query(_pstMql, sSql.c_str(), sSql.length());
        }
    }

    if (iRet != 0)
    {
        throw TC_Mysql_Exception("[TC_Mysql::execute]: mysql_query: [ " + sSql+" ] :" + string(mysql_error(_pstMql)));    
    }
}

TC_Mysql::MysqlData TC_Mysql::queryRecord(const string& sSql)
{
    MysqlData   data;

    /**
    没有连上, 连接数据库
    */
    if(!_bConnected)
    {
        connect();
    }

    _sLastSql = sSql;

    int iRet = mysql_real_query(_pstMql, sSql.c_str(), sSql.length());
    if(iRet != 0)
    {
        /**
        自动重新连接
        */
        int iErrno = mysql_errno(_pstMql);
        if (iErrno == 2013 || iErrno == 2006)
        {
            connect();
            iRet = mysql_real_query(_pstMql, sSql.c_str(), sSql.length());
        }
    }

    if (iRet != 0)
    {
        throw TC_Mysql_Exception("[TC_Mysql::execute]: mysql_query: [ " + sSql+" ] :" + string(mysql_error(_pstMql)));    
    }
    
    MYSQL_RES *pstRes = mysql_store_result(_pstMql);

    if(pstRes == NULL)
    {
        throw TC_Mysql_Exception("[TC_Mysql::queryRecord]: mysql_store_result: " + sSql + " : " + string(mysql_error(_pstMql)));
    }
    
    vector<string> vtFields;
    MYSQL_FIELD *field;
    while((field = mysql_fetch_field(pstRes)))
    {
         vtFields.push_back(field->name);
    }

    map<string, string> mpRow;
    MYSQL_ROW stRow;
    
    while((stRow = mysql_fetch_row(pstRes)) != (MYSQL_ROW)NULL)
    {
        mpRow.clear();
        unsigned long * lengths = mysql_fetch_lengths(pstRes);
        for(size_t i = 0; i < vtFields.size(); i++)
        {
            if(stRow[i])
            {
                mpRow[vtFields[i]] = string(stRow[i], lengths[i]);
            }
            else
            {
                mpRow[vtFields[i]] = "";
            }
        }

        data.data().push_back(mpRow);
    }

    mysql_free_result(pstRes);

    return data;
}

size_t TC_Mysql::updateRecord(const string &sTableName, const RECORD_DATA &mpColumns, const string &sCondition)
{
    string sSql = buildUpdateSQL(sTableName, mpColumns, sCondition);
    execute(sSql);

    return mysql_affected_rows(_pstMql);
}

size_t TC_Mysql::insertRecord(const string &sTableName, const RECORD_DATA &mpColumns)
{
    string sSql = buildInsertSQL(sTableName, mpColumns);
    execute(sSql);

    return mysql_affected_rows(_pstMql);
}

size_t TC_Mysql::replaceRecord(const string &sTableName, const RECORD_DATA &mpColumns)
{
    string sSql = buildReplaceSQL(sTableName, mpColumns);
    execute(sSql);

    return mysql_affected_rows(_pstMql);
}

size_t TC_Mysql::deleteRecord(const string &sTableName, const string &sCondition)
{
    ostringstream sSql;
    sSql << "delete from " << sTableName << " " << sCondition;

    execute(sSql.str());

    return mysql_affected_rows(_pstMql);
}

size_t TC_Mysql::getRecordCount(const string& sTableName, const string &sCondition)
{
    ostringstream sSql;
    sSql << "select count(*) as num from " << sTableName << " " << sCondition;

    MysqlData data = queryRecord(sSql.str());

    long n = atol(data[0]["num"].c_str());

    return n;

}

size_t TC_Mysql::getSqlCount(const string &sCondition)
{
    ostringstream sSql;
    sSql << "select count(*) as num " << sCondition;

    MysqlData data = queryRecord(sSql.str());

    long n = atol(data[0]["num"].c_str());

    return n;
}

int TC_Mysql::getMaxValue(const string& sTableName, const string& sFieldName,const string &sCondition)
{
    ostringstream sSql;
    sSql << "select " << sFieldName << " as f from " << sTableName << " " << sCondition << " order by f desc limit 1";

    MysqlData data = queryRecord(sSql.str());

    int n = 0;
    
    if(data.size() == 0)
    {
        n = 0;
    }
    else
    {
        n = atol(data[0]["f"].c_str());
    }

    return n;
}

bool TC_Mysql::existRecord(const string& sql)
{
    return queryRecord(sql).size() > 0;
}

long TC_Mysql::lastInsertID()
{
    return mysql_insert_id(_pstMql);
}

size_t TC_Mysql::getAffectedRows()
{
    return mysql_affected_rows(_pstMql);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
TC_Mysql::MysqlRecord::MysqlRecord(const map<string, string> &record)
: _record(record)
{
}

const string& TC_Mysql::MysqlRecord::operator[](const string &s)
{
    map<string, string>::const_iterator it = _record.find(s);
    if(it == _record.end())
    {
        throw TC_Mysql_Exception("field '" + s + "' not exists.");
    }
    return it->second;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

vector<map<string, string> >& TC_Mysql::MysqlData::data()
{
    return _data;
}

size_t TC_Mysql::MysqlData::size()
{
    return _data.size();
}

TC_Mysql::MysqlRecord TC_Mysql::MysqlData::operator[](size_t i)
{
    return MysqlRecord(_data[i]);
}

}

