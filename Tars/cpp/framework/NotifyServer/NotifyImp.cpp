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

#include "NotifyImp.h"
#include "jmem/jmem_hashmap.h"
#include "NotifyServer.h"

extern TC_Config * g_pconf;
extern TarsHashMap<NotifyKey, NotifyInfo, ThreadLockPolicy, FileStorePolicy> * g_notifyHash;

void NotifyImp::loadconf()
{
    TC_DBConf tcDBConf;
    tcDBConf.loadFromMap(g_pconf->getDomainMap("/tars/db"));

    _mysqlConfig.init(tcDBConf);

    _sql = (*g_pconf)["/tars/<sql>"];
    _maxPageSize = TC_Common::strto<size_t>((*g_pconf)["/tars/hash<max_page_size>"]);
    _maxPageNum  = TC_Common::strto<size_t>((*g_pconf)["/tars/hash<max_page_num>"]);

    map<string, string> m = g_pconf->getDomainMap("/tars/filter"); 
    for(map<string,string>::iterator it= m.begin();it != m.end();it++)
    {
        vector<string> vFilters = TC_Common::sepstr<string>(it->second,";|");
        _setFilter[it->first].insert(vFilters.begin(),vFilters.end()); 
    }
    
}

void NotifyImp::initialize()
{
    loadconf();
}

bool NotifyImp::IsdbTableExist(const string& sTbName)
{
    try
    {
        TC_Mysql::MysqlData tTotalRecord = _mysqlConfig.queryRecord("show tables like '%"+sTbName+"%'");

        TLOGINFO("NotifyImp::IsdbTableExist show tables like '%" + sTbName + "%|affected:" << tTotalRecord.size() << endl);

        if (tTotalRecord.size() > 0)
        {
            return true;
        }
        else
        {
            TLOGERROR("NotifyImp::IsdbTableExist sTbName:" << sTbName << "|doesn't exist" << endl);
            return false;
        }

    }
    catch(TC_Mysql_Exception& ex)
    {
        TLOGERROR("NotifyImp::IsdbTableExist exception:" << ex.what() << endl);
        return false;
    }
    catch(...)
    {
        TLOGERROR("NotifyImp::IsdbTableExist unknown exception." << endl);
        return false;
    }
}

void NotifyImp::creatTb(const string &sTbName)
{
    try
    {
        if (!IsdbTableExist(sTbName))
        {
            string sSql = TC_Common::replace(_sql, "${TABLE}",sTbName);

            _mysqlConfig.execute(sSql);

            TLOGDEBUG("NotifyImp::creatTb sSql:" << sSql << endl);
        }
    }
    catch (TC_Mysql_Exception& ex)
    {
        TLOGERROR("NotifyImp::creatTb exception:" << ex.what() << endl);
    }
    catch(...)
    {
        TLOGERROR("NotifyImp::IsdbTableExist unknown exception." << endl);
    }
}

bool NotifyImp::IsNeedFilte(const string& sServerName,const string& sResult)
{
    if(_setFilter.find(sServerName) != _setFilter.end())
    {
        set<string>& setFilter = _setFilter.find(sServerName)->second;
        for(set<string>::iterator it = setFilter.begin(); it != setFilter.end();it++)
        {
            if(sResult.find(*it) != string::npos)
            {
                return true;
            }
        }    
    }

    const string sDefault = "default";
    if(_setFilter.find(sDefault) != _setFilter.end())
    {
        set<string>& setFilter = _setFilter.find(sDefault)->second;
        for(set<string>::iterator it = setFilter.begin(); it != setFilter.end();it++)
        {
            if(sResult.find(*it) != string::npos)
            {
                return true;
            }
        }        
    }

    return false;
}

void NotifyImp::reportServer(const string& sServerName, const string& sThreadId, const string& sResult, tars::TarsCurrentPtr current)
{
    TLOGDEBUG("NotifyImp::reportServer sServerName:" << sServerName << "|ip:" << current->getIp() << "|sThreadId:" << sThreadId << "|sResult:" << sResult << endl);
    //DLOG << "NotifyImp::reportServer sServerName:" << sServerName << "|ip:" << current->getIp() << "|sThreadId:" << sThreadId << "|sResult:" << sResult << endl;

    if(IsNeedFilte(sServerName,sResult))
    {
        TLOGWARN("NotifyImp::reportServer sServerName:" << sServerName << "|ip:" << current->getIp() << "|sThreadId:" << sThreadId << "|sResult:" << sResult <<"|filted"<< endl);
        return;
    }
    
    ReportInfo info;
    info.sApp = sServerName;
    info.sServer = sServerName;

    vector<string> vModule = TC_Common::sepstr<string>(sServerName,".");
    if(vModule.size() >= 2)
    {
        info.sApp = vModule[0];
        info.sServer = vModule[1];
    }

    info.eType     = REPORT;
    info.sSet      = g_app.getLoadDbThread()->getSetName(sServerName + current->getIp());
    info.sMessage  = sResult;
    info.sThreadId = sThreadId;

    reportNotifyInfo(info, current);
    
    return;
}

