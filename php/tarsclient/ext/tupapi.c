/*
 *
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2010 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: vinsonliu@tencent.com                                        |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include "php.h"
#include "php_ini.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"
#include "ext/standard/info.h"

#include "./include/tup_c.h"
#include "./include/tars_c.h"
#include "./include/php_tupapi.h"
#include "./include/ttars.h"


ZEND_BEGIN_ARG_INFO_EX(tup_put_vector_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_OBJ_INFO(0, clazz, \\TARS_Vector, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(tup_get_vector_arginfo, 0, 0, 3)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_OBJ_INFO(1, clazz, \\TARS_Vector, 0)
    ZEND_ARG_INFO(1, buf)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(tup_put_map_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_OBJ_INFO(0, clazz, \\TARS_Map, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(tup_get_map_arginfo, 0, 0, 3)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_OBJ_INFO(1, clazz, \\TARS_Map, 0)
    ZEND_ARG_INFO(1, buf)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(tup_put_common_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(tup_get_common_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(1, buf)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(tup_get_struct_arginfo, 0,0,3)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(1, clazz)
    ZEND_ARG_INFO(1, buf)
ZEND_END_ARG_INFO()


/* {{{ tup_methods[]
 *
 * Every user visible function must have an entry in ttup_functions[].
 */
