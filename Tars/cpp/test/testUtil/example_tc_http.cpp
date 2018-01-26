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
#include "util/tc_file.h"
#include "util/tc_epoller.h"
#include "util/tc_common.h"
#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <zlib.h>

using namespace tars;


void testTC_HTTPClient()
{
    TC_HttpRequest stHttpReq;
    stHttpReq.setCacheControl("no-cache");
    stHttpReq.setUserAgent("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0; InfoPath.1; .NET CLR 1.1.4322)");
//    stHttpReq.setGetRequest("http://www.baidu.com");
//    stHttpReq.setGetRequest("http://www.google.com");
    stHttpReq.setGetRequest("http://s39.cnzz.com/stat.php?id=533357&web_id=533357&show=pic2");
//    stHttpReq.setGetRequest("http://172.23.154.13:8080/");
//    stHttpReq.setGetRequest("http://172.25.38.67:8080");
//    stHttpReq.setGetRequest("http://172.27.31.50:8090/mqqservice/jiaxin.jsp?qq=39067484&type=4");
    
    string sSendBuffer = stHttpReq.encode();
    cout << sSendBuffer << endl;
    cout << "***********************************" << endl;

    TC_HttpResponse stHttpRep;
    int iRet = stHttpReq.doRequest(stHttpRep, 3000);
    if(iRet != 0)
    {
        cout << iRet << endl;
    }

    cout << TC_Common::tostr(stHttpRep.getHeaders()) << endl;
    
    string r = stHttpRep.encode();
//    cout << r << endl;
    cout << "***********************************" << endl;
    TC_File::save2file("tmp.html", stHttpRep.getContent());
    cout << "***********************************" << endl;
    cout << stHttpRep.getContent().length() << endl;
}

void testHttpHeader1()
{
    string s = "HTTP/1.1 200 OK\r\n";
    s += "Server:nginx/0.6.39\r\n";
    s += "Accept-Ranges: bytes\r\n";
    s += "Accept-Ranges: bytes\r\n";

    TC_Http::http_header_type m;
    TC_Http::parseHeader(s.c_str(), m);
    cout << TC_Common::tostr(m) << endl;

    TC_Http t;
    t.setHeader("abc", "def");
    t.setHeader("abC", "def");

    m = t.getHeaders();

    cout << TC_Common::tostr(m) << endl;
}

void testHttp1()
{
    TC_HttpRequest stHttpReq;
//    stHttpReq.setGetRequest("http://www.baidu.com");
//    stHttpReq.setGetRequest("http://www.google.com");
//    stHttpReq.setGetRequest("https://s39.cnzz.com:8080/stat.php?id=533357&web_id=533357&show=pic2", true);
    string a;
    stHttpReq.setPostRequest("https://s39.cnzz.com:8080/stat.php?id=533357&web_id=533357&show=pic2", a, true );

    cout << stHttpReq.encode() << endl;

    cout << "----------------------------------------" << endl;

    cout << stHttpReq.getHost() << endl;
    cout << stHttpReq.getOriginRequest() << endl;
    cout << stHttpReq.getRequest() << endl;
    cout << stHttpReq.getRequestParam() << endl;
    cout << stHttpReq.getRequestUrl() << endl;

    cout << "----------------------------------------" << endl;

    string sSendBuffer = stHttpReq.encode();
    cout << sSendBuffer << endl;
    cout << "***********************************" << endl;
}

void testHttp2()
{
    TC_HttpRequest stHttpReq;

    stHttpReq.setGetRequest("http://s39.cnzz.com:8080/stat.php?id=533357&web_id=533357&show=pic2", true);

    string sSendBuffer = stHttpReq.encode();

    cout << sSendBuffer << endl;

    string sHost;
    uint32_t iPort;
    stHttpReq.getHostPort(sHost, iPort);

    cout << "-----" << sHost << ", " << iPort <<endl;

    cout << stHttpReq.getHost() << endl;
    cout << stHttpReq.getOriginRequest() << endl;
    cout << stHttpReq.getRequest() << endl;
    cout << stHttpReq.getRequestParam() << endl;
    cout << stHttpReq.getRequestUrl() << endl;

    cout << "***********************************" << endl;
}

