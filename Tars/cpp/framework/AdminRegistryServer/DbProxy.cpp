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
#include "AdminRegistryServer.h"
#include "util/tc_parsepara.h"
#include "ExecuteTask.h"

TC_ThreadLock DbProxy::_mutex;

map<string, NodePrx> DbProxy::_mapNodePrxCache;

TC_ThreadLock DbProxy::_NodePrxLock;
vector<map<string, string> >DbProxy::_serverGroupRule;
//key-ip, value-组编号
map<string, int> DbProxy::_serverGroupCache;


int DbProxy::init(TC_Config *pconf)
{
    try
    {

        TC_DBConf tcDBConf;
        tcDBConf.loadFromMap(pconf->getDomainMap("/tars/db"));
        _mysqlReg.init(tcDBConf);
    }
    catch (TC_Config_Exception& ex)
    {
        TLOGERROR(__FUNCTION__ << " exception: " << ex.what() << endl);
    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR(__FUNCTION__ << " exception: " << ex.what() << endl);
    }

    return 0;
}

int DbProxy::addTaskReq(const TaskReq &taskReq)
{
    try
    {
        for (size_t i = 0; i < taskReq.taskItemReq.size(); i++)
        {
            map<string, pair<TC_Mysql::FT, string> > data;
            data["task_no"]     = make_pair(TC_Mysql::DB_STR, taskReq.taskNo);
            data["item_no"]     = make_pair(TC_Mysql::DB_STR, taskReq.taskItemReq[i].itemNo);
            data["application"] = make_pair(TC_Mysql::DB_STR, taskReq.taskItemReq[i].application);
            data["server_name"] = make_pair(TC_Mysql::DB_STR, taskReq.taskItemReq[i].serverName);
            data["node_name"]   = make_pair(TC_Mysql::DB_STR, taskReq.taskItemReq[i].nodeName);
            data["command"]     = make_pair(TC_Mysql::DB_STR, taskReq.taskItemReq[i].command);
            data["parameters"]  = make_pair(TC_Mysql::DB_STR, TC_Parsepara(taskReq.taskItemReq[i].parameters).tostr());

            _mysqlReg.insertRecord("t_task_item", data);
        }

        {
            map<string, pair<TC_Mysql::FT, string> > data;
            data["task_no"]     = make_pair(TC_Mysql::DB_STR, taskReq.taskNo);
            data["serial"]      = make_pair(TC_Mysql::DB_INT, TC_Common::tostr(taskReq.serial));
            data["create_time"] = make_pair(TC_Mysql::DB_INT, "now()");
            data["user_name"]   = make_pair(TC_Mysql::DB_STR, taskReq.userName);

            _mysqlReg.insertRecord("t_task", data);
        }
    }
    catch (exception &ex)
    {
        TLOGERROR("DbProxy::addTaskReq exception: " << ex.what() << endl);
        return -1;
    }

    return 0;
}

int DbProxy::getTaskRsp(const string &taskNo, TaskRsp &taskRsp)
{

    try
    {
        string sSql = "select * from t_task as t1, t_task_item as t2 where t1.task_no=t2.task_no and t2.task_no='" 
            + _mysqlReg.escapeString(taskNo) + "'";

        tars::TC_Mysql::MysqlData item = _mysqlReg.queryRecord(sSql);
        if (item.size() == 0)
        {
            TLOGERROR("DbProxy::getTaskRsp 't_task' not task: " << taskNo << endl);
            return -1;
        }

        taskRsp.taskNo = item[0]["task_no"];
        taskRsp.serial = TC_Common::strto<int>(item[0]["serial"]);
        taskRsp.userName = item[0]["user_name"];


        for (unsigned i = 0; i < item.size(); i++) 
        {
            TaskItemRsp rsp;
            rsp.startTime  = item[i]["start_time"];
            rsp.endTime    = item[i]["end_time"];
            rsp.status     = (EMTaskItemStatus)TC_Common::strto<int>(item[i]["status"]);
            rsp.statusInfo = etos(rsp.status);

            rsp.req.taskNo       = item[i]["task_no"];
            rsp.req.itemNo       = item[i]["item_no"];
            rsp.req.application  = item[i]["application"];
            rsp.req.serverName   = item[i]["server_name"];
            rsp.req.nodeName     = item[i]["node_name"];
            rsp.req.setName      = item[i]["set_name"];
            rsp.req.command      = item[i]["command"];
            rsp.req.parameters   = TC_Parsepara(item[i]["parameters"]).toMap();

            taskRsp.taskItemRsp.push_back(rsp);
        }
        
        ExecuteTask::getInstance()->checkTaskRspStatus(taskRsp);
    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR("DbProxy::getTaskRsp exception"<<ex.what()<<endl);
        return -3;
    }

    return 0;
}

