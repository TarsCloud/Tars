<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/4/29
 * Time: 下午2:54.
 */

namespace Tars\monitor\client;

class TUPAPIWrapper
{
    public static function putBool($paramName, $tag, $bool, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $boolBuffer = \TUPAPI::putBool($tag, $bool, $iVersion);
            } else {
                $boolBuffer = \TUPAPI::putBool($paramName, $bool, $iVersion);
            }

            if (!is_string($boolBuffer)) {
                throw new \Exception(Code::getErrMsg(Code::TARS_PUT_BOOL_FAILED), Code::TARS_PUT_BOOL_FAILED);
            }

            return  $boolBuffer;
        } catch (\Exception $e) {
            $code = Code::TARS_PUT_BOOL_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }
    public static function getBool($name, $tag, $sBuffer, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $result = \TUPAPI::getBool($tag, $sBuffer, false, $iVersion);
            } else {
                $result = \TUPAPI::getBool($name, $sBuffer, false, $iVersion);
            }

            return $result;
        } catch (\Exception $e) {
            $code = Code::TARS_GET_BOOL_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function putChar($paramName, $tag, $char, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $charBuffer = \TUPAPI::putChar($tag, $char, $iVersion);
            } else {
                $charBuffer = \TUPAPI::putChar($paramName, $char, $iVersion);
            }
            if (!is_string($charBuffer)) {
                throw new \Exception(Code::getErrMsg(Code::TARS_PUT_CHAR_FAILED), Code::TARS_PUT_CHAR_FAILED);
            }

            return  $charBuffer;
        } catch (\Exception $e) {
            $code = Code::TARS_PUT_CHAR_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function getChar($name, $tag, $sBuffer, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $result = \TUPAPI::getChar($tag, $sBuffer, false, $iVersion);
            } else {
                $result = \TUPAPI::getChar($name, $sBuffer, false, $iVersion);
            }

            return  $result;
        } catch (\Exception $e) {
            $code = Code::TARS_GET_CHAR_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function putUInt8($paramName, $tag, $uint8, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $uint8Buffer = \TUPAPI::putUint8($tag, $uint8, $iVersion);
            } else {
                $uint8Buffer = \TUPAPI::putUint8($paramName, $uint8, $iVersion);
            }
            if (!is_string($uint8Buffer)) {
                throw new \Exception(Code::getErrMsg(Code::TARS_PUT_UINT8_FAILED), Code::TARS_PUT_UINT8_FAILED);
            }

            return $uint8Buffer;
        } catch (\Exception $e) {
            $code = Code::TARS_PUT_UINT8_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function getUint8($name, $tag, $sBuffer, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $result = \TUPAPI::getUint8($tag, $sBuffer, false, $iVersion);
            } else {
                $result = \TUPAPI::getUint8($name, $sBuffer, false, $iVersion);
            }

            return  $result;
        } catch (\Exception $e) {
            $code = Code::TARS_GET_UINT8_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function putShort($paramName, $tag, $short, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $shortBuffer = \TUPAPI::putShort($tag, $short, $iVersion);
            } else {
                $shortBuffer = \TUPAPI::putShort($paramName, $short, $iVersion);
            }
            if (!is_string($shortBuffer)) {
                throw new \Exception(Code::getErrMsg(Code::TARS_PUT_SHORT_FAILED), Code::TARS_PUT_SHORT_FAILED);
            }

            return  $shortBuffer;
        } catch (\Exception $e) {
            $code = Code::TARS_PUT_SHORT_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function getShort($name, $tag, $sBuffer, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $result = \TUPAPI::getShort($tag, $sBuffer, false, $iVersion);
            } else {
                $result = \TUPAPI::getShort($name, $sBuffer, false, $iVersion);
            }

            return $result;
        } catch (\Exception $e) {
            $code = Code::TARS_GET_SHORT_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function putUInt16($paramName, $tag, $uint16, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $buffer = \TUPAPI::putUint16($tag, $uint16, $iVersion);
            } else {
                $buffer = \TUPAPI::putUint16($paramName, $uint16, $iVersion);
            }
            if (!is_string($buffer)) {
                throw new \Exception(Code::getErrMsg(Code::TARS_PUT_UINT16_FAILED), Code::TARS_PUT_UINT16_FAILED);
            }

            return $buffer;
        } catch (\Exception $e) {
            $code = Code::TARS_PUT_UINT16_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function getUint16($name, $tag, $sBuffer, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $result = \TUPAPI::getUint16($tag, $sBuffer, false, $iVersion);
            } else {
                $result = \TUPAPI::getUint16($name, $sBuffer, false, $iVersion);
            }

            return  $result;
        } catch (\Exception $e) {
            $code = Code::TARS_GET_UINT16_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function putInt32($paramName, $tag, $int, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $buffer = \TUPAPI::putInt32($tag, $int, $iVersion);
            } else {
                $buffer = \TUPAPI::putInt32($paramName, $int, $iVersion);
            }
            if (!is_string($buffer)) {
                throw new \Exception(Code::getErrMsg(Code::TARS_PUT_INT32_FAILED), Code::TARS_PUT_INT32_FAILED);
            }

            return  $buffer;
        } catch (\Exception $e) {
            $code = Code::TARS_PUT_INT32_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function getInt32($name, $tag, $sBuffer, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $result = \TUPAPI::getInt32($tag, $sBuffer, false, $iVersion);
            } else {
                $result = \TUPAPI::getInt32($name, $sBuffer, false, $iVersion);
            }

            return $result;
        } catch (\Exception $e) {
            $code = Code::TARS_GET_INT32_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function putUint32($paramName, $tag, $uint, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $buffer = \TUPAPI::putInt32($tag, $uint, $iVersion);
            } else {
                $buffer = \TUPAPI::putInt32($paramName, $uint, $iVersion);
            }
            if (!is_string($buffer)) {
                throw new \Exception(Code::getErrMsg(Code::TARS_PUT_UINT32_FAILED), Code::TARS_PUT_UINT32_FAILED);
            }

            return $buffer;
        } catch (\Exception $e) {
            $code = Code::TARS_PUT_UINT32_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function getUint32($name, $tag, $sBuffer, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $result = \TUPAPI::getUint32($tag, $sBuffer, false, $iVersion);
            } else {
                $result = \TUPAPI::getUint32($name, $sBuffer, false, $iVersion);
            }

            return  $result;
        } catch (\Exception $e) {
            $code = Code::TARS_GET_UINT32_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function putInt64($paramName, $tag, $bigint, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $buffer = \TUPAPI::putInt64($tag, $bigint, $iVersion);
            } else {
                $buffer = \TUPAPI::putInt64($paramName, $bigint, $iVersion);
            }
            if (!is_string($buffer)) {
                throw new \Exception(Code::getErrMsg(Code::TARS_PUT_INT64_FAILED), Code::TARS_PUT_INT64_FAILED);
            }

            return $buffer;
        } catch (\Exception $e) {
            $code = Code::TARS_PUT_INT64_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function getInt64($name, $tag, $sBuffer, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $result = \TUPAPI::getInt64($tag, $sBuffer, false, $iVersion);
            } else {
                $result = \TUPAPI::getInt64($name, $sBuffer, false, $iVersion);
            }

            return  $result;
        } catch (\Exception $e) {
            $code = Code::TARS_GET_INT64_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function putDouble($paramName, $tag, $double, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $buffer = \TUPAPI::putDouble($tag, $double, $iVersion);
            } else {
                $buffer = \TUPAPI::putDouble($paramName, $double, $iVersion);
            }
            if (!is_string($buffer)) {
                throw new \Exception(Code::getErrMsg(Code::TARS_PUT_DOUBLE_FAILED), Code::TARS_PUT_DOUBLE_FAILED);
            }

            return  $buffer;
        } catch (\Exception $e) {
            $code = Code::TARS_PUT_DOUBLE_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function getDouble($name, $tag, $sBuffer, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $result = \TUPAPI::getDouble($tag, $sBuffer, false, $iVersion);
            } else {
                $result = \TUPAPI::getDouble($name, $sBuffer, false, $iVersion);
            }

            return  $result;
        } catch (\Exception $e) {
            $code = Code::TARS_GET_DOUBLE_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function putFloat($paramName, $tag, $float, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $buffer = \TUPAPI::putFloat($tag, $float, $iVersion);
            } else {
                $buffer = \TUPAPI::putFloat($paramName, $float, $iVersion);
            }
            if (!is_string($buffer)) {
                throw new \Exception(Code::getErrMsg(Code::TARS_PUT_FLOAT_FAILED), Code::TARS_PUT_FLOAT_FAILED);
            }

            return  $buffer;
        } catch (\Exception $e) {
            $code = Code::TARS_PUT_FLOAT_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function getFloat($name, $tag, $sBuffer, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $result = \TUPAPI::getFloat($tag, $sBuffer, false, $iVersion);
            } else {
                $result = \TUPAPI::getFloat($name, $sBuffer, false, $iVersion);
            }

            return $result;
        } catch (\Exception $e) {
            $code = Code::TARS_GET_FLOAT_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function putString($paramName, $tag, $string, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $buffer = \TUPAPI::putString($tag, $string, $iVersion);
            } else {
                $buffer = \TUPAPI::putString($paramName, $string, $iVersion);
            }
            if (!is_string($buffer)) {
                throw new \Exception(Code::getErrMsg(Code::TARS_PUT_STRING_FAILED), Code::TARS_PUT_STRING_FAILED);
            }

            return $buffer;
        } catch (\Exception $e) {
            $code = Code::TARS_PUT_STRING_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function getString($name, $tag, $sBuffer, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $result = \TUPAPI::getString($tag, $sBuffer, false, $iVersion);
            } else {
                $result = \TUPAPI::getString($name, $sBuffer, false, $iVersion);
            }
            if ($result < 0) {
                throw new \Exception(Code::getErrMsg(Code::TARS_GET_STRING_FAILED), Code::TARS_GET_STRING_FAILED);
            }

            return  $result;
        } catch (\Exception $e) {
            $code = Code::TARS_GET_STRING_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function putVector($paramName, $tag, $vec, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $buffer = \TUPAPI::putVector($tag, $vec, $iVersion);
            } else {
                $buffer = \TUPAPI::putVector($paramName, $vec, $iVersion);
            }
            if (!is_string($buffer)) {
                throw new \Exception(Code::getErrMsg(Code::TARS_PUT_VECTOR_FAILED), Code::TARS_PUT_VECTOR_FAILED);
            }

            return $buffer;
        } catch (\Exception $e) {
            $code = Code::TARS_PUT_VECTOR_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function getVector($name, $tag, $vec, $sBuffer, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $result = \TUPAPI::getVector($tag, $vec, $sBuffer, false, $iVersion);
            } else {
                $result = \TUPAPI::getVector($name, $vec, $sBuffer, false, $iVersion);
            }

            return  $result;
        } catch (\Exception $e) {
            $code = Code::TARS_GET_VECTOR_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function putMap($paramName, $tag, $map, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $buffer = \TUPAPI::putMap($tag, $map, $iVersion);
            } else {
                $buffer = \TUPAPI::putMap($paramName, $map, $iVersion);
            }
            if (!is_string($buffer)) {
                throw new \Exception(Code::getErrMsg(Code::TARS_PUT_MAP_FAILED), Code::TARS_PUT_MAP_FAILED);
            }

            return $buffer;
        } catch (\Exception $e) {
            $code = Code::TARS_PUT_MAP_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public function getMap($name, $tag, $obj, $sBuffer, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $result = \TUPAPI::getMap($tag, $obj, $sBuffer, false, $iVersion);
            } else {
                $result = \TUPAPI::getMap($name, $obj, $sBuffer, false, $iVersion);
            }
            if (!is_array($result)) {
                throw new \Exception(Code::getErrMsg(Code::TARS_GET_MAP_FAILED), Code::TARS_GET_MAP_FAILED);
            }

            return  $result;
        } catch (\Exception $e) {
            $code = Code::TARS_GET_MAP_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function putStruct($paramName, $tag, $obj, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $buffer = \TUPAPI::putStruct($tag, $obj, $iVersion);
            } else {
                $buffer = \TUPAPI::putStruct($paramName, $obj, $iVersion);
            }
            if (!is_string($buffer)) {
                throw new \Exception(Code::getErrMsg(Code::TARS_PUT_STRUCT_FAILED), Code::TARS_PUT_STRUCT_FAILED);
            }

            return $buffer;
        } catch (\Exception $e) {
            $code = Code::TARS_PUT_STRUCT_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    public static function getStruct($name, $tag, &$obj, $sBuffer, $iVersion)
    {
        try {
            if ($iVersion === Consts::TARSVERSION) {
                $result = \TUPAPI::getStruct($tag, $obj, $sBuffer, false, $iVersion);
            } else {
                $result = \TUPAPI::getStruct($name, $obj, $sBuffer, false, $iVersion);
            }

            if (!is_array($result)) {
                throw new \Exception(Code::getErrMsg(Code::TARS_GET_STRUCT_FAILED), Code::TARS_GET_STRUCT_FAILED);
            }
            self::fromArray($result, $obj);

            return $result;
        } catch (\Exception $e) {
            $code = Code::TARS_GET_STRUCT_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
    }

    // 将数组转换成对象
    public static function fromArray($data, &$structObj)
    {
        if (!empty($data)) {
            foreach ($data as $key => $value) {
                if ($structObj->$key instanceof \TARS_Struct) {
                    self::fromArray($value, $structObj->$key);
                } else {
                    $structObj->$key = $value;
                }
            }
        }
    }
}
