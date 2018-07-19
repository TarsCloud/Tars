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

#include "AServant.h"
#include "servant/Communicator.h"
#include "util/tc_thread_pool.h"
#include <iostream>

using namespace std;
using namespace Test;
using namespace tars;

class Test1
{
public:
    Test1(const string &sStr);

    ~Test1();

    void  queryResult(int iFlag, int iExecuteNum);

private:
    Communicator    _comm;
    AServantPrx        prx;
};

Test1::Test1(const string &sStr)
{
    _comm.setProperty("locator", "tars.tarsregistry.QueryObj@    tcp -h 10.208.139.242 -p 17890 -t 10000 ");
    _comm.stringToProxy(sStr, prx);
}

Test1::~Test1()
{
    
}

void Test1::queryResult(int iFlag, int iExecuteNum)
{
    string sIn(10,'a');
    string sOut("");

    tars::Int32 count = 0;
    unsigned long sum = 0;

    time_t _iTime=TC_TimeProvider::getInstance()->getNowMs();

    for(int i=0; i<iExecuteNum; i++) 
    {
        sOut = "";
        try
        {
            int ret = -1;
            if(iFlag == 0)
            {
                ret = prx->queryResultSerial(sIn, sOut);
            }
            else
            {
                ret = prx->queryResultParallel(sIn, sOut);
            }

            if(ret == 0)
            {
                ++sum;
                ++count;
                if(count == iExecuteNum)
                {
                    cout << "pthread id: " << pthread_self() << " | " << TC_TimeProvider::getInstance()->getNowMs() - _iTime << endl;
                    _iTime=TC_TimeProvider::getInstance()->getNowMs();
                    count = 0;
                }
            }
        }
        catch(TC_Exception &e)
        {
            cout << "pthread id: " << pthread_self() << "id: " << i << "exception: " << e.what() << endl;
        }
        catch(...)
        {
            cout << "pthread id: " << pthread_self() << "id: " << i << "unknown exception." << endl;
        }
    }
    cout << "succ:" << sum << endl;
    cout << "sOut:" << sOut << endl;
}

int main(int argc,char ** argv)
{
    if(argc != 5)
    {
        cout << "usage: " << argv[0] << " sObj ThreadNum CallTimes  CallMode" << endl;
        return -1;
    }

    string s = string(argv[1]);

    Test1 test1(s);
    try
    {
        tars::Int32 threads = TC_Common::strto<tars::Int32>(string(argv[2]));
        TC_ThreadPool tp;
        tp.init(threads);
        tp.start();
        tars::Int32 times = TC_Common::strto<tars::Int32>(string(argv[3]));
        tars::Int32 callMode = TC_Common::strto<tars::Int32>(string(argv[4]));
        
        for(int i = 0; i<threads; i++) 
        {
            auto fw = std::bind(&Test1::queryResult, &test1, callMode, times);
            tp.exec(fw);
            cout << "********************" <<endl;
        }
        tp.waitForAllDone(); 
    }catch(exception &e)
    {
        cout<<e.what()<<endl;
    }
    catch(...)
    {
        
    }
    
    return 0;
}
