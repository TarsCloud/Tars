#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#if PHP_MAJOR_VERSION < 7
#include "ext/standard/php_smart_str.h"
#else
#include "ext/standard/php_smart_string.h"
#endif
#include "./include/tars_c.h"
#include "./include/tup_c.h"
#include "./include/php_tupapi.h"
#include "./include/ttars.h"

/* {{{ static variable
 */
static zend_object_handlers vector_wrapper_handlers;
static zend_object_handlers map_wrapper_handlers;

static void vector_wrapper_dtor(zend_object * object TSRMLS_DC);
static void map_wrapper_dtor(zend_object * object TSRMLS_DC);

static size_t tars_type_name(zval * this_ptr, int * type, char ** out);

#if PHP_MAJOR_VERSION < 7
    static inline zend_object_value map_wrapper_object_new(zend_class_entry * clazz TSRMLS_DC);
    static inline zend_object_value vector_wrapper_object_new(zend_class_entry * clazz TSRMLS_DC);
#else
static inline zend_object * map_wrapper_object_new(zend_class_entry * clazz TSRMLS_DC);
static inline zend_object * vector_wrapper_object_new(zend_class_entry * clazz TSRMLS_DC);
#endif
/* }}} */

/* {{{ _TARS_ZVAL_TO_BIG_NUMERIC 将zval结构转换成特定的数字类型
 */
#define _TARS_ZVAL_TO_NUMERIC(z, type, out, tag, s) do { \
    type __dest, *tmp;                                  \
    switch (Z_TYPE_P(z)) {                              \
        case IS_LONG : break;                           \
        case IS_NULL :                                  \
            ZVAL_LONG(z, 0);                            \
            break;                                      \
        case IS_TRUE :                                  \
        case IS_FALSE :                                 \
        case IS_DOUBLE :                                \
        case IS_STRING :                                \
            convert_to_long(z);                         \
            break;                                      \
        default :                                       \
            return ERR_CANNOT_CONVERT;                  \
    }                                                   \
    __dest = (type) Z_LVAL_P(z);                        \
    if (Z_LVAL_P(z) != (long) __dest) {                 \
        return ERR_OUTOF_RANGE;                         \
    }                                                   \
    if (s) {                                            \
        tmp = s;                                        \
        *tmp = __dest ;                                 \
    }                                                   \
    if (out) return TarsOutputStream_write ##type (out, __dest, tag);\
} while (0)
/* }}} */

/* {{{ _TARS_ZVAL_TO_BIG_NUMERIC 将zval转成uint32和int64
 */
#if __WORDSIZE == 64
#define STR_TO_INT64 strtol
#else
#define STR_TO_INT64 strtoll
#endif

#define _TARS_ZVAL_TO_BIG_NUMERIC(z, type, out, tag, s) do {\
    type *tmp, __dest;                                 \
    Int64 ll;                                          \
    char * end_ptr;                                    \
    if (Z_TYPE_P(z) == IS_STRING) {                    \
        errno = 0;                                     \
        ll = STR_TO_INT64 (Z_STRVAL_P(z), &end_ptr, 10); \
        if (*end_ptr != '\0') {                        \
            return ERR_CANNOT_CONVERT;                 \
        }                                              \
        __dest = (type) ll;                            \
        if (errno == ERANGE || ll != (Int64) __dest) { \
            return ERR_OUTOF_RANGE;                    \
        }                                              \
        if (s) {                                       \
            tmp = s;                                   \
            *tmp = __dest;                             \
        }                                              \
        if (out) return TarsOutputStream_write ##type (out, __dest, tag);\
    } else {                                           \
        _TARS_ZVAL_TO_NUMERIC(z, type, out, tag, s);    \
    }                                                  \
} while (0)
/* }}} */

/* {{{ TARS_ZVAL_TO_FD  将zval 转成 浮点数
 */
#define TARS_ZVAL_TO_FD(z, type, out, tag, s) do {     \
    type __dest, *tmp;                                \
    switch (Z_TYPE_P(z)) {                            \
        case IS_DOUBLE : break;                       \
        case IS_NULL :                                \
            ZVAL_DOUBLE(z, 0.0);                      \
            break;                                    \
        case IS_TRUE :                                \
        case IS_FALSE :                               \
        case IS_LONG :                                \
        case IS_STRING :                              \
            convert_to_double(z);                     \
            break;                                    \
        default :                                     \
            return ERR_CANNOT_CONVERT;                \
    }                                                 \
    __dest = Z_DVAL_P(z);                             \
    if (s) {                                          \
        tmp = s;                                      \
        *tmp = __dest ;                               \
    }                                                 \
    if (out) return TarsOutputStream_write ##type (out, __dest, tag);\
} while (0)
/* }}} */

/* {{{ bool_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void * out)
 */
int bool_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void * out) {

    int ret = TARS_SUCCESS;
    if (Z_TYPE_P(z) == IS_OBJECT || Z_TYPE_P(z) == IS_RESOURCE) {
        return ERR_CANNOT_CONVERT;
    }
    convert_to_boolean(z);
    if (out) {
        Bool * tmp = out;
        *tmp = (MY_Z_TYPE_P(z) == IS_TRUE) ? true : false;
    }
    if (stream) ret = TarsOutputStream_writeBool(stream, (MY_Z_TYPE_P(z) == IS_TRUE) ? true : false, tag);
    return ret;
}
/* }}} */

/* {{{ char_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void * out)
 */
int char_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void * out) {

    int ret = TARS_SUCCESS;
    Char __dest;
    switch(MY_Z_TYPE_P(z)) {
        case IS_STRING :
            if (Z_STRLEN_P(z) > 1) {
                return ERR_CANNOT_CONVERT;
            }
            __dest = Z_STRVAL_P(z)[0];
            break;
        case IS_TRUE:
            __dest = '1';
            break;
        case IS_FALSE:
            __dest = '0';
            break;
        case IS_NULL :
            __dest = '0';
            break;
        case IS_DOUBLE :
            convert_to_long(z);
        case IS_LONG :
            if (Z_LVAL_P(z) > 127 || Z_LVAL_P(z) < -128) {
                return ERR_OUTOF_RANGE;
            }
            __dest = Z_LVAL_P(z);
            break;
        default :
            return ERR_CANNOT_CONVERT;
    }

    if (out) {
        Char *tmp = out;
        *tmp = __dest;
    }

    if (stream) ret = TarsOutputStream_writeChar(stream, __dest, tag);
    return ret;
}
/* }}} */

/* {{{ uint8_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void * out)
 */
int uint8_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void * out) {

    _TARS_ZVAL_TO_NUMERIC(z, UInt8, stream, tag, out);
    return TARS_SUCCESS;
}
/* }}} */

/* {{{ short_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void * out)
 */
int short_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void * out) {

    _TARS_ZVAL_TO_NUMERIC(z, Short, stream, tag, out);
    return TARS_SUCCESS;
}
/* }}} */

/* {{{ uint16_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void * out)
 */
int uint16_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void * out) {

    _TARS_ZVAL_TO_NUMERIC(z, UInt16, stream, tag, out);
    return TARS_SUCCESS;
}
/* }}} */

/* {{{ int32_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void *out)
 */
int int32_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void *out) {

    _TARS_ZVAL_TO_NUMERIC(z, Int32, stream, tag, out);
    return TARS_SUCCESS;
}
/* }}} */

/* {{{ uint32_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void * out)
 */
int uint32_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void * out) {

    _TARS_ZVAL_TO_BIG_NUMERIC(z, UInt32, stream, tag, out);
    return TARS_SUCCESS;
}
/* }}} */

/* {{{ int64_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void * out)
 */
int int64_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void * out) {

    _TARS_ZVAL_TO_BIG_NUMERIC(z, Int64, stream, tag, out);
    return TARS_SUCCESS;
}
/* }}} */

/* {{{ dobule_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void * out)
 */
int double_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void * out) {

    TARS_ZVAL_TO_FD(z, Double, stream, tag, out);
    return TARS_SUCCESS;
}
/* }}} */

/* {{{ float_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void * out)
 */
int float_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void * out) {

    TARS_ZVAL_TO_FD(z, Float, stream, tag, out);
    return TARS_SUCCESS;
}
/* }}} */

/* {{{ string_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void * out)
 */
int string_packer(zval * z, TarsOutputStream * stream, uint8_t tag, void * out) {

    int ret = TARS_SUCCESS;
    switch (Z_TYPE_P(z)) {
        case IS_STRING :
            break;
        case IS_LONG :
        case IS_DOUBLE :
        case IS_NULL :
        case IS_TRUE :
        case IS_FALSE :
            convert_to_string(z);
            break;
        default :
            return ERR_CANNOT_CONVERT;
    }
    if (out) out = z;
    if (stream) ret = TarsOutputStream_writeStringBuffer(stream, Z_STRVAL_P(z), Z_STRLEN_P(z), tag);

    return ret;
}
/* }}} */

/* {{{ vector_packer(zval * this_ptr, TarsOutputStream * out, uint8_t tag, void * arr)
 */
int vector_packer(zval * zv, TarsOutputStream * out, uint8_t tag, void * vector_ptr) {

    int ret = ERR_CANNOT_CONVERT;
    zval * this_ptr, *tmp = vector_ptr;

    if (IS_CLASS_VECTOR(zv)) {
        this_ptr = zv;
    } else if (IS_CLASS_VECTOR(tmp)) {
        this_ptr = tmp;
    } else {
        return ret;
    }

    vector_wrapper * obj = Z_VECTOR_WRAPPER_P(this_ptr TSRMLS_CC);
    if (IS_JSTRING(obj->t)) {
        JString * js = obj->ctx->str;
        //if (JString_size(js) == 0) {
        //    ret = vector_converter(this_ptr, zv, 0);
        //    if (ret != TARS_SUCCESS) return ret;
        //}

        ret = TarsOutputStream_writeVectorCharBuffer(out, JString_data(js), JString_size(js), tag);
        JString_clear(js);
    } else {
        JArray * container = obj->ctx->vct;
        // 暂时不需要为空的逻辑
        //if (JArray_size(container) == 0) {
        //    ret = vector_converter(this_ptr, zv, 0);
        //    if (ret != TARS_SUCCESS) return ret;
        //}

        ret = TarsOutputStream_writeVector(out, container, tag);
        JArray_clear(container);
    }

    return ret;
}
/* }}} */

