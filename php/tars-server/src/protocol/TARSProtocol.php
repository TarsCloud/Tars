<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/3/4
 * Time: 下午4:53.
 */

namespace Tars\protocol;

use Tars\core\Request;
use Tars\Code;
use Tars\core\Response;

class TARSProtocol implements Protocol
{
    // 可以支持自定义的
    // 决定传什么参数进来、
    // 路由的方式
    public function route(Request $request, Response $response, $tarsConfig = [])
    {
        $unpackResult = $this->unpackReq($request->reqBuf);
        $sServantName = $unpackResult['sServantName'];
        $sFuncName = $unpackResult['sFuncName'];

        $paramInfos = $request->paramInfos;
        // 发生了注释异常,说明生成的注释有问题
        if (!isset($paramInfos[$sFuncName])) {
            throw new \Exception(Code::TARSSERVERUNKNOWNERR);
        }
        $paramInfo = $paramInfos[$sFuncName];

        // 需要一个函数,专门把参数,转换为args

        $args = $this->convertToArgs($paramInfo, $unpackResult);

        return [
            'args' => $args,
            'unpackResult' => $unpackResult,
            'sFuncName' => $sFuncName,
        ];
    }

    // 另外就是发包的时候怎么组包
    public function unpackReq($data)
    {
        $decodeRet = \TUPAPI::decodeReqPacket($data);

        return $decodeRet;

//        $respBuf = $decodeRet['sBuffer'];

//        $iVersion = $decodeRet['iVersion'];
//        $sFuncName = $decodeRet['sFuncName'];
//        $sServantName = $decodeRet['sServantName'];
    }

    public function packBuffer($type, $argv, $tag, $name, $iVersion = 3)
    {
        $packMethods = [
            'bool' => '\TUPAPI::putBool',
            'byte' => '\TUPAPI::putChar',
            'char' => '\TUPAPI::putChar',
            'unsigned byte' => '\TUPAPI::putUInt8',
            'unsigned char' => '\TUPAPI::putUInt8',
            'short' => '\TUPAPI::putShort',
            'unsigned short' => '\TUPAPI::putUInt16',
            'int' => '\TUPAPI::putInt32',
            'unsigned int' => '\TUPAPI::putUInt32',
            'long' => '\TUPAPI::putInt64',
            'float' => '\TUPAPI::putFloat',
            'double' => '\TUPAPI::putDouble',
            'string' => '\TUPAPI::putString',
            'enum' => '\TUPAPI::putShort',
            'map' => '\TUPAPI::putMap',
            'vector' => '\TUPAPI::putVector',
            'struct' => '\TUPAPI::putStruct',
        ];

        $packMethod = $packMethods[$type];
        if ($iVersion === 3) {
            $buf = $packMethod($name, $argv, $iVersion);
        }
        // jce类型是用tag进行区分的
        else {
            $buf = $packMethod($tag, $argv, $iVersion);
        }

        return $buf;
    }

    /**
     * @param $unpackResult
     * @param $code
     * @param $msg
     *
     * @return mixed
     */
    public function packErrRsp($unpackResult, $code, $msg)
    {
        $iVersion = $unpackResult['iVersion'];
        $cPacketType = 0;
        $iMessageType = 0;
        $iRequestId = $unpackResult['iRequestId']; // 使用协程的id
        $statuses = [];
        $encodeBufs = [];

        if ($iVersion === 1) {
            $rspBuf = \TUPAPI::encodeRspPacket($iVersion, $cPacketType,
                $iMessageType, $iRequestId, $code, empty($msg) ? Code::getMsg($code) : $msg, $encodeBufs, $statuses);
        } else {
            $servantName = $unpackResult['sServantName'];
            $funcName = $unpackResult['sFuncName'];
            $context = [];
            $iTimeout = 0;
            $statuses['STATUS_RESULT_CODE'] = $code;
            $rspBuf = \TUPAPI::encode($iVersion, $iRequestId, $servantName, $funcName, $cPacketType,
                $iMessageType, $iTimeout, $context, $statuses, $encodeBufs);
        }

        return $rspBuf;
    }

