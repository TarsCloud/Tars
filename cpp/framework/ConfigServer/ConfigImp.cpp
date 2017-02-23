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

#include "ConfigImp.h"
#include "ConfigServer.h"

extern TC_Config * g_pconf;

#define CHECKLIMIT(a,b,c,f)  do{\
    if(IsLimited((a),(b),(c),(f)))\
    {\
        return 0;\
    }}while(0)
    
using namespace tars;

map<ServerKey,pair<time_t,int> > ConfigImp::_loadConfigLimited;

void ConfigImp::loadconf()
{
    TC_DBConf tcDBConf;
    tcDBConf.loadFromMap(g_pconf->getDomainMap("/tars/db"));
    _mysqlConfig.init(tcDBConf);

    _limitInterval = TC_Common::strto<int>(g_pconf->get("/tars/limit<limitinterval>", "10"));
    _limitInterval = _limitInterval <= 0 ? 10 : _limitInterval;
    _interval      = TC_Common::strto<int>(g_pconf->get("/tars/limit<interval>", "1"));
    _interval      = _interval <= 0 ? 1 : _interval;
}

void ConfigImp::initialize()
{
    loadconf();
}

int ConfigImp::ListConfig(const string &app, const string &server, vector<string> &vf,tars::TarsCurrentPtr current)
{
    try
    {
        string host =  current->getIp();

        TLOGDEBUG("ConfigImp::ListConfig app:" << app << "|server:" << server << "|host:" << host << endl);

       //查ip对应配置
        string sNULL("");
        string sSql =
            "select filename from t_config_files "
            "where server_name = '" + _mysqlConfig.escapeString(app+"."+server) + "' "
            "and host='"            + _mysqlConfig.escapeString(host) +"' "
            "and level="            + TC_Common::tostr<int>(eLevelIpServer) + " "
            "and set_name ='"       + _mysqlConfig.escapeString(sNULL) +"' "
            "and set_area ='"       + _mysqlConfig.escapeString(sNULL) +"' "
            "and set_group ='"      + _mysqlConfig.escapeString(sNULL) +"' ";

        TC_Mysql::MysqlData res = _mysqlConfig.queryRecord(sSql);

        TLOGDEBUG("ConfigImp::ListConfig sql:" << sSql << "|res.size:" << res.size() << endl);

        for(unsigned i=0; i<res.size();i++)
        {
            vf.push_back(res[i]["filename"]);
        }
    }
    catch(TC_Mysql_Exception & ex)
    {
        TLOGERROR("ConfigImp::ListConfig exception:" << ex.what() << endl);
        return -1;
    }
    catch(...)
    {
        TLOGERROR("ConfigImp::ListConfig unknown exception." << endl);
        return -1;
    }

    return 0;
}

bool ConfigImp::getSetInfo(string& sSetName, string& sSetArea, string& sSetGroup, const string& sSetDivision)
{
    vector<string> vtSetDivisions = TC_Common::sepstr<string>(sSetDivision, ".");
    if(vtSetDivisions.size() != 3)
    {
        return false;
    }

    sSetName  = vtSetDivisions[0];
    sSetArea  = vtSetDivisions[1];
    sSetGroup = vtSetDivisions[2];

    return true;
}