/* {{{ map_packer(zval * this_ptr, TarsOutputStream * out, uint8_t tag, void * map)
 */
int map_packer(zval * zv, TarsOutputStream * out, uint8_t tag, void * map_ptr) {

    int ret = ERR_CANNOT_CONVERT;
    zval * this_ptr, *tmp = map_ptr;

    if (IS_CLASS_MAP(zv)) {
        this_ptr = zv;
    } else if (IS_CLASS_MAP(tmp)) {
        this_ptr = tmp;
    } else {
        return ret;
    }

    map_wrapper * obj = Z_MAP_WRAPPER_P(this_ptr TSRMLS_CC);
    JMapWrapper * container = obj->ctx;
    // 打包数据,这里应该是不需要的,之前遗留的逻辑
    //if (JMapWrapper_size(container) == 0) {
    //    ret = map_converter(this_ptr, zv, 0);
    //    if (ret != TARS_SUCCESS) return ret;
    //}

    ret = TarsOutputStream_writeMap(out, container, tag);
    JMapWrapper_clear(container);
    return ret;
}
/* }}} */

int struct_packer(zval * occupy, TarsOutputStream * out, uint8_t tag, void * struct_ptr) {

    int ret;

    TarsOutputStream * o = TarsOutputStream_new();

    if (!o) return ERR_MALLOC_FAILED;
    ret = struct_packer_wrapper(o, struct_ptr);
    if (ret == TARS_SUCCESS) {
        ret = TarsOutputStream_writeStructBuffer(out, TarsOutputStream_getBuffer(o),TarsOutputStream_getLength(o), tag);
    }

    TarsOutputStream_del(&o);
    return ret;
}

int struct_packer_wrapper(TarsOutputStream * o, void * struct_ptr) {

    int ret = ERR_CANNOT_CONVERT;
    zval *this_ptr = struct_ptr;
    HashTable * props, *fields = NULL;

    if (!IS_CLASS_STRUCT(this_ptr)) return ERR_CLASS_MISMATCH;

    // 获取字段信息列表
    zval * tmp = my_zend_read_property(tars_struct_ce, this_ptr, ZEND_STRL(STRUCT_PROP_FIELDS), 1 TSRMLS_CC);
    if (Z_TYPE_P(tmp) == IS_ARRAY) {
        fields = Z_ARRVAL_P(tmp);
    }

    // 思路需要转换，只需要打包那些必须的元素，也就是fields中的内容
#if PHP_MAJOR_VERSION < 7

    props = Z_OBJPROP_P(this_ptr);

    for (
            zend_hash_internal_pointer_reset(fields);
            zend_hash_has_more_elements(fields) == SUCCESS;
            zend_hash_move_forward(fields)
            ) {

        char * tag_key;
        int tag_key_len,t;
        ulong tag;
        zval ** attributes, **val, **type, *sub = NULL ,**extType;

        if (zend_hash_get_current_key_ex(fields, &tag_key, &tag_key_len, &tag, 0, NULL) == HASH_KEY_IS_LONG)
        {
            // 获取fields中的属性
            if (zend_hash_get_current_data(fields, (void **)&attributes) == FAILURE) {
                    return ERR_STATIC_FIELDS_PARAM_LOST;
            } else {
                char *name_key;
                uint name_key_len ;
                // 首先获取name的具体名称，作为获取实际值的key
                if (zend_hash_find(Z_ARRVAL_P(*attributes), "name", sizeof("name"), (void **)&val) == SUCCESS) {
                    if (Z_TYPE_PP(val) == IS_STRING) {
                        name_key = Z_STRVAL_PP(val);
                    } else {
                        zval copyval = **val;
                        zval_copy_ctor(&copyval);
                        convert_to_string(&copyval);
                        name_key = Z_STRVAL(copyval);
                        zval_dtor(&copyval);
                    }

                    // 其次获取是否是必选的字段
                    Bool is_required;
                    if (zend_hash_find(Z_ARRVAL_P(*attributes), "required", sizeof("required"), (void **)&val) == SUCCESS) {
                        if (Z_TYPE_PP(val) == IS_BOOL) {
                            is_required = Z_BVAL_PP(val);
                        } else {
                            zval copyval = **val;
                            zval_copy_ctor(&copyval);
                            convert_to_boolean(&copyval);
                            is_required = Z_BVAL(copyval);
                            zval_dtor(&copyval);
                        }

                        // 获取其中的值
                        if (zend_hash_find(props, name_key, strlen(name_key)+1, (void **)&val) == SUCCESS) {
                            // 再次获取具体的类型
                            if (zend_hash_find(Z_ARRVAL_P(*attributes), "type", sizeof("type"), (void **)&type) == SUCCESS) {

                                int real_type;
                                if (Z_TYPE_PP(type) == IS_LONG) {
                                    real_type = Z_LVAL_PP(type);
                                } else {
                                    zval copyval = **type;
                                    zval_copy_ctor(&copyval);
                                    convert_to_long(&copyval);
                                    real_type = Z_LVAL(copyval);
                                    zval_dtor(&copyval);
                                }
                                // 此时的val是真正从类中获取到的对象

                                // 如果是struct类型
                                if(IS_STRUCT(real_type)) {
                                    sub = *val;
                                    ret = struct_packer(NULL,o, tag, sub);
                                }
                                // 其他复杂类型,包括vector和map sub本来是候补用的，在新的架构下，获取与普通类型并无差别
                                else if (!IS_BASE_TYPE(real_type)) {
                                    sub = *val;
                                    ret = packer_dispatch[real_type](sub, o, tag, sub);

                                    // 需要重新获取extType参数,从而能够获取到vector和map的真正类型
                                    //if (zend_hash_find(Z_ARRVAL_P(*attributes), "extType", sizeof("extType"),
                                    //(void **)&extType) == SUCCESS) {
                                    //    sub = *extType;
                                    //    ret = packer_dispatch[real_type](*val, o, tag, sub);
                                    //}
                                    //else return ERR_VECOTR_OR_MAP_EXT_PARAM_LOST;
                                }
                                else {
                                    ret = packer_dispatch[real_type](*val, o, tag, sub);
                                }


                                if (ret != TARS_SUCCESS) return ret;
                            }
                            else {
                                return ERR_STATIC_FIELDS_PARAM_LOST;
                            }
                        }
                        else {
                            // 如果是必填但是获取失败，那么报错
                            if(is_required) {
                                return ERR_REQUIRED_FIELD_LOST;
                            }
                            else return ERR_ARRAY_RETRIEVE;
                        }
                    }
                    else {
                        return ERR_STATIC_FIELDS_PARAM_LOST;
                    }
                }
                else {
                    return ERR_REQUIRED_FIELD_LOST;
                }
            }
        }
    }
#else
    //PHP7
    ulong tag;
    zend_string *zkey;
    zval *attributes;

    ZEND_HASH_FOREACH_KEY_VAL(fields, tag, zkey, attributes) {
        if (!attributes) {
            return ERR_STATIC_FIELDS_PARAM_LOST;
        }

        char *name_key;
        zval * val, *type, *sub = NULL, *extType;

        // 首先获取name的具体名称，作为获取实际值的key
        if ((val = zend_hash_str_find(Z_ARRVAL_P(attributes), "name", sizeof("name")-1)) != NULL) {
            if (Z_TYPE_P(val) == IS_STRING) {
                name_key = Z_STRVAL_P(val);
            } else {
                zval copyval = *val;
                zval_copy_ctor(&copyval);
                convert_to_string(&copyval);
                name_key = Z_STRVAL(copyval);
                zval_dtor(&copyval);
            }

            // 其次获取是否是必选的字段
            Bool is_required;
            if ((val = zend_hash_str_find(Z_ARRVAL_P(attributes), "required", sizeof("required")-1)) != NULL) {
                if (Z_TYPE_P(val) == IS_TRUE || Z_TYPE_P(val) == IS_FALSE) {
                    is_required = (Z_TYPE_P(val) == IS_TRUE ? true : false);
                } else {
                    zval copyval = *val;
                    zval_copy_ctor(&copyval);
                    convert_to_boolean(&copyval);
                    is_required = (Z_TYPE_P(val) == IS_TRUE ? true : false);
                    zval_dtor(&copyval);
                }

                // 获取其中的值 //TODO change old code ?
                if ((val = my_zend_read_property(tars_struct_ce, this_ptr, name_key, strlen(name_key), 1 TSRMLS_CC))) {
                    // 再次获取具体的类型
                    if ((type = zend_hash_str_find(Z_ARRVAL_P(attributes), "type", sizeof("type")-1)) != NULL) {
                        int real_type;
                        if (Z_TYPE_P(type) == IS_LONG) {
                            real_type = Z_LVAL_P(type);
                        } else {
                            zval copyval = *type;
                            zval_copy_ctor(&copyval);
                            convert_to_long(&copyval);
                            real_type = Z_LVAL(copyval);
                            zval_dtor(&copyval);
                        }
                        // 此时的val是真正从类中获取到的对象

                        // 如果是struct类型
                        if(IS_STRUCT(real_type)) {
                            sub = val;
                            ret = struct_packer(NULL,o, tag, sub);
                        }
                            // 其他复杂类型,包括vector和map sub本来是候补用的，在新的架构下，获取与普通类型并无差别
                        else if (!IS_BASE_TYPE(real_type)) {
                            // 需要重新获取extType参数,从而能够获取到vector和map的真正类型
                            if ((extType = zend_hash_str_find(Z_ARRVAL_P(attributes), "extType", sizeof("extType")-1)) != NULL) {
                                sub = extType;
                                ret = packer_dispatch[real_type](val, o, tag, sub);
                            }else {
                                sub = val;
                                ret = packer_dispatch[real_type](sub, o, tag, sub);
                            }
                        }
                        else {
                            ret = packer_dispatch[real_type](val, o, tag, sub);
                        }

                        if (ret != TARS_SUCCESS) return ret;
                    }
                    else {
                        return ERR_STATIC_FIELDS_PARAM_LOST;
                    }
                }
                else {
                    // 如果是必填但是获取失败，那么报错
                    if(is_required) {
                        return ERR_REQUIRED_FIELD_LOST;
                    }
                    else return ERR_ARRAY_RETRIEVE;
                }
            }
            else {
                return ERR_STATIC_FIELDS_PARAM_LOST;
            }
        }
        else {
            return ERR_REQUIRED_FIELD_LOST;
        }

    } ZEND_HASH_FOREACH_END();
#endif

    return ret;
}

