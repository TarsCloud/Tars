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

#include "StatServer.h"
#include "servant/AppCache.h"
#include "StatImp.h"

void StatServer::initialize()
{
    try
    {
        //关闭远程日志
        TarsTimeLogger::getInstance()->enableRemote("", false);

        //增加对象
        addServant<StatImp>( ServerConfig::Application + "." + ServerConfig::ServerName +".StatObj" );

        _iInsertInterval  = TC_Common::strto<int>(g_pconf->get("/tars/hashmap<insertInterval>","5"));
        if(_iInsertInterval < 5)
        {
            _iInsertInterval = 5;
        }

        //获取业务线程个数
        string sAdapter = ServerConfig::Application + "." + ServerConfig::ServerName + ".StatObjAdapter";
        string sHandleNum = "/tars/application/server/";
        sHandleNum += sAdapter;
        sHandleNum += "<threads>";

        int iHandleNum = TC_Common::strto<int>(g_pconf->get(sHandleNum, "20"));
        vector<int64_t> vec;
        vec.resize(iHandleNum);
        for(size_t i =0; i < vec.size(); ++i)
        {
            vec[i] = 0;
        }

        _vBuffer[0]=vec;
        _vBuffer[1]=vec;

        TLOGDEBUG("StatServer::initialize iHandleNum:" << iHandleNum<< endl);

        initHashMap();

        string s("");
        _iSelectBuffer = getSelectBufferFromFlag(s);

        TLOGDEBUG("StatServer::initialize iSelectBuffer:" << _iSelectBuffer<< endl);

        vector<string> vIpGroup = g_pconf->getDomainKey("/tars/masteripgroup");
        for (unsigned i = 0; i < vIpGroup.size(); i++)
        {
            vector<string> vOneGroup = TC_Common::sepstr<string>(vIpGroup[i], ";", true);
            if (vOneGroup.size() < 2)
            {
                TLOGERROR("StatImp::initialize wrong masterip:" << vIpGroup[i] << endl);
                continue;
            }
            _mVirtualMasterIp[vOneGroup[0]] = vOneGroup[1];
        }

        _sRandOrder = AppCache::getInstance()->get("RandOrder");
        TLOGDEBUG("StatImp::initialize randorder:" << _sRandOrder << endl);

        _pReapSSDThread = new ReapSSDThread();
        _pReapSSDThread->start();

        TARS_ADD_ADMIN_CMD_PREFIX("tars.tarsstat.randorder", StatServer::cmdSetRandOrder);
    }
    catch ( exception& ex )
    {
        TLOGERROR("StatServer::initialize catch exception:" << ex.what() << endl);
        exit( 0 );
    }
    catch ( ... )
    {
        TLOGERROR("StatServer::initialize unknow  exception  catched" << endl);
        exit( 0 );
    }
}

map<string, string>& StatServer::getVirtualMasterIp(void)
{
    return _mVirtualMasterIp;
}

string StatServer::getRandOrder(void)
{
    return _sRandOrder;
}

string StatServer::getClonePath(void)
{
    return _sClonePath;
}

int StatServer::getInserInterv(void)
{
    return _iInsertInterval;
}

bool StatServer::getSelectBuffer(int iIndex, int64_t iInterval)
{
    int64_t iNow = TNOW;
    bool bFlag = true;
    vector<int64_t> &vBuffer = _vBuffer[iIndex];

    for(vector<int64_t>::size_type i=0; i != vBuffer.size(); i++)
    {
        //if(vBuffer[i] != 0 && (iNow - vBuffer[i]) > iInterval)
        if((iNow - vBuffer[i]) < iInterval)
        {
            bFlag = false;
        }
    }

    if(bFlag)
    {
        return true;
    }

    return false;
}
int StatServer::getSelectBufferFromFlag(const string& sFlag)
{
    if(sFlag.length()!=0)
    {
        return (TC_Common::strto<int>(sFlag.substr(2,2))/_iInsertInterval)%2;
    }
    else
    {
        time_t tTime=0;
        string sDate="",flag="";
        getTimeInfo(tTime,sDate,flag);
        return (TC_Common::strto<int>(flag.substr(2,2))/_iInsertInterval)%2;
    }
}
bool StatServer::cmdSetRandOrder(const string& command, const string& params, string& result)
{
    try
    {
        TLOGINFO("StatServer::cmdSetRandOrder:" << command << " " << params << endl);

        _sRandOrder = params;

        result = "set RandOrder [" + _sRandOrder + "] ok";

        AppCache::getInstance()->set("RandOrder",_sRandOrder);
    }
    catch (exception &ex)
    {
        result = ex.what();
    }
    return true;
}