int DbProxy::getTaskHistory(const string & application, const string & serverName, const string & command, vector<TaskRsp> &taskRsp)
{
    string sSql = "select t1.`create_time`, t1.`serial`, t1.`user_name`, t2.* from t_task as t1, t_task_item as t2 where t1.task_no=t2.task_no and t2.application='" 
        + _mysqlReg.escapeString(application) + "' and t2.server_name='" 
        + _mysqlReg.escapeString(serverName) +  "' and t2.command='" 
        + _mysqlReg.escapeString(command) +     "' order by create_time desc, task_no";

    try
    {
        tars::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);
        TLOGDEBUG("DbProxy::getTaskHistory size:" << res.size() << ", sql:" << sSql << endl);
        for (unsigned i = 0; i < res.size(); i++)
        {
            string taskNo = res[i]["task_no"];

            //获取Item
            TaskItemRsp itemRsp;
            itemRsp.startTime = res[i]["start_time"];
            itemRsp.endTime   = res[i]["end_time"];
            itemRsp.status    = (EMTaskItemStatus)TC_Common::strto<int>(res[i]["status"]);
            itemRsp.statusInfo= etos(itemRsp.status);

            itemRsp.req.taskNo      = res[i]["task_no"];
            itemRsp.req.itemNo      = res[i]["item_no"];
            itemRsp.req.application = res[i]["application"];
            itemRsp.req.serverName  = res[i]["server_name"];
            itemRsp.req.nodeName    = res[i]["node_name"];
            itemRsp.req.setName     = res[i]["set_name"];
            itemRsp.req.command     = res[i]["command"];
            itemRsp.req.parameters  = TC_Parsepara(res[i]["parameters"]).toMap();

            if (taskRsp.empty() || taskRsp[taskRsp.size() - 1].taskNo != taskNo)
            {
                //新的TaskRsp
                TaskRsp rsp;
                rsp.taskNo   = res[i]["task_no"];
                rsp.serial   = TC_Common::strto<int>(res[i]["serial"]);
                rsp.userName = res[i]["user_name"];

                rsp.taskItemRsp.push_back(itemRsp);

                taskRsp.push_back(rsp);
            }
            else
            {
                taskRsp.back().taskItemRsp.push_back(itemRsp);
            }
        }
    }
    catch (exception &ex)
    {
        TLOGERROR("DbProxy::getTaskHistory exception: " << ex.what() << endl);
        return -1;
    }

    return 0;
}

int DbProxy::setTaskItemInfo(const string & itemNo, const map<string, string> &info)
{
    string where = " where item_no='" + itemNo + "'";
    try
    {
        map<string, pair<TC_Mysql::FT, string> > data;
        data["item_no"] = make_pair(TC_Mysql::DB_STR, itemNo);
        map<string, string>::const_iterator it = info.find("start_time");
        if (it != info.end())
        {
            data["start_time"] = make_pair(TC_Mysql::DB_STR, it->second); 
        }
        it = info.find("end_time");
        if (it != info.end())
        {
            data["end_time"] = make_pair(TC_Mysql::DB_STR, it->second); 
        }
        it = info.find("status");
        if (it != info.end())
        {
            data["status"] = make_pair(TC_Mysql::DB_INT, it->second); 
        }
        it = info.find("log");
        if (it != info.end())
        {
            data["log"] = make_pair(TC_Mysql::DB_STR, it->second);
        }

        _mysqlReg.updateRecord("t_task_item", data, where);
    }
    catch (exception &ex)
    {
        TLOGERROR("DbProxy::setTaskItemInfo exception: " << ex.what() << endl);
        return -1;
    }

    return 0;
}