/* {{{ bool_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv)
 */
int bool_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv) {

    int ret;
    zval * tmp;
    Bool b = 0;
    ret = TarsInputStream_readBool(stream, &b, tag, is_require);
    if (ret == TARS_SUCCESS) {
        ALLOC_INIT_ZVAL(tmp);
        ZVAL_BOOL(tmp, b?1:0);
        *zv = tmp;
    }
    return ret;
}
/* }}} */

/* {{{ char_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv)
 */
int char_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv) {

    int ret;
    Char c = '\0';
    zval * tmp;
    ret = TarsInputStream_readChar(stream, &c, tag, is_require);
    if (ret == TARS_SUCCESS) {
        ALLOC_INIT_ZVAL(tmp);
        MY_ZVAL_STRINGL(tmp, &c, 1, 1);
        * zv = tmp;
    }
    return ret;
}
/* }}} */

/* {{{  _TARS_CONVERT_NUMERIC_ZVAL
 */
#define _TARS_CONVERT_NUMERIC_ZVAL(type, stream, tag, require, this_ptr, zv) do { \
    int ret; type l = 0; \
    zval * tmp; \
    ret = TarsInputStream_read ##type (stream, &l, tag, require); \
    if (ret == TARS_SUCCESS) { \
        ALLOC_INIT_ZVAL(tmp); \
        ZVAL_LONG(tmp, l); \
        * zv = tmp; \
    } \
    return ret ; \
} while (0)
/* }}} */

/* {{{ uint8_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv)
 */
int uint8_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv) {

    _TARS_CONVERT_NUMERIC_ZVAL(UInt8, stream, tag, is_require, this_ptr, zv);
}
/* }}} */

/* {{{ short_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv)
 */
int short_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv) {

    _TARS_CONVERT_NUMERIC_ZVAL(Short, stream, tag, is_require, this_ptr, zv);
}
/* }}} */

/* {{{ uint16_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv)
 */
int uint16_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv) {

    _TARS_CONVERT_NUMERIC_ZVAL(UInt16, stream, tag, is_require, this_ptr, zv);
}
/* }}} */

/* {{{ int32_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv)
 */
int int32_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv) {

    _TARS_CONVERT_NUMERIC_ZVAL(Int32, stream, tag, is_require, this_ptr, zv);
}
/* }}} */

/* {{{ __UNPACK_INT64 解包64位数
 */
#define __UNPACK_INT64(stream, tag, require, this_ptr, zv) do { \
    int ret; Int64 t = 0; \
    zval * tmp; long l; \
    ret = TarsInputStream_readInt64(stream, &t, tag, require); \
    if (ret == TARS_SUCCESS) { \
        l = (long) t;         \
        ALLOC_INIT_ZVAL(tmp); \
        if (t == (Int64) l) { \
            ZVAL_LONG(tmp, l); \
        } else {  \
            char buf[32]; \
            int len = slprintf(buf, 32, "%lld", l); \
            MY_ZVAL_STRINGL(tmp, buf, len, 1); \
        } \
        * zv = tmp; \
    } \
    return ret ; \
} while (0)
/* }}} */

/* {{{ uint32_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv)
 */
int uint32_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv) {

    __UNPACK_INT64(stream, tag, is_require, this_ptr, zv);
}
/* }}} */

/* {{{ int64_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv)
 */
int int64_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv) {

    __UNPACK_INT64(stream, tag, is_require, this_ptr, zv);
}
/* }}} */

/* {{{ __UNPACK_FD 解包浮点数
 */
#define __UNPACK_FD(type, stream, tag, require, this_ptr, zv) do { \
    int ret; type f = 0.0;\
    zval * tmp; \
    ret = TarsInputStream_read ##type (stream, &f, tag, require); \
    if (ret == TARS_SUCCESS) { \
        ALLOC_INIT_ZVAL(tmp); \
        ZVAL_DOUBLE(tmp, (double)f); \
        * zv = tmp; \
    } \
    return ret; \
} while (0)
/* }}} */

/* {{{ float_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv)
 */
int float_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv) {

    __UNPACK_FD(Float, stream, tag, is_require, this_ptr, zv);
}
/* }}} */

/* {{{ double_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv)
 */
int double_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv) {

    __UNPACK_FD(Double, stream, tag, is_require, this_ptr, zv);
}
/* }}} */

/* {{{ string_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void **zv)
 */
int string_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void **zv) {

    int ret;
    zval * tmp;

    JString * js = JString_new();
    if (!js) return ERR_MALLOC_FAILED;

    ret = TarsInputStream_readString(stream, js, tag, is_require);
    if (ret == TARS_SUCCESS) {
        ALLOC_INIT_ZVAL(tmp);
        MY_ZVAL_STRINGL(tmp, JS_STRVAL(js), JS_STRLEN(js), 1);
        * zv = tmp;
    }

    JString_del(&js);
    return ret;
}
/* }}} */

/* {{{ vector_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv)
 */
int vector_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv) {

    zval *sub = NULL, *cntr = NULL, *tmp = NULL;
    JArray * container = NULL;
    JString * js = NULL;
    TarsInputStream * is = NULL;
    tars_unpack_func_t unpacker;
    int i, ret;

    // 检查对象类型
    if (!IS_CLASS_VECTOR(this_ptr)) return ERR_CLASS_MISMATCH;

    vector_wrapper * obj = Z_VECTOR_WRAPPER_P(this_ptr TSRMLS_CC);
    if (!IS_VALID_TYPE(obj->t)) return ERR_TYPE_INVALID;

    // vector<char> 特殊处理
    if (IS_JSTRING(obj->t)) {

        js = obj->ctx->str;
        JString_clear(js);
        ret = TarsInputStream_readVectorChar(stream, js, tag, is_require);
        if (ret != TARS_SUCCESS) return ret;

        ALLOC_INIT_ZVAL(cntr);
        MY_ZVAL_STRINGL(cntr, JS_STRVAL(js), JS_STRLEN(js), 1);
    } else {

        container = obj->ctx->vct;
        unpacker = unpacker_dispatch[obj->t];

        if (!IS_BASE_TYPE(obj->t)) {
            sub = my_zend_read_property(tars_vector_ce, this_ptr, ZEND_STRL(TARS_PROP_TYPE_CLASS), 1 TSRMLS_CC);
        }

        if (stream) {
            JArray_clear(container);
            ret = TarsInputStream_readVector(stream, container, tag, is_require);
            if (ret != TARS_SUCCESS) return ret;
        }

        is = TarsInputStream_new();
        if (!is) return ERR_MALLOC_FAILED;

        ALLOC_INIT_ZVAL(cntr);
        array_init(cntr);

        for(i = 0; i < JArray_size(container); ++i) {
            ret = TarsInputStream_setBuffer(is, JArray_getPtr(container, i), JArray_getLength(container, i));
            if (ret != TARS_SUCCESS) goto error_clean;

            ret = unpacker(is, 0, true, sub, (void **)&tmp);
            if (ret != TARS_SUCCESS) goto error_clean;
            my_zend_hash_next_index_insert(Z_ARRVAL_P(cntr), (void **) &tmp, sizeof(zval *), NULL);
        }

        TarsInputStream_del(&is);
    }

    *zv = cntr;

    return TARS_SUCCESS;

error_clean:
    if (cntr) my_zval_ptr_dtor(&cntr);
    return ret;
}
/* }}} */

/* {{{ map_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv)
 */
int map_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv) {

    JMapWrapper * container;
    int ret;

    if (!IS_CLASS_MAP(this_ptr)) return ERR_CLASS_MISMATCH;
    map_wrapper * obj = Z_MAP_WRAPPER_P(this_ptr TSRMLS_CC);
    container = obj->ctx;
    JMapWrapper_clear(container);

    ret = TarsInputStream_readMap(stream, container, tag, is_require);

    if (ret != TARS_SUCCESS) return ret;
    return _map_to_array(this_ptr, container, zv);
}
/* }}} */

/* {{{ _map_to_array (zval * this_ptr, JMapWrapper * container, void **zv)
 */
