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

#include "ServerLimitResource.h"
#include "util/tc_timeprovider.h"
#include "servant/TarsLogger.h"
#include "NodeRollLogger.h"
#include "util.h"

ServerLimitResource::ServerLimitResource( int iMaxCount,int iTimeInterval/*分钟*/ ,int iExpiredTime/*分钟*/,const string& app,const string& servername)
: _maxExcStopCount(iMaxCount)
, _coreLimitTimeInterval(iTimeInterval*60)
, _coreLimitExpiredTime(iExpiredTime*60)
, _appName(app)
, _serverName(servername)
{
    resetRunTimeData();
}

void ServerLimitResource::resetRunTimeData()
{
    time_t tNow          = TNOW;
    _closeCore           = false;
    _curExcStopCount     = 0;
    _excStopRecordTime   = tNow;
    _enableCoreLimitTime = 0;
}

void ServerLimitResource::setLimitCheckInfo(int iMaxCount,int iTimeInterval,int iExpiredTime)
{
    TC_ThreadLock::Lock lock(*this);
    _maxExcStopCount       = (iMaxCount>0)?iMaxCount:1;
    _coreLimitTimeInterval = (iTimeInterval>0?iTimeInterval:1)*60;
    _coreLimitExpiredTime  = (iExpiredTime>0?iExpiredTime:1)*60;
}

void ServerLimitResource::addExcStopRecord()
{
    time_t tNow     = TNOW;

    TC_ThreadLock::Lock lock(*this);
    //在允许的时间段内才检查是否增加计数和关闭core属性
    NODE_LOG("core")->debug() << FILE_FUN<< (_appName+"."+_serverName) << "|before|" << tNow << "|" << _excStopRecordTime
            << "|" << _coreLimitTimeInterval << "|" << _curExcStopCount << "|" << _closeCore << "|" << _maxExcStopCount << endl;
    if((tNow - _excStopRecordTime) < _coreLimitTimeInterval)
    {
        _curExcStopCount++;
        if(!_closeCore && (_curExcStopCount >= _maxExcStopCount))
        {
            _closeCore           = true;
            _enableCoreLimitTime = tNow;
        }
    }
    else
    {
        //重新计数
        _curExcStopCount   = 1;
        _excStopRecordTime = tNow;
    }
    NODE_LOG("core")->debug() << FILE_FUN << (_appName+"."+_serverName) << "|after|" << tNow << "|" << _excStopRecordTime
            << "|" << _coreLimitTimeInterval << "|" << _curExcStopCount << "|" << _closeCore << "|" << _maxExcStopCount << endl;
}

int ServerLimitResource::IsCoreLimitNeedClose(bool& bClose)
{
    time_t tNow     = TNOW;

    int ret = 0;
    if(_closeCore)
    {
        if((tNow -_enableCoreLimitTime) < _coreLimitExpiredTime)
        {
            bClose = true;
            ret    = 1;
        }
        //如果core屏蔽过期，则重新打开
        else
        {
            TC_ThreadLock::Lock lock(*this);
            NODE_LOG("core")->debug() <<FILE_FUN <<" expired|"<<(_appName+"."+_serverName)<<"|"<<tNow<<"|"<<_enableCoreLimitTime<<endl;
            _curExcStopCount     = 0;
            _closeCore           = false;
            _excStopRecordTime   = 0;
            _enableCoreLimitTime = 0;
            bClose = false;
            ret = 2;
        }
    }
    else
    {
        bClose = false;
        ret    = 1;
    }
    NODE_LOG("core")->debug() <<FILE_FUN<<"|"<<(_appName+"."+_serverName)<<"|"<<_closeCore<<"|"<< _curExcStopCount<<"|"<<tNow<<"|"<<_enableCoreLimitTime
            << "|" << ret <<"|" << std::boolalpha << bClose << endl;
    return ret;
}

