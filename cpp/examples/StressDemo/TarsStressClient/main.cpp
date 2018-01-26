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

#include "Stress.h"
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
    Test1(){}
    ~Test1();
    void dohandle(int excut_num,int size);
private:
    Communicator _comm;
    StressPrx prx;
};

Test1::Test1(const string &sStr)
{
//    _comm.setProperty("locator","tars.tarsregistry.QueryObj@tcp -h 172.27.194.147 -p 17890 -t 50000");
    _comm.setProperty("locator","tars.tarsregistry.QueryObj@tcp -h 10.120.129.226 -p 17890 -t 10000");
    _comm.setProperty("stat", "tars.tarsstat.StatObj");
    _comm.stringToProxy(sStr, prx);
}

Test1::~Test1()
{
    
}

void Test1::dohandle(int excut_num,int size)
{
    string s(size,'a');

    tars::Int32 count = 0;
    unsigned long sum = 0;

    time_t _iTime=TC_TimeProvider::getInstance()->getNowMs();

    for(int i=0; i<excut_num; i++) 
    {
        try
        {

            string ret="";
            prx->tars_set_timeout(15000)->testStr(s,ret);
            if(ret.size() == s.size())
            {
                ++sum;
                ++count;
            }

            if(count == excut_num)
            {
                cout << "pthread id: " << pthread_self() << " | " << TC_TimeProvider::getInstance()->getNowMs() - _iTime << endl;
                count = 0;
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
    cout << "succ:" << sum <<endl;
}

int main(int argc,char ** argv)
{
    if(argc != 5)
    {
        cout << "usage: " << argv[0] << " ThreadNum CallTimes sObj size" << endl;
        return -1;
    }

    // string s="Test.TarsStressServer.StressObj";
    string s=string(argv[3]);
    Test1 tm(s);
       try
    {
        tars::Int32  threads = TC_Common::strto<tars::Int32>(string(argv[1]));
        TC_ThreadPool tp;
        tp.init(threads);
        tp.start();    

        cout << "init tp succ" << endl;

        typedef void (Test1::*TpMem)(int,int);
        TC_Functor<void, TL::TLMaker<int,int>::Result> cmd(&tm,static_cast<TpMem>( &Test1::dohandle));
        tars::Int32 times = TC_Common::strto<tars::Int32>(string(argv[2]));
	tars::Int32 size  = TC_Common::strto<tars::Int32>(string(argv[4]));
        cout << "times:" << times << endl;

        for(int i = 0; i<threads; i++) 
        {
            TC_Functor<void, TL::TLMaker< int,int>::Result>::wrapper_type fw(cmd,times,size);
            tp.exec(fw);
            cout << "********************" <<endl;
        }

        tp.waitForAllDone(); 
    }
    catch(exception &e)
    {
        cout<<e.what()<<endl;
    }
    catch(...)
    {
        
    }
    
    return 0;
}
