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

#ifndef _TARS_TC_TEA_H_
#define _TARS_TC_TEA_H_

#include <vector>
#include <string>
#include <string.h>
#include <stdexcept>

#include "util/tc_ex.h"

using namespace std;

namespace tars
{
/////////////////////////////////////////////////
/**
 * @file tc_tea.h 
 * @brief tea加解密类(修改至c版本) . 
 *  
 */
/////////////////////////////////////////////////
           
	
	
/**
 * @brief 加密异常类
 */	           
struct TC_Tea_Exception : public TC_Exception
{
    TC_Tea_Exception(const string &buffer) : TC_Exception(buffer){};
    ~TC_Tea_Exception() throw(){};
};

/**
 * @brief tea算法，通常用第二代算法 
 */
class TC_Tea
{
public:
    /**
     * @brief 第一代加密.
     *
     * @param key      加密的key, 16个字节 
     * @param sIn      输入buffer 
	 * @param iLength  输入buffer长度 
     * @return         vector<char>, 加密后二进制串
     */
    static vector<char> encrypt(const char *key, const char *sIn, size_t iLength);

	/**
	 * @brief 第一代解密.
     *   
     * @param key       解密的key, 16个字节 
     * @param sIn       需要解密的buffer 
     * @param iLength   buffer长度 
	 * @throw           TC_Tea_Exception, 解密失败会抛出异常 
     * @return          vector<char>, 解密后数据
	 */
	static vector<char> decrypt(const char *key, const char *sIn, size_t iLength);

    /**
     * @brief 第二代加密.
     *
     * @param key       加密的key, 16个字节 
     * @param sIn       输入buffer 
	 * @param iLength   输入buffer长度 
     * @return          vector<char>, 加密后二进制串
     */
    static vector<char> encrypt2(const char *key, const char *sIn, size_t iLength);

	/**
	 * @brief 第二代解密.
     *   
     * @param key      解密的key, 16个字节 
     * @param sIn      需要解密的buffer 
     * @param iLength  buffer长度 
	 * @throw          TC_Tea_Exception, 解密失败会抛出异常 
     * @return         vector<char>, 解密后数据
	 */
	static vector<char> decrypt2(const char *key, const char *sIn, size_t iLength);

protected:
    /**
     * @brief 加密运算一个块.
     * 
     * @param pInBuf   8个字节  
     * @param pKey     16个字节的key
     * @param pOutBuf  输出8个字节的结果
     */
    static void TeaEncryptECB(const char *pInBuf, const char *pKey, char *pOutBuf);

    /**
     * @brief 解密运算一个块.
     * 
     * @param pInBuf   8个字节  
     * @param pKey     16个字节的key
     * @param pOutBuf  输出8个字节的结果
     */
    static void TeaDecryptECB(const char *pInBuf, const char *pKey, char *pOutBuf);

    /**
     * @brief 加密.
     * 
     * @param pInBuf       加密的明文部分(Body)
     * @param nInBufLen    长度
     * @param pKey         key 必须是可见字符
	 * @param pOutBuf      加密后的明文部分 
     * @param pOutBufLen   加密后的长度
     */
    static void oi_symmetry_encrypt(const char* pInBuf, size_t nInBufLen, const char* pKey, char* pOutBuf, size_t *pOutBufLen);

    /**
     * @brief 解密.
     * 
     * @param pInBuf      加密的明文部分(Body) 
     * @param nInBufLen   长度
     * @param pKey        key 必须是可见字符
     * @param pOutBuf     解密后的明文部分
     * @param pOutBufLen  解密后的长度
     * @return bool       解密成功返回true，否则返回false
     */
    static bool oi_symmetry_decrypt(const char* pInBuf, size_t nInBufLen, const char* pKey, char* pOutBuf, size_t *pOutBufLen);

    /**
     * @brief 根据加密输入buffer长度, 计算加密输出长度(8的整数倍).
     * 
     * @param len   buffer长度
     * @return      size_t ，加密输出长度
     */
    static size_t oi_symmetry_encrypt2_len(size_t len);

    /**
     * @brief 加密.
     * 
     * @param pInBuf      加密的明文部分(Body)
     * @param nInBufLen   长度
     * @param pKey        key 必须是可见字符
     * @param pOutBuf     加密后的明文部分
     * @param pOutBufLen  加密后的长度
     */
    static void oi_symmetry_encrypt2(const char* pInBuf, size_t nInBufLen, const char* pKey, char* pOutBuf, size_t *pOutBufLen);

    /**
     * @brief 解密.
     * 
     * @param pInBuf        加密的明文部分(Body) 
     * @param nInBufLen     长度
     * @param pKey          key 必须是可见字符
     * @param pOutBuf       解密后的明文部分
     * @param pOutBufLen    解密后的长度 
     * @return              bool ，成功解密返回true，否则返回false
     */
    static bool oi_symmetry_decrypt2(const char* pInBuf, size_t nInBufLen, const char* pKey, char* pOutBuf, size_t *pOutBufLen);
};

}

#endif

