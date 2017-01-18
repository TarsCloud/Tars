/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
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
#include "AdminReg.h"
#include "servant/Communicator.h"
#include <iostream>

using namespace std;
using namespace tars;

class Test1
{
public:
    Test1(const string &sStr);
    ~Test1();
    void  th_dohandle(int excut_num);
private:
    Communicator _comm;
    AdminRegPrx  _prx;
};

Test1::Test1(const string &sStr)
{
    _comm.stringToProxy("tars.tarsAdminRegistry.AdminRegObj@tcp -h 10.208.139.242 -p 12000 -t 60000", _prx);
}

Test1::~Test1()
{
    
}

void Test1::th_dohandle(int excut_num)
{
    for(int i=0; i<excut_num; i++) 
    {
        try
        {
            string application("tars");
            string serverName("tarsconfig");
            string nodeName("10.208.139.242");

            ServerStateDesc state;
            string result;

            int iRet = _prx->tars_set_timeout(15000)->getServerState(application, serverName, nodeName, state, result);
            if(iRet == 0)
            {
                cout << "succ." << state.presentStateInReg << endl;
            }
            else
            {
                cout << "fail." << endl;
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
}

int main(int argc,char ** argv)
{
    if(argc != 4)
    {
        cout << "usage: " << argv[0] << " ThreadNum CallTimes sObj" << endl;
        return -1;
    }

    string s = string(argv[3]);

    Test1 test1(s);
    try
    {

        tars::Int32 times = TC_Common::strto<tars::Int32>(string(argv[3]));

        test1.th_dohandle(times);

    }
    catch(exception &e)
    {
        cout<<e.what()<<endl;
    }
    catch(...)
    {
        
    }

    cout << "end" << endl;
    
    return 0;
}