int DbProxy::undeploy(const string & application, const string & serverName, const string & nodeName, const string &user, string &log)
{
    string where = "where application='" + application + "' and server_name='" + serverName + "' and node_name='" +nodeName + "'";

    try
    {

        _mysqlReg.deleteRecord("t_server_conf", where);

        _mysqlReg.deleteRecord("t_adapter_conf", where);

    }
    catch (exception &ex)
    {
        log = ex.what();
        TLOGERROR("DbProxy::undeploy exception: " << ex.what() << endl);
        return -1;
    }

    return 0;
}

map<string, string> DbProxy::getActiveNodeList(string& result)
{
    map<string, string> mapNodeList;
    try
    {
        string sSql =
                      "select node_name, node_obj from t_node_info "
                      "where present_state='active'";

        tars::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);
        TLOGDEBUG("DbProxy::getActiveNodeList (present_state='active') affected:" << res.size() << endl);
        for (unsigned i = 0; i < res.size(); i++)
        {
            mapNodeList[res[i]["node_name"]] = res[i]["node_obj"];
        }
    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR("DbProxy::getActiveNodeList exception: " <<ex.what() << endl);
        return mapNodeList;
    }

    return  mapNodeList;
}
int DbProxy::setPatchInfo(const string& app, const string& serverName, const string& nodeName,
                          const string& version, const string& user)
{
    try
    {
        string sSql =
                      "update t_server_conf "
                      "set patch_version = '" + _mysqlReg.escapeString(version) + "', "
                      "   patch_user = '"     + _mysqlReg.escapeString(user) + "', "
                      "   patch_time = now() "
                      "where application='"   + _mysqlReg.escapeString(app) + "' "
                      "    and server_name='" + _mysqlReg.escapeString(serverName) + "' "
                      "    and node_name='"   + _mysqlReg.escapeString(nodeName) + "' ";
         
        _mysqlReg.execute(sSql);
        TLOGDEBUG("DbProxy::setPatchInfo " << app << "." << serverName << "_" << nodeName
                  << " affected:" << _mysqlReg.getAffectedRows() << endl);

        return 0;
    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR("DbProxy::setPatchInfo " << app << "." << serverName << "_" << nodeName
                  << " exception: " << ex.what() << endl);
        return -1;
    }
}


int DbProxy::getNodeVersion(const string& nodeName, string& version, string& result)
{
    try
    {
        string sSql =
                      "select tars_version from t_node_info "
                      "where node_name='" + _mysqlReg.escapeString(nodeName) + "'";

        tars::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);
        TLOGDEBUG(__FUNCTION__ << " (node_name='" << nodeName << "') affected:" << res.size() << endl);
        if (res.size() > 0)
        {
            version = res[0]["tars_version"];
            return 0;

        }
        result = "node_name(" + nodeName + ") int table t_node_info not exist";
    }
    catch (TC_Mysql_Exception& ex)
    {
        result = string(__FUNCTION__) + " exception: " + ex.what();
        TLOGERROR(result << endl);
    }
    return  -1;
}