int ConfigImp::ListConfigByInfo(const ConfigInfo& configInfo, vector<string> &vf,tars::TarsCurrentPtr current)
{
    try
    {
        string sHost =  configInfo.host.empty() ? current->getIp() : configInfo.host;

        TLOGDEBUG("ConfigImp::ListConfigByInfo app:" << configInfo.appname << "|server:" << configInfo.servername << "|set:" << configInfo.setdivision << "|host:" << sHost << endl);

        string sCondition;
        if(!configInfo.setdivision.empty())
        {
            string sSetName,sSetArea,sSetGroup;
            if(getSetInfo(sSetName,sSetArea,sSetGroup,configInfo.setdivision))
            {
                sCondition += " and set_name='" +_mysqlConfig.escapeString(sSetName)+"' ";
                sCondition += " and set_area='" +_mysqlConfig.escapeString(sSetArea)+"' ";
                sCondition += " and set_group='"+_mysqlConfig.escapeString(sSetGroup)+"' ";
            }
            else
            {
                TLOGERROR("ConfigImp::ListConfigByInfo setdivision is invalid|setdivision:" << configInfo.setdivision << endl);
                return -1;
            }
        }
        else//兼容没有set信息的业务
        {
            string sNULL("");
            sCondition += " and set_name='" + _mysqlConfig.escapeString(sNULL)  +"' ";
            sCondition += " and set_area='" + _mysqlConfig.escapeString(sNULL)  +"' ";
            sCondition += " and set_group='" + _mysqlConfig.escapeString(sNULL) +"' ";
        }

        //查ip对应配置
        string sSql =
            "select filename from t_config_files "
            "where server_name = '" + _mysqlConfig.escapeString(configInfo.appname + "." + configInfo.servername) + "' "
            "and host='" + _mysqlConfig.escapeString(sHost) + "' "
            "and level=" + TC_Common::tostr<int>(eLevelIpServer) + sCondition;

        TC_Mysql::MysqlData res = _mysqlConfig.queryRecord(sSql);

        TLOGDEBUG("ConfigImp::ListConfigByInfo sql:" << sSql << "|res.size:" << res.size() << endl);

        for(unsigned i=0; i<res.size();i++)
        {
            vf.push_back(res[i]["filename"]);
        }
    }
    catch(TC_Mysql_Exception & ex)
    {
        TLOGERROR("ConfigImp::ListConfigByInfo exception:" << ex.what() << endl);
        return -1;
    }
    catch(...)
    {
        TLOGERROR("ConfigImp::ListConfigByInfo unknown exception." << endl);
        return -1;
    }

    return 0;
}

int ConfigImp::loadConfigByInfo(const ConfigInfo & configInfo,string &config,tars::TarsCurrentPtr current)
{
    int iRet = 0 ;

    TLOGDEBUG("ConfigImp::loadConfigByInfo app:" << configInfo.appname << "|server:" << configInfo.servername << "|filename:" << configInfo.filename << "|setdivision:" << configInfo.setdivision << endl);

    CHECKLIMIT(configInfo.appname,configInfo.servername,current->getIp(),configInfo.filename);
    
    if(configInfo.bAppOnly || configInfo.servername.empty())//应用级配置或者set级配置
    {
        iRet = loadAppConfigByInfo(configInfo,config,current);
    }
    else
    {
        iRet = loadConfigByHost(configInfo,config,current);
    }

    return iRet;
}

int ConfigImp::checkConfigByInfo(const ConfigInfo & configInfo, string &result,tars::TarsCurrentPtr current)
{
    TLOGDEBUG("ConfigImp::checkConfigByInfo app:" << configInfo.appname << "|server:" << configInfo.servername << "|filename:" << configInfo.filename << "|setdivsion:" << configInfo.setdivision << endl);

    int iRet = loadConfigByHost(configInfo,result,current);
    if (iRet != 0)
    {
        TLOGERROR("ConfigImp::checkConfigByInfo loadConfigByHost fail." << endl);
        return -1;
    }

    try
    {
        TC_Config conf;
        conf.parseString(result);
    }
    catch(exception &ex)
    {
        result = ex.what();
        TLOGERROR("ConfigImp::checkConfigByInfo exception:" << ex.what() << endl);
        return -1;
    }

    return 0;
}

int ConfigImp::loadConfig(const std::string& app, const std::string& server, const std::string& fileName, string &config, tars::TarsCurrentPtr current)
{
    TLOGDEBUG("ConfigImp::loadConfig app:" << app << "|server:" << server << "|fileName:" << fileName << "|host:" << current->getIp() << endl);

    CHECKLIMIT(app,server,current->getIp(),fileName);

    if(!server.empty())
    {
        return loadConfigByHost(app + "." + server, fileName, current->getIp(), config, current);
    }
    else
    {
        return loadAppConfig(app, fileName, config, current);
    }
}

