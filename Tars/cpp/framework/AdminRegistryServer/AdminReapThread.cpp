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

#include "AdminReapThread.h"

extern TC_Config * g_pconf;

AdminReapThread::AdminReapThread()
: _terminate(false)
, _timeout(150)
{
}

AdminReapThread::~AdminReapThread()
{
    if (isAlive())
    {
        terminate();
        notifyAll();
        getThreadControl().join();
    }
}


int AdminReapThread::init()
{
    TLOGDEBUG("begin AdminReapThread init"<<endl);

    //初始化配置db连接
    _db.init(g_pconf);

    //服务心跳更新时间间隔
    _updateInterval = TC_Common::strto<int>((*g_pconf).get("/tars/reap<updateHeartInterval>", "10"));
    //最小值保护
    _updateInterval = _updateInterval < 5 ? 5 : _updateInterval;

    //管理主控心跳超时时间
    _timeout = TC_Common::strto<int>((*g_pconf)["/tars/reap<reigistryTimeout>"]);
    _timeout       = _timeout < 5 ? 5 : _timeout;

    //是否关闭更新管理主控心跳时间,一般需要迁移时，设置此项为Y
    _heartBeatOff = (*g_pconf).get("/tars/reap<heartbeatoff>", "N") == "Y"?true:false;

    _db.updateRegistryInfo2Db(_heartBeatOff);
    _db.loadIPPhysicalGroupInfo();

    TLOGDEBUG("AdminReapThread init ok."<<endl);

    return 0;
}

void AdminReapThread::terminate()
{
    TLOGDEBUG("[ReapThread terminate.]" << endl);
    _terminate = true;
}

void AdminReapThread::run()
{
    //更新服务心跳时间
    time_t tLastUpdateTime = TC_TimeProvider::getInstance()->getNow();
    time_t tLastQueryServer = 0;
    time_t tNow;
    while(!_terminate)
    {
        try
        {
            tNow = TNOW;
            //更新心跳
            if(tNow - tLastUpdateTime >= _updateInterval)
            {
                tLastUpdateTime = tNow;
                _db.updateRegistryInfo2Db(_heartBeatOff);
                _db.loadIPPhysicalGroupInfo();
            }

            //轮询心跳超时的主控
            if(tNow - tLastQueryServer >= _timeout)
            {
                tLastQueryServer = tNow;
                _db.checkRegistryTimeout(_timeout);
            }

            TC_ThreadLock::Lock lock(*this);
            timedWait(100); //ms
        }
        catch(exception & ex)
        {
            TLOGERROR("AdminReapThread exception:" << ex.what() << endl);
        }
        catch(...)
        {
            TLOGERROR("AdminReapThread unknown exception:" << endl);
        }
    }
}



