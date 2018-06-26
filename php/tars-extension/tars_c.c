#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "./include/tars_c.h"

//����ֵ����
const Int32 TARS_SUCCESS        =  0;	  //�ɹ�
const Int32 TARS_ATTR_NOT_FOUND = -1;	  //���Ҳ����������
const Int32 TARS_ENCODE_ERROR   = -2;	  //�������
const Int32 TARS_DECODE_ERROR   = -3;	  //�������
const Int32 TARS_RUNTIME_ERROR  = -4;	  //��������ʱ����
const Int32 TARS_MALLOC_ERROR   = -5;	  //�ڴ�����ʧ�ܴ���
const Int32 TARS_DECODE_EOPNEXT = -6;	  //��ѡ�ֶβ�����

Int64 tars__bswap_constant_64(Int64 x)
{
#ifdef __MTK_64
	Int64 __t__;
	__t__.high = tars__bswap_constant_32(x.high);
	__t__.low  = tars__bswap_constant_32(x.low);
	return __t__;
#else
	return((((x) & 0xff00000000000000ull) >> 56)                      \
		   | (((x) & 0x00ff000000000000ull) >> 40)                     \
		   | (((x) & 0x0000ff0000000000ull) >> 24)                     \
		   | (((x) & 0x000000ff00000000ull) >> 8)                      \
		   | (((x) & 0x00000000ff000000ull) << 8)                      \
		   | (((x) & 0x0000000000ff0000ull) << 24)                     \
		   | (((x) & 0x000000000000ff00ull) << 40)                     \
		   | (((x) & 0x00000000000000ffull) << 56));
#endif
}

Int64 tars_ntohll(Int64 x)
{
	return tars__bswap_constant_64(x);
}

Float tars_ntohf(Float x)
{
	Int32 __t__ = tars__bswap_constant_32((*((Int32 *)&x)));
	return *((Float *) &__t__);
}

Double tars_ntohd(Double x)
{
	Int64 __t__ = tars__bswap_constant_64((*((Int64 *)&x)));
	return *((Double *) &__t__);
}
/////////////////////////////////////////////////////////////////////////////////////

void JString_del(JString **st)
{
	if (st == NULL || *st == NULL) return;
	if (*st != NULL && (*st)->_data) TarsFree((*st)->_data);
	if (*st != NULL) TarsFree(*st);
	*st = NULL;
}

void JString_copy(char * dest, const char * src, uint32_t len)
{
	char * pe = dest + len;
	if (dest == NULL || src == NULL || (Int32)len < 0)
		return;
	for (; dest != pe; ++dest, ++src)
		*dest = *src;
}

Int32 JString_copyChar(JString * s, char * data, uint32_t len)
{
	char * p = TarsMalloc(sizeof(char) * len + 1);
	if (!p)
		return TARS_MALLOC_ERROR;

	TarsFree(s->_data);

	s->_data = p;
	s->_len = len;
	s->_buf_len = len + 1;
	JString_copy(s->_data, data, len);

	return 0;
}

void JString_clear(JString *s)          { if (s != NULL) { s->_len = 0; s->_data[0] = 0;}}

/**
 * ����n���ȵ�JString�ռ�
 * @param  s [description]
 * @param  n [description]
 * @return   [description]
 */
Int32 JString_reserve(JString *s, uint32_t n)
{
	if (s == NULL || (Int32)n < 0)
		return TARS_MALLOC_ERROR;
	if (s->_buf_len < n + 1)
	{
		char * p = TarsMalloc(n + 1);

		if (!p)
			return TARS_MALLOC_ERROR;

		JString_copy(p, s->_data, s->_len + 1);
		TarsFree(s->_data);
		s->_data = p;
		s->_buf_len = n + 1;
	}
	return 0;
}

/**
 * Jstring���ڴ濽��
 * @param  s    [description]
 * @param  data [description]
 * @param  len  [description]
 * @return      [description]
 */
Int32 JString_assign(JString *s, const char * data, uint32_t len)
{
	Int32 ret=0;
	if (s == NULL || data == NULL || (Int32)len < 0)
		return TARS_MALLOC_ERROR;

	JString_clear(s);

	ret = JString_reserve(s, len);
	if (ret)
		return ret;

	JString_copy(s->_data, data, len);
	s->_len = len;
	s->_data[s->_len] = 0;

	return 0;
}

Int32 JString_append(JString * s, const char * data, uint32_t len)
{
	if (s == NULL || data == NULL || (Int32)len < 0)
		return TARS_MALLOC_ERROR;

	if (s->_len + len + 1 > s->_buf_len)
	{
		Int32 ret = JString_reserve(s, 2*(s->_len + len));
		if (ret)
			return ret;
	}

	JString_copy(s->_data + s->_len, data, len);
	s->_len += len;
	s->_data[s->_len] = 0;

	return TARS_SUCCESS;
}

Int32 JString_insert(JString *s, uint32_t pos, char v)
{
	if (s == NULL || (Int32)pos < 0)
		return TARS_MALLOC_ERROR;

	if (s->_len >= s->_buf_len)
	{
		Int32 ret = JString_reserve(s, s->_len * 2);
		if (ret)
			return ret;
	}
	JString_copy(s->_data + pos + 1, s->_data + pos, s->_len - pos);
	s->_data[pos] = v;
	++(s->_len);

	return TARS_SUCCESS;
}

uint32_t JString_size(JString *s)       { if (s) return s->_len; return 0;}
int JString_empty(JString *s)           { if (s) return s->_len == 0; return 0;}
uint32_t JString_capacity(JString *s)   { if (s) return s->_buf_len; return 0;}
char * JString_data(JString *s)         { if (s) return s->_data; return NULL;}
Int32 JString_resize(JString *s, uint32_t n)
{
	if (s == NULL || (Int32)n < 0)
		return TARS_MALLOC_ERROR;

	if (n > JString_size(s))
	{
		Int32 ret = JString_reserve(s, n);
		return ret;
	}
	return 0;
}

Int32 JString_init(JString *s)
{
	if (s == NULL)
		return TARS_MALLOC_ERROR;

	s->_data = TarsMalloc(1);
	if (!s->_data)
		return TARS_MALLOC_ERROR;

	s->_data[0] = 0;
	s->_len  = 0;
	s->_buf_len = 1;

	return TARS_SUCCESS;
}

JString * JString_new()
{
	Int32 ret;
	JString *s = TarsMalloc(sizeof(JString));
	if (!s)
		return NULL;

	ret = JString_init(s);
	if (ret)
	{
		TarsFree(s);
		return NULL;
	}

	return s;
}

void JArray_del(JArray ** arr)
{
	if (arr == NULL || *arr == NULL) return;
	if ((*arr)->elem_type_name != NULL)	TarsFree((*arr)->elem_type_name);
	if ((*arr)->list           != NULL)	TarsFree((*arr)->list);
	if ((*arr)->buff           != NULL)	TarsFree((*arr)->buff);

	TarsFree(*arr);
	*arr = NULL;
}

Int32 JArray_reserveList(JArray *arr, uint32_t num)
{
	if (arr == NULL || (Int32)num < 0)
		return TARS_MALLOC_ERROR;

	if (arr->list_len < num)
	{
		int * p = TarsMalloc(num);
		if (!p)
			return TARS_MALLOC_ERROR;

		memcpy(p, arr->list, arr->list_len);
		TarsFree(arr->list);
		arr->list = p;
		arr->list_len = num;
	}

	return 0;
}