int _map_to_array(zval * this_ptr, JMapWrapper * container, void **zv) {

    zval *type, *fsub = NULL, *ssub = NULL, *cntr = NULL;
    tars_unpack_func_t funpacker, sunpacker;
    int i, ft, st, ret, format = 0;

    type = my_zend_read_property(tars_map_ce, this_ptr, ZEND_STRL(MAP_PROP_FIRST_TYPE), 1 TSRMLS_CC);
    ft = Z_LVAL_P(type);
    if (!IS_VALID_TYPE(ft)) return ERR_TYPE_INVALID;
    if (!IS_BASE_TYPE(ft)) fsub = my_zend_read_property(tars_map_ce, this_ptr, ZEND_STRL(MAP_FIRST_TYPE_CLASS), 1 TSRMLS_CC);

    type = my_zend_read_property(tars_map_ce, this_ptr, ZEND_STRL(MAP_PROP_SECOND_TYPE), 1 TSRMLS_CC);
    st = Z_LVAL_P(type);
    if (!IS_VALID_TYPE(st)) return ERR_TYPE_INVALID;
    if (!IS_BASE_TYPE(st)) ssub = my_zend_read_property(tars_map_ce, this_ptr, ZEND_STRL(MAP_SECOND_TYPE_CLASS), 1 TSRMLS_CC);

    type = my_zend_read_property(tars_map_ce, this_ptr, ZEND_STRL(MAP_PROP_PARAM_FORMAT), 1 TSRMLS_CC);
    if (MY_Z_TYPE_P(type) == IS_TRUE) {
        format = 1;
    }

    funpacker = unpacker_dispatch[ft];
    sunpacker = unpacker_dispatch[st];

    TarsInputStream * stream = TarsInputStream_new();
    if (!stream) return ERR_MALLOC_FAILED;

    ALLOC_INIT_ZVAL(cntr);
    array_init(cntr);

    for (i = 0; i < JMapWrapper_size(container); ++i) {
        zval * key = NULL, * value = NULL;

        // 每一个键
        ret = TarsInputStream_setBuffer(stream, JArray_getPtr(container->first, i), JArray_getLength(container->first, i));
        if (ret != TARS_SUCCESS) goto do_clean;

        ret = funpacker(stream, 0, true, fsub, (void **)&key);
        if (ret != TARS_SUCCESS) goto do_clean;

        // 第二个键
        ret = TarsInputStream_setBuffer(stream, JArray_getPtr(container->second, i), JArray_getLength(container->second, i));
        if (ret != TARS_SUCCESS) goto do_clean;
        ret = sunpacker(stream, 1, true, ssub, (void **)&value);
        if (ret != TARS_SUCCESS) goto do_clean;

        if (!format) {
            // 以一维数组的方式返回

            /*
             *  php数组的转换规则:
             *    1. 数组的key 只能是长整型或字符串
             *    2. 浮点数转换成长整型
             *    3. 纯数字([0-9])字符串转换成长整型
             *    4. 其他类型要强制转换成长整型
             */
            if (Z_TYPE_P(key) == IS_LONG) {
                my_zend_hash_index_update(Z_ARRVAL_P(cntr), Z_LVAL_P(key), (void *) &value, sizeof(zval *), NULL);
            } else {
#if PHP_MAJOR_VERSION < 7
                zend_symtable_update(Z_ARRVAL_P(cntr), Z_STRVAL_P(key), Z_STRLEN_P(key) + 1, &value, sizeof(zval *), NULL);
#else
                zend_symtable_update(Z_ARRVAL_P(cntr), Z_STR_P(key), value);
#endif
            }

            my_zval_ptr_dtor(&key);
        } else {
            // 以两维数组的方式返回
            zval * item;

            ALLOC_INIT_ZVAL(item);
            array_init(item);

            my_zend_hash_add(Z_ARRVAL_P(item), MAP_FIRST_KEY, sizeof(MAP_FIRST_KEY), (void *)&key, sizeof(zval *), NULL);
            my_zend_hash_add(Z_ARRVAL_P(item), MAP_SECOND_KEY, sizeof(MAP_SECOND_KEY), (void *)&value, sizeof(zval *), NULL);
            my_zend_hash_next_index_insert(Z_ARRVAL_P(cntr), (void **) &item, sizeof(zval *), NULL);
        }
    }

    *zv = cntr;
    TarsInputStream_del(&stream);
    return TARS_SUCCESS;

do_clean :
    TarsInputStream_del(&stream);
    if (cntr) my_zval_ptr_dtor(&cntr);
    return ret;
}
/* }}} */

/* {{{ struct_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr, void ** zv)
 */
int struct_unpacker(TarsInputStream * stream, uint8_t tag, Bool is_require, zval * this_ptr,void ** zv) {

    JString * js;
    TarsInputStream * is;
    int ret ;

    js = JString_new();
    if (!js) return TARS_MALLOC_ERROR;

    is = TarsInputStream_new();
    if (!is) {JString_del(&js); return TARS_MALLOC_ERROR;}

    TarsInputStream_readStructString(stream, js, tag, is_require);
    TarsInputStream_setBuffer(is, JString_data(js), JString_size(js));
    JString_del(&js);

    ret = struct_unpacker_wrapper(is, this_ptr,zv);
    TarsInputStream_del(&is);

    return ret;
}
/* }}} */

/* {{{ struct_unpacker_wrapper(TarsInputStream * stream, zval * this_ptr, void ** zv)
 */
int struct_unpacker_wrapper(TarsInputStream * is, zval * this_ptr,void ** zv) {

    zval *prop, *ret_zv;
    HashTable * props, *tpl_ht = NULL, * fields = NULL;
    int ret = ERR_CANNOT_CONVERT;


    if (!IS_CLASS_STRUCT(this_ptr)) return ERR_CLASS_MISMATCH;
    props = Z_OBJPROP_P(this_ptr);


    // 获取字段信息列表
    zval * tmp = my_zend_read_property(tars_struct_ce, this_ptr, ZEND_STRL(STRUCT_PROP_FIELDS), 1 TSRMLS_CC);
    if (MY_Z_TYPE_P(tmp) == IS_ARRAY) {
        fields = Z_ARRVAL_P(tmp);
    }

    ALLOC_INIT_ZVAL(ret_zv);
    array_init(ret_zv);

    // 思路需要转换，只需要打包那些必须的元素，也就是fields中的内容
#if PHP_MAJOR_VERSION < 7
    for (
            zend_hash_internal_pointer_reset(fields);
            zend_hash_has_more_elements(fields) == SUCCESS;
            zend_hash_move_forward(fields)
            ) {

        char * tag_key;
        int tag_key_len,t;
        ulong tag;
        zval ** attributes, **val, **type,**extType,** complicate_val, *sub = NULL;

        if (zend_hash_get_current_key_ex(fields, &tag_key, &tag_key_len, &tag, 0, NULL) == HASH_KEY_IS_LONG)
        {
            // 获取fields中的属性
            if (zend_hash_get_current_data(fields, (void **)&attributes) == FAILURE) {
                    return ERR_STATIC_FIELDS_PARAM_LOST;
            } else {
                char *name_key;
                uint name_key_len ;
                // 首先获取name的具体名称，作为获取实际值的key
                if (zend_hash_find(Z_ARRVAL_P(*attributes), "name", sizeof("name"), (void **)&val) == SUCCESS) {
                    if (Z_TYPE_PP(val) == IS_STRING) {
                        name_key = Z_STRVAL_PP(val);
                        name_key_len = Z_STRLEN_PP(val);
                    } else {
                        zval copyval = **val;
                        zval_copy_ctor(&copyval);
                        convert_to_string(&copyval);
                        name_key = Z_STRVAL(copyval);
                        name_key_len = Z_STRLEN(copyval);
                        zval_dtor(&copyval);
                    }

                    // 其次获取是否是必选的字段
                    Bool is_required;
                    if (zend_hash_find(Z_ARRVAL_P(*attributes), "required", sizeof("required"), (void **)&val) == SUCCESS) {
                        if (Z_TYPE_PP(val) == IS_BOOL) {
                            is_required = Z_BVAL_PP(val);
                        } else {
                            zval copyval = **val;
                            zval_copy_ctor(&copyval);
                            convert_to_boolean(&copyval);
                            is_required = Z_BVAL(copyval);
                            zval_dtor(&copyval);
                        }

                            // 再次获取具体的类型
                        if (zend_hash_find(props, name_key, strlen(name_key)+1, (void **)&val) == SUCCESS) {
                            if (zend_hash_find(Z_ARRVAL_P(*attributes), "type", sizeof("type"), (void **)&type) == SUCCESS) {
                                int real_type;
                                if (Z_TYPE_PP(type) == IS_LONG) {
                                    real_type = Z_LVAL_PP(type);
                                } else {
                                    zval copyval = **type;
                                    zval_copy_ctor(&copyval);
                                    convert_to_long(&copyval);
                                    real_type = Z_LVAL(copyval);
                                    zval_dtor(&copyval);
                                }

                                zval * complicate_type;

                                // 如果是struct类型
                                if(IS_STRUCT(real_type)) {
                                    ret = struct_unpacker(is, tag, is_required, *val, (void **)&prop);
                                }
                                // 其他复杂类型,包括vector和map, 增加了对extType的兼容
                                else if (!IS_BASE_TYPE(real_type)) {
                                // 需要重新获取extType参数,从而能够获取到vector和map的真正类型
                                    //if (zend_hash_find(Z_ARRVAL_P(*attributes), "extType", sizeof("extType"),
                                    //(void **)&extType) == SUCCESS) {
                                    //    ret = unpacker_dispatch[real_type](is, tag, is_required, *extType, (void **)&prop);
                                    //}
                                    //else ret = unpacker_dispatch[real_type](is, tag, is_required, *val, (void **)&prop);
                                    ret = unpacker_dispatch[real_type](is, tag, is_required, *val, (void **)&prop);
                                }
                                // 普通类型直接取fields里面的type
                                else {
                                    ret = unpacker_dispatch[real_type](is, tag, is_required, *type, (void **)&prop);
                                }

                                if (ret != TARS_SUCCESS) return ret;

                                //if(IS_STRUCT(real_type))
                                //    zend_update_property(tars_struct_ce, this_ptr,name_key, name_key_len,*val);
                                // 希望能够更新到原有的对象上
                                //else zend_update_property(tars_struct_ce, this_ptr,name_key, name_key_len,prop);

                                // 获取其中的值
                                zend_hash_add(Z_ARRVAL_P(ret_zv), name_key, name_key_len+1, (void **)&prop, sizeof(zval *), NULL);
                            }
                            else {
                                return ERR_STATIC_FIELDS_PARAM_LOST;
                            }

                        }
                        else {
                            return ERR_STATIC_FIELDS_PARAM_LOST;
                        }
                    }
                    else {
                        return ERR_STATIC_FIELDS_PARAM_LOST;
                    }
                }
                else {
                    return ERR_REQUIRED_FIELD_LOST;
                }
            }
        }
    }
#else
    //PHP7
    ulong tag;
    zend_string *zkey;
    zval *attributes;

    ZEND_HASH_FOREACH_KEY_VAL(fields, tag, zkey, attributes) {
        if (!attributes) {
            return ERR_STATIC_FIELDS_PARAM_LOST;
        }

        char *name_key;
        zval * val, *type, *sub = NULL, *extType;

        // 首先获取name的具体名称，作为获取实际值的key
        if ((val = zend_hash_str_find(Z_ARRVAL_P(attributes), "name", sizeof("name")-1)) != NULL) {
            if (Z_TYPE_P(val) == IS_STRING) {
                name_key = Z_STRVAL_P(val);
            } else {
                zval copyval = *val;
                zval_copy_ctor(&copyval);
                convert_to_string(&copyval);
                name_key = Z_STRVAL(copyval);
                zval_dtor(&copyval);
            }

            // 其次获取是否是必选的字段
            Bool is_required;
            if ((val = zend_hash_str_find(Z_ARRVAL_P(attributes), "required", sizeof("required")-1)) != NULL) {
                if (Z_TYPE_P(val) == IS_TRUE || Z_TYPE_P(val) == IS_FALSE) {
                    is_required = (Z_TYPE_P(val) == IS_TRUE ? true : false);
                } else {
                    zval copyval = *val;
                    zval_copy_ctor(&copyval);
                    convert_to_boolean(&copyval);
                    is_required = (Z_TYPE_P(val) == IS_TRUE ? true : false);
                    zval_dtor(&copyval);
                }

                // 再次获取具体的类型
                // 获取其中的值 //TODO change old code ?
                if ((val = my_zend_read_property(tars_struct_ce, this_ptr, name_key, strlen(name_key), 1 TSRMLS_CC))) {
                    if ((type = zend_hash_str_find(Z_ARRVAL_P(attributes), "type", sizeof("type")-1)) != NULL) {
                        int real_type;
                        if (Z_TYPE_P(type) == IS_LONG) {
                            real_type = Z_LVAL_P(type);
                        } else {
                            zval copyval = *type;
                            zval_copy_ctor(&copyval);
                            convert_to_long(&copyval);
                            real_type = Z_LVAL(copyval);
                            zval_dtor(&copyval);
                        }

                        zval * complicate_type;

                        // 如果是struct类型
                        if(IS_STRUCT(real_type)) {
                            ret = struct_unpacker(is, tag, is_required, val, (void **)&prop);
                        }
                            // 其他复杂类型,包括vector和map
                        else if (!IS_BASE_TYPE(real_type)) {
                            ret = unpacker_dispatch[real_type](is, tag, is_required, val, (void **)&prop);
                        }
                            // 普通类型直接取fields里面的type
                        else {
                            ret = unpacker_dispatch[real_type](is, tag, is_required, type, (void **)&prop);
                        }


                        if (ret != TARS_SUCCESS) return ret;
                        // 获取其中的值
                        my_zend_hash_add(Z_ARRVAL_P(ret_zv), name_key, strlen(name_key)+1, (void **)&prop, sizeof(zval *), NULL);
                    }
                    else {
                        return ERR_STATIC_FIELDS_PARAM_LOST;
                    }

                }
                else {
                    return ERR_STATIC_FIELDS_PARAM_LOST;
                }
            }
            else {
                return ERR_STATIC_FIELDS_PARAM_LOST;
            }
        }
        else {
            return ERR_REQUIRED_FIELD_LOST;
        }

    } ZEND_HASH_FOREACH_END();
#endif

    * zv = ret_zv;
    return TARS_SUCCESS;
}
/* }}} */

