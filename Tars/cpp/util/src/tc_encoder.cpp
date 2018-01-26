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

#include <iconv.h>
#include <errno.h>
#include <string.h>
#include "util/tc_encoder.h"

namespace tars
{

void TC_Encoder::gbk2utf8(char *sOut, int &iMaxOutLen, const char *sIn, int iInLen)
{
    char * pIn = (char*)sIn;
    char * pEnd = pIn+iInLen;
    char * pOut = sOut;
    size_t iLeftLen;
    size_t iGbkLen;
    iconv_t cd;

    if (iInLen > iMaxOutLen)
    {
        throw TC_Encoder_Exception("[TC_Encoder::gbk2utf8] iInLen > iMaxOutLen error : ", errno);
    }

    cd = iconv_open("UTF-8","GBK");
    if (cd == (iconv_t)-1)
    {
        throw TC_Encoder_Exception("[TC_Encoder::gbk2utf8] iconv_open error : ", errno);
    }

    iLeftLen = iMaxOutLen;
    while(pIn < pEnd)
    {
        if((unsigned char)(*pIn)==0x80)
        {
            //注意GBK的0x80转换为UTF-8时为E2 82 AC
            *pOut = 0xe2; pOut++; iLeftLen--;
            *pOut = 0x82; pOut++; iLeftLen--;
            *pOut = 0xac; pOut++; iLeftLen--;
            pIn++;
        }
        else if((unsigned char)(*pIn)<0x80)
        {
            //单字节(GBK: 0x00-0x7F)
            *pOut = *pIn;
            pIn++;pOut++;iLeftLen--;
        }
        else
        {
            //双字节
            iGbkLen=2;
            int iRet=iconv(cd, &pIn, (size_t *)&iGbkLen, (char **)&pOut, (size_t *)&iLeftLen);
            if(iRet < 0)
            {
                *pOut = ' '; //转换不了替换为空格
                pIn+=2; pOut++; iLeftLen--;
            }
        }
    }

    iconv_close(cd);
    sOut[iMaxOutLen - iLeftLen] = '\0';
    iMaxOutLen = iMaxOutLen - iLeftLen;
}

string TC_Encoder::gbk2utf8(const string &sIn)
{
    iconv_t cd;

    cd = iconv_open("UTF-8","GBK");
    if (cd == (iconv_t)-1)
    {
        throw TC_Encoder_Exception("[TC_Encoder::gbk2utf8] iconv_open error", errno);
    }

    string sOut;

    for(string::size_type pos = 0; pos < sIn.length(); ++pos)
    {
        if((unsigned char)sIn[pos] == 0x80)
        {
            //注意GBK的0x80转换为UTF-8时为E2 82 AC
            sOut += 0xe2;
            sOut += 0x82;
            sOut += 0xac;
        }
        else if((unsigned char)sIn[pos] < 0x80)
        {
            //单字节(GBK: 0x00-0x7F)
            sOut += sIn[pos];
        }
        else
        {
            //双字节
            size_t sizeGbkLen = 2;
            char pIn[128] = "\0";

            strncpy(pIn, sIn.c_str() + pos, sizeGbkLen);
            char *p = pIn;

            size_t sizeLeftLen = 128;
            char pOut[128] = "\0";
            char *o = pOut;
            int iRet = iconv(cd, &p, &sizeGbkLen, (char **)&o, &sizeLeftLen);
            if(iRet < 0)
            {
                //转换不了, 暂时替换为空格
                sOut += ' ';
            }
            else
            {
                sOut += pOut;
            }

            ++pos;

        }
    }

    iconv_close(cd);
    return sOut;
}

void TC_Encoder::gbk2utf8(const string &sIn, vector<string> &vtStr)
{
    iconv_t cd;

    cd = iconv_open("UTF-8","GBK");
    if (cd == (iconv_t)-1)
    {
        throw TC_Encoder_Exception("[TC_Encoder::gbk2utf8] iconv_open error", errno);
    }

    vtStr.clear();

    for(string::size_type pos = 0; pos < sIn.length(); ++pos)
    {
        string sOut;

        if((unsigned char)sIn[pos] == 0x80)
        {
            //注意GBK的0x80转换为UTF-8时为E2 82 AC
            sOut += 0xe2;
            sOut += 0x82;
            sOut += 0xac;
        }
        else if((unsigned char)sIn[pos] < 0x80)
        {
            //单字节(GBK: 0x00-0x7F)
            sOut += sIn[pos];
        }
        else
        {
            //双字节
            size_t iGbkLen = 2;
            char pIn[128] = "\0";

            strncpy(pIn, sIn.c_str() + pos, iGbkLen);
            char *p = pIn;

            size_t iLeftLen = 128;
            char pOut[128] = "\0";
            char *o = pOut;
            int iRet = iconv(cd, &p, (size_t *)&iGbkLen, (char **)&o, (size_t *)&iLeftLen);
            if(iRet < 0)
            {
                //转换不了, 暂时替换为空格
                sOut += ' ';
            }
            else
            {
                sOut += pOut;
            }

            ++pos;

        }

        vtStr.push_back(sOut);
    }

    iconv_close(cd);
}

void TC_Encoder::utf82gbk(char *sOut, int &iMaxOutLen, const char *sIn, int iInLen)
{
    iconv_t cd;

    cd = iconv_open("GBK","UTF-8");
    if (cd == (iconv_t)-1)
    {
        throw TC_Encoder_Exception("[TC_Encoder::utf82gbk] iconv_open error", errno);
    }

    char * pIn = (char*)sIn;
    size_t sizeLeftLen     = iMaxOutLen;
    size_t sizeInLen    = iInLen;
    char* pOut = sOut;

    size_t ret = iconv(cd, &pIn, &sizeInLen, (char **)&sOut, &sizeLeftLen);
    if (ret == (size_t) - 1)
    {
        iMaxOutLen = 0;
        iconv_close(cd);
        throw TC_Encoder_Exception("[TC_Encoder::utf82gbk] iconv error", errno);
        return;
    }

    iconv_close(cd);

    pOut[iMaxOutLen - (int)sizeLeftLen] = '\0';

    iMaxOutLen = iMaxOutLen - (int)sizeLeftLen;
}

string TC_Encoder::utf82gbk(const string &sIn)
{
    if(sIn.length() == 0)
    {
        return "";
    }

    string sOut;

    int iLen = sIn.length() * 2 + 1;
    char *pOut = new char[iLen];

    try
    {
        utf82gbk(pOut, iLen, sIn.c_str(), sIn.length());
    }
    catch (TC_Encoder_Exception& e)
    {
        delete[] pOut;

        throw e;
    }

    sOut.assign(pOut, iLen);

    delete[] pOut;

    return sOut;
}

/**
 * \n -> \r\0
 * \r -> \r\r
 */
string TC_Encoder::transTo(const string& str, char f /*='\n'*/, char t /*= '\r'*/, char u /*= '\0'*/)
{
    string ret = str;

    for (size_t i = 0; i < ret.length(); ++i)
    {
        if (ret[i] == f)
        {
            ret[i] = t;

            ret.insert(++i, 1, u);
        }
        else if (ret[i] == t)
        {
            ret.insert(++i, 1, t);
        }
    }
    return ret;
}

/**
 * \r\0 -> \n
 * \r\r -> \r
 */
string TC_Encoder::transFrom(const string& str, char f /*= '\n'*/, char t /*= '\r'*/, char u /*= '\0'*/)
{
    string ret = "";

    for (string::const_iterator it = str.begin()
        ; it != str.end()
        ; ++it)
    {
        ret.append(1, *it);

        if (*it == t)
        {
            if (*(++it) == u)
            {
                *ret.rbegin() = f;
            }
        }
    }
    return ret;
}

}

