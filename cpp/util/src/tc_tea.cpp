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

#include "util/tc_tea.h"
#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>

namespace tars
{

vector<char> TC_Tea::encrypt(const char *key, const char *sIn, size_t iLength)
{
    size_t outlen = 16 + 2*iLength;

    vector<char> v;

    v.resize(outlen);

    oi_symmetry_encrypt(sIn, iLength, key, &v[0], &outlen);

    v.resize(outlen);

    return v;
}

vector<char> TC_Tea::decrypt(const char *key, const char *sIn, size_t iLength)
{
    size_t outlen = 2*iLength;

    vector<char> v;

    v.resize(outlen);

    if (!oi_symmetry_decrypt(sIn, iLength, key, &v[0], &outlen))
    {
        throw TC_Tea_Exception("[TC_Tea::decrypt] decrypt error.");
    }

    v.resize(outlen);

    return v;
}

vector<char> TC_Tea::encrypt2(const char *key, const char *sIn, size_t iLength)
{
    size_t outlen = oi_symmetry_encrypt2_len(iLength);

    vector<char> v;

    v.resize(outlen);

    oi_symmetry_encrypt2(sIn, iLength, key, &v[0], &outlen);

    v.resize(outlen);

    return v;
}

vector<char> TC_Tea::decrypt2(const char *key, const char *sIn, size_t iLength)
{
    size_t outlen = 2*iLength;

    vector<char> v;

    v.resize(outlen);

    if (!oi_symmetry_decrypt2(sIn, iLength, key, &v[0], &outlen))
    {
        throw TC_Tea_Exception("[TC_Tea::decrypt] decrypt error.");
    }

    v.resize(outlen);

    return v;
}

//////////////////////////////////////////////////////////////////////////////////////////
const uint32_t DELTA = 0x9e3779b9;

#define SALT_LEN 2

#define ZERO_LEN 7

#define ROUNDS 16

#define LOG_ROUNDS 4

void TC_Tea::TeaEncryptECB(const char *pInBuf, const char *pKey, char *pOutBuf)
{
	uint32_t y, z;
	uint32_t sum;
	uint32_t k[4];
	int i;

	/*plain-text is TCP/IP-endian;*/

	/*GetBlockBigEndian(in, y, z);*/
	y = ntohl(*((uint32_t*)pInBuf));
	z = ntohl(*((uint32_t*)(pInBuf+4)));
	/*TCP/IP network byte order (which is big-endian).*/

	for ( i = 0; i<4; i++)
	{
		/*now key is TCP/IP-endian;*/
		k[i] = ntohl(*((uint32_t*)(pKey+i*4)));
	}

	sum = 0;
	for (i=0; i<ROUNDS; i++)
	{
		sum += DELTA;
		y += (z << 4) + (k[0] ^ z) + (sum ^ (z >> 5)) + k[1];
		z += (y << 4) + (k[2] ^ y) + (sum ^ (y >> 5)) + k[3];
	}

	*((uint32_t*)pOutBuf) = htonl(y);
	*((uint32_t*)(pOutBuf+4)) = htonl(z);

	/*now encrypted buf is TCP/IP-endian;*/
}

void TC_Tea::TeaDecryptECB(const char *pInBuf, const char *pKey, char *pOutBuf)
{
	uint32_t y, z, sum;
	uint32_t k[4];
	int i;

	/*now encrypted buf is TCP/IP-endian;*/
	/*TCP/IP network byte order (which is big-endian).*/
	y = ntohl(*((uint32_t*)pInBuf));
	z = ntohl(*((uint32_t*)(pInBuf+4)));

	for ( i=0; i<4; i++)
	{
		/*key is TCP/IP-endian;*/
		k[i] = ntohl(*((uint32_t*)(pKey+i*4)));
	}

	sum = DELTA << LOG_ROUNDS;
	for (i=0; i<ROUNDS; i++)
	{
		z -= (y << 4) + (k[2] ^ y) + (sum ^ (y >> 5)) + k[3];
		y -= (z << 4) + (k[0] ^ z) + (sum ^ (z >> 5)) + k[1];
		sum -= DELTA;
	}

	*((uint32_t*)pOutBuf) = htonl(y);
	*((uint32_t*)(pOutBuf+4)) = htonl(z);

	/*now plain-text is TCP/IP-endian;*/
}

//////////////////////////////////////////////////////////////////////////////////////
///

/*
	输入:pInBuf为需加密的明文部分(Body),nInBufLen为pInBuf长度;
	输出:pOutBuf为密文格式,pOutBufLen为pOutBuf的长度是8byte的倍数;
*/
/*TEA加密算法,CBC模式*/
/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
void TC_Tea::oi_symmetry_encrypt(const char* pInBuf, size_t nInBufLen, const char* pKey, char* pOutBuf, size_t *pOutBufLen)
{
	size_t nPadSaltBodyZeroLen/*PadLen(1byte)+Salt+Body+Zero的长度*/;
	size_t nPadlen;
	char src_buf[8], zero_iv[8], *iv_buf;
	size_t src_i, i, j;

	/*根据Body长度计算PadLen,最小必需长度必需为8byte的整数倍*/
	nPadSaltBodyZeroLen = nInBufLen/*Body长度*/+1+SALT_LEN+ZERO_LEN/*PadLen(1byte)+Salt(2byte)+Zero(7byte)*/;
	if((nPadlen=nPadSaltBodyZeroLen%8)) /*len=nSaltBodyZeroLen%8*/
	{
		/*模8余0需补0,余1补7,余2补6,...,余7补1*/
		nPadlen=8-nPadlen;
	}

	/*srand( (unsigned)time( NULL ) ); 初始化随机数*/
	/*加密第一块数据(8byte),取前面10byte*/
	src_buf[0] = (((char)rand()) & 0x0f8)/*最低三位存PadLen,清零*/ | (char)nPadlen;
	src_i = 1; /*src_i指向src_buf下一个位置*/

	while(nPadlen--)
		src_buf[src_i++]=(char)rand(); /*Padding*/

	/*come here, i must <= 8*/

	memset(zero_iv, 0, 8);
	iv_buf = zero_iv; /*make iv*/

	*pOutBufLen = 0; /*init OutBufLen*/

	for (i=1;i<=SALT_LEN;) /*Salt(2byte)*/
	{
		if (src_i<8)
		{
			src_buf[src_i++]=(char)rand();
			i++; /*i inc in here*/
		}

		if (src_i==8)
		{
			/*src_i==8*/

			for (j=0;j<8;j++) /*CBC XOR*/
				src_buf[j]^=iv_buf[j];
			/*pOutBuffer、pInBuffer均为8byte, pKey为16byte*/
			TeaEncryptECB(src_buf, pKey, pOutBuf);
			src_i=0;
			iv_buf=pOutBuf;
			*pOutBufLen+=8;
			pOutBuf+=8;
		}
	}

	/*src_i指向src_buf下一个位置*/

	while(nInBufLen)
	{
		if (src_i<8)
		{
			src_buf[src_i++]=*(pInBuf++);
			nInBufLen--;
		}

		if (src_i==8)
		{
			/*src_i==8*/

			for (i=0;i<8;i++) /*CBC XOR*/
				src_buf[i]^=iv_buf[i];
			/*pOutBuffer、pInBuffer均为8byte, pKey为16byte*/
			TeaEncryptECB(src_buf, pKey, pOutBuf);
			src_i=0;
			iv_buf=pOutBuf;
			*pOutBufLen+=8;
			pOutBuf+=8;
		}
	}

	/*src_i指向src_buf下一个位置*/

	for (i=1;i<=ZERO_LEN;)
	{
		if (src_i<8)
		{
			src_buf[src_i++]=0;
			i++; /*i inc in here*/
		}

		if (src_i==8)
		{
			/*src_i==8*/

			for (j=0;j<8;j++) /*CBC XOR*/
				src_buf[j]^=iv_buf[j];
			/*pOutBuffer、pInBuffer均为8byte, pKey为16byte*/
			TeaEncryptECB(src_buf, pKey, pOutBuf);
			src_i=0;
			iv_buf=pOutBuf;
			*pOutBufLen+=8;
			pOutBuf+=8;
		}
	}
}

/*
	输入:pInBuf为密文格式,nInBufLen为pInBuf的长度是8byte的倍数;
	输出:pOutBuf为明文(Body),pOutBufLen为pOutBuf的长度;
	返回值:如果格式正确返回TRUE;
*/
/*TEA解密算法,CBC模式*/
/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
bool TC_Tea::oi_symmetry_decrypt(const char* pInBuf, size_t nInBufLen, const char* pKey, char* pOutBuf, size_t *pOutBufLen)
{
	size_t nPadLen, nPlainLen;
	char dest_buf[8];
	const char *iv_buf;
	size_t dest_i, i, j;

	if ((nInBufLen%8) || (nInBufLen<16)) return false;

	TeaDecryptECB(pInBuf, pKey, dest_buf);

	nPadLen = dest_buf[0] & 0x7/*只要最低三位*/;

	/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
	i = nInBufLen-1/*PadLen(1byte)*/-nPadLen-SALT_LEN-ZERO_LEN; /*明文长度*/
	if (*pOutBufLen<i) return false;
	*pOutBufLen = i;
	//here it should never success
	//if (*pOutBufLen < 0) return false;

	iv_buf = pInBuf; /*init iv*/
	nInBufLen -= 8;
	pInBuf += 8;

	dest_i=1; /*dest_i指向dest_buf下一个位置*/

	/*把Padding滤掉*/
	dest_i+=nPadLen;

	/*dest_i must <=8*/

	/*把Salt滤掉*/
	for (i=1; i<=SALT_LEN;)
	{
		if (dest_i<8)
		{
			dest_i++;
			i++;
		}

		if (dest_i==8)
		{
			/*dest_i==8*/
			TeaDecryptECB(pInBuf, pKey, dest_buf);
			for (j=0; j<8; j++)
				dest_buf[j]^=iv_buf[j];

			iv_buf = pInBuf;
			nInBufLen -= 8;
			pInBuf += 8;

			dest_i=0; /*dest_i指向dest_buf下一个位置*/
		}
	}

	/*还原明文*/

	nPlainLen=*pOutBufLen;
	while (nPlainLen)
	{
		if (dest_i<8)
		{
			*(pOutBuf++)=dest_buf[dest_i++];
			nPlainLen--;
		}
		else if (dest_i==8)
		{
			/*dest_i==8*/
			TeaDecryptECB(pInBuf, pKey, dest_buf);
			for (i=0; i<8; i++)
				dest_buf[i]^=iv_buf[i];

			iv_buf = pInBuf;
			nInBufLen -= 8;
			pInBuf += 8;

			dest_i=0; /*dest_i指向dest_buf下一个位置*/
		}
	}

	/*校验Zero*/
	for (i=1;i<=ZERO_LEN;)
	{
		if (dest_i<8)
		{
			if(dest_buf[dest_i++]) return false;
			i++;
		}
		else if (dest_i==8)
		{
			/*dest_i==8*/
			TeaDecryptECB(pInBuf, pKey, dest_buf);
			for (j=0; j<8; j++)
				dest_buf[j]^=iv_buf[j];

			iv_buf = pInBuf;
			nInBufLen -= 8;
			pInBuf += 8;

			dest_i=0; /*dest_i指向dest_buf下一个位置*/
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////

/*TEA加密算法,CBC模式*/
/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
size_t TC_Tea::oi_symmetry_encrypt2_len(size_t nInBufLen)
{
	size_t nPadSaltBodyZeroLen/*PadLen(1byte)+Salt+Body+Zero的长度*/;
	size_t nPadlen;

	/*根据Body长度计算PadLen,最小必需长度必需为8byte的整数倍*/
	nPadSaltBodyZeroLen = nInBufLen/*Body长度*/+1+SALT_LEN+ZERO_LEN/*PadLen(1byte)+Salt(2byte)+Zero(7byte)*/;
	if((nPadlen=nPadSaltBodyZeroLen%8)) /*len=nSaltBodyZeroLen%8*/
	{
		/*模8余0需补0,余1补7,余2补6,...,余7补1*/
		nPadlen=8-nPadlen;
	}

	return nPadSaltBodyZeroLen+nPadlen;
}

/*pKey为16byte*/
/*
	输入:pInBuf为需加密的明文部分(Body),nInBufLen为pInBuf长度;
	输出:pOutBuf为密文格式,pOutBufLen为pOutBuf的长度是8byte的倍数;
*/
/*TEA加密算法,CBC模式*/
/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
void TC_Tea::oi_symmetry_encrypt2(const char* pInBuf, size_t nInBufLen, const char* pKey, char* pOutBuf, size_t *pOutBufLen)
{
	size_t nPadSaltBodyZeroLen/*PadLen(1byte)+Salt+Body+Zero的长度*/;
	size_t nPadlen;
	char src_buf[8], iv_plain[8], *iv_crypt;
	size_t src_i, i, j;

	/*根据Body长度计算PadLen,最小必需长度必需为8byte的整数倍*/
	nPadSaltBodyZeroLen = nInBufLen/*Body长度*/+1+SALT_LEN+ZERO_LEN/*PadLen(1byte)+Salt(2byte)+Zero(7byte)*/;
	if((nPadlen=nPadSaltBodyZeroLen%8)) /*len=nSaltBodyZeroLen%8*/
	{
		/*模8余0需补0,余1补7,余2补6,...,余7补1*/
		nPadlen=8-nPadlen;
	}

	/*srand( (unsigned)time( NULL ) ); 初始化随机数*/
	/*加密第一块数据(8byte),取前面10byte*/
//	src_buf[0] = ((char)rand()) & 0x0f8/*最低三位存PadLen,清零*/ | (char)nPadlen;
	src_buf[0] = (((char)rand()) & 0x0f8) | (char)nPadlen;
	src_i = 1; /*src_i指向src_buf下一个位置*/

	while(nPadlen--)
		src_buf[src_i++]=(char)rand(); /*Padding*/

	/*come here, src_i must <= 8*/

	for ( i=0; i<8; i++)
		iv_plain[i] = 0;
	iv_crypt = iv_plain; /*make zero iv*/

	*pOutBufLen = 0; /*init OutBufLen*/

	for (i=1;i<=SALT_LEN;) /*Salt(2byte)*/
	{
		if (src_i<8)
		{
			src_buf[src_i++]=(char)rand();
			i++; /*i inc in here*/
		}

		if (src_i==8)
		{
			/*src_i==8*/

			for (j=0;j<8;j++) /*加密前异或前8个byte的密文(iv_crypt指向的)*/
				src_buf[j]^=iv_crypt[j];

			/*pOutBuffer、pInBuffer均为8byte, pKey为16byte*/
			/*加密*/
			TeaEncryptECB(src_buf, pKey, pOutBuf);

			for (j=0;j<8;j++) /*加密后异或前8个byte的明文(iv_plain指向的)*/
				pOutBuf[j]^=iv_plain[j];

			/*保存当前的iv_plain*/
			for (j=0;j<8;j++)
				iv_plain[j]=src_buf[j];

			/*更新iv_crypt*/
			src_i=0;
			iv_crypt=pOutBuf;
			*pOutBufLen+=8;
			pOutBuf+=8;
		}
	}

	/*src_i指向src_buf下一个位置*/

	while(nInBufLen)
	{
		if (src_i<8)
		{
			src_buf[src_i++]=*(pInBuf++);
			nInBufLen--;
		}

		if (src_i==8)
		{
			/*src_i==8*/

			for (j=0;j<8;j++) /*加密前异或前8个byte的密文(iv_crypt指向的)*/
				src_buf[j]^=iv_crypt[j];
			/*pOutBuffer、pInBuffer均为8byte, pKey为16byte*/
			TeaEncryptECB(src_buf, pKey, pOutBuf);

			for (j=0;j<8;j++) /*加密后异或前8个byte的明文(iv_plain指向的)*/
				pOutBuf[j]^=iv_plain[j];

			/*保存当前的iv_plain*/
			for (j=0;j<8;j++)
				iv_plain[j]=src_buf[j];

			src_i=0;
			iv_crypt=pOutBuf;
			*pOutBufLen+=8;
			pOutBuf+=8;
		}
	}

	/*src_i指向src_buf下一个位置*/

	for (i=1;i<=ZERO_LEN;)
	{
		if (src_i<8)
		{
			src_buf[src_i++]=0;
			i++; /*i inc in here*/
		}

		if (src_i==8)
		{
			/*src_i==8*/

			for (j=0;j<8;j++) /*加密前异或前8个byte的密文(iv_crypt指向的)*/
				src_buf[j]^=iv_crypt[j];
			/*pOutBuffer、pInBuffer均为8byte, pKey为16byte*/
			TeaEncryptECB(src_buf, pKey, pOutBuf);

			for (j=0;j<8;j++) /*加密后异或前8个byte的明文(iv_plain指向的)*/
				pOutBuf[j]^=iv_plain[j];

			/*保存当前的iv_plain*/
			for (j=0;j<8;j++)
				iv_plain[j]=src_buf[j];

			src_i=0;
			iv_crypt=pOutBuf;
			*pOutBufLen+=8;
			pOutBuf+=8;
		}
	}
}

/*
	输入:pInBuf为密文格式,nInBufLen为pInBuf的长度是8byte的倍数; *pOutBufLen为接收缓冲区的长度
		特别注意*pOutBufLen应预置接收缓冲区的长度!
	输出:pOutBuf为明文(Body),pOutBufLen为pOutBuf的长度,至少应预留nInBufLen-10;
	返回值:如果格式正确返回TRUE;
*/
/*TEA解密算法,CBC模式*/
/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
bool TC_Tea::oi_symmetry_decrypt2(const char* pInBuf, size_t nInBufLen, const char* pKey, char* pOutBuf, size_t *pOutBufLen)
{
	size_t nPadLen, nPlainLen;
	char dest_buf[8], zero_buf[8];
	const char *iv_pre_crypt, *iv_cur_crypt;
	size_t dest_i, i, j;
	//const char *pInBufBoundary;
	size_t nBufPos;
	nBufPos = 0;

	if ((nInBufLen%8) || (nInBufLen<16)) return false;

	TeaDecryptECB(pInBuf, pKey, dest_buf);

	nPadLen = dest_buf[0] & 0x7/*只要最低三位*/;

	/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
	i = nInBufLen-1/*PadLen(1byte)*/-nPadLen-SALT_LEN-ZERO_LEN; /*明文长度*/

	if ((*pOutBufLen<i)) return false;

	*pOutBufLen = i;

	//pInBufBoundary = pInBuf + nInBufLen; /*输入缓冲区的边界，下面不能pInBuf>=pInBufBoundary*/

	for ( i=0; i<8; i++)
		zero_buf[i] = 0;

	iv_pre_crypt = zero_buf;
	iv_cur_crypt = pInBuf; /*init iv*/

	pInBuf += 8;
	nBufPos += 8;

	dest_i=1; /*dest_i指向dest_buf下一个位置*/

	/*把Padding滤掉*/
	dest_i+=nPadLen;

	/*dest_i must <=8*/

	/*把Salt滤掉*/
	for (i=1; i<=SALT_LEN;)
	{
		if (dest_i<8)
		{
			dest_i++;
			i++;
		}
		else if (dest_i==8)
		{
			/*解开一个新的加密块*/

			/*改变前一个加密块的指针*/
			iv_pre_crypt = iv_cur_crypt;
			iv_cur_crypt = pInBuf;

			/*异或前一块明文(在dest_buf[]中)*/
			for (j=0; j<8; j++)
			{
				if( (nBufPos + j) >= nInBufLen)
				{
					return false;
				}
					
				dest_buf[j]^=pInBuf[j];
			}

			/*dest_i==8*/
			TeaDecryptECB(dest_buf, pKey, dest_buf);

			/*在取出的时候才异或前一块密文(iv_pre_crypt)*/


			pInBuf += 8;
			nBufPos += 8;

			dest_i=0; /*dest_i指向dest_buf下一个位置*/
		}
	}

	/*还原明文*/

	nPlainLen=*pOutBufLen;
	while (nPlainLen)
	{
		if (dest_i<8)
		{
			*(pOutBuf++)=dest_buf[dest_i]^iv_pre_crypt[dest_i];
			dest_i++;
			nPlainLen--;
		}
		else if (dest_i==8)
		{
			/*dest_i==8*/

			/*改变前一个加密块的指针*/
			iv_pre_crypt = iv_cur_crypt;
			iv_cur_crypt = pInBuf;

			/*解开一个新的加密块*/

			/*异或前一块明文(在dest_buf[]中)*/
			for (j=0; j<8; j++)
			{
				if( (nBufPos + j) >= nInBufLen)
				{
					return false;
				}
					
				dest_buf[j]^=pInBuf[j];
			}

			TeaDecryptECB(dest_buf, pKey, dest_buf);

			/*在取出的时候才异或前一块密文(iv_pre_crypt)*/


			pInBuf += 8;
			nBufPos += 8;

			dest_i=0; /*dest_i指向dest_buf下一个位置*/
		}
	}

	/*校验Zero*/
	for (i=1;i<=ZERO_LEN;)
	{
		if (dest_i<8)
		{
			if(dest_buf[dest_i]^iv_pre_crypt[dest_i]) return false;
			dest_i++;
			i++;
		}
		else if (dest_i==8)
		{
			/*改变前一个加密块的指针*/
			iv_pre_crypt = iv_cur_crypt;
			iv_cur_crypt = pInBuf;

			/*解开一个新的加密块*/

			/*异或前一块明文(在dest_buf[]中)*/
			for (j=0; j<8; j++)
			{
				if( (nBufPos + j) >= nInBufLen)
				{
					return false;
				}
					
				dest_buf[j]^=pInBuf[j];
			}

			TeaDecryptECB(dest_buf, pKey, dest_buf);

			/*在取出的时候才异或前一块密文(iv_pre_crypt)*/

			pInBuf += 8;
			nBufPos += 8;
			dest_i=0; /*dest_i指向dest_buf下一个位置*/
		}
	}

	return true;
}

}

