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

#include "LogServer.h"
#include "util/tc_logger.h"
#include "LogImp.h"

void LogServer::initialize()
{
    string s;
    loadLogFormat("","",s);

    //日志路径
    g_globe._log_path = _conf["/tars/log<logpath>"];

    //启动写线程
    g_globe._group.start(TC_Common::strto<size_t>(_conf["/tars/log<logthread>"]));

    string prefix = TC_Common::lower(_conf.get("/tars/log<ipfix>","true"));
    g_globe._bIpPrefix = (prefix == "true") ? true : false;

    //增加对象
    addServant<LogImp>(ServerConfig::Application + "." + ServerConfig::ServerName +".LogObj");

    TARS_ADD_ADMIN_CMD_NORMAL("reloadLogFormat", LogServer::loadLogFormat);
}

bool LogServer::loadLogFormat(const string& command, const string& params, string& result)
{
    TLOGDEBUG("LogServer::loadLogFormat command:" << command << "|params:" << params << endl);

    try
    {
        TC_Config conf;

        conf.parseFile(ServerConfig::ConfigFile);

        vector<string> vHourlist;

        map<string,string> mLogType;

        try
        {
            string sHour = conf["/tars/log/format<hour>"];
            
            vHourlist = TC_Common::sepstr<string>(sHour,"|;,");

            sort(vHourlist.begin(),vHourlist.end());

            unique(vHourlist.begin(),vHourlist.end());

            result = "loadLogFormat succ:" + sHour;

            TLOGDEBUG("LogServer::loadLogFormat result:" << result << endl);

            DLOG<< "LogServer::loadLogFormat result:" << result << endl;

            //hour=app.server.file|app2.server2.file2
            map<string,string> mType;
            if(conf.getDomainMap("/tars/log/logtype", mType))
            {
                map<string,string>::iterator it = mType.begin();
                while(it != mType.end())
                {
                    vector<string> vList = TC_Common::sepstr<string>(it->second,"|;,");
                    for(size_t i = 0;i < vList.size();i++)
                    {
                        //app.server.file = hour
                        mLogType[vList[i]] = it->first;

                        TLOGDEBUG("LogServer::loadLogFormat " << vList[i] << "|" << it->first << endl);

                        DLOG<<"LogServer::loadLogFormat " << vList[i] << "|" << it->first << endl;
                    }
                    it++;
                }
            }

            g_globe.update(vHourlist, mLogType);

        }
        catch(exception& e)
        {
            result +=  e.what();
            TLOGERROR("LogServer::loadLogFormat command:" << command << "|params:" << params << "|result:" << result << endl);
        }

        return true;
    }
    catch(exception &e)
    {
        result +=  e.what();
        TLOGERROR("LogServer::loadLogFormat command:" << command << "|params:" << params << "|result:" << result << endl);
    }

    return false;
}

void LogServer::destroyApp()
{
    TLOGDEBUG("LogServer::destroyApp ok" << endl);
}

