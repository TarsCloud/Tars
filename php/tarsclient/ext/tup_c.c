#include "./include/tup_c.h"
#include "./include/tars_c.h"

#include "php.h"
#include "php_ini.h"
#include "Zend/zend_interfaces.h"
#include "ext/standard/info.h"
#include "./include/php_tupapi.h"
#include "./include/ttars.h"

Int32 TUP_putBool(void * pack, const char * name, Bool value)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeBool(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TARS_SUCCESS;
}

Int32 TUP_putChar  (void * pack, const char * name, Char value)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeChar(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TARS_SUCCESS;
}

Int32 TUP_putUInt8  (void * pack, const char * name, UInt8 value)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeUInt8(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TARS_SUCCESS;
}

Int32 TUP_putShort  (void * pack, const char * name, Short value)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeShort(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TARS_SUCCESS;
}

Int32 TUP_putUInt16 (void * pack, const char * name, UInt16 value)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeUInt16(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TARS_SUCCESS;
}

Int32 TUP_putFloat  (void * pack, const char * name, Float value)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeFloat(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TARS_SUCCESS;
}

Int32 TUP_putDouble(void * pack, const char * name, Double value)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeDouble(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TARS_SUCCESS;
}

Int32 TUP_putInt32 (void * pack, const char * name, Int32  value)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeInt32(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TARS_SUCCESS;
}

Int32 TUP_putUInt32	(void * pack, const char * name, UInt32 value)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeUInt32(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TARS_SUCCESS;
}


Int32 TUP_putInt64 (void * pack, const char * name, Int64  value)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	TarsOutputStream_writeInt64(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TARS_SUCCESS;
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

	return TARS_SUCCESS;
}