zend_function_entry tup_methods[] = {
    // 编解码接口
    PHP_ME(tup,encode,NULL,ZEND_ACC_PUBLIC| ZEND_ACC_STATIC)
    PHP_ME(tup,decode,NULL,ZEND_ACC_PUBLIC| ZEND_ACC_STATIC)
    PHP_ME(tup, putStruct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, getStruct, tup_get_struct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

    PHP_ME(tup, putBool, tup_put_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, getBool, tup_get_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, putChar, tup_put_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, getChar, tup_get_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, putUInt8, tup_put_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, getUInt8, tup_get_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, putShort, tup_put_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, getShort, tup_get_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, putUInt16, tup_put_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, getUInt16, tup_get_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, putInt32, tup_put_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, getInt32, tup_get_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, putUInt32, tup_put_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, getUInt32, tup_get_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, putInt64, tup_put_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, getInt64, tup_get_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, putDouble, tup_put_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, getDouble, tup_get_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, putFloat, tup_put_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, getFloat, tup_get_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, putString, tup_put_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, getString, tup_get_common_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, putVector, tup_put_vector_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, getVector, tup_get_vector_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, putMap, tup_put_map_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(tup, getMap, tup_get_map_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    {NULL, NULL, NULL}
};

/* }}} */

/* {{{ ttup_exception_methods[]
 *
 * Every user visible function must have an entry in ttup_functions[].
 */
zend_function_entry tup_exception_methods[] = {
    PHP_ME(tup_exception, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    {NULL, NULL, NULL}
};
/* }}} */

/* {{{ ttup_module_entry
 */
zend_module_entry phptars_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "phptars",
    tup_methods,
    PHP_MINIT(phptars),
    PHP_MSHUTDOWN(phptars),
    NULL,
    NULL,
    PHP_MINFO(phptars),
#if ZEND_MODULE_API_NO >= 20010901
    "0.2", /* Replace with version number for your extension */
#endif
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PHPTARS
ZEND_GET_MODULE(phptars)
#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(phptars)
{
    zend_class_entry ce;

    // tup
    INIT_CLASS_ENTRY(ce, "TUPAPI", tup_methods);
    tup_ce = zend_register_internal_class(&ce TSRMLS_CC);

    // tup_exception
    INIT_CLASS_ENTRY(ce, "TARS_Exception", tup_exception_methods);
    tup_exception_ce = my_zend_register_internal_class_ex(&ce, zend_exception_get_default(TSRMLS_C), NULL TSRMLS_CC);


    TUP_STARTUP(ttars);

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(phptars)
{
    /* uncomment this line if you have INI entries
    UNREGISTER_INI_ENTRIES();
    */
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(phptars)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "phptars support", "enabled");
    php_info_print_table_row(2, "phptars version", PHP_TARS_VERSION);
    php_info_print_table_end();

}
/* }}} */

/* {{{ tup_throw_exception(int err_code, char * fmt, ...)
 */
void tup_throw_exception(long err_code, char * fmt, ...) {

    va_list args;
    char *err_msg;

    va_start(args, fmt);
    vspprintf(&err_msg, 128, fmt, args);
    va_end(args);

    zend_throw_exception(tup_exception_ce, err_msg, err_code TSRMLS_CC);
    efree(err_msg);
}
/* }}} */

PHP_METHOD(tup_exception, __construct) {
    char * msg;
    int code;
    zend_size_t msg_len;
    zval * self;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &msg, &msg_len, &code) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "Wrong parameters for TARS_Exception([string $exception [, long $code]])");
        return ;
    }

    self = getThis();
    zend_update_property_string(Z_OBJCE_P(self), self, ZEND_STRL("message"), msg TSRMLS_CC);

    if (code) {
        zend_update_property_long(Z_OBJCE_P(self), self, ZEND_STRL("code"), code TSRMLS_CC);
    }
}
/* }}} */

/* {{{ __tup_PUT
 */
#define __TUP_PUT(type, packer) do { \
    char * name, * buf; \
    int ret; \
    zend_size_t name_len; \
    uint32_t len; \
    zval * value; \
    type dest; \
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &name_len, &value) == FAILURE) { \
        ZEND_WRONG_PARAM_COUNT(); \
        return ; \
    } \
    if (name_len < TUP_NAME_MIN_LEN) return NAME_EXCEPTOIN(); \
    ret = packer ## _packer(value, NULL, 0, (void *)&dest); \
    if (ret != TARS_SUCCESS) {return CONVERT_EXCEPTION(ret, #type);}\
    UniAttribute * att = UniAttribute_new(); \
    if (!att) {return MALLOC_EXCEPTION(#type);} \
    ret = TUP_put ## type(att, name, dest); \
    if (ret != TARS_SUCCESS) { \
        UniAttribute_del(&att); \
        return PACK_EXCEPTION(ret, #type); \
    } \
    len = TarsOutputStream_getLength(att->value_os); \
    buf = TarsMalloc(len); \
    if(!buf) { \
        UniAttribute_del(&att); \
        RETURN_LONG(TARS_MALLOC_ERROR); \
    } \
    memcpy(buf, TarsOutputStream_getBuffer(att->value_os), TarsOutputStream_getLength(att->value_os)); \
    UniAttribute_del(&att); \
    MY_RETVAL_STRINGL(buf, len, 1); \
    TarsFree(buf); \
} while (0)
/* }}} */

/* {{{ __TUP_GET
 */
#define __TUP_GET(type, dest) do { \
    char * name, *buf; \
    int ret; \
    zend_size_t name_len, len; \
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &name_len, &buf, &len) == FAILURE) { \
        ZEND_WRONG_PARAM_COUNT(); \
        return ; \
    } \
    UniAttribute * att = UniAttribute_new(); \
    if (!att) {MALLOC_EXCEPTION(#type); return ;}\
    /* 解码 */ \
    TarsInputStream *is_string;\
    is_string = TarsInputStream_new();\
    if (!is_string){UniAttribute_del(&att); MALLOC_EXCEPTION(#type); return ;} \
    TarsInputStream_setBuffer(is_string, buf, len); \
    ret = TarsInputStream_readMap(is_string, att->m_data, 0, true); \
    if (TARS_SUCCESS != ret) {TarsInputStream_del(&is_string); UniAttribute_del(&att); READ_MAP_EXCEPTION(); return ;} \
    /* 解包 */ \
    ret = TUP_get ## type(att, name, dest); \
    if (ret != TARS_SUCCESS) { \
        UNPACK_EXCEPTION(ret, #type); \
        UniAttribute_del(&att); \
        TarsInputStream_del(&is_string); \
        return ; \
    } \
    TarsInputStream_del(&is_string); \
    UniAttribute_del(&att); \
} while (0)
/* }}} */

/**
 * 通用编码函数
 */
PHP_METHOD(tup,encode) {
    char * servantName, *funcName;
    long iVersion, iRequestId, cPacketType, iMessageType, iTimeout;
    uint32_t outBuffLen;
    char *outBuff = NULL;
    zend_size_t servantLen, funcLen;

    zval *inbuf_arr;
    zval * contexts;
    zval * statuses;
    int ret;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llsslllaaa", &iVersion, &iRequestId,&servantName,&servantLen, &funcName,&funcLen
        ,&cPacketType,&iMessageType,&iTimeout,&contexts,&statuses,&inbuf_arr) == FAILURE) {
        ZEND_WRONG_PARAM_COUNT();
        return ;
    }

    /* 从buf数组中读出一个个的buffer，每个buffer就对应了输入的一个参数和它的值
    / 这是一个map的结构打成char之后的成果，tup_unipackert要做的事情就是把它放入正确的
    / sBuffer中 一个所谓的vector<char>
    */

    // 遍历buf数组,顺序看起来没关系，太赞了。
    JMapWrapper* map_wrapper = JMapWrapper_new("string", "list<char>");
    TarsOutputStream *os_tmp = TarsOutputStream_new();
    if (!os_tmp) {
        MALLOC_EXCEPTION("Encode");
        goto do_clean;
    }
    TarsOutputStream *os_map=NULL;

    os_map = TarsOutputStream_new();
    if (!os_map)
    {
        MALLOC_EXCEPTION("Encode");
        goto do_clean;
    }

    // 将buf装入预先设定的一个mapwrapper中
#if PHP_MAJOR_VERSION < 7
    HashTable *inbuf_ht = Z_ARRVAL_P(inbuf_arr);
    for (
            zend_hash_internal_pointer_reset(inbuf_ht);
            zend_hash_has_more_elements(inbuf_ht) == SUCCESS;
            zend_hash_move_forward(inbuf_ht)
            ) {
        char *key;
        uint key_len;
        zval ** inbuf_iter;

        if (zend_hash_get_current_key_ex(inbuf_ht, &key, &key_len, NULL, 0, NULL) == HASH_KEY_IS_STRING) {
            if (zend_hash_get_current_data(inbuf_ht, (void **)&inbuf_iter) == FAILURE) {
                continue;
            } else {
                // 针对每一个buf,现在已经获取了key->buf这样的一个键值对
                char *inbuf_val;
                uint32_t inbuf_len;
                convert_to_string(*inbuf_iter);
                inbuf_val = Z_STRVAL_PP(inbuf_iter);
                inbuf_len = Z_STRLEN_PP(inbuf_iter);

                TarsOutputStream_reset(os_tmp);
                TarsOutputStream_reset(os_map);

                ret = TarsOutputStream_writeStringBuffer(os_tmp, key, strlen(key), 0);
                if (ret) {
                    ENCODE_BUF_EXCEPTION();
                    goto do_clean;
                }

                ret = TarsOutputStream_writeVectorCharBuffer(os_map, inbuf_val, inbuf_len, 1);
                if (ret) {
                    ENCODE_BUF_EXCEPTION();
                    goto do_clean;
                }

                ret = JMapWrapper_put(map_wrapper, TarsOutputStream_getBuffer(os_tmp), TarsOutputStream_getLength(os_tmp),
                          TarsOutputStream_getBuffer(os_map), TarsOutputStream_getLength(os_map));
                if (ret) {
                    ENCODE_BUF_EXCEPTION();
                    goto do_clean;
                }
            }
        }
    }
#else
    //PHP7
    zend_string *zkey;
    zval *inbuf_iter;
    ulong num_key;

    char *key;

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(inbuf_arr), num_key, zkey, inbuf_iter){

                key = ZSTR_VAL(zkey);

                // 针对每一个buf,现在已经获取了key->buf这样的一个键值对
                char *inbuf_val;
                uint32_t inbuf_len;
                convert_to_string(inbuf_iter);
                inbuf_val = Z_STRVAL_P(inbuf_iter);
                inbuf_len = Z_STRLEN_P(inbuf_iter);

                TarsOutputStream_reset(os_tmp);
                TarsOutputStream_reset(os_map);

                ret = TarsOutputStream_writeStringBuffer(os_tmp, key, strlen(key), 0);
                if (ret) {
                    ENCODE_BUF_EXCEPTION();
                    goto do_clean;
                }

                ret = TarsOutputStream_writeVectorCharBuffer(os_map, inbuf_val, inbuf_len, 1);
                if (ret) {
                    ENCODE_BUF_EXCEPTION();
                    goto do_clean;
                }

                ret = JMapWrapper_put(map_wrapper, TarsOutputStream_getBuffer(os_tmp), TarsOutputStream_getLength(os_tmp),
                                      TarsOutputStream_getBuffer(os_map), TarsOutputStream_getLength(os_map));
                if (ret) {
                    ENCODE_BUF_EXCEPTION();
                    goto do_clean;
                }

            }ZEND_HASH_FOREACH_END();
#endif

    // 将mapwrapper进行uniattribute的encode使之成为符合需求的字符串
    // 设置tup包初始化参数
    UniPacket* pack = UniPacket_new();
    pack->iVersion = iVersion;
    pack->cPacketType = (char) cPacketType;
    pack->iMessageType  = iMessageType;
    pack->iRequestId = iRequestId;
    pack->iTimeout = iTimeout;

    TarsOutputStream *context_key_tmp = TarsOutputStream_new();
    if (!context_key_tmp) {
        MALLOC_EXCEPTION("Encode");
        goto do_clean;
    }
    TarsOutputStream *context_value_tmp = TarsOutputStream_new();
    if (!context_value_tmp) {
        MALLOC_EXCEPTION("Encode");
        goto do_clean;
    }
    // 如果设置了context
    if(NULL != contexts) {
        HashTable *contextsHt= Z_ARRVAL_P(contexts);
#if PHP_MAJOR_VERSION < 7
        for (
            zend_hash_internal_pointer_reset(contextsHt);
            zend_hash_has_more_elements(contextsHt) == SUCCESS;
            zend_hash_move_forward(contextsHt)
            ) {
            char *key;
            uint keyLen;
            zval ** contextsIter;


            if (zend_hash_get_current_key_ex(contextsHt, &key, &keyLen, NULL, 0, NULL) == HASH_KEY_IS_STRING) {
                if (zend_hash_get_current_data(contextsHt, (void **)&contextsIter) == FAILURE) {
                    continue;
                } else {
                    char *contextVal;
                    uint32_t contextLen;
                    convert_to_string(*contextsIter);
                    contextVal = Z_STRVAL_PP(contextsIter);
                    contextLen = Z_STRLEN_PP(contextsIter);

                    ret = TarsOutputStream_writeStringBuffer(context_key_tmp, key, strlen(key), 0);
                    if (ret) {
                        ENCODE_BUF_EXCEPTION();
                        goto do_clean;
                    }

                    ret = TarsOutputStream_writeStringBuffer(context_value_tmp, contextVal, contextLen, 1);
                    if (ret) {
                        ENCODE_BUF_EXCEPTION();
                        goto do_clean;
                    }

                    ret = JMapWrapper_put(pack->context, TarsOutputStream_getBuffer(context_key_tmp), TarsOutputStream_getLength(context_key_tmp),
                              TarsOutputStream_getBuffer(context_value_tmp), TarsOutputStream_getLength(context_value_tmp));
                    if(ret) {
                        TUP_SET_CONTEXT_EXCEPTION();
                        goto do_clean;
                    }
                }
            }
        }
#else
//PHP7
        zend_string *zkey;
        zval *contextsIter;
        ulong num_key;

        char *key;

        ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(contexts), num_key, zkey, contextsIter) {

            key = ZSTR_VAL(zkey);

            char *contextVal;
            uint32_t contextLen;
            convert_to_string(contextsIter);
            contextVal = Z_STRVAL_P(contextsIter);
            contextLen = Z_STRLEN_P(contextsIter);

            ret = TarsOutputStream_writeStringBuffer(context_key_tmp, key, strlen(key), 0);
            if (ret) {
                ENCODE_BUF_EXCEPTION();
                goto do_clean;
            }

            ret = TarsOutputStream_writeStringBuffer(context_value_tmp, contextVal, contextLen, 1);
            if (ret) {
                ENCODE_BUF_EXCEPTION();
                goto do_clean;
            }

            ret = JMapWrapper_put(pack->context, TarsOutputStream_getBuffer(context_key_tmp), TarsOutputStream_getLength(context_key_tmp),
                                  TarsOutputStream_getBuffer(context_value_tmp), TarsOutputStream_getLength(context_value_tmp));
            if(ret) {
                TUP_SET_CONTEXT_EXCEPTION();
                goto do_clean;
            }

        } ZEND_HASH_FOREACH_END();
#endif
    }

    // 如果设置了status
    if(NULL != statuses) {
        TarsOutputStream_reset(context_key_tmp);
        TarsOutputStream_reset(context_value_tmp);

#if PHP_MAJOR_VERSION < 7
        HashTable *statusesHt= Z_ARRVAL_P(statuses);
        for (
            zend_hash_internal_pointer_reset(statusesHt);
            zend_hash_has_more_elements(statusesHt) == SUCCESS;
            zend_hash_move_forward(statusesHt)
            ) {
            char *key;
            uint keyLen;
            zval ** statusesIter;

            if (zend_hash_get_current_key_ex(statusesHt, &key, &keyLen, NULL, 0, NULL) == HASH_KEY_IS_STRING) {
                if (zend_hash_get_current_data(statusesHt, (void **)&statusesIter) == FAILURE) {
                    continue;
                } else {
                    char *statusVal;
                    uint32_t statusLen;
                    convert_to_string(*statusesIter);
                    statusVal = Z_STRVAL_PP(statusesIter);
                    statusLen = Z_STRLEN_PP(statusesIter);


                    ret = TarsOutputStream_writeStringBuffer(context_key_tmp, key, strlen(key), 0);
                    if (ret) {
                        ENCODE_BUF_EXCEPTION();
                        goto do_clean;
                    }

                    ret = TarsOutputStream_writeStringBuffer(context_value_tmp, statusVal, statusLen, 1);
                    if (ret) {
                        ENCODE_BUF_EXCEPTION();
                        goto do_clean;
                    }

                    ret = JMapWrapper_put(pack->status, TarsOutputStream_getBuffer(context_key_tmp), TarsOutputStream_getLength(context_key_tmp),
                              TarsOutputStream_getBuffer(context_value_tmp), TarsOutputStream_getLength(context_value_tmp));

                    if(ret) {
                        TUP_SET_STATUS_EXCEPTION();
                        goto do_clean;
                    }
                }
            }
        }
#else
        //PHP7
        zend_string *zkey;
        zval *statusesIter;
        ulong num_key;

        char *key;

        ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(statuses), num_key, zkey, statusesIter) {

            key = ZSTR_VAL(zkey);

            char *statusVal;
            uint32_t statusLen;
            convert_to_string(statusesIter);
            statusVal = Z_STRVAL_P(statusesIter);
            statusLen = Z_STRLEN_P(statusesIter);

            ret = TarsOutputStream_writeStringBuffer(context_key_tmp, key, strlen(key), 0);
            if (ret) {
                ENCODE_BUF_EXCEPTION();
                goto do_clean;
            }

            ret = TarsOutputStream_writeStringBuffer(context_value_tmp, statusVal, statusLen, 1);
            if (ret) {
                ENCODE_BUF_EXCEPTION();
                goto do_clean;
            }

            ret = JMapWrapper_put(pack->status, TarsOutputStream_getBuffer(context_key_tmp), TarsOutputStream_getLength(context_key_tmp),
                                  TarsOutputStream_getBuffer(context_value_tmp), TarsOutputStream_getLength(context_value_tmp));

            if(ret) {
                TUP_SET_STATUS_EXCEPTION();
                goto do_clean;
            }

        } ZEND_HASH_FOREACH_END();
#endif
    }
    if (context_key_tmp)      TarsOutputStream_del(&context_key_tmp);
    if (context_value_tmp)      TarsOutputStream_del(&context_value_tmp);


    JString_assign(pack->sServantName, servantName, servantLen);
    if(funcLen == 0) funcLen = strlen(funcName);
    JString_assign(pack->sFuncName, funcName, funcLen);

    Int32 iHeaderLen = 0;

    TarsOutputStream_reset(os_tmp);

    TarsOutputStream_writeMap(os_tmp,map_wrapper,0);

    // 拷贝进入sBuffer
    ret = JString_assign(pack->sBuffer, TarsOutputStream_getBuffer(os_tmp), TarsOutputStream_getLength(os_tmp));
    if (TARS_SUCCESS != ret) {
        MALLOC_EXCEPTION("Encode");
        goto do_clean;
    }

    TarsOutputStream_reset(os_tmp);

    // 设置输出buffer的其他信息
    ret = TarsOutputStream_writeShort(os_tmp, pack->iVersion, 1);
    if (TARS_SUCCESS != ret) {
        ENCODE_WRITE_IVERSION_EXCEPTION();
        goto do_clean;
    }
    ret = TarsOutputStream_writeChar(os_tmp, pack->cPacketType, 2);
    if (TARS_SUCCESS != ret) {
        ENCODE_WRITE_CPACKETTYPE_EXCEPTION();
        goto do_clean;
    }
    ret = TarsOutputStream_writeInt32(os_tmp, pack->iMessageType, 3);
    if (TARS_SUCCESS != ret) {
        ENCODE_WRITE_IMESSAGETYPE_EXCEPTION();
        goto do_clean;
    }
    ret = TarsOutputStream_writeInt32(os_tmp, pack->iRequestId, 4);
    if (TARS_SUCCESS != ret) {
        ENCODE_WRITE_IREQUESTID_EXCEPTION();
        goto do_clean;
    }
    ret = TarsOutputStream_writeString(os_tmp, pack->sServantName, 5);
    if (TARS_SUCCESS != ret) {
        ENCODE_WRITE_SSERVANTNAME_EXCEPTION();
        goto do_clean;
    }
    ret = TarsOutputStream_writeString(os_tmp, pack->sFuncName, 6);
    if (TARS_SUCCESS != ret)
    {
        ENCODE_WRITE_SFUNCNAME_EXCEPTION();
        goto do_clean;
    }
    ret = TarsOutputStream_writeVectorChar(os_tmp, pack->sBuffer, 7);
    if (TARS_SUCCESS != ret) {
        ENCODE_WRITE_SBUFFER_EXCEPTION();
        goto do_clean;
    }
    ret = TarsOutputStream_writeInt32(os_tmp, pack->iTimeout, 8);
    if (TARS_SUCCESS != ret) {
        ENCODE_WRITE_ITIMEOUT_EXCEPTION();
        goto do_clean;
    }
    ret = TarsOutputStream_writeMap(os_tmp, pack->context, 9);
    if (TARS_SUCCESS != ret) {
        ENCODE_WRITE_CONTEXT_EXCEPTION();
        goto do_clean;
    }
    ret = TarsOutputStream_writeMap(os_tmp, pack->status, 10);
    if (TARS_SUCCESS != ret) {
        ENCODE_WRITE_STATUS_EXCEPTION();
        goto do_clean;
    }
    // 以网络序的方式处理包头
    iHeaderLen = tars_htonl(sizeof(Int32) + TarsOutputStream_getLength(os_tmp));

    uint32_t len;
    len = sizeof(Int32) + TarsOutputStream_getLength(os_tmp);

    outBuff = TarsMalloc(len);
    if (!outBuff)
    {
        MALLOC_EXCEPTION("Encode");
        goto do_clean;
    }

    memcpy(outBuff, &iHeaderLen, sizeof(Int32));
    memcpy(outBuff + sizeof(Int32), TarsOutputStream_getBuffer(os_tmp), TarsOutputStream_getLength(os_tmp));

    MY_RETVAL_STRINGL(outBuff, len, 1);
    free(outBuff);

do_clean:
    if(pack) UniPacket_del(&pack);
    if(os_tmp) TarsOutputStream_del(&os_tmp);
    //if(outBuff) TarsFree(outBuff);
    if (map_wrapper) JMapWrapper_del(&map_wrapper);
    if (os_map)      TarsOutputStream_del(&os_map);
    if (context_key_tmp)      TarsOutputStream_del(&context_key_tmp);
    if (context_value_tmp)      TarsOutputStream_del(&context_value_tmp);
}


PHP_METHOD(tup,decode) {
    char *outBuff = NULL;

    char * respBuffer;
    zend_size_t respBufferLen;
    int ret;
    zval *ret_val;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&respBuffer,
        &respBufferLen) == FAILURE) {
        ZEND_WRONG_PARAM_COUNT();
        return ;
    }

    UniPacket *unpack = UniPacket_new();

    UniPacket_decode(unpack, respBuffer, respBufferLen);



    array_init(return_value);
    // 获取返回码
    JString * tmp;
    tmp = JString_new();

    int code;
    ret = Unipacket_getCode(unpack,tmp);

    code = atoi(JString_data(tmp));

    // 获取msg
    char * msg;
    ret = Unipacket_getMsg(unpack,&tmp);

    msg = JS_STRVAL(tmp);
    uint32_t msg_len = JS_STRLEN(tmp);

    if(code != TARS_SUCCESS) {
        add_assoc_long(return_value,"code",code);
        my_add_assoc_stringl(return_value,"msg",msg, msg_len, 1);

        if(tmp) JString_del(&tmp);
        if(unpack) UniPacket_del(&unpack);
        return;
    }

    TarsOutputStream *os = TarsOutputStream_new();
    if (!os) {
        MALLOC_EXCEPTION("Decode");
        goto do_clean;
    }

    TarsOutputStream_writeMap(os,((UniAttribute *)unpack)->m_data,0);


    uint32_t len;
    len = TarsOutputStream_getLength(os);

    outBuff = TarsMalloc(len);
    memcpy(outBuff, TarsOutputStream_getBuffer(os), TarsOutputStream_getLength(os));

    my_add_assoc_stringl(return_value,"sBuffer",outBuff, len, 1);
    add_assoc_long(return_value,"code",code);
    my_add_assoc_stringl(return_value,"msg",msg, msg_len, 1);

