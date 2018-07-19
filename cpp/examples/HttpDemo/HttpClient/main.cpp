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

#include <iostream>
#include "util/tc_http.h"
#include "util/tc_common.h"
#include "util/tc_clientsocket.h"
#include "util/tc_thread_pool.h"
#include "tup/Tars.h"
#include "tup/tup.h"
#include "util/tc_timeprovider.h"
using namespace std;
using namespace tars;
using namespace tup;

int doRequest(TC_HttpRequest& stHttp,TC_TCPClient&tcpClient, TC_HttpResponse &stHttpRsp, int iTimeout)
{
    string sSendBuffer = stHttp.encode();

    int iRet = tcpClient.send(sSendBuffer.c_str(), sSendBuffer.length());
    if(iRet != TC_ClientSocket::EM_SUCCESS)
    {
        return iRet;
    }

    stHttpRsp.reset();

    string sBuffer;

    char *sTmpBuffer = new char[10240];
    size_t iRecvLen  = 10240;

    while(true)
    {
        iRecvLen = 10240;

        iRet = tcpClient.recv(sTmpBuffer, iRecvLen);

        if(iRet == TC_ClientSocket::EM_SUCCESS)
        sBuffer.append(sTmpBuffer, iRecvLen);

        switch(iRet)
        {
        case TC_ClientSocket::EM_SUCCESS:
            if(stHttpRsp.incrementDecode(sBuffer))
            {
                delete []sTmpBuffer;
                return TC_ClientSocket::EM_SUCCESS;
            }
            continue;
        case TC_ClientSocket::EM_CLOSE:
            delete []sTmpBuffer;
            stHttpRsp.incrementDecode(sBuffer);
            return TC_ClientSocket::EM_SUCCESS;
        default:
            delete []sTmpBuffer;
            return iRet;
        }
    }

    assert(true);

    return 0;
}

void th_dohandle(int excut_num, int iSplit)
{
    tars::Int32 count = 0;
    unsigned long sum = 0;
    unsigned long id = 1;
    int64_t _iTime = TC_TimeProvider::getInstance()->getNowMs();

    TC_HttpRequest stHttpReq;
    stHttpReq.setCacheControl("no-cache");

    string sServer1("http://10.120.129.226:10024/");

    TC_TCPClient tcpClient1;
    tcpClient1.init("10.120.129.226", 10024, 3000);

    int iRet = 0;

    for (int i = 0; i<excut_num; i++)
    {
        try
        {
            TC_HttpResponse stHttpRsp;

            stHttpReq.setGetRequest(sServer1);

            iRet = doRequest(stHttpReq, tcpClient1, stHttpRsp, 3000);  //³¤Á´½Ó
            
            if (iRet == 0)
            {
                ++sum;
                ++count;

                if (excut_num == count)
                {
                    cout << "pthread id: " << pthread_self() << " | " << TC_TimeProvider::getInstance()->getNowMs() - _iTime <<"(ms)"<< endl;
                    _iTime=TC_TimeProvider::getInstance()->getNowMs();
                    count = 0;
                }
            }
            else
            {
                 cout <<"pthread id: " << pthread_self()<< "|iRet:"<<iRet<<endl;
            }
        }
        catch(TC_Exception &e)
        {
               cout << "pthread id: " << pthread_self() << " id: " << i << " exception: " << e.what() << endl;
        }
        catch(...)
        {
             cout << "pthread id: " << pthread_self() << " id: " << i << " unknown exception." << endl;
        }
        id += iSplit;
    }
    cout << "succ:" << sum <<endl;
}

int main(int argc,char ** argv)
{
    if(argc != 3 && argc != 4)
    {
        cout << "usage: " << argv[0] << " ThreadNum CallTimes Split" << endl;
        return -1;
    }

    try
    {
        tars::Int32 threads = TC_Common::strto<tars::Int32>(string(argv[1]));
        TC_ThreadPool tp;
        tp.init(threads);
        tp.start(); 
        cout << "init tp succ" << endl;
        tars::Int32 times = TC_Common::strto<tars::Int32>(string(argv[2]));
 
        int iSplit = 1;
        if(argc == 4)
        {
             iSplit = TC_Common::strto<tars::Int32>(string(argv[3]));
        }
        auto fwrapper3 = std::bind(&th_dohandle, times, iSplit);
        for(int i = 0; i<threads; i++)
        {
            tp.exec(fwrapper3);
            cout << "********************" <<endl;
        }
        tp.wait();
    }catch(exception &e)
    {
        cout<<e.what()<<endl;
    }
    catch(...)
    {

    }

    return 0;
}