/* {{{ __CYCLE_PACKER
 */
#define __CYCLE_PACKER(type, item, prop) \
    if (!IS_BASE_TYPE(type) && MY_Z_TYPE_PP(item) != IS_OBJECT) { \
        zval * tmp = my_zend_read_property(Z_OBJCE_P(this_ptr), this_ptr, ZEND_STRL(prop), 1 TSRMLS_CC); \
        if (!ZVAL_IS_NULL(tmp)) { \
            if (TTARS_TYPE_VECTOR == type) { \
                ret = vector_converter(tmp, *item, ++depth); \
            } else if (TTARS_TYPE_MAP == type) { \
                ret = map_converter(tmp, *item, ++depth); \
            } else { \
                return; \
            } \
            if (ret != TARS_SUCCESS) { \
                goto do_clean ; \
            } \
            **item = *tmp; \
            zval_copy_ctor(*item); \
        } \
    } \
/* }}} */

/* {{{ vector_converter(zval * this_ptr, zval * value, int tag, int depth)
 */
int vector_converter(zval * this_ptr, zval * value, int depth) {

    tars_pack_func_t packer;
    HashTable * ht;
    JArray * container = NULL;
    JString * js = NULL;
    my_smart_str js_str = {0};
    int ret = TARS_SUCCESS, free_zv = 0;
    zval * zv = NULL;
    char c;

    vector_wrapper * obj = Z_VECTOR_WRAPPER_P(this_ptr TSRMLS_CC);
    if (!IS_VALID_TYPE(obj->t)) {
        return ERR_TYPE_INVALID;
    }

    TarsOutputStream * out = TarsOutputStream_new();
    if (!out) return TARS_MALLOC_ERROR;
    packer = packer_dispatch[obj->t];

    if (IS_JSTRING(obj->t)) {
        js = obj->ctx->str;
        if (Z_TYPE_P(value) != IS_ARRAY) {
            // 需转成字符串
            zv = value;
            goto string_pack;
        }
    } else {
        container = obj->ctx->vct;
        if (Z_TYPE_P(value) != IS_ARRAY) {
            return ERR_DATA_FORMAT_ERROR;
        }
    }

    ht = Z_ARRVAL_P(value);

#if PHP_MAJOR_VERSION < 7
    for(
            zend_hash_internal_pointer_reset(ht);
            zend_hash_has_more_elements(ht) == SUCCESS;
            zend_hash_move_forward(ht))
    {
        zval ** item;
        if (zend_hash_get_current_data(ht, (void**)&item) == FAILURE) {
            continue;
        }

        if (IS_JSTRING(obj->t)) {
            ret = packer(*item, NULL, 0, &c);
            if (ret != TARS_SUCCESS) goto do_clean;

            // 将单个字符拼成字符串串
            smart_str_appendc(&js_str, c);
        }
        // todo 这里针对struct的逻辑
        // 增加对结构体的单独处理
        else if(IS_STRUCT(obj->t)) {
            ret = packer (NULL,out, 0, *item);
            if (ret != TARS_SUCCESS) goto do_clean;
            ret = JArray_pushBack(container, TarsOutputStream_getBuffer(out), TarsOutputStream_getLength(out));
            if (ret != TARS_SUCCESS) goto do_clean;
            TarsOutputStream_reset(out);
        }
        else {
            __CYCLE_PACKER(obj->t, item, TARS_PROP_TYPE_CLASS);
            ret = packer (*item, out, 0, NULL);
            if (ret != TARS_SUCCESS) goto do_clean;
            ret = JArray_pushBack(container, TarsOutputStream_getBuffer(out), TarsOutputStream_getLength(out));
            if (ret != TARS_SUCCESS) goto do_clean;
            TarsOutputStream_reset(out);
        }
    }
#else
    //PHP7
    zend_string *zkey;
    zval *item;
    ulong num_key;
    zval itemTmp;

    ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, zkey, item) {
        if (item) {
            ZVAL_COPY(&itemTmp, item);

            if (IS_JSTRING(obj->t)) {
                ret = packer(item, NULL, 0, &c);
                if (ret != TARS_SUCCESS) goto do_clean;

                // 将单个字符拼成字符串串
                smart_string_appendc(&js_str, c);
            }
                // todo 这里针对struct的逻辑
                // 增加对结构体的单独处理
            else if(IS_STRUCT(obj->t)) {
                ret = packer (NULL,out, 0, item);
                if (ret != TARS_SUCCESS) goto do_clean;
                ret = JArray_pushBack(container, TarsOutputStream_getBuffer(out), TarsOutputStream_getLength(out));
                if (ret != TARS_SUCCESS) goto do_clean;
                TarsOutputStream_reset(out);
            }
            else {
                __CYCLE_PACKER(obj->t, (zval **)&item, TARS_PROP_TYPE_CLASS);
                ret = packer (item, out, 0, NULL);
                if (ret != TARS_SUCCESS) goto do_clean;
                ret = JArray_pushBack(container, TarsOutputStream_getBuffer(out), TarsOutputStream_getLength(out));
                if (ret != TARS_SUCCESS) goto do_clean;
                TarsOutputStream_reset(out);
            }
        }
    } ZEND_HASH_FOREACH_END();
#endif

string_pack :

    if (IS_JSTRING(obj->t)) {
        if (!zv) {
            ALLOC_INIT_ZVAL(zv);
            MY_ZVAL_STRINGL(zv, js_str.c, js_str.len, 0);
            free_zv = 1;
        }

        ret = string_packer(zv, NULL, 0, NULL);
        if (ret != TARS_SUCCESS) goto do_clean;
        JString_append(js, Z_STRVAL_P(zv), Z_STRLEN_P(zv));
    }

do_clean :
    TarsOutputStream_del(&out);
    if (free_zv) my_zval_ptr_dtor(&zv);
    return ret;
}
/* }}} */

/* {{{  map_converter(zval * this_ptr, zval * zv, int tag, int depth)
 */
