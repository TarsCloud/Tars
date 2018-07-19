<?php

namespace Tars\log;

use Tars\client\CommunicatorConfig;
use Tars\client\Communicator;
use Tars\client\RequestPacket;
use Tars\client\TUPAPIWrapper;
use Tars\log\classes\LogInfo;

class LogServant
{
    protected $_communicator;
    protected $_iVersion;
    protected $_iTimeout;
    public $_servantName = 'tars.tarslog.LogObj';

    public function __construct(CommunicatorConfig $config,
                                $servantName = "tars.tarslog.LogObj")
    {
        try {
            $this->_servantName = $servantName;
            $config->setServantName($this->_servantName);

            $this->_communicator = new Communicator($config);
            $this->_iVersion = $config->getIVersion();
            $this->_iTimeout = empty($config->getAsyncInvokeTimeout()) ? 2 : $config->getAsyncInvokeTimeout();
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function logger($app, $server, $file, $format, $buffer)
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $__buffer = TUPAPIWrapper::putString('app', 1, $app, $this->_iVersion);
            $encodeBufs['app'] = $__buffer;
            $__buffer = TUPAPIWrapper::putString('server', 2, $server, $this->_iVersion);
            $encodeBufs['server'] = $__buffer;
            $__buffer = TUPAPIWrapper::putString('file', 3, $file, $this->_iVersion);
            $encodeBufs['file'] = $__buffer;
            $__buffer = TUPAPIWrapper::putString('format', 4, $format, $this->_iVersion);
            $encodeBufs['format'] = $__buffer;
            $buffer_vec = new \TARS_Vector(\TARS::STRING);
            foreach ($buffer as $singlebuffer) {
                $buffer_vec->pushBack($singlebuffer);
            }
            $__buffer = TUPAPIWrapper::putVector('buffer', 5, $buffer_vec, $this->_iVersion);
            $encodeBufs['buffer'] = $__buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function loggerbyInfo(LogInfo $info, $buffer)
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $__buffer = TUPAPIWrapper::putStruct('info', 1, $info, $this->_iVersion);
            $encodeBufs['info'] = $__buffer;
            $buffer_vec = new \TARS_Vector(\TARS::STRING);
            foreach ($buffer as $singlebuffer) {
                $buffer_vec->pushBack($singlebuffer);
            }
            $__buffer = TUPAPIWrapper::putVector('buffer', 2, $buffer_vec, $this->_iVersion);
            $encodeBufs['buffer'] = $__buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);
        } catch (\Exception $e) {
            throw $e;
        }
    }
}
