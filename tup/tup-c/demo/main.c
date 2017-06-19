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

#include <sys/time.h>
#include <time.h>
#include "unistd.h"
#include "tup_c.h"

#include "Test.h"

void testJString()
{
	JString * src	= JString_new();
	JString * dst	= JString_new();
	printf("-----------------testJString------------------------------\n");
	JString_assign(src, "abcd", sizeof("abcd"));
	printf("after assign \"abcd\":%s\n", JString_data(src));

	JString_insert(src, 2, 'i');
	printf("after insert \"i\": %s\n", JString_data(src));

	JString_append(src, "ef", sizeof("ef"));
	printf("after append \"ef\": %s\n", JString_data(src));

	JString_resize(dst, JString_size(src));
	JString_copyChar(dst, JString_data(src), JString_size(src));
	printf("copy src:%s  to dst: %s\n", JString_data(src), JString_data(dst));

	JString_clear(src);
	printf("after clear, JString size=%d\n", JString_size(src));
	JString_del(&src);
	JString_del(&dst);
}


void testJArray()
{
	JArray * arr = JArray_new("testarray");

	printf("-----------------testJArray------------------------------\n");
	JArray_pushBackString(arr, "JArray_pushBackString1111");
	printf("JArray_pushBackString: %s ,len=%d\n", JArray_getPtr(arr, 0), JArray_getLength(arr, 0));

	JArray_pushBackString(arr, "JArray_pushBackString22");
	printf("JArray_pushBackString: %s,len=%d\n", JArray_getPtr(arr, 1), JArray_getLength(arr, 1));

	JArray_del(&arr);
}


void  testStruct()
{
	char		*buff	= NULL;
	uint32_t	len	= 0;

	printf("-----------------testStruct------------------------------\n");
	/* encode */
	{
		TarsOutputStream* os = TarsOutputStream_new();

		UniAttribute	* attr	= UniAttribute_new();
		Test_TestInfo	*st	= Test_TestInfo_new();

		/* 基本类型的赋值 */
		st->ibegin	= 1111;
        st->ii = 54321;
		st->iend	= 9999;
		st->f		= 0.999f;
		st->d		= 123.66f;

		/* string 类型的成员赋值 */
		JString_assign(st->s, "teststring", sizeof("teststring"));

		/* vector<byte> 类型的成员赋值 */
		JString_assign(st->vb, "testvectrbyte", sizeof("testvectrbyte"));

		/* vector<int> 类型的成员赋值,请使用流模式赋值 */
		int i = 0;
		for(i = 0; i < 10; i++)
		{
			TarsOutputStream_reset(os);
			TarsOutputStream_writeInt32(os, 1000 + i, 0);
			JArray_pushBack(st->vi, TarsOutputStream_getBuffer(os), TarsOutputStream_getLength(os));
		}


		st->aa->a	= 200;
		st->aa->b->a	= 300;
		st->aa->b->f	= 0.300;

		uint32_t temp = 323;

		TUP_putUInt32(attr, "int", temp);
		TUP_putStruct(attr, "mystruct", st);
		UniAttribute_encode(attr, &buff, &len);

		printf("UniAttribute_encode len: %d\n", len);
		/* 回收分配的内存 */
		TarsOutputStream_del(&os);
		Test_TestInfo_del(&st);
		UniAttribute_del(&attr);
	}

	/* decode */
	{
		Test_TestInfo	*st = Test_TestInfo_new();
		UniAttribute	*attr_out = UniAttribute_new();
		UniAttribute_decode(attr_out, buff, len);

		printf("UniAttribute_decode len: %d\n", UniAttribute_size(attr_out));
		TarsFree(buff);
		buff = NULL;

		int ret = TUP_getStruct(attr_out, "mystruct", st);

		printf("\nTest_TestInfo get struct: ret = %d, st->className=%s, st->ibegin=%d, st->ii=%d, st->iend=%d, st->d=%f,\n",
		       ret, st->className, st->ibegin, st->ii, st->iend, st->d);

		printf("Test_TestInfo get struct: ret = %d, st->aa->className=%s,st->aa->a=%d\n",
		       ret, st->aa->className, st->aa->a);

		printf("Test_TestInfo get struct: ret = %d, st->aa->b->className=%s,st->aa->b->a=%d,st->aa->b->f=%f\n",
		       ret, st->aa->b->className, st->aa->b->a, st->aa->b->f);
		printf("\n");


		/* vector<int> 类型的读取,请使用流模式赋值 */
		TarsInputStream* is = TarsInputStream_new();

		int i = 0;
		for(i = 0; i < JArray_size(st->vi); i++)
		{
			TarsInputStream_reset(is);
			TarsInputStream_setBuffer(is, JArray_getPtr(st->vi, i), JArray_getLength(st->vi, i));
			int n = 0;
			TarsInputStream_readInt32(is, &n, 0, true);
			printf("st-vi[%d]: %d\n", i, n);
		}

		uint32_t temp;
		TUP_getUInt32(attr_out, "int", &temp);
		printf("int: %d\n", temp);
		TarsInputStream_del(&is);
		Test_TestInfo_del(&st);
		UniAttribute_del(&attr_out);
	}
}