int map_converter(zval * this_ptr, zval * zv, int depth) {

    zval *type, *key_zv = NULL, *value_zv = NULL;
    HashTable * ht;
    int ftype, stype, ret = 0, format = 0;
    JMapWrapper * container;
    tars_pack_func_t fpacker, spacker;

    if (Z_TYPE_P(zv) != IS_ARRAY) {
        return ERR_DATA_FORMAT_ERROR;
    }
    ht = Z_ARRVAL_P(zv);

    // 第一项参数类型
    type = my_zend_read_property(tars_map_ce, this_ptr, ZEND_STRL(MAP_PROP_FIRST_TYPE), 1 TSRMLS_CC);
    if (Z_TYPE_P(type) != IS_LONG || !IS_VALID_TYPE(Z_LVAL_P(type))) return ERR_WRONG_PARAMS;
    ftype = Z_LVAL_P(type);

    // 第二个参数类型
    type = my_zend_read_property(tars_map_ce, this_ptr, ZEND_STRL(MAP_PROP_SECOND_TYPE), 1 TSRMLS_CC);
    if (Z_TYPE_P(type) != IS_LONG || !IS_VALID_TYPE(Z_LVAL_P(type))) return ERR_WRONG_PARAMS;
    stype = Z_LVAL_P(type);

    type = my_zend_read_property(tars_map_ce, this_ptr, ZEND_STRL(MAP_PROP_PARAM_FORMAT), 1 TSRMLS_CC);
    if ((MY_Z_TYPE_P(type) == IS_TRUE)) {
        format = 1;
    }

    // 打包器,第一个一般不会出现特殊的结构，但是第二个会的。
    fpacker = packer_dispatch[ftype];
    spacker = packer_dispatch[stype];

    TarsOutputStream *fStream  = TarsOutputStream_new();
    if (!fStream) return ERR_MALLOC_FAILED;

    TarsOutputStream *sStream  = TarsOutputStream_new();
    if (!sStream) { TarsOutputStream_del(&fStream); return ERR_MALLOC_FAILED;}

    map_wrapper * obj = Z_MAP_WRAPPER_P(this_ptr TSRMLS_CC);
    container = obj->ctx;

#if PHP_MAJOR_VERSION < 7
    for(
            zend_hash_internal_pointer_reset(ht);
            zend_hash_has_more_elements(ht) == SUCCESS;
            zend_hash_move_forward(ht))
    {
        zval ** key, **item, **value;
        char * str;
        ulong idx;
        int key_type;
        uint str_len;

        if (!format) {
            // 参数是以关联数组的方式传即 数组的键是map的第一个元素, 数组的值是第二个元素

            key_type = zend_hash_get_current_key_ex(ht, &str, &str_len, &idx, 0, NULL);
            // 取一条记录
            if (zend_hash_get_current_data(ht, (void**)&value) == FAILURE) {
                continue;
            }

            // 申请一个zval
            ALLOC_INIT_ZVAL(key_zv);

            if (key_type == HASH_KEY_IS_STRING) {
                // 复制一份字符串
                ZVAL_STRINGL(key_zv, str, str_len - 1, 1);
            } else {
                ZVAL_LONG(key_zv, idx);
            }
            key = &key_zv;

        } else {

            // 参数是以二维数组的方式传即 数组中的键key对应的值是map的第一个元素, 键value对应的值是map的第二个元素

            // 取一条记录
            if (zend_hash_get_current_data(ht, (void**)&item) == FAILURE) {
                continue;
            }

            // 检查参数是否正确
            if (Z_TYPE_PP(item) != IS_ARRAY ||
                    zend_hash_find(Z_ARRVAL_PP(item), ZEND_STRS(MAP_FIRST_KEY), (void **)&key) == FAILURE ||
                    zend_hash_find(Z_ARRVAL_PP(item), ZEND_STRS(MAP_SECOND_KEY), (void **)&value) == FAILURE) {
                ret = ERR_DATA_FORMAT_ERROR;
                goto do_clean;
            }
        }

        TarsOutputStream_reset(fStream);
        __CYCLE_PACKER(ftype, key, MAP_FIRST_TYPE_CLASS);
        ret = fpacker (*key, fStream, 0, NULL);
        if (ret != TARS_SUCCESS) goto do_clean;

        TarsOutputStream_reset(sStream);

        // 针对结构体，进行特殊处理
        if(IS_STRUCT(stype)) {
            ret = spacker (NULL,sStream, 1, *value);
        }
        else {
            __CYCLE_PACKER(stype, value, MAP_SECOND_TYPE_CLASS);
            ret = spacker (*value, sStream, 1, NULL);
        }
        if (ret != TARS_SUCCESS) goto do_clean;

        ret = JMapWrapper_put(container, TarsOutputStream_getBuffer(fStream), TarsOutputStream_getLength(fStream),
                TarsOutputStream_getBuffer(sStream), TarsOutputStream_getLength(sStream));
        if (ret != TARS_SUCCESS) goto do_clean;
        if (key_zv) {zval_ptr_dtor(&key_zv); key_zv = NULL; }
    }
#else
    //PHP7
    zend_string * zskey;
    ulong num_key;

    zval * key, *value, *item;

    ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, zskey, item) {
        if (!item) {
            continue;
        }

        if (!format) {

            // 申请一个zval
            ALLOC_INIT_ZVAL(key_zv);

            // 参数是以关联数组的方式传即 数组的键是map的第一个元素, 数组的值是第二个元素
            if (zskey) {
                ZVAL_STRINGL(key_zv, ZSTR_VAL(zskey), strlen(ZSTR_VAL(zskey)));
            } else {
                ZVAL_LONG(key_zv, num_key);
            }

            key = key_zv;

            ALLOC_INIT_ZVAL(value_zv);
            ZVAL_COPY(value_zv, item);
            value = value_zv;
        } else {
            // 参数是以二维数组的方式传即 数组中的键key对应的值是map的第一个元素, 键value对应的值是map的第二个元素
            // 取一条记录

            // 检查参数是否正确
            if (Z_TYPE_P(item) != IS_ARRAY ||
                ((key = (zval *) zend_hash_str_find(Z_ARRVAL_P(item), ZEND_STRL(MAP_FIRST_KEY))) == NULL) ||
                ((value = (zval *) zend_hash_str_find(Z_ARRVAL_P(item), ZEND_STRL(MAP_SECOND_KEY))) == NULL)) {
                ret = ERR_DATA_FORMAT_ERROR;
                goto do_clean;
            }
        }

        TarsOutputStream_reset(fStream);
        __CYCLE_PACKER(ftype, (zval **)&key, MAP_FIRST_TYPE_CLASS);
        ret = fpacker(key, fStream, 0, NULL);
        if (ret != TARS_SUCCESS) goto do_clean;

        TarsOutputStream_reset(sStream);

        // 针对结构体，进行特殊处理
        if (IS_STRUCT(stype)) {
            ret = spacker(NULL, sStream, 1, value);
        } else {
            __CYCLE_PACKER(stype, (zval **) &value, MAP_SECOND_TYPE_CLASS);
            ret = spacker(value, sStream, 1, NULL);
        }
        if (ret != TARS_SUCCESS) goto do_clean;

        ret = JMapWrapper_put(container, TarsOutputStream_getBuffer(fStream), TarsOutputStream_getLength(fStream),
                              TarsOutputStream_getBuffer(sStream), TarsOutputStream_getLength(sStream));
        if (ret != TARS_SUCCESS) goto do_clean;
        if (key_zv) {
            my_zval_ptr_dtor(&key_zv);
            key_zv = NULL;
        }
        if (value_zv) {
            my_zval_ptr_dtor(&value_zv);
            value_zv = NULL;
        }
    } ZEND_HASH_FOREACH_END();
#endif

do_clean :
    if (key_zv) my_zval_ptr_dtor(&key_zv);
    if (value_zv) my_zval_ptr_dtor(&value_zv);

    TarsOutputStream_del(&fStream);
    TarsOutputStream_del(&sStream);
    return ret;
}
/* }}} */

/* {{{ tars_type_name(zval * this_ptr, int * type, char **out)
 */
size_t tars_type_name(zval * this_ptr, int * type, char ** out) {

    zval * name = NULL;
    char * type_name;
    size_t len;


    if (Z_TYPE_P(this_ptr) == IS_OBJECT) {
        zval * orig_type = zend_read_static_property(Z_OBJCE_P(this_ptr), ZEND_STRL(TARS_PROP_ORIG_TYPE), 1 TSRMLS_CC);

#if PHP_MAJOR_VERSION >= 7
        if (Z_TYPE_P(orig_type) == IS_REFERENCE) {
            orig_type = Z_REFVAL_P(orig_type);
        }
#endif

        if (Z_TYPE_P(orig_type) == IS_LONG) {
            * type = Z_LVAL_P(orig_type);
            if (IS_BASE_TYPE(*type)) {
                return 0;
            }

            name = my_zend_read_property(Z_OBJCE_P(this_ptr), this_ptr, ZEND_STRL(TARS_PROP_TYPE_NAME), 1 TSRMLS_CC);
            if (Z_TYPE_P(name) != IS_STRING) {
                return 0;
            }
        }
    } else if (Z_TYPE_P(this_ptr) == IS_LONG) {
        * type = Z_LVAL_P(this_ptr);
        if (!IS_BASE_TYPE(*type)) {
            return 0;
        }
    }

    switch (*type) {
        case TTARS_TYPE_BOOL :
            len = spprintf(&type_name, 0, "bool");
            break;
        case TTARS_TYPE_CHAR :
            len = spprintf(&type_name, 0, "char");
            break;
        case TTARS_TYPE_UINT8 :
            len = spprintf(&type_name, 0, "uint8");
            break;
        case TTARS_TYPE_SHORT :
            len = spprintf(&type_name, 0, "short");
            break;
        case TTARS_TYPE_UINT16 :
            len = spprintf(&type_name, 0, "uint16");
            break;
        case TTARS_TYPE_FLOAT :
            len = spprintf(&type_name, 0, "float");
            break;
        case TTARS_TYPE_DOUBLE :
            len = spprintf(&type_name, 0, "double");
            break;
        case TTARS_TYPE_INT32 :
            len = spprintf(&type_name, 0, "int32");
            break;
        case TTARS_TYPE_UINT32 :
            len = spprintf(&type_name, 0, "uint32");
            break;
        case TTARS_TYPE_INT64 :
            len = spprintf(&type_name, 0, "int64");
            break;
        case TTARS_TYPE_STRING :
            len = spprintf(&type_name, 0, "string");
            break;
        case TTARS_TYPE_STRUCT :
            len = spprintf(&type_name, 0, "%s", Z_STRVAL_P(name));
            break;
        case TTARS_TYPE_VECTOR :
            len = spprintf(&type_name, 0, "list<%s>", Z_STRVAL_P(name));
            break;
        case TTARS_TYPE_MAP :
            len = spprintf(&type_name, 0, "map<%s>", Z_STRVAL_P(name));
            break;
        default :
            return 0;
    }

    if (out) {
        *out = type_name;
    } else {
        efree(type_name);
    }

    return len;
}
/* }}} */

