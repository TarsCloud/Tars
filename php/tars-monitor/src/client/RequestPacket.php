<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/4/29
 * Time: 下午3:53.
 */

namespace Tars\monitor\client;

class RequestPacket
{
    public $_encodeBufs = array();
    public $_requestBuf;
    public $_responseBuf;
    public $_sBuffer;

    public $_iVersion;

    public $_servantName;
    public $_funcName;

    public $_iRequestId = 1;

    public $_cPacketType = 0;
    public $_iMessageType = 0;
    public $_tarsTimeout = 2000;
    public $_iTimeout = 2;
    public $_contexts = [];
    public $_statuses = [];

    public function encode()
    {
        if ($this->_iVersion === Consts::TARSVERSION) {
            // 需要对数据进行兼容
            $newEncodeBufs = [];
            foreach ($this->_encodeBufs as $buf) {
                $newEncodeBufs[] = $buf;
            }
            $requestBuf = \TUPAPI::encode($this->_iVersion, $this->_iRequestId,
                $this->_servantName, $this->_funcName, $this->_cPacketType,
                $this->_iMessageType, $this->_tarsTimeout, $this->_contexts,
                $this->_statuses, $newEncodeBufs);
        } else {
            $requestBuf = \TUPAPI::encode($this->_iVersion, $this->_iRequestId,
                $this->_servantName, $this->_funcName, $this->_cPacketType,
                $this->_iMessageType, $this->_tarsTimeout, $this->_contexts,
                $this->_statuses, $this->_encodeBufs);
        }

        return $requestBuf;
    }
}