int DbProxy::updateServerState(const string& app, const string& serverName, const string& nodeName,
                               const string& stateFields, tars::ServerState state, int processId)
{
    try
    {
        int64_t iStart = TC_TimeProvider::getInstance()->getNowMs();
        if (stateFields != "setting_state" && stateFields != "present_state")
        {
            TLOGDEBUG(app << "." << serverName << "_" << nodeName
                      << " not supported fields:" << stateFields << endl);
            return -1;
        }

        string sProcessIdSql = (stateFields == "present_state" ?
                                (", process_id = " + TC_Common::tostr<int>(processId) + " ") : "");

        string sSql =
                      "update t_server_conf "
                      "set " + stateFields + " = '" + etos(state) + "' " + sProcessIdSql +
                      "where application='" + _mysqlReg.escapeString(app) + "' "
                      "    and server_name='" + _mysqlReg.escapeString(serverName) + "' "
                      "    and node_name='" + _mysqlReg.escapeString(nodeName) + "' ";

        _mysqlReg.execute(sSql);
        TLOGDEBUG(__FUNCTION__ << " " << app << "." << serverName << "_" << nodeName
                  << " affected:" << _mysqlReg.getAffectedRows()
                  << "|cost:" << (TC_TimeProvider::getInstance()->getNowMs() - iStart) << endl);
        return 0;

    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR(__FUNCTION__ << " " << app << "." << serverName << "_" << nodeName
                  << " exception: " << ex.what() << endl);
        return -1;
    }
}

int DbProxy::gridPatchServer(const string& app, const string& servername, const string& nodename, const string& status)
{
    try
    {
        string sSql("update t_server_conf");
        sSql += " set grid_flag='";
        sSql += status;
        sSql += "' where application='";
        sSql += _mysqlReg.escapeString(app);
        sSql += "' and server_name='";
        sSql += _mysqlReg.escapeString(servername);
        sSql += "' and node_name='";
        sSql += _mysqlReg.escapeString(nodename);
        sSql += "'";

        int64_t iStart = TC_TimeProvider::getInstance()->getNowMs();

        _mysqlReg.execute(sSql);

        TLOGDEBUG(__FUNCTION__ << "|app:" << app << "|server:" << servername << "|node:" << nodename
                  << "|affected:" << _mysqlReg.getAffectedRows() << "|cost:" << (TC_TimeProvider::getInstance()->getNowMs() - iStart) << endl);

        return 0;

    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR(__FUNCTION__ << "|app:" << app << "|server:" << servername << "|node:" << nodename
                  << "|exception: " << ex.what() << endl);
        return -1;
    }
}

