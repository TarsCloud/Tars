<?php

namespace Tars\monitor;

use Tars\monitor\classes\StatPropMsgBody;
use Tars\monitor\classes\StatPropMsgHead;
use Tars\monitor\client\CommunicatorMonitor;
use Tars\monitor\client\RequestPacketMonitor;
use Tars\monitor\client\TUPAPIWrapperMonitor;

class PropertyFServant
{
    protected $_communicator;
    protected $_iVersion;
    protected $_iTimeout;
    public $_servantName; // "tars.tarsproperty.PropertyObj";

    public function __construct($locator, $socketMode, $propertyName = 'tars.tarsproperty.PropertyObj')
    {
        try {
            $this->_servantName = $propertyName;
            $this->_communicator = new CommunicatorMonitor($locator,
                $this->_servantName, $socketMode);
            $this->_iVersion = 3;
            $this->_iTimeout = 2;
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function reportPropMsg($statmsg)
    {
        try {
            $requestPacket = new RequestPacketMonitor();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $statmsg_map = new \TARS_Map(new StatPropMsgHead(), new StatPropMsgBody(), 1);
            foreach ($statmsg as  $value) {
                $statmsg_map->pushBack($value);
            }
            $buffer = TUPAPIWrapperMonitor::putMap('statmsg', 1, $statmsg_map, $this->_iVersion);
            $encodeBufs['statmsg'] = $buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $this->_communicator->invoke($requestPacket, $this->_iTimeout);

            return;
        } catch (\Exception $e) {
            throw $e;
        }
    }
}
