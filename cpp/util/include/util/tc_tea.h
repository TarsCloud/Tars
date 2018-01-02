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
 * @brief tea�ӽ�����(�޸���c�汾) . 
 *  
 */
/////////////////////////////////////////////////
           
	
	
/**
 * @brief �����쳣��
 */	           
struct TC_Tea_Exception : public TC_Exception
{
    TC_Tea_Exception(const string &buffer) : TC_Exception(buffer){};
    ~TC_Tea_Exception() throw(){};
};

/**
 * @brief tea�㷨��ͨ���õڶ����㷨 
 */
class TC_Tea
{
public:
    /**
     * @brief ��һ������.
     *
     * @param key      ���ܵ�key, 16���ֽ� 
     * @param sIn      ����buffer 
	 * @param iLength  ����buffer���� 
     * @return         vector<char>, ���ܺ�����ƴ�
     */
    static vector<char> encrypt(const char *key, const char *sIn, size_t iLength);

	/**
	 * @brief ��һ������.
     *   
     * @param key       ���ܵ�key, 16���ֽ� 
     * @param sIn       ��Ҫ���ܵ�buffer 
     * @param iLength   buffer���� 
	 * @throw           TC_Tea_Exception, ����ʧ�ܻ��׳��쳣 
     * @return          vector<char>, ���ܺ�����
	 */
	static vector<char> decrypt(const char *key, const char *sIn, size_t iLength);

    /**
     * @brief �ڶ�������.
     *
     * @param key       ���ܵ�key, 16���ֽ� 
     * @param sIn       ����buffer 
	 * @param iLength   ����buffer���� 
     * @return          vector<char>, ���ܺ�����ƴ�
     */
    static vector<char> encrypt2(const char *key, const char *sIn, size_t iLength);

	/**
	 * @brief �ڶ�������.
     *   
     * @param key      ���ܵ�key, 16���ֽ� 
     * @param sIn      ��Ҫ���ܵ�buffer 
     * @param iLength  buffer���� 
	 * @throw          TC_Tea_Exception, ����ʧ�ܻ��׳��쳣 
     * @return         vector<char>, ���ܺ�����
	 */
	static vector<char> decrypt2(const char *key, const char *sIn, size_t iLength);

protected:
    /**
     * @brief ��������һ����.
     * 
     * @param pInBuf   8���ֽ�  
     * @param pKey     16���ֽڵ�key
     * @param pOutBuf  ���8���ֽڵĽ��
     */
    static void TeaEncryptECB(const char *pInBuf, const char *pKey, char *pOutBuf);

    /**
     * @brief ��������һ����.
     * 
     * @param pInBuf   8���ֽ�  
     * @param pKey     16���ֽڵ�key
     * @param pOutBuf  ���8���ֽڵĽ��
     */
    static void TeaDecryptECB(const char *pInBuf, const char *pKey, char *pOutBuf);

    /**
     * @brief ����.
     * 
     * @param pInBuf       ���ܵ����Ĳ���(Body)
     * @param nInBufLen    ����
     * @param pKey         key �����ǿɼ��ַ�
	 * @param pOutBuf      ���ܺ�����Ĳ��� 
     * @param pOutBufLen   ���ܺ�ĳ���
     */
    static void oi_symmetry_encrypt(const char* pInBuf, size_t nInBufLen, const char* pKey, char* pOutBuf, size_t *pOutBufLen);

    /**
     * @brief ����.
     * 
     * @param pInBuf      ���ܵ����Ĳ���(Body) 
     * @param nInBufLen   ����
     * @param pKey        key �����ǿɼ��ַ�
     * @param pOutBuf     ���ܺ�����Ĳ���
     * @param pOutBufLen  ���ܺ�ĳ���
     * @return bool       ���ܳɹ�����true�����򷵻�false
     */
    static bool oi_symmetry_decrypt(const char* pInBuf, size_t nInBufLen, const char* pKey, char* pOutBuf, size_t *pOutBufLen);

    /**
     * @brief ���ݼ�������buffer����, ��������������(8��������).
     * 
     * @param len   buffer����
     * @return      size_t �������������
     */
    static size_t oi_symmetry_encrypt2_len(size_t len);

    /**
     * @brief ����.
     * 
     * @param pInBuf      ���ܵ����Ĳ���(Body)
     * @param nInBufLen   ����
     * @param pKey        key �����ǿɼ��ַ�
     * @param pOutBuf     ���ܺ�����Ĳ���
     * @param pOutBufLen  ���ܺ�ĳ���
     */
    static void oi_symmetry_encrypt2(const char* pInBuf, size_t nInBufLen, const char* pKey, char* pOutBuf, size_t *pOutBufLen);

    /**
     * @brief ����.
     * 
     * @param pInBuf        ���ܵ����Ĳ���(Body) 
     * @param nInBufLen     ����
     * @param pKey          key �����ǿɼ��ַ�
     * @param pOutBuf       ���ܺ�����Ĳ���
     * @param pOutBufLen    ���ܺ�ĳ��� 
     * @return              bool ���ɹ����ܷ���true�����򷵻�false
     */
    static bool oi_symmetry_decrypt2(const char* pInBuf, size_t nInBufLen, const char* pKey, char* pOutBuf, size_t *pOutBufLen);
};

}

#endif

