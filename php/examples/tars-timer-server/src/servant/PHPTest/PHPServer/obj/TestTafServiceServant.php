<?php

namespace TimerServer\servant\PHPTest\PHPServer\obj;

use Tars\client\CommunicatorConfig;
use Tars\client\Communicator;
use Tars\client\RequestPacket;
use Tars\client\TUPAPIWrapper;
use TimerServer\servant\PHPTest\PHPServer\obj\classes\LotofTags;
use TimerServer\servant\PHPTest\PHPServer\obj\classes\SimpleStruct;
use TimerServer\servant\PHPTest\PHPServer\obj\classes\OutStruct;
use TimerServer\servant\PHPTest\PHPServer\obj\classes\ComplicatedStruct;

class TestTafServiceServant
{
    protected $_communicator;
    protected $_iVersion;
    protected $_iTimeout;
    public $_servantName = 'PHPTest.PHPServer.obj';

    public function __construct(CommunicatorConfig $config)
    {
        try {
            $config->setServantName($this->_servantName);
            $this->_communicator = new Communicator($config);
            $this->_iVersion = $config->getIVersion();
            $this->_iTimeout = empty($config->getAsyncInvokeTimeout()) ? 2 : $config->getAsyncInvokeTimeout();
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function testTafServer()
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function testLofofTags(LotofTags $tags, LotofTags &$outtags)
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $__buffer = TUPAPIWrapper::putStruct('tags', 1, $tags, $this->_iVersion);
            $encodeBufs['tags'] = $__buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $ret = TUPAPIWrapper::getStruct('outtags', 2, $outtags, $sBuffer, $this->_iVersion);

            return TUPAPIWrapper::getInt32('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function sayHelloWorld($name, &$outGreetings)
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $__buffer = TUPAPIWrapper::putString('name', 1, $name, $this->_iVersion);
            $encodeBufs['name'] = $__buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $outGreetings = TUPAPIWrapper::getString('outGreetings', 2, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function testBasic($a, $b, $c, &$d, &$e, &$f)
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $__buffer = TUPAPIWrapper::putBool('a', 1, $a, $this->_iVersion);
            $encodeBufs['a'] = $__buffer;
            $__buffer = TUPAPIWrapper::putInt32('b', 2, $b, $this->_iVersion);
            $encodeBufs['b'] = $__buffer;
            $__buffer = TUPAPIWrapper::putString('c', 3, $c, $this->_iVersion);
            $encodeBufs['c'] = $__buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $d = TUPAPIWrapper::getBool('d', 4, $sBuffer, $this->_iVersion);
            $e = TUPAPIWrapper::getInt32('e', 5, $sBuffer, $this->_iVersion);
            $f = TUPAPIWrapper::getString('f', 6, $sBuffer, $this->_iVersion);

            return TUPAPIWrapper::getInt32('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function testStruct($a, SimpleStruct $b, OutStruct &$d)
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $__buffer = TUPAPIWrapper::putInt64('a', 1, $a, $this->_iVersion);
            $encodeBufs['a'] = $__buffer;
            $__buffer = TUPAPIWrapper::putStruct('b', 2, $b, $this->_iVersion);
            $encodeBufs['b'] = $__buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $ret = TUPAPIWrapper::getStruct('d', 3, $d, $sBuffer, $this->_iVersion);

            return TUPAPIWrapper::getString('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function testMap($a, SimpleStruct $b, $m1, OutStruct &$d, &$m2)
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $__buffer = TUPAPIWrapper::putShort('a', 1, $a, $this->_iVersion);
            $encodeBufs['a'] = $__buffer;
            $__buffer = TUPAPIWrapper::putStruct('b', 2, $b, $this->_iVersion);
            $encodeBufs['b'] = $__buffer;
            $m1_map = new \TARS_Map(\TARS::STRING, \TARS::STRING);
            foreach ($m1 as $key => $value) {
                $m1_map->pushBack([$key => $value]);
            }
            $__buffer = TUPAPIWrapper::putMap('m1', 3, $m1_map, $this->_iVersion);
            $encodeBufs['m1'] = $__buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $ret = TUPAPIWrapper::getStruct('d', 4, $d, $sBuffer, $this->_iVersion);
            $m2 = TUPAPIWrapper::getMap('m2', 5, new \TARS_Map(\TARS::INT32, new SimpleStruct()), $sBuffer, $this->_iVersion);

            return TUPAPIWrapper::getString('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function testVector($a, $v1, $v2, &$v3, &$v4)
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $__buffer = TUPAPIWrapper::putInt32('a', 1, $a, $this->_iVersion);
            $encodeBufs['a'] = $__buffer;
            $v1_vec = new \TARS_Vector(\TARS::STRING);
            foreach ($v1 as $singlev1) {
                $v1_vec->pushBack($singlev1);
            }
            $__buffer = TUPAPIWrapper::putVector('v1', 2, $v1_vec, $this->_iVersion);
            $encodeBufs['v1'] = $__buffer;
            $v2_vec = new \TARS_Vector(new SimpleStruct());
            foreach ($v2 as $singlev2) {
                $v2_vec->pushBack($singlev2);
            }
            $__buffer = TUPAPIWrapper::putVector('v2', 3, $v2_vec, $this->_iVersion);
            $encodeBufs['v2'] = $__buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $v3 = TUPAPIWrapper::getVector('v3', 4, new \TARS_Vector(\TARS::INT32), $sBuffer, $this->_iVersion);
            $v4 = TUPAPIWrapper::getVector('v4', 5, new \TARS_Vector(new OutStruct()), $sBuffer, $this->_iVersion);

            return TUPAPIWrapper::getString('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function testReturn()
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $returnVal = new SimpleStruct();
            TUPAPIWrapper::getStruct('', 0, $returnVal, $sBuffer, $this->_iVersion);

            return $returnVal;
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function testReturn2()
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            return TUPAPIWrapper::getMap('', 0, new \TARS_Map(\TARS::STRING, \TARS::STRING), $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function testComplicatedStruct(ComplicatedStruct $cs, $vcs, ComplicatedStruct &$ocs, &$ovcs)
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $__buffer = TUPAPIWrapper::putStruct('cs', 1, $cs, $this->_iVersion);
            $encodeBufs['cs'] = $__buffer;
            $vcs_vec = new \TARS_Vector(new ComplicatedStruct());
            foreach ($vcs as $singlevcs) {
                $vcs_vec->pushBack($singlevcs);
            }
            $__buffer = TUPAPIWrapper::putVector('vcs', 2, $vcs_vec, $this->_iVersion);
            $encodeBufs['vcs'] = $__buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $ret = TUPAPIWrapper::getStruct('ocs', 3, $ocs, $sBuffer, $this->_iVersion);
            $ovcs = TUPAPIWrapper::getVector('ovcs', 4, new \TARS_Vector(new ComplicatedStruct()), $sBuffer, $this->_iVersion);

            return TUPAPIWrapper::getVector('', 0, new \TARS_Vector(new SimpleStruct()), $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function testComplicatedMap($mcs, &$omcs)
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $mcs_map = new \TARS_Map(\TARS::STRING, new ComplicatedStruct());
            foreach ($mcs as $key => $value) {
                $mcs_map->pushBack([$key => $value]);
            }
            $__buffer = TUPAPIWrapper::putMap('mcs', 1, $mcs_map, $this->_iVersion);
            $encodeBufs['mcs'] = $__buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $omcs = TUPAPIWrapper::getMap('omcs', 2, new \TARS_Map(\TARS::STRING, new ComplicatedStruct()), $sBuffer, $this->_iVersion);

            return TUPAPIWrapper::getMap('', 0, new \TARS_Map(\TARS::STRING, new ComplicatedStruct()), $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function testEmpty($a, &$b1, &$in2, OutStruct &$d, &$v3, &$v4)
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $__buffer = TUPAPIWrapper::putShort('a', 1, $a, $this->_iVersion);
            $encodeBufs['a'] = $__buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $b1 = TUPAPIWrapper::getBool('b1', 2, $sBuffer, $this->_iVersion);
            $in2 = TUPAPIWrapper::getInt32('in2', 3, $sBuffer, $this->_iVersion);
            $ret = TUPAPIWrapper::getStruct('d', 4, $d, $sBuffer, $this->_iVersion);
            $v3 = TUPAPIWrapper::getVector('v3', 5, new \TARS_Vector(new OutStruct()), $sBuffer, $this->_iVersion);
            $v4 = TUPAPIWrapper::getVector('v4', 6, new \TARS_Vector(\TARS::INT32), $sBuffer, $this->_iVersion);

            return TUPAPIWrapper::getInt32('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function testSelf()
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            return TUPAPIWrapper::getInt32('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }
    public function testProperty()
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            return TUPAPIWrapper::getInt32('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }
}
