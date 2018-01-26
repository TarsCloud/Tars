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

#include "NodeServer.h"
#include "NodeImp.h"
#include "ServerImp.h"
#include "RegistryProxy.h"
#include "NodeRollLogger.h"

string NodeServer::g_sNodeIp;

BatchPatch *g_BatchPatchThread;
RemoveLogManager *g_RemoveLogThread;

void NodeServer::initialize()
{
    //滚动日志也打印毫秒
    TarsRollLogger::getInstance()->logger()->modFlag(TC_DayLogger::HAS_MTIME);

    //node使用的循环日志初始化
    RollLoggerManager::getInstance()->setLogInfo(ServerConfig::Application,
            ServerConfig::ServerName, ServerConfig::LogPath,
            ServerConfig::LogSize, ServerConfig::LogNum, _communicator,
            ServerConfig::Log);

    initRegistryObj();

    addConfig("tarsnode.conf");

    //增加对象
    string sNodeObj     = ServerConfig::Application + "." + ServerConfig::ServerName + ".NodeObj";
    string sServerObj   = ServerConfig::Application + "." + ServerConfig::ServerName + ".ServerObj";

    addServant<NodeImp>(sNodeObj);
    addServant<ServerImp>(sServerObj);

    TLOGDEBUG("NodeServer::initialize ServerAdapter " << (getAdapterEndpoint("ServerAdapter")).toString() << endl);
    TLOGDEBUG("NodeServer::initialize NodeAdapter "   << (getAdapterEndpoint("NodeAdapter")).toString() << endl);

    g_sNodeIp = getAdapterEndpoint("NodeAdapter").getHost();

    if (!ServerFactory::getInstance()->loadConfig())
    {
        TLOGERROR("NodeServer::initialize ServerFactory loadConfig failure" << endl);
    }

    //查看服务desc
    TARS_ADD_ADMIN_CMD_PREFIX("tars.serverdesc", NodeServer::cmdViewServerDesc);
    TARS_ADD_ADMIN_CMD_PREFIX("reloadconfig", NodeServer::cmdReLoadConfig);

    initHashMap();

    //启动KeepAliveThread
    _keepAliveThread   = new KeepAliveThread();
    _keepAliveThread->start();

    TLOGDEBUG("NodeServer::initialize |KeepAliveThread start" << endl);

    _reportMemThread = new ReportMemThread();
    _reportMemThread->start();

    TLOGDEBUG("NodeServer::initialize |_reportMemThread start" << endl);

    //启动批量发布线程
    PlatformInfo plat;
    int iThreads        = TC_Common::strto<int>(g_pconf->get("/tars/node<bpthreads>", "10"));

    _batchPatchThread  = new BatchPatch();
    _batchPatchThread->setPath(plat.getDownLoadDir());
    _batchPatchThread->start(iThreads);

    g_BatchPatchThread  = _batchPatchThread;

    TLOGDEBUG("NodeServer::initialize |BatchPatchThread start(" << iThreads << ")" << endl);

    _removeLogThread = new RemoveLogManager();
    _removeLogThread->start(iThreads);

    g_RemoveLogThread = _removeLogThread;

    TLOGDEBUG("NodeServer::initialize |RemoveLogThread start(" << iThreads << ")" << endl);
}

void NodeServer::initRegistryObj()
{
    string sLocator =    Application::getCommunicator()->getProperty("locator");
    vector<string> vtLocator = TC_Common::sepstr<string>(sLocator, "@");
    TLOGDEBUG("locator:" << sLocator << endl);
    if (vtLocator.size() == 0)
    {
        TLOGERROR("NodeServer::initRegistryObj failed to parse locator" << endl);
        exit(1);
    }

    vector<string> vObj = TC_Common::sepstr<string>(vtLocator[0], ".");
    if (vObj.size() != 3)
    {
        TLOGERROR("NodeServer::initRegistryObj failed to parse locator" << endl);
        exit(1);
    }

    //获取主控名字的前缀
    string sObjPrefix("");
    string::size_type pos = vObj[2].find("QueryObj");
    if (pos != string::npos)
    {
        sObjPrefix = vObj[2].substr(0, pos);
    }

    AdminProxy::getInstance()->setRegistryObjName("tars.tarsregistry." + sObjPrefix + "RegistryObj",
                                                  "tars.tarsregistry." + sObjPrefix + "QueryObj");

    TLOGDEBUG("NodeServer::initRegistryObj RegistryObj:" << ("tars.tarsregistry." + sObjPrefix + "RegistryObj") << endl);
    TLOGDEBUG("NodeServer::initRegistryObj QueryObj:" << ("tars.tarsregistry." + sObjPrefix + "QueryObj") << endl);
}