vector<ServerDescriptor> DbProxy::getServers(const string& app, const string& serverName, const string& nodeName, bool withDnsServer)
{
    string sSql;
    vector<ServerDescriptor>  vServers;
    unsigned num = 0;
    int64_t iStart = TC_TimeProvider::getInstance()->getNowMs();

    try
    {
        //server详细配置
        string sCondition;
        sCondition += "server.node_name='" + _mysqlReg.escapeString(nodeName) + "'";
        if (app != "")        sCondition += " and server.application='" + _mysqlReg.escapeString(app) + "' ";
        if (serverName != "") sCondition += " and server.server_name='" + _mysqlReg.escapeString(serverName) + "' ";
        if (withDnsServer == false) sCondition += " and server.server_type !='tars_dns' "; //不获取dns服务

//        "    allow_ip, max_connections, servant, queuecap, queuetimeout,protocol,handlegroup,shmkey,shmcap,"

        sSql =
               "select server.application, server.server_name, server.node_name, base_path, "
               "    exe_path, setting_state, present_state, adapter_name, thread_num, async_thread_num, endpoint,"
               "    profile,template_name, "
               "    allow_ip, max_connections, servant, queuecap, queuetimeout,protocol,handlegroup,"
               "    patch_version, patch_time, patch_user, "
               "    server_type, start_script_path, stop_script_path, monitor_script_path,config_center_port ,"
               "     enable_set, set_name, set_area, set_group "
               "from t_server_conf as server "
               "    left join t_adapter_conf as adapter using(application, server_name, node_name) "
               "where " + sCondition;

        tars::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);
        num = res.size();
        //对应server在vector的下标
        map<string, int> mapAppServerTemp;

        //获取模版profile内容
        map<string, string> mapProfile;

        //分拆数据到server的信息结构里
        for (unsigned i = 0; i < res.size(); i++)
        {
            string sServerId = res[i]["application"] + "." + res[i]["server_name"]
                               + "_" + res[i]["node_name"];

            if (mapAppServerTemp.find(sServerId) == mapAppServerTemp.end())
            {
                //server配置
                ServerDescriptor server;
                server.application  = res[i]["application"];
                server.serverName   = res[i]["server_name"];
                server.nodeName     = res[i]["node_name"];
                server.basePath     = res[i]["base_path"];
                server.exePath      = res[i]["exe_path"];
                server.settingState = res[i]["setting_state"];
                server.presentState = res[i]["present_state"];
                server.patchVersion = res[i]["patch_version"];
                server.patchTime    = res[i]["patch_time"];
                server.patchUser    = res[i]["patch_user"];
                server.profile      = res[i]["profile"];
                server.serverType   = res[i]["server_type"];
                server.startScript  = res[i]["start_script_path"];
                server.stopScript   = res[i]["stop_script_path"];
                server.monitorScript    = res[i]["monitor_script_path"];
                server.configCenterPort = TC_Common::strto<int>(res[i]["config_center_port"]);

                server.setId = "";
                if (TC_Common::lower(res[i]["enable_set"]) == "y")
                {
                    server.setId = res[i]["set_name"] + "." +  res[i]["set_area"] + "." + res[i]["set_group"];
                }

                //获取父模版profile内容
                if (mapProfile.find(res[i]["template_name"]) == mapProfile.end())
                {
                    string sResult;
                    mapProfile[res[i]["template_name"]] = getProfileTemplate(res[i]["template_name"], sResult);
                }

                TC_Config tParent, tProfile;
                tParent.parseString(mapProfile[res[i]["template_name"]]);
                tProfile.parseString(server.profile);
                int iDefaultAsyncThreadNum = 3;
                if (server.serverType == "tars_nodejs")
                {
                    iDefaultAsyncThreadNum = 0;
                }
                int iConfigAsyncThreadNum  = TC_Common::strto<int>(TC_Common::trim(res[i]["async_thread_num"]));
                iDefaultAsyncThreadNum     = iConfigAsyncThreadNum > iDefaultAsyncThreadNum ? iConfigAsyncThreadNum : iDefaultAsyncThreadNum;
                server.asyncThreadNum      = TC_Common::strto<int>(tProfile.get("/tars/application/client<asyncthread>", TC_Common::tostr(iDefaultAsyncThreadNum)));
                tParent.joinConfig(tProfile, true);
                server.profile = tParent.tostr();

                mapAppServerTemp[sServerId] = vServers.size();
                vServers.push_back(server);
            }

            //adapter配置
            AdapterDescriptor adapter;
            adapter.adapterName = res[i]["adapter_name"];
            if (adapter.adapterName == "")
            {
                //adapter没配置，left join 后为 NULL,不放到adapters map
                continue;
            }

            adapter.threadNum       = res[i]["thread_num"];
            adapter.endpoint        = res[i]["endpoint"];
            adapter.maxConnections  = TC_Common::strto<int>(res[i]["max_connections"]);
            adapter.allowIp         = res[i]["allow_ip"];
            adapter.servant         = res[i]["servant"];
            adapter.queuecap        = TC_Common::strto<int>(res[i]["queuecap"]);
            adapter.queuetimeout    = TC_Common::strto<int>(res[i]["queuetimeout"]);
            adapter.protocol        = res[i]["protocol"];
            adapter.handlegroup     = res[i]["handlegroup"];

            vServers[mapAppServerTemp[sServerId]].adapters[adapter.adapterName] = adapter;
        }
    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR(__FUNCTION__ << " " << app << "." << serverName << "_" << nodeName
                  << " exception: " << ex.what() << "|" << sSql << endl);
        return vServers;
    }
    catch (TC_Config_Exception& ex)
    {
        TLOGERROR(__FUNCTION__ << " " << app << "." << serverName << "_" << nodeName
                  << " TC_Config_Exception exception: " << ex.what() << endl);
        throw TarsException(string("TC_Config_Exception exception: ") + ex.what());
    }

    TLOGDEBUG(app << "." << serverName << "_" << nodeName
              << " getServers affected:" << num
              << "|cost:" << (TC_TimeProvider::getInstance()->getNowMs() - iStart) << endl);

    return  vServers;

}

