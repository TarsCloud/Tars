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

#include "util/tc_cgi.h"
#include "util/tc_common.h"

using namespace tars;

void testStdin()
{
    string sFileName = "/data/tars/test/util/" + TC_Common::now2str() + ".tmp";

    TC_Cgi cgi;
    cout << TC_Cgi::htmlHeader("text/html"); 

    cgi.setUpload(sFileName, 2, 1024*1024*10);
    cgi.parseCgi();

    cout << cgi.setCookie(cgi.getValue("cookie1"), cgi.getValue("cookie2"), TC_Common::now2str());
    cout << "value:" << cgi.getValue("value") << endl;

    string s = "abc 中文"; 
    cout << TC_Cgi::encodeURL(s) << "<br/>";
    cout << TC_Cgi::decodeURL(s) << "<br/>";;

    cout << TC_Cgi::encodeHTML(cgi.getValue("D1"), true) ;
    cout << TC_Common::tostr(cgi.getParamMap()) << "<br/>";
    cout << "<br/>";
    cout << TC_Common::tostr(cgi.getEnvMap()) << "<br/>";
}

void parseNormal(multimap<string, string> &mmpParams, const string& sBuffer)
{
    int iFlag = 0;
    string sName;
    string sValue;
    string sTmp;
    string::size_type len = sBuffer.length();
    string::size_type pos = 0;

    while (pos < len)
    {
        sTmp = "";

        if(iFlag == 0)
        {
            while ( (sBuffer[pos] != '=') && (pos < len) )
            {
                sTmp += (sBuffer[pos] == '+') ? ' ' : sBuffer[pos];

                ++pos;
            }
        }
        else
        {
            while ( (sBuffer[pos] != '&') && (pos < len) )
            {
                sTmp += (sBuffer[pos] == '+') ? ' ' : sBuffer[pos];

                ++pos;
            }
        }

        if (iFlag == 0)                         //param name
        {
            sName = TC_Cgi::decodeURL(sTmp);

            if ( (sBuffer[pos] != '=') || (pos == len - 1) )
            {
                sValue = "";

                mmpParams.insert(multimap<string, string>::value_type(sName, sValue));
            }
            else
            {
                iFlag = 1;
            }
        }
        else
        {
            sValue = TC_Cgi::decodeURL(sTmp);

            mmpParams.insert(multimap<string, string>::value_type(sName, sValue));

            iFlag = 0;
        }

        ++pos;
    }
}

void testCgiParam(const string &buffer)
{
    

    multimap<string, string> m;

    parseNormal(m, buffer);

    cout << TC_Common::tostr(m) << endl;
}

int main(int argc, char *argv[])
{
    try
    {
//        testStdin();
//        testCgiParam("a=b&c=d&e=f=");
//        testCgiParam("abc&=b&c=d&e=f=");
//        testCgiParam("a++&=b&c=d&e=f=");
//        testCgiParam("=b&c=d===&e=f=");
//        testCgiParam("=b&c=d++=&e=f=");

        testCgiParam("db=sina&bid=161423,230233,235253&cid=0,0,0&sid=224587&advid=1293&camid=25420&show=ignore&url=http://bbs.fangyou.com/viewthread.php?tid=1021196&pi=sina-langshou-wzl1");

    }                              
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}