void testtuppack()
{
	char buffer[4096] = { 0 };

	uint32_t len = 0;
	printf("-----------------testtuppack------------------------------\n");
	/* encode */
	int i = 0;
	for(i = 0; i < 10; i++)
	{
		Test_TestInfo *st = Test_TestInfo_new();

        st->aa->a= 200*i;
		st->aa->b->a	= 300;
		st->aa->b->f	= 0.300;
		st->ibegin	= 1111;
        st->ii = 543*i;
		st->iend	= 9999;
		st->d		= 23.0023f;
		st->f		= 0.22f;

		UniPacket* pack = UniPacket_new();

		pack->iVersion = 3;

		pack->cPacketType	= 1;
		pack->iRequestId = 23;

		JString_assign(pack->sServantName, "myservant", sizeof("myservant"));
		JString_assign(pack->sFuncName, "myfun", sizeof("myfun"));

		TUP_putStruct(pack, "mystruct", st);
		uint32_t	l	= 0;
		char		*buff	= NULL;
		UniPacket_encode(pack, &buff, &l);

		printf("UniPacket_encode len: %d\n", l);
		/* 回收分配的内存 */
		Test_TestInfo_del(&st);
		UniPacket_del(&pack);

		memcpy(buffer + len, buff, l);
		len += l;

		TarsFree(buff);
	}

	/*
	 * decode
	 */
	 for( i = 0; i < len;)
	{
		UniPacket *unpack = UniPacket_new();

		UniPacket_decode(unpack, buffer, len);
		i += UniPacket_size(unpack);
		printf("UniPacket_decode len:%d,cur:%d\n", UniPacket_size(unpack),i);

		Test_TestInfo	*st	= Test_TestInfo_new();
		int		ret	= TUP_getStruct(unpack, "mystruct", st);

		printf("\nversion:%d", unpack->iVersion);
		printf("\nservantname:%s", JString_data(unpack->sServantName));
		printf("\nfunname:%s", JString_data(unpack->sFuncName));

		printf("\nTest_TestInfo get struct: ret = %d, st->className=%s, st->ibegin=%d, st->ii=%d, st->iend=%d, st->d=%f,unpack->iRequestId=%d\n",
		       ret, st->className, st->ibegin, st->ii, st->iend, st->d,unpack->iRequestId);

		printf("Test_TestInfo get struct: ret = %d, st->aa->className=%s,st->aa->a=%d\n",
		       ret, st->aa->className, st->aa->a);

		printf("Test_TestInfo get struct: ret = %d, st->aa->b->className=%s,st->aa->b->a=%d,st->aa->b->f=%f\n",
		       ret, st->aa->b->className, st->aa->b->a, st->aa->b->f);
		printf("\n");

		TarsInputStream	* is	= TarsInputStream_new();
		JString		* s	= JString_new();

		JString_del(&s);
		TarsInputStream_del(&is);
		Test_TestInfo_del(&st);
		UniPacket_del(&unpack);
	}
}