int DbProxy::getNodeList(const string& app, const string& serverName, vector<string>& nodeNames)
{
    int ret = 0;
    nodeNames.clear();
    try
    {
        string sSql = "select node_name from t_server_conf where application='" + _mysqlReg.escapeString(app) + "' and server_name='" + _mysqlReg.escapeString(serverName) + "'";

        tars::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);

        for (unsigned i = 0; i < res.size(); i++)
        {
            nodeNames.push_back(res[i]["node_name"]);
        }
    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR(__FUNCTION__ << " exception: " << ex.what() << endl);
        ret = -1;
    }
    return ret;
}


string DbProxy::getProfileTemplate(const string& sTemplateName, string& sResultDesc)
{
    map<string, int> mapRecursion;
    return getProfileTemplate(sTemplateName, mapRecursion, sResultDesc);
}

string DbProxy::getProfileTemplate(const string& sTemplateName, map<string, int>& mapRecursion, string& sResultDesc)
{
    try
    {
        string sSql = "select template_name, parents_name, profile from t_profile_template "
                      "where template_name='" + _mysqlReg.escapeString(sTemplateName) + "'";

        tars::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);

        if (res.size() == 0)
        {
            sResultDesc += "(" + sTemplateName + ":template not found)";
            return "";
        }

        TC_Config confMyself, confParents;
        confMyself.parseString(res[0]["profile"]);
        //mapRecursion用于避免重复继承
        mapRecursion[res[0]["template_name"]] = 1;

        if (res[0]["parents_name"] != "" && mapRecursion.find(res[0]["parents_name"]) == mapRecursion.end())
        {
            confParents.parseString(getProfileTemplate(res[0]["parents_name"], mapRecursion, sResultDesc));
            confMyself.joinConfig(confParents, false);
        }
        sResultDesc += "(" + sTemplateName + ":OK)";

        TLOGDEBUG(__FUNCTION__ << " " << sTemplateName << " " << sResultDesc << endl);

        return confMyself.tostr();
    }
    catch (TC_Mysql_Exception& ex)
    {
        sResultDesc += "(" + sTemplateName + ":" + ex.what() + ")";
        TLOGERROR(__FUNCTION__ << " exception: " << ex.what() << endl);
    }
    catch (TC_Config_Exception& ex)
    {
        sResultDesc += "(" + sTemplateName + ":" + ex.what() + ")";
        TLOGERROR(__FUNCTION__ << " TC_Config_Exception exception: " << ex.what() << endl);
    }

    return  "";
}

vector<string> DbProxy::getAllApplicationNames(string& result)
{
    vector<string> vApps;
    try
    {
        string sSql = "select distinct application from t_server_conf";

        tars::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);
        TLOGDEBUG(__FUNCTION__ << " affected:" << res.size() << endl);

        for (unsigned i = 0; i < res.size(); i++)
        {
            vApps.push_back(res[i]["application"]);
        }
    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR(__FUNCTION__ << " exception: " << ex.what() << endl);
        return vApps;
    }

    return vApps;

}