/* {{{ vector_wrapper_dtor(void * object)
 */
static void vector_wrapper_dtor(zend_object * object TSRMLS_DC) {
#if PHP_MAJOR_VERSION < 7
    vector_wrapper * obj = (vector_wrapper *) object;
#else
    vector_wrapper * obj = vector_wrapper_fetch_object(object);
#endif

    if (obj->ctx) {
        if (obj->t == TTARS_TYPE_CHAR) {
            JString_del(&obj->ctx->str);
        } else {
            JArray_del(&obj->ctx->vct);
        }

        efree(obj->ctx);
    }

    zend_object_std_dtor(&obj->std TSRMLS_CC);
    //efree(obj);
}
/* }}} */

/* {{{ map_wrapper_dtor(void * object TSRMLS_DC)
 */
static void map_wrapper_dtor(zend_object * object TSRMLS_DC) {
#if PHP_MAJOR_VERSION < 7
    map_wrapper * obj = (map_wrapper *) object;
#else
    map_wrapper * obj = map_wrapper_fetch_object(object);
#endif

    if (obj->ctx) {
        JMapWrapper_del(&obj->ctx);
    }

    zend_object_std_dtor(&obj->std TSRMLS_CC);
//    efree(obj);
}
/* }}} */

/* {{{  vector_wrapper_object_new(zend_class_entry * clazz TSRMLS_DC)
 */
#if PHP_MAJOR_VERSION < 7
static inline zend_object_value vector_wrapper_object_new(zend_class_entry * clazz TSRMLS_DC) {

    zval * tmp;
    zend_object_value retval;
    vector_wrapper * obj = (vector_wrapper *)emalloc(sizeof(vector_wrapper));
    memset(obj, 0, sizeof(vector_wrapper));

    zend_object_std_init(&obj->std, clazz TSRMLS_CC);

#if PHP_VERSION_ID >=50400
    object_properties_init( (zend_object *) obj, clazz);
#else
    {
        zend_hash_copy(obj->std.properties, &clazz->properties_info,(copy_ctor_func_t)zval_add_ref, (void *) &tmp, sizeof(zval *)
);
    }
#endif

    retval.handle = zend_objects_store_put(obj, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t)vector_wrapper_dtor, NULL TSRMLS_CC);
    retval.handlers = &vector_wrapper_handlers;

    return retval;
}
#else
static inline zend_object * vector_wrapper_object_new(zend_class_entry * clazz) {
    vector_wrapper * obj = (vector_wrapper *) ecalloc(1, sizeof(vector_wrapper) + zend_object_properties_size(clazz));

//    memset(obj, 0, sizeof(vector_wrapper));
//    vector_wrapper * obj = ecalloc(1, sizeof(vector_wrapper) + zend_object_properties_size(clazz));

    zend_object_std_init(&obj->std, clazz TSRMLS_CC);
    object_properties_init(&obj->std, clazz);

    vector_wrapper_handlers.offset = XtOffsetOf(vector_wrapper, std);
    vector_wrapper_handlers.free_obj = (zend_object_free_obj_t) vector_wrapper_dtor;

    obj->std.handlers = &vector_wrapper_handlers;

    return &obj->std;
}

#endif
/* }}} */

/* {{{ map_wrapper_object_new(zend_class_entry * clazz TSRMLS_DC)
 */
#if PHP_MAJOR_VERSION < 7
static inline zend_object_value map_wrapper_object_new(zend_class_entry * clazz TSRMLS_DC) {

    zval * tmp;
    zend_object_value retval;
    map_wrapper * obj = (map_wrapper *)emalloc(sizeof(map_wrapper));
    memset(obj, 0, sizeof(map_wrapper));

    zend_object_std_init(&obj->std, clazz TSRMLS_CC);

#if PHP_VERSION_ID >=50400
    object_properties_init( (zend_object *) obj, clazz);
#else
    {
        zend_hash_copy(obj->std.properties, &clazz->properties_info,(copy_ctor_func_t)zval_add_ref, (void *) &tmp, sizeof(zval *)
);
    }
#endif

    retval.handle = zend_objects_store_put(obj, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) map_wrapper_dtor, NULL TSRMLS_CC);
    retval.handlers = &map_wrapper_handlers;

    return retval;
}
#else
static inline zend_object * map_wrapper_object_new(zend_class_entry * clazz TSRMLS_DC) {
    map_wrapper * obj = (map_wrapper *) ecalloc(1, sizeof(map_wrapper) + zend_object_properties_size(clazz));
//    map_wrapper * obj = (map_wrapper *)emalloc(sizeof(map_wrapper));
//    memset(obj, 0, sizeof(map_wrapper));

    zend_object_std_init(&obj->std, clazz TSRMLS_CC);
    object_properties_init(&obj->std, clazz);

    map_wrapper_handlers.offset = XtOffsetOf(map_wrapper, std);
    map_wrapper_handlers.free_obj = (zend_object_free_obj_t) map_wrapper_dtor;

    obj->std.handlers = &map_wrapper_handlers;

    return &obj->std;
}
#endif
/* }}} */

/* {{{  TTARS ARG INFO
 */
