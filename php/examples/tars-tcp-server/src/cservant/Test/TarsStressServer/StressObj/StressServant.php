<?php

namespace Server\cservant\Test\TarsStressServer\StressObj;

use Tars\client\CommunicatorConfig;
use Tars\client\Communicator;
use Tars\client\RequestPacket;
use Tars\client\TUPAPIWrapper;

class StressServant
{
    protected $_communicator;
    protected $_iVersion;
    protected $_iTimeout;
    public $_servantName = 'Test.TarsStressServer.StressObj';

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

    public function test()
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

    public function testStr($sIn, &$sOut)
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $__buffer = TUPAPIWrapper::putString('sIn', 1, $sIn, $this->_iVersion);
            $encodeBufs['sIn'] = $__buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $sOut = TUPAPIWrapper::getString('sOut', 2, $sBuffer, $this->_iVersion);

            return TUPAPIWrapper::getInt32('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }
}