do_clean:
    if(os) TarsOutputStream_del(&os);
    if(unpack) UniPacket_del(&unpack);
    if(outBuff) TarsFree(outBuff);
    if(tmp) JString_del(&tmp);
}

/* {{{ Ttup::putBool(string $name, boolean $bool)
 */
PHP_METHOD(tup, putBool) {
    __TUP_PUT(Bool, bool);
}
/* }}} */

/* {{{ TTUP::getBool(string $name, string $buf)
 */
PHP_METHOD(tup, getBool) {
    Bool b;
    __TUP_GET(Bool, &b);
    RETURN_BOOL(b);
}
/* }}} */

/* {{{ TTUP::putChar(string $name, mixed $value)
 */
PHP_METHOD(tup, putChar) {
    __TUP_PUT(Char, char);
}
/* }}} */

/* {{{ TTUP::getChar(string $name, string $buf)
 */
PHP_METHOD(tup, getChar) {

    Char b;
    __TUP_GET(Char, &b);
    MY_RETURN_STRINGL(&b, 1, 1);
}
/* }}} */

/* {{{ TTUP::putChar(string $name, mixed $value)
 */
PHP_METHOD(tup, putUInt8) {
    __TUP_PUT(UInt8, uint8);
}
/* }}} */

/* {{{ TTUP::getUInt8(string $name, string $buf)
 */
