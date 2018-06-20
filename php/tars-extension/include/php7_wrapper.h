/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:yong@yuewen.com                                               |
  +----------------------------------------------------------------------+
*/

#ifndef EXT_PHP7_WRAPPER_H_
#define EXT_PHP7_WRAPPER_H_


#if PHP_MAJOR_VERSION < 7
typedef int zend_size_t;
#define IS_TRUE                               11
#define IS_FALSE                              12
#define MY_MAKE_STD_ZVAL(p)                   MAKE_STD_ZVAL(p)
#define MY_RETURN_STRINGL                     RETURN_STRINGL
#define MY_ZEND_FETCH_RESOURCE                ZEND_FETCH_RESOURCE
#define MY_ZEND_REGISTER_RESOURCE             ZEND_REGISTER_RESOURCE
#define my_add_assoc_string                   add_assoc_string
#define my_zend_read_property                 zend_read_property
#define my_zval_ptr_dtor                      zval_ptr_dtor
#define MY_RETVAL_STRINGL                     RETVAL_STRINGL
#define my_add_assoc_stringl                  add_assoc_stringl
#define my_zend_register_internal_class_ex    zend_register_internal_class_ex
#define my_zend_hash_get_current_key_ex     zend_hash_get_current_key_ex
#define my_smart_str                          smart_str
#define MY_Z_ARRVAL_P                         Z_ARRVAL_P
#define MY_ZVAL_STRINGL                       ZVAL_STRINGL
#define my_zend_hash_index_update             zend_hash_index_update
#define my_zend_hash_add                      zend_hash_add
#define my_zend_hash_next_index_insert  zend_hash_next_index_insert

static inline int MY_Z_TYPE_P(zval *z)
{
    if (Z_TYPE_P(z) == IS_BOOL)
    {
        if ((uint8_t) Z_BVAL_P(z) == 1)
        {
            return IS_TRUE;
        }
        else
        {
            return IS_FALSE;
        }
    }
    else
    {
        return Z_TYPE_P(z);
    }
}

inline int MY_Z_TYPE_P(zval *z);
#define MY_Z_TYPE_PP(z)        MY_Z_TYPE_P(*z)

#else

typedef size_t zend_size_t;
#define MY_MAKE_STD_ZVAL(p)             zval _stack_zval_##p; p = &(_stack_zval_##p)
#define MY_RETURN_STRINGL(s, l, dup)    RETURN_STRINGL(s, l)
#define MY_ZEND_FETCH_RESOURCE(rsrc, rsrc_type, passed_id, default_id, resource_type_name, resource_type)    \
    rsrc = (rsrc_type) zend_fetch_resource(Z_RES_P(*passed_id), resource_type_name, resource_type);
#define MY_ZEND_REGISTER_RESOURCE(return_value, result, le_result)  ZVAL_RES(return_value,zend_register_resource(result, le_result))
#define my_add_assoc_string(array, key, value, duplicate)   add_assoc_string(array, key, value)
static zval* my_zend_read_property(zend_class_entry *class_ptr, zval *obj, char *s, int len, int silent)
{
    zval rv;
    return zend_read_property(class_ptr, obj, s, len, silent, &rv);
}
#define my_zval_ptr_dtor(p)                 zval_ptr_dtor(*p)
#define MY_RETVAL_STRINGL(s, l, dup)    RETVAL_STRINGL(s, l); if (dup == 0) efree(s)
#define my_add_assoc_stringl(__arg, __key, __str, __length, __duplicate)   add_assoc_stringl_ex(__arg, __key, strlen(__key), __str, __length)
#define Z_STRVAL_PP(s)                             Z_STRVAL_P(*s)
#define my_zend_register_internal_class_ex(entry,parent_ptr,str)    zend_register_internal_class_ex(entry,parent_ptr)
#define my_smart_str                          smart_string
#define MY_Z_ARRVAL_P(z)                          Z_ARRVAL_P(z)->ht
#define Z_ARRVAL_PP(s)                             Z_ARRVAL_P(*s)
#define Z_STRLEN_PP(s)                             Z_STRLEN_P(*s)
#define Z_LVAL_PP(v)                               Z_LVAL_P(*v)
#define MY_ZVAL_STRINGL(z, s, l, dup)         ZVAL_STRINGL(z, s, l)
#define MY_Z_TYPE_P                                Z_TYPE_P
#define MY_Z_TYPE_PP(s)                            MY_Z_TYPE_P(*s)
static inline int my_zend_hash_add(HashTable *ht, char *k, long len, void *pData, int datasize, void **pDest)
{
    zval **real_p = pData;
    return zend_hash_str_add(ht, k, len - 1, *real_p) ? SUCCESS : FAILURE;
}
static inline int my_zend_hash_index_update(HashTable *ht, long key, void *pData, int datasize, void **pDest)
{
    zval **real_p = pData;
    return zend_hash_index_update(ht, key, *real_p) ? SUCCESS : FAILURE;
}
#define ALLOC_INIT_ZVAL(z)                          zval z##Tmp; z = &(z##Tmp);
#define my_zend_hash_next_index_insert(ht, pData, a, b)  zend_hash_next_index_insert(ht, *pData)






#endif

#endif /* EXT_PHP7_WRAPPER_H_ */