int ConfigImp::loadConfigByHost(const std::string& appServerName, const std::string& fileName, const string &host, string &config, tars::TarsCurrentPtr current)
{
    TLOGDEBUG("ConfigImp::loadConfigByHost appServerName:" << appServerName << "|fileName:" << fileName << "|host:" << host << endl);

    int iRet = 0;
    config = "";

    //多配置文件的分割符
    string sSep = "\r\n\r\n";
    try
    {
        string sAllServerConfig("");
        int iAllConfigId = 0;

        //查公有配置
        string sNULL("");
        string sSql =
            "select id,config from t_config_files "
            "where server_name = '" + _mysqlConfig.escapeString(appServerName) + "' "
            "and filename='"   + _mysqlConfig.escapeString(fileName) + "' "
            "and level="       + TC_Common::tostr<int>(eLevelAllServer) + " "
            "and set_name ='"  + _mysqlConfig.escapeString(sNULL) + "' "
            "and set_area ='"  + _mysqlConfig.escapeString(sNULL) + "' "
            "and set_group ='" + _mysqlConfig.escapeString(sNULL) + "' ";

        TC_Mysql::MysqlData res = _mysqlConfig.queryRecord(sSql);

        TLOGDEBUG("ConfigImp::loadConfigByHost sql:" << sSql << "|res.size:" << res.size() << endl);

        if(res.size() == 1)
        {
            sAllServerConfig = res[0]["config"];
            iAllConfigId     = TC_Common::strto<int>(res[0]["id"]);
        }

        vector<int> referenceIds;

        iRet = getReferenceIds(iAllConfigId,referenceIds);

        TLOGDEBUG("ConfigImp::loadConfigByHost referenceIds.size:" << referenceIds.size() << endl);

        if(iRet == 0)
        {
            for(size_t i = 0; i < referenceIds.size();i++)
            {
                int iRefId = referenceIds[i];
                string refConfig("");

                iRet = loadConfigByPK(iRefId,refConfig);
                if(iRet == 0 && (!refConfig.empty()))
                {
                    if(config.empty())
                    {
                        config += refConfig;
                    }
                    else
                    {
                        config += sSep + refConfig;
                    }
                }
            }
        }

        //添加配置本身
        if(!sAllServerConfig.empty())
        {
            if(config.empty())
            {
                config += sAllServerConfig;
            }
            else
            {
                config += sSep + sAllServerConfig;
            }
        }


        string sIpServerConfig = "";
        int iIpConfigId = 0;

        sSql =
            "select id,config from t_config_files "
            "where server_name = '" + _mysqlConfig.escapeString(appServerName) + "' "
            "and filename='"   + _mysqlConfig.escapeString(fileName) + "' "
            "and host='"       + _mysqlConfig.escapeString(host)  + "' "
            "and level="       + TC_Common::tostr<int>(eLevelIpServer) + " "
            "and set_name ='"  + _mysqlConfig.escapeString(sNULL) + "' "
            "and set_area ='"  + _mysqlConfig.escapeString(sNULL) + "' "
            "and set_group ='" + _mysqlConfig.escapeString(sNULL) + "' ";

        res = _mysqlConfig.queryRecord(sSql);

        TLOGDEBUG("ConfigImp::loadConfigByHost sql:" << sSql << "|res.size:" << res.size() << endl);

        if(res.size() == 1)
        {
            sIpServerConfig = res[0]["config"];
            iIpConfigId = TC_Common::strto<int>(res[0]["id"]);
        }

        iRet = getReferenceIds(iIpConfigId,referenceIds);

        TLOGDEBUG("ConfigImp::loadConfigByHost referenceIds.size:" << referenceIds.size() << endl);

        if(iRet == 0)
        {
            for(size_t i = 0; i < referenceIds.size(); i++)
            {
                int iRefId = referenceIds[i];
                string refConfig("");

                iRet = loadConfigByPK(iRefId,refConfig);
                if(iRet == 0 && (!refConfig.empty()) )
                {
                    if(config.empty())
                    {
                        config += refConfig;
                    }
                    else
                    {
                        config += sSep + refConfig;
                    }
                }
            }
        }

        //添加配置本身
        if(!sIpServerConfig.empty())
        {
            if(config.empty())
            {
                config += sIpServerConfig;
            }
            else
            {
                config += sSep + sIpServerConfig;
            }
        }
    }
    catch(TC_Mysql_Exception & ex)
    {
        config = ex.what();
        TLOGERROR("ConfigImp::loadConfigByHost exception:" << ex.what() << endl);
        iRet = -1;
    }
    catch(...)
    {
        TLOGERROR("ConfigImp::loadConfigByHost unknown exception." << endl);
        iRet = -1;
    }

    return iRet;
}

