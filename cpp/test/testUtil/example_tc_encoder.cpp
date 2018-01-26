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

#include "util/tc_encoder.h"
#include "util/tc_file.h"
#include "util/tc_http.h"
#include <errno.h>
#include <iostream>

using namespace tars;

bool isUTF8(const string &buffer)
{
    if(buffer.length() <= strlen("charset=utf"))
    {
        return false;
    }

    size_t len = strlen("charset");

    for(size_t i = 0; i < buffer.length() - len; i++)
    {
        if(strncasecmp(buffer.c_str() + i, "charset", len) != 0)
            continue;
        
        if(i > 0 && buffer[i-1] != ' ' 
                && buffer[i-1] != '\t' 
                && buffer[i-1] != ';'
                && buffer[i-1] != '\"'
                && buffer[i-1] != '\'')
            continue;

        int flag = 0;
        for(size_t j = i + len; j < buffer.length(); j++)
        {
            if(flag == 0)
            {
                if(buffer[j] == ' ' || buffer[j] == '\t')
                    continue;

                if(buffer[j] == '=')
                {
                    flag = 1;
                    continue;
                }
                
                break;
            }
            else if(flag == 1)
            {
                if(buffer[j] == ' ' || buffer[j] == '\t')
                    continue;

                string tmp = TC_Common::trimleft(buffer.c_str() + j, "\" \t'");

                if(strncmp(tmp.c_str(), "utf", 3) == 0)
                    return true;

                return false;
            }
            else
            {
                break;
            }
        }
    }

    return false;
}


int main(int argc, char *argv[])
{
    try
    {
        /*
        string s = "我们";
//        string v = gbk2utf8(s);
//        cout << s << ":" << v  << TC_Encoder::utf82gbk(v) << endl;

        string n = TC_File::load2str("./content.html");
        cout << n << endl;
*/
        string s = "<meta content=\"text/html\" charset=\"utf-8\" http-equiv=\"content-type\"/>";
        cout << isUTF8(s) << endl;

/*
        TC_HttpRequest stHttpReq;
        stHttpReq.setCacheControl("no-cache");
        stHttpReq.setUserAgent("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0; InfoPath.1; .NET CLR 1.1.4322)");
        stHttpReq.setGetRequest("http://cn.engadget.com/");
        string sSendBuffer = stHttpReq.encode();

        TC_HttpResponse stHttpRsp;
        int iRet = stHttpReq.doRequest(stHttpRsp, 60000);
        if(iRet != 0)
        {
            return 0;
        }
        string sRspHeader = stHttpRsp.encode();
        sRspHeader = sRspHeader.substr(0, sRspHeader.find("\r\n\r\n"));
        cout << "[preview] http response:" << sRspHeader << endl;

        string content = stHttpRsp.getContent();

        string contentType = TC_Common::lower(stHttpRsp.getContentType());

        cout << "[preview] contentType:" << contentType << endl;
        
        string tmp = TC_Common::lower(content).substr(0, 512);
        cout << "[preview] tmp:" << tmp << endl;

        if(contentType.find("utf8") == string::npos 
            && contentType.find("utf-8") == string::npos
            && !isUTF8(tmp))
        {
            cout << "[preview] gbk to utf8" << endl;
//            content = gbk2utf8(content);
        }

        cout << isUTF8(tmp) << endl;
*/
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


