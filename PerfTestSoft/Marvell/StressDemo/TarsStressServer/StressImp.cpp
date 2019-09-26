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

#include "StressImp.h"
#include "servant/Application.h"
#include "TarsStressServer.h"

using namespace std;

//////////////////////////////////////////////////////
void StressImp::initialize()
{
    //initialize servant here:
    //...
}

//////////////////////////////////////////////////////
void StressImp::destroy()
{
    //destroy servant here:
    //...
}

int StressImp::test(tars::TarsCurrentPtr current) 
{ 

    return 0;
}


tars::Int32 StressImp::testStr(const std::string& in, std::string &out, tars::TarsCurrentPtr current)
{
    out = in;

    ++_num;

    if(_num == 100000)
    {
        TLOGDEBUG("pthread id:"<<gettid()<<"|time(ms):"<<TC_TimeProvider::getInstance()->getNowMs()-_time<<endl);
        _time=TC_TimeProvider::getInstance()->getNowMs();
        _num=0;
    }

    return 0;
}