void NodeServer::initHashMap()
{
    TLOGDEBUG("NodeServer::initHashMap " << endl);

    string sFile        = ServerConfig::DataPath + "/" + g_pconf->get("/tars/node/hashmap<file>", "__tarsnode_servers");
    string sPath        = TC_File::extractFilePath(sFile);
    int iMinBlock       = TC_Common::strto<int>(g_pconf->get("/tars/node/hashmap<minBlock>", "500"));
    int iMaxBlock       = TC_Common::strto<int>(g_pconf->get("/tars/node/hashmap<maxBlock>", "500"));
    float iFactor       = TC_Common::strto<float>(g_pconf->get("/tars/node/hashmap<factor>", "1"));
    int iSize           = TC_Common::toSize(g_pconf->get("/tars/node/hashmap<size>"), 1024 * 1024 * 10);

    if (!TC_File::makeDirRecursive(sPath))
    {
        TLOGDEBUG("NodeServer::initHashMap cannot create hashmap file " << sPath << endl);
        exit(0);
    }

    try
    {
        g_serverInfoHashmap.initDataBlockSize(iMinBlock, iMaxBlock, iFactor);
        g_serverInfoHashmap.initStore(sFile.c_str(), iSize);

        TLOGDEBUG("NodeServer::initHashMap init hash map succ" << endl);
    }
    catch (TC_HashMap_Exception& e)
    {
        TC_File::removeFile(sFile, false);
        runtime_error(e.what());
    }
}

TC_Endpoint NodeServer::getAdapterEndpoint(const string& name) const
{
    TLOGINFO("NodeServer::getAdapterEndpoint:" << name << endl);

    TC_EpollServerPtr pEpollServerPtr = Application::getEpollServer();
    assert(pEpollServerPtr);

    TC_EpollServer::BindAdapterPtr pBindAdapterPtr = pEpollServerPtr->getBindAdapter(name);
    assert(pBindAdapterPtr);

    return pBindAdapterPtr->getEndpoint();
}

bool NodeServer::cmdViewServerDesc(const string& command, const string& params, string& result)
{
    TLOGINFO("NodeServer::cmdViewServerDesc" << command << " " << params << endl);

    vector<string> v = TC_Common::sepstr<string>(params, ".");
    if (v.size() != 2)
    {
        result = "invalid params:" + params;
        return false;
    }

    string application  = v[0];
    string serverName   = v[1];

    ServerObjectPtr pServerObjectPtr = ServerFactory::getInstance()->getServer(application, serverName);
    if (pServerObjectPtr)
    {
        ostringstream os;
        pServerObjectPtr->getServerDescriptor().display(os);
        result = os.str();
        return false;
    }

    result = "server " + params + " not exist";

    return true;
}

bool NodeServer::cmdReLoadConfig(const string& command, const string& params, string& result)
{
    TLOGDEBUG("NodeServer::cmdReLoadConfig " << endl);

    bool bRet = false;

    if (addConfig("tarsnode.conf"))
    {
        bRet = ServerFactory::getInstance()->loadConfig();
    }

    string s = bRet ? "OK" : "failure";

    result = "cmdReLoadConfig " + s;

    return bRet;
}

