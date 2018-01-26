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

#include "AServantImp.h"
#include "AServer.h"

using namespace std;

//////////////////////////////////////////////////////
void AServantImp::initialize()
{
	//initialize servant here:
	//...
}

//////////////////////////////////////////////////////
void AServantImp::destroy()
{
	//destroy servant here:
	//...
}


int AServantImp::test(tars::TarsCurrentPtr current) 
{
	return 0;
}

tars::Int32 AServantImp::testInt(tars::Int32 iIn,tars::Int32 &iOut,tars::TarsCurrentPtr current)
{

    iOut = iIn;
    
    return 0;
}

tars::Int32 AServantImp::testStr(const std::string& sIn, std::string &sOut, tars::TarsCurrentPtr current)
{

	sOut = sIn;

	return 0;
}