Int32 JArray_reserveBuff(JArray *arr, uint32_t len)
{
	if (arr == NULL || (Int32)len < 0)
		return TARS_MALLOC_ERROR;

	if (arr->buff_len < len)
	{
		char * p = TarsMalloc(len);
		if (!p)
			return TARS_MALLOC_ERROR;

		memcpy(p, arr->buff, arr->buff_len);
		TarsFree(arr->buff);
		arr->buff = p;
		arr->buff_len = len;
	}
	return 0;
}

Int32 JArray_pushBack(JArray *arr, const char * data, uint32_t len)
{
	Int32 ret;
	if (arr == NULL || data == NULL || (Int32)len < 0)
		return TARS_MALLOC_ERROR;

	++ arr->elem_num;

	if (arr->list_len <= arr->elem_num * sizeof(int))
	{
		ret = JArray_reserveList(arr, 2* (arr->list_len + sizeof(int)) );
		if (ret)
			return ret;
	}
	if (arr->buff_len < arr->buff_used + len)
	{
		ret = JArray_reserveBuff(arr, 2*(arr->buff_len + len));
		if (ret)
			return ret;
	}

	arr->list[arr->elem_num - 1] = (int)(arr->buff_used);
	memcpy(arr->buff + arr->buff_used, data, len);
	arr->buff_used += len;

	arr->list[arr->elem_num] = (int)(arr->buff_used);

	return 0;
}

Int32 JArray_pushBackString(JArray *arr, const char * data)
{
	return JArray_pushBack(arr, data, strlen(data));
}

Int32 JArray_get(JArray *arr, unsigned index, char *data, uint32_t * len)
{
	unsigned data_len = 0;
	if (arr == NULL || (Int32)index < 0 || data == NULL || len == NULL)
		return TARS_MALLOC_ERROR;

	if (index > arr->elem_num - 1)
	{
		*len = 0;
		return -1;
	}

	data_len = arr->list[index + 1] - arr->list[index];
	if (*len < data_len)
	{
		*len = 0;
		return -2;
	}
	*len = data_len;
	memcpy(data, arr->buff + arr->list[index], *len);

	return 0;
}

char *JArray_getPtr(JArray *arr, unsigned index)
{
	if (arr == NULL || (Int32)index < 0)
		return NULL;

	if (index > arr->elem_num - 1)
	{
		return NULL;
	}

	return arr->buff + arr->list[index];
}


int JArray_getLength(JArray *arr, unsigned index)
{
	if (arr == NULL || (Int32)index < 0)
		return 0;

	if (index > arr->elem_num - 1)
	{
		return 0;
	}

	return arr->list[index + 1] - arr->list[index];
}


int JArray_size(JArray *arr)
{
	if (arr == NULL)
		return 0;
	return arr->elem_num;
}

int JArray_empty(JArray *arr)
{
	if (arr == NULL)
		return 0;
	return JArray_size(arr) == 0;
}

void JArray_clear(JArray *arr)
{
	if (arr == NULL)
		return;
	arr->elem_num = 0;
	arr->buff_used = 0;
}

void JArray_init(JArray *arr)
{
/*
	arr->elem_type_name = NULL;
	arr->elem_num       = 0;
	arr->list_len       = 0;
	arr->list           = NULL;
	arr->buff_used      = 0;
	arr->buff_len       = 0;
	arr->buff           = NULL;
  */
	memset(arr, 0, sizeof(JArray));
}

JArray * JArray_new(const char * type)
{
	unsigned len = 0;
	JArray * arr = TarsMalloc(sizeof(JArray));
	if (!arr)
		return NULL;

	JArray_init(arr);

	len = strlen(type);
	arr->elem_type_name = TarsMalloc(len+1);
	if (!arr->elem_type_name)
	{
		TarsFree(arr);
		return NULL;
	}

	memcpy(arr->elem_type_name, type, len+1);

	return arr;
}

void JMapWrapper_del(JMapWrapper ** m)
{
	if (m == NULL || *m == NULL) return;
	JArray_del(&((*m)->first));
	JArray_del(&((*m)->second));

	TarsFree(*m);
	*m = NULL;
}
/**
 * �ֱ��key��val���ַ���ѹ��JString��
 * @param  m          [description]
 * @param  first_data [description]
 * @param  first_len  [description]
 * @param  value_data [description]
 * @param  value_len  [description]
 * @return            [description]
 */
Int32 JMapWrapper_put(JMapWrapper *m, const char * first_data, unsigned first_len, const char *value_data, unsigned value_len)
{
	Int32 ret = 0;
	ret = JArray_pushBack(m->first, first_data, first_len);
	if (ret)
		return ret;

	return JArray_pushBack(m->second, value_data, value_len);
}

int JMapWrapper_find(JMapWrapper *m, const char * first_data, unsigned first_len, char **value_data, unsigned * value_len)
{
	uint32_t i;

	for (i = 0; i< JMapWrapper_size(m); ++i)
	{
		if (first_len == JArray_getLength(m->first, i)
			&& memcmp(JArray_getPtr(m->first, i), first_data, first_len) == 0)
		{
			*value_len = JArray_getLength(m->second, i);
			*value_data =  JArray_getPtr(m->second, i);
			return 0;
		}
	}

	return TARS_ATTR_NOT_FOUND;
}


int JMapWrapper_size(JMapWrapper * m)
{
	return JArray_size(m->first);
}

int JMapWrapper_getByIndex(JMapWrapper *m, unsigned index, char *first, uint32_t * first_len, char* second, uint32_t * second_len)
{
	JArray_get(m->first, index, first, first_len);
	JArray_get(m->second, index, second, second_len);
	return 0;
}

int JMapWrapper_empty(JMapWrapper *m)
{
	return JArray_empty(m->first) && JArray_empty(m->second);
}

void JMapWrapper_clear(JMapWrapper *m)
{
	JArray_clear(m->first);
	JArray_clear(m->second);
}

void JMapWrapper_init(JMapWrapper *m)
{
}