void NodeServer::destroyApp()
{
    if (_keepAliveThread)
    {
        delete _keepAliveThread;
        _keepAliveThread = NULL;
    }

    if (_reportMemThread)
    {
        delete _reportMemThread;
        _reportMemThread = NULL;
    }

    if (_batchPatchThread)
    {
        delete _batchPatchThread;
        _batchPatchThread = NULL;
    }

    if (_removeLogThread)
    {
        delete _removeLogThread;
        _removeLogThread = NULL;
    }

    TLOGDEBUG("NodeServer::destroyApp "<< pthread_self() << endl);
}

string tostr(const set<string>& setStr)
{
    string str = "{";
    set<string>::iterator it = setStr.begin();
    int i = 0;
    for (; it != setStr.end(); it++, i++)
    {
        if (i > 0)
        {
            str += "," + *it;
        }
        else
        {
            str += *it;
        }
    }
    str += "}";

    return str;
}

bool NodeServer::isValid(const string& ip)
{
    static time_t g_tTime = 0;
    static set<string> g_ipSet;

    time_t tNow = TNOW;

    static  TC_ThreadLock g_tMutex;

    TC_ThreadLock::Lock  lock(g_tMutex);
    if (tNow - g_tTime > 60)
    {
        string objs = g_pconf->get("/tars/node<cmd_white_list>", "tars.tarsregistry.AdminRegObj:tars.tarsAdminRegistry.AdminRegObj");
        string ips  = g_pconf->get("/tars/node<cmd_white_list_ip>", "172.25.38.208:172.25.38.208");

        TLOGDEBUG("NodeServer::isValid objs:" << objs << "|ips:" << ips << endl);

        vector<string> vObj = TC_Common::sepstr<string>(objs, ":");

        vector<string> vIp = TC_Common::sepstr<string>(ips, ":");
        for (size_t i = 0; i < vIp.size(); i++)
        {
            g_ipSet.insert(vIp[i]);
            TLOGDEBUG(ips << "g_ipSet insert ip:" << vIp[i] << endl);
        }

        for (size_t i = 0; i < vObj.size(); i++)
        {
            set<string> tempSet;
            string obj = vObj[i];
            try
            {

                QueryFPrx queryPrx = Application::getCommunicator()->stringToProxy<QueryFPrx>(obj);
                vector<EndpointInfo> vActiveEp, vInactiveEp;
                queryPrx->tars_endpointsAll(vActiveEp, vInactiveEp);

                for (unsigned i = 0; i < vActiveEp.size(); i++)
                {
                    tempSet.insert(vActiveEp[i].host());
                }

                for (unsigned i = 0; i < vInactiveEp.size(); i++)
                {
                    tempSet.insert(vInactiveEp[i].host());
                }

                TLOGDEBUG("NodeServer::isValid "<< obj << "|tempSet.size():" << tempSet.size() << "|" << tostr(tempSet) << endl);
            }
            catch (exception& e)
            {
                TLOGERROR("NodeServer::isValid catch error: " << e.what() << endl);
            }
            catch (...)
            {
                TLOGERROR("NodeServer::isValid catch error: " << endl);
            }

            if (tempSet.size() > 0)
            {
                g_ipSet.insert(tempSet.begin(), tempSet.end());
            }
        }

        TLOGDEBUG("NodeServer::isValid g_ipSet.size():" << g_ipSet.size() << "|" << tostr(g_ipSet) << endl);
        g_tTime = tNow;
    }

    if (g_ipSet.count(ip) > 0)
    {
        return true;
    }

    if (g_sNodeIp == ip)
    {
        return true;
    }

    return false;
}

void NodeServer::reportServer(const string& sServerId, const string& sResult)
{
    try
    {
        //上报到notify
        NotifyPrx pNotifyPrx = Application::getCommunicator()->stringToProxy<NotifyPrx>(ServerConfig::Notify);
        if (pNotifyPrx && sResult != "")
        {
            pNotifyPrx->async_reportServer(NULL, sServerId, "", sResult);
        }
    }
    catch (exception& ex)
    {
        TLOGERROR("NodeServer::reportServer error:" << ex.what() << endl);
    }
}