    public function packRsp($paramInfo, $unpackResult, $args, $returnVal)
    {
        try {
            $iVersion = $unpackResult['iVersion'];
            // 获取返回值之后,需要按照正确的格式进行打包,这里应该进行抽象(之后再说)
            $encodeBufs = [];
            $iRequestId = $unpackResult['iRequestId']; // 使用协程的id
            if ($iVersion === 1) {
                $return = $paramInfo['return'];
                if ($return['type'] !== 'void') {
                    $returnBuf = $this->packBuffer($return['type'], $returnVal, $return['tag'], '', $iVersion);
                    $encodeBufs[] = $returnBuf;
                }

                // 输出参数开始打包
                $outStartIndex = count($paramInfo['inParams']);

                $outParams = $paramInfo['outParams'];
                // 开始遍历输出参数,每一个都应该是完成赋值的内容
                for ($i = $outStartIndex; $i < count($args); ++$i) {
                    $j = $i - $outStartIndex;
                    $argv = $args[$i];
                    $buf = $this->packBuffer($outParams[$j]['type'], $argv, $outParams[$j]['tag'],
                        $outParams[$j]['name'], $iVersion);
                    $encodeBufs[] = $buf;
                }

                // 完成所有的打包之后,开始编码
                $cPacketType = 0;
                $iMessageType = 0;

                $statuses = [];

                $rspBuf = \TUPAPI::encodeRspPacket($iVersion, $cPacketType,
                    $iMessageType, $iRequestId, Code::TARSSERVERSUCCESS, 'success', $encodeBufs, $statuses);
            } else {
                $return = $paramInfo['return'];
                if ($return['type'] !== 'void') {
                    $returnBuf = $this->packBuffer($return['type'], $returnVal, $return['tag'], '', $iVersion);

                    $encodeBufs[''] = $returnBuf;
                }
                // 输出参数开始打包
                $outStartIndex = count($paramInfo['inParams']);

                $outParams = $paramInfo['outParams'];
                // 开始遍历输出参数,每一个都应该是完成赋值的内容
                for ($i = $outStartIndex; $i < count($args); ++$i) {
                    $j = $i - $outStartIndex;
                    $argv = $args[$i];
                    $buf = $this->packBuffer($outParams[$j]['type'], $argv, $outParams[$j]['tag'],
                        $outParams[$j]['name'], $iVersion);
                    $encodeBufs[$outParams[$j]['name']] = $buf;
                }

                // 完成所有的打包之后,开始编码
                $cPacketType = 0;
                $iMessageType = 0;
                $iRequestId = 1; // 使用协程的id
                $statuses = [];

                $servantName = $unpackResult['sServantName'];
                $funcName = $unpackResult['sFuncName'];
                $context = [];
                $iTimeout = 0;
                $statuses['STATUS_RESULT_CODE'] = Code::TARSSERVERSUCCESS;

                $rspBuf = \TUPAPI::encode($iVersion, $iRequestId, $servantName, $funcName, $cPacketType,
                    $iMessageType, $iTimeout, $context, $statuses, $encodeBufs);
            }

            return $rspBuf;
        } catch (\Exception $e) {
            throw new \Exception(Code::TARSSERVERSUCCESS);
        }
    }

