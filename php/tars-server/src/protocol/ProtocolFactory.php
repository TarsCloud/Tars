<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/3/9
 * Time: 下午2:36.
 */

namespace Tars\protocol;

class ProtocolFactory
{
    public static function getProtocol($protocolName)
    {
        if (empty($protocolName)) {
            return new TARSProtocol();
        } elseif ($protocolName == 'tars') {
            return new TARSProtocol();
        } elseif ($protocolName == 'json') {
            return new JSONProtocol();
        } else {
            $protocolIns = 'Tars\\protocol\\'.strtoupper($protocolName).'Protocol';

            return new $protocolIns();
        }
    }
}