int ConfigImp::checkConfig(const std::string& appServerName, const std::string& fileName, const string &host, string &result, tars::TarsCurrentPtr current)
{
    TLOGDEBUG("ConfigImp::checkConfig appServerName:" << appServerName << "|fileName:" << fileName << "|host:" << host << endl);

    int ret = loadConfigByHost(appServerName, fileName, host, result, current);
    if (ret != 0)
    {
        TLOGDEBUG("ConfigImp::checkConfig loadConfigByHost fail." << endl);
        return -1;
    }

    try
    {
        TC_Config conf;
        conf.parseString(result);
    }
    catch(exception &ex)
    {
        TLOGDEBUG("ConfigImp::checkConfig exception:" << ex.what() << endl);
        result = ex.what();
        return -1;
    }

    return 0;
}

///////////////////////////////////////////////private
int ConfigImp::loadConfigByPK(int iConfigId, string &sConfig)
{
    int iRet = 0 ;
    //按照建立引用的先后返回
    try
    {
        string sSql =
            "select config from t_config_files where id=" + TC_Common::tostr<int>(iConfigId) +" order by id";

        TC_Mysql::MysqlData res = _mysqlConfig.queryRecord(sSql);

        TLOGDEBUG("ConfigImp::loadConfigByPK sql:" << sSql << "|res.size:" << res.size() << endl);

        if(res.size() == 1)
        {
            sConfig = res[0]["config"];
        }
    }
    catch(TC_Mysql_Exception & ex)
    {
        sConfig = ex.what();
        TLOGERROR("ConfigImp::loadConfigByPK exception:" << ex.what() << endl);
        return -1;
    }
    catch(...)
    {
        TLOGERROR("ConfigImp::loadConfigByPK unknown exception." << endl);
        return -1;
    }

    return iRet;
}

int ConfigImp::loadRefConfigByPK(int iConfigId, const string& setdivision,string &sConfig)
{
    int iRet = 0 ;
    //按照建立引用的先后返回
    try
    {
        string sSql =
            "select server_name,filename,config from t_config_files where id=" + TC_Common::tostr<int>(iConfigId) +" order by id";

        TC_Mysql::MysqlData res = _mysqlConfig.queryRecord(sSql);

        TLOGDEBUG("ConfigImp::loadRefConfigByPK sql:" << sSql << "|res.size:" << res.size() << endl);

        if(res.size() == 1)
        {
            string sAppConfig  = res[0]["config"];
            string sFileName   = res[0]["filename"];
            string sServerName = res[0]["server_name"];

            string sSetName("");
            string sSetArea("");
            string sSetGroup("");
            string sSetConfig("");

            //查询该配置是否有set配置信息
            if(getSetInfo(sSetName,sSetArea,sSetGroup,setdivision))
            {
                string sCondition;
                sCondition += " and set_name='" +_mysqlConfig.escapeString(sSetName)+"' ";
                sCondition += " and set_area='" +_mysqlConfig.escapeString(sSetArea)+"' ";
                sCondition += " and set_group='" +_mysqlConfig.escapeString(sSetGroup)+"' ";

                string sSql =
                    "select config from t_config_files "
                    "where server_name = '" + _mysqlConfig.escapeString(sServerName) + "' "
                    "and filename='" + _mysqlConfig.escapeString(sFileName) + "' "
                    "and level=" + TC_Common::tostr<int>(eLevelApp) + sCondition;

                TC_Mysql::MysqlData resSet = _mysqlConfig.queryRecord(sSql);

                TLOGDEBUG("ConfigImp::loadRefConfigByPK sql:" << sSql << "|resSet.size:" << resSet.size() << endl);

                if(resSet.size() == 1)
                {
                    sSetConfig = resSet[0]["config"];
                }
            }

            sConfig = mergeConfig(sAppConfig,sSetConfig);
        }
    }
    catch(TC_Mysql_Exception & ex)
    {
        sConfig = ex.what();
        TLOGERROR("ConfigImp::loadRefConfigByPK exception: "<<ex.what()<<endl);
        return -1;
    }
    catch(...)
    {
        TLOGERROR("ConfigImp::loadRefConfigByPK unknown exception." << endl);
        return -1;
    }

    return iRet;
}

