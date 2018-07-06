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

#include "util/tc_common.h"
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <cmath>

namespace tars
{
template <>
string TC_Common::tostr<bool>(const bool &t)
{
    char buf[2];
    buf[0] = t ? '1' : '0';
    buf[1] = '\0';
    return string(buf);
}


template <>
string TC_Common::tostr<char>(const char &t)
{
    char buf[2];
    snprintf(buf, 2, "%c", t);
    return string(buf);
}

template <>
string TC_Common::tostr<unsigned char>(const unsigned char &t)
{
    char buf[2];
    snprintf(buf, 2, "%c", t);
    return string(buf);
}

template <>
string TC_Common::tostr<short>(const short &t)
{
    char buf[16];
    snprintf(buf, 16, "%d", t);
    return string(buf);
}

template <>
string TC_Common::tostr<unsigned short>(const unsigned short &t)
{
    char buf[16];
    snprintf(buf, 16, "%u", t);
    return string(buf);
}

template <>
string TC_Common::tostr<int>(const int &t)
{
    char buf[16];
    snprintf(buf, 16, "%d", t);
    return string(buf);
}

template <>
string TC_Common::tostr<unsigned int>(const unsigned int &t)
{
    char buf[16];
    snprintf(buf, 16, "%u", t);
    return string(buf);
}

template <>
string TC_Common::tostr<long>(const long &t)
{
    char buf[32];
    snprintf(buf, 32, "%ld", t);
    return string(buf);
}

template <>
string TC_Common::tostr<long long>(const long long &t)
{
    char buf[32];
    snprintf(buf, 32, "%lld", t);
    return string(buf);
}


template <>
string TC_Common::tostr<unsigned long>(const unsigned long &t)
{
    char buf[32];
    snprintf(buf, 32, "%lu", t);
    return string(buf);
}

template <>
string TC_Common::tostr<float>(const float &t)
{
    char buf[32];
    snprintf(buf, 32, "%.5f", t);
    string s(buf);

    //去掉无效0, eg. 1.0300 -> 1.03;1.00 -> 1
    bool bFlag = false;
    int pos = int(s.size() - 1);
    for(; pos > 0; --pos)
    {
        if(s[pos] == '0')
        {
            bFlag = true;
            if(s[pos-1] == '.')
            {
                //-2为了去掉"."号
                pos -= 2;
                break;
            }
        }
        else
        {
            break;
        }
    }

    if(bFlag)
        s = s.substr(0, pos+1);

    return s;
}

template <>
string TC_Common::tostr<double>(const double &t)
{
    char buf[32];
    snprintf(buf, 32, "%.5f", t);
    string s(buf);

    //去掉无效0, eg. 1.0300 -> 1.03;1.00 -> 1
    bool bFlag = false;
    int pos = int(s.size() - 1);
    for(; pos > 0; --pos)
    {
        if(s[pos] == '0')
        {
            bFlag = true;
            if(s[pos-1] == '.')
            {
                //-2为了去掉"."号
                pos -= 2;
                break;
            }
        }
        else
        {
            break;
        }
    }

    if(bFlag)
        s = s.substr(0, pos+1);

    return s;

}

template <>
string TC_Common::tostr<long double>(const long double &t)
{
    char buf[32];
    snprintf(buf, 32, "%Lf", t);
    string s(buf);

    //去掉无效0, eg. 1.0300 -> 1.03;1.00 -> 1
    bool bFlag = false;
    int pos = int(s.size() - 1);
    for(; pos > 0; --pos)
    {
        if(s[pos] == '0')
        {
            bFlag = true;
            if(s[pos-1] == '.')
            {
                //-2为了去掉"."号
                pos -= 2;
                break;
            }
        }
        else
        {
            break;
        }
    }

    if(bFlag)
        s = s.substr(0, pos+1);

    return s;

}

template <>
string TC_Common::tostr<std::string>(const std::string &t)
{
    return t;
}

string TC_Common::trim(const string &sStr, const string &s, bool bChar)
{
    if(sStr.empty())
    {
        return sStr;
    }

    /**
    * 将完全与s相同的字符串去掉
    */
    if(!bChar)
    {
        return trimright(trimleft(sStr, s, false), s, false);
    }

    return trimright(trimleft(sStr, s, true), s, true);
}

string TC_Common::trimleft(const string &sStr, const string &s, bool bChar)
{
    if(sStr.empty())
    {
        return sStr;
    }

    /**
    * 去掉sStr左边的字符串s
    */
    if(!bChar)
    {
        if(sStr.length() < s.length())
        {
            return sStr;
        }

        if(sStr.compare(0, s.length(), s) == 0)
        {
            return sStr.substr(s.length());
        }

        return sStr;
    }

    /**
    * 去掉sStr左边的 字符串s中的字符
    */
    string::size_type pos = 0;
    while(pos < sStr.length())
    {
        if(s.find_first_of(sStr[pos]) == string::npos)
        {
            break;
        }

        pos++;
    }

    if(pos == 0) return sStr;

    return sStr.substr(pos);
}

string TC_Common::trimright(const string &sStr, const string &s, bool bChar)
{
    if(sStr.empty())
    {
        return sStr;
    }

    /**
    * 去掉sStr右边的字符串s
    */
    if(!bChar)
    {
        if(sStr.length() < s.length())
        {
            return sStr;
        }

        if(sStr.compare(sStr.length() - s.length(), s.length(), s) == 0)
        {
            return sStr.substr(0, sStr.length() - s.length());
        }

        return sStr;
    }

    /**
    * 去掉sStr右边的 字符串s中的字符
    */
    string::size_type pos = sStr.length();
    while(pos != 0)
    {
        if(s.find_first_of(sStr[pos-1]) == string::npos)
        {
            break;
        }

        pos--;
    }

    if(pos == sStr.length()) return sStr;

    return sStr.substr(0, pos);
}

string TC_Common::lower(const string &s)
{
    string sString = s;
    for (string::iterator iter = sString.begin(); iter != sString.end(); ++iter)
    {
        *iter = tolower(*iter);
    }

    return sString;
}

string TC_Common::upper(const string &s)
{
    string sString = s;

    for (string::iterator iter = sString.begin(); iter != sString.end(); ++iter)
    {
        *iter = toupper(*iter);
    }

    return sString;
}

bool TC_Common::isdigit(const string &sInput)
{
    string::const_iterator iter = sInput.begin();

    if(sInput.empty())
    {
        return false;
    }

    while(iter != sInput.end())
    {
        if (!::isdigit(*iter))
        {
            return false;
        }
        ++iter;
    }
    return true;
}

int TC_Common::str2tm(const string &sString, const string &sFormat, struct tm &stTm)
{
    char *p = strptime(sString.c_str(), sFormat.c_str(), &stTm);
    return (p != NULL) ? 0 : -1;
}

int TC_Common::strgmt2tm(const string &sString, struct tm &stTm)
{
    return str2tm(sString, "%a, %d %b %Y %H:%M:%S GMT", stTm);
}

string TC_Common::tm2str(const struct tm &stTm, const string &sFormat)
{
    char sTimeString[255] = "\0";

    strftime(sTimeString, sizeof(sTimeString), sFormat.c_str(), &stTm);

    return string(sTimeString);
}

string TC_Common::tm2str(const time_t &t, const string &sFormat)
{
    struct tm tt;
    localtime_r(&t, &tt);

    return tm2str(tt, sFormat);
}

string TC_Common::now2str(const string &sFormat)
{
    time_t t = time(NULL);
    return tm2str(t, sFormat.c_str());
}

string TC_Common::now2GMTstr()
{
    time_t t = time(NULL);
    return tm2GMTstr(t);
}

string TC_Common::tm2GMTstr(const time_t &t)
{
    struct tm tt;
    gmtime_r(&t, &tt);
    return tm2str(tt, "%a, %d %b %Y %H:%M:%S GMT");
}

string TC_Common::tm2GMTstr(const struct tm &stTm)
{
    return tm2str(stTm, "%a, %d %b %Y %H:%M:%S GMT");
}

string TC_Common::nowdate2str()
{
    return now2str("%Y%m%d");
}

string TC_Common::nowtime2str()
{
    return now2str("%H%M%S");
}

int64_t TC_Common::now2ms()
{
    struct timeval tv;

    gettimeofday(&tv, 0);

    return tv.tv_sec * (int64_t)1000 + tv.tv_usec/1000;
}

int64_t TC_Common::now2us()
{
    struct timeval tv;

    gettimeofday(&tv, 0);

    return tv.tv_sec * (int64_t)1000000 + tv.tv_usec;
}

//参照phorix的优化
static char c_b2s[256][4]={"00","01","02","03","04","05","06","07","08","09","0a","0b","0c","0d","0e","0f","10","11","12","13","14","15","16","17","18","19","1a","1b","1c","1d","1e","1f","20","21","22","23","24","25","26","27","28","29","2a","2b","2c","2d","2e","2f","30","31","32","33","34","35","36","37","38","39","3a","3b","3c","3d","3e","3f","40","41","42","43","44","45","46","47","48","49","4a","4b","4c","4d","4e","4f","50","51","52","53","54","55","56","57","58","59","5a","5b","5c","5d","5e","5f","60","61","62","63","64","65","66","67","68","69","6a","6b","6c","6d","6e","6f","70","71","72","73","74","75","76","77","78","79","7a","7b","7c","7d","7e","7f","80","81","82","83","84","85","86","87","88","89","8a","8b","8c","8d","8e","8f","90","91","92","93","94","95","96","97","98","99","9a","9b","9c","9d","9e","9f","a0","a1","a2","a3","a4","a5","a6","a7","a8","a9","aa","ab","ac","ad","ae","af","b0","b1","b2","b3","b4","b5","b6","b7","b8","b9","ba","bb","bc","bd","be","bf","c0","c1","c2","c3","c4","c5","c6","c7","c8","c9","ca","cb","cc","cd","ce","cf","d0","d1","d2","d3","d4","d5","d6","d7","d8","d9","da","db","dc","dd","de","df","e0","e1","e2","e3","e4","e5","e6","e7","e8","e9","ea","eb","ec","ed","ee","ef","f0","f1","f2","f3","f4","f5","f6","f7","f8","f9","fa","fb","fc","fd","fe","ff"};

string TC_Common::bin2str(const void *buf, size_t len, const string &sSep, size_t lines)
{
    if(buf == NULL || len <=0 )
    {
        return "";
    }

    string sOut;
    const unsigned char *p = (const unsigned char *) buf;

    for (size_t i = 0; i < len; ++i, ++p)
    {
        sOut += c_b2s[*p][0];
        sOut += c_b2s[*p][1];
        sOut += sSep;

        //换行
        if ((lines != 0) && ((i+1) % lines == 0))
        {
            sOut += "\n";
        }
    }

    return sOut;
}

string TC_Common::bin2str(const string &sBinData, const string &sSep, size_t lines)
{
    return bin2str((const void *)sBinData.data(), sBinData.length(), sSep, lines);
}

int TC_Common::str2bin(const char *psAsciiData, unsigned char *sBinData, int iBinSize)
{
    int iAsciiLength = strlen(psAsciiData);

    int iRealLength = (iAsciiLength/2 > iBinSize)?iBinSize:(iAsciiLength/2);
    for (int i = 0 ; i < iRealLength ; i++)
    {
        sBinData[i] = x2c(psAsciiData + i*2);
    }
    return iRealLength;
}

string TC_Common::str2bin(const string &sString, const string &sSep, size_t lines)
{
    const char *psAsciiData = sString.c_str();

    int iAsciiLength = sString.length();
    string sBinData;
    for (int i = 0 ; i < iAsciiLength ; i++)
    {
        sBinData += x2c(psAsciiData + i);
        i++;
        i += sSep.length(); //过滤掉分隔符

        if (lines != 0 && sBinData.length()%lines == 0)
        {
            i++;    //过滤掉回车
        }
    }

    return sBinData;
}

char TC_Common::x2c(const string &sWhat)
{
    register char digit;

    if(sWhat.length() < 2)
    {
        return '\0';
    }

    digit = (sWhat[0] >= 'A' ? ((sWhat[0] & 0xdf) - 'A')+10 : (sWhat[0] - '0'));
    digit *= 16;
    digit += (sWhat[1] >= 'A' ? ((sWhat[1] & 0xdf) - 'A')+10 : (sWhat[1] - '0'));

    return(digit);
}

string TC_Common::replace(const string &sString, const string &sSrc, const string &sDest)
{
    if(sSrc.empty())
    {
        return sString;
    }

    string sBuf = sString;

    string::size_type pos = 0;

    while( (pos = sBuf.find(sSrc, pos)) != string::npos)
    {
        sBuf.replace(pos, sSrc.length(), sDest);
        pos += sDest.length();
    }

    return sBuf;
}

string TC_Common::replace(const string &sString, const map<string,string>& mSrcDest)
{
    if(sString.empty())
    {
        return sString;
    }

    string tmp = sString;
    map<string,string>::const_iterator it = mSrcDest.begin();

    while(it != mSrcDest.end())
    {

        string::size_type pos = 0;
        while((pos = tmp.find(it->first, pos)) != string::npos)
        {
            tmp.replace(pos, it->first.length(), it->second);
            pos += it->second.length();
        }

        ++it;
    }

    return tmp;
}

bool TC_Common::matchPeriod(const string& s, const string& pat)
{
    if(s.empty())
    {
        return false;
    }

    if(pat.empty())
    {
        return true;
    }

    if(pat.find('*') == string::npos)
    {
        return s == pat;
    }

    string::size_type sIndex = 0;
    string::size_type patIndex = 0;
    do
    {
        if(pat[patIndex] == '*')
        {
            if(s[sIndex] == '.')
            {
                return false;
            }

            while(sIndex < s.size() && s[sIndex] != '.')
            {
                ++sIndex;
            }
            patIndex++;
        }
        else
        {
            if(pat[patIndex] != s[sIndex])
            {
                return false;
            }
            ++sIndex;
            ++patIndex;
        }
    }
    while(sIndex < s.size() && patIndex < pat.size());

    return sIndex == s.size() && patIndex == pat.size();
}

bool TC_Common::matchPeriod(const string& s, const vector<string>& pat)
{
    for(size_t i = 0; i < pat.size(); i++)
    {
        if(TC_Common::matchPeriod(s,pat[i]))
        {
            return true;
        }
    }
    return false;
}

void TC_Common::daemon()
{
    pid_t pid;

    if ((pid = fork()) != 0)
    {
        exit(0);
    }

    setsid();

    signal(SIGINT, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    ignorePipe();

    if((pid = fork()) != 0)
    {
        //父进程结束,变成daemon
        exit(0);
    }

    umask(0);

  //  chdir("/");
}

void TC_Common::ignorePipe()
{
    struct sigaction sig;

    memset(&sig,0,sizeof(struct sigaction));

    sig.sa_handler = SIG_IGN;
    sig.sa_flags = 0;
    sigemptyset(&sig.sa_mask);
    sigaction(SIGPIPE,&sig,NULL);
}

bool TC_Common::isPrimeNumber(size_t n)
{
    size_t nn = (size_t)sqrt((double)n);
    for(size_t i = 2; i < nn;i++)
    {
        if(n % i == 0)
        {
            return false;
        }
    }
    return true;
}

size_t TC_Common::toSize(const string &s, size_t iDefaultSize)
{
    if (s.empty())
    {
        return iDefaultSize;
    }

    char c = s[s.length()-1];
    if(c != 'K' && c != 'M' && c != 'G' && TC_Common::trim(s) == TC_Common::tostr(TC_Common::strto<size_t>(s)))
    {
        //没有后缀, 且转换是正确的
        return (size_t)(TC_Common::strto<size_t>(s));
    }
    else if(c == 'K' || c == 'M' || c == 'G')
    {
        if (s.length() == 1)
        {
            return iDefaultSize;
        }

        float n = TC_Common::strto<float>(s.substr(0, s.length()-1));

        if(TC_Common::trim(s.substr(0, s.length()-1)) != TC_Common::tostr(n))
        {
            return iDefaultSize;
        }

        if(c == 'K')
        {
            return (size_t)(n * 1024);
        }
        if(c == 'M')
        {
            return (size_t)(n * 1024*1024);
        }
        if(c == 'G')
        {
            return (size_t)(n * 1024 * 1024 * 1024);
        }
    }

    return iDefaultSize;
}

// Generate the randome string, a SHA1-sized random number
void TC_Common::getRandomHexChars(char* p, unsigned int len)
{
    const char* const chars = "0123456789abcdef";
    FILE *fp = fopen("/dev/urandom","r");
    if (!fp || fread(p,len,1,fp) == 0)
    {
        for (unsigned int j = 0; j < len; j++)
            p[j] ^= rand();
    }

    if (fp) fclose(fp);
   
    for (unsigned int j = 0; j < len; j++)
        p[j] = chars[p[j] & 0x0F];

}

}

