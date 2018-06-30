#ifndef PHP_TTARS_H
#define PHP_TTARS_H

#include "tars_c.h"
#include "php_error.h"
#include "ext/spl/spl_array.h"
#include "ext/spl/spl_iterators.h"
#include "zend_interfaces.h"

#define TTARS_TYPE_BOOL    1
#define TTARS_TYPE_CHAR    2
#define TTARS_TYPE_UINT8   3
#define TTARS_TYPE_SHORT   4
#define TTARS_TYPE_UINT16  5
#define TTARS_TYPE_FLOAT   6
#define TTARS_TYPE_DOUBLE  7
#define TTARS_TYPE_INT32   8
#define TTARS_TYPE_UINT32  9
#define TTARS_TYPE_INT64   10
#define TTARS_TYPE_STRING  11
#define TTARS_TYPE_VECTOR  12
#define TTARS_TYPE_MAP     13
#define TTARS_TYPE_STRUCT  14

#define TARS_PROP_TYPE_NAME "__typeName"
#define TARS_PROP_TYPE_CLASS "__typeClass"
#define TARS_PROP_ORIG_TYPE "__origType"


/* struct */
#define STRUCT_PROP_ORIG_TYPE TTARS_TYPE_STRUCT
#define STRUCT_PROP_TPL "__tpl"
#define STRUCT_PROP_FIELDS "__fields"
#define STRUCT_NAME_MAX 128
#define STRUCT_NAME_MIN 2

/* vector */
#define VECTOR_PROP_TYPE "__type"
#define VECTOR_PROP_TYPE_CLASS "__typeClass"
#define VECTOR_PROP_ORIG_TYPE TTARS_TYPE_VECTOR
#define TARS_PROP_VEC_COUNT "_count"
#define TARS_PROP_VEC_POS "_position"


/* map */
#define MAP_PROP_ORIG_TYPE TTARS_TYPE_MAP

#define MAP_PROP_PARAM_FORMAT "__format"
#define MAP_PROP_FIRST_TYPE "__ft"
#define MAP_PROP_SECOND_TYPE "__st"

#define MAP_FIRST_TYPE_NAME "__ftn"
#define MAP_SECOND_TYPE_NAME "__stn"

#define MAP_FIRST_TYPE_CLASS "__ftc"
#define MAP_SECOND_TYPE_CLASS "__stc"

#define MAP_FIRST_KEY "key"
#define MAP_SECOND_KEY "value"

#define PHP_TTARS_BOOL      "BOOL"
#define PHP_TTARS_CHAR      "CHAR"
#define PHP_TTARS_UINT8     "UINT8"
#define PHP_TTARS_SHORT     "SHORT"
#define PHP_TTARS_UINT16    "UINT16"
#define PHP_TTARS_FLOAT     "FLOAT"
#define PHP_TTARS_DOUBLE    "DOUBLE"
#define PHP_TTARS_INT32     "INT32"
#define PHP_TTARS_UINT32    "UINT32"
#define PHP_TTARS_INT64     "INT64"
#define PHP_TTARS_STRING    "STRING"
#define PHP_TTARS_VECTOR    "VECTOR"
#define PHP_TTARS_MAP      "MAP"
#define PHP_TTARS_STRUCT    "STRUCT"

#define IS_CLASS_VECTOR(t) ((t) && (Z_TYPE_P(t) == IS_OBJECT) && instanceof_function(Z_OBJCE_P(t), tars_vector_ce TSRMLS_CC))
#define IS_CLASS_MAP(t) ((t) && (Z_TYPE_P(t) == IS_OBJECT) && instanceof_function(Z_OBJCE_P(t), tars_map_ce TSRMLS_CC))
#define IS_CLASS_STRUCT(t) ((t) && (Z_TYPE_P(t) == IS_OBJECT) && instanceof_function(Z_OBJCE_P(t), tars_struct_ce TSRMLS_CC))

#define IS_VALID_TYPE(t) ((t) >= TTARS_TYPE_BOOL && (t) <= TTARS_TYPE_STRUCT)
#define IS_BASE_TYPE(t) ((t) >= TTARS_TYPE_BOOL && (t) <= TTARS_TYPE_STRING)
#define IS_JSTRING(t) ((t) == TTARS_TYPE_CHAR)
#define IS_STRUCT(t) ((t) == TTARS_TYPE_STRUCT)

typedef int (* tars_pack_func_t)(zval *, TarsOutputStream *, uint8_t, void *);

typedef union vector_ctx {
	JArray * vct;
	JString * str;
} vector_ctx;

