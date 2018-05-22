<?php

namespace Tars\registry;

use Tars\registry\client\CommunicatorRegistry;
use Tars\registry\client\RequestPacketRegistry;
use Tars\registry\client\TUPAPIWrapperRegistry;

class QueryFServant
{
    protected $_communicator;
    protected $_iVersion;
    protected $_iTimeout;
    public $_servantName; //"tars.tarsregistry.QueryObj";

    public function __construct($routeInfo, $socketMode, $locatorName)
    {
        try {
            $this->_servantName = $locatorName;
            $this->_communicator = new CommunicatorRegistry($routeInfo,
                2, $socketMode, 3);
            $this->_iVersion = 3;
            $this->_iTimeout = 2;
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function findObjectById($id)
    {
        try {
            $requestPacket = new RequestPacketRegistry();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $buffer = TUPAPIWrapperRegistry::putString('id', 1, $id, $this->_iVersion);
            $encodeBufs['id'] = $buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            return TUPAPIWrapperRegistry::getVector('', 0, new \TARS_Vector(new EndpointF()), $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function findObjectById4Any($id, &$activeEp, &$inactiveEp)
    {
        try {
            $requestPacket = new RequestPacketRegistry();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $buffer = TUPAPIWrapperRegistry::putString('id', 1, $id, $this->_iVersion);
            $encodeBufs['id'] = $buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $activeEp = TUPAPIWrapperRegistry::getVector('activeEp', 2, new \TARS_Vector(new EndpointF()), $sBuffer, $this->_iVersion);
            $inactiveEp = TUPAPIWrapperRegistry::getVector('inactiveEp', 3, new \TARS_Vector(new EndpointF()), $sBuffer, $this->_iVersion);

            return TUPAPIWrapperRegistry::getInt32('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function findObjectById4All($id, &$activeEp, &$inactiveEp)
    {
        try {
            $requestPacket = new RequestPacketRegistry();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $buffer = TUPAPIWrapperRegistry::putString('id', 1, $id, $this->_iVersion);
            $encodeBufs['id'] = $buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $activeEp = TUPAPIWrapperRegistry::getVector('activeEp', 2, new \TARS_Vector(new EndpointF()), $sBuffer, $this->_iVersion);
            $inactiveEp = TUPAPIWrapperRegistry::getVector('inactiveEp', 3, new \TARS_Vector(new EndpointF()), $sBuffer, $this->_iVersion);

            return TUPAPIWrapperRegistry::getInt32('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function findObjectByIdInSameGroup($id, &$activeEp, &$inactiveEp)
    {
        try {
            $requestPacket = new RequestPacketRegistry();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $buffer = TUPAPIWrapperRegistry::putString('id', 1, $id, $this->_iVersion);
            $encodeBufs['id'] = $buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $activeEp = TUPAPIWrapperRegistry::getVector('activeEp', 2, new \TARS_Vector(new EndpointF()), $sBuffer, $this->_iVersion);
            $inactiveEp = TUPAPIWrapperRegistry::getVector('inactiveEp', 3, new \TARS_Vector(new EndpointF()), $sBuffer, $this->_iVersion);

            return TUPAPIWrapperRegistry::getInt32('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function findObjectByIdInSameStation($id, $sStation, &$activeEp, &$inactiveEp)
    {
        try {
            $requestPacket = new RequestPacketRegistry();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $buffer = TUPAPIWrapperRegistry::putString('id', 1, $id, $this->_iVersion);
            $encodeBufs['id'] = $buffer;
            $buffer = TUPAPIWrapperRegistry::putString('sStation', 2, $sStation, $this->_iVersion);
            $encodeBufs['sStation'] = $buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $activeEp = TUPAPIWrapperRegistry::getVector('activeEp', 3, new \TARS_Vector(new EndpointF()), $sBuffer, $this->_iVersion);
            $inactiveEp = TUPAPIWrapperRegistry::getVector('inactiveEp', 4, new \TARS_Vector(new EndpointF()), $sBuffer, $this->_iVersion);

            return TUPAPIWrapperRegistry::getInt32('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function findObjectByIdInSameSet($id, $setId, &$activeEp, &$inactiveEp)
    {
        try {
            $requestPacket = new RequestPacketRegistry();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $buffer = TUPAPIWrapperRegistry::putString('id', 1, $id, $this->_iVersion);
            $encodeBufs['id'] = $buffer;
            $buffer = TUPAPIWrapperRegistry::putString('setId', 2, $setId, $this->_iVersion);
            $encodeBufs['setId'] = $buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $sBuffer = $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            $activeEp = TUPAPIWrapperRegistry::getVector('activeEp', 3, new \TARS_Vector(new EndpointF()), $sBuffer, $this->_iVersion);
            $inactiveEp = TUPAPIWrapperRegistry::getVector('inactiveEp', 4, new \TARS_Vector(new EndpointF()), $sBuffer, $this->_iVersion);

            return TUPAPIWrapperRegistry::getInt32('', 0, $sBuffer, $this->_iVersion);
        } catch (\Exception $e) {
            throw $e;
        }
    }
}
