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

#include "QueryServer.h"
#include "QueryImp.h"

using namespace std;

QueryServer g_app;

TC_Config * g_pconf;
/////////////////////////////////////////////////////////////////

struct JsonProtocol
{
    
    static int parse(string &in, string &out)
    {
        TLOGINFO("JsonProtocol parse:" << in << endl);

        string::size_type jsonEnd = in.find("}");

        if (jsonEnd != string::npos )
        {
            out = in;
            in = "";
            return TC_EpollServer::PACKET_FULL;   //返回1表示收到的包已经完全
        }

        return TC_EpollServer::PACKET_ERR;        //返回-1表示收到包协议错误，框架会自动关闭当前连接
    }
};



void  QueryServer::initialize()
{
    //initialize application here:
    //...
    vector<string> v_dblist;
    vector<string> v_dbcountlist;

    g_pconf->getDomainVector("/tars/countdb", v_dbcountlist);

    size_t iDbNumber = v_dblist.size();

    size_t iDbCountNumber = v_dbcountlist.size();

    TLOGDEBUG("QueryServer::initialize stat dbstat size:" << iDbNumber << "|dbcount size:" << iDbCountNumber << endl);

    for(size_t i = 0; i < iDbCountNumber; i++)
    {
        TC_DBConf tcDBConf;

        string path= "/tars/countdb/" + v_dbcountlist[i];

        tcDBConf.loadFromMap(g_pconf->getDomainMap(path));

        _dbStatInfo.push_back(tcDBConf);
    }

    _activeDbInfo = _dbStatInfo;

    _dBThread = new DBThread();

    _dBThread->start();

    _queryFlag.insert("f_date");
    _queryFlag.insert("f_tflag");
    _queryFlag.insert("master_name");
    _queryFlag.insert("slave_name");
    _queryFlag.insert("slave_ip");
    _queryFlag.insert("interface_name");

    _insertInterval = TC_Common::strto<int>(g_pconf->get("/tars<interval>","5"));

    size_t iDbThreadPoolSize = TC_Common::strto<int>(g_pconf->get("/tars/threadpool<count_db_tpoolsize>","8"));

    size_t iTimeCheckPoolSize = TC_Common::strto<int>(g_pconf->get("/tars/threadpool<time_check_tpoolsize>","8"));

    size_t iQueryDbPoolSize = TC_Common::strto<int>(g_pconf->get("/tars/threadpool<query_countdb_tpoolsize>","4"));

    _timeCheck.init(iTimeCheckPoolSize);

    _timeCheck.start();

    _poolDb.init(iDbThreadPoolSize);

    _poolDb.start();

    _tpoolQueryDb = new QueryDbThread();

    _tpoolQueryDb->start(iQueryDbPoolSize);

    vector<string> vIpGroup = g_pconf->getDomainKey("/tars/notarsslavename");
    TLOGDEBUG("QueryServer::initialize vIpGroup size:" << vIpGroup.size() << endl);
    for (unsigned i = 0; i < vIpGroup.size(); i++)
    {
        _notTarsSlaveName.insert(vIpGroup[i]);
        TLOGDEBUG("QueryServer::initialize i:" << i << "|notarsslavename:" << vIpGroup[i] << endl);
    }
    addServant<QueryImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".NoTarsObj");
    addServantProtocol(ServerConfig::Application + "." + ServerConfig::ServerName + ".NoTarsObj", &JsonProtocol::parse);
}
/////////////////////////////////////////////////////////////////

void QueryServer::setActiveDb(const vector<TC_DBConf> &vDbInfo)
{
    TC_LockT<TC_ThreadMutex> lock(*this);
    _activeDbInfo = vDbInfo;
}

vector<TC_DBConf> QueryServer::getAllActiveDbInfo() const
{
    TC_LockT<TC_ThreadMutex> lock(*this);
    vector<TC_DBConf> vDbInfo;
    for(size_t i = 0; i < _activeDbInfo.size(); ++i)
    {
        vDbInfo.push_back(_activeDbInfo[i]);
    }

    return vDbInfo;
}

vector<TC_DBConf> QueryServer::getDbInfo() const
{
    TC_LockT<TC_ThreadMutex> lock(*this);
    return _dbStatInfo;
}

vector<TC_DBConf> QueryServer::getActiveDbInfo() const
{
    TC_LockT<TC_ThreadMutex> lock(*this);
    return _activeDbInfo;
}

uint32_t QueryServer::genUid()
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    while (++_uniqId == 0);

    return _uniqId;
}

string QueryServer::dumpDbInfo(const vector<TC_DBConf>& vDbInfo) const
{
    ostringstream os;

    os <<endl;
    for(size_t i = 0; i < vDbInfo.size();i++)
    {
        os << "[charset]=[" <<vDbInfo[i]._charset <<"] "
           << "[dbhost]=[" <<vDbInfo[i]._host <<"] "
           << "[dbpass]=[" <<vDbInfo[i]._password <<"] "
           << "[dbport]=[" <<TC_Common::tostr(vDbInfo[i]._port) <<"] "
           << "[dbuser]=[" <<vDbInfo[i]._user <<"]"
           <<endl;
    }
    return os.str();
}

bool QueryServer::searchQueryFlag(const string &sKey)
{
    set<string>::const_iterator it = _queryFlag.find(sKey);
    if(it != _queryFlag.end())
    {
        return true;
    }
    return false;
}

set<string>& QueryServer::getNotTarsSlaveName()
{
    return _notTarsSlaveName;
}

/////////////////////////////////////////////////////////////////
void
QueryServer::destroyApp()
{
    //destroy application here:
    //...
    if(_dBThread)
    {
        delete _dBThread;
        _dBThread = NULL;
    }

    if(_tpoolQueryDb)
    {
        delete _tpoolQueryDb;
        _tpoolQueryDb = NULL;
    }

    bool b = _poolDb.waitForAllDone(1000);

    TLOGDEBUG("QueryServer::destroyApp waitForAllDone _poolDb return:" << b << "|getJobNum:" << _poolDb.getJobNum() << endl);

    _poolDb.stop();

    b = _timeCheck.waitForAllDone(1000);

    TLOGDEBUG("QueryServer::destroyApp waitForAllDone _timeCheck return:" << b << "|getJobNum:" << _timeCheck.getJobNum() << endl);

    _timeCheck.stop();
}
/////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
    try
    {
        g_pconf =  & g_app.getConfig();
        g_app.main(argc, argv);
        TarsTimeLogger::getInstance()->enableRemote("inout",false);
        g_app.waitForShutdown();
    }
    catch (std::exception& e)
    {
        cerr << "std::exception:" << e.what() << std::endl;
    }
    catch (...)
    {
        cerr << "unknown exception." << std::endl;
    }
    return -1;
}
/////////////////////////////////////////////////////////////////
