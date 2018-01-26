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

#ifndef __TARS_C_H__
#define __TARS_C_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/////////////////////////////////////////////////////////////////////////////////////
typedef int     Bool;
#ifdef ANDROID
typedef signed char  Char;
#else
typedef char    Char;
#endif
typedef short   Short;
typedef float   Float;
typedef double  Double;
typedef int     Int32;

typedef unsigned char   uint8_t;
typedef unsigned int    uint32_t;

typedef unsigned char	UInt8;
typedef unsigned short	UInt16;
typedef unsigned int	UInt32;

#ifndef __WORDSIZE
#define __WORDSIZE 32
#endif

typedef struct
{
    Int32 high;
    Int32 low;
}MTKInt64;

#ifdef __MTK_64
typedef MTKInt64  Int64;
#else
#if __WORDSIZE == 64
typedef long    Int64;
#else
typedef long long   Int64;
#endif
#endif

#ifndef true
#define true  1
#endif
#ifndef false
#define false 0
#endif

#ifndef TarsMalloc
#define TarsMalloc(nsize)       malloc(nsize);
#endif
#ifndef TarsFree
#define TarsFree(p)             free(p);
#endif

//返回值定义
extern const Int32 TARS_SUCCESS        ;      //成功
extern const Int32 TARS_ATTR_NOT_FOUND ;      //查找不到相关属性
extern const Int32 TARS_ENCODE_ERROR   ;      //编码错误
extern const Int32 TARS_DECODE_ERROR   ;      //解码错误
extern const Int32 TARS_RUNTIME_ERROR  ;      //其他运行时错误
extern const Int32 TARS_MALLOC_ERROR   ;      //内存申请失败错误

#ifndef TARS_MAX_STRING_LENGTH
#define TARS_MAX_STRING_LENGTH   (100 * 1024 * 1024)
#endif

#define tars__bswap_constant_16(x) \
        ((((x) & 0xff00) >> 8) | (((x) & 0x00ff) << 8));

#define tars__bswap_constant_32(x) \
        ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |           \
            (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24));
            
Int64 tars__bswap_constant_64(Int64 x);
           
Int64 tars_ntohll(Int64 x);
Float tars_ntohf(Float x);
Double tars_ntohd(Double x);

#define tars_htonll(x)    tars_ntohll(x);
#define tars_htonf(x)     tars_ntohf(x);
#define tars_htond(x)     tars_ntohd(x);
#define tars_ntohs(x)     tars__bswap_constant_16(x);
#define tars_htons(x)     tars__bswap_constant_16(x);
#define tars_ntohl(x)     tars__bswap_constant_32(x);
#define tars_htonl(x)     tars__bswap_constant_32(x);

////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct JStructBase JStructBase; 
typedef struct JString JString;
typedef struct JArray JArray;
typedef struct JMapWrapper JMapWrapper;

struct DataHead;
struct TarsStream;

typedef struct DataHead    DataHead;
typedef struct TarsStream   TarsStream;
typedef struct TarsStream   TarsInputStream;
typedef struct TarsStream   TarsOutputStream;


struct JStructBase
{
    char * className;
    Int32 (*writeTo)(const void*, TarsOutputStream *);
    Int32 (*readFrom)(void*, TarsInputStream *);
};

//string封装
struct JString
{
    char *        _data;
    unsigned int  _len;
    unsigned int  _buf_len;
};

void JString_del(JString **st);


void JString_copy(char * dest, const char * src, uint32_t len);

Int32 JString_copyChar(JString * s, char * data, uint32_t len);

void JString_clear(JString *s);

Int32 JString_reserve(JString *s, uint32_t n);

Int32 JString_assign(JString *s, const char * data, uint32_t len);

Int32 JString_append(JString * s, const char * data, uint32_t len);

Int32 JString_insert(JString *s, uint32_t pos, char v);

uint32_t JString_size(JString *s);
int JString_empty(JString *s);
uint32_t JString_capacity(JString *s);
char * JString_data(JString *s);
Int32 JString_resize(JString *s, uint32_t n);


Int32 JString_init(JString *s);

JString * JString_new(void);


//数组封装
struct JArray
{
    char *   elem_type_name;//类型名称
    unsigned elem_num;
    unsigned list_len;
    int *    list;
    unsigned buff_used;
    unsigned buff_len;
    char *   buff;          //元素的tars编码结果
};

void JArray_del(JArray ** arr);

Int32 JArray_reserveList(JArray *arr, uint32_t num);

Int32 JArray_reserveBuff(JArray *arr, uint32_t len);

Int32 JArray_pushBack(JArray *arr, const char * data, uint32_t len);

Int32 JArray_pushBackString(JArray *arr, const char * data);

int JArray_get(JArray *arr, unsigned index, char *data, uint32_t * len);

char *JArray_getPtr(JArray *arr, unsigned index);

int JArray_getLength(JArray *arr, unsigned index);

int JArray_size(JArray *arr);

int JArray_empty(JArray *arr);

void JArray_clear(JArray *arr);