void testHttpHeader2()
{
    string s = string("GET /stat.php?id=533357&web_id=533357&show=pic2 HTTP/1.1\r\n")
               +string("Host: s39.cnzz.com:8080\r\n")
               +string("User-Agent: TC_Http\r\n")
               +string("\r\n");

    TC_HttpRequest stHttpReq;
    stHttpReq.decode(s);

    string sDomain;
    uint32_t iPort;
    stHttpReq.getHostPort(sDomain, iPort);

    cout << "getHostPort:" << sDomain << ":" << iPort <<endl;

    cout << stHttpReq.getHost() << endl;
    assert(stHttpReq.getHost() == "s39.cnzz.com:8080");
    cout << stHttpReq.getOriginRequest() << endl;
    assert(stHttpReq.getOriginRequest() == "http://s39.cnzz.com:8080/stat.php?id=533357&web_id=533357&show=pic2");
    cout << stHttpReq.getRequest() << endl;
    assert(stHttpReq.getRequest() == "/stat.php?id=533357&web_id=533357&show=pic2");
    cout << stHttpReq.getRequestParam() << endl;
    assert(stHttpReq.getRequestParam() == "id=533357&web_id=533357&show=pic2");
    cout << stHttpReq.getRequestUrl() << endl;
    assert(stHttpReq.getRequestUrl() == "/stat.php");

    cout << "***********************************" << endl;
}
void testHttpHeader3()
{
    string s = string("GET /stat.php?id=533357&web_id=533357&show=pic2 HTTP/1.1\r\n")
               +string("Host: s39.cnzz.com\r\n")
               +string("User-Agent: TC_Http\r\n")
               +string("\r\n");

    TC_HttpRequest stHttpReq;
    stHttpReq.decode(s);

    string sDomain;
    uint32_t iPort;
    stHttpReq.getHostPort(sDomain, iPort);

    cout << "getHostPort:" << sDomain << ":" << iPort <<endl;

    cout << stHttpReq.getHost() << endl;
    assert(stHttpReq.getHost() == "s39.cnzz.com");
    cout << stHttpReq.getOriginRequest() << endl;
    assert(stHttpReq.getOriginRequest() == "http://s39.cnzz.com/stat.php?id=533357&web_id=533357&show=pic2");
    cout << stHttpReq.getRequest() << endl;
    assert(stHttpReq.getRequest() == "/stat.php?id=533357&web_id=533357&show=pic2");
    cout << stHttpReq.getRequestParam() << endl;
    assert(stHttpReq.getRequestParam() == "id=533357&web_id=533357&show=pic2");
    cout << stHttpReq.getRequestUrl() << endl;
    assert(stHttpReq.getRequestUrl() == "/stat.php");

    cout << "***********************************" << endl;
}
void testHttpHeader4()
{
    string s = string("GET http://s39.cnzz.com:80/stat.php?id=533357&web_id=533357&show=pic2 HTTP/1.1\r\n")
               +string("User-Agent: TC_Http\r\n")
               +string("\r\n");

    TC_HttpRequest stHttpReq;
    stHttpReq.decode(s);

    string sDomain;
    uint32_t iPort;
    stHttpReq.getHostPort(sDomain, iPort);

    cout << "getHostPort:" << sDomain << ":" << iPort <<endl;

    cout << stHttpReq.getHost() << endl;
    assert(stHttpReq.getHost() == "s39.cnzz.com");
    cout << stHttpReq.getOriginRequest() << endl;
    assert(stHttpReq.getOriginRequest() == "http://s39.cnzz.com/stat.php?id=533357&web_id=533357&show=pic2");
    cout << stHttpReq.getRequest() << endl;
    assert(stHttpReq.getRequest() == "/stat.php?id=533357&web_id=533357&show=pic2");
    cout << stHttpReq.getRequestParam() << endl;
    assert(stHttpReq.getRequestParam() == "id=533357&web_id=533357&show=pic2");
    cout << stHttpReq.getRequestUrl() << endl;
    assert(stHttpReq.getRequestUrl() == "/stat.php");

    cout << "***********************************" << endl;
}

