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
#include "tup_c.h"
#include "Tars_c.h"

///////////////////////////////////////////////////////////////
//put
static Int32 TUP_putAttribute(UniAttribute *pack, const char * name, const char * type, TarsOutputStream * data_value)
{
	Int32 ret = 0;
	TarsOutputStream *os_map_first=NULL, *os_map_second=NULL;

	UniAttribute * attr = pack;

	os_map_first = TarsOutputStream_new();
	os_map_second= TarsOutputStream_new();

	JMapWrapper_clear(attr->m_data);

	if (!os_map_second || !os_map_first)
	{
		ret = TARS_MALLOC_ERROR; 
		goto do_clean;
	}

	TarsOutputStream_reset(attr->os_string);
	ret = TarsOutputStream_writeStringBuffer(attr->os_string, type, strlen(type), 0);
	if (ret) goto do_clean;

	ret = TarsOutputStream_writeVectorCharBuffer(os_map_second, TarsOutputStream_getBuffer(data_value), TarsOutputStream_getLength(data_value), 1);
	if (ret) goto do_clean;

	ret = JMapWrapper_put(attr->m_data, TarsOutputStream_getBuffer(attr->os_string), TarsOutputStream_getLength(attr->os_string),
						  TarsOutputStream_getBuffer(os_map_second), TarsOutputStream_getLength(os_map_second));
	if (ret) goto do_clean;

	ret = TarsOutputStream_writeMap(os_map_first, attr->m_data, 1);
	if (ret) goto do_clean;

	TarsOutputStream_reset(attr->os_string);
	ret = TarsOutputStream_writeStringBuffer(attr->os_string, name, strlen(name), 0);
	if (ret) goto do_clean;

	ret = JMapWrapper_put(attr->_data, TarsOutputStream_getBuffer(attr->os_string), TarsOutputStream_getLength(attr->os_string),
						  TarsOutputStream_getBuffer(os_map_first), TarsOutputStream_getLength(os_map_first));
	if (ret) goto do_clean;

	do_clean:
	if (os_map_first)	   TarsOutputStream_del(&os_map_first);
	if (os_map_second)	   TarsOutputStream_del(&os_map_second);

	return ret;
}

Int32 TUP_putBool(void * pack, const char * name, Bool value)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeBool(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TUP_putAttribute(pack, name, "bool", attr->value_os);
}

Int32 TUP_putChar  (void * pack, const char * name, Char value)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeChar(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TUP_putAttribute(pack, name, "char", attr->value_os);
}

Int32 TUP_putUInt8  (void * pack, const char * name, UInt8 value)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeUInt8(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TUP_putAttribute(pack, name, "uint8", attr->value_os);
}

Int32 TUP_putShort  (void * pack, const char * name, Short value)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeShort(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TUP_putAttribute(pack, name, "short", attr->value_os);
}

Int32 TUP_putUInt16 (void * pack, const char * name, UInt16 value)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeUInt16(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TUP_putAttribute(pack, name, "uint16", attr->value_os); 
}

Int32 TUP_putFloat  (void * pack, const char * name, Float value)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeFloat(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TUP_putAttribute(pack, name, "float", attr->value_os);
}

Int32 TUP_putDouble(void * pack, const char * name, Double value)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeDouble(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TUP_putAttribute(pack, name, "double", attr->value_os);
}

Int32 TUP_putInt32 (void * pack, const char * name, Int32  value)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeInt32(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TUP_putAttribute(pack, name, "int32", attr->value_os);
}

Int32 TUP_putUInt32	(void * pack, const char * name, UInt32 value)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeUInt32(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TUP_putAttribute(pack, name, "uint32", attr->value_os);
}


Int32 TUP_putInt64 (void * pack, const char * name, Int64  value)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	TarsOutputStream_writeInt64(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TUP_putAttribute(pack, name, "int64", attr->value_os);
}

Int32 TUP_putString(void * pack, const char * name, JString * value)
{
	return TUP_putStringBuffer(pack, name, JString_data(value), JString_size(value));
}

Int32 TUP_putStringBuffer(void * pack, const char * name, const char* buff, uint32_t len)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeStringBuffer(attr->value_os, buff, len, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TUP_putAttribute(pack, name, "string", attr->value_os);
}

