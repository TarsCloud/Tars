<?php

namespace Tars\monitor;

use Tars\monitor\client\CommunicatorMonitor;
use Tars\monitor\client\RequestPacketMonitor;
use Tars\monitor\client\TUPAPIWrapperMonitor;

use Tars\monitor\classes\StatMicMsgHead;
use Tars\monitor\classes\StatMicMsgBody;
use Tars\monitor\classes\StatSampleMsg;

class StatFServant {
    protected $_communicator;
    protected $_iVersion;
    protected $_iTimeout;
    public $_servantName = "tars.tarsstat.StatObj";

    public function __construct($locator,$socketMode,$statServantName) {
        try {
            $this->_servantName = $statServantName;
            $this->_communicator = new CommunicatorMonitor($locator,
                $this->_servantName,$socketMode);
            $this->_iVersion = 3;
            $this->_iTimeout = 2;
        } catch (\Exception $e) {
            throw $e;
        }
    }

    public function reportMicMsg($msg,$bFromClient) {
        try {
            $requestPacket = new RequestPacketMonitor();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $msg_map = new \TARS_Map(new StatMicMsgHead(),new StatMicMsgBody(),1);
            foreach($msg as $value) {
                $msg_map->pushBack($value);
            }
            $buffer = TUPAPIWrapperMonitor::putMap("msg",1,$msg_map,$this->_iVersion);
            $encodeBufs['msg'] = $buffer;
            $buffer = TUPAPIWrapperMonitor::putBool("bFromClient",2,$bFromClient,$this->_iVersion);
            $encodeBufs['bFromClient'] = $buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $this->_communicator->invoke($requestPacket,$this->_iTimeout);

            return;

        }
        catch (\Exception $e) {
            throw $e;
        }
    }

    public function reportSampleMsg($msg) {
        try {
            $requestPacket = new RequestPacketMonitor();
            $requestPacket->_iVersion = $this->_iVersion;
            $requestPacket->_funcName = __FUNCTION__;
            $requestPacket->_servantName = $this->_servantName;
            $encodeBufs = [];

            $msg_vec = new \TARS_Vector(new StatSampleMsg());
            foreach($msg as $singlemsg) {
                $msg_vec->pushBack($singlemsg);
            }
            $buffer = TUPAPIWrapperMonitor::putVector("msg",1,$msg_vec,$this->_iVersion);
            $encodeBufs['msg'] = $buffer;
            $requestPacket->_encodeBufs = $encodeBufs;

            $this->_communicator->invoke($requestPacket,$this->_iTimeout);

            return;

        }
        catch (\Exception $e) {
            throw $e;
        }
    }

}