PHP_METHOD(tup, getUInt8) {

    UInt8 i;
    __TUP_GET(UInt8, &i);
    RETURN_LONG(i);
}
/* }}} */

/* {{{ TTUP::putShort(string $name, mixed $value)
 */
PHP_METHOD(tup, putShort) {
    __TUP_PUT(Short, short);
}
/* }}} */

/* {{{ TTUP::getShort(string $name, string $buf)
 */
PHP_METHOD(tup, getShort) {

    Short i;
    __TUP_GET(Short, &i);
    RETURN_LONG(i);
}
/* }}} */

/* {{{ TTUP::putUInt16(string $name, mixed $value)
 */
PHP_METHOD(tup, putUInt16) {
    __TUP_PUT(UInt16, uint16);
}
/* }}} */

/* {{{ TTUP::getUInt16(string $name, string $buf)
 */
PHP_METHOD(tup, getUInt16) {

    UInt16 i;
    __TUP_GET(UInt16, &i);
    RETURN_LONG(i);
}
/* }}} */

/* {{{ TTUP::putInt32(string $name, mixed $value)
 */
PHP_METHOD(tup, putInt32) {
    __TUP_PUT(Int32, int32);
}
/* }}} */

/* {{{ TTUP::getInt32(string $name, string $buf)
 */