void testHttpsHeader2()
{
    string s = string("GET https://s39.cnzz.com:8080/stat.php?id=533357&web_id=533357&show=pic2 HTTP/1.1\r\n")
               +string("Host: s39.cnzz.com:8080\r\n")
               +string("User-Agent: TC_Http\r\n")
               +string("\r\n");

    TC_HttpRequest stHttpReq;
    stHttpReq.decode(s);

    string sDomain;
    uint32_t iPort;
    stHttpReq.getHostPort(sDomain, iPort);

    cout << "getHostPort:" << sDomain << ":" << iPort <<endl;

    cout << stHttpReq.getHost() << endl;
    assert(stHttpReq.getHost() == "s39.cnzz.com:8080");
    cout << stHttpReq.getOriginRequest() << endl;
    assert(stHttpReq.getOriginRequest() == "https://s39.cnzz.com:8080/stat.php?id=533357&web_id=533357&show=pic2");
    cout << stHttpReq.getRequest() << endl;
    assert(stHttpReq.getRequest() == "/stat.php?id=533357&web_id=533357&show=pic2");
    cout << stHttpReq.getRequestParam() << endl;
    assert(stHttpReq.getRequestParam() == "id=533357&web_id=533357&show=pic2");
    cout << stHttpReq.getRequestUrl() << endl;
    assert(stHttpReq.getRequestUrl() == "/stat.php");

    cout << "***********************************" << endl;
}

void testEastMoney()
{
    string s = string("GET / HTTP/1.1\r\n")
+string("Accept: application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5\r\n")
+string("Accept-Charset: utf-8,gb2321;q=0.7,*;q=0.7\r\n")
+string("Accept-Encoding: gzip\r\n")
+string("Accept-Language: zh-cn,zh;q=0.5\r\n")
+string("Connection: close\r\n")
+string("Host: www.eastmoney.com\r\n")
+string("Q-GUID: 08f0373a192a45778cc8567d1c641475\r\n")
+string("Q-UA: SQB12_GA/120450&SMTT_3/020100&SYM3&201514&E71&V2\r\n")
+string("User-Agent: E71/SymbianOS/9.1 Series60/3.0\r\n\r\n");

    TC_HttpRequest stHttpReq;
    stHttpReq.decode(s);
    
    string sSendBuffer = stHttpReq.encode();
    cout << sSendBuffer << endl;
    cout << "***********************************" << endl;

    TC_HttpResponse stHttpRep;
    int iRet = stHttpReq.doRequest(stHttpRep, 10000);
    if(iRet != 0)
    {
        cout << iRet << endl;
    }

    cout << TC_Common::tostr(stHttpRep.getHeaders()) << endl;
    
//    string r = stHttpRep.encode();
//    cout << r << endl;
    cout << "***********************************" << endl;
    TC_File::save2file("tmp.html", stHttpRep.getContent());
    cout << "***********************************" << endl;
    cout << stHttpRep.getContent().length() << endl;
}

void testHttpResponse()
{
    string s = "HTTP/1.1  200  aaa \r\n";//  200  Aouut Error\r\n";
    s += "Server:nginx/0.6.39\r\n";
    s += "Accept-Ranges: bytes\r\n";
    s += "Accept-Ranges: bytes\r\n\r\n";

    TC_HttpResponse rsp;
    rsp.decode(s);

    cout << rsp.getAbout() << endl;
    cout << rsp.getStatus() << endl;
    cout << rsp.getVersion() << endl;
}

void testURL(TC_URL &url, const string &sURL, const string &sURL1)
{
    cout << sURL << endl;

    url.parseURL(sURL);

    cout << "[" << sURL << "] to [" << url.getURL() << "] rootpath:" << url.getRootPath() << " relativePath:" << url.getRelativePath() << endl;

    assert(url.getURL() == sURL1);
}

void testBuildURL(TC_URL &url, const string &sURL, const string &sURL1, const string &sURL2)
{
    url.parseURL(sURL);

    TC_URL u = url.buildWithRelativePath(sURL1);
    cout << url.getPath() << endl;

    cout << "[" << sURL << "] " << sURL1 << " to [ " << u.getURL() << "] " << endl;

//    assert(u.getURL() == sURL2);
}