int ConfigImp::getReferenceIds(int iConfigId,vector<int> &referenceIds)
{
    int iRet = 0 ;

    referenceIds.clear();

    string sSql =
        "select reference_id from t_config_references where config_id=" + TC_Common::tostr<int>(iConfigId);

    TLOGDEBUG("ConfigImp::getReferenceIds sql:" << sSql << endl);

    try
    {
        TC_Mysql::MysqlData res = _mysqlConfig.queryRecord(sSql);

        TLOGDEBUG("ConfigImp::getReferenceIds res.size:" << res.size() << endl);

        for(size_t i = 0 ; i < res.size(); i++)
        {
            int iRefId = TC_Common::strto<int>(res[i]["reference_id"]);
            referenceIds.push_back(iRefId);
        }
    }
    catch(TC_Mysql_Exception & ex)
    {
        TLOGERROR("ConfigImp::getReferenceIds exception:" << ex.what() << endl);
        return -1;
    }
    catch(...)
    {
        TLOGERROR("ConfigImp::getReferenceIds unknown exception" << endl);
        return -1;
    }

    return iRet;
}


int ConfigImp::loadAppConfig(const std::string& appName, const std::string& fileName, string &config, tars::TarsCurrentPtr current)
{
    TLOGDEBUG("ConfigImp::loadAppConfig appName:" << appName << "|fileName:" << fileName << endl);

    int iRet = 0 ;
    config = "";

    try
    {
        string sNULL;
        //查公有配置
        string sSql =
            "select id,config from t_config_files "
            "where server_name = '" + _mysqlConfig.escapeString(appName) + "' "
            "and filename='"        + _mysqlConfig.escapeString(fileName) + "' "
            "and level="            + TC_Common::tostr<int>(eLevelApp) + " "
            "and set_name ='"       + _mysqlConfig.escapeString(sNULL) + "' "
            "and set_area ='"       + _mysqlConfig.escapeString(sNULL) + "' "
            "and set_group ='"      + _mysqlConfig.escapeString(sNULL) + "' ";

        TC_Mysql::MysqlData res = _mysqlConfig.queryRecord(sSql);

        TLOGDEBUG("ConfigImp::loadAppConfig sql:" << sSql << "|res.size:" << res.size() << endl);

        if(res.size() == 1)
        {
            config = res[0]["config"];
        }
    }
    catch(TC_Mysql_Exception & ex)
    {
        config = ex.what();
        TLOGERROR("ConfigImp::loadAppConfig exception:" << ex.what() << endl);
        iRet = -1;
    }
    catch(...)
    {
        TLOGERROR("ConfigImp::loadAppConfig unknown exception." << endl);
        iRet = -1;
    }

    return iRet;

}