PHP_METHOD(tup, getInt32) {

    Int32 i;
    __TUP_GET(Int32, &i);
    RETURN_LONG(i);
}
/* }}} */


/* {{{ TTUP::putUInt32(string $name, mixed $value)
 */
PHP_METHOD(tup, putUInt32) {
    __TUP_PUT(UInt32, uint32);
}
/* }}} */

/* {{{ __TUP_GET_BIG_INT
 */
#if __WORDSIZE == 64 || defined(__x86_64) || defined(__x86_64__)
#define __TUP_GET_BIG_INT(type) \
    type i; \
    __TUP_GET(type, &i); \
    RETURN_LONG(i);
#else
#define __TUP_GET_BIG_INT(type) \
    type i; \
    __TUP_GET(type, &i); \
    if (i >= LONG_MIN && i <= LONG_MAX) { \
        RETURN_LONG(i);
    } else { \
        char  ll[32]; \
        int len; \
        len = slprintf(ll, 32, "%lld", i); \
        RETURN_STRINGL(ll, len, 1); \
    }
#endif
/* }}} */

/* {{{ TTUP::getInt32(string $name, string $buf)
 */
PHP_METHOD(tup, getUInt32) {

    __TUP_GET_BIG_INT(UInt32)
}
/* }}} */

/* {{{ TTUP::putInt64(string $name, mixed $value)
 */