ZEND_BEGIN_ARG_INFO_EX(vector_ctor_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, clazz)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(vector_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(map_ctor_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, fclazz)
    ZEND_ARG_INFO(0, sclazz)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(map_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, value, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(struct_ctor_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, class_name)
    ZEND_ARG_ARRAY_INFO(0, fields, 0)
ZEND_END_ARG_INFO()

zend_function_entry tars_methods[] = {
    {NULL, NULL, NULL}
};
/* }}} */

/* {{{ method list
 */
zend_function_entry tars_vector_methods[] = {
    PHP_ME(tars_vector, __construct, vector_ctor_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(tars_vector, pushBack, vector_arginfo, ZEND_ACC_PUBLIC)
    PHP_MALIAS(tars_vector, push_back, pushBack, vector_arginfo, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

zend_function_entry tars_map_methods[] = {
    PHP_ME(tars_map, __construct, map_ctor_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(tars_map, pushBack,map_arginfo, ZEND_ACC_PUBLIC)
    PHP_MALIAS(tars_map, push_back, pushBack,map_arginfo, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

zend_function_entry tars_struct_methods[] = {
    PHP_ME(tars_struct, __construct, struct_ctor_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    {NULL, NULL, NULL}
};
/* }}} */

/* {{{ TUP_STARTUP_FUNC
 */
TUP_STARTUP_FUNC(ttars) {

    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "TARS", tars_methods);
    tars_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_class_constant_long(tars_ce, ZEND_STRL(PHP_TTARS_BOOL), TTARS_TYPE_BOOL);
    zend_declare_class_constant_long(tars_ce, ZEND_STRL(PHP_TTARS_CHAR), TTARS_TYPE_CHAR);
    zend_declare_class_constant_long(tars_ce, ZEND_STRL(PHP_TTARS_UINT8), TTARS_TYPE_UINT8);
    zend_declare_class_constant_long(tars_ce, ZEND_STRL(PHP_TTARS_SHORT), TTARS_TYPE_SHORT);
    zend_declare_class_constant_long(tars_ce, ZEND_STRL(PHP_TTARS_UINT16), TTARS_TYPE_UINT16);
    zend_declare_class_constant_long(tars_ce, ZEND_STRL(PHP_TTARS_FLOAT), TTARS_TYPE_FLOAT);
    zend_declare_class_constant_long(tars_ce, ZEND_STRL(PHP_TTARS_DOUBLE), TTARS_TYPE_DOUBLE);
    zend_declare_class_constant_long(tars_ce, ZEND_STRL(PHP_TTARS_INT32), TTARS_TYPE_INT32);
    zend_declare_class_constant_long(tars_ce, ZEND_STRL(PHP_TTARS_UINT32), TTARS_TYPE_UINT32);
    zend_declare_class_constant_long(tars_ce, ZEND_STRL(PHP_TTARS_INT64), TTARS_TYPE_INT64);
    zend_declare_class_constant_long(tars_ce, ZEND_STRL(PHP_TTARS_STRING), TTARS_TYPE_STRING);
    zend_declare_class_constant_long(tars_ce, ZEND_STRL(PHP_TTARS_VECTOR), TTARS_TYPE_VECTOR);
    zend_declare_class_constant_long(tars_ce, ZEND_STRL(PHP_TTARS_MAP), TTARS_TYPE_MAP);
    zend_declare_class_constant_long(tars_ce, ZEND_STRL(PHP_TTARS_STRUCT), TTARS_TYPE_STRUCT);

    INIT_CLASS_ENTRY(ce, "TARS_Vector", tars_vector_methods);
    ce.create_object = vector_wrapper_object_new;
    tars_vector_ce = zend_register_internal_class(&ce TSRMLS_CC);
    memcpy(&vector_wrapper_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    vector_wrapper_handlers.clone_obj = NULL;
    zend_declare_property_long(tars_vector_ce, ZEND_STRL(TARS_PROP_ORIG_TYPE), VECTOR_PROP_ORIG_TYPE, ZEND_ACC_PRIVATE | ZEND_ACC_STATIC TSRMLS_CC);

    INIT_CLASS_ENTRY(ce, "TARS_Map", tars_map_methods);
    ce.create_object = map_wrapper_object_new;
    tars_map_ce = zend_register_internal_class(&ce TSRMLS_CC);
    memcpy(&map_wrapper_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    map_wrapper_handlers.clone_obj = NULL;
    zend_declare_property_long(tars_map_ce, ZEND_STRL(TARS_PROP_ORIG_TYPE), MAP_PROP_ORIG_TYPE, ZEND_ACC_PRIVATE | ZEND_ACC_STATIC TSRMLS_CC);
    zend_declare_property_bool(tars_map_ce, ZEND_STRL(MAP_PROP_PARAM_FORMAT), 0, ZEND_ACC_PRIVATE TSRMLS_CC);

    INIT_CLASS_ENTRY(ce, "TARS_Struct", tars_struct_methods);
    tars_struct_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_long(tars_struct_ce, ZEND_STRL(TARS_PROP_ORIG_TYPE), STRUCT_PROP_ORIG_TYPE, ZEND_ACC_PROTECTED | ZEND_ACC_STATIC TSRMLS_CC);

    return SUCCESS;
}
/* }}} */

/* {{{ TTARS_Vector::__construct(mixed $type)
 */
PHP_METHOD(tars_vector, __construct) {

    zval * clazz;
    char * name = NULL;
    int type;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &clazz) == FAILURE) {
        ZEND_WRONG_PARAM_COUNT();
        return ;
    }

    tars_type_name(clazz, &type, &name);
    if (!name) return TYPE_EXCEPTOIN();

    vector_wrapper * obj = Z_VECTOR_WRAPPER_P(getThis() TSRMLS_CC);
    obj->ctx = (vector_ctx *) ecalloc(1, sizeof(vector_ctx));
    if (IS_JSTRING(type)) {
        obj->ctx->str = JString_new();
    } else {
        obj->ctx->vct = JArray_new(name);
    }
    obj->t = type;

    zend_update_property(tars_vector_ce, getThis(), ZEND_STRL(VECTOR_PROP_TYPE_CLASS), clazz TSRMLS_CC);
    zend_update_property_long(tars_vector_ce, getThis(), ZEND_STRL(VECTOR_PROP_TYPE), type TSRMLS_CC);

    zend_update_property_string(tars_vector_ce, getThis(), ZEND_STRL(TARS_PROP_TYPE_NAME), name TSRMLS_CC);
    efree(name);

    RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */



/* {{{ TTARS_Vector::push_back(mixed $param)
 */
PHP_METHOD(tars_vector, pushBack) {

    zval * value = NULL, * sub = NULL;
    JString * js = NULL;
    JArray * vct = NULL;
    TarsOutputStream * out = NULL;
    int ret = 0;
    char b ;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value) == FAILURE) {
        ZEND_WRONG_PARAM_COUNT();
        return;
    }

    vector_wrapper * obj = Z_VECTOR_WRAPPER_P(getThis() TSRMLS_CC);
    if (!IS_VALID_TYPE(obj->t)) return UNINIT_EXCEPTION(tars_vector_ce->name);

    if (IS_JSTRING(obj->t)) {
        js = obj->ctx->str;

    } else {
        vct = obj->ctx->vct;
        out = TarsOutputStream_new();
        if (!out) return MALLOC_EXCEPTION("Vector");
    }
    // type struct,同样是数组,但是在php层面转换
    if(IS_STRUCT(obj->t)) {
            ret = struct_packer(NULL,out, 0, value);
    }
    else if(!IS_BASE_TYPE(obj->t)) {
        sub = my_zend_read_property(tars_vector_ce, getThis(), ZEND_STRL(TARS_PROP_TYPE_CLASS), 1 TSRMLS_CC);
        ret = packer_dispatch[obj->t](value, out, 0, sub);
    }
    else ret = packer_dispatch[obj->t](value, out, 0, (void *)&b);
    if (ret != TARS_SUCCESS) {
        PACK_EXCEPTION(ret, "Vector");
        goto do_clean;
    }

    if (IS_JSTRING(obj->t)) {
        ret = JString_append(js, &b, 1);
    } else {
        ret = JArray_pushBack(vct, TarsOutputStream_getBuffer(out), TarsOutputStream_getLength(out));
    }

    if (ret != TARS_SUCCESS) {
        INTERNET_EXCEPTION(ret);
        goto do_clean;
    }

    RETVAL_ZVAL(getThis(), 1, 0);

do_clean :
    if (out) TarsOutputStream_del(&out);
}
/* }}} */

/*
实现计数的方法
 */
PHP_METHOD(tars_vector, count) {
    zval * count_p = my_zend_read_property(tars_vector_ce, getThis(), ZEND_STRL(TARS_PROP_VEC_COUNT), 1 TSRMLS_CC);
    int count = Z_LVAL_P(count_p);
    RETURN_LONG(count);
}


/* }}} */

/* {{{ TTARS_Map::__construct(mixed $first, mixed $second[, bool $is_complex = false])
 */
PHP_METHOD(tars_map, __construct) {

    zval * fclazz, * sclazz;
    char * fname = NULL, * sname = NULL;
    int ftype, stype;
    zend_bool format = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|b", &fclazz, &sclazz, &format) == FAILURE) {
        ZEND_WRONG_PARAM_COUNT();
        return ;
    }

    tars_type_name(fclazz, &ftype, &fname);
    tars_type_name(sclazz, &stype, &sname);
    if (!fname || !sname) return TYPE_EXCEPTOIN();

    char * type_name;
    spprintf(&type_name, 0, "%s,%s", fname, sname);
    zend_update_property_string(tars_map_ce, getThis(), ZEND_STRL(TARS_PROP_TYPE_NAME), type_name TSRMLS_CC);

    map_wrapper * obj = Z_MAP_WRAPPER_P(getThis() TSRMLS_CC);
    obj->ctx = JMapWrapper_new(fname, sname);
    if (!obj->ctx) return MALLOC_EXCEPTION("Map");

    // 类型名
    zend_update_property_string(tars_map_ce, getThis(), ZEND_STRL(MAP_FIRST_TYPE_NAME), fname TSRMLS_CC);
    zend_update_property_string(tars_map_ce, getThis(), ZEND_STRL(MAP_SECOND_TYPE_NAME), sname TSRMLS_CC);

    // 类型值
    zend_update_property_long(tars_map_ce, getThis(), ZEND_STRL(MAP_PROP_FIRST_TYPE), ftype TSRMLS_CC);
    zend_update_property_long(tars_map_ce, getThis(), ZEND_STRL(MAP_PROP_SECOND_TYPE), stype TSRMLS_CC);

    // 类型对象
    zend_update_property(tars_map_ce, getThis(), ZEND_STRL(MAP_FIRST_TYPE_CLASS), fclazz TSRMLS_CC);
    zend_update_property(tars_map_ce, getThis(), ZEND_STRL(MAP_SECOND_TYPE_CLASS), sclazz TSRMLS_CC);

    // 打包解包方式
    if (format) {
        zend_update_property_bool(tars_map_ce, getThis(), ZEND_STRL(MAP_PROP_PARAM_FORMAT), 1 TSRMLS_CC);
    }

    efree(fname);
    efree(sname);
    efree(type_name);

    RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/* {{{ TTARS_Map::pushBack(array $param)
 */
PHP_METHOD(tars_map, pushBack) {

    zval *type, * value, * array = NULL;
    int ret;
    int format = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &value) == FAILURE) {
        ZEND_WRONG_PARAM_COUNT();
        return ;
    }

    type = my_zend_read_property(tars_map_ce, getThis(), ZEND_STRL(MAP_PROP_PARAM_FORMAT), 1 TSRMLS_CC);
    if (MY_Z_TYPE_P(type) == IS_TRUE) {
        format = 1;
    }

    if (format) {

        // 模拟成添加多个记录
        ALLOC_INIT_ZVAL(array);
        array_init(array);

        // 增加计数
#if PHP_MAJOR_VERSION <7
        Z_ADDREF_P(value);
#else
        Z_TRY_ADDREF_P(value);
#endif

        my_zend_hash_next_index_insert(Z_ARRVAL_P(array), (void **) &value, sizeof(zval *), NULL);
    } else {
        array = value;
    }

    ret = map_converter(getThis(), array, 0);

    if(format) my_zval_ptr_dtor(&array);

    if (ret != TARS_SUCCESS) return CONVERT_EXCEPTION(ret, "Map");

    RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */


/* {{{ TTARS_Struct::__construct(string $name, array $fields)
 */
PHP_METHOD(tars_struct, __construct) {

    char * class_name;
    zend_size_t class_len;
    zval * fields = NULL; // 所有字段信息列表

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa",&class_name, &class_len ,&fields ) == FAILURE) {
        ZEND_WRONG_PARAM_COUNT();
        return;
    }

    if (class_len < STRUCT_NAME_MIN || class_len > STRUCT_NAME_MAX) {
        tup_throw_exception(ERR_WRONG_PARAMS, "class %s name length exceeds the limit.", Z_OBJCE_P(getThis())->name);
        return ;
    }

    if (fields) {
        // 所有字段信息
        zend_update_property(tars_struct_ce, getThis(), ZEND_STRL(STRUCT_PROP_FIELDS), fields TSRMLS_CC);
    }

    zend_update_property_string(tars_struct_ce, getThis(), ZEND_STRL(TARS_PROP_TYPE_NAME), class_name TSRMLS_CC);
    RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */
