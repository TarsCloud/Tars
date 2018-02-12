/*
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
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header 297205 2010-03-30 21:09:07Z johannes $ */

#ifndef PHP_TUPAPI_H
#define PHP_TUPAPI_H

extern zend_module_entry phptars_module_entry;
#define phpext_phptars_ptr &phptars_module_entry

#ifdef PHP_WIN32
#define PHP_PHPTARS_API __declspec(dllexport)
#else
#define PHP_PHPTARS_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#include "php7_wrapper.h"

#define PHP_TARS_VERSION "2.1.3"

zend_class_entry * tup_ce;
zend_class_entry * tup_exception_ce;
zend_class_entry * tars_ce;
zend_class_entry * tars_struct_ce;
zend_class_entry * tars_vector_ce;
zend_class_entry * tars_map_ce;

#define JS_STRVAL(t) ((t)->_data)
#define JS_STRLEN(t) ((t)->_len)

#define TUP_STARTUP_FUNC(module)   ZEND_MINIT_FUNCTION(module)
#define TUP_STARTUP(module) ZEND_MODULE_STARTUP_N(module)(INIT_FUNC_ARGS_PASSTHRU)

PHP_METHOD(tup, __construct);
PHP_METHOD(tup, encode);
PHP_METHOD(tup, decode);
PHP_METHOD(tup, putStruct);
PHP_METHOD(tup, getStruct);

PHP_METHOD(tup, putBool);
PHP_METHOD(tup, getBool);

PHP_METHOD(tup, putChar);
PHP_METHOD(tup, getChar);

PHP_METHOD(tup, putUInt8);
PHP_METHOD(tup, getUInt8);

PHP_METHOD(tup, putShort);
PHP_METHOD(tup, getShort);

PHP_METHOD(tup, putUInt16);
PHP_METHOD(tup, getUInt16);

PHP_METHOD(tup, putInt32);
PHP_METHOD(tup, getInt32);

PHP_METHOD(tup, putUInt32);
PHP_METHOD(tup, getUInt32);

PHP_METHOD(tup, putInt64);
PHP_METHOD(tup, getInt64);

PHP_METHOD(tup, putDouble);
PHP_METHOD(tup, getDouble);

PHP_METHOD(tup, putFloat);
PHP_METHOD(tup, getFloat);

PHP_METHOD(tup, putString);
PHP_METHOD(tup, getString);

PHP_METHOD(tup, putVector);
PHP_METHOD(tup, getVector);

PHP_METHOD(tup, putMap);
PHP_METHOD(tup, getMap);


PHP_METHOD(tup_exception, __construct);

#define OUTOF_EXCEPTION()           tup_throw_exception(ERR_OUTOF_RANGE, "Outof the specified type range.")
#define MALLOC_EXCEPTION(t)          tup_throw_exception(ERR_MALLOC_FAILED, "Unable to allocate enough memory type %s.", t)
#define CONVERT_EXCEPTION(errno, t) tup_throw_exception(errno, "Parameter can not be converted to the type %s.", t)
#define PACK_EXCEPTION(errno, t)       tup_throw_exception(errno, "An error occurred when packing type %s.", t)
#define UNPACK_EXCEPTION(errno, t)     tup_throw_exception(errno, "An error occurred when unpacking type %s, make sure you get type right", t)
#define UNINIT_EXCEPTION(clazz)     tup_throw_exception(ERR_CLASS_UNINIT, "%s instance is not initialized.", clazz)
#define ENCODE_EXCEPTION(errno)     tup_throw_exception(errno, "An error occurred when encoding.")
#define DECODE_EXCEPTION(errno)     tup_throw_exception(errno, "An error occurred when decoding.")
#define NAME_EXCEPTOIN()            tup_throw_exception(ERR_WRONG_PARAMS, "The length of the package name too short.")
#define TYPE_EXCEPTOIN()            tup_throw_exception(ERR_TYPE_INVALID, "Data type is invalid.")
#define INTERNET_EXCEPTION(errno)   tup_throw_exception(errno, "An internet error occurred.")
#define PARAM_FMT_EXCEPTION()       tup_throw_exception(ERR_DATA_FORMAT_ERROR, "Parameter format is incorrect.")
#define READ_MAP_EXCEPTION()       tup_throw_exception(ERR_READ_MAP_ERROR, "Read map failed, please check binary buffer.")
#define TUP_SET_CONTEXT_EXCEPTION() tup_throw_exception(ERR_SET_CONTEXT_ERROR, "Set context while encoding failed, please check contexts parameter while encoding.")
#define TUP_SET_STATUS_EXCEPTION() tup_throw_exception(ERR_SET_STATUS_ERROR, "Set status while encoding failed, please check statuses parameter while encoding.")
#define ENCODE_BUF_EXCEPTION() tup_throw_exception(ERR_ENCODE_BUF_ERROR, "Encode buf while encoding failed, please check in buf array parameter while encoding.")
#define ENCODE_WRITE_IVERSION_EXCEPTION() tup_throw_exception(ERR_WRITE_IVERSION_ERROR, "Encode failed while writing iversion.")
#define ENCODE_WRITE_CPACKETTYPE_EXCEPTION() tup_throw_exception(ERR_WRITE_CPACKETTYPE_ERROR, "Encode failed while writing cpackettype .")
#define ENCODE_WRITE_IMESSAGETYPE_EXCEPTION() tup_throw_exception(ERR_WRITE_IMESSAGETYPE_ERROR, "Encode failed while writing imessagetype .")
#define ENCODE_WRITE_IREQUESTID_EXCEPTION() tup_throw_exception(ERR_WRITE_IREQUESTID_ERROR, "Encode failed while writing irequestid .")
#define ENCODE_WRITE_SSERVANTNAME_EXCEPTION() tup_throw_exception(ERR_WRITE_SSERVANTNAME_ERROR, "Encode failed while writing SSERVANTNAME .")
#define ENCODE_WRITE_SFUNCNAME_EXCEPTION() tup_throw_exception(ERR_WRITE_SFUNCNAME_ERROR, "Encode failed while writing SFUNCNAME .")
#define ENCODE_WRITE_SBUFFER_EXCEPTION() tup_throw_exception(ERR_WRITE_SBUFFER_ERROR, "Encode failed while writing SBUFFER .")
#define ENCODE_WRITE_ITIMEOUT_EXCEPTION() tup_throw_exception(ERR_WRITE_ITIMEOUT_ERROR, "Encode failed while writing ITIMEOUT .")
#define ENCODE_WRITE_CONTEXT_EXCEPTION() tup_throw_exception(ERR_WRITE_CONTEXT_ERROR, "Encode failed while writing CONTEXT .")
#define ENCODE_WRITE_STATUS_EXCEPTION() tup_throw_exception(ERR_WRITE_STATUS_ERROR, "Encode failed while writing STATUS .")


PHP_MINIT_FUNCTION(phptars);
PHP_MSHUTDOWN_FUNCTION(phptars);
PHP_RINIT_FUNCTION(phptars);
PHP_RSHUTDOWN_FUNCTION(phptars);
PHP_MINFO_FUNCTION(phptars);

void tup_throw_exception(long err_code, char * fmt, ...);

#define  TUP_NAME_MIN_LEN 1

#ifdef ZTS
#define PHPTARS_G(v) TSRMG(phptars_globals_id, zend_phptars_globals *, v)
#else
#define PHPTARS_G(v) (phptars_globals.v)
#endif

#endif	/* PHP_TTUP_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