JMapWrapper * JMapWrapper_new(const char * first_type, const char * second_type)
{
	JMapWrapper * m = TarsMalloc(sizeof(JMapWrapper));
	if (!m)
		return NULL;

	JMapWrapper_init(m);

	m->first = JArray_new(first_type);
	if (!m->first)
	{
		TarsFree(m);
		return NULL;
	}
	m->second = JArray_new(second_type);
	if (!m->second)
	{
		JArray_del(&m->first);
		TarsFree(m);
		return NULL;
	}

	return m;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t helper_getTag(helper *h)
{
	return h->type_tag >> 4;
}

void helper_setTag(helper *h, unsigned int t)
{
	t <<= 4;
	h->type_tag &= 0x0F;
	h->type_tag |= t;
}

uint8_t helper_getType(helper *h)
{
	return h->type_tag & 0x0F;
}

void helper_setType(helper *h, unsigned int t)
{
	h->type_tag &= 0xF0;
	h->type_tag |= (t & 0x0F);
}

void DataHead_del(DataHead ** head)
{
	if (head == NULL || *head == NULL) return;
	TarsFree(*head);
	*head = NULL;
}


uint8_t DataHead_getTag(DataHead * head)              { return head->_tag;}
//void DataHead_setTag(DataHead * head, uint8_t t)      { head->_tag = t; }
uint8_t DataHead_getType(DataHead * head)             { return head->_type;}
//void DataHead_setType(DataHead * head, uint8_t t)     { head->_type = t; }

/// ��ȡͷ��Ϣ������ǰ������ƫ����
Int32 DataHead_peekFrom(DataHead * head, TarsInputStream* is, uint32_t *n)
{
	Int32 ret = 0;
	helper h;
	*n = sizeof(h);
	ret = TarsInputStream_peekBuf(is, &h, sizeof(h), 0);
	if (TARS_SUCCESS != ret)	return ret;

	head->_type = helper_getType(&h);
	if (helper_getTag(&h) == 15)
	{
		ret = TarsInputStream_peekBuf(is, &head->_tag, sizeof(head->_tag), sizeof(h));
		if (TARS_SUCCESS != ret)	return ret;

		*n += sizeof(head->_tag);
	}
	else
	{
		head->_tag = helper_getTag(&h);
	}
	return TARS_SUCCESS;
}

/// ��ȡ����ͷ��Ϣ
Int32 DataHead_readFrom(DataHead * head, TarsInputStream* is)
{
	Int32 ret = 0;
	uint32_t n;

	ret = DataHead_peekFrom(head, is, &n);
	if (TARS_SUCCESS != ret)	return ret;

	ret = TarsInputStream_skip(is, n);
	return ret;
}


/// д������ͷ��Ϣ
Int32 DataHead_writeTo(DataHead * head, TarsOutputStream* os)
{
	Int32 ret;
	helper h;
	helper_setType(&h, head->_type);
	if (head->_tag < 15)
	{
		helper_setTag(&h, head->_tag);
		return TarsOutputStream_writeBuf(os, &h, sizeof(h));
	}
	else
	{
		helper_setTag(&h, 15);
		ret = TarsOutputStream_writeBuf(os, &h, sizeof(h));
		if (TARS_SUCCESS != ret)	return ret;
		return TarsOutputStream_writeBuf(os, &head->_tag, sizeof(head->_tag));
	}
	return TARS_SUCCESS;
}
Int32 DataHead_setAndWriteTo(DataHead * head, unsigned int type, unsigned int tag,TarsOutputStream* os)
{
	head->_tag = tag;
	head->_type = type;
	return DataHead_writeTo(head, os);
}


void DataHead_init(DataHead * head)
{
	head->_type = 0;
	head->_tag  = 0;
}

DataHead * DataHead_new()
{
	DataHead * head = TarsMalloc(sizeof(DataHead));
	if (!head)
	{
		return NULL;
	}
	DataHead_init(head);
	return head;
}
////////////////////////////////////////////////////////////////////////////
static void TarsStream_del(TarsStream ** js)
{
	if (js == NULL || *js == NULL) return;
	JString_del(& (*js)->_buf);
	DataHead_del(& (*js)->_h);

	TarsFree(*js);
	*js = NULL;
}

static Int32 TarsStream_init(TarsStream* js)
{
	js->_buf = JString_new();
	if (!js->_buf)
		return TARS_MALLOC_ERROR;

	js->_h   = DataHead_new();
	if (!js->_h)
	{
		JString_del(&js->_buf);
		return TARS_MALLOC_ERROR;
	}

	js->_cur = 0;
	js->_err[0] = 0;

	return TARS_SUCCESS;
}

static TarsStream * TarsStream_new()
{
	Int32 ret;
	TarsStream * js = TarsMalloc(sizeof(TarsStream));
	if (!js)
	{
		return NULL;
	}

	ret = TarsStream_init(js);
	if (ret)
	{
		TarsFree(js);
		return NULL;
	}

	return js;
}

////////////////////////////////////////////////////////////////////////////

void TarsInputStream_del(TarsInputStream ** is)
{
	TarsStream_del(is);
}

void TarsInputStream_reset(TarsInputStream * is)
{
	is->_cur = 0;
}

/// ��ȡ����
Int32 TarsInputStream_readBuf(TarsInputStream * is, void * buf, uint32_t len)
{
	Int32 ret;

	ret = TarsInputStream_peekBuf(is, buf, len, 0);
	if (TARS_SUCCESS != ret)	return ret;

	is->_cur += len;
	return TARS_SUCCESS;
}

/// ��ȡ���棬�����ı�ƫ����
Int32 TarsInputStream_peekBuf(TarsInputStream * is, void * buf, uint32_t len, uint32_t offset)
{
	if (is->_cur + offset + len > JString_size(is->_buf))
	{
		snprintf(is->_err, sizeof(is->_err), "buffer overflow when peekBuf, over %u.", (uint32_t)JString_size(is->_buf));
		return TARS_DECODE_ERROR;
	}
	memcpy(buf, JString_data(is->_buf) + is->_cur + offset, len);
	return TARS_SUCCESS;
}



/// ����len���ֽ�
Int32 TarsInputStream_skip(TarsInputStream * is, uint32_t len)
{
	is->_cur += len;
	return TARS_SUCCESS;
}

/// ���û���
Int32 TarsInputStream_setBuffer(TarsInputStream * is, const char * buf, uint32_t len)
{
	Int32 ret = JString_assign(is->_buf, buf, len);
	if (ret)
		return ret;

	is->_cur = 0;
	return TARS_SUCCESS;
}


/// ����ָ����ǩ��Ԫ��ǰ
Int32 TarsInputStream_skipToTag(TarsInputStream * is, uint8_t tag)
{
	Int32 ret;
	while (1)
	{
		uint32_t len;
		ret = DataHead_peekFrom(is->_h, is, &len);
		if (TARS_SUCCESS != ret)	return ret;

		if (tag <= DataHead_getTag(is->_h) || DataHead_getType(is->_h) == eStructEnd)
		{
			if (tag != DataHead_getTag(is->_h))
			{
				snprintf(is->_err, sizeof(is->_err), "tag %d not found", DataHead_getTag(is->_h));
				return TARS_DECODE_ERROR;
			}
			return TARS_SUCCESS;
		}
		ret = TarsInputStream_skip(is, len);
		if (TARS_SUCCESS != ret)	return ret;

		ret = TarsInputStream_skipFieldByType(is, DataHead_getType(is->_h));
		if (TARS_SUCCESS != ret)	return ret;
	}
	snprintf(is->_err, sizeof(is->_err), "tag %d not found", DataHead_getTag(is->_h));
	return TARS_DECODE_ERROR;
}

/// ������ǰ�ṹ�Ľ���
Int32 TarsInputStream_skipToStructEnd(TarsInputStream * is)
{
	Int32 ret;
	Int32 level = 1; //�жϽṹǶ�׵����
	do
	{
		ret = DataHead_readFrom(is->_h, is);
		if (TARS_SUCCESS != ret)	return ret;

		if (DataHead_getType(is->_h) == eStructBegin)
			level++;
		else if (DataHead_getType(is->_h) == eStructEnd)
			level--;
		else
		{
			ret = TarsInputStream_skipFieldByType(is, DataHead_getType(is->_h));
			if (TARS_SUCCESS != ret)	return ret;
		}
	}while (DataHead_getType(is->_h) != eStructEnd || level != 0);

	return TARS_SUCCESS;
}

/// ����һ���ֶ�
Int32 TarsInputStream_skipField(TarsInputStream * is)
{
	Int32 ret;

	ret = DataHead_readFrom(is->_h, is);
	if (TARS_SUCCESS != ret)	return ret;

	ret = TarsInputStream_skipFieldByType(is, DataHead_getType(is->_h));
	if (TARS_SUCCESS != ret)	return ret;

	return TARS_SUCCESS;
}

/// ����һ���ֶΣ�������ͷ��Ϣ
Int32 TarsInputStream_skipFieldByType(TarsInputStream * is, uint8_t type)
{
	Int32 ret;
	uint32_t len = 0;

	switch (type)
	{
	case eChar:
		TarsInputStream_skip(is, sizeof(Char));
		break;
	case eShort:
		TarsInputStream_skip(is, sizeof(Short));
		break;
	case eInt32:
		TarsInputStream_skip(is, sizeof(Int32));
		break;
	case eInt64:
		TarsInputStream_skip(is, sizeof(Int64));
		break;
	case eFloat:
		TarsInputStream_skip(is, sizeof(Float));
		break;
	case eDouble:
		TarsInputStream_skip(is, sizeof(Double));
		break;
	case eString1:
		{
			uint8_t n;
			ret = TarsInputStream_readBuf(is, &n, sizeof(n));
			if (TARS_SUCCESS != ret)	return ret;

			len = n;
			TarsInputStream_skip(is, len);
		}
		break;
	case eString4:
		{
			uint32_t n;
			ret = TarsInputStream_readBuf(is, &n, sizeof(n));
			if (TARS_SUCCESS != ret)	return ret;

			len = tars_ntohl(n);
			TarsInputStream_skip(is, len);
		}
		break;
	case eMap:
		{
			Int32 size, i;
			ret = TarsInputStream_readInt32(is, &size, 0, 1);
			if (TARS_SUCCESS != ret)	return ret;

			for (i = 0; i < size * 2; ++i)
			{
				ret = TarsInputStream_skipField(is);
				if (TARS_SUCCESS != ret)	return ret;
			}
		}
		break;
	case eList:
		{
			Int32 size, i;
			ret = TarsInputStream_readInt32(is, &size, 0, 1);
			if (TARS_SUCCESS != ret)	return ret;

			for (i = 0; i < size; ++i)
			{
				ret = TarsInputStream_skipField(is);
				if (TARS_SUCCESS != ret)	return ret;
			}
		}
		break;
	case eSimpleList:
		{
			Int32 size;

			ret = DataHead_readFrom(is->_h, is);
			if (TARS_SUCCESS != ret)	return ret;

			if (DataHead_getType(is->_h) != eChar)
			{

				snprintf(is->_err, sizeof(is->_err), "skipField with invalid type, type value: %d, %d.", type, DataHead_getType(is->_h));
				return TARS_DECODE_ERROR;
			}
			ret = TarsInputStream_readInt32(is, &size, 0, 1);
			if (TARS_SUCCESS != ret)	return ret;

			ret = TarsInputStream_skip(is, size);
			if (TARS_SUCCESS != ret)	return ret;
		}
		break;
	case eStructBegin:
		ret = TarsInputStream_skipToStructEnd(is);
		if (TARS_SUCCESS != ret)	return ret;
		break;
	case eStructEnd:
	case eZeroTag:
		break;
	default:
		{
			snprintf(is->_err, sizeof(is->_err), "skipField with invalid type, type value:%d.", type);
			return TARS_DECODE_ERROR;
		}
	}
	return TARS_SUCCESS;
}

Int32 TarsInputStream_checkValid(TarsInputStream * is,uint8_t tag, Bool isRequire)
{
	if (!TarsInputStream_skipToTag(is, tag))
	{
		Int32 ret = DataHead_readFrom(is->_h, is);
		return ret;
	}
	else if (isRequire)
	{
		snprintf(is->_err, sizeof(is->_err), "require field not exist, tag: %d", tag);
		return TARS_DECODE_ERROR;
	}
	return TARS_DECODE_EOPNEXT;
}

Int32 TarsInputStream_readByChar(TarsInputStream * is, Char * n)
{
	Int32 ret;
	ret = TarsInputStream_readBuf(is, n, sizeof(*n));
	return ret;
}
Int32 TarsInputStream_readByShort(TarsInputStream * is, Short * n)
{
	Int32 ret;
	ret = TarsInputStream_readBuf(is, n, sizeof(*n));
	return ret;
}


Int32 TarsInputStream_readByInt32(TarsInputStream * is, Int32 * n)
{
	Int32 ret;
	ret = TarsInputStream_readBuf(is, n, sizeof(*n));
	return ret;
}

Int32 TarsInputStream_readBool(TarsInputStream * is, Bool* b, uint8_t tag, Bool isRequire)
{
	Int32 ret;
	Char c = *b;
	ret = TarsInputStream_readChar(is, &c, tag, isRequire);
	*b = c ? 1 : 0;

	return ret;
}

Int32 TarsInputStream_readChar(TarsInputStream * is, Char* c, uint8_t tag, Bool isRequire)
{
	Int32 ret = TarsInputStream_checkValid(is,tag,isRequire);
	if (TARS_DECODE_EOPNEXT == ret) return TARS_DECODE_EOPNEXT;
	if (TARS_SUCCESS != ret)	return ret;

	switch (DataHead_getType(is->_h))
	{
	case eZeroTag:
		*c = 0;
		break;
	case eChar:
		ret = TarsInputStream_readBuf(is, c, sizeof(*c));
		if (TARS_SUCCESS != ret)	return ret;

		break;
	default:
		{
			snprintf(is->_err, sizeof(is->_err), "read 'Char' type mismatch, tag: %d, get type: %d.", tag, DataHead_getType(is->_h));
			return TARS_DECODE_ERROR;
		}
	}
	return TARS_SUCCESS;
}

Int32 TarsInputStream_readUInt8	(TarsInputStream * is, UInt8 * n, uint8_t tag, Bool isRequire)
{
	Short ns = 0;
	Int32 ret = TarsInputStream_readShort(is, &ns, tag, isRequire);
	if (ret != TARS_SUCCESS) return ret;
	*n = (UInt8)ns;

	return TARS_SUCCESS;
}

Int32 TarsInputStream_readShort(TarsInputStream * is, Short* n, uint8_t tag, Bool isRequire)
{
	Char c;
	Int32 ret = TarsInputStream_checkValid(is,tag,isRequire);
	if (TARS_DECODE_EOPNEXT == ret) return TARS_DECODE_EOPNEXT;
	if (TARS_SUCCESS != ret)	return ret;

	switch (DataHead_getType(is->_h))
	{
	case eZeroTag:
		*n = 0;
		break;
	case eChar:
		ret = TarsInputStream_readByChar(is, &c);
		if (TARS_SUCCESS != ret)	return ret;
		*n = c;

		break;
	case eShort:
		ret = TarsInputStream_readBuf(is, n, sizeof(*n));
		if (TARS_SUCCESS != ret)	return ret;
		*n = tars_ntohs(*n);
		break;
	default:
		{
			snprintf(is->_err, sizeof(is->_err), "read 'Short' type mismatch, tag: %d, get type: %d.", tag, DataHead_getType(is->_h));
			return TARS_DECODE_ERROR;
		}
	}
	return TARS_SUCCESS;
}

Int32 TarsInputStream_readUInt16	(TarsInputStream *is, UInt16 *n, uint8_t tag, Bool isRequire)
{
	Int32 ns = 0;
	Int32 ret = TarsInputStream_readInt32(is, &ns, tag, isRequire);
	if (ret != TARS_SUCCESS) return ret;
	*n = (UInt16)ns;

	return TARS_SUCCESS;
}

Int32 TarsInputStream_readInt32(TarsInputStream * is, Int32* n, uint8_t tag, Bool isRequire)
{
	Char c;
	Short sh;
	Int32 ret = TarsInputStream_checkValid(is,tag,isRequire);
	if (TARS_DECODE_EOPNEXT == ret) return TARS_DECODE_EOPNEXT;
	if (TARS_SUCCESS != ret)	return ret;

	switch (DataHead_getType(is->_h))
	{
	case eZeroTag:
		*n = 0;
		break;
	case eChar:
		ret = TarsInputStream_readByChar(is, &c);
		if (TARS_SUCCESS != ret)	return ret;
		*n = c;

		break;
	case eShort:
		ret = TarsInputStream_readByShort(is, &sh);
		if (TARS_SUCCESS != ret)	return ret;
		*n = (Short) tars_ntohs(sh);
		break;
	case eInt32:
		ret = TarsInputStream_readBuf(is, n, sizeof(*n));
		if (TARS_SUCCESS != ret)	return ret;
		*n = tars_ntohl(*n);
		break;
	default:
		{
			snprintf(is->_err, sizeof(is->_err), "read 'Int32' type mismatch, tag: %d, get type: %d.", tag, DataHead_getType(is->_h));
			return TARS_DECODE_ERROR;
		}
	}
	return TARS_SUCCESS;

}

Int32 TarsInputStream_readUInt32	(TarsInputStream *is, UInt32 * n, uint8_t tag, Bool isRequire)
{
	Int64 ns = 0;
	Int32 ret = TarsInputStream_readInt64(is, &ns, tag, isRequire);
	if (ret != TARS_SUCCESS) return ret;
	*n = (UInt32)ns;

	return TARS_SUCCESS;
}

Int32 TarsInputStream_readInt64(TarsInputStream * is, Int64* n, uint8_t tag, Bool isRequire)
{

	Char c;
	Short sh;
	Int32 i32;
	Int32 ret = TarsInputStream_checkValid(is,tag,isRequire);
	if (TARS_DECODE_EOPNEXT == ret) return TARS_DECODE_EOPNEXT;
	if (TARS_SUCCESS != ret)	return ret;
#ifdef __MTK_64
	Int64 i64;
	i64.high = 0;
	i64.low = 0;
	switch (DataHead_getType(is->_h))
	{
	case eZeroTag:
		*n = i64;
		break;
	case eChar:
		ret = TarsInputStream_readByChar(is, &c);
		if (TARS_SUCCESS != ret)	return ret;
		i64.low = c;
		*n = i64;
		break;
	case eShort:
		ret = TarsInputStream_readByShort(is, &sh);
		if (TARS_SUCCESS != ret)	return ret;
		i64.low = (Short) tars_ntohs(sh);
		*n = i64;
		break;
	case eInt32:
		ret = (Int32) TarsInputStream_readByInt32(is, &i32);
		if (TARS_SUCCESS != ret)	return ret;
		i64.low = (Int32) tars_ntohl(i32);
		*n = i64;
		break;
	case eInt64:
		ret = TarsInputStream_readBuf(is, n, sizeof(*n));
		if (TARS_SUCCESS != ret)	return ret;
		*n = tars_ntohll(*n);
		break;
#else

	switch (DataHead_getType(is->_h))
	{
	case eZeroTag:
		*n = 0;
		break;
	case eChar:
		ret = TarsInputStream_readByChar(is, &c);
		if (TARS_SUCCESS != ret)	return ret;
		*n = c;
		break;
	case eShort:
		ret = TarsInputStream_readByShort(is, &sh);
		if (TARS_SUCCESS != ret)	return ret;
		*n = (Short) tars_ntohs(sh);
		break;
	case eInt32:
		ret = (Int32) TarsInputStream_readByInt32(is, &i32);
		if (TARS_SUCCESS != ret)	return ret;
		*n = (Int32) tars_ntohl(i32);
		break;
	case eInt64:
		ret = TarsInputStream_readBuf(is, n, sizeof(*n));
		if (TARS_SUCCESS != ret)	return ret;
		*n = tars_ntohll(*n);
		break;
#endif
	default:
		{
			snprintf(is->_err, sizeof(is->_err), "read 'Int64' type mismatch, tag: %d, get type: %d.", tag, DataHead_getType(is->_h));
			return TARS_DECODE_ERROR;
		}
	}
	return TARS_SUCCESS;

}
/*
Int32 TarsInputStream_readInt64_HL(TarsInputStream * is, Int32* high, Int32* low, uint8_t tag, Bool isRequire)
{
	Int64 n = 0;
	TarsInputStream_readInt64(is, &n, tag,isRequire );
	*low     = (Int32)n;
	*high    = (Int32)(n>>32);
}
*/

Int32 TarsInputStream_readFloat(TarsInputStream * is, Float* n, uint8_t tag, Bool isRequire)
{
	Int32 ret = TarsInputStream_checkValid(is,tag,isRequire);
	if (TARS_DECODE_EOPNEXT == ret) return TARS_DECODE_EOPNEXT;
	if (TARS_SUCCESS != ret)	return ret;

	switch (DataHead_getType(is->_h))
	{
	case eZeroTag:
		*n = 0;
		break;
	case eFloat:
		ret = TarsInputStream_readBuf(is, n, sizeof(*n));
		if (TARS_SUCCESS != ret)	return ret;
		*n = tars_ntohf(*n);
		break;
	default:
		{
			snprintf(is->_err, sizeof(is->_err), "read 'Float' type mismatch, tag: %d, get type: %d.", tag, DataHead_getType(is->_h));
			return TARS_DECODE_ERROR;
		}
	}
	return TARS_SUCCESS;

}

Int32 TarsInputStream_readDouble(TarsInputStream * is, Double* n, uint8_t tag, Bool isRequire)
{
	Int32 ret = TarsInputStream_checkValid(is,tag,isRequire);
	if (TARS_DECODE_EOPNEXT == ret) return TARS_DECODE_EOPNEXT;
	if (TARS_SUCCESS != ret)	return ret;

	switch (DataHead_getType(is->_h))
	{
	case eZeroTag:
		*n = 0;
		break;
	case eFloat:
		{
			Float n1;
			ret = TarsInputStream_readBuf(is, &n1, sizeof(n1));
			if (TARS_SUCCESS != ret)	return ret;

			*n = n1;
			break;
		}
	case eDouble:
		ret = TarsInputStream_readBuf(is, n, sizeof(*n));
		if (TARS_SUCCESS != ret)	return ret;
		*n = tars_ntohd(*n);
		break;
	default:
		{
			snprintf(is->_err, sizeof(is->_err), "read 'Double' type mismatch, tag: %d, get type: %d.", tag, DataHead_getType(is->_h));
			return TARS_DECODE_ERROR;
		}
	}
	return TARS_SUCCESS;
}

Int32 TarsInputStream_readString(TarsInputStream * is, JString* s, uint8_t tag, Bool isRequire)
{
	Int32 ret = TarsInputStream_checkValid(is,tag,isRequire);
	if (TARS_DECODE_EOPNEXT == ret) return TARS_DECODE_EOPNEXT;
	if (TARS_SUCCESS != ret)	return ret;

	switch (DataHead_getType(is->_h))
	{
	case eString1:
		{
			uint8_t n;
			uint32_t len;
			char ss[256];
			ret = TarsInputStream_readBuf(is, &n, sizeof(n));
			if (TARS_SUCCESS != ret)	return ret;
			len = n;
			ret = TarsInputStream_readBuf(is, ss, len);
			if (TARS_SUCCESS != ret)	return ret;

			ret = JString_assign(s, ss, len);
			if (TARS_SUCCESS != ret)	return ret;
		}
		break;
	case eString4:
		{
			uint32_t len;
			char *ss;
			ret = TarsInputStream_readBuf(is, &len, sizeof(len));
			if (TARS_SUCCESS != ret)	return ret;
			len = tars_ntohl(len);
			if (len > TARS_MAX_STRING_LENGTH)
			{
				snprintf(is->_err, sizeof(is->_err), "invalid string size, tag: %d, size: %d", tag, len);
				return TARS_DECODE_ERROR;
			}
			ss = TarsMalloc(sizeof(Char)*len);
			if (!ss)
				return TARS_MALLOC_ERROR;

			ret = TarsInputStream_readBuf(is, ss, len);
			if (TARS_SUCCESS != ret)
			{
				TarsFree(ss);
				return ret;
			}

			ret = JString_assign(s, ss, len);

			TarsFree(ss);
			return ret;
		}
		break;
	default:
		{
			snprintf(is->_err, sizeof(is->_err), "read 'string' type mismatch, tag: %d, get type: %d.", tag, DataHead_getType(is->_h));
			return TARS_DECODE_ERROR;
		}
	}
	return TARS_SUCCESS;
}

Int32 TarsInputStream_readMap(TarsInputStream * is, JMapWrapper* m, uint8_t tag, Bool isRequire)
{
	Int32 ret = TarsInputStream_checkValid(is,tag,isRequire);
	if (TARS_DECODE_EOPNEXT == ret) return TARS_DECODE_EOPNEXT;
	if (TARS_SUCCESS != ret)	return ret;

	switch (DataHead_getType(is->_h))
	{
	case eMap:
		{
			Int32 size, i;
			ret = TarsInputStream_readInt32(is, &size, 0, true);
			if (TARS_SUCCESS != ret)	return ret;

			if (size < 0)
			{
				snprintf(is->_err, sizeof(is->_err), "invalid map, tag: %d, size: %d", tag, size);
				return TARS_DECODE_ERROR;
			}
			for (i = 0; i < size; ++i)
			{

				uint32_t p1, p2, p3;
				p1 = is->_cur;
				ret = TarsInputStream_skipField(is);
				if (TARS_SUCCESS != ret)	return ret;
				p2 = is->_cur;
				ret = TarsInputStream_skipField(is);
				if (TARS_SUCCESS != ret)	return ret;
				p3 = is->_cur;

				ret = JMapWrapper_put(m, JString_data(is->_buf)+p1, p2-p1, JString_data(is->_buf)+p2, p3-p2);
				if (TARS_SUCCESS != ret)	return ret;
			}
		}
		break;
	default:
		{
			snprintf(is->_err, sizeof(is->_err), "read 'map' type mismatch, tag: %d, get type: %d.", tag, DataHead_getType(is->_h));
			return TARS_DECODE_ERROR;
		}
	}
	return TARS_SUCCESS;
}

Int32 TarsInputStream_readVectorChar(TarsInputStream * is, JString *v, uint8_t tag, Bool isRequire)
{
	Int32 ret = TarsInputStream_checkValid(is,tag,isRequire);
	if (TARS_DECODE_EOPNEXT == ret) return TARS_DECODE_EOPNEXT;
	if (TARS_SUCCESS != ret)	return ret;

	switch (DataHead_getType(is->_h))
	{
	case eSimpleList:
		{
			Int32 size;
			DataHead *hh = DataHead_new();
			if (!hh) return TARS_MALLOC_ERROR;

			ret = DataHead_readFrom(hh, is);
			if (TARS_SUCCESS != ret)
			{
				DataHead_del(&hh);
				return ret;
			}

			if (DataHead_getType(hh) != eChar)
			{
				snprintf(is->_err, sizeof(is->_err), "type mismatch, tag: %d, type: %d, %d", tag, DataHead_getType(is->_h), DataHead_getType(hh));
				DataHead_del(&hh);
				return TARS_DECODE_ERROR;
			}

			ret = TarsInputStream_readInt32(is, &size, 0, true);
			if (TARS_SUCCESS != ret)
			{
				DataHead_del(&hh);
				return ret;
			}
			if (size < 0)
			{
				snprintf(is->_err, sizeof(is->_err), "invalid size, tag: %d, type: %d, %d, size: %d", tag, DataHead_getType(is->_h), DataHead_getType(hh), size);
				DataHead_del(&hh);
				return TARS_DECODE_ERROR;
			}

			DataHead_del(&hh);
			ret = JString_resize(v, size);
			if (TARS_SUCCESS != ret)	return ret;

			ret = TarsInputStream_readBuf(is, JString_data(v), size);
			if (TARS_SUCCESS != ret)	return ret;

			v->_len = size;

		}
		break;
	default:
		{
			snprintf(is->_err, sizeof(is->_err), "type mismatch, tag: %d, type: %d", tag, DataHead_getType(is->_h));
			return TARS_DECODE_ERROR;
		}
	}
	return TARS_SUCCESS;
}

Int32 TarsInputStream_readVector(TarsInputStream * is, JArray* v, uint8_t tag, Bool isRequire)
{
	Int32 ret = TarsInputStream_checkValid(is,tag,isRequire);
	if (TARS_DECODE_EOPNEXT == ret) return TARS_DECODE_EOPNEXT;
	if (TARS_SUCCESS != ret)	return ret;

	switch (DataHead_getType(is->_h))
	{
	case eList:
		{
			Int32 size, i;
			ret = TarsInputStream_readInt32(is, &size, 0, true);
			if (TARS_SUCCESS != ret)	return ret;

			if (size < 0)
			{
				snprintf(is->_err, sizeof(is->_err), "invalid size, tag: %d, type: %d, size: %d", tag, DataHead_getType(is->_h), size);
				return TARS_DECODE_ERROR;
			}
			for (i = 0; i < size; ++i)
			{
				uint32_t p1, p2;
				p1 = is->_cur;
				ret = TarsInputStream_skipField(is);
				if (TARS_SUCCESS != ret)	return ret;

				p2 = is->_cur;

				ret = JArray_pushBack(v, JString_data(is->_buf)+p1, p2-p1);
				if (TARS_SUCCESS != ret)	return ret;
			}
		}
		break;
	default:
		{
			snprintf(is->_err, sizeof(is->_err), "read 'vector' type mismatch, tag: %d, get type: %d.", tag, DataHead_getType(is->_h));
			return TARS_DECODE_ERROR;
		}
	}
	return TARS_SUCCESS;
}


/// ��ȡ�ṹ
Int32 TarsInputStream_readStruct(TarsInputStream * is, void * st, uint8_t tag, Bool isRequire)
{
	Int32 ret=0;
	const JStructBase* jst = st;
	JString *s = JString_new();
	TarsInputStream *i = TarsInputStream_new();
	do
	{
		if (!s || !i)
		{
			ret = TARS_MALLOC_ERROR; break;
		}
		ret = TarsInputStream_readStructString(is, s, tag, isRequire);
		if ( TARS_SUCCESS != ret ) break;
		ret= TarsInputStream_setBuffer(i, JString_data(s), JString_size(s));
		if ( TARS_SUCCESS != ret ) break;
		ret = jst->readFrom(st, i);
	}while (0);
	if (s) JString_del(&s);
	if (i) TarsInputStream_del(&i);
	return ret;
}

Int32 TarsInputStream_readStructString(TarsInputStream * is, JString * st, uint8_t tag, Bool isRequire)
{
	uint32_t pos1,pos2;
	Int32 ret = TarsInputStream_checkValid(is,tag,isRequire);
	if (TARS_DECODE_EOPNEXT == ret) return TARS_DECODE_EOPNEXT;
	if (TARS_SUCCESS != ret)	return ret;

	if (DataHead_getType(is->_h) != eStructBegin)
	{
		snprintf(is->_err, sizeof(is->_err), "read 'struct' type mismatch, tag: %d, get type: %d.", tag, DataHead_getType(is->_h));
		return TARS_DECODE_ERROR;
	}
	pos1 = is->_cur;
	ret = TarsInputStream_skipToStructEnd(is);
	if (TARS_SUCCESS != ret)	return ret;
	pos2 = is->_cur;

	ret = JString_assign(st, (const char*)(JString_data(is->_buf) + pos1), pos2-pos1-1);
	if (TARS_SUCCESS != ret)	return ret;

	return TARS_SUCCESS;
}


Int32 TarsInputStream_init(TarsInputStream* is)
{
	return TarsStream_init(is);
}

TarsInputStream * TarsInputStream_new()
{
	return TarsStream_new();
}

////////////////////////////////////////////////
void TarsOutputStream_del(TarsOutputStream ** os)
{
	TarsStream_del(os);
}


void TarsOutputStream_reset(TarsOutputStream * os)
{
	JString_clear(os->_buf);
}

Int32 TarsOutputStream_writeBuf(TarsOutputStream * os, const void * buf, uint32_t len)
{
	char * p = (char *) buf;
	return JString_append(os->_buf, p, len);
}

char * TarsOutputStream_getBuffer(TarsOutputStream * os)          { return JString_data(os->_buf);}
uint32_t TarsOutputStream_getLength(TarsOutputStream * os)       { return JString_size(os->_buf);}


Int32 TarsOutputStream_writeBool(TarsOutputStream * os, Bool b, uint8_t tag)
{
	TarsOutputStream_writeChar(os, (Char) b, tag);

	return TARS_SUCCESS;
}

Int32 TarsOutputStream_writeChar(TarsOutputStream * os, Char n, uint8_t tag)
{
	Int32 ret;
	//DataHead_setTag(os->_h, tag);
	if (n == 0)
	{
		//DataHead_setType(os->_h, eZeroTag);
		//ret = DataHead_writeTo(os->_h, os);
		ret = DataHead_setAndWriteTo(os->_h, eZeroTag, tag, os);

		if (TARS_SUCCESS != ret)	return ret;
	}
	else
	{
		//DataHead_setType(os->_h, eChar);
		//ret = DataHead_writeTo(os->_h, os);
		ret = DataHead_setAndWriteTo(os->_h, eChar, tag, os);
		if (TARS_SUCCESS != ret)	return ret;

		return TarsOutputStream_writeBuf(os, &n, sizeof(n));
	}

	return TARS_SUCCESS;
}

Int32 TarsOutputStream_writeUInt8(TarsOutputStream * os, UInt8 n, uint8_t tag)
{
	return TarsOutputStream_writeShort(os, (Short)n, tag);
}

Int32 TarsOutputStream_writeShort(TarsOutputStream * os, Short n, uint8_t tag)
{
	Int32 ret;

	if (n >= (-128) && n <= 127)
	{
		return TarsOutputStream_writeChar(os, (Char) n, tag);
	}
	else
	{
		ret = DataHead_setAndWriteTo(os->_h, eShort, tag, os);
		if (TARS_SUCCESS != ret)	return ret;

		n = tars_htons(n);
		return TarsOutputStream_writeBuf(os, &n, sizeof(n));
	}

	return TARS_SUCCESS;
}

Int32 TarsOutputStream_writeUInt16(TarsOutputStream * os, UInt16 n, uint8_t tag)
{
	return TarsOutputStream_writeInt32(os, (Int32)n, tag);
}

Int32 TarsOutputStream_writeInt32(TarsOutputStream * os, Int32 n, uint8_t tag)
{
	Int32 ret;

	if (n >= (-32768) && n <= 32767)
	{
		return TarsOutputStream_writeShort(os, (Short) n, tag);
	}
	else
	{
		ret = DataHead_setAndWriteTo(os->_h, eInt32, tag, os);
		if (TARS_SUCCESS != ret)	return ret;

		n = tars_htonl(n);
		return TarsOutputStream_writeBuf(os, &n, sizeof(n));
	}

	return TARS_SUCCESS;
}

Int32 TarsOutputStream_writeUInt32(TarsOutputStream * os, UInt32 n, uint8_t tag)
{
	return TarsOutputStream_writeInt64(os, (Int64)n, tag);
}

Int32 TarsOutputStream_writeInt64(TarsOutputStream * os, Int64 n, uint8_t tag)
{
	Int32 ret;

#ifdef __MTK_64
	if (n.high == 0 )
	{
		return TarsOutputStream_writeInt32(os, (Int32)n.low, tag);
	}
#else
	if (n >= (-2147483647-1) && n <= 2147483647)
	{
		return TarsOutputStream_writeInt32(os, (Int32) n, tag);
	}
#endif
	else
	{
		ret = DataHead_setAndWriteTo(os->_h, eInt64, tag, os);
		if (TARS_SUCCESS != ret)	return ret;

		n = tars_htonll(n);
		return TarsOutputStream_writeBuf(os, &n, sizeof(n));
	}

	return TARS_SUCCESS;
}



Int32 TarsOutputStream_writeFloat(TarsOutputStream * os, Float n, uint8_t tag)
{
	Int32 ret;

	ret = DataHead_setAndWriteTo(os->_h, eFloat, tag, os);
	if (TARS_SUCCESS != ret)	return ret;

	n = tars_htonf(n);
	return TarsOutputStream_writeBuf(os, &n, sizeof(n));
}

Int32 TarsOutputStream_writeDouble(TarsOutputStream * os, Double n, uint8_t tag)
{
	Int32 ret=0;
	//DataHead_setType(os->_h, eDouble);
	//DataHead_setTag(os->_h, tag);

	//ret = DataHead_writeTo(os->_h, os);
	ret = DataHead_setAndWriteTo(os->_h, eDouble, tag, os);
	if (TARS_SUCCESS != ret)	return ret;

	n = tars_htond(n);
	return TarsOutputStream_writeBuf(os, &n, sizeof(n));
}
Int32 TarsOutputStream_writeString(TarsOutputStream * os, JString* s, uint8_t tag)
{
	return TarsOutputStream_writeStringBuffer(os, JString_data(s), JString_size(s), tag);
}

Int32 TarsOutputStream_writeStringBuffer(TarsOutputStream * os, const char* buff, uint32_t len, uint8_t tag)
{
	Int32 ret=0;

	//DataHead_setType(os->_h, eString1);
	//DataHead_setTag(os->_h, tag);

	if (len > 255)
	{
		uint32_t n;

		//DataHead_setType(os->_h, eString4);
		//ret = DataHead_writeTo(os->_h, os);
		ret = DataHead_setAndWriteTo(os->_h, eString4, tag, os);
		if (TARS_SUCCESS != ret)	return ret;

		n = tars_htonl(len);
		ret = TarsOutputStream_writeBuf(os, &n, sizeof(n));
		if (TARS_SUCCESS != ret)	return ret;

		return TarsOutputStream_writeBuf(os, buff, len);
	}
	else
	{
		uint8_t n;

		if (len > TARS_MAX_STRING_LENGTH)
		{

			snprintf(os->_err, sizeof(os->_err), "invalid string size, tag: %d, size: %u", tag, (uint32_t)len);
			return TARS_ENCODE_ERROR;
		}

		//ret = DataHead_writeTo(os->_h, os);
		ret = DataHead_setAndWriteTo(os->_h, eString1, tag, os);

		n = len;
		ret = TarsOutputStream_writeBuf(os, &n, sizeof(n));
		if (TARS_SUCCESS != ret)	return ret;

		return TarsOutputStream_writeBuf(os, buff, len);
	}

	return TARS_SUCCESS;
}

/**
 * �����һ��map�Ľṹ
 * @param  os  [description]
 * @param  m   [description]
 * @param  tag [description]
 * @return     [description]
 */
Int32 TarsOutputStream_writeMap(TarsOutputStream * os, JMapWrapper* m, uint8_t tag)
{
	uint32_t i;
	Int32 n;
	Int32 ret;

	//DataHead_setType(os->_h, eMap);
	//DataHead_setTag(os->_h, tag);

	// ret = DataHead_writeTo(os->_h, os);
	ret = DataHead_setAndWriteTo(os->_h, eMap, tag, os);
	if (TARS_SUCCESS != ret)	return ret;

	n = JMapWrapper_size(m);
	ret = TarsOutputStream_writeInt32(os, n, 0);
	if (TARS_SUCCESS != ret)	return ret;

	for (i = 0; i < JMapWrapper_size(m); ++i)
	{
		ret = TarsOutputStream_writeBuf(os, JArray_getPtr(m->first, i), JArray_getLength(m->first, i));
		if (TARS_SUCCESS != ret)	return ret;

		ret = TarsOutputStream_writeBuf(os, JArray_getPtr(m->second, i), JArray_getLength(m->second, i));
		if (TARS_SUCCESS != ret)	return ret;
	}

	return TARS_SUCCESS;
}


Int32 TarsOutputStream_writeVector(TarsOutputStream * os, JArray* v, uint8_t tag)
{
	Int32 i, n, ret;

	//DataHead_setType(os->_h, eList);
	// DataHead_setTag(os->_h, tag);

	//ret = DataHead_writeTo(os->_h, os);
	ret = DataHead_setAndWriteTo(os->_h, eList, tag, os);
	if (TARS_SUCCESS != ret)	return ret;

	n = JArray_size(v);
	ret = TarsOutputStream_writeInt32(os, n, 0);
	if (TARS_SUCCESS != ret)	return ret;

	for (i = 0; i < JArray_size(v); ++i)
	{
		ret = TarsOutputStream_writeBuf(os, JArray_getPtr(v, i), JArray_getLength(v, i));
		if (TARS_SUCCESS != ret)	return ret;
	}

	return TARS_SUCCESS;
}

Int32 TarsOutputStream_writeVectorChar(TarsOutputStream * os, JString *v, uint8_t tag)
{
	return TarsOutputStream_writeVectorCharBuffer(os, JString_data(v), JString_size(v), tag);
}

Int32 TarsOutputStream_writeVectorCharBuffer(TarsOutputStream * os, const char* buff, uint32_t len, uint8_t tag)
{
	Int32 ret;
	DataHead *hh;

	//DataHead_setType(os->_h, eSimpleList);
	// DataHead_setTag(os->_h, tag);

	//ret = DataHead_writeTo(os->_h, os);
	ret = DataHead_setAndWriteTo(os->_h, eSimpleList, tag, os);
	if (TARS_SUCCESS != ret)	return ret;

	hh = DataHead_new();
	if (!hh) return TARS_MALLOC_ERROR;

	//DataHead_setType(hh, eChar);
	//DataHead_setTag(hh, 0);

	//ret = DataHead_writeTo(hh, os);
	ret = DataHead_setAndWriteTo(hh, eChar, 0, os);
	DataHead_del(&hh);

	if (TARS_SUCCESS != ret)	return ret;

	ret = TarsOutputStream_writeInt32(os, len, 0);
	if (TARS_SUCCESS != ret)	return ret;

	return TarsOutputStream_writeBuf(os, buff, len);
}

/**
 * �������д��һ���ṹ�壬�㵱ȻҲ������buffer��д��һ���ṹ��
 * @param  os  [description]
 * @param  st  [description]
 * @param  tag [description]
 * @return     [description]
 */
Int32 TarsOutputStream_writeStruct(TarsOutputStream * os,const void * st,  uint8_t tag)
{
	Int32 ret=0;
	const JStructBase* jst = st;
	TarsOutputStream *o = TarsOutputStream_new();
	do
	{
		if (!o)
		{
			ret = TARS_MALLOC_ERROR; break;
		}
		ret = jst->writeTo(st, o);
		if ( TARS_SUCCESS != ret ) break;
		ret = TarsOutputStream_writeStructBuffer(os, TarsOutputStream_getBuffer(o),TarsOutputStream_getLength(o), tag);
	}while (0);
	if (o) TarsOutputStream_del(&o);
	return ret;
}

Int32 TarsOutputStream_writeStructString(TarsOutputStream * os, JString * v,  uint8_t tag)
{
	return TarsOutputStream_writeStructBuffer(os, JString_data(v), JString_size(v), tag);
}


Int32 TarsOutputStream_writeStructBuffer(TarsOutputStream * os, const char* buff, uint32_t len, uint8_t tag)
{
	Int32 ret;
	//DataHead_setType(os->_h, eStructBegin);
	//DataHead_setTag(os->_h, tag);

	//ret = DataHead_writeTo(os->_h, os);
	ret = DataHead_setAndWriteTo(os->_h, eStructBegin, tag, os);
	if (TARS_SUCCESS != ret)	return ret;

	ret = TarsOutputStream_writeBuf(os, buff, len);
	if (TARS_SUCCESS != ret)	return ret;

	//DataHead_setType(os->_h, eStructEnd);
	//DataHead_setTag(os->_h, 0);

	//ret = DataHead_writeTo(os->_h, os);
	ret = DataHead_setAndWriteTo(os->_h, eStructEnd, 0, os);
	return ret;
}


Int32 TarsOutputStream_init(TarsOutputStream * os)
{
	return TarsStream_init(os);
}


TarsOutputStream * TarsOutputStream_new()
{
	return TarsStream_new();
}


