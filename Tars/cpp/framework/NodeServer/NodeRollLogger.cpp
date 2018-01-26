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

#include "NodeRollLogger.h"
#include "servant/Communicator.h"


/////////////////////////////////////////////////////////////////////////////////////
RollLoggerManager::RollLoggerManager()
: _maxSize(1024 * 1024 * 50), _maxNum(10)
{
}

RollLoggerManager::~RollLoggerManager()
{

    map<string, RollLogger*>::iterator it = _loggers.begin();
    while(it != _loggers.end())
    {
        delete it->second;
        ++it;
    }
    _loggers.clear();
}

void RollLoggerManager::setLogInfo(const string &sApp, const string &sServer, const string &sLogpath, int iMaxSize, int iMaxNum, const CommunicatorPtr &comm, const string &sLogObj)
//void RollLoggerManager::setLogInfo(const string &sApp, const string &sServer, const string &sLogpath, int iMaxSize, int iMaxNum, const string &sLogObj)
{
    _app       = sApp;
    _server    = sServer;
    _logpath   = sLogpath;

    _maxSize   = iMaxSize;
    _maxNum    = iMaxNum;
    _comm      = comm;
    _logObj    = sLogObj;
    //生成目录
    TC_File::makeDirRecursive(_logpath + "/" + _app + "/" + _server);

    _local.start(1);
}


void RollLoggerManager::sync(RollLogger *pRollLogger, bool bSync)
{
    if(bSync)
    {
        pRollLogger->unSetupThread();
    }
    else
    {
        pRollLogger->setupThread(&_local);
    }
}

/*
void NodeRollLogger::enableDyeing(bool bEnable, const string& sDyeingKey)
{
    _logger.getRoll()->enableDyeing(bEnable, sDyeingKey);
}
*/

RollLoggerManager::RollLogger* RollLoggerManager::logger(const string &sFile)
{
    Lock lock(*this);
    map<string, RollLogger*>::iterator it = _loggers.find(sFile);
    if( it == _loggers.end())
    {
        RollLogger *p = new RollLogger();
        //p->modFlag(RollLogger::HAS_MTIME);
        initRollLogger(p, sFile, "%Y%m%d");
        _loggers[sFile] = p;
        return p;
    }

    return it->second;
}

void RollLoggerManager::initRollLogger(RollLogger *pRollLogger, const string &sFile, const string &sFormat)
{

    //初始化本地循环日志
    pRollLogger->init(_logpath + "/" + _app + "/" + _server + "/" + _app + "." + _server + "_" + sFile, _maxSize, _maxNum);
    pRollLogger->modFlag(TC_DayLogger::HAS_TIME, false);
    pRollLogger->modFlag(TC_DayLogger::HAS_TIME|TC_DayLogger::HAS_LEVEL|TC_DayLogger::HAS_PID, true);

    //设置为异步
    sync(pRollLogger, false);


    //设置染色日志信息
    pRollLogger->getWriteT().setDyeingLogInfo(_app, _server, _logpath, _maxSize, _maxNum, _comm, _logObj);
//    pRollLogger->getWriteT().setDyeingLogInfo(_app, _server, _logpath, _maxSize, _maxNum, _logObj);
}