Int32 TUP_putVector(void * pack, const char * name, JArray * value)
{
	Int32 ret=0;
	char sType[64+1]={0};

	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeVector(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return  TARS_SUCCESS;
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

	return  TARS_SUCCESS;
}

Int32 TUP_putMap(void * pack, const char * name, JMapWrapper * value)
{
	Int32 ret=0;
	char sType[64+1]={0};

	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	ret = TarsOutputStream_writeMap(attr->value_os, value, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TARS_SUCCESS;
}

Int32 TUP_putStruct(void * pack, const char * name, const void* st)
{
	Int32 ret=0;
	const JStructBase * jst = st;
	UniAttribute * attr = pack;
	TarsOutputStream_reset(attr->value_os);
	// value_os中写入结构体，但是似乎之后没有用到？
	// 不对，就在下面的最后一个参数，用到了。
	ret = TarsOutputStream_writeStruct(attr->value_os, st, 0);
	if (TARS_SUCCESS != ret)	return ret;
	return TARS_SUCCESS;
}

static Int32 TUP_getAttributeV3(const UniAttribute *pack, const char * name, JString ** s)
{
	Int32 ret = 0;
	char * pBuff = NULL;
	uint32_t len = 0;
	TarsInputStream *is_vectorchar;

	const UniAttribute * attr = pack;

	is_vectorchar = TarsInputStream_new();

	if (!is_vectorchar)
	{
		ret = TARS_MALLOC_ERROR;
		goto do_clean;
	}

	// 首先以name作为key找到对应的vectorchar
	TarsOutputStream_reset(attr->value_os);

	TarsOutputStream_reset(attr->os_string);
	ret = TarsOutputStream_writeStringBuffer(attr->os_string, name, strlen(name), 0);
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = JMapWrapper_find(attr->m_data, TarsOutputStream_getBuffer(attr->os_string), TarsOutputStream_getLength(attr->os_string), &pBuff, &len);
	if (TARS_SUCCESS != ret)	goto do_clean;

    // 把pBuff写入is_vectorchar中（）
	TarsInputStream_setBuffer(is_vectorchar, pBuff, len);

	*s = JString_new();
	if (! (*s))
	{
		ret = TARS_MALLOC_ERROR;
		goto do_clean;
	}

	ret = TarsInputStream_readVectorChar(is_vectorchar, *s, 1, true);
	if (TARS_SUCCESS != ret)	goto do_clean;

	do_clean:
	if (is_vectorchar) TarsInputStream_del(&is_vectorchar);

	return ret;
}

//get
Int32 TUP_getBool  (const void * pack, const char * name, Bool* value)
{
	JString *s = NULL;
	const UniAttribute * attr;
	Int32 ret;

	//s = JString_new();
	//if(!s) return TARS_MALLOC_ERROR;

	ret = TUP_getAttributeV3(pack, name, &s);
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

	//s = JString_new();
	//if(!s) return TARS_MALLOC_ERROR;

	ret = TUP_getAttributeV3(pack, name, &s);
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

	ret = TUP_getAttributeV3(pack, name, &s);
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

	//s = JString_new();
	//if(!s) return TARS_MALLOC_ERROR;

	ret = TUP_getAttributeV3(pack, name, &s);
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

	ret = TUP_getAttributeV3(pack, name, &s);
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

	//s = JString_new();
	//if(!s) return TARS_MALLOC_ERROR;

	ret = TUP_getAttributeV3(pack, name, &s);
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

	//s = JString_new();
	//if(!s) return TARS_MALLOC_ERROR;

	ret = TUP_getAttributeV3(pack, name, &s);
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

	ret = TUP_getAttributeV3(pack, name, &s);
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

	ret = TUP_getAttributeV3(pack, name, &s);
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

	ret = TUP_getAttributeV3(pack, name, &s);
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

	//s = JString_new();
	//if(!s) return TARS_MALLOC_ERROR;

	ret = TUP_getAttributeV3(pack, name, &s);
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

	ret = TUP_getAttributeV3(pack, name, &s);
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

	ret = TUP_getAttributeV3(pack, name, &s);
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

	ret = TUP_getAttributeV3(pack, name, &s);
	if (TARS_SUCCESS != ret)	goto do_clean;

	attr = pack;
	ret = TarsInputStream_setBuffer(attr->value_is, JString_data(s), JString_size(s));
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = TarsInputStream_readMap(attr->value_is, value, 0, true);

	do_clean:
	JString_del(&s);

	return ret;
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
		//TarsFree(thiz);
		UniAttribute_del(&thiz);
		return NULL;
	}
	return thiz;
}

///////////////////////////////////////////////////////////////



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
	ret = TarsInputStream_readMap(is, pack->status, 10, true);        if (TARS_SUCCESS != ret) goto do_exit;

	TarsInputStream_reset(is);

	ret = TarsInputStream_setBuffer(is, JString_data(pack->sBuffer), JString_size(pack->sBuffer));
	if (TARS_SUCCESS != ret)	goto do_exit;

	JMapWrapper_clear(((UniAttribute*)pack)->m_data);
	ret = TarsInputStream_readMap(is, ((UniAttribute*)pack)->m_data, 0, true);

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

	//clean attr
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
		//TarsFree(thiz);
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

/* ====================== for php only ===================== */
Int32 php_TUP_putStruct(void * pack, const char * name, void * st)
{
	Int32 ret=0;
	UniAttribute * attr = pack;
	zval * clazz = st;
	TarsOutputStream_reset(attr->value_os);

	ret = struct_packer(NULL,attr->value_os, 0, clazz);
	if (TARS_SUCCESS != ret) return ret;

	return TARS_SUCCESS;
}


Int32 php_TUP_getStruct(void * pack, const char * name, void * st,void ** ret_val)
{

	Int32 ret=0;
	UniAttribute * att = pack;
	zval *struct_name, * clazz = st;

	JString * js = NULL;
	do {
		ret = TUP_getAttributeV3(att, name, &js);
		if (ret != TARS_SUCCESS) break;
		ret = TarsInputStream_setBuffer(att->value_is, JString_data(js), JString_size(js));
		if (ret != TARS_SUCCESS) break;
		ret = struct_unpacker(att->value_is, 0, true, clazz,ret_val);

	} while (0);
	if (js) JString_del(&js);

	return ret;
}

Int32 Unipacket_getCode(UniPacket* unpack,JString *tmp) {

	Int32 ret = 0;
	char * pBuff = NULL;
	uint32_t len = 0;
	TarsInputStream *is_string;

	is_string = TarsInputStream_new();

	if (!is_string)
	{
		ret = TARS_MALLOC_ERROR;
		goto do_clean;
	}

	TarsOutputStream_reset(((UniAttribute*)unpack)->os_string);
	ret = TarsOutputStream_writeStringBuffer(((UniAttribute*)unpack)->os_string, "STATUS_RESULT_CODE", strlen("STATUS_RESULT_CODE"), 0);
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = JMapWrapper_find(unpack->status, TarsOutputStream_getBuffer(((UniAttribute*)unpack)->os_string), TarsOutputStream_getLength(((UniAttribute*)unpack)->os_string), &pBuff, &len);
	if (TARS_SUCCESS != ret)	goto do_clean;

    // 把pBuff写入is_vectorchar中（）
	TarsInputStream_setBuffer(is_string, pBuff, len);


	ret = TarsInputStream_readString(is_string, tmp, 1, true);

	if (TARS_SUCCESS != ret)	goto do_clean;

	do_clean:
	if (is_string) TarsInputStream_del(&is_string);

	return ret;
}

Int32 Unipacket_getMsg(UniPacket* unpack,JString **tmp) {

	Int32 ret = 0;
	char * pBuff = NULL;
	uint32_t len = 0;
	TarsInputStream *is_string;

	is_string = TarsInputStream_new();

	if (!is_string)
	{
		ret = TARS_MALLOC_ERROR;
		goto do_clean;
	}

	TarsOutputStream_reset(((UniAttribute*)unpack)->os_string);
	ret = TarsOutputStream_writeStringBuffer(((UniAttribute*)unpack)->os_string, "STATUS_RESULT_DESC", strlen("STATUS_RESULT_DESC"), 0);
	if (TARS_SUCCESS != ret)	goto do_clean;

	ret = JMapWrapper_find(unpack->status, TarsOutputStream_getBuffer(((UniAttribute*)unpack)->os_string), TarsOutputStream_getLength(((UniAttribute*)unpack)->os_string), &pBuff, &len);
	if (TARS_SUCCESS != ret)	goto do_clean;

    // 把pBuff写入is_vectorchar中（）
	TarsInputStream_setBuffer(is_string, pBuff, len);


	ret = TarsInputStream_readString(is_string, *tmp, 1, true);

	if (TARS_SUCCESS != ret)	goto do_clean;

	do_clean:
	if (is_string) TarsInputStream_del(&is_string);

	return ret;
}