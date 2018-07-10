<?php

namespace Tars\config;

use Tars\client\CommunicatorConfig;
use Tars\client\Communicator;
use Tars\client\RequestPacket;
use Tars\client\TUPAPIWrapper;
use Tars\config\classes\ConfigInfo;
use Tars\config\classes\GetConfigListInfo;

class ConfigServant
{
    protected $_communicator;
    protected $_iVersion;
    protected $_iTimeout;
    public $_servantName = 'tars.tarsconfig.ConfigObj';

    public function __construct(CommunicatorConfig $config,
                                $servantName = "tars.tarsconfig.ConfigObj")
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

    public function ListConfig($app, $server, &$vf)
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $buffer = TUPAPIWrapper::putString('app', 1, $app, $this->_iVersion);
            $encodeBufs['app'] = $buffer;
            $buffer = TUPAPIWrapper::putString('server', 2, $server, $this->_iVersion);
            $encodeBufs['server'] = $buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $vf = TUPAPIWrapper::getVector('vf', 3, new \TARS_Vector(\TARS::STRING), $sBuffer, $this->_iVersion);

            return TUPAPIWrapper::getInt32('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function loadConfig($app, $server, $filename, &$config)
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $buffer = TUPAPIWrapper::putString('app', 1, $app, $this->_iVersion);
            $encodeBufs['app'] = $buffer;
            $buffer = TUPAPIWrapper::putString('server', 2, $server, $this->_iVersion);
            $encodeBufs['server'] = $buffer;
            $buffer = TUPAPIWrapper::putString('filename', 3, $filename, $this->_iVersion);
            $encodeBufs['filename'] = $buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $config = TUPAPIWrapper::getString('config', 4, $sBuffer, $this->_iVersion);

            return TUPAPIWrapper::getInt32('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function loadConfigByHost($appServerName, $filename, $host, &$config)
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $buffer = TUPAPIWrapper::putString('appServerName', 1, $appServerName, $this->_iVersion);
            $encodeBufs['appServerName'] = $buffer;
            $buffer = TUPAPIWrapper::putString('filename', 2, $filename, $this->_iVersion);
            $encodeBufs['filename'] = $buffer;
            $buffer = TUPAPIWrapper::putString('host', 3, $host, $this->_iVersion);
            $encodeBufs['host'] = $buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $config = TUPAPIWrapper::getString('config', 4, $sBuffer, $this->_iVersion);

            return TUPAPIWrapper::getInt32('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function checkConfig($appServerName, $filename, $host, &$result)
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $buffer = TUPAPIWrapper::putString('appServerName', 1, $appServerName, $this->_iVersion);
            $encodeBufs['appServerName'] = $buffer;
            $buffer = TUPAPIWrapper::putString('filename', 2, $filename, $this->_iVersion);
            $encodeBufs['filename'] = $buffer;
            $buffer = TUPAPIWrapper::putString('host', 3, $host, $this->_iVersion);
            $encodeBufs['host'] = $buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $result = TUPAPIWrapper::getString('result', 4, $sBuffer, $this->_iVersion);

            return TUPAPIWrapper::getInt32('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function ListConfigByInfo(ConfigInfo $configInfo, &$vf)
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $buffer = TUPAPIWrapper::putStruct('configInfo', 1, $configInfo, $this->_iVersion);
            $encodeBufs['configInfo'] = $buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $vf = TUPAPIWrapper::getVector('vf', 2, new \TARS_Vector(\TARS::STRING), $sBuffer, $this->_iVersion);

            return TUPAPIWrapper::getInt32('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function loadConfigByInfo(ConfigInfo $configInfo, &$config)
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $buffer = TUPAPIWrapper::putStruct('configInfo', 1, $configInfo, $this->_iVersion);
            $encodeBufs['configInfo'] = $buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $config = TUPAPIWrapper::getString('config', 2, $sBuffer, $this->_iVersion);

            return TUPAPIWrapper::getInt32('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function checkConfigByInfo(ConfigInfo $configInfo, &$result)
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $buffer = TUPAPIWrapper::putStruct('configInfo', 1, $configInfo, $this->_iVersion);
            $encodeBufs['configInfo'] = $buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $result = TUPAPIWrapper::getString('result', 2, $sBuffer, $this->_iVersion);

            return TUPAPIWrapper::getInt32('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function ListAllConfigByInfo(GetConfigListInfo $configInfo, &$vf)
    {
        try {
            $requestPacket = new RequestPacket();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $buffer = TUPAPIWrapper::putStruct('configInfo', 1, $configInfo, $this->_iVersion);
            $encodeBufs['configInfo'] = $buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $vf = TUPAPIWrapper::getVector('vf', 2, new \TARS_Vector(\TARS::STRING), $sBuffer, $this->_iVersion);

            return TUPAPIWrapper::getInt32('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }
}
