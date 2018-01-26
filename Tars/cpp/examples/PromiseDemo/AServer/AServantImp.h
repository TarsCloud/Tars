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

#ifndef _BServantImp_H_
#define _BServantImp_H_

#include "servant/Application.h"
#include "AServant.h"
#include "BServant.h"
#include "CServant.h"
#include "promise/promise.h"
//#include "promise/tuple.h"

#include "promise/when_all.h"

using namespace Test;

///////////////////////////////////
promise::Future<std::string> sendBReq(BServantPrx prx, const std::string& sIn, tars::TarsCurrentPtr current);

promise::Future<std::string> handleBRspAndSendCReq(CServantPrx prx, TarsCurrentPtr current, const promise::Future<std::string>& future);

promise::Future<std::string> sendCReq(CServantPrx prx, const std::string& sIn, tars::TarsCurrentPtr current);

int handleCRspAndReturnClient(TarsCurrentPtr current, const promise::Future<std::string>& future);

///////////////////////////////////
int handleBCRspAndReturnClient(TarsCurrentPtr current, const promise::Future<promise::Tuple<promise::Future<std::string>, promise::Future<std::string> > >& allFuture);

///////////////////////////////////
class AServantImp : public Test::AServant
{
public:

    virtual ~AServantImp() {}

    virtual void initialize();

    virtual void destroy();

    tars::Int32 queryResultSerial(const std::string& sIn, std::string &sOut, tars::TarsCurrentPtr current);

    tars::Int32 queryResultParallel(const std::string& sIn, std::string &sOut, tars::TarsCurrentPtr current);

private:
    BServantPrx _pPrxB;
    CServantPrx _pPrxC;
};
/////////////////////////////////////////////////////
#endif
