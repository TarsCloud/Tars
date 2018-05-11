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

#ifndef __TC_COMMON_H
#define __TC_COMMON_H

#ifndef __USE_XOPEN
#define __USE_XOPEN
#endif

#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cassert>
#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <map>
#include <stack>
#include <vector>

using namespace std;

namespace tars
{
/////////////////////////////////////////////////
/**
* @file tc_common.h
* @brief  帮助类,都是静态函数,提供一些常用的函数 .
*
*/
/////////////////////////////////////////////////

 /**
 * @brief  基础工具类，提供了一些非常基本的函数使用.
 *
 * 这些函数都是以静态函数提供。 包括以下几种函数:
 *
 * Trim类函数,大小写转换函数,分隔字符串函数（直接分隔字符串，
 *
 * 数字等）,时间相关函数,字符串转换函数,二进制字符串互转函数,
 *
 * 替换字符串函数,Ip匹配函数,判断一个数是否是素数等
 */
class TC_Common
{
public:

    /**
    * @brief  去掉头部以及尾部的字符或字符串.
    *
    * @param sStr    输入字符串
    * @param s       需要去掉的字符
    * @param bChar   如果为true, 则去掉s中每个字符; 如果为false, 则去掉s字符串
    * @return string 返回去掉后的字符串
    */
    static string trim(const string &sStr, const string &s = " \r\n\t", bool bChar = true);

    /**
    * @brief  去掉左边的字符或字符串.
    *
    * @param sStr    输入字符串
    * @param s       需要去掉的字符
    * @param bChar   如果为true, 则去掉s中每个字符; 如果为false, 则去掉s字符串
    * @return string 返回去掉后的字符串
    */
    static string trimleft(const string &sStr, const string &s = " \r\n\t", bool bChar = true);

    /**
    * @brief  去掉右边的字符或字符串.
    *
    * @param sStr    输入字符串
    * @param s       需要去掉的字符
    * @param bChar   如果为true, 则去掉s中每个字符; 如果为false, 则去掉s字符串
    * @return string 返回去掉后的字符串
    */
    static string trimright(const string &sStr, const string &s = " \r\n\t", bool bChar = true);

    /**
    * @brief  字符串转换成小写.
    *
    * @param sString  字符串
    * @return string  转换后的字符串
    */
    static string lower(const string &sString);

    /**
    * @brief  字符串转换成大写.
    *
    * @param sString  字符串
    * @return string  转换后的大写的字符串
    */
    static string upper(const string &sString);

    /**
    * @brief  字符串是否都是数字的.
    *
    * @param sString  字符串
    * @return bool    是否是数字
    */
    static bool isdigit(const string &sInput);

    /**
    * @brief  字符串转换成时间结构.
    *
    * @param sString  字符串时间格式
    * @param sFormat  格式
    * @param stTm     时间结构
    * @return         0: 成功, -1:失败
    */
    static int str2tm(const string &sString, const string &sFormat, struct tm &stTm);

    /**
     * @brief GMT格式的时间转化为时间结构
     *
     * eg.Sat, 06 Feb 2010 09:29:29 GMT, %a, %d %b %Y %H:%M:%S GMT
     *
     * 可以用mktime换成time_t, 但是注意时区 可以用mktime(&stTm)
     *
     * - timezone换成本地的秒(time(NULL)值相同) timezone是系统的 ,
     *
     * 需要extern long timezone;
     *
     * @param sString  GMT格式的时间
     * @param stTm     转换后的时间结构
     * @return         0: 成功, -1:失败
     */
    static int strgmt2tm(const string &sString, struct tm &stTm);

    /**
    * @brief  时间转换成字符串.
    *
    * @param stTm     时间结构
    * @param sFormat  需要转换的目标格式，默认为紧凑格式
    * @return string  转换后的时间字符串
    */
    static string tm2str(const struct tm &stTm, const string &sFormat = "%Y%m%d%H%M%S");

    /**
    * @brief  时间转换成字符串.
    *
    * @param t        时间结构
    * @param sFormat  需要转换的目标格式，默认为紧凑格式
    * @return string  转换后的时间字符串
    */
    static string tm2str(const time_t &t, const string &sFormat = "%Y%m%d%H%M%S");

    /**
    * @brief  当前时间转换成紧凑格式字符串
    * @param sFormat 格式，默认为紧凑格式
    * @return string 转换后的时间字符串
    */
    static string now2str(const string &sFormat = "%Y%m%d%H%M%S");