void JArray_init(JArray *arr);


JArray * JArray_new(const char * type);



//map封装
struct JMapWrapper
{
    JArray * first;
    JArray * second;
};

void JMapWrapper_del(JMapWrapper ** m);

int JMapWrapper_put(JMapWrapper *m, const char * first_data, unsigned first_len, const char *value_data, unsigned value_len);

int JMapWrapper_find(JMapWrapper *m, const char * first_data, unsigned first_len, char **value_data, unsigned * value_len);

int JMapWrapper_size(JMapWrapper * m);

int JMapWrapper_getByIndex(JMapWrapper *m, unsigned index, char *first, uint32_t * first_len, char* second, uint32_t * second_len);

int JMapWrapper_empty(JMapWrapper *m);

void JMapWrapper_clear(JMapWrapper *m);

void JMapWrapper_init(JMapWrapper *m);


JMapWrapper * JMapWrapper_new(const char * first_type, const char * second_type);


enum
{
    eChar = 0,
    eShort = 1,
    eInt32 = 2,
    eInt64 = 3,
    eFloat = 4,
    eDouble = 5,
    eString1 = 6,
    eString4 = 7,
    eMap = 8,
    eList = 9,
    eStructBegin = 10,
    eStructEnd = 11,
    eZeroTag = 12,
    eSimpleList = 13
};

////////////////////////////////////////////////////////////////////////////////////////////////////


//缓冲区读写器封装
struct TarsStream
{
    JString     *    _buf;       //< 缓冲区
    uint32_t         _cur;       //< 当前位置
    char             _err[32];   //< 错误描述
    DataHead    *    _h;         //< 辅助协议头解析
};

/*
/// 缓冲区读取器封装
struct TarsInputStream
{
    JString     *    _buf;       //< 缓冲区
    uint32_t         _cur;       //< 当前位置
    char             _err[32];  //< 错误描述
    DataHead    *    _h;         //  辅助协议头解析
};

////////////////////////////////////////////////////////////////////////////
/// 缓冲区写入器封装
struct  TarsOutputStream
{
    JString    * _buf;      //< 缓冲区
    char       _err[32];   //< 错误描述
    DataHead   * _h;
};
*/

struct DataHead
{
    uint8_t _type;
    uint8_t _tag;
};

typedef struct helper
{
    //unsigned int    type : 4;
    //unsigned int    tag : 4;
    unsigned char type_tag; 
} helper;
//}__attribute__((packed))  helper;

uint8_t helper_getTag(helper *h);

void helper_setTag(helper *h, unsigned int t);

uint8_t helper_getType(helper *h);

void helper_setType(helper *h, unsigned int t);

void DataHead_del(DataHead ** head);

uint8_t DataHead_getTag(DataHead * head);
//void DataHead_setTag(DataHead * head, uint8_t t);
uint8_t DataHead_getType(DataHead * head);
//void DataHead_setType(DataHead * head, uint8_t t);

/// 读取头信息，但不前移流的偏移量
Int32 DataHead_peekFrom(DataHead * head, TarsInputStream* is, uint32_t *n);

/// 读取数据头信息
Int32 DataHead_readFrom(DataHead * head, TarsInputStream* is);

/// 写入数据头信息
Int32 DataHead_writeTo(DataHead * head, TarsOutputStream* os);

//设置type tag 并写到os
Int32 DataHead_setAndWriteTo(DataHead * head, unsigned int type, unsigned int tag,TarsOutputStream* os);
 
 
void DataHead_init(DataHead * head);

DataHead * DataHead_new(void);
////////////////////////////////////////////////////////////////////////////

void TarsInputStream_del(TarsInputStream ** is);

void TarsInputStream_reset(TarsInputStream * is);

/// 读取缓存
Int32 TarsInputStream_readBuf(TarsInputStream * is, void * buf, uint32_t len);

/// 读取缓存，但不改变偏移量
Int32 TarsInputStream_peekBuf(TarsInputStream * is, void * buf, uint32_t len, uint32_t offset);

/// 跳过len个字节
Int32 TarsInputStream_skip(TarsInputStream * is, uint32_t len);

/// 设置缓存
Int32 TarsInputStream_setBuffer(TarsInputStream * is, const char * buf, uint32_t len);


/// 跳到指定标签的元素前
Int32 TarsInputStream_skipToTag(TarsInputStream * is, uint8_t tag);

/// 跳到当前结构的结束
Int32 TarsInputStream_skipToStructEnd(TarsInputStream * is);

/// 跳过一个字段
Int32 TarsInputStream_skipField(TarsInputStream * is);

/// 跳过一个字段，不包含头信息
Int32 TarsInputStream_skipFieldByType(TarsInputStream * is, uint8_t type);

Int32 TarsInputStream_checkValid(TarsInputStream * is, uint8_t tag, Bool isRequire);

Int32 TarsInputStream_readByChar(TarsInputStream * is, Char * n);

Int32 TarsInputStream_readByShort(TarsInputStream * is, Short * n);