void StatServer::initHashMap()
{
    TLOGDEBUG("StatServer::initHashMap begin" << endl);

    int iHashMapNum      = TC_Common::strto<int>(g_pconf->get("/tars/hashmap<hashmapnum>","1"));

    _iBuffNum = iHashMapNum;

    _hashmap = new StatHashMap *[2];

    for(int k = 0; k < 2; ++k)
    {
        _hashmap[k] = new StatHashMap[iHashMapNum]();
    }

    int iMinBlock       = TC_Common::strto<int>(g_pconf->get("/tars/hashmap<minBlock>","128"));
    int iMaxBlock       = TC_Common::strto<int>(g_pconf->get("/tars/hashmap<maxBlock>","256"));
    float iFactor       = TC_Common::strto<float>(g_pconf->get("/tars/hashmap<factor>","2"));
    int iSize           = TC_Common::toSize(g_pconf->get("/tars/hashmap<size>"), 1024*1024*256);

    _sClonePath         = ServerConfig::DataPath + "/" + g_pconf->get("/tars/hashmap<clonePatch>","clone");

    if(!TC_File::makeDirRecursive(_sClonePath))
    {
        TLOGERROR("cannot create hashmap file " << _sClonePath << endl);
        exit(0);
    }

    TLOGDEBUG("StatServer::initHashMap init multi hashmap begin..." << endl);

    char a[26];
    int iChar = 0;
    for(int n = 0; n < 26; n++)
    {
        a[n] = 'a' + n;
    }

    for(int i = 0; i < 2; ++i)
    {
        for(int k = 0; k < iHashMapNum; ++k)
        {
            string sFileConf("/tars/hashmap<file");
            string sFileDefault("hashmap");

            sFileConf += TC_Common::tostr(i);
            sFileConf += TC_Common::tostr(k);
            sFileConf += ">";

            sFileDefault += TC_Common::tostr(i);
            sFileDefault += TC_Common::tostr(k);
            sFileDefault += ".txt";

            string sHashMapFile = ServerConfig::DataPath + "/" + g_pconf->get(sFileConf, sFileDefault);

            string sPath        = TC_File::extractFilePath(sHashMapFile);
            
            if(!TC_File::makeDirRecursive(sPath))
            {
                TLOGERROR("cannot create hashmap file " << sPath << endl);
                exit(0);
            }

            try
            {
                TLOGINFO("initDataBlockSize size: " << iMinBlock << ", " << iMaxBlock << ", " << iFactor << endl);

                _hashmap[i][k].initDataBlockSize(iMinBlock,iMaxBlock,iFactor);

                if(TC_File::isFileExist(sHashMapFile))
                {
                    iSize = TC_File::getFileSize(sHashMapFile);
                }
                else
                {
                    int fd = open(sHashMapFile.c_str(), O_CREAT|O_EXCL|O_RDWR, 0666);
                    if(fd == -1)
                    {
                        if(errno != EEXIST)
                        {
                            throw TC_Exception("open1 file '" + sHashMapFile + "' error", errno);
                        }
                        else
                        {
                            fd = open(sHashMapFile.c_str(), O_CREAT|O_RDWR, 0666);
                            if(fd == -1)
                            {
                                throw TC_Exception("open2 file '" + sHashMapFile + "' error", errno);
                            }
                        }
                    }

                    lseek(fd, iSize-1, SEEK_SET);
                    write(fd,"\0",1);
                    if(fd != -1)
                    {
                       close(fd); 
                    }
                }

                key_t key = ftok(sHashMapFile.c_str(), a[iChar%26]);

                iChar++;

                TLOGDEBUG("init hash mem，shm key: 0x" << hex << key << dec << endl);

                //_hashmap[i][k].initStore( sHashMapFile.c_str(), iSize );
                _hashmap[i][k].initStore(key, iSize);
                _hashmap[i][k].setAutoErase(false);

                TLOGINFO("\n" <<  _hashmap[i][k].desc() << endl);
            }
            catch(TC_HashMap_Exception &e)
            {
               TC_File::removeFile(sHashMapFile,false);
               throw runtime_error(e.what());
            }
            
        }
    }

    TLOGDEBUG("StatServer::initHashMap init multi hashmap end..." << endl);
}

void StatServer::destroyApp()
{
    if(_pReapSSDThread)
    {
        delete _pReapSSDThread;
        _pReapSSDThread = NULL;
    }

    for(int i = 0; i < 2; ++i)
    {
        delete [] _hashmap[i];
    }

    delete [] _hashmap;

    TLOGDEBUG("StatServer::destroyApp ok" << endl);
}

void StatServer::getTimeInfo(time_t &tTime,string &sDate,string &sFlag)
{
    string sTime,sHour,sMinute;
    time_t t    = TC_TimeProvider::getInstance()->getNow();
    t           = (t/(_iInsertInterval*60))*_iInsertInterval*60; //要求必须为loadIntev整数倍
    tTime       = t;
    t           = (t%3600 == 0?t-60:t);                           //要求将9点写作0860
    sTime       = TC_Common::tm2str(t,"%Y%m%d%H%M");
    sDate       = sTime.substr(0,8);
    sHour       = sTime.substr(8,2);
    sMinute     = sTime.substr(10,2);
    sFlag       = sHour +  (sMinute=="59"?"60":sMinute);    //要求将9点写作0860
}


