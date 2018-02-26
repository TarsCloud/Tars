<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2017/6/16
 * Time: 下午6:15
 */

require_once "SimpleStruct.php";
require_once "NestedStruct.php";

class TarsTest extends PHPUnit_Framework_TestCase
{
    public $iVersion = 3;
    public $iRequestId = 1;
    public $servantName = "test.test.test";
    public $funcName = "example";
    public $cPacketType=0;
    public $iMessageType=0;
    public $iTimeout=2;
    public $contexts=array();
    public $statuses=array();

    public function testBool()
    {
        $bool = true;

        $buf = \TUPAPI::putBool("bool",$bool);

        $encodeBufs['bool'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);
        if($decodeRet['code'] !== 0) {

        }
        $respBuf = $decodeRet['sBuffer'];

        $out = \TUPAPI::getBool("bool",$respBuf);

        $this->assertEquals($bool,$out);
    }

    public function testChar()
    {
        $char = 1;

        $buf = \TUPAPI::putChar("char",$char);

        $encodeBufs['char'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);
        if($decodeRet['code'] !== 0) {

        }
        $respBuf = $decodeRet['sBuffer'];


        $out = \TUPAPI::getChar("char",$respBuf);

        $this->assertEquals(pack("h",$char),$out);
    }

    public function testUInt8()
    {
        $uint8 = 255;

        $buf = \TUPAPI::putUInt8("uint8",$uint8);

        $encodeBufs['uint8'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);
        if($decodeRet['code'] !== 0) {

        }
        $respBuf = $decodeRet['sBuffer'];

        $out = \TUPAPI::getUInt8("uint8",$respBuf);

        $this->assertEquals($uint8,$out);
    }

    public function testShort()
    {
        $short = 255;

        $buf = \TUPAPI::putShort("short",$short);

        $encodeBufs['short'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);
        if($decodeRet['code'] !== 0) {

        }
        $respBuf = $decodeRet['sBuffer'];

        $out = \TUPAPI::getShort("short",$respBuf);

        $this->assertEquals($short,$out);
    }

    public function testUInt16()
    {
        $uint16 = 255;

        $buf = \TUPAPI::putUInt16("uint16",$uint16);

        $encodeBufs['uint16'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);
        if($decodeRet['code'] !== 0) {

        }
        $respBuf = $decodeRet['sBuffer'];

        $out = \TUPAPI::getUInt16("uint16",$respBuf);

        $this->assertEquals($uint16,$out);
    }

    public function testInt32()
    {
        $int32 = 255;

        $buf = \TUPAPI::putInt32("int32",$int32);

        $encodeBufs['int32'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);
        if($decodeRet['code'] !== 0) {

        }
        $respBuf = $decodeRet['sBuffer'];

        $out = \TUPAPI::getInt32("int32",$respBuf);

        $this->assertEquals($int32,$out);
    }

    public function testUInt32()
    {
        $uint32 = 255;

        $buf = \TUPAPI::putUInt32("uint32",$uint32);

        $encodeBufs['uint32'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);
        if($decodeRet['code'] !== 0) {

        }
        $respBuf = $decodeRet['sBuffer'];

        $out = \TUPAPI::getUInt32("uint32",$respBuf);

        $this->assertEquals($uint32,$out);
    }

    public function testInt64()
    {
        $int64 = 1025458;

        $buf = \TUPAPI::putInt64("int64",$int64);

        $encodeBufs['int64'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);
        if($decodeRet['code'] !== 0) {

        }
        $respBuf = $decodeRet['sBuffer'];

        $out = \TUPAPI::getInt64("int64",$respBuf);

        $this->assertEquals($int64,$out);
    }
    public function testDouble()
    {
        $double = 1.225845125412365;

        $buf = \TUPAPI::putDouble("double",$double);

        $encodeBufs['double'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);
        if($decodeRet['code'] !== 0) {

        }
        $respBuf = $decodeRet['sBuffer'];

        $out = \TUPAPI::getDouble("double",$respBuf);

        $this->assertEquals($double,$out);
    }

    public function testFloat()
    {
        $float = 3.141592;

        $buf = \TUPAPI::putFloat("float",$float);

        $encodeBufs['float'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);
        if($decodeRet['code'] !== 0) {

        }
        $respBuf = $decodeRet['sBuffer'];

        $out = \TUPAPI::getFloat("float",$respBuf);

        $this->assertEquals((float)$float,(float)$out,"not equal",0.0000001);
    }

    public function testString()
    {
        $string  = "test";

        $buf = \TUPAPI::putString("string",$string);

        $encodeBufs['string'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);
        if($decodeRet['code'] !== 0) {

        }
        $respBuf = $decodeRet['sBuffer'];

        $out = \TUPAPI::getString("string",$respBuf);

        $this->assertEquals($string,$out);
    }