    public function parseAnnotation($docblock)
    {
        $docblock = trim($docblock, '/** ');
        $lines = explode('*', $docblock);
        $validLines = [];
        foreach ($lines as $line) {
            $line = trim($line);
            if (!empty($line)) {
                $validLines[] = $line;
            }
        }
        // 对解析出来的annotation进行处理
        $index = 0;
        $inParams = [];
        $outParams = [];
        $returnParam = [];

        foreach ($validLines as $validLine) {
            // 说明是参数类型
            if (strstr($validLine, '@param')) {
                ++$index;
                // 说明是输出参数
                if (strstr($validLine, '=out=')) {
                    $parts = explode(' ', $validLine);

                    $outParams[] = [
                        'type' => $parts[1],
                        'proto' => $parts[3],
                        'name' => trim($parts[2], '$'),
                        'tag' => $index,
                    ];
                }
                // 输入参数
                else {
                    $parts = explode(' ', $validLine);

                    $inParams[] = [
                        'type' => $parts[1],
                        'proto' => isset($parts[3])?$parts[3]:'',
                        'name' => trim($parts[2], '$'),
                        'tag' => $index,
                    ];
                }
            }
            // 说明是返回类型
            else {
                $parts = explode(' ', $validLine);

                if (count($parts) > 2) {
                    $returnParam = [
                        'type' => $parts[1],
                        'proto' => $parts[2],
                        'tag' => 0,
                    ];
                } else {
                    $returnParam = [
                        'type' => $parts[1],
                        'tag' => 0,
                    ];
                }
            }
        }

        return [
            'inParams' => $inParams,
            'outParams' => $outParams,
            'return' => $returnParam,
        ];
    }

    // 完成了对入包的decode之后,获取到了sBuffer
    public function convertToArgs($paramInfo, $unpackResult)
    {
        try {
            $sBuffer = $unpackResult['sBuffer'];
            $iVersion = $unpackResult['iVersion'];

            $unpackMethods = [
                'bool' => '\TUPAPI::getBool',
                'byte' => '\TUPAPI::getChar',
                'char' => '\TUPAPI::getChar',
                'unsigned byte' => '\TUPAPI::getUInt8',
                'unsigned char' => '\TUPAPI::getUInt8',
                'short' => '\TUPAPI::getShort',
                'unsigned short' => '\TUPAPI::getUInt16',
                'int' => '\TUPAPI::getInt32',
                'unsigned int' => '\TUPAPI::getUInt32',
                'long' => '\TUPAPI::getInt64',
                'float' => '\TUPAPI::getFloat',
                'double' => '\TUPAPI::getDouble',
                'string' => '\TUPAPI::getString',
                'enum' => '\TUPAPI::getShort',
                'map' => '\TUPAPI::getMap',
                'vector' => '\TUPAPI::getVector',
                'struct' => '\TUPAPI::getStruct',
            ];

            $inParams = $paramInfo['inParams'];
            $args = [];

            foreach ($inParams as $inParam) {
                $type = $inParam['type'];
                $unpackMethod = $unpackMethods[$type];
                if ($iVersion === 3) {
                    // 需要判断是否是简单类型,还是vector或map或struct
                    if ($type === 'map' || $type === 'vector') {
                        // 对于复杂的类型,需要进行实例化
                        $proto = $this->createInstance($inParam['proto']);
                        $value = $unpackMethod($inParam['name'], $proto, $sBuffer, false, $iVersion);
                    } elseif ($type === 'struct') {
                        // 对于复杂的类型,需要进行实例化
                        $proto = new $inParam['proto']();
                        $value = $unpackMethod($inParam['name'], $proto, $sBuffer, false, $iVersion);
                        $this->fromArray($value, $proto);
                        $value = $proto;
                    }
                    // 基本类型
                    else {
                        $value = $unpackMethod($inParam['name'], $sBuffer, false, $iVersion);
                    }
                }
                // jce类型是用tag进行区分的
                else {
                    // 需要判断是否是简单类型,还是vector或map或struct
                    if ($type === 'map' || $type === 'vector') {
                        // 对于复杂的类型,需要进行实例化
                        $proto = $this->createInstance($inParam['proto']);
                        $value = $unpackMethod($inParam['tag'], $proto, $sBuffer, false, $iVersion);
                    } elseif ($type === 'struct') {
                        // 对于复杂的类型,需要进行实例化
                        // 结构体还需要再转换回对象
                        $proto = new $inParam['proto']();
                        $value = $unpackMethod($inParam['tag'], $proto, $sBuffer, false, $iVersion);
                        $this->fromArray($value, $proto);
                        $value = $proto;
                    }
                    // 基本类型
                    else {
                        $value = $unpackMethod($inParam['tag'], $sBuffer, false, $iVersion);
                    }
                }

                $args[] = $value;
            }

            $outParams = $paramInfo['outParams'];

            // 对于输出参数而言,所需要的仅仅是对应的实例化而已
            $index = 0;
            foreach ($outParams as $outParam) {
                ++$index;
                $type = $outParam['type'];

                $protoName = 'proto'.$index;

                // 如果是结构体
                if ($type === 'map' || $type === 'vector') {
                    $$protoName = $this->createInstance($outParam['proto']);
                    $args[] = &$$protoName;
                } elseif ($type === 'struct') {
                    $$protoName = new $outParam['proto']();
                    $args[] = &$$protoName;
                } else {
                    $protoName = null;
                    $args[] = &$protoName;
                }
            }

            return $args;
        } catch (\Exception $e) {
            throw new \Exception(Code::TARSSERVERSUCCESS);
        }
    }

