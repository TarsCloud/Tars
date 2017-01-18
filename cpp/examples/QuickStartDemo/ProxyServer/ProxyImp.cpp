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

#include "ProxyImp.h"
#include "ProxyServer.h"

using namespace std;
using namespace tars;

class HelloCallback : public HelloPrxCallback
{

public:
    HelloCallback(TarsCurrentPtr &current)
    : _current(current)
    {}

    virtual void callback_testHello(tars::Int32 ret,  const std::string& sOut)
    {
        Proxy::async_response_testProxy(_current, ret, sOut);
    }
    virtual void callback_testHello_exception(tars::Int32 ret)
    { 
        TLOGERROR("HelloCallback callback_testHello_exception ret:" << ret << endl); 

        Proxy::async_response_testProxy(_current, ret, "");
    }

    TarsCurrentPtr _current;
};



//////////////////////////////////////////////////////
void ProxyImp::initialize()
{
    //initialize servant here:
    //...

    _prx = Application::getCommunicator()->stringToProxy<HelloPrx>("TestApp.HelloServer.HelloObj");
}

//////////////////////////////////////////////////////
void ProxyImp::destroy()
{
}
  
//////////////////////////////////////////////////////
tars::Int32 ProxyImp::test(tars::TarsCurrentPtr current) { return 0;}

//////////////////////////////////////////////////////
tars::Int32 ProxyImp::testProxy(const std::string& sIn, std::string &sOut, tars::TarsCurrentPtr current)
{
    try
    {
        current->setResponse(false);

        TestApp::HelloPrxCallbackPtr cb = new HelloCallback(current);

        _prx->tars_set_timeout(3000)->async_testHello(cb,sIn);
    }
    catch(std::exception &ex)
    {
        current->setResponse(true);

        TLOGERROR("ProxyImp::testProxy ex:" << ex.what() << endl);
    }

    return 0;
}
