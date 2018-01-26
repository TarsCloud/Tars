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

#include "CheckSettingState.h"
#include "servant/TarsLogger.h"

extern TC_Config * g_pconf;

CheckSettingState::CheckSettingState()
: _terminate(false)
, _checkingInterval(10)
, _leastChangedTime(10*60)
{
}

CheckSettingState::~CheckSettingState()
{
    if (isAlive())
    {
        terminate();
        notifyAll();
        getThreadControl().join();
    }
}

int CheckSettingState::init()
{
    TLOGDEBUG("CheckSettingStateThread init"<<endl);

    //初始化配置db连接
    _db.init(g_pconf);

    //轮询server状态的间隔时间
    _checkingInterval = TC_Common::strto<int>((*g_pconf).get("/tars/reap<queryInterval>", "10"));
    _checkingInterval = _checkingInterval       < 5 ? 5 : _checkingInterval;

    _leastChangedTime = TC_Common::strto<int>((*g_pconf).get("/tars/reap<querylesttime>", "600"));
    _leastChangedTime = _leastChangedTime < 60 ? 60 : _leastChangedTime;

    TLOGDEBUG("CheckSettingStateThread init ok."<<endl);

    return 0;
}

void CheckSettingState::terminate()
{
    TLOGDEBUG("[CheckSettingStateThread terminate.]" << endl);
    _terminate = true;
}

void CheckSettingState::run()
{
    time_t tLastQueryServer = 0;
    time_t tNow;

    while(!_terminate)
    {
        try
        {
            tNow = TC_TimeProvider::getInstance()->getNow();

            //核查各server在node的设置状态
            if(tNow - tLastQueryServer >= _checkingInterval)
            {
                _db.checkSettingState(_leastChangedTime);

                //执行完成后再赋当前时间值
                tLastQueryServer = TC_TimeProvider::getInstance()->getNow();
            }

            TC_ThreadLock::Lock lock(*this);
            timedWait(100); //ms
        }
        catch(exception & ex)
        {
            TLOGERROR("CheckSettingState exception:" << ex.what() << endl);
        }
        catch(...)
        {
            TLOGERROR("CheckSettingState unknown exception." << endl);
        }
    }
}

