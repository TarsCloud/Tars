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

#include "QueryImp.h"
#include "util/tc_clientsocket.h"

extern TC_Config * g_pconf;

void QueryImp::initialize()
{
    TLOGDEBUG("begin QueryImp init"<<endl);

    //初始化配置db连接
    _db.init(g_pconf);

}

vector<EndpointF> QueryImp::findObjectById(const string & id, tars::TarsCurrentPtr current)
{
    vector<EndpointF> eps = _db.findObjectById(id);

    ostringstream os;
    doDaylog(FUNID_findObjectById,id,eps,vector<EndpointF>(),current,os);

    return eps;
}

tars::Int32 QueryImp::findObjectById4Any(const std::string & id,vector<tars::EndpointF> &activeEp,vector<tars::EndpointF> &inactiveEp,tars::TarsCurrentPtr current)
{
    int iRet = _db.findObjectById4All(id, activeEp, inactiveEp);

    ostringstream os;
    doDaylog(FUNID_findObjectById4Any,id,activeEp,inactiveEp,current,os);

    return iRet;
}

int QueryImp::findObjectById4All(const std::string & id, vector<tars::EndpointF> &activeEp,vector<tars::EndpointF> &inactiveEp,tars::TarsCurrentPtr current)
{
    ostringstream os;

    int iRet = _db.findObjectByIdInGroupPriority(id,current->getIp(),activeEp, inactiveEp,os);

    doDaylog(FUNID_findObjectById4All,id,activeEp,inactiveEp,current,os);

    return iRet;
}

int QueryImp::findObjectByIdInSameGroup(const std::string & id, vector<tars::EndpointF> &activeEp,vector<tars::EndpointF> &inactiveEp, tars::TarsCurrentPtr current)
{
    ostringstream os;
    TLOGINFO(__FUNCTION__ << ":" << __LINE__ << "|" << id << "|" << current->getIp() << endl);

    int iRet = _db.findObjectByIdInGroupPriority(id, current->getIp(), activeEp, inactiveEp, os);

    doDaylog(FUNID_findObjectByIdInSameGroup,id,activeEp,inactiveEp,current,os);

    return iRet;
}

Int32 QueryImp::findObjectByIdInSameStation(const std::string & id, const std::string & sStation, vector<tars::EndpointF> &activeEp, vector<tars::EndpointF> &inactiveEp, tars::TarsCurrentPtr current)
{
    ostringstream os;

    int iRet = _db.findObjectByIdInSameStation(id, sStation, activeEp, inactiveEp, os);

    doDaylog(FUNID_findObjectByIdInSameStation,id,activeEp,inactiveEp,current,os);

    return iRet;
}

Int32 QueryImp::findObjectByIdInSameSet(const std::string & id,const std::string & setId,vector<tars::EndpointF> &activeEp,vector<tars::EndpointF> &inactiveEp, tars::TarsCurrentPtr current)
{
    vector<string> vtSetInfo = TC_Common::sepstr<string>(setId,".");

    if (vtSetInfo.size()!=3 ||(vtSetInfo.size()==3&&(vtSetInfo[0]=="*"||vtSetInfo[1]=="*")))
    {
        TLOGERROR("QueryImp::findObjectByIdInSameSet:|set full name error[" << id << "_" << setId <<"]|" << current->getIp() << endl);
        return -1;
    }

    ostringstream os;
    int iRet = _db.findObjectByIdInSameSet(id, vtSetInfo, activeEp, inactiveEp, os);
    if (-1 == iRet)
    {
        //未启动set，启动ip分组策略
        return findObjectByIdInSameGroup(id, activeEp, inactiveEp, current);
    }
    else if (-2 == iRet)
    {
        //启动了set，但未找到任何服务节点
        TLOGERROR("QueryImp::findObjectByIdInSameSet |no one server found for [" << id << "_" << setId <<"]|" << current->getIp() << endl);
        return -1;
    }
    else if (-3 == iRet)
    {
        //启动了set，但未找到任何地区set,严格上不应该出现此类情形,配置错误或主调设置错误会引起此类错误
        TLOGERROR("QueryImp::findObjectByIdInSameSet |no set area found [" << id << "_" << setId <<"]|" << current->getIp()  << endl);
        return -1;
    }

    doDaylog(FUNID_findObjectByIdInSameSet,id,activeEp,inactiveEp,current,os,setId);

    return iRet;
}

void QueryImp::doDaylog(const FUNID eFnId,const string& id,const vector<tars::EndpointF> &activeEp, const vector<tars::EndpointF> &inactiveEp, const tars::TarsCurrentPtr& current,const ostringstream& os,const string& sSetid)
{
    string sEpList;
    for(size_t i = 0; i < activeEp.size(); i++)
    {
        if(0 != i)
        {
            sEpList += ";";
        }
        sEpList += activeEp[i].host + ":" + TC_Common::tostr(activeEp[i].port);
    }

    sEpList += "|";

    for(size_t i = 0; i < inactiveEp.size(); i++)
    {
        if(0 != i)
        {
            sEpList += ";";
        }
        sEpList += inactiveEp[i].host + ":" + TC_Common::tostr(inactiveEp[i].port);
    }

    switch(eFnId)
    {
        case FUNID_findObjectById4All:
        case FUNID_findObjectByIdInSameGroup:
        {
            FDLOG("query_idc") << eFunTostr(eFnId)<<"|"<<current->getIp() << "|"<< current->getPort() << "|" << id << "|" <<sSetid << "|" << sEpList <<os.str()<< endl;
        }
        break;
        case FUNID_findObjectByIdInSameSet:
        {
            FDLOG("query_set") << eFunTostr(eFnId)<<"|"<<current->getIp() << "|"<< current->getPort() << "|" << id << "|" <<sSetid << "|" << sEpList <<os.str()<< endl;
        }
        break;
        case FUNID_findObjectById4Any:
        case FUNID_findObjectById:
        case FUNID_findObjectByIdInSameStation:
        default:
        {
            FDLOG("query") << eFunTostr(eFnId)<<"|"<<current->getIp() << "|"<< current->getPort() << "|" << id << "|" <<sSetid << "|" << sEpList <<os.str()<< endl;
        }
        break;
    }
}

string QueryImp::eFunTostr(const FUNID eFnId)
{
    string sFun = "";
    switch(eFnId)
    {
        case FUNID_findObjectByIdInSameGroup:
        {
            sFun = "findObjectByIdInSameGroup";
        }
        break;
        case FUNID_findObjectByIdInSameSet:
        {
            sFun = "findObjectByIdInSameSet";
        }
        break;
        case FUNID_findObjectById4Any:
        {
            sFun = "findObjectById4All";
        }
        break;
        case FUNID_findObjectById:
        {
            sFun = "findObjectById";
        }
        break;
        case FUNID_findObjectById4All:
        {
            sFun = "findObjectById4All";
        }
        break;
        case FUNID_findObjectByIdInSameStation:
        {
            sFun = "findObjectByIdInSameStation";
        }
        break;
        default:
            sFun = "UNKNOWN";
        break;
    }
    return sFun;
}