PHP_METHOD(tup, putInt64) {
    __TUP_PUT(Int64, int64);
}
/* }}} */

/* {{{ TTUP::getInt32(string $name, string $buf)
 */
PHP_METHOD(tup, getInt64) {

    __TUP_GET_BIG_INT(Int64)
}
/* }}} */

/* {{{ TTUP::putDouble(string $name, mixed $value)
 */
PHP_METHOD(tup, putDouble) {
    __TUP_PUT(Double, double);
}
/* }}} */

/* {{{ TTUP::getDouble(string $name, string $buf)
 */
PHP_METHOD(tup, getDouble) {

    Double i;
    __TUP_GET(Double, &i);
    RETURN_DOUBLE(i);
}
/* }}} */

/* {{{ TTUP::putFloat(string $name, mixed $value)
 */
PHP_METHOD(tup, putFloat) {
    __TUP_PUT(Float, float);
}
/* }}} */

/* {{{ TTUP::getFloat(string $name, string $buf)
 */
PHP_METHOD(tup, getFloat) {

    Float i;
    __TUP_GET(Float, &i);
    RETURN_DOUBLE(i);
}
/* }}} */

/* {{{ TTUP::putString(string $name, mixed $value)
 */
PHP_METHOD(tup, putString) {

    char * name, * buf;
    int ret;
    zend_size_t name_len;
    uint32_t len;
    zval * value;
    JString * js = NULL;
    UniAttribute * att = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &name_len, &value) == FAILURE) {
        ZEND_WRONG_PARAM_COUNT();
        return ;
    }

    js = JString_new();
    if (!js) {MALLOC_EXCEPTION("String"); return;}

    ret = string_packer(value, NULL, 0, NULL);
    if (ret != TARS_SUCCESS) {
        CONVERT_EXCEPTION(ret, "String");
        goto do_clean;
    }

    ret = JString_assign(js, Z_STRVAL_P(value), Z_STRLEN_P(value));
    if (ret != TARS_SUCCESS) {
        MALLOC_EXCEPTION("String");
        goto do_clean;
    }

    att = UniAttribute_new();
    if (!att) {
        MALLOC_EXCEPTION("String");
        goto do_clean;
    }

    ret = TUP_putString(att, name, js);
    if (ret != TARS_SUCCESS) {
        PACK_EXCEPTION(ret, "String");
        goto do_clean;
    }


    // 这次不需要再encode了
    len = TarsOutputStream_getLength(att->value_os);

    buf = TarsMalloc(len);

    if(NULL == buf) {
        MALLOC_EXCEPTION("String");
        goto do_clean;
    }

    memcpy(buf, TarsOutputStream_getBuffer(att->value_os), TarsOutputStream_getLength(att->value_os));

    MY_RETVAL_STRINGL(buf, len, 1);

do_clean :
    if (js) JString_del(&js);
    if (att) UniAttribute_del(&att);
    if (buf) TarsFree(buf);
}
/* }}} */

/* {{{ TTUP::getString(string $name, string $buf)
 */
