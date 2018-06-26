// **********************************************************************
// TUP_C version 1.0.4 by WSRD Tencent.
// **********************************************************************

#ifndef __TUP_C_H__
#define __TUP_C_H__

#include "tars_c.h"

typedef struct UniAttribute UniAttribute;
typedef struct UniPacket    UniPacket;
//typedef struct UniPacket    TafUniPacket;
typedef struct ResponsePacket ResponsePacket;

//���Է�װ
struct UniAttribute
{
	JMapWrapper 	*_data;
	TarsOutputStream *value_os;
	TarsInputStream  *value_is;

	JMapWrapper     *m_data;  	//encode decode helper
	TarsOutputStream *os_string;	//encode decode helper

	short			_iVer;		//TUP�汾�ı�ʾ(2:��ʾ��ͨTUPЭ�飬3:��ʾ����TUPЭ��)

	char _sLastError[32];
};

///////////////////////////////////////////////////////////////
//put

Int32  php_TUP_putStruct(void * pack, const char * name, void * st);


Int32 php_TUP_getStruct(void * pack, const char * name, void * st, void ** ret_val, Bool is_require);


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

Int32 TUP_getBool	(const void * pack, const char * name, Bool  * value, Bool is_require);

Int32 TUP_getChar	(const void * pack, const char * name, Char  * value, Bool is_require);

Int32 TUP_getUInt8	(const void * pack, const char * name, UInt8 * value, Bool is_require);

Int32 TUP_getShort 	(const void * pack, const char * name, Short * value, Bool is_require);

Int32 TUP_getUInt16	(const void * pack, const char * name, UInt16 * value, Bool is_require);

Int32 TUP_getFloat 	(const void * pack, const char * name, Float* value, Bool is_require);

Int32 TUP_getDouble	(const void * pack, const char * name, Double* value, Bool is_require);

Int32 TUP_getInt32 	(const void * pack, const char * name, Int32*  value, Bool is_require);

Int32 TUP_getUInt32	(const void * pack, const char * name, UInt32 * value, Bool is_require);

Int32 TUP_getInt64 (const void * pack, const char * name, Int64*  value, Bool is_require);

Int32 TUP_getString(const void * pack, const char * name, JString * value, Bool is_require);

Int32 TUP_getVector(const void * pack, const char * name, JArray * value, Bool is_require);

Int32 TUP_getVectorChar(const void * pack, const char * name, JString * value, Bool is_require);

Int32 TUP_getMap   (const void * pack, const char * name, JMapWrapper * value, Bool is_require);


UniAttribute * UniAttribute_new(void);

Int32 UniAttribute_init(UniAttribute *handle);

void UniAttribute_del(UniAttribute ** handle);

///////////////////////////////////////////////////////////////
//�����Ӧ����װ
struct UniPacket
{
	UniAttribute attr;

	Short iVersion;
	Char cPacketType;
	Int32 iMessageType;
	Int32 iRequestId;
	JString *sServantName;
	JString *sFuncName;
	JString *sBuffer;	  //vector<taf::Char> sBuffer;
	Int32 iTimeout;
	JMapWrapper *context;//map<std::string, std::string> context;
	JMapWrapper *status; //map<std::string, std::string> status;
};

void  UniPacket_setVersion(UniPacket * pack, Short siVersion);

Short UniPacket_getVersion(UniPacket * pack);

Int32 Unipacket_getCode(UniPacket* unpack,JString *tmp);

Int32 Unipacket_getMsg(UniPacket* unpack,JString **tmp);

Int32 Unipacket_getStatus(UniPacket *unpack, JString *tmp);

Int32 UniPacket_encode(const UniPacket *pack, TarsOutputStream *os_tmp);

Int32 UniPacket_decode(UniPacket * pack, const char* buff, uint32_t len);

void UniPacket_del(UniPacket ** handle);

Int32 UniPacket_init(UniPacket *handle);

UniPacket * UniPacket_new(void);

struct ResponsePacket
{
	UniAttribute attr;

	Short iVersion;
	Char cPacketType;
	Int32 iRequestId;
	Int32 iMessageType;
	Int32 iRet;
	JString *sBuffer;	  //vector<taf::Char> sBuffer;
	JMapWrapper *status; //map<std::string, std::string> status;
	JString *sResultDesc;
};
ResponsePacket * ResponsePacket_new(void);

Int32 ResponsePacket_init(ResponsePacket *handle);

Int32 ResponsePacket_decode(ResponsePacket * rsp_pack, const char* buff, uint32_t len);

Int32 ResponsePacket_encode(const ResponsePacket * rsp_pack, TarsOutputStream *os_tmp);

void ResponsePacket_del(ResponsePacket ** handle);

#endif