int ConfigImp::loadConfigByHost(const ConfigInfo & configInfo, string &config, tars::TarsCurrentPtr current)
{
    string sHost =  configInfo.host.empty() ? current->getIp() : configInfo.host;

    TLOGDEBUG("ConfigImp::loadConfigByHost app:" << configInfo.appname << "|server:" << configInfo.servername << "|filename:" << configInfo.filename
        << "|host:" << sHost << "|setdivision:"    <<configInfo.setdivision << endl);

    int iRet = 0 ;
    config = "";
    string sTemp = "";

    //多配置文件的分割符
    string sSep = "\r\n\r\n";
    try
    {
        string sCondition;
        if(!configInfo.setdivision.empty())
        {
            string sSetName,sSetArea,sSetGroup;
            if(getSetInfo(sSetName,sSetArea,sSetGroup,configInfo.setdivision))
            {
                sCondition += " and set_name='" +_mysqlConfig.escapeString(sSetName)+"' ";
                sCondition += " and set_area='" +_mysqlConfig.escapeString(sSetArea)+"' ";
                sCondition += " and set_group='" +_mysqlConfig.escapeString(sSetGroup)+"' ";
            }
            else
            {
                TLOGERROR("ConfigImp::loadConfigByHost setdivision is invalid|setdivision:" << configInfo.setdivision << endl);
                return -1;
            }
        }
        else//兼容没有set信息的业务
        {
            string sNULL;
            sCondition += " and set_name='" + _mysqlConfig.escapeString(sNULL) + "' ";
            sCondition += " and set_area='" + _mysqlConfig.escapeString(sNULL) + "' ";
            sCondition += " and set_group='" + _mysqlConfig.escapeString(sNULL) + "' ";
        }

        string sAllServerConfig("");
        int iAllConfigId = 0;

        //查服务配置
        string sSql =
            "select id,config from t_config_files "
            "where server_name = '" + _mysqlConfig.escapeString(configInfo.appname+"."+configInfo.servername) + "' "
            "and filename='" + _mysqlConfig.escapeString(configInfo.filename) + "' "
            "and level="     + TC_Common::tostr<int>(eLevelAllServer) + sCondition;

        TC_Mysql::MysqlData res = _mysqlConfig.queryRecord(sSql);

        TLOGDEBUG("ConfigImp::loadConfigByHost sql:" << sSql << "|res.size:" << res.size() << endl);

        if(res.size() == 1)
        {
            sAllServerConfig = res[0]["config"];
            iAllConfigId = TC_Common::strto<int>(res[0]["id"]);
        }

        //查服务配置的引用配置
        vector<int> referenceIds;
        iRet = getReferenceIds(iAllConfigId,referenceIds);

        TLOGDEBUG("ConfigImp::loadConfigByHost referenceIds.size:" << referenceIds.size() << endl);

        if(iRet == 0)
        {
            for(size_t i = 0; i < referenceIds.size();i++)
            {
                int iRefId = referenceIds[i];
                string refConfig("");

                //此处获取的服务配置的引用配置信息
                iRet = loadRefConfigByPK(iRefId,configInfo.setdivision,refConfig);
                if(iRet == 0 && (!refConfig.empty()))
                {
                    sTemp = mergeConfig(sTemp,refConfig);
                }
            }
        }

        //添加服务配置本身
        config = mergeConfig(sTemp,sAllServerConfig);

        sTemp.clear();

        string sIpServerConfig("");
        int iIpConfigId = 0;

        //查看节点级配置
        sSql =
            "select id,config from t_config_files "
            "where server_name = '" + _mysqlConfig.escapeString(configInfo.appname + "." + configInfo.servername) + "' "
            "and filename='" + _mysqlConfig.escapeString(configInfo.filename) + "' "
            "and host='" + _mysqlConfig.escapeString(sHost) + "' "
            "and level=" + TC_Common::tostr<int>(eLevelIpServer);

        res = _mysqlConfig.queryRecord(sSql);

        TLOGDEBUG("ConfigImp::loadConfigByHost sql:" << sSql<< "|res.size:" << res.size() << endl);

        if(res.size() == 1)
        {
            sIpServerConfig = res[0]["config"];
            iIpConfigId = TC_Common::strto<int>(res[0]["id"]);
        }

        iRet = getReferenceIds(iIpConfigId, referenceIds);

        TLOGDEBUG("ConfigImp::loadConfigByHost referenceIds.size:" << referenceIds.size() << endl);

        if(iRet == 0)
        {
            string refAppConfig("");
            string refSetConfig("");
            for(size_t i = 0; i < referenceIds.size();i++)
            {
                int iRefId = referenceIds[i];
                string refConfig("");

                //此处获取的节点配置的引用配置信息
                iRet = loadRefConfigByPK(iRefId,configInfo.setdivision,refConfig);
                if(iRet == 0 && (!refConfig.empty()))
                {
                    sTemp = mergeConfig(sTemp,refConfig);
                }
            }
        }

        config = mergeConfig(config,mergeConfig(sTemp, sIpServerConfig));

    }
    catch(TC_Mysql_Exception & ex)
    {
        config = ex.what();
        TLOGERROR("ConfigImp::loadConfigByHost exception:" << ex.what() << endl);
        iRet = -1;
    }
    catch(...)
    {
        TLOGERROR("ConfigImp::loadConfigByHost unknown exception" << endl);
        iRet = -1;
    }

    return iRet;
}

int ConfigImp::loadAppConfigByInfo(const ConfigInfo & configInfo, string &config, tars::TarsCurrentPtr current)
{
    int iRet = 0;
    config="";
    try
    {
        string sSql =
            "select id,config from t_config_files "
            "where server_name = '" + _mysqlConfig.escapeString(configInfo.appname) + "' "
            "and filename='" + _mysqlConfig.escapeString(configInfo.filename) + "' "
            "and level=" + TC_Common::tostr<int>(eLevelApp);

        string sNULL("");
        string sCondition("");
        sCondition += " and set_name='" + _mysqlConfig.escapeString(sNULL) +"' ";
        sCondition += " and set_area='" + _mysqlConfig.escapeString(sNULL) +"' ";
        sCondition += " and set_group='" + _mysqlConfig.escapeString(sNULL) +"' ";

        //先获取app配置
        string sSqlAppNoSet = sSql + sCondition;

        TC_Mysql::MysqlData res = _mysqlConfig.queryRecord(sSqlAppNoSet);

        TLOGDEBUG("ConfigImp::loadAppConfigByInfo sSqlAppNoSet:" << sSqlAppNoSet << "|res.size:" << res.size() << endl);

        string sAppConfig("");

        if(res.size() == 1)
        {
            sAppConfig = res[0]["config"];
        }

        //再获取app下有set信息的配置
        string sSetConfig;
        if(!configInfo.setdivision.empty())//存在set信息
        {
            string sSetName,sSetArea,sSetGroup;

            if(getSetInfo(sSetName,sSetArea,sSetGroup,configInfo.setdivision))
            {
                string sCondition;
                sCondition += " and set_name='" +_mysqlConfig.escapeString(sSetName)+"' ";
                sCondition += " and set_area='" +_mysqlConfig.escapeString(sSetArea)+"' ";
                sCondition += " and set_group='" +_mysqlConfig.escapeString(sSetGroup)+"' ";

                string sSqlSet = sSql + sCondition;
                res = _mysqlConfig.queryRecord(sSqlSet);

                TLOGDEBUG("ConfigImp::loadAppConfigByInfo sSqlSet:" << sSqlSet << "|res.size:" << res.size() << endl);

                if(res.size() == 1)
                {
                    sSetConfig = res[0]["config"];
                }
            }
            else
            {
                TLOGERROR("ConfigImp::loadAppConfigByInfo setdivision is invalid|configInfo.setdivision:" << configInfo.setdivision << endl);
                return -1;
            }
        }

        config = mergeConfig(sAppConfig,sSetConfig);

    }
    catch(TC_Mysql_Exception & ex)
    {
        config = ex.what();
        TLOGERROR("ConfigImp::loadAppConfigByInfo exception:" << ex.what() << endl);
        iRet = -1;
    }
    catch(...)
    {
        TLOGERROR("ConfigImp::loadAppConfigByInfo unknown exception" << endl);
        iRet = -1;
    }

    return iRet;
}

string ConfigImp::mergeConfig(const string& sLowConf,const string& sHighConf)
{

    if(sLowConf.empty())
    {
        return  sHighConf;
    }
    else if(sHighConf.empty())
    {
        return  sLowConf;
    }
    else
    {
        return sLowConf + "\r\n\r\n" + sHighConf;
    }
}