    /**
    * @brief  时间转换成GMT字符串，GMT格式:Fri, 12 Jan 2001 18:18:18 GMT
    * @param stTm    时间结构
    * @return string GMT格式的时间字符串
    */
    static string tm2GMTstr(const struct tm &stTm);

    /**
    * @brief  时间转换成GMT字符串，GMT格式:Fri, 12 Jan 2001 18:18:18 GMT
    * @param stTm    时间结构
    * @return string GMT格式的时间字符串
    */
    static string tm2GMTstr(const time_t &t);

    /**
    * @brief  当前时间转换成GMT字符串，GMT格式:Fri, 12 Jan 2001 18:18:18 GMT
    * @return string GMT格式的时间字符串
    */
    static string now2GMTstr();

    /**
    * @brief  当前的日期(年月日)转换成字符串(%Y%m%d).
    *
    * @return string (%Y%m%d)格式的时间字符串
    */
    static string nowdate2str();

    /**
    * @brief  当前的时间(时分秒)转换成字符串(%H%M%S).
    *
    * @return string (%H%M%S)格式的时间字符串
    */
    static string nowtime2str();

    /**
     * @brief  获取当前时间的的毫秒数.
     *
     * @return int64_t 当前时间的的毫秒数
     */
    static int64_t now2ms();

    /**
     * @brief  取出当前时间的微秒.
     *
     * @return int64_t 取出当前时间的微秒
     */
    static int64_t now2us();

    /**
    * @brief  字符串转化成T型，如果T是数值类型, 如果str为空,则T为0.
    *
    * @param sStr  要转换的字符串
    * @return T    T型类型
    */
    template<typename T>
    static T strto(const string &sStr);

    /**
    * @brief  字符串转化成T型.
    *
    * @param sStr      要转换的字符串
    * @param sDefault  缺省值
    * @return T        转换后的T类型
    */
    template<typename T>
    static T strto(const string &sStr, const string &sDefault);

    /**
    * @brief  解析字符串,用分隔符号分隔,保存在vector里
    *
    * 例子: |a|b||c|
    *
    * 如果withEmpty=true时, 采用|分隔为:"","a", "b", "", "c", ""
    *
    * 如果withEmpty=false时, 采用|分隔为:"a", "b", "c"
    *
    * 如果T类型为int等数值类型, 则分隔的字符串为"", 则强制转化为0
    *
    * @param sStr      输入字符串
    * @param sSep      分隔字符串(每个字符都算为分隔符)
    * @param withEmpty true代表空的也算一个元素, false时空的过滤
    * @return          解析后的字符vector
    */
    template<typename T>
    static vector<T> sepstr(const string &sStr, const string &sSep, bool withEmpty = false);

    /**
    * @brief T型转换成字符串，只要T能够使用ostream对象用<<重载,即可以被该函数支持
    * @param t 要转换的数据
    * @return  转换后的字符串
    */
    template<typename T>
    static string tostr(const T &t);

    /**
     * @brief  vector转换成string.
     *
     * @param t 要转换的vector型的数据
     * @return  转换后的字符串
     */
    template<typename T>
    static string tostr(const vector<T> &t);

    /**
     * @brief  把map输出为字符串.
     *
     * @param map<K, V, D, A>  要转换的map对象
     * @return                    string 输出的字符串
     */
    template<typename K, typename V, typename D, typename A>
    static string tostr(const map<K, V, D, A> &t);

    /**
     * @brief  map输出为字符串.
     *
     * @param multimap<K, V, D, A>  map对象
     * @return                      输出的字符串
     */
    template<typename K, typename V, typename D, typename A>
    static string tostr(const multimap<K, V, D, A> &t);

    /**
    * @brief  pair 转化为字符串，保证map等关系容器可以直接用tostr来输出
    * @param pair<F, S> pair对象
    * @return           输出的字符串
    */
    template<typename F, typename S>
    static string tostr(const pair<F, S> &itPair);

    /**
    * @brief  container 转换成字符串.
    *
    * @param iFirst  迭代器
    * @param iLast   迭代器
    * @param sSep    两个元素之间的分隔符
    * @return        转换后的字符串
    */
    template <typename InputIter>
    static string tostr(InputIter iFirst, InputIter iLast, const string &sSep = "|");

