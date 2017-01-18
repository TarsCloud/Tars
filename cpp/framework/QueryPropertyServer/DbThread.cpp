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

#include "DbThread.h"
#include "util/tc_config.h"
#include "QueryServer.h"


DBThread::DBThread()
: _bTerminate(false)
{
    TLOGDEBUG("DBThread init ok" << endl);
}

DBThread::~DBThread()
{
    if (isAlive())
    {
        terminate();

        getThreadControl().join();
    }
    TLOGDEBUG("DBThread terminate." << endl);
}

void DBThread::terminate()
{
    _bTerminate = true;

    TC_ThreadLock::Lock lock(*this);

    notifyAll();
}

void DBThread::run()
{
    TLOGDEBUG("DBThread::run begin." << endl);
    while (!_bTerminate)
    {
        try
        {
            tryConnect();
        }
        catch ( exception& ex )
        {
            TLOGERROR("DBThread::run exception:" << ex.what() << endl);

        }
        catch (... )
        {
            TLOGERROR("DBThread::run unkonw exception catched" << endl);
        }
        TC_ThreadLock::Lock lock(*this);
        timedWait(REAP_INTERVAL);
    }
}

int DBThread::connect(MYSQL** pstMql,const TC_DBConf& tcConf)
{

    *pstMql = mysql_init(NULL);
    if (mysql_real_connect(*pstMql,tcConf._host.c_str(), tcConf._user.c_str(), tcConf._password.c_str(), tcConf._database.c_str(), tcConf._port, NULL, tcConf._flag) == NULL)
    {
        TLOGERROR("[TC_Mysql::connect]: mysql_real_connect: " + string(mysql_error(*pstMql)) + ", erron:" <<mysql_errno(*pstMql) << " port:"<< tcConf._port << endl);
        return -1;
    }
    return 0;
}

void DBThread::sendAlarmSMS(const string &sMsg)
{
    string errInfo = " ERROR:" + ServerConfig::LocalIp + "|" + sMsg;
    TARS_NOTIFY_ERROR(errInfo);
}

void DBThread::tryConnect()
{
    try
    {


        vector<TC_DBConf> vDbCountInfo = g_app.getDbInfo();

        size_t iDbCountInfoSize = vDbCountInfo.size();

    //    vector<TC_DBConf> vAtivedbInfo;

        vector<TC_DBConf> vAtivedbCountInfo;

    

        for(size_t i = 0; i < vDbCountInfo.size(); i++)
        {
             TC_DBConf tcDBConf = vDbCountInfo[i];

            //tcDBConf._database = "tars_stat";

            MYSQL* _pstMql = NULL;

            int iRet = connect(&_pstMql,tcDBConf);

            if (_pstMql != NULL)
            {
                mysql_close(_pstMql);
                _pstMql = NULL;
            }

            if (iRet != 0)
            {
                TLOGERROR("DBThread::tryConnect new db host:" << tcDBConf._host << "|port:" << TC_Common::tostr(tcDBConf._port) << "|database:" << tcDBConf._database << endl);
                string s("host");

                s += tcDBConf._host;
                s += "|port";
                s += TC_Common::tostr(tcDBConf._port);
                s += "|database:";
                s += tcDBConf._database;

                sendAlarmSMS(s);
                continue;
            }

            vAtivedbCountInfo.push_back(vDbCountInfo[i]);;
        }

        size_t iActiveDbCountInfoSize = vAtivedbCountInfo.size();

        if(iDbCountInfoSize != iActiveDbCountInfoSize)
        {
            string sMsg = "statcount old db size:";
            sMsg += TC_Common::tostr(iDbCountInfoSize);
            sMsg += "|statcount active db size:";
            sMsg += TC_Common::tostr(iActiveDbCountInfoSize);
            sMsg += "|no equal.";

            sendAlarmSMS(sMsg);
        }

        g_app.setActiveDb(vAtivedbCountInfo);
    }
    catch (exception& ex)
    {
        TLOGERROR("DBThread::tryConnect exception:"<< ex.what() << endl);
    }
}