void NotifyImp::notifyServer(const string& sServerName, NOTIFYLEVEL level, const string& sMessage, tars::TarsCurrentPtr current)
{
    ReportInfo info;
    info.sApp    = sServerName;
    info.sServer = sServerName;


    vector<string> vModule = TC_Common::sepstr<string>(sServerName, ".");
    if (vModule.size() >= 2)
    {
        info.sApp = vModule[0];
        info.sServer = vModule[1];
    }

    info.eType    = NOTIFY;
    info.sSet     = g_app.getLoadDbThread()->getSetName(sServerName + current->getIp());
    info.sMessage = sMessage;
    info.eLevel   = level;

    reportNotifyInfo(info, current);
    
    return;
}

int NotifyImp::getNotifyInfo(const tars::NotifyKey & stKey,tars::NotifyInfo &stInfo,tars::TarsCurrentPtr current)
{
    int iRet = g_notifyHash->get(stKey, stInfo);

    ostringstream os;
    stKey.displaySimple(os);
    os << "|";
    stInfo.displaySimple(os);

    TLOGDEBUG("NotifyImp::getNotifyInfo iRet:" << iRet << "|os:" << os.str() << endl);

    return iRet;
}

void NotifyImp::reportNotifyInfo(const tars::ReportInfo & info, tars::TarsCurrentPtr current)
{
    switch (info.eType)
    {
        case (REPORT):
        {
            TLOGDEBUG("NotifyImp::reportNotifyInfo reportServer:" << info.sApp + "." + info.sServer << "|sSet:" << info.sSet << "|sContainer:" << info.sContainer << "|ip:" << current->getIp() 
                << "|sThreadId:" << info.sThreadId << "|sMessage:" << info.sMessage << endl);

        //   DLOG << "NotifyImp::reportNotifyInfo reportServer:" << info.sApp + "." + info.sServer << "|sSet:" << info.sSet << "|sContainer:" << info.sContainer << "|ip:" << current->getIp() 
           //    << "|sThreadId:" << info.sThreadId << "|sMessage:" << info.sMessage << endl;

            if (IsNeedFilte(info.sApp + info.sServer, info.sMessage))
            {
                TLOGWARN("NotifyImp::reportNotifyInfo reportServer:" << info.sApp + "." + info.sServer << "|sSet:" << info.sSet << "|sContainer:" << info.sContainer << "|ip:" << current->getIp() 
                    << "|sThreadId:" << info.sThreadId << "|sMessage:" << info.sMessage << "|filted" << endl);

                return;
            }

            string sql;
            TC_Mysql::RECORD_DATA rd;

            rd["application"]    = make_pair(TC_Mysql::DB_STR, info.sApp);
            rd["server_name"]    = make_pair(TC_Mysql::DB_STR, info.sServer);
            rd["container_name"] = make_pair(TC_Mysql::DB_STR, info.sContainer);
            rd["server_id"]      = make_pair(TC_Mysql::DB_STR, info.sApp +"."+ info.sServer + "_" + current->getIp());
            rd["node_name"]      = make_pair(TC_Mysql::DB_STR, current->getIp());
            rd["thread_id"]      = make_pair(TC_Mysql::DB_STR, info.sThreadId);

            if (!info.sSet.empty())
            {
                vector<string> v = TC_Common::sepstr<string>(info.sSet, ".");
                if (v.size() != 3 || (v.size() == 3 && (v[0] == "*" || v[1] == "*")))
                {
                    TLOGERROR("NotifyImp::reportNotifyInfo bad set name:" << info.sSet << endl);
                }
                else
                {
                    rd["set_name"]  = make_pair(TC_Mysql::DB_STR, v[0]);
                    rd["set_area"]  = make_pair(TC_Mysql::DB_STR, v[1]);
                    rd["set_group"] = make_pair(TC_Mysql::DB_STR, v[2]);
                }
                
            }
            else
            {
                rd["set_name"]  = make_pair(TC_Mysql::DB_STR, "");
                rd["set_area"]  = make_pair(TC_Mysql::DB_STR, "");
                rd["set_group"] = make_pair(TC_Mysql::DB_STR, "");
            }
            
            rd["result"]     = make_pair(TC_Mysql::DB_STR, info.sMessage);
            rd["notifytime"] = make_pair(TC_Mysql::DB_INT, "now()");
            string sTable = "t_server_notifys";
            try
            {
                _mysqlConfig.insertRecord(sTable, rd);
            }
            catch (TC_Mysql_Exception& ex)
            {
                string err = string(ex.what());
                if (std::string::npos != err.find("doesn't exist"))
                {
                    creatTb(sTable);
                }
                else
                {
                    string sInfo = string("insert2Db exception") + "|" + ServerConfig::LocalIp + "|" + ServerConfig::Application + "." + ServerConfig::ServerName;
                    TARS_NOTIFY_ERROR(sInfo);
                }
                TLOGERROR("NotifyImp::reportNotifyInfo insert2Db exception:" << ex.what() << endl);
            }
            catch (exception& ex)
            {
                TLOGERROR("NotifyImp::reportNotifyInfo insert2Db exception:" << ex.what() << endl);
                string sInfo = string("insert2Db exception") + "|" + ServerConfig::LocalIp + "|" + ServerConfig::Application + "." + ServerConfig::ServerName;
                //TARS_NOTIFY_ERROR(sInfo);
            }
        }
        case (NOTIFY):
        {
            TLOGDEBUG("NotifyImp::reportNotifyInfo  notifyServer:" << info.sApp + "." + info.sServer << "|sSet:" << info.sSet << "|sContainer:" << info.sContainer << "|ip:" << current->getIp() 
                << "|eLevel:" << tars::etos(info.eLevel) << "|sMessage:" << info.sMessage << endl);

            if (info.eLevel == NOTIFYERROR)
            {
               // FDLOG("NOTIFYERROR") << "NotifyImp::reportNotifyInfo  notifyServer:" << info.sApp + "." + info.sServer << "|sSet:" << info.sSet << "|sContainer:" << info.sContainer << "|ip:" << current->getIp() 
               //     << "|eLevel:" << tars::etos(info.eLevel) << "|sMessage:" << info.sMessage << endl;
            }
            else
            {
               // DLOG << "NotifyImp::reportNotifyInfo  notifyServer:" << info.sApp + "." + info.sServer << "|sSet:" << info.sSet << "|sContainer:" << info.sContainer << "|ip:" << current->getIp() 
                //    << "|eLevel:" << tars::etos(info.eLevel) << "|sMessage:" << info.sMessage << endl;
            }

            string sServerId = info.sApp + info.sServer + "_" + current->getIp();

            NotifyKey stKey0;
            stKey0.name = info.sApp + info.sServer;
            stKey0.ip   = current->getIp();
            stKey0.page = 0;

            NotifyInfo stInfo0;

            NotifyItem stItem;
            stItem.sTimeStamp = TC_Common::now2str("%Y-%m-%d %H:%M:%S");
            stItem.sServerId  = sServerId;
            stItem.iLevel     = info.eLevel;
            stItem.sMessage   = info.sMessage;

            int iRet;
            iRet = g_notifyHash->get(stKey0, stInfo0);
            TLOGDEBUG("get " << sServerId << " page " << stKey0.page << " info return :" << iRet << endl);
            if (iRet == TC_HashMap::RT_LOAD_DATA_ERR)
            {
                return;
            }

            if (stInfo0.notifyItems.size() < _maxPageSize)
            {
                stInfo0.notifyItems.push_back(stItem);
                iRet = g_notifyHash->set(stKey0, stInfo0);
                TLOGDEBUG("NotifyImp::reportNotifyInfo set sServerId:" << sServerId << "|page:" << stKey0.page << "|iRet:" << iRet << endl);
                return;
            }

            //0页置换出去
            NotifyKey stKeyReplPage = stKey0;
            stKeyReplPage.page = (stInfo0.nextpage + 1) % _maxPageNum;
            if (stKeyReplPage.page == 0)
            {
                stKeyReplPage.page = 1;
            }
            iRet = g_notifyHash->set(stKeyReplPage, stInfo0);

            //修改0页
            stInfo0.nextpage = stKeyReplPage.page;
            stInfo0.notifyItems.clear();
            stInfo0.notifyItems.push_back(stItem);
            iRet = g_notifyHash->set(stKey0, stInfo0);
        }
        default:
            break;
    }

    return;
}