    /**
    * @brief  二进制数据转换成字符串.
    *
    * @param buf     二进制buffer
    * @param len     buffer长度
    * @param sSep    分隔符
    * @param lines   多少个字节换一行, 默认0表示不换行
    * @return        转换后的字符串
    */
    static string bin2str(const void *buf, size_t len, const string &sSep = "", size_t lines = 0);

    /**
    * @brief  二进制数据转换成字符串.
    *
    * @param sBinData  二进制数据
    * @param sSep     分隔符
    * @param lines    多少个字节换一行, 默认0表示不换行
    * @return         转换后的字符串
    */
    static string bin2str(const string &sBinData, const string &sSep = "", size_t lines = 0);

    /**
    * @brief   字符串转换成二进制.
    *
    * @param psAsciiData 字符串
    * @param sBinData    二进制数据
    * @param iBinSize    需要转换的字符串长度
    * @return            转换长度，小于等于0则表示失败
    */
    static int str2bin(const char *psAsciiData, unsigned char *sBinData, int iBinSize);

    /**
     * @brief  字符串转换成二进制.
     *
     * @param sBinData  要转换的字符串
     * @param sSep      分隔符
     * @param lines     多少个字节换一行, 默认0表示不换行
     * @return          转换后的二进制数据
     */
    static string str2bin(const string &sBinData, const string &sSep = "", size_t lines = 0);

    /**
    * @brief  替换字符串.
    *
    * @param sString  输入字符串
    * @param sSrc     原字符串
    * @param sDest    目的字符串
    * @return string  替换后的字符串
    */
    static string replace(const string &sString, const string &sSrc, const string &sDest);

    /**
    * @brief  批量替换字符串.
    *
    * @param sString  输入字符串
    * @param mSrcDest  map<原字符串,目的字符串>
    * @return string  替换后的字符串
    */
    static string replace(const string &sString, const map<string,string>& mSrcDest);

    /**
     * @brief 匹配以.分隔的字符串，pat中*则代表通配符，代表非空的任何字符串
     * s为空, 返回false ，pat为空, 返回true
     * @param s    普通字符串
     * @param pat  带*则被匹配的字符串，用来匹配ip地址
     * @return     是否匹配成功
     */
    static bool matchPeriod(const string& s, const string& pat);

     /**
     * @brief  匹配以.分隔的字符串.
     *
     * @param s   普通字符串
     * @param pat vector中的每个元素都是带*则被匹配的字符串，用来匹配ip地址
     * @return    是否匹配成功
     */
    static bool matchPeriod(const string& s, const vector<string>& pat);

    /**
     * @brief  判断一个数是否为素数.
     *
     * @param n  需要被判断的数据
     * @return   true代表是素数，false表示非素数
     */
    static bool isPrimeNumber(size_t n);

    /**
     * @brief  daemon
     */
    static void daemon();

    /**
     * @brief  忽略管道异常
     */
    static void ignorePipe();

    /**
     * @brief  将一个string类型转成一个字节 .
     *
     * @param sWhat 要转换的字符串
     * @return char    转换后的字节
     */
    static char x2c(const string &sWhat);

    /**
     * @brief  大小字符串换成字节数，支持K, M, G两种 例如: 1K, 3M, 4G, 4.5M, 2.3G
     * @param s            要转换的字符串
     * @param iDefaultSize 格式错误时, 缺省的大小
     * @return             字节数
     */
    static size_t toSize(const string &s, size_t iDefaultSize);
};

namespace p
{
    template<typename D>
    struct strto1
    {
        D operator()(const string &sStr)
        {
            string s = "0";

            if(!sStr.empty())
            {
                s = sStr;
            }

            istringstream sBuffer(s);

            D t;
            sBuffer >> t;

            return t;
        }
    };