Int32 TarsInputStream_readByInt32(TarsInputStream * is, Int32 * n);

Int32 TarsInputStream_readBool	(TarsInputStream * is, Bool* b, uint8_t tag, Bool isRequire);

Int32 TarsInputStream_readChar	(TarsInputStream * is, Char* c, uint8_t tag, Bool isRequire);

Int32 TarsInputStream_readUInt8	(TarsInputStream * is, UInt8  * n, uint8_t tag, Bool isRequire);

Int32 TarsInputStream_readShort	(TarsInputStream * is, Short  * n, uint8_t tag, Bool isRequire);

Int32 TarsInputStream_readUInt16	(TarsInputStream * is, UInt16 * n, uint8_t tag, Bool isRequire);

Int32 TarsInputStream_readInt32	(TarsInputStream * is, Int32 * n, uint8_t tag, Bool isRequire);

Int32 TarsInputStream_readUInt32	(TarsInputStream * is, UInt32 * n, uint8_t tag, Bool isRequire);

Int32 TarsInputStream_readInt64(TarsInputStream * is, Int64* n, uint8_t tag, Bool isRequire);

Int32 TarsInputStream_readFloat(TarsInputStream * is, Float* n, uint8_t tag, Bool isRequire);

Int32 TarsInputStream_readDouble(TarsInputStream * is, Double* n, uint8_t tag, Bool isRequire);

Int32 TarsInputStream_readString(TarsInputStream * is, JString* s, uint8_t tag, Bool isRequire);

Int32 TarsInputStream_readMap(TarsInputStream * is, JMapWrapper* m, uint8_t tag, Bool isRequire);

Int32 TarsInputStream_readVector(TarsInputStream * is, JArray* v, uint8_t tag, Bool isRequire);

Int32 TarsInputStream_readVectorChar(TarsInputStream * is, JString *v, uint8_t tag, Bool isRequire);

/// 读取结构
Int32 TarsInputStream_readStruct(TarsInputStream * is, void *st, uint8_t tag, Bool isRequire);

Int32 TarsInputStream_readStructString(TarsInputStream * is, JString * st, uint8_t tag, Bool isRequire);

Int32 TarsInputStream_init(TarsInputStream* is);

TarsInputStream * TarsInputStream_new(void);

////////////////////////////////////////////////
void TarsOutputStream_del(TarsOutputStream ** os);

void TarsOutputStream_reset(TarsOutputStream * os);

Int32 TarsOutputStream_writeBuf(TarsOutputStream * os, const void * buf, uint32_t len);

char * TarsOutputStream_getBuffer(TarsOutputStream * os);

uint32_t TarsOutputStream_getLength(TarsOutputStream * os);

Int32 TarsOutputStream_writeBool(TarsOutputStream * os, Bool b, uint8_t tag);

Int32 TarsOutputStream_writeChar(TarsOutputStream * os, Char n, uint8_t tag);

Int32 TarsOutputStream_writeUInt8(TarsOutputStream * os, UInt8 n, uint8_t tag);

Int32 TarsOutputStream_writeShort(TarsOutputStream * os, Short n, uint8_t tag);

Int32 TarsOutputStream_writeUInt16(TarsOutputStream * os, UInt16 n, uint8_t tag);

Int32 TarsOutputStream_writeInt32(TarsOutputStream * os, Int32 n, uint8_t tag);

Int32 TarsOutputStream_writeUInt32(TarsOutputStream * os, UInt32 n, uint8_t tag);

Int32 TarsOutputStream_writeInt64(TarsOutputStream * os, Int64 n, uint8_t tag);

Int32 TarsOutputStream_writeFloat(TarsOutputStream * os, Float n, uint8_t tag);

Int32 TarsOutputStream_writeDouble(TarsOutputStream * os, Double n, uint8_t tag);

Int32 TarsOutputStream_writeString(TarsOutputStream * os, JString* s, uint8_t tag);
Int32 TarsOutputStream_writeStringBuffer(TarsOutputStream * os, const char* buff, uint32_t len, uint8_t tag);

Int32 TarsOutputStream_writeMap(TarsOutputStream * os, JMapWrapper* m, uint8_t tag);

Int32 TarsOutputStream_writeVector(TarsOutputStream * os, JArray* v, uint8_t tag);

Int32 TarsOutputStream_writeVectorChar(TarsOutputStream * os, JString *v, uint8_t tag);
Int32 TarsOutputStream_writeVectorCharBuffer(TarsOutputStream * os, const char* buff, uint32_t len, uint8_t tag);

Int32 TarsOutputStream_writeStruct(TarsOutputStream * os,const void * st, uint8_t tag);
Int32 TarsOutputStream_writeStructString(TarsOutputStream * os, JString * v, uint8_t tag);
Int32 TarsOutputStream_writeStructBuffer(TarsOutputStream * os, const char* buff, uint32_t len, uint8_t tag);

Int32 TarsOutputStream_init(TarsOutputStream * os);

TarsOutputStream * TarsOutputStream_new(void);

#endif