void  testVector()
{
	int		i	= 0;
	char		*buff	= NULL;
	uint32_t	len	= 0;
	printf("-----------------testVector------------------------------\n");
	/* encode */
	{
		UniAttribute	* attrv = UniAttribute_new();
		Test_A		*sta	= Test_A_new();
		JArray		*arr	= JArray_new(sta->className);
		TarsOutputStream *vos	= TarsOutputStream_new();


		for(i = 0; i < 10; i++)
		{
			sta->a		= i;
			sta->b->a	= i;
			sta->b->f	= i * 0.1;

			TarsOutputStream_writeStruct(vos, sta, 0);
			JArray_pushBack(arr, TarsOutputStream_getBuffer(vos), TarsOutputStream_getLength(vos));

			TarsOutputStream_reset(vos);
		}

		printf("\n");
		/* 结构体的array直接用put进行编码 */
		TUP_putVector(attrv, "myvector", arr);

		UniAttribute_encode(attrv, &buff, &len);
		UniAttribute_del(&attrv);
		Test_A_del(&sta);
		TarsOutputStream_del(&vos);
		JArray_del(&arr);
	}

	/* decode */
	{
		Test_A		*sta		= Test_A_new();
		UniAttribute	* attrv_out	= UniAttribute_new();;
		JArray		*arrRes		= JArray_new(sta->className);
		TarsInputStream	*ais		= TarsInputStream_new();

		UniAttribute_decode(attrv_out, buff, len);
		TarsFree(buff);
		buff = NULL;
		int ret = TUP_getVector(attrv_out, "myvector", arrRes);
		printf("ret:%d, vector size:%d\n", ret, JArray_size(arrRes));

		for(i = 0; i < JArray_size(arrRes); i++)
		{
			TarsInputStream_setBuffer(ais, JArray_getPtr(arrRes, i), JArray_getLength(arrRes, i));
			TarsInputStream_readStruct(ais, sta, 0, true);
			printf("vector index:%d value:%d,%d,%f\n", i, sta->a, sta->b->a, sta->b->f);
		}
		printf("\n");

		JArray_del(&arrRes);
		UniAttribute_del(&attrv_out);
		Test_A_del(&sta);
		TarsOutputStream_del(&ais);
	}
}


void  testMap()
{
	int		i	= 0;
	char		*buff	= NULL;
	uint32_t	len	= 0;
	printf("-----------------testMap------------------------------\n");
	/* encode */
	{
		char		c[10]	= { 0 };
		UniAttribute	* attr	= UniAttribute_new();;
		JMapWrapper	* m	= JMapWrapper_new("int32", "string");
		TarsOutputStream *first	= TarsOutputStream_new();
		TarsOutputStream *second = TarsOutputStream_new();
		TarsInputStream	*ais	= TarsInputStream_new();

		/* map<int,string>类型的赋值,请使用流模式赋值 */
		for(i = 0; i < 10; i++)
		{
			TarsOutputStream_reset(first);
			TarsOutputStream_writeInt32(first, i, 0);
			sprintf(c, "testmap%d", i);

			TarsOutputStream_reset(second);
			TarsOutputStream_writeStringBuffer(second, c, strlen(c), 2);
			JMapWrapper_put(m, TarsOutputStream_getBuffer(first), TarsOutputStream_getLength(first), TarsOutputStream_getBuffer(second), TarsOutputStream_getLength(second));
		}

		/* map类型的tup编码 */
		int ret = TUP_putMap(attr, "mymap", m);
		printf("ret:%d, map size:%d\n", ret, JMapWrapper_size(m));
		/* 在内部分配内存，需要由用户自己释放 */
		UniAttribute_encode(attr, &buff, &len);

		TarsInputStream_del(&ais);
		TarsOutputStream_del(&first);
		TarsOutputStream_del(&second);
		JMapWrapper_del(&m);
		UniAttribute_del(&attr);
	}


	/* decode */
	{
		JString		* s	= JString_new();
		UniAttribute	* attr	= UniAttribute_new();;
		JMapWrapper	* mRes	= JMapWrapper_new("int32", "string");
		TarsInputStream	* ais	= TarsInputStream_new();

		UniAttribute_decode(attr, buff, len);
		/* map类型的tup解码 */
		int ret = TUP_getMap(attr, "mymap", mRes);
		printf("ret:%d, map size:%d\n", ret, JMapWrapper_size(mRes));
		/* 读map<int, string> 类型的成员 */
		for(i = 0; i < JMapWrapper_size(mRes); ++i)
		{
			Int32 ai;
			TarsInputStream_reset(ais);
			TarsInputStream_setBuffer(ais, JArray_getPtr(mRes->first, i), JArray_getLength(mRes->first, i));
			TarsInputStream_readInt32(ais, &ai, 0, true);

			TarsInputStream_setBuffer(ais, JArray_getPtr(mRes->second, i), JArray_getLength(mRes->second, i));
			TarsInputStream_readString(ais, s, 2, true);
			printf("map index:%d, first:%d, second:%s\n", i, ai, JString_data(s));
		}
		printf("\n");

		JString_del(&s);
		TarsInputStream_del(&ais);
		JMapWrapper_del(&mRes);
		UniAttribute_del(&attr);

		TarsFree(buff);
		buff = NULL;
	}
}


