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

#include "LoadDbThread.h"
#include "NotifyServer.h"

LoadDbThread::LoadDbThread()
: _interval(30)
, _terminate(false)
{

}

LoadDbThread::~LoadDbThread()
{
    if(isAlive())
    {
        terminate();
        getThreadControl().join();
    }
}

void LoadDbThread::terminate()
{
    _terminate = true;

    TC_ThreadLock::Lock lock(*this);

    notifyAll();
}

void LoadDbThread::init()
{
    try
    {
        map<string, string> mDbConf = g_pconf->getDomainMap("/tars/db");
        assert(!mDbConf.empty());

        TC_DBConf tcDBConf;
        tcDBConf.loadFromMap(mDbConf);

        _mysql.init(tcDBConf);

        TLOGDEBUG("LoadDbThread::init init mysql conf succ." << endl);
    }
    catch(exception &ex)
    {
        TLOGERROR("LoadDbThread::init ex:" << ex.what() << endl);
        FDLOG("EX") << "LoadDbThread::init ex:" << ex.what() << endl;
    }
}

void LoadDbThread::run()
{
    size_t iLastTime = 0;
    size_t iNow = TNOW;

    while (!_terminate)
    {
        if(iNow - iLastTime >= _interval)
        {
            loadData();

            iLastTime = iNow;
        }

        {
            TC_ThreadLock::Lock lock(*this);
            timedWait(1000);
        }
    }
}

void LoadDbThread::loadData()
{
    try
    {
        TC_Mysql::MysqlData mysqlData;
        map<string, string> &mTmep = _data.getWriter();
        mTmep.clear();
        size_t iOffset(0);

        do
        {
            string sSql("select application, set_name, set_area, set_group, server_name, node_name from t_server_conf limit 1000 offset ");
            sSql = sSql + TC_Common::tostr<size_t>(iOffset) +";";

            mysqlData = _mysql.queryRecord(sSql);

            for (size_t i = 0; i < mysqlData.size(); i++)
            {
                string sValue = mysqlData[i]["set_name"] +"."+ mysqlData[i]["set_area"]+ "." + mysqlData[i]["set_group"];
                if (mysqlData[i]["set_name"].empty())
                {
                    continue;
                }

                string sKey = mysqlData[i]["application"] + "." +  mysqlData[i]["server_name"] + mysqlData[i]["node_name"];
                mTmep.insert(map<string, string>::value_type(sKey, sValue));
            }

            iOffset += mysqlData.size();

        } while (iOffset % 1000 == 0);

        _data.swap();

        TLOGDEBUG("LoadDbThread::loadData load data finish, _mSetApp size:" << mTmep.size() << endl);

    }
    catch (exception &ex)
    {
        TLOGERROR("LoadDbThread::loadData exception:" << ex.what() << endl);
        FDLOG("EX") << "LoadDbThread::loadData exception:" << ex.what() << endl;
    }
}
