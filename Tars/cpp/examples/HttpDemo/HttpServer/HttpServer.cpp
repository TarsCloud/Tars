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

#include "HttpServer.h"
#include "HttpImp.h"

using namespace std;

HttpServer g_app;

/////////////////////////////////////////////////////////////////
struct HttpProtocol
{
    /**
     * 解析http请求
     * @param in
     * @param out
     *
     * @return int
     */
    static int parseHttp(string &in, string &out)
    {
        try
        {
                        //判断请求是否是HTTP请求
            bool b = TC_HttpRequest ::checkRequest(in.c_str(), in.length());
                        //完整的HTTP请求
            if(b)
            {
                out = in;
                in  = "";
                //TLOGDEBUG("out size: " << out.size() << endl);
                return TC_EpollServer::PACKET_FULL;
            }
            else
            {
                return TC_EpollServer::PACKET_LESS;
            }
        }
        catch(exception &ex)
        {
            return TC_EpollServer::PACKET_ERR;
        }

        return TC_EpollServer::PACKET_LESS;             //表示收到的包不完全
    }

};

void
HttpServer::initialize()
{
    //initialize application here:
    //...

    addServant<HttpImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".HttpObj");
    addServantProtocol(ServerConfig::Application + "." + ServerConfig::ServerName + ".HttpObj",&HttpProtocol::parseHttp);
}
/////////////////////////////////////////////////////////////////
void
HttpServer::destroyApp()
{
    //destroy application here:
    //...
}
/////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
    try
    {
        g_app.main(argc, argv);
        g_app.waitForShutdown();
    }
    catch (std::exception& e)
    {
        cerr << "std::exception:" << e.what() << std::endl;
    }
    catch (...)
    {
        cerr << "unknown exception." << std::endl;
    }
    return -1;
}
/////////////////////////////////////////////////////////////////