void  testMapWithStruct()
{
	int		i	= 0;
	char		*buff	= NULL;
	uint32_t	len	= 0;
	printf("-----------------testMapWithStruct------------------------------\n");
	/* encode */
	{
		Test_A		*sta	= Test_A_new();
		UniAttribute	* attr	= UniAttribute_new();;
		JMapWrapper	*m	= JMapWrapper_new("int32", sta->className);
		TarsOutputStream *first	= TarsOutputStream_new();
		TarsOutputStream *second = TarsOutputStream_new();

		/* 请使用流模式赋值 */
		for(i = 0; i < 3; i++)
		{
			TarsOutputStream_reset(first);
			TarsOutputStream_writeInt32(first, i, 0);
			sta->a		= i;
			sta->b->a	= i;
			sta->b->f	= i * 0.1;
			TarsOutputStream_reset(second);
			TarsOutputStream_writeStruct(second, sta, 1);
			JMapWrapper_put(m, TarsOutputStream_getBuffer(first), TarsOutputStream_getLength(first), TarsOutputStream_getBuffer(second), TarsOutputStream_getLength(second));
		}

		TUP_putMap(attr, "mymap", m);
		UniAttribute_encode(attr, &buff, &len);

		TarsOutputStream_del(&first);
		TarsOutputStream_del(&second);
		JMapWrapper_del(&m);
		UniAttribute_del(&attr);
		Test_A_del(&sta);
	}
	/* decode */
	{
		Test_A		*sta	= Test_A_new();
		UniAttribute	* attr	= UniAttribute_new();;
		JMapWrapper	*mRes	= JMapWrapper_new("int32", sta->className);
		TarsInputStream	*ais	= TarsInputStream_new();
		UniAttribute_decode(attr, buff, len);
		int ret = TUP_getMap(attr, "mymap", mRes);
		printf("ret:%d, map size:%d\n", ret, JMapWrapper_size(mRes));
		for(i = 0; i < JMapWrapper_size(mRes); ++i)
		{
			Int32 ai;
			TarsInputStream_setBuffer(ais, JArray_getPtr(mRes->first, i), JArray_getLength(mRes->first, i));
			TarsInputStream_readInt32(ais, &ai, 0, true);

			TarsInputStream_setBuffer(ais, JArray_getPtr(mRes->second, i), JArray_getLength(mRes->second, i));
			TarsInputStream_readStruct(ais, sta, 1, true);
			printf("map index:%d, first:%d, second:%s,%d,%s,%d,%f\n", i, ai, sta->className, sta->a, sta->b->className, sta->b->a, sta->b->f);
		}
		printf("\n");

		Test_A_del(&sta);
		TarsInputStream_del(&ais);
		JMapWrapper_del(&mRes);
		UniAttribute_del(&attr);
		TarsFree(buff);
		buff = NULL;
	}
}


int main(int argc, char *argv[])
{
	/* while(1) */
	{
		usleep(2);
		/*
		 * testJString();
		 * testJArray();
		 */

		//testStruct(); 
		testtuppack();

		/*
		 * testVector();
		 * testMap();
		 */

		/* testMapWithStruct(); */
	}

	return(0);
}


