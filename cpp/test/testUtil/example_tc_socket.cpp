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

#include "util/tc_socket.h"
#include "util/tc_clientsocket.h"
#include "util/tc_http.h"
#include "util/tc_epoller.h"
#include "util/tc_common.h"
#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>

using namespace tars;

string now2str(const string &sFormat = "%Y%m%d%H%M%S")
{
    time_t t = time(NULL);
    struct tm *pTm = localtime(&t);
    if(pTm == NULL)
    {
        return "";
    }

    char sTimeString[255] = "\0";

    strftime(sTimeString, sizeof(sTimeString), sFormat.c_str(), pTm);

    return string(sTimeString);
}

void testTC_Socket()
{
    TC_Socket tcSocket;
    tcSocket.createSocket();
    tcSocket.bind("192.168.128.66", 8765);
    tcSocket.listen(5);
}

void testTC_TCPClient()
{
    TC_TCPClient tc;
    tc.init("172.16.28.79", 8382, 3);

    cout << now2str() << endl;
    int i = 10000;
    while(i>0)
    {
        string s = "test";
        char c[1024] = "\0";
        size_t length = 4;
        int iRet = tc.sendRecv(s.c_str(), s.length(), c, length);
        if(iRet < 0)
        {
            cout << "send recv error:" << iRet << ":" << c << endl;
        }
        i--;
 //       cout << c << endl;
        assert(c == s);
    }
    cout << now2str() << endl;

}

void testShortSock()
{
    int i = 1000;
    while(i>0)
    {
        TC_TCPClient tc;
        tc.init("127.0.0.1", 8382, 10);

        string s = "test";
        char c[1024] = "\0";
        size_t length = 4;
        int iRet = tc.sendRecv(s.c_str(), s.length(), c, length);
        if(iRet < 0)
        {
            cout << "send recv error" << endl;
        }
        if(i % 1000 == 0)
        {
            cout << i << endl;
        }
        usleep(10);
        i--;
        assert(c == s);
    }
}

void testUdp()
{
    fork();fork();fork();fork();fork();
    int i = 1000;
    string s;
    for(int j = 0; j < 7192; j++)
    {
        s += "0";
    }
    s += "\n";

    while(i>0)
    {
        TC_UDPClient tc;
        tc.init("127.0.0.1", 8082, 3000);

        char c[10240] = "\0";
        size_t length = sizeof(c);

        int iRet = tc.sendRecv(s.c_str(), s.length(), c, length);
        if(iRet < 0)
        {
            cout << "send recv error:" << iRet << endl;
        }
        if(i % 1000 == 0)
        {
            cout << i << endl;
        }

        i--;
        if(c != s)
        {
            cout << c << endl;
//            break;
        }
    }
}
void testTimeoutSock()
{
    int i = 10;
    while(i>0)
    {
        TC_TCPClient tc;
        tc.init("127.0.0.1", 8382, 3);

        string s = "test";
        char c[1024] = "\0";
        size_t length = 4;
        int iRet = tc.sendRecv(s.c_str(), s.length(), c, length);
        if(iRet < 0)
        {
            cout << "send recv error" << endl;
        }
        if(i % 1000 == 0)
        {
            cout << i << endl;
        }
        i--;
        sleep(3);

        assert(c == s);
    }
}

void testLocalHost()
{
    vector<string> v = TC_Socket::getLocalHosts();
    cout << TC_Common::tostr(v.begin(), v.end()) << endl;
}

int main(int argc, char *argv[])
{
    try
    {
        testUdp();
        return 0;

        TC_Socket t;
        t.createSocket();
        t.bind("127.0.0.1", 0);
        string d;
        uint16_t p;
        t.getSockName(d, p);
        t.close();

        cout << d << ":" << p << endl;
        return 0;

        testLocalHost();

        string st;
        TC_Socket s;
        s.createSocket(SOCK_STREAM, AF_LOCAL);
        if(argc > 1)
        {
            s.bind("/tmp/tmp.udp.sock");
            s.listen(5);

            s.getSockName(st);
            cout << st << endl;

            struct sockaddr_un stSockAddr;
            socklen_t iSockAddrSize = sizeof(sockaddr_un);
            TC_Socket c;
            s.accept(c, (struct sockaddr *) &stSockAddr, iSockAddrSize);
        }
        else
        {
            s.connect("/tmp/tmp.udp.sock");
            s.getPeerName(st);
            cout << st << endl;
        }

    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