vector<vector<string> > DbProxy::getAllServerIds(string& result)
{
    vector<vector<string> > vServers;
    try
    {
        string sSql =
                      "select application, server_name, node_name, setting_state, present_state,server_type from t_server_conf";

        tars::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);
        TLOGDEBUG(__FUNCTION__ << " affected:" << res.size() << endl);

        for (unsigned i = 0; i < res.size(); i++)
        {
            vector<string> server;
            server.push_back(res[i]["application"] + "." + res[i]["server_name"] +  "_" + res[i]["node_name"]);
            server.push_back(res[i]["setting_state"]);
            server.push_back(res[i]["present_state"]);
            server.push_back(res[i]["server_type"]);
            vServers.push_back(server);
        }
    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR(__FUNCTION__ << " exception: " << ex.what() << endl);
        return vServers;
    }

    return vServers;

}
int DbProxy::getGroupId(const string& ip)
{
    bool bFind      = false;
    int iGroupId    = -1;
    string sOrder;
    string sAllowIpRule;
    string sDennyIpRule;
    vector<map<string, string> > vServerGroupInfo;
    try
    {
        {
            TC_ThreadLock::Lock lock(_mutex);
            map<string, int>::iterator it = _serverGroupCache.find(ip);
            if (it != _serverGroupCache.end())
            {
                return it->second;
            }
            vServerGroupInfo = _serverGroupRule;
        }

        for (unsigned i = 0; i < vServerGroupInfo.size(); i++)
        {
            iGroupId                    = TC_Common::strto<int>(vServerGroupInfo[i]["group_id"]);
            sOrder                      = vServerGroupInfo[i]["ip_order"];
            sAllowIpRule                = vServerGroupInfo[i]["allow_ip_rule"];
            sDennyIpRule                = vServerGroupInfo[i]["denny_ip_rule"];
            vector<string> vAllowIp     = TC_Common::sepstr<string>(sAllowIpRule, ",|;");
            vector<string> vDennyIp     = TC_Common::sepstr<string>(sDennyIpRule, ",|;");
            if (sOrder == "allow_denny")
            {
                if (TC_Common::matchPeriod(ip, vAllowIp))
                {
                    bFind = true;
                    break;
                }
            }
            else if (sOrder == "denny_allow")
            {
                if (TC_Common::matchPeriod(ip, vDennyIp))
                {
                    //在不允许的ip列表中则不属于本行所对应组  继续匹配查找
                    continue;
                }
                if (TC_Common::matchPeriod(ip, vAllowIp))
                {
                    bFind = true;
                    break;
                }
            }
        }

        if (bFind == true)
        {
            TC_ThreadLock::Lock lock(_mutex);
            _serverGroupCache[ip] = iGroupId;

            TLOGINFO("get groupId succ|ip|" << ip
                     << "|group_id|" << iGroupId << "|ip_order|" << sOrder
                     << "|allow_ip_rule|" << sAllowIpRule
                     << "|denny_ip_rule|" << sDennyIpRule
                     << "|ServerGroupCache|" << TC_Common::tostr(_serverGroupCache) << endl);

            return iGroupId;
        }
    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR(__FUNCTION__ << " exception: " << ex.what() << endl);
    }
    catch (exception& ex)
    {
        TLOGERROR(__FUNCTION__ << " " << ex.what() << endl);
    }
    return -1;
}

NodePrx DbProxy::getNodePrx(const string& nodeName)
{
    try
    {
        TC_ThreadLock::Lock lock(_NodePrxLock);

        if (_mapNodePrxCache.find(nodeName) != _mapNodePrxCache.end())
        {
            return _mapNodePrxCache[nodeName];
        }

        string sSql =
                      "select node_obj "
                      "from t_node_info "
                      "where node_name='" + _mysqlReg.escapeString(nodeName) + "' and present_state='active'";

        tars::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);
        TLOGDEBUG(__FUNCTION__ << " '" << nodeName << "' affected:" << res.size() << endl);

        if (res.size() == 0)
        {
            throw TarsNodeNotRegistryException("node '" + nodeName + "' not registered  or heartbeart timeout,please check for it");
        }

        NodePrx nodePrx;
        g_app.getCommunicator()->stringToProxy(res[0]["node_obj"], nodePrx);

        _mapNodePrxCache[nodeName] = nodePrx;

        return nodePrx;

    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR(__FUNCTION__ << " " << nodeName << " exception: " << ex.what() << endl);
        throw TarsNodeNotRegistryException(string("get node record from db error:") + ex.what());
    }
    catch (tars::TarsException& ex)
    {
        TLOGERROR(__FUNCTION__ << " " << nodeName << " exception: " << ex.what() << endl);
        throw ex;
    }

}

int DbProxy::checkRegistryTimeout(unsigned uTimeout)
{
    try
    {
        string sSql =
                      "update t_registry_info "
                      "set present_state='inactive' "
                      "where last_heartbeat < date_sub(now(), INTERVAL " + tars::TC_Common::tostr(uTimeout) + " SECOND)";

        _mysqlReg.execute(sSql);
        TLOGDEBUG(__FUNCTION__ << " (" << uTimeout  << "s) affected:" << _mysqlReg.getAffectedRows() << endl);

        return _mysqlReg.getAffectedRows();

    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR(__FUNCTION__ << " exception: " << ex.what() << endl);
        return -1;
    }

}