string simplifyDirectory(const string& path)
{
    //所有./都去掉
    size_t pos      = 0;
    string sNewPath = path;

    while (true)
    {
        size_t dotPos = sNewPath.find("./", pos);

        if (dotPos != string::npos)
        {
            if ((dotPos == 0) || (sNewPath.at(dotPos - 1) == '/'))
            {
                sNewPath.erase(dotPos, 2);
            }
            else
            {
                pos = dotPos + 2;
            }
        }
        else
        {
            break;
        }
    }

    //如果路径是以.结尾的, 则.去掉
    if (((sNewPath.length() >= 2) && (sNewPath.substr(sNewPath.length()-2) == "/.")) || (sNewPath == "."))
    {
        sNewPath.erase(sNewPath.length() - 1);
    }

    //处理/../的形式
    pos = 0;
    size_t startPos = 0;

    while (1)
    {
        size_t slashDot = sNewPath.find("/../", pos);

        if (slashDot != string::npos)
        {
            if (0 == slashDot)
            {
                sNewPath.erase(0, 3);
                continue;
            }

            if ( (slashDot > 1) && (sNewPath.substr(slashDot - 2, 2) == "..") )
            {
                pos = slashDot + 4;
                continue;
            }

            startPos = sNewPath.rfind('/', slashDot - 1);

            if (startPos == string::npos) startPos = 0;

            sNewPath.erase(startPos, slashDot + 4 - startPos - 1);
        }
        else
        {
            break;
        }
    }

    //处理/..结尾的情况
    if ((sNewPath.size() >= 3) && (sNewPath.substr(sNewPath.size() - 3, 3) == "/.."))
    {
        size_t slashDot = sNewPath.size() - 3;
        if (!((slashDot > 1) && (sNewPath.substr(slashDot - 2, 2) == "..")))
        {
            startPos = sNewPath.rfind ('/', slashDot - 1);
            if (startPos == string::npos) startPos = 0;
            sNewPath.erase (startPos + 1);
        }
    }

    return sNewPath;
                /*
    string result = path;

    string::size_type pos;

    pos = 0;
    while((pos = result.find("//", pos)) != string::npos)
    {
        result.erase(pos, 1);
    }

    pos = 0;
    while((pos = result.find("/./", pos)) != string::npos)
    {
        result.erase(pos, 2);
    }

    pos = 0;
    while((pos = result.find("/../", pos)) != string::npos)
    {
        result.erase(pos, 3);
    }

    while(result.substr(0, 4) == "/../")
    {
        result.erase(0, 3);
    }

    if(result == "/.")
    {
       return result.substr(0, result.size() - 1);
    }

    if(result.size() >= 2 && result.substr(result.size() - 2, 2) == "/.")
    {
        result.erase(result.size() - 2, 2);
    }

    if(result == "/")
    {
        return result;
    }

    if(result.size() >= 1 && result[result.size() - 1] == '/')
    {
        result.erase(result.size() - 1);
    }

    if(result == "/..")
    {
        result = "/";
    }

    return result;
    */
}