//频率限制
bool ConfigImp::IsLimited(const std::string & app, const std::string & server, const std::string & sIp,const string& sFile)
{
    const time_t INTERVAL = 1 * 60;
    bool bLimited = false;

    {
        ServerKey sKey = {app, server, sIp,sFile};

        static TC_ThreadLock s_mutex;

        TC_ThreadLock::Lock lock(s_mutex);

        map<ServerKey,pair<time_t,int> >::iterator it = _loadConfigLimited.find(sKey);
        if(it != _loadConfigLimited.end())
        {
            ///1分钟内最多允许拉取10次
            if((TNOW - it->second.first) <= (INTERVAL*_interval))
            {
                if(it->second.second <= _limitInterval)
                {
                    it->second.second++;
                }
                else
                {
                  bLimited = true;
                  TLOGERROR("ConfigImp::IsLimited app:" << app << "|server:" << server << "|sIp:" << sIp << "|sFile:" << sFile 
                      << "|out of limit,now:" << it->second.second << "|limitInterval:" << _limitInterval << endl);
                }
            }
            //已经超过1分钟了
            else
            {
                _loadConfigLimited.erase(sKey);
            }
        }
        else
        {
            _loadConfigLimited.insert(std::make_pair(sKey,std::make_pair(TNOW,1)));
        }
    }

    return bLimited;
}

int ConfigImp::ListAllConfigByInfo(const tars::GetConfigListInfo & configInfo, vector<std::string> &vf, tars::TarsCurrentPtr current)
{
	CHECKLIMIT(configInfo.appname,configInfo.servername,current->getIp(),"");

	try
	{
		if(configInfo.bAppOnly)
		{
			//查ip对应配置
			string sSql = "select distinct filename from t_config_files "
				"where server_name = '" + _mysqlConfig.escapeString(configInfo.appname) + "' "
				"and level=" + TC_Common::tostr<int>(eLevelApp);

            TLOGDEBUG("ConfigImp::ListAllConfigByInfo sql:" << sSql << endl);

			TC_Mysql::MysqlData res = _mysqlConfig.queryRecord(sSql);

            TLOGDEBUG("ConfigImp::ListAllConfigByInfo sql:" << sSql << "|res:" << res.size() << endl);

			for(unsigned i=0; i<res.size();i++)
			{
				vf.push_back(res[i]["filename"]);
			}

			return 0;
		}
		else
		{
			string sHost = configInfo.host;
			string sContainer = configInfo.containername;
			string sSql = "select distinct filename from t_config_files where server_name = '" + _mysqlConfig.escapeString(configInfo.appname+"."+configInfo.servername) + "' ";
			
			string sCondition("");
			if(!sHost.empty())
			{
				sCondition += " and host='"+_mysqlConfig.escapeString(sHost)+"' ";
				sCondition += " and level=" + TC_Common::tostr<int>(eLevelIpServer);
			}
			
			if(!configInfo.setdivision.empty())
			{
				string sSetName,sSetArea,sSetGroup;
				if(getSetInfo(sSetName,sSetArea,sSetGroup,configInfo.setdivision))
				{
					sCondition += " and set_name='" +_mysqlConfig.escapeString(sSetName)+"' ";
					sCondition += " and set_area='" +_mysqlConfig.escapeString(sSetArea)+"' ";
					sCondition += " and set_group='" +_mysqlConfig.escapeString(sSetGroup)+"' ";
				}
				else
				{
					TLOGERROR("ConfigImp::ListAllConfigByInfo setdivision is invalid:" << configInfo.setdivision << endl);
					return -1;
				}
			}
			else//兼容没有set信息的业务
			{
				string sNULL;
				sCondition += " and set_name='" + _mysqlConfig.escapeString(sNULL) +"' ";
				sCondition += " and set_area='" + _mysqlConfig.escapeString(sNULL) +"' ";
				sCondition += " and set_group='" + _mysqlConfig.escapeString(sNULL) +"' ";
			}

			sSql += sCondition;

            TLOGDEBUG("ConfigImp::ListAllConfigByInfo sql:" << sSql << endl);

			TC_Mysql::MysqlData res = _mysqlConfig.queryRecord(sSql);

            TLOGDEBUG("ConfigImp::ListAllConfigByInfo sql:" << sSql << "|res:" << res.size() << endl);

			for(unsigned i=0; i<res.size();i++)
			{
				vf.push_back(res[i]["filename"]);
			}
		}
	}
	catch(TC_Mysql_Exception & ex)
	{
		TLOGERROR("ConfigImp::ListAllConfigByInfo mysql exception:" << ex.what() << endl);
		return -1;
	}
    catch(exception& ex)
	{
		TLOGERROR("ConfigImp::ListAllConfigByInfo exception:" << ex.what() << endl);
		return -1;
	}

	return 0;
}