int DbProxy::updateRegistryInfo2Db(bool bRegHeartbeatOff)
{
    if (bRegHeartbeatOff)
    {
        TLOGDEBUG("updateRegistryInfo2Db not need to update reigstry status !" << endl);
        return 0;
    }

    map<string, string>::iterator iter;
    map<string, string> mapServantEndpoint = g_app.getServantEndpoint();
    if (mapServantEndpoint.size() == 0)
    {
        TLOGERROR("fatal error, get registry servant failed!" << endl);
        return -1;
    }

    try
    {
        string sSql = "replace into t_registry_info (locator_id, servant, endpoint, last_heartbeat, present_state, tars_version) "
                      "values ";

        string sVersion = TARS_VERSION;
        sVersion += "_";
        sVersion += SERVER_VERSION;

        for (iter = mapServantEndpoint.begin(); iter != mapServantEndpoint.end(); iter++)
        {
            TC_Endpoint locator;
            locator.parse(iter->second);

            sSql += (iter == mapServantEndpoint.begin() ? string("") : string(", ")) +
                    "('" + locator.getHost() + ":" + TC_Common::tostr<int>(locator.getPort()) + "', "
                    "'" + iter->first + "', '" + iter->second + "', now(), 'active', " +
                    "'" + _mysqlReg.escapeString(sVersion) + "')";
        }

        _mysqlReg.execute(sSql);
        TLOGDEBUG(__FUNCTION__ << " affected:" << _mysqlReg.getAffectedRows() << endl);
    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR(__FUNCTION__ << " exception: " << ex.what() << endl);
        return -1;
    }
    catch (exception& ex)
    {
        TLOGERROR(__FUNCTION__ << " exception: " << ex.what() << endl);
        return -1;
    }

    return 0;
}

int DbProxy::loadIPPhysicalGroupInfo()
{
    try
    {
        string sSql = "select group_id,ip_order,allow_ip_rule,denny_ip_rule,group_name from t_server_group_rule "
                      "order by group_id";
        tars::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);
        TLOGDEBUG(__FUNCTION__ << " get server group from db, records affected:" << res.size() << endl);


        TC_ThreadLock::Lock lock(_mutex);
        _serverGroupRule.clear();
        _serverGroupRule = res.data();

        _serverGroupCache.clear();  //规则改变 清除以前缓存
    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR(__FUNCTION__ << " exception: " << ex.what() << endl);
    }
    catch (exception& ex)
    {
        TLOGERROR(__FUNCTION__ << " " << ex.what() << endl);
    }
    return -1;
}

int DbProxy::getInfoByPatchId(const string &patchId, string &patchFile, string &md5)
{
    try
    {
        string sSql = "select tgz, md5 from t_server_patchs where id=" + patchId;
        tars::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);

        if (res.size() == 0)
        {
            TLOGERROR(__FUNCTION__ << " get patch tgz, md5 from db error, no records! patchId=" << patchId << endl);
            return -1;
        }

        patchFile = res[0]["tgz"];
        md5 = res[0]["md5"];

        return 0;
    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR(__FUNCTION__ << " exception: " << ex.what() << endl);
    }
    catch (exception& ex)
    {
        TLOGERROR(__FUNCTION__ << " " << ex.what() << endl);
    }
    return -1;
}

int DbProxy::updatePatchByPatchId(const string &application, const string & serverName, const string & nodeName, const string & patchId, const string & user, const string &patchType, bool succ)
{
    try
    {
        string sql = "update t_server_patchs set publish='1',publish_user='" + user 
            + "',publish_time=now(),lastuser='"+ user +"' where id=" + patchId;

        _mysqlReg.execute(sql);

        return 0;
    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR(__FUNCTION__ << " exception: " << ex.what() << endl);
    }
    catch (exception& ex)
    {
        TLOGERROR(__FUNCTION__ << " " << ex.what() << endl);
    }
    return -1;
}

