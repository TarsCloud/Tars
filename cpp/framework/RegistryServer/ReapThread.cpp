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

#include "ReapThread.h"

extern TC_Config * g_pconf;

ReapThread::ReapThread()
: _terminate(false)
, _loadObjectsInterval1(10)
, _leastChangedTime1(60)
, _loadObjectsInterval2(3600)
, _leastChangedTime2(30*60)
, _registryTimeout(150)
, _recoverProtect(true)
, _recoverProtectRate(30)
, _heartBeatOff(false)
{
}

ReapThread::~ReapThread()
{
    if (isAlive())
    {
        terminate();
        notifyAll();
        getThreadControl().join();
    }
}

int ReapThread::init()
{
    TLOGDEBUG("begin ReapThread init"<<endl);

    //初始化配置db连接
    _db.init(g_pconf);

    //加载对象列表的时间间隔
    _loadObjectsInterval1 = TC_Common::strto<int>((*g_pconf).get("/tars/reap<loadObjectsInterval1>", "10"));
    //第一阶段加载最近时间更新的记录,默认是60秒
    _leastChangedTime1    = TC_Common::strto<int>((*g_pconf).get("/tars/reap<LeastChangedTime1>", "150"));

    _loadObjectsInterval2 = TC_Common::strto<int>((*g_pconf).get("/tars/reap<loadObjectsInterval2>", "300"));

    //主控心跳超时时间
    _registryTimeout      = TC_Common::strto<int>((*g_pconf)["/tars/reap<reigistryTimeout>"]);

    //是否启用DB恢复保护功能
    _recoverProtect       = (*g_pconf).get("/tars/reap<recoverProtect>", "Y") == "N"?false:true;

    //启用DB恢复保护功能状态下极限值
    _recoverProtectRate   = TC_Common::strto<int>((*g_pconf).get("/tars/reap<recoverProtectRate>", "30"));

    //是否关闭更新主控心跳时间,一般需要迁移主控服务是，设置此项为Y
    _heartBeatOff = (*g_pconf).get("/tars/reap<heartbeatoff>", "N") == "Y"?true:false;

    //最小值保护
    _loadObjectsInterval1  = _loadObjectsInterval1 < 5 ? 5 : _loadObjectsInterval1;

    _registryTimeout       = _registryTimeout      < 5 ? 5 : _registryTimeout;

    _recoverProtectRate    = _recoverProtectRate   < 1 ? 30: _recoverProtectRate;

    _recoverProtect        = false;

    //加载对象列表
    _db.loadObjectIdCache(_recoverProtect, _recoverProtectRate,0,true, true);

    TLOGDEBUG("ReapThread init ok."<<endl);

    return 0;
}

void ReapThread::terminate()
{
    TLOGDEBUG("[ReapThread terminate.]" << endl);
    _terminate = true;
}

void ReapThread::run()
{
    //增量加载服务分两个阶段
    //第一阶段加载时间
    time_t tLastLoadObjectsStep1 = TC_TimeProvider::getInstance()->getNow();

    //全量加载时间
    time_t tLastLoadObjectsStep2 = TC_TimeProvider::getInstance()->getNow();

    time_t tLastQueryServer      = 0;
    time_t tNow;
    while(!_terminate)
    {
        try
        {
            tNow = TC_TimeProvider::getInstance()->getNow();

            //加载对象列表
            if(tNow - tLastLoadObjectsStep1 >= _loadObjectsInterval1)
            {
                tLastLoadObjectsStep1 = tNow;

                _db.updateRegistryInfo2Db(_heartBeatOff);

                if(tNow - tLastLoadObjectsStep2 >= _loadObjectsInterval2)
                {
                    tLastLoadObjectsStep2 = tNow;
                    //全量加载,_leastChangedTime2参数没有意义
                    _db.loadObjectIdCache(_recoverProtect, _recoverProtectRate,_leastChangedTime2,true, false);
                }
                else
                {
                    _db.loadObjectIdCache(_recoverProtect, _recoverProtectRate,_leastChangedTime1,false, false);
                }

            }

            //轮询心跳超时的主控
            if(tNow - tLastQueryServer >= _registryTimeout)
            {
                tLastQueryServer = tNow;
                _db.checkRegistryTimeout(_registryTimeout);
            }


            TC_ThreadLock::Lock lock(*this);
            timedWait(100); //ms
        }
        catch(exception & ex)
        {
            TLOGERROR("ReapThread exception:" << ex.what() << endl);
        }
        catch(...)
        {
            TLOGERROR("ReapThread unknown exception:" << endl);
        }
    }
}



