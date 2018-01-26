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
#include "util/tc_common.h"

#include <iterator>
#include <sys/time.h>
using namespace tars;



string tm2GMTstr(const time_t &t)
{
    struct tm tt;
    gmtime_r(&t, &tt);
    return TC_Common::tm2str(tt, "%a, %d %b %Y %H:%M:%S GMT");
}

extern char *tzname[2];
extern long timezone;

struct CmpCase
{
    bool operator()(const string &s1, const string &s2)
    {
        return TC_Common::upper(s1) < TC_Common::upper(s2);
    }
};

int main(int argc, char *argv[])
{
    try
    {
        map<string, string, CmpCase> m;
        m["abc"] = "def";
        m["Abc"] = "def";
        m["AbC"] = "def";
        m["AbCd"] = "def";
        m["AbCD"] = "def";

        cout << TC_Common::tostr(m) << endl;
        cout << m["ABC"] << endl;
        cout << m["ABCD"] << endl;


        /*
        {
            string n = "abc";
        vector<char> s;
        s.assign(n.c_str(), n.c_str() + n.length());
        cout << s.size() << ":" << s.capacity() << endl;
        s.insert(s.end(), n.c_str(), n.c_str() + n.length());
        cout << s.size() << ":" << s.capacity() << endl;
        s.insert(s.end(), n.c_str(), n.c_str() + n.length());
        cout << s.size() << ":" << s.capacity() << endl;
        s.clear();
        cout << s.size() << ":" << s.capacity() << endl;
        s.reserve(100);
        memcpy((&s[0] + s.size()), "dfetdg", 6);
        cout << TC_Common::tostr(s) << endl;

        s.resize(s.size() + 10);
        cout << s.size() << ":" << s.capacity() << endl;
        cout << TC_Common::tostr(s) << endl;
        }
        return 0;

        {
        string s;
        s = "abc";
        cout << s.length() << ":" << s.capacity() << endl;
        s += "abcd";
        cout << s.length() << ":" << s.capacity() << endl;
        s += "abcd";
        cout << s.length() << ":" << s.capacity() << endl;
//        s.clear();
//        s = s.substr(5);
        s.erase(s.begin(), s.begin() + min((size_t)100, s.size()));
        cout << s.size() << ":" << s.capacity() << endl;
        s.reserve(100);
        cout << s.length() << ":" << s.capacity() << endl;
        }
        return 0;

        time_t tt = time(NULL);
        string ss = tm2GMTstr(tt);
        cout << ss << endl;
        cout << tt << endl;

//        Sat, 02 Feb 2010 08:57:34 GMT
//        Fri, 30 Oct 1998 13:19:41 GMT

//        tzset();
        struct tm stTm;
        int ret = TC_Common::strgmt2tm(ss, stTm);

        cout << stTm.tm_sec << endl;
        cout << stTm.tm_min << endl;  
        cout << stTm.tm_hour << endl; 
        cout << stTm.tm_mday << endl; 
        cout << stTm.tm_mon << endl;  
        cout << stTm.tm_year << endl; 
        cout << timezone << endl;
        cout << mktime(&stTm) - timezone << endl;

        cout << "***" << endl;
        cout << stTm.tm_zone << endl;
        cout << "***" << endl;
        cout << tzname[0] << ":" << tzname[1] << endl;
        cout << endl;
        cout << mktime(&stTm) - tt << ":" << stTm.tm_year << endl;
        cout << ret << endl;
        return 0;

        vector<string> v = TC_Common::sepstr<string>("|a|b||c|", "|", true);
        cout << TC_Common::tostr(v.begin(), v.end(), "|") << endl;

        vector<string> v2 = TC_Common::sepstr<string>("|a|b||c|", "|", false);
        cout << TC_Common::tostr(v2.begin(), v2.end(), "|") << endl;

        vector<int> v1 = TC_Common::sepstr<int>("|3|4||2|", "|", true);
        cout << TC_Common::tostr(v1) << endl;

        map<string, string> m;
        m["abc"] = "";
        m["abc1"] = "ef1";
        m["abc2"] = "ef2";
        cout << TC_Common::tostr(m) << endl;

        v = TC_Common::sepstr<string>("|a|b||c|", "|", true);
        cout << TC_Common::tostr(v) << endl;

        return 0;

        struct timeval t;
        gettimeofday(&t, NULL);

        cout << TC_Common::tm2str(t.tv_sec, "%Y-%m-%d %H:%M:%S.") << outfill(TC_Common::tostr(t.tv_usec/1000), '0', 3) << "|";

        string s = "asdfoasdfijfa";
        cout << s << endl;
        string s1 = TC_Common::bin2str(s);
        cout << s1 << endl;
        string s2 = TC_Common::bin2str(s, " ", 4);
        cout << s2 << endl;
        string s3 = TC_Common::str2bin(s1);
        cout << s3 << endl;
        string s4 = TC_Common::str2bin(s2, " ", 4);
        cout << s4 << endl;

        cout << "matchPeriod :" << TC_Common::matchPeriod("127.0.0.1", "127.*.*.*") << endl;

        cout << "[" << TC_Common::trim("abc\n\t") << "]" << endl;
        cout << "[" << TC_Common::trimleft("abc\n\t") << "]" << endl;
        cout << "[" << TC_Common::trim("\r\nabc") << "]" << endl;
        cout << "[" << TC_Common::trim("\r\nabc\r\n") << "]" << endl;
        cout << "[" << TC_Common::trim(" abc ", " \r\t") << "]" << endl;
        cout << "[" << TC_Common::trim("abcdefab", "ab", false) << "]" << endl;

        cout << TC_Common::toSize("3K", 3000) << endl;
        cout << TC_Common::toSize("23M", 3000) << endl;
        cout << TC_Common::toSize("15G", 3000) << endl;
        cout << TC_Common::toSize("15", 3000) << endl;
        cout << TC_Common::toSize("M", 3000) << endl;
        cout << TC_Common::toSize("34.32M", 3000) << endl;
        cout << TC_Common::toSize("3.2G", 3000) << endl;
        cout << TC_Common::toSize("3r,.2G", 3000) << endl;
        */
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