    public function testStringFromInt()
    {
        $string  = 222;

        $buf = \TUPAPI::putString("string",$string);

        $encodeBufs['string'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);
        if($decodeRet['code'] !== 0) {

        }
        $respBuf = $decodeRet['sBuffer'];

        $out = \TUPAPI::getString("string",$respBuf);

        $this->assertEquals((string)$string,$out);
    }

    public function testSimpleVector()
    {
        $shorts = ["test1","test2"];
        $vector = new \TARS_VECTOR(\TARS::STRING);
        foreach ($shorts as $short) {
            $vector->pushBack($short);
        }

        $buf = \TUPAPI::putVector("vec",$vector);

        $encodeBufs['vec'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);
        if($decodeRet['code'] !== 0) {

        }
        $respBuf = $decodeRet['sBuffer'];

        $m = new \TARS_VECTOR(\TARS::STRING);
        $out = \TUPAPI::getVector("vec", $m, $respBuf);

        $this->assertEquals($shorts,$out);
    }

    public function testEmptyVector()
    {

        $vector = new \TARS_VECTOR(\TARS::STRING);


        $buf = \TUPAPI::putVector("vec",$vector);

        $encodeBufs['vec'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);
        if($decodeRet['code'] !== 0) {

        }
        $respBuf = $decodeRet['sBuffer'];

        $v = new \TARS_VECTOR(\TARS::STRING);
        $out = \TUPAPI::getVector("vec", $v,$respBuf);

        $this->assertEquals([],$out);
    }

    public function testSimpleMap()
    {
        $strings = ["test1"=>1,"test2"=>2];
        $map = new \TARS_MAP(\TARS::STRING,\TARS::INT64);
        foreach ($strings as $key => $value) {
            $map->pushBack([$key => $value]);
        }

        $buf = \TUPAPI::putMap("map",$map);

        $encodeBufs['map'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);

        $respBuf = $decodeRet['sBuffer'];

        $m = new \TARS_MAP(\TARS::STRING,\TARS::INT64);
        $out = \TUPAPI::getMap("map", $m, $respBuf);

        $okData = ["test1" => 1, "test2" => 2];

        $this->assertEquals($okData,$out);
    }

    public function testSimpleMapIntToString()
    {
        $strings = [["test1"=>1],["test2"=>2]];
        $map = new \TARS_MAP(\TARS::STRING,\TARS::STRING);
        foreach ($strings as $string) {
            $map->pushBack($string);
        }

        $buf = \TUPAPI::putMap("map",$map);

        $encodeBufs['map'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);

        $respBuf = $decodeRet['sBuffer'];

        $map = new \TARS_MAP(\TARS::STRING,\TARS::STRING);
        $out = \TUPAPI::getMap("map", $map,$respBuf);

        $okData = ["test1" => "1", "test2" => "2"];

        $this->assertEquals($okData,$out);
    }

    public function testEmptyMap()
    {
        $map = new \TARS_MAP(\TARS::STRING,\TARS::INT64);

        $buf = \TUPAPI::putMap("map",$map);

        $encodeBufs['map'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);


        $respBuf = $decodeRet['sBuffer'];

        $m = new \TARS_MAP(\TARS::STRING,\TARS::INT64);
        $out = \TUPAPI::getMap("map", $m, $respBuf);

        $this->assertEquals([],$out);
    }

    public function testVectorInMap()
    {
        $shorts = ["test1","test2"];
        $vector = new \TARS_VECTOR(\TARS::STRING);
        foreach ($shorts as $short) {
            $vector->pushBack($short);
        }

        $map = new \TARS_MAP(\TARS::STRING, new \TARS_VECTOR(\TARS::STRING));
        $map->pushBack(["testMap" => $vector]);

        $buf = \TUPAPI::putMap("map",$map);

        $encodeBufs['map'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);


        $respBuf = $decodeRet['sBuffer'];

        $map = new \TARS_MAP(\TARS::STRING, new \TARS_VECTOR(\TARS::STRING));
        $out = \TUPAPI::getMap("map", $map, $respBuf);

        $rightData = ['testMap' => ['test1', 'test2']];

        $this->assertEquals($rightData,$out);
    }

    public function testVectorInMap2()
    {
        $shorts = ["test1","test2"];

        $map = new \TARS_MAP(\TARS::STRING, new \TARS_VECTOR(\TARS::STRING), 1);
        $map->pushBack(['key' => 'testMap', 'value' => $shorts]);

        $buf = \TUPAPI::putMap("map",$map);

        $encodeBufs['map'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);


        $respBuf = $decodeRet['sBuffer'];

        $map = new \TARS_MAP(\TARS::STRING, new \TARS_VECTOR(\TARS::STRING));
        $out = \TUPAPI::getMap("map", $map, $respBuf);

        $rightData = ['testMap' => ['test1', 'test2']];

        $this->assertEquals($rightData,$out);
    }

