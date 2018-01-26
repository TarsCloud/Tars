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

#ifndef _AServantImp_H_
#define _AServantImp_H_

#include "servant/Application.h"
#include "AServant.h"

/**
 *
 *
 */
class AServantImp : public Test::AServant
{
public:
	/**
	 *
	 */
	virtual ~AServantImp() {}

	/**
	 *
	 */
	virtual void initialize();

	/**
	 *
	 */
    virtual void destroy();

	/**
	 *
	 */
	virtual int test(tars::TarsCurrentPtr current); 

	tars::Int32 testInt(tars::Int32 iIn,tars::Int32 &iOut,tars::TarsCurrentPtr current);

	tars::Int32 testStr(const std::string& sIn, std::string &sOut, tars::TarsCurrentPtr current);

private:

};
/////////////////////////////////////////////////////
#endif