void testURL()
{
    TC_URL url;
    /*
    testURL(url, ":pass@www.google.com", "http://www.google.com/");


    testURL(url, "http://:pass@www.google.com", "http://www.google.com/");
    testURL(url, "http://www.google.com/abc/def", "http://www.google.com/abc/def");
    cout << url.getPath() << endl;
    cout << url.getQuery() << endl;

    testURL(url, "http://user:pass@google.com:99/foo;bar?q=a#ref", "http://user:pass@google.com:99/foo;bar?q=a#ref");
    testURL(url, "http://user@www.qq.com:8080/abc/dev/query?a=b&c=3#ref", "http://user@www.qq.com:8080/abc/dev/query?a=b&c=3#ref");
    testURL(url, "http://@www.qq.com:8080/abc/dev/query?a=b&c=3#ref", "http://www.qq.com:8080/abc/dev/query?a=b&c=3#ref");
    testURL(url, "http://www.qq.com/abc/dev/query?a=b&c=3#ref", "http://www.qq.com/abc/dev/query?a=b&c=3#ref");
    testURL(url, "http://www.qq.com?a=b&c=3#ref", "http://www.qq.com/?a=b&c=3#ref");
    testURL(url, "http://www.qq.com#ref", "http://www.qq.com/#ref");
    testURL(url, "http://www.qq.com?#ref", "http://www.qq.com/#ref");
    testURL(url, "http://:pass@www.google.com", "http://www.google.com/"),
    testURL(url, "http://:@www.google.com", "http://www.google.com/"),

    testURL(url, "ftp://user:password@www.qq.com:21/abc/dev/query?a=b&c=3#ref", "ftp://user:password@www.qq.com/abc/dev/query?a=b&c=3#ref");
    testURL(url, "https://user:password@www.qq.com:433/abc/dev/query?a=b&c=3#ref", "https://user:password@www.qq.com/abc/dev/query?a=b&c=3#ref");
    testURL(url, "https://user:password@www.qq.com:8080/abc/dev/query?a=b&c=3#ref", "https://user:password@www.qq.com:8080/abc/dev/query?a=b&c=3#ref");
*/
    testBuildURL(url, "https://user:password@www.qq.com:8080/abc/dev/query?a=b&c=3#ref", "abc/def", "https://user:password@www.qq.com:8080/abc/dev/abc/def");
    testBuildURL(url, "https://user:password@www.qq.com:8080/abc/dev/query?a=b&c=3#ref", "/abc/def", "https://user:password@www.qq.com:8080/abc/def");
    testBuildURL(url, "https://user:password@www.qq.com:8080/abc/dev/query?a=b&c=3#ref", "../../abc/def", "https://user:password@www.qq.com:8080/abc/def");
    testBuildURL(url, "https://user:password@www.qq.com:8080/abc/dev/query?a=b&c=3#ref", "#abc", "https://user:password@www.qq.com:8080/abc/def");
    testBuildURL(url, "http://www.qq.com", "#abc", "http://www.qq.com/#abc");
    testBuildURL(url, "http://www.qq.com/a/b", "#abc", "http://www.qq.com/#abc");
    testBuildURL(url, "http://www.qq.com/a/b", "../../#abc", "http://www.qq.com/#abc");
    testBuildURL(url, "http://www.qq.com/a/b", "./../../#abc", "http://www.qq.com/#abc");
    testBuildURL(url, "http://www.qq.com/a/b/", "../#abc", "http://www.qq.com/#abc");
    testBuildURL(url, "http://www.qq.com/a/b/", "../abc/def/..", "http://www.qq.com/#abc");
    testBuildURL(url, "http://www.qq.com/a/b", "../abc/def/../../.", "http://www.qq.com/#abc");
}

void testCookie(const string &sRspURL, const string &sReqURL, const vector<string> &vsCookie)
{
    cout << sRspURL << "=>" << sReqURL << "-----------------------------------" << endl;

    TC_HttpCookie cookie;

    cookie.addCookie(sRspURL, vsCookie);

    list<TC_HttpCookie::Cookie> vCookie = cookie.getAllCookie();

    cout << "All Cookie:" << sRspURL << "-----------------------------------" << endl;

    list<TC_HttpCookie::Cookie>::iterator it = vCookie.begin();

    while(it != vCookie.end())
    {
        cout << TC_Common::tostr(it->_data.begin(), it->_data.end(), "; ") << ", " << it->_expires << ", " << it->_path << endl;

        ++it;
    }

    cout << "-----------------------------------" << endl << endl;

    string sCookie;

    cookie.getCookieForURL(sReqURL, sCookie);

    cout << TC_Common::tostr(sCookie) << endl;    

    cout << "-----------------------------------" << endl << endl;
}

