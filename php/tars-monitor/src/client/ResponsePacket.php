<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/4/29
 * Time: 下午3:53.
 */

namespace Tars\monitor\client;

class ResponsePacket
{
    public $_responseBuf;
    public $iVersion;

    public function decode()
    {
        // 接下来解码
        $decodeRet = \TUPAPI::decode($this->_responseBuf, $this->iVersion);
        if ($decodeRet['iRet'] !== 0) {
            $msg = isset($decodeRet['sResultDesc'])?$decodeRet['sResultDesc']:"";
            throw new \Exception($msg, $decodeRet['iRet']);
        }
        $sBuffer = $decodeRet['sBuffer'];

        return $sBuffer;
    }
}
