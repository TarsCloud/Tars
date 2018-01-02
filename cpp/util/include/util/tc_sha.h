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

#ifndef __TARS_TC_SHA_H
#define __TARS_TC_SHA_H

#include <string>
#include <vector>
#include <stdint.h>
#include "util/tc_ex.h"
using namespace std;

namespace tars
{
/////////////////////////////////////////////////
/** 
 * @file tc_sha.h
 * @brief sha各种hash算法.
 *  
 */             
/////////////////////////////////////////////////


/** 
 SHA-1   in   C 
 By   Steve   Reid   <steve@edmweb.com> 
 100%   Public   Domain 
 Test   Vectors   (from   FIPS   PUB   180-1) 
 "abc " 
 A9993E36   4706816A   BA3E2571   7850C26C   9CD0D89D 
 "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq " 
 84983E44   1C3BD26E   BAAE4AA1   F95129E5   E54670F1 
 A   million   repetitions   of   "a " 
 34AA973C   D4C4DAA4   F61EEB2B   DBAD2731   6534016F 
 */

/**
 * @brief sha异常. 
 */
struct TC_SHA_Exception : public TC_Exception
{
    TC_SHA_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_SHA_Exception() throw(){};
};

/**
 * @brief sha各种hash算法, 
 *
 */
class TC_SHA
{
public:
    /**
     * @brief sha1 hash算法.
     *  
     * @param sIn     输入buffer
     * @param iInLen  输入buffer长度
     * @return vector<char> hash的内容(20个字节)
     */
    static vector<char> sha1bin(const char *buffer, size_t length);

    /**
     * @brief sha1 hash算法.
     *  
     * @param sIn     输入buffer
     * @param iInLen  输入buffer长度
     * @return        string 加密后的内容(20*2个字符)
     */
    static string sha1str(const char *buffer, size_t length);

    /**
     * @brief 对文件计算sha1 hash算法.
     *  
     * @param fileName  文件名
     * @throw TC_SHA_Exception, 文件读取错误会抛出异常 
     * @return string   加密后的内容(20*2个字符)
     */
    static string sha1file(const string &fileName);

    /**
     * @brief sha256 hash算法.
     *  
     * @param sIn     输入buffer
     * @param iInLen  输入buffer长度
     * @return vector<char> hash的内容(32个字节)
     */
    static vector<char> sha256bin(const char *buffer, size_t length);

    /**
     * @brief sha256 hash算法.
     *  
     * @param sIn     输入buffer
     * @param iInLen  输入buffer长度
     * @return        string 加密后的内容(32*2个字符)
     */
    static string sha256str(const char *buffer, size_t length);

    /**
     * @brief 对文件计算sha256 hash算法.
     *  
     * @param fileName  文件名
     * @throw TC_SHA_Exception, 文件读取错误会抛出异常 
     * @return string   加密后的内容(32*2个字符)
     */
    static string sha256file(const string &fileName);

    /**
     * @brief sha1 hash算法.
     *  
     * @param sIn     输入buffer
     * @param iInLen  输入buffer长度
     * @return vector<char> hash的内容(48个字节)
     */
    static vector<char> sha384bin(const char *buffer, size_t length);

    /**
     * @brief sha1 hash算法.
     *  
     * @param sIn     输入buffer
     * @param iInLen  输入buffer长度
     * @return        string 加密后的内容(48*2个字符)
     */
    static string sha384str(const char *buffer, size_t length);

    /**
     * @brief 对文件计算sha384 hash算法.
     *  
     * @param fileName  文件名
     * @throw TC_SHA_Exception, 文件读取错误会抛出异常 
     * @return string   加密后的内容(48*2个字符)
     */
    static string sha384file(const string &fileName);

    /**
     * @brief sha1 hash算法.
     *  
     * @param sIn     输入buffer
     * @param iInLen  输入buffer长度
     * @return vector<char> hash的内容(64个字节)
     */
    static vector<char> sha512bin(const char *buffer, size_t length);

    /**
     * @brief sha1 hash算法.
     *  
     * @param sIn     输入buffer
     * @param iInLen  输入buffer长度
     * @return        string 加密后的内容(64*2个字符)
     */
    static string sha512str(const char *buffer, size_t length);

    /**
     * @brief 对文件计算sha512 hash算法.
     *  
     * @param fileName  文件名
     * @throw TC_SHA_Exception, 文件读取错误会抛出异常 
     * @return string   加密后的内容(64*2个字符)
     */
    static string sha512file(const string &fileName);
};

}
#endif