Int32 TUP_putVector(void * pack, const char * name, JArray * value)
{
	Int32 ret=0;
	char sType[64+1]={0};

	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeVector(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	snprintf(sType, sizeof(sType)-1, "list<%s>", value->elem_type_name);

	return  TUP_putAttribute(pack, name, sType , attr->value_os);
}

Int32 TUP_putVectorChar(void * pack, const char * name,  JString* value)
{
	return TUP_putVectorCharBuffer(pack, name, JString_data(value), JString_size(value));
}

Int32 TUP_putVectorCharBuffer(void * pack, const char * name,  const char* buff, uint32_t len)
{
	Int32 ret=0;

	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeVectorCharBuffer(attr->value_os, buff, len, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return  TUP_putAttribute(pack, name, "list<char>", attr->value_os);
}

Int32 TUP_putMap(void * pack, const char * name, JMapWrapper * value)
{
	Int32 ret=0;
	char sType[64+1]={0};

	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeMap(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	snprintf(sType, sizeof(sType)-1, "map<%s,%s>", value->first->elem_type_name, value->second->elem_type_name);
	return TUP_putAttribute(pack, name, sType, attr->value_os);
}

Int32 TUP_putStruct(void * pack, const char * name, const void* st)
{
	Int32 ret=0;
	const JStructBase * jst = st; 
	UniAttribute * attr = pack; 
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeStruct(attr->value_os, st, 0);
	if (TARS_SUCCESS != ret)	return ret;
	return TUP_putAttribute(pack, name, jst->className, attr->value_os);
}

static Int32 TUP_getAttribute(const UniAttribute *pack, const char * name, const char * type, JString ** s)
{
	Int32 ret = 0;
	char * pBuff = NULL;
	uint32_t len = 0;
	TarsInputStream *is_map;

	const UniAttribute * attr = pack;

	JMapWrapper_clear(attr->m_data);

	is_map = TarsInputStream_new();

	if (!is_map)
	{
		ret = TARS_MALLOC_ERROR;
		goto do_clean;
	}

	TarsOutputStream_reset(attr->value_os);

	TarsOutputStream_reset(attr->os_string);
	ret = TarsOutputStream_writeStringBuffer(attr->os_string, name, strlen(name), 0);
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = JMapWrapper_find(attr->_data, TarsOutputStream_getBuffer(attr->os_string), TarsOutputStream_getLength(attr->os_string), &pBuff, &len);
	if (TARS_SUCCESS != ret)	goto do_clean;

	TarsInputStream_setBuffer(is_map, pBuff, len);

	ret = TarsInputStream_readMap(is_map, attr->m_data, 1, true);
	if (TARS_SUCCESS != ret)	goto do_clean;

	TarsOutputStream_reset(attr->os_string);
	ret = TarsOutputStream_writeStringBuffer(attr->os_string, type, strlen(type), 0);
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = JMapWrapper_find(attr->m_data, TarsOutputStream_getBuffer(attr->os_string), TarsOutputStream_getLength(attr->os_string), &pBuff, &len);
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = TarsInputStream_setBuffer(is_map, pBuff, len);
	if (TARS_SUCCESS != ret)	goto do_clean;

	*s = JString_new();
	if (! (*s))
	{
		ret = TARS_MALLOC_ERROR;
		goto do_clean;
	}

	ret = TarsInputStream_readVectorChar(is_map, *s, 1, true);
	if (TARS_SUCCESS != ret)	goto do_clean;

	do_clean:
	if (is_map)			  TarsInputStream_del(&is_map);

	return ret;
}


//get
Int32 TUP_getBool  (const void * pack, const char * name, Bool* value)
{
	JString *s = NULL;
	const UniAttribute * attr;
	Int32 ret;

	ret = TUP_getAttribute(pack, name, "bool", &s);
	if (TARS_SUCCESS != ret)	goto do_clean;

	attr = pack;
	ret = TarsInputStream_setBuffer(attr->value_is, JString_data(s), JString_size(s));
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = TarsInputStream_readBool(attr->value_is, value, 0, true);

	do_clean:
	JString_del(&s);

	return ret;
}

Int32 TUP_getChar  (const void * pack, const char * name, Char* value)
{
	JString *s = NULL;
	const UniAttribute * attr;
	Int32 ret;

	ret = TUP_getAttribute(pack, name, "char", &s);
	if (TARS_SUCCESS != ret)	goto do_clean;

	attr = pack;
	ret = TarsInputStream_setBuffer(attr->value_is, JString_data(s), JString_size(s));
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = TarsInputStream_readChar(attr->value_is, value, 0, true);

	do_clean:
	JString_del(&s);

	return ret;
}

Int32 TUP_getUInt8	(const void * pack, const char * name, UInt8 * value)
{
	JString *s = NULL;
	const UniAttribute * attr;
	Int32 ret;

	ret = TUP_getAttribute(pack, name, "uint8", &s);
	if (TARS_SUCCESS != ret)	goto do_clean;

	attr = pack;
	ret = TarsInputStream_setBuffer(attr->value_is, JString_data(s), JString_size(s));
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = TarsInputStream_readUInt8(attr->value_is, value, 0, true);

do_clean:
	JString_del(&s);

	return ret;	
}

Int32 TUP_getShort	(const void * pack, const char * name, Short * value)
{
	JString *s = NULL;
	const UniAttribute * attr;
	Int32 ret;

	ret = TUP_getAttribute(pack, name, "short", &s);
	if (TARS_SUCCESS != ret)	goto do_clean;

	attr = pack;
	ret = TarsInputStream_setBuffer(attr->value_is, JString_data(s), JString_size(s));
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = TarsInputStream_readShort(attr->value_is, value, 0, true);

	do_clean:
	JString_del(&s);

	return ret;
}

Int32 TUP_getUInt16	(const void * pack, const char * name, UInt16 * value)
{
	JString *s = NULL;
	const UniAttribute * attr;
	Int32 ret;

	ret = TUP_getAttribute(pack, name, "uint16", &s);
	if (TARS_SUCCESS != ret)	goto do_clean;

	attr = pack;
	ret = TarsInputStream_setBuffer(attr->value_is, JString_data(s), JString_size(s));
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = TarsInputStream_readUInt16(attr->value_is, value, 0, true);

do_clean:
	JString_del(&s);

	return ret;
}

Int32 TUP_getFloat (const void * pack, const char * name, Float* value)
{
	JString *s = NULL;
	const UniAttribute * attr;
	Int32 ret;

	ret = TUP_getAttribute(pack, name, "float", &s);
	if (TARS_SUCCESS != ret)	goto do_clean;

	attr = pack;
	ret = TarsInputStream_setBuffer(attr->value_is, JString_data(s), JString_size(s));
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = TarsInputStream_readFloat(attr->value_is, value, 0, true);

	do_clean:
	JString_del(&s);

	return ret;
}

Int32 TUP_getDouble(const void * pack, const char * name, Double* value)
{
	JString *s = NULL;
	const UniAttribute * attr;
	Int32 ret;

	ret = TUP_getAttribute(pack, name, "double", &s);
	if (TARS_SUCCESS != ret)	goto do_clean;

	attr = pack;
	ret = TarsInputStream_setBuffer(attr->value_is, JString_data(s), JString_size(s));
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = TarsInputStream_readDouble(attr->value_is, value, 0, true);

	do_clean:
	JString_del(&s);

	return ret;
}

Int32 TUP_getInt32 (const void * pack, const char * name, Int32*  value)
{
	JString *s = NULL;
	const UniAttribute * attr;
	Int32 ret;

	ret = TUP_getAttribute(pack, name, "int32", &s);
	if (TARS_SUCCESS != ret)	goto do_clean;

	attr = pack;
	ret = TarsInputStream_setBuffer(attr->value_is, JString_data(s), JString_size(s));
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = TarsInputStream_readInt32(attr->value_is, value, 0, true);

	do_clean:
	JString_del(&s);

	return ret;
}

Int32 TUP_getUInt32	(const void * pack, const char * name, UInt32 * value)
{
	JString *s = NULL;
	const UniAttribute * attr;
	Int32 ret;

	ret = TUP_getAttribute(pack, name, "uint32", &s);
	if (TARS_SUCCESS != ret)	goto do_clean;

	attr = pack;
	ret = TarsInputStream_setBuffer(attr->value_is, JString_data(s), JString_size(s));
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = TarsInputStream_readUInt32(attr->value_is, value, 0, true);

do_clean:
	JString_del(&s);

	return ret;	
}

Int32 TUP_getInt64 (const void * pack, const char * name, Int64*  value)
{
	JString *s = NULL;
	const UniAttribute * attr;
	Int32 ret;

	ret = TUP_getAttribute(pack, name, "int64", &s);
	if (TARS_SUCCESS != ret)	goto do_clean;

	attr = pack;
	ret = TarsInputStream_setBuffer(attr->value_is, JString_data(s), JString_size(s));
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = TarsInputStream_readInt64(attr->value_is, value, 0, true);

	do_clean:
	JString_del(&s);

	return ret;
}


Int32 TUP_getString(const void * pack, const char * name, JString * value)
{
	JString *s = NULL;
	const UniAttribute * attr;
	Int32 ret;

	ret = TUP_getAttribute(pack, name, "string", &s);
	if (TARS_SUCCESS != ret)	goto do_clean;

	attr = pack;
	ret = TarsInputStream_setBuffer(attr->value_is, JString_data(s), JString_size(s));
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = TarsInputStream_readString(attr->value_is, value, 0, true);

	do_clean:
	JString_del(&s);

	return ret;
}

Int32 TUP_getVector(const void * pack, const char * name, JArray * value)
{
	JString *s = NULL;
	const UniAttribute * attr;
	char sType[64+1]={0};
	Int32 ret;

	snprintf(sType, sizeof(sType)-1, "list<%s>", value->elem_type_name);

	ret = TUP_getAttribute(pack, name, sType, &s);
	if (TARS_SUCCESS != ret)	goto do_clean;

	attr = pack;
	ret = TarsInputStream_setBuffer(attr->value_is, JString_data(s), JString_size(s));
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = TarsInputStream_readVector(attr->value_is, value, 0, true);

	do_clean:
	JString_del(&s);

	return ret;
}

Int32 TUP_getVectorChar(const void * pack, const char * name, JString * value)
{
	JString *s = NULL;
	const UniAttribute * attr;
	Int32 ret;

	ret = TUP_getAttribute(pack, name, "list<char>", &s);
	if (TARS_SUCCESS != ret)	goto do_clean;

	attr = pack;
	ret = TarsInputStream_setBuffer(attr->value_is, JString_data(s), JString_size(s));
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = TarsInputStream_readVectorChar(attr->value_is, value, 0, true);

	do_clean:
	JString_del(&s);

	return ret;
}

Int32 TUP_getMap   (const void * pack, const char * name, JMapWrapper * value)
{
	JString *s = NULL;
	const UniAttribute * attr;
	Int32 ret;
	char sType[64+1]={0};

	snprintf(sType, sizeof(sType)-1, "map<%s,%s>", value->first->elem_type_name, value->second->elem_type_name);

	ret = TUP_getAttribute(pack, name, sType, &s);
	if (TARS_SUCCESS != ret)	goto do_clean;

	attr = pack;
	ret = TarsInputStream_setBuffer(attr->value_is, JString_data(s), JString_size(s));
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = TarsInputStream_readMap(attr->value_is, value, 0, true);

	do_clean:
	JString_del(&s);

	return ret;
}

Int32 TUP_getStruct(const void * pack, const char * name, void *st)
{
	Int32 ret=0;
	JString * s = NULL;
	const UniAttribute * attr = pack;
	const JStructBase *jst = st;
	do
	{
		ret = TUP_getAttribute(pack, name, jst->className, &s);
		if ( TARS_SUCCESS != ret ) break;
		ret = TarsInputStream_setBuffer(attr->value_is, JString_data(s), JString_size(s));
		if (TARS_SUCCESS != ret)	 break;
		ret = TarsInputStream_readStruct(attr->value_is, st, 0,true);
	}while (0);
	if (s) JString_del(&s);
	return ret;
}


Int32 UniAttribute_encode(const UniAttribute * pack, char** buff, uint32_t *len)
{
	Int32 ret = 0;
	TarsOutputStream *os = TarsOutputStream_new();
	if (!os) return TARS_MALLOC_ERROR;

	ret = TarsOutputStream_writeMap(os, pack->_data, 0);
	if (TARS_SUCCESS != ret)
	{
		TarsOutputStream_del(&os);
		return ret;
	}

	*buff = TarsMalloc(TarsOutputStream_getLength(os));
	if (! *buff)
	{
		TarsOutputStream_del(&os);
		return TARS_MALLOC_ERROR;
	}
	*len = TarsOutputStream_getLength(os);
	memcpy(*buff, TarsOutputStream_getBuffer(os), *len);

	TarsOutputStream_del(&os);

	return ret;
}

Int32 UniAttribute_decode(UniAttribute * pack, const char* buff, uint32_t len)
{
	Int32 ret;
	TarsInputStream *is = TarsInputStream_new();
	if (!is) return TARS_MALLOC_ERROR;

	ret = TarsInputStream_setBuffer(is, buff, len);
	if (TARS_SUCCESS != ret) goto do_exit;

	ret = TarsInputStream_readMap(is, pack->_data, 0, true);

	pack->_iDecPacketLen = is->_cur;
do_exit:

	TarsInputStream_del(&is);

	return ret;
}

Int32 UniAttribute_size(UniAttribute * pack)
{
	if(!pack) return 0;
	return pack->_iDecPacketLen;
}

///////////////////////////////////////////////////////////////
void UniAttribute_del(UniAttribute ** handle)
{
	UniAttribute ** thiz = (UniAttribute **)handle;
	if (thiz == NULL || *thiz == NULL) return;

	if ((*handle)->_data)	JMapWrapper_del(&((*handle)->_data));
	if ((*thiz)->value_os)	TarsOutputStream_del(&((*thiz)->value_os));
	if ((*thiz)->value_is)	TarsInputStream_del(&((*thiz)->value_is));
	if ((*thiz)->os_string)	TarsOutputStream_del(&((*thiz)->os_string));
	if ((*thiz)->m_data)	JMapWrapper_del(&((*thiz)->m_data));

	TarsFree(*thiz);
	*thiz = NULL;
}

Int32 UniAttribute_init(UniAttribute *handle)
{
	UniAttribute * thiz = (UniAttribute*) handle;

	thiz->_data     = JMapWrapper_new("string", "map<string,list<char>>");
	thiz->value_os  = TarsOutputStream_new();
	thiz->value_is  = TarsInputStream_new();
	thiz->os_string = TarsOutputStream_new();
	thiz->m_data    = JMapWrapper_new("string", "list<char>");
	thiz->_iDecPacketLen = 0;

	if (!thiz->_data || !thiz->value_os || !thiz->value_is || !thiz->os_string || !thiz->m_data)
	{
		if (thiz->_data)	JMapWrapper_del(&thiz->_data);
		if (thiz->value_os)	TarsOutputStream_del(&thiz->value_os);
		if (thiz->value_is)	TarsInputStream_del(&thiz->value_is);
		if (thiz->os_string)TarsOutputStream_del(&thiz->os_string);
		if (thiz->m_data)	JMapWrapper_del(&thiz->m_data);
		return TARS_MALLOC_ERROR;
	}
	return 0;
}


UniAttribute * UniAttribute_new()
{
	Int32 ret=0;
	UniAttribute *thiz = (UniAttribute*)TarsMalloc(sizeof(UniAttribute));
	if (!thiz) return NULL;

	ret = UniAttribute_init(thiz);
	if (ret)
	{
		UniAttribute_del(&thiz);
		return NULL;
	}
	return thiz;
}

///////////////////////////////////////////////////////////////
Int32 UniPacket_size(UniPacket * pack)
{
	const UniAttribute * attr = (UniAttribute*)pack;
	if(!attr) return 0;
	return attr->_iDecPacketLen;
}

Int32 UniPacket_encode(const UniPacket * pack, char** buff, uint32_t *len)
{
	Int32 ret=0;
	Int32 iHeaderLen = 0;
	TarsOutputStream *os;

	os = TarsOutputStream_new();
	if (!os) return TARS_MALLOC_ERROR;

	ret = TarsOutputStream_writeMap(os, ((UniAttribute*)pack)->_data, 0);
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = JString_assign(pack->sBuffer, TarsOutputStream_getBuffer(os), TarsOutputStream_getLength(os));
	if (TARS_SUCCESS != ret)	goto do_clean;

	TarsOutputStream_reset(os);

	ret = TarsOutputStream_writeShort(os, pack->iVersion, 1);     if (TARS_SUCCESS != ret) goto do_clean;
	ret = TarsOutputStream_writeChar(os, pack->cPacketType, 2);   if (TARS_SUCCESS != ret) goto do_clean;
	ret = TarsOutputStream_writeInt32(os, pack->iMessageType, 3); if (TARS_SUCCESS != ret) goto do_clean;
	ret = TarsOutputStream_writeInt32(os, pack->iRequestId, 4);   if (TARS_SUCCESS != ret) goto do_clean;
	ret = TarsOutputStream_writeString(os, pack->sServantName, 5);if (TARS_SUCCESS != ret) goto do_clean;
	ret = TarsOutputStream_writeString(os, pack->sFuncName, 6);   if (TARS_SUCCESS != ret) goto do_clean;
	ret = TarsOutputStream_writeVectorChar(os, pack->sBuffer, 7); if (TARS_SUCCESS != ret) goto do_clean;
	ret = TarsOutputStream_writeInt32(os, pack->iTimeout, 8);     if (TARS_SUCCESS != ret) goto do_clean;
	ret = TarsOutputStream_writeMap(os, pack->context, 9);        if (TARS_SUCCESS != ret) goto do_clean;
	ret = TarsOutputStream_writeMap(os, pack->status, 10);        if (TARS_SUCCESS != ret) goto do_clean;

	iHeaderLen = tars_htonl(sizeof(Int32) + TarsOutputStream_getLength(os));

	*len = sizeof(Int32) + TarsOutputStream_getLength(os);
	*buff = TarsMalloc(*len);
	if (*buff == NULL)
	{
		*len = 0;
		TarsOutputStream_del(&os);
		return TARS_MALLOC_ERROR;
	}

	memcpy(*buff, &iHeaderLen, sizeof(Int32));
	memcpy(*buff + sizeof(Int32), TarsOutputStream_getBuffer(os), TarsOutputStream_getLength(os));

	do_clean:
	TarsOutputStream_del(&os);

	return ret;
}

Int32 UniPacket_decode(UniPacket * pack, const char* buff, uint32_t len)
{
	TarsInputStream  *is;
	Int32 ret;

	if (len < sizeof(Int32))
	{
		return TARS_DECODE_ERROR;
	}

	is = TarsInputStream_new();
	if (!is) return TARS_MALLOC_ERROR;

	ret = TarsInputStream_setBuffer(is, buff + sizeof(Int32), len - sizeof(Int32));
	if (TARS_SUCCESS != ret)	goto do_exit;

	ret = TarsInputStream_readShort(is, &pack->iVersion, 1, true);     if (TARS_SUCCESS != ret) goto do_exit;
	ret = TarsInputStream_readChar(is, &pack->cPacketType, 2, false);  if (TARS_SUCCESS != ret) goto do_exit;
	ret = TarsInputStream_readInt32(is, &pack->iMessageType, 3, false);if (TARS_SUCCESS != ret) goto do_exit;
	ret = TarsInputStream_readInt32(is, &pack->iRequestId, 4, true);   if (TARS_SUCCESS != ret) goto do_exit;
	ret = TarsInputStream_readString(is, pack->sServantName, 5, true); if (TARS_SUCCESS != ret) goto do_exit;
	ret = TarsInputStream_readString(is, pack->sFuncName, 6, true);    if (TARS_SUCCESS != ret) goto do_exit;
	ret = TarsInputStream_readVectorChar (is, pack->sBuffer, 7, true); if (TARS_SUCCESS != ret) goto do_exit;
	ret = TarsInputStream_readInt32(is, &pack->iTimeout, 8, false);    if (TARS_SUCCESS != ret) goto do_exit;
	ret = TarsInputStream_readMap(is, pack->context, 9, false);        if (TARS_SUCCESS != ret) goto do_exit;
	ret = TarsInputStream_readMap(is, pack->status, 10, false);        if (TARS_SUCCESS != ret) goto do_exit;

	TarsInputStream_reset(is);

	ret = TarsInputStream_setBuffer(is, JString_data(pack->sBuffer), JString_size(pack->sBuffer));
	if (TARS_SUCCESS != ret)	goto do_exit;

	JMapWrapper_clear(((UniAttribute*)pack)->_data);
	ret = TarsInputStream_readMap(is, ((UniAttribute*)pack)->_data, 0, true);

	((UniAttribute*)pack)->_iDecPacketLen = tars_ntohl(*(Int32*)(buff));
do_exit:
	TarsInputStream_del(&is);

	return ret;
}

void UniPacket_del(UniPacket ** handle)
{
	UniPacket ** thiz = (UniPacket**)handle;
	if (handle == NULL || *handle == NULL) return;

	if ((*thiz)->sServantName)	JString_del(&(*thiz)->sServantName);
	if ((*thiz)->sFuncName)		JString_del(&(*thiz)->sFuncName);
	if ((*thiz)->sBuffer)		JString_del(&(*thiz)->sBuffer);
	if ((*thiz)->context)		JMapWrapper_del(&(*thiz)->context);
	if ((*thiz)->status)		JMapWrapper_del(&(*thiz)->status);

	if (((UniAttribute*)(*thiz))->_data)		JMapWrapper_del(&(((UniAttribute*)(*thiz))->_data));
	if (((UniAttribute*)(*thiz))->value_os)		TarsOutputStream_del(&(((UniAttribute*)(*thiz))->value_os));
	if (((UniAttribute*)(*thiz))->value_is)		TarsInputStream_del(&(((UniAttribute*)(*thiz))->value_is));
	if (((UniAttribute*)(*thiz))->os_string)	TarsOutputStream_del(&(((UniAttribute*)(*thiz))->os_string));
	if (((UniAttribute*)(*thiz))->m_data)		JMapWrapper_del(&(((UniAttribute*)(*thiz))->m_data));

	TarsFree(*thiz);
	*thiz = NULL;
}

Int32 UniPacket_init(UniPacket *handle)
{
	Int32 ret;

	UniPacket * thiz = (UniPacket*) handle;

	ret = UniAttribute_init((UniAttribute*)thiz);
	if (ret) return ret;

	thiz->iVersion     = 2;
	thiz->cPacketType  = 0;
	thiz->iMessageType = 0;
	thiz->iRequestId   = 0;
	thiz->sServantName = JString_new();
	thiz->sFuncName    = JString_new();
	thiz->sBuffer      = JString_new();
	thiz->iTimeout     = 0;
	thiz->context      = JMapWrapper_new("string", "string");
	thiz->status       = JMapWrapper_new("string", "string");

	if (!thiz->sServantName || !thiz->sFuncName || !thiz->sBuffer ||
		!thiz->context || !thiz->status)
	{
		if (thiz->sServantName)	JString_del(&thiz->sServantName);
		if (thiz->sFuncName)	JString_del(&thiz->sFuncName);
		if (thiz->sBuffer)		JString_del(&thiz->sBuffer);
		if (thiz->context)		JMapWrapper_del(&thiz->context);
		if (thiz->status)		JMapWrapper_del(&thiz->status);

		return TARS_MALLOC_ERROR;
	}

	return 0;

}

UniPacket * UniPacket_new()
{
	Int32 ret;
	UniPacket *thiz = (UniPacket*)TarsMalloc(sizeof(UniPacket));
	if (!thiz) return NULL;

	ret = UniPacket_init(thiz);
	if (TARS_SUCCESS != ret)
	{
		UniPacket_del(&thiz);
		return NULL;
	}
	return thiz;
}

//////////////////////////////////////////////////////////////////////////

Int32 UniPacket_getRequestId(UniPacket *handle)
{
	return handle->iRequestId;
}

void UniPacket_setRequestId(UniPacket *handle, Int32 value)
{
	handle->iRequestId = value;
}

const char* UniPacket_getServantName(UniPacket *handle)
{
	return JString_data(handle->sServantName);
}

void  UniPacket_setServantName(UniPacket *handle, const char*value)
{
	JString_assign(handle->sServantName, value, strlen((const char*)value));
}

const char* UniPacket_getFuncName(UniPacket *handle)
{
	if (handle != NULL) {
		return JString_data(handle->sFuncName);
	}

	return NULL;
}

void UniPacket_setFuncName(UniPacket *handle, const char*value)
{
	JString_assign(handle->sFuncName, value, strlen((const char*)value));
}