PHP_METHOD(tup, getString) {

    char * name, *buf;
    zend_size_t name_len, len;
    int ret;
    JString * js = NULL;
    UniAttribute * att = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &name_len, &buf, &len) == FAILURE) {
        ZEND_WRONG_PARAM_COUNT();
        return ;
    }

    att = UniAttribute_new();
    if (!att) {
        MALLOC_EXCEPTION("String");
        return ;
    }

    TarsInputStream *is_string;
    is_string = TarsInputStream_new();
    if (!is_string)
    {
        MALLOC_EXCEPTION("String");
        goto do_clean;
    }

    TarsInputStream_setBuffer(is_string, buf, len);


    ret = TarsInputStream_readMap(is_string, att->m_data, 0, true);
    if (TARS_SUCCESS != ret) {
        UNPACK_EXCEPTION(ret, "String");
        goto do_clean;
    }

    js = JString_new();

    ret = TUP_getString(att, name, js);
    if (TARS_SUCCESS != ret) {
        UNPACK_EXCEPTION(ret, "String");
        goto do_clean;
    }

    MY_RETVAL_STRINGL(JS_STRVAL(js), JS_STRLEN(js), 1);


do_clean :
    if (js) JString_del(&js);
    if (att) UniAttribute_del(&att);
    if(is_string) TarsInputStream_del(&is_string);
}
/* }}} */

/* {{{ TUP::putVector
 */
PHP_METHOD(tup, putVector) {

    zval * clazz;
    char * name, *buf = NULL;
    zend_size_t name_len;
    int ret;
    uint32_t len;
    JArray * vct = NULL;
    JString * js = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sO", &name, &name_len, &clazz, tars_vector_ce) == FAILURE) {
        ZEND_WRONG_PARAM_COUNT();
        return ;
    }

    if (name_len < TUP_NAME_MIN_LEN) return NAME_EXCEPTOIN();

    vector_wrapper * obj = Z_VECTOR_WRAPPER_P(clazz);
    if (!IS_VALID_TYPE(obj->t)) return TYPE_EXCEPTOIN();

    if (IS_JSTRING(obj->t)) {
        js = obj->ctx->str;
    } else {
        vct = obj->ctx->vct;
    }

    UniAttribute * att = UniAttribute_new();
    if (!att) {
        MALLOC_EXCEPTION("Vector");
        return;
    }

    if (IS_JSTRING(obj->t)) {
        ret = TUP_putVectorChar(att, name, js);
    } else {
        ret = TUP_putVector(att, name, vct);
    }

    if (ret != TARS_SUCCESS) {
        PACK_EXCEPTION(ret, "Vector");
        goto do_clean;
    }

    // 这次不需要再encode了
    len = TarsOutputStream_getLength(att->value_os);

    buf = TarsMalloc(len);

    if(NULL == buf) {
        MALLOC_EXCEPTION("Vector");
        goto do_clean;
    }

    memcpy(buf, TarsOutputStream_getBuffer(att->value_os), TarsOutputStream_getLength(att->value_os));

    MY_RETVAL_STRINGL(buf, len, 1);

do_clean :
    if (att) UniAttribute_del(&att);
    if (buf) TarsFree(buf);
    if (js) JString_del(&js);
}
/* }}} */

/* {{{ TUP::getVector
 */
PHP_METHOD(tup, getVector) {

    char * buf, * name;
    zend_size_t len, name_len;
    int ret;
    zval * clazz, * ret_val = NULL;
    JArray * vct;
    JString * js;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sOs", &name, &name_len, &clazz, tars_vector_ce, &buf, &len) == FAILURE) {
        ZEND_WRONG_PARAM_COUNT();
        return ;
    }

    vector_wrapper * obj = Z_VECTOR_WRAPPER_P(clazz TSRMLS_CC);
    if (!IS_VALID_TYPE(obj->t)) return TYPE_EXCEPTOIN();

    UniAttribute * att = UniAttribute_new();
    if (!att) {
        MALLOC_EXCEPTION("Vector");
        goto do_clean;
    }

    TarsInputStream *is_string;
    is_string = TarsInputStream_new();

    if (!is_string)
    {
        MALLOC_EXCEPTION("Vector");
        goto do_clean;
    }

    TarsInputStream_setBuffer(is_string, buf, len);


    ret = TarsInputStream_readMap(is_string, att->m_data, 0, true);
    if (TARS_SUCCESS != ret) {
        UNPACK_EXCEPTION(ret, "Vector");
        goto do_clean;
    }

    // 区分vector的类型
    if (IS_JSTRING(obj->t)) {
        js = obj->ctx->str;
        JString_clear(js);
        ret = TUP_getVectorChar(att, name, js);
        if (ret != TARS_SUCCESS) {
            UNPACK_EXCEPTION(ret, "Vector");
            goto do_clean;
        }

        MY_RETVAL_STRINGL(JS_STRVAL(js), JS_STRLEN(js), 1);
    } else {

        vct = obj->ctx->vct;
        JArray_clear(vct);
        ret = TUP_getVector(att, name, vct);

        if (ret != TARS_SUCCESS) {
            UNPACK_EXCEPTION(ret, "Vector");
            goto do_clean;
        }

        // 解包成PHP数据结构
        ret = vector_unpacker(NULL, 0, true, clazz, (void **)&ret_val);

        if (ret != TARS_SUCCESS) {
            UNPACK_EXCEPTION(ret, "Vector");
            goto do_clean;
        }

        RETVAL_ZVAL(ret_val, 1, 1);
    }

do_clean :
    if (att) UniAttribute_del(&att);
    if(is_string) TarsInputStream_del(&is_string);
}
/* }}} */

/* {{{ TTUP::putMap
 */