void testCookie()
{
    assert(TC_HttpCookie::matchDomain("qq.com", "www.qq.com") == true);
    assert(TC_HttpCookie::matchDomain(".qq.com", "www.qq.com") == true);
    assert(TC_HttpCookie::matchDomain(".qq.com", "qq.com") == true);
    assert(TC_HttpCookie::matchDomain("t.qq.com", "www.qq.com") == false);
    assert(TC_HttpCookie::matchDomain(".t.qq.com", "www.qq.com") == false);
    assert(TC_HttpCookie::matchDomain(".t.qq.com", "t.qq.com") == true);
    assert(TC_HttpCookie::matchDomain(".com", "www.qq.com") == false);
    assert(TC_HttpCookie::matchDomain(".com", "qq.com") == false);
    assert(TC_HttpCookie::matchDomain(".y.qq.com", "x.y.qq.com") == true);
    assert(TC_HttpCookie::matchDomain(".x.y.qq.com", "x.y.qq.com") == true);
    assert(TC_HttpCookie::matchDomain(".qq.com", "x.y.qq.com") == true);
    assert(TC_HttpCookie::matchDomain(".qq.com", "y.qq.com") == true);
    assert(TC_HttpCookie::matchDomain("qq.com", "y.qq.com") == true);

    cout << TC_Common::now2GMTstr() << endl;

    string gmt = TC_Common::tm2GMTstr(time(NULL) + 10);

    string s = "HTTP/1.1  200  OK\r\n";//  200  Aouut Error\r\n";
    s += "Set-Cookie: n1=1; a=1; c=d; Path=/; Domain=qq.com; Expires=" + gmt + "\r\n";
    s += "Set-Cookie: n2=2; a=0; c=d; Path=/abc/def; Domain=.qq.com; Expires=" + gmt + "\r\n";
    s += "Set-Cookie: n3=3; a=5; c=d; Path=/abc/def/aaa; Domain=.qq.com; Expires=" + gmt + "\r\n";
    s += "Set-Cookie: n4=4; a=6; c=d; Path=/abc; Domain=.qq.com; Expires=" + gmt + "\r\n";
    s += "Set-Cookie: n5=5; a=2; c=d; Path=/; Domain=.qq.com; Expires=" + gmt + "\r\n";
    s += "Set-Cookie: n6=6; c=3; Path=/; Domain=y.qq.com; Expires=" + gmt + "\r\n";
    s += "Set-Cookie: n7=7; c=3; Path=/abc; Domain=.y.qq.com; Expires=" + gmt + "\r\n";
    s += "Set-Cookie: n8=6; c=3; Path=/; Domain=x.y.qq.com; Expires=" + gmt + "\r\n";
    s += "Set-Cookie: n9=7; c=4; Path=/; Domain=.x.y.qq.com; Expires=" + gmt + "\r\n";
    s += "Set-Cookie: n10=7; c=4; Path=/; Domain=qqq.com; Expires=" + gmt + "\r\n";
    s += "Set-Cookie: n11=7; c=4; Path=/; Domain=.qqq.com; Expires=" + gmt + "\r\n";
    s += "Set-Cookie: n12=8; c=4; Expires=" + gmt + "\r\n";
    s += "Accept-Ranges: bytes\r\n\r\n";

    TC_HttpResponse rsp;
    rsp.decode(s);

    cout << "-----------------------------------" << endl;

    vector<string> vsCookie = rsp.getSetCookie();

    cout << TC_Common::tostr(vsCookie.begin(), vsCookie.end(), "\r\n") << endl << endl;

    testCookie("http://www.qq.com", "http://www.qq.com", vsCookie);
    testCookie("http://www.qq.com/abc/def", "http://www.qq.com", vsCookie);
    testCookie("http://www.qq.com/abc/def", "http://www.qq.com/abc", vsCookie);

    cout << endl;
//    sleep(11);

    testCookie("http://www.qq.com", "http://qq.com", vsCookie);
    testCookie("http://www.qq.com/abc/def/aaa", "http://www.qq.com/abc/def/aaa", vsCookie);
    testCookie("http://www.qq.com/abc/def/aaa", "http://www.qq.com", vsCookie);
    testCookie("http://www.qq.com", "http://www.qq.com/abc/def", vsCookie);
    testCookie("http://qq.com", "http://qq.com/abc/def", vsCookie);
    testCookie("http://qq.com", "http://t.qq.com/abc/def", vsCookie);
    testCookie("http://qq.com", "http://y.qq.com/", vsCookie);
    testCookie("http://qq.com", "http://y.qq.com/abc", vsCookie);
    testCookie("http://x.y.qq.com", "http://x.y.qq.com", vsCookie);
}

int main(int argc, char *argv[])
{
    try
    {
//        testHttp1();
//        testHttp2();

//        return 0;

        cout << "testURL-----------------------------------" << endl;
        testURL();

        return 0;
        cout << "testHttpHeader2-----------------------------------" << endl;
        testHttpHeader2();
        cout << "-----------------------------------" << endl;
        testHttpHeader3();
        cout << "testHttpHeader3-----------------------------------" << endl;
        testHttpHeader4();
        cout << "testHttpHeader4-----------------------------------" << endl;
        testHttpsHeader2();
        cout << "testCookie-----------------------------------" << endl;
//        testCookie();

//        testHttpResponse();
//        testEastMoney();

//        testHttp2();
//        testTC_HTTPClient();
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


