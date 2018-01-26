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

#include "util/tc_base64.h"

namespace tars
{
// Base64编码表：将输入数据流每次取6 bit，用此6 bit的值(0-63)作为索引去查表，输出相应字符。这样，每3个字节将编码为4个字符(3×8 → 4×6)；不满4个字符的以'='填充。
const char  TC_Base64::EnBase64Tab[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
// Base64解码表：将64个可打印字符的值作为索引，查表得到的值（范围为0-63）依次连起来得到解码结果。
// 解码表size为256，非法字符将被解码为ASCII 0
const char  TC_Base64::DeBase64Tab[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    62,        // '+'
    0, 0, 0,
    63,        // '/'
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61,        // '0'-'9'
    0, 0, 0, 0, 0, 0, 0,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,        // 'A'-'Z'
    0, 0, 0, 0, 0, 0,
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
    39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,        // 'a'-'z'
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

string TC_Base64::encode(const string &data, bool bChangeLine/* = false*/)
{
    if(data.empty())
        return "";    
    //设原始串长度为a,结果串中算上回车换行及'/0',最终长度为(a/3+1)*4+(a/3+1)*4*2/76+1,约为1.369*a+6
    char *pDst = NULL;
    int iBufSize = (int)(data.size()*1.4) + 6;
    pDst = new char[iBufSize];
    if(pDst == NULL)
        return "";   
    int iDstLen = encode((unsigned char*)data.c_str(), data.size(), pDst, bChangeLine);
    string ret(pDst,iDstLen);
    delete [] pDst;
    return ret;
}

string TC_Base64::decode(const string &data)
{
    if(data.empty())
        return "";
    unsigned char *pDst = NULL;  
    pDst = new unsigned char[data.size()];
    if(pDst == NULL)
        return "";
    int iDstLen = decode(data.c_str(), data.size(), pDst);
    string ret((char*)pDst,iDstLen);
    delete [] pDst;
    return ret;
}

int TC_Base64::encode(const unsigned char* pSrc, int nSrcLen, char* pDst, bool bChangeLine/* = false*/)
{
    unsigned char c1, c2, c3;   
    int nDstLen = 0;             
    int nLineLen = 0;         
    int nDiv = nSrcLen / 3;      
    int nMod = nSrcLen % 3;    
    // 每次取3个字节，编码成4个字符
    for (int i = 0; i < nDiv; i ++)
    {
        c1 = *pSrc++;
        c2 = *pSrc++;
        c3 = *pSrc++;
 
        *pDst++ = EnBase64Tab[c1 >> 2];
        *pDst++ = EnBase64Tab[((c1 << 4) | (c2 >> 4)) & 0x3f];
        *pDst++ = EnBase64Tab[((c2 << 2) | (c3 >> 6)) & 0x3f];
        *pDst++ = EnBase64Tab[c3 & 0x3f];
        nLineLen += 4;
        nDstLen += 4;
        // 相关RFC中每行超过76字符时需要添加回车换行
        if (bChangeLine && nLineLen > 72)
        {
            *pDst++ = '\r';
            *pDst++ = '\n';
            nLineLen = 0;
            nDstLen += 2;
        }
    }
    // 编码余下的字节
    if (nMod == 1)
    {
        c1 = *pSrc++;
        *pDst++ = EnBase64Tab[(c1 & 0xfc) >> 2];
        *pDst++ = EnBase64Tab[((c1 & 0x03) << 4)];
        *pDst++ = '=';
        *pDst++ = '=';
        nLineLen += 4;
        nDstLen += 4;
    }
    else if (nMod == 2)
    {
        c1 = *pSrc++;
        c2 = *pSrc++;
        *pDst++ = EnBase64Tab[(c1 & 0xfc) >> 2];
        *pDst++ = EnBase64Tab[((c1 & 0x03) << 4) | ((c2 & 0xf0) >> 4)];
        *pDst++ = EnBase64Tab[((c2 & 0x0f) << 2)];
        *pDst++ = '=';
        nDstLen += 4;
    }
    // 输出加个结束符
    *pDst = '\0';
 
    return nDstLen;
}

 
int  TC_Base64::decode(const char* pSrc, int nSrcLen, unsigned char* pDst)
{
    int nDstLen;            // 输出的字符计数 
    int nValue;             // 解码用到的整数
    int i; 
    i = 0;
    nDstLen = 0;
 
    // 取4个字符，解码到一个长整数，再经过移位得到3个字节
    while (i < nSrcLen)
    {    
        // 跳过回车换行    
        if (*pSrc != '\r' && *pSrc!='\n')
        {
            if(i + 4 > nSrcLen)                             //待解码字符串不合法，立即停止解码返回
                break;           

            nValue = DeBase64Tab[int(*pSrc++)] << 18;         
            nValue += DeBase64Tab[int(*pSrc++)] << 12;
            *pDst++ = (nValue & 0x00ff0000) >> 16;
            nDstLen++; 
            if (*pSrc != '=')
            {                
                nValue += DeBase64Tab[int(*pSrc++)] << 6;
                *pDst++ = (nValue & 0x0000ff00) >> 8;
                nDstLen++; 
                if (*pSrc != '=')
                {                    
                    nValue += DeBase64Tab[int(*pSrc++)];
                    *pDst++ =nValue & 0x000000ff;
                    nDstLen++;
                }
            }
 
            i += 4;
        }
        else       
        {
            pSrc++;
            i++;
        }
     }
    // 输出加个结束符
    *pDst = '\0'; 
    return nDstLen;
}

}