#if PHP_MAJOR_VERSION < 7
typedef struct {
	zend_object std;
	zend_uchar t;
	vector_ctx * ctx;
} vector_wrapper ;

typedef struct {
	zend_object std;
	JMapWrapper * ctx;
} map_wrapper ;
#else
typedef struct {
	zend_uchar t;
	vector_ctx * ctx;
	HashTable *props;
	zend_object std;
} vector_wrapper ;

typedef struct {
	JMapWrapper * ctx;
	HashTable *props;
	zend_object std;
} map_wrapper ;
#endif

int bool_packer(zval * , TarsOutputStream *, uint8_t, void *);
int char_packer(zval * , TarsOutputStream *, uint8_t, void *);
int uint8_packer(zval * , TarsOutputStream *, uint8_t, void *);
int short_packer(zval * , TarsOutputStream *, uint8_t, void *);
int uint16_packer(zval * , TarsOutputStream *, uint8_t, void *);
int int32_packer(zval * , TarsOutputStream *, uint8_t, void *);
int uint32_packer(zval * , TarsOutputStream *, uint8_t, void *);
int int64_packer(zval * , TarsOutputStream *, uint8_t, void *);
int string_packer(zval * , TarsOutputStream *, uint8_t, void *);
int double_packer(zval * , TarsOutputStream *, uint8_t, void *);
int float_packer(zval * , TarsOutputStream *, uint8_t, void *);
int vector_packer(zval * , TarsOutputStream *, uint8_t, void *);
int map_packer(zval * , TarsOutputStream *, uint8_t, void *);
int struct_packer(zval * occupy, TarsOutputStream *, uint8_t,void *);

static const tars_pack_func_t packer_dispatch[] = {
	NULL,
	bool_packer,
	char_packer,
	uint8_packer,
	short_packer,
	uint16_packer,
	float_packer,
	double_packer,
	int32_packer,
	uint32_packer,
	int64_packer,
	string_packer,
	vector_packer,
	map_packer,
	struct_packer
};

int bool_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv);
int char_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv);
int uint8_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv);
int short_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv);
int uint16_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv);
int float_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv);
int double_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv);
int int64_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv);

int int32_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv);
int uint32_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv);
int string_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv);
int vector_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv);
int map_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv);
int struct_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr,void ** zv);

typedef int (* tars_unpack_func_t)(TarsOutputStream *, uint8_t, Bool, zval *, void **);
static const  tars_unpack_func_t unpacker_dispatch[] = {
	NULL,
	bool_unpacker,
	char_unpacker,
	uint8_unpacker,
	short_unpacker,
	uint16_unpacker,
	float_unpacker,
	double_unpacker,
	int32_unpacker,
	uint32_unpacker,
	int64_unpacker,
	string_unpacker,
	vector_unpacker,
	map_unpacker,
	struct_unpacker
};

TUP_STARTUP_FUNC(ttars);
int tars_struct_write(zval * struct_obj, TarsOutputStream * out);
int php_TarsOutputStream_writeStruct(TarsOutputStream * os, zval * st,  uint8_t tag);
inline const char * __complex_name(zval * this_ptr);

int map_converter(zval * , zval *);

int struct_packer_wrapper(TarsOutputStream * out, void * struct_ptr);
int struct_unpacker_wrapper(TarsInputStream * is, zval * this_ptr, void ** zv);
int _map_to_array (zval * this_ptr, JMapWrapper * container, void **zv);

#if PHP_MAJOR_VERSION < 7
#define Z_VECTOR_WRAPPER_P(zv) (vector_wrapper * ) zend_object_store_get_object(zv)
#define Z_MAP_WRAPPER_P(zv) (map_wrapper * ) zend_object_store_get_object(zv)
#else
#define Z_VECTOR_WRAPPER_P(zv) vector_wrapper_fetch_object(Z_OBJ_P(zv))
static inline vector_wrapper *vector_wrapper_fetch_object(zend_object *obj) /* {{{ */ {
	return (vector_wrapper *)((char*)obj - XtOffsetOf(vector_wrapper, std));
}

#define Z_MAP_WRAPPER_P(zv) map_wrapper_fetch_object(Z_OBJ_P(zv))
static inline map_wrapper *map_wrapper_fetch_object(zend_object *obj) /* {{{ */ {
	return (map_wrapper *)((char*)obj - XtOffsetOf(map_wrapper, std));
}
#endif

PHP_METHOD(tars, __construct);
PHP_METHOD(tars_vector, __construct);
PHP_METHOD(tars_vector, pushBack);

PHP_METHOD(tars_map, __construct);
PHP_METHOD(tars_map, pushBack);

PHP_METHOD(tars_struct, __construct);

#endif