    private function createInstance($proto)
    {
        if ($this->isBasicType($proto)) {
            return $this->convertBasicType($proto);
        } elseif (!strpos($proto, '(')) {
            $structInst = new $proto();

            return $structInst;
        } else {
            $pos = strpos($proto, '(');
            $className = substr($proto, 0, $pos);
            if ($className == '\TARS_Vector') {
                $next = trim(substr($proto, $pos, strlen($proto) - $pos), '()');
                $args[] = $this->createInstance($next);
            } elseif ($className == '\TARS_Map') {
                $next = trim(substr($proto, $pos, strlen($proto) - $pos), '()');
                $pos = strpos($next, ',');
                $left = substr($next, 0, $pos);
                $right = trim(substr($next, $pos, strlen($next) - $pos), ',');

                $args[] = $this->createInstance($left);
                $args[] = $this->createInstance($right);
            } elseif ($this->isBasicType($className)) {
                $next = trim(substr($proto, $pos, strlen($proto) - $pos), '()');
                $basicInst = $this->createInstance($next);
                $args[] = $basicInst;
            } else {
                $structInst = new $className();
                $args[] = $structInst;
            }
            $ins = new $className(...$args);
        }

        return $ins;
    }

    private function isBasicType($type)
    {
        $basicTypes = [
            '\TARS::BOOL',
            '\TARS::CHAR',
            '\TARS::CHAR',
            '\TARS::UINT8',
            '\TARS::UINT8',
            '\TARS::SHORT',
            '\TARS::UINT16',
            '\TARS::INT32',
            '\TARS::UINT32',
            '\TARS::INT64',
            '\TARS::FLOAT',
            '\TARS::DOUBLE',
            '\TARS::STRING',
            '\TARS::INT32',
        ];

        return in_array($type, $basicTypes);
    }

    private function convertBasicType($type)
    {
        $basicTypes = [
            '\TARS::BOOL' => 1,
            '\TARS::CHAR' => 2,
            '\TARS::UINT8' => 3,
            '\TARS::SHORT' => 4,
            '\TARS::UINT16' => 5,
            '\TARS::FLOAT' => 6,
            '\TARS::DOUBLE' => 7,
            '\TARS::INT32' => 8,
            '\TARS::UINT32' => 9,
            '\TARS::INT64' => 10,
            '\TARS::STRING' => 11,
        ];

        return $basicTypes[$type];
    }
    // 将数组转换成对象
    private function fromArray($data, &$structObj)
    {
        if (!empty($data)) {
            foreach ($data as $key => $value) {
                if (method_exists($structObj, 'set'.ucfirst($key))) {
                    call_user_func_array([$this, 'set'.ucfirst($key)], [$value]);
                } elseif ($structObj->$key instanceof \TARS_Struct) {
                    $this->fromArray($value, $structObj->$key);
                } else {
                    $structObj->$key = $value;
                }
            }
        }
    }
}
