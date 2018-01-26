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

// **********************************************************************
// TUP_C version 1.0.4 by WSRD Tencent.
// **********************************************************************

#ifndef __TUP_C_H__
#define __TUP_C_H__

#include "Tars_c.h"

typedef struct UniAttribute UniAttribute;
typedef struct UniPacket    UniPacket;
typedef struct UniPacket    TarsUniPacket;

//属性封装
struct UniAttribute
{
	JMapWrapper 	*_data;
	TarsOutputStream *value_os;
	TarsInputStream  *value_is;

	JMapWrapper     *m_data;  	//encode decode helper
	TarsOutputStream *os_string;	//encode decode helper

	short			_iVer;		//TUP版本的标示(2:标示普通TUP协议，3:标示精简TUP协议)

	Int32           _iDecPacketLen;//解码的包长
	char _sLastError[32];
};

///////////////////////////////////////////////////////////////
//put
void  TUP_setVersion(void * pack, Short iVersion);

Int32 TUP_putBool	(void * pack, const char * name, Bool value);

Int32 TUP_putChar	(void * pack, const char * name, Char value);

Int32 TUP_putUInt8	(void * pack, const char * name, UInt8 value);

Int32 TUP_putShort	(void * pack, const char * name, Short value);

Int32 TUP_putUInt16	(void * pack, const char * name, UInt16 value);

Int32 TUP_putFloat	(void * pack, const char * name, Float value);

Int32 TUP_putDouble	(void * pack, const char * name, Double value);

Int32 TUP_putInt32	(void * pack, const char * name, Int32  value);

Int32 TUP_putUInt32	(void * pack, const char * name, UInt32 value);

Int32 TUP_putInt64	(void * pack, const char * name, Int64  value);

Int32 TUP_putString	(void * pack, const char * name, JString * value);
Int32 TUP_putStringBuffer(void * pack, const char * name, const char* buff, uint32_t len);

Int32 TUP_putVector	(void * pack, const char * name, JArray * value);

Int32 TUP_putVectorChar(void * pack, const char * name,  JString* value);
Int32 TUP_putVectorCharBuffer(void * pack, const char * name,  const char* buff, uint32_t len);

Int32 TUP_putMap	(void * pack, const char * name, JMapWrapper * value);

Int32 TUP_putStruct	(void * pack, const char * name, const void* st);

//get
Short TUP_getVersion(const void * pack);

Int32 TUP_getBool	(const void * pack, const char * name, Bool  * value);

Int32 TUP_getChar	(const void * pack, const char * name, Char  * value);

Int32 TUP_getUInt8	(const void * pack, const char * name, UInt8 * value);

Int32 TUP_getShort 	(const void * pack, const char * name, Short * value);

Int32 TUP_getUInt16	(const void * pack, const char * name, UInt16 * value);

Int32 TUP_getFloat 	(const void * pack, const char * name, Float* value);

Int32 TUP_getDouble	(const void * pack, const char * name, Double* value);

Int32 TUP_getInt32 	(const void * pack, const char * name, Int32*  value);

Int32 TUP_getUInt32	(const void * pack, const char * name, UInt32 * value);

Int32 TUP_getInt64 (const void * pack, const char * name, Int64*  value);

Int32 TUP_getString(const void * pack, const char * name, JString * value);

Int32 TUP_getVector(const void * pack, const char * name, JArray * value);

Int32 TUP_getVectorChar(const void * pack, const char * name, JString * value);

Int32 TUP_getMap   (const void * pack, const char * name, JMapWrapper * value);

Int32 TUP_getStruct(const void * pack, const char * name, void *st);


UniAttribute * UniAttribute_new(void);

Int32 UniAttribute_init(UniAttribute *handle);

void UniAttribute_del(UniAttribute ** handle);

Int32 UniAttribute_encode(const UniAttribute * pack, char** buff, uint32_t *len);

Int32 UniAttribute_decode(UniAttribute * pack, const char* buff, uint32_t len);

Int32 UniAttribute_size(UniAttribute * pack);
///////////////////////////////////////////////////////////////
//请求回应包封装
struct UniPacket
{
	UniAttribute attr;

	Short iVersion;
	Char cPacketType;
	Int32 iMessageType;
	Int32 iRequestId;
	JString *sServantName;
	JString *sFuncName;
	JString *sBuffer;
	Int32 iTimeout;
	JMapWrapper *context;
	JMapWrapper *status;
};

void  UniPacket_setVersion(UniPacket * pack, Short siVersion);

Short UniPacket_getVersion(UniPacket * pack);

Int32 UniPacket_encode(const UniPacket * pack, char** buff, uint32_t *len);

Int32 UniPacket_decode(UniPacket * pack, const char* buff, uint32_t len);

Int32 UniPacket_size(UniPacket * pack);

void UniPacket_del(UniPacket ** handle);

Int32 UniPacket_init(UniPacket *handle);

UniPacket * UniPacket_new(void);

#endif