PHP_METHOD(tup, putMap) {
    zval * clazz;
    char * name, *buf = NULL;
    int ret;
    zend_size_t name_len;
    uint32_t len;
    JMapWrapper * container;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sO", &name, &name_len, &clazz, tars_map_ce) == FAILURE) {
        ZEND_WRONG_PARAM_COUNT();
        return ;
    }

    if (name_len < TUP_NAME_MIN_LEN) return NAME_EXCEPTOIN();

    map_wrapper * obj = Z_MAP_WRAPPER_P(clazz TSRMLS_CC);
    container = obj->ctx;

    UniAttribute * att = UniAttribute_new();
    if (!att) return MALLOC_EXCEPTION("Map");

    ret = TUP_putMap(att, name, container);
    if (ret != TARS_SUCCESS) {
        PACK_EXCEPTION(ret, "Map");
        goto do_clean;
    }

    len = TarsOutputStream_getLength(att->value_os);
    buf = TarsMalloc(len);

    if(NULL == buf) {
        MALLOC_EXCEPTION("Map");
        goto do_clean;
    }

    memcpy(buf, TarsOutputStream_getBuffer(att->value_os), TarsOutputStream_getLength(att->value_os));

    MY_RETVAL_STRINGL(buf, len, 1);
do_clean :
    UniAttribute_del(&att);
    if (buf) TarsFree(buf);
}
/* }}} */

/* {{{ TTUP::getMap(stirng $name, TTARS_Map $map)
 */
PHP_METHOD(tup, getMap) {

    zval * clazz, *ret_val = NULL;
    char * buf, * name;
    zend_size_t len, name_len;
    int ret = 0;
    JMapWrapper * container;
    UniAttribute * att = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sOs", &name, &name_len, &clazz, tars_map_ce, &buf, &len) == FAILURE) {
        ZEND_WRONG_PARAM_COUNT();
        return ;
    }

    att = UniAttribute_new();
    if (!att) {ret = TARS_MALLOC_ERROR; goto do_clean; }

    TarsInputStream *is_string;
    is_string = TarsInputStream_new();
    if (!is_string)
    {
        MALLOC_EXCEPTION("Map");
        goto do_clean;
    }

    TarsInputStream_setBuffer(is_string, buf, len);


    ret = TarsInputStream_readMap(is_string, att->m_data, 0, true);
    if (TARS_SUCCESS != ret) {
        UNPACK_EXCEPTION(ret, "Map");
        goto do_clean;
    }


    map_wrapper * obj = Z_MAP_WRAPPER_P(clazz TSRMLS_CC);
    container = obj->ctx;
    JMapWrapper_clear(container);

    ret = TUP_getMap(att, name, container);
    if (ret != TARS_SUCCESS) {
        UNPACK_EXCEPTION(ret, "Map");
        goto do_clean;
    }

    ret = _map_to_array(clazz, container, (void **)&ret_val);
    if (ret != TARS_SUCCESS) {
        UNPACK_EXCEPTION(ret, "Map");
        goto do_clean;
    }

    RETVAL_ZVAL(ret_val, 1, 1);

do_clean :
    UniAttribute_del(&att);
    if(NULL != is_string) TarsInputStream_del(&is_string);

}
/* }}} */

PHP_METHOD(tup, putStruct) {
    zval * clazz;
    char * name, *buf = NULL;
    int ret;
    zend_size_t name_len;
    uint32_t len;
    UniAttribute * att = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sO", &name, &name_len, &clazz, tars_struct_ce) == FAILURE) {
        ZEND_WRONG_PARAM_COUNT();
        return ;
    }

    if (name_len < TUP_NAME_MIN_LEN)  return NAME_EXCEPTOIN();

    att = UniAttribute_new();
    if (!att)  return MALLOC_EXCEPTION("Struct");

    ret = php_TUP_putStruct(att, name, clazz);

    if (ret != TARS_SUCCESS) {
        PACK_EXCEPTION(ret, name);
        goto do_clean;
    }

    len = TarsOutputStream_getLength(att->value_os);

    buf = TarsMalloc(len);

    if(NULL == buf) {
        MALLOC_EXCEPTION("Struct");
        goto do_clean;
    }

    memcpy(buf, TarsOutputStream_getBuffer(att->value_os), TarsOutputStream_getLength(att->value_os));

    MY_RETVAL_STRINGL(buf, len, 1);

do_clean :
    UniAttribute_del(&att);
    if (buf) TarsFree(buf);
}
/* }}} */


/* {{{ TTUP::getStruct(string $name, string $buf, TTARS_Struct $struct)
+ */
PHP_METHOD(tup, getStruct) {

    char * buf, * name;
    zend_size_t len, name_len;
    int ret;
    zval * clazz, *ret_val;
    UniAttribute * attr = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sOs", &name, &name_len, &clazz, tars_struct_ce, &buf, &len) == FAILURE) {
        ZEND_WRONG_PARAM_COUNT();
        return ;
    }

    // 解码
    attr = UniAttribute_new();
    TarsInputStream *is_string;
    is_string = TarsInputStream_new();

    if (!is_string)
    {
        MALLOC_EXCEPTION("Struct");
        goto do_clean;
    }

    TarsInputStream_setBuffer(is_string, buf, len);

    ret = TarsInputStream_readMap(is_string, attr->m_data, 0, true);
    if (TARS_SUCCESS != ret) {
        UNPACK_EXCEPTION(ret, name);
        goto do_clean;
    }

    ret = php_TUP_getStruct(attr, name, clazz,(void **)&ret_val);
    if (TARS_SUCCESS != ret) {
        UNPACK_EXCEPTION(ret, name);
        goto do_clean;
    }

    RETVAL_ZVAL(ret_val, 1, 1);

do_clean :
    if(NULL != is_string) TarsInputStream_del(&is_string);
    if(NULL != attr) UniAttribute_del(&attr);
}