    template<>
    struct strto1<char>
    {
        char operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return sStr[0];
            }
            return 0;
        }
    };

    template<>
    struct strto1<short>
    {
        short operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return atoi(sStr.c_str());
            }
            return 0;
        }
    };

    template<>
    struct strto1<unsigned short>
    {
        unsigned short operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return strtoul(sStr.c_str(), NULL, 10);
            }
            return 0;
        }
    };

    template<>
    struct strto1<int>
    {
        int operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return atoi(sStr.c_str());
            }
            return 0;
        }
    };

    template<>
    struct strto1<unsigned int>
    {
        unsigned int operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return strtoul(sStr.c_str(), NULL, 10);
            }
            return 0;
        }
    };

    template<>
    struct strto1<long>
    {
        long operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return atol(sStr.c_str());
            }
            return 0;
        }
    };

    template<>
    struct strto1<long long>
    {
        long long operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return atoll(sStr.c_str());
            }
            return 0;
        }
    };

    template<>
    struct strto1<unsigned long>
    {
        unsigned long operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return strtoul(sStr.c_str(), NULL, 10);
            }
            return 0;
        }
    };

    template<>
    struct strto1<float>
    {
        float operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return atof(sStr.c_str());
            }
            return 0;
        }
    };

    template<>
    struct strto1<double>
    {
        double operator()(const string &sStr)
        {
            if(!sStr.empty())
            {
                return atof(sStr.c_str());
            }
            return 0;
        }
    };

    template<typename D>
    struct strto2
    {
        D operator()(const string &sStr)
        {
            istringstream sBuffer(sStr);

            D t;
            sBuffer >> t;

            return t;
        }
    };

    template<>
    struct strto2<string>
    {
        string operator()(const string &sStr)
        {
            return sStr;
        }
    };

}

template<typename T>
T TC_Common::strto(const string &sStr)
{
    using strto_type = typename std::conditional<std::is_arithmetic<T>::value, p::strto1<T>, p::strto2<T>>::type;

    return strto_type()(sStr);
}

template<typename T>
T TC_Common::strto(const string &sStr, const string &sDefault)
{
    string s;

    if(!sStr.empty())
    {
        s = sStr;
    }
    else
    {
        s = sDefault;
    }

    return strto<T>(s);
}


template<typename T>
vector<T> TC_Common::sepstr(const string &sStr, const string &sSep, bool withEmpty)
{
    vector<T> vt;

    string::size_type pos = 0;
    string::size_type pos1 = 0;

    while(true)
    {
        string s;
        pos1 = sStr.find_first_of(sSep, pos);
        if(pos1 == string::npos)
        {
            if(pos + 1 <= sStr.length())
            {
                s = sStr.substr(pos);
            }
        }
        else if(pos1 == pos)
        {
            s = "";
        }
        else
        {
            s = sStr.substr(pos, pos1 - pos);
            pos = pos1;
        }

        if(withEmpty)
        {
            vt.push_back(strto<T>(s));
        }
        else
        {
            if(!s.empty())
            {
                T tmp = strto<T>(s);
                vt.push_back(tmp);
            }
        }

        if(pos1 == string::npos)
        {
            break;
        }

        pos++;
    }

    return vt;
}
template<typename T>
string TC_Common::tostr(const T &t)
{
    ostringstream sBuffer;
    sBuffer << t;
    return sBuffer.str();
}

template<typename T>
string TC_Common::tostr(const vector<T> &t)
{
    string s;
    for(size_t i = 0; i < t.size(); i++)
    {
        s += tostr(t[i]);
        s += " ";
    }
    return s;
}

template<typename K, typename V, typename D, typename A>
string TC_Common::tostr(const map<K, V, D, A> &t)
{
    string sBuffer;
    typename map<K, V, D, A>::const_iterator it = t.begin();
    while(it != t.end())
    {
        sBuffer += " [";
        sBuffer += tostr(it->first);
        sBuffer += "]=[";
        sBuffer += tostr(it->second);
        sBuffer += "] ";
        ++it;
    }
    return sBuffer;
}

template<typename K, typename V, typename D, typename A>
string TC_Common::tostr(const multimap<K, V, D, A> &t)
{
    string sBuffer;
    typename multimap<K, V, D, A>::const_iterator it = t.begin();
    while(it != t.end())
    {
        sBuffer += " [";
        sBuffer += tostr(it->first);
        sBuffer += "]=[";
        sBuffer += tostr(it->second);
        sBuffer += "] ";
        ++it;
    }
    return sBuffer;
}

template<typename F, typename S>
string TC_Common::tostr(const pair<F, S> &itPair)
{
    string sBuffer;
    sBuffer += "[";
    sBuffer += tostr(itPair.first);
    sBuffer += "]=[";
    sBuffer += tostr(itPair.second);
    sBuffer += "]";
    return sBuffer;
}

template <typename InputIter>
string TC_Common::tostr(InputIter iFirst, InputIter iLast, const string &sSep)
{
    string sBuffer;
    InputIter it = iFirst;

    while(it != iLast)
    {
        sBuffer += tostr(*it);
        ++it;

        if(it != iLast)
        {
            sBuffer += sSep;
        }
        else
        {
            break;
        }
    }

    return sBuffer;
}

}
#endif