    public function testSimpleStruct()
    {
        $simpleStruct = new SimpleStruct();
        $simpleStruct->id = 1;
        $simpleStruct->count = 2;

        $buf = \TUPAPI::putStruct("struct",$simpleStruct);

        $encodeBufs['struct'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);
        if($decodeRet['code'] !== 0) {

        }
        $respBuf = $decodeRet['sBuffer'];


        $outSimpleStruct = new SimpleStruct();
        $result = \TUPAPI::getStruct("struct",$outSimpleStruct,$respBuf);

        $this->fromArray($result,$outSimpleStruct);


        $this->assertEquals($simpleStruct,$outSimpleStruct);
    }

    public function testComplicateVector()
    {
        $simpleStruct1 = new SimpleStruct();
        $simpleStruct1->id = 1;
        $simpleStruct1->count = 2;

        $simpleStruct2 = new SimpleStruct();
        $simpleStruct2->id = 2;
        $simpleStruct2->count = 4;


        $vector = new \TARS_VECTOR(new SimpleStruct());

        $vector->pushBack($simpleStruct1);
        $vector->pushBack($simpleStruct2);


        $buf = \TUPAPI::putVector("vec",$vector);

        $encodeBufs['vec'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);

        $respBuf = $decodeRet['sBuffer'];

        $v = new \TARS_VECTOR(new SimpleStruct());
        $out = \TUPAPI::getVector("vec", $v,$respBuf);

        $okData = [
            [
                'id' => 1,
                'count' => 2,
                'page' => 1,
            ],
            [
                'id' => 2,
                'count' => 4,
                'page' => 1,
            ],
        ];

        $this->assertEquals($okData,$out);
    }

    public function testComplicateMap()
    {
        $simpleStruct1 = new SimpleStruct();
        $simpleStruct1->id = 1;
        $simpleStruct1->count = 2;

        $simpleStruct2 = new SimpleStruct();
        $simpleStruct2->id = 2;
        $simpleStruct2->count = 4;


        $map = new \TARS_MAP(\TARS::STRING,new SimpleStruct());

        $map->pushBack(["test1"=>$simpleStruct1]);
        $map->pushBack(["test2"=>$simpleStruct2]);


        $buf = \TUPAPI::putMap("map",$map);

        $encodeBufs['map'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);


        $respBuf = $decodeRet['sBuffer'];

        $m = new \TARS_MAP(\TARS::STRING,new SimpleStruct());
        $out = \TUPAPI::getMap("map", $m,$respBuf);

        $okData = [
            'test1' =>
                [
                    'id' => 1,
                    'count' => 2,
                    'page' => 1,
                ],
            'test2' =>
                [
                    'id' => 2,
                    'count' => 4,
                    'page' => 1,
                ],
        ];

        $this->assertEquals($okData,$out);
    }

    public function testComplicateStruct()
    {
        $simpleStruct1 = new SimpleStruct();
        $simpleStruct1->id = 1;
        $simpleStruct1->count = 2;

        //
        $nestedStruct = new NestedStruct();
        $nestedStruct->structMap->pushBack(["test1"=>$simpleStruct1]);
        $nestedStruct->structMap->pushBack(["test4"=>$simpleStruct1]);
        $nestedStruct->structList->pushBack($simpleStruct1);
        $nestedStruct->structList->pushBack($simpleStruct1);

        $nestedStruct->nestedStruct = $simpleStruct1;

        $structList = new \TARS_VECTOR(new SimpleStruct());
        $structList->pushBack($simpleStruct1);

        $nestedStruct->ext->pushBack(["test2" => $structList]);
        $nestedStruct->ext->pushBack(["test3" => $structList]);

        $buf = \TUPAPI::putStruct("struct",$nestedStruct);

        $encodeBufs['struct'] = $buf;

        $requestBuf = \TUPAPI::encode($this->iVersion,$this->iRequestId,$this->servantName,
            $this->funcName,$this->cPacketType,$this->iMessageType,$this->iTimeout,
            $this->contexts,$this->statuses,$encodeBufs);

        $decodeRet = \TUPAPI::decode($requestBuf);

        $respBuf = $decodeRet['sBuffer'];

        $nestedStructOut = new NestedStruct();

        $result = \TUPAPI::getStruct("struct",$nestedStructOut,$respBuf);

        $this->fromArray($result,$nestedStructOut);

        $this->assertEquals($simpleStruct1,$nestedStructOut->nestedStruct);

    }

    public function fromArray($data,&$structObj)
    {
        if(!empty($data)) {
            foreach ($data as $key => $value) {
                if (method_exists($structObj, 'set' . ucfirst($key))){
                    call_user_func_array([$this, 'set' . ucfirst($key)], [$value]);
                } else if ($structObj->$key instanceOf \TARS_Struct) {
                    $this->fromArray($value,$structObj->$key);
                } else {
                    $structObj->$key = $value;
                }
            }
        }
    }

}

