<?php

namespace Tars\report;

class ServerF
{
    private $_ip;
    private $_port;
    private $_objName;

    public function __construct($ip = '', $port = '', $objName = '')
    {
        $this->_ip = $ip;
        $this->_port = $port;
        $this->_objName = $objName;
    }

    public function keepAlive(ServerInfo $serverInfo)
    {
        try {
            $structBuffer = \TUPAPI::putStruct('serverInfo', $serverInfo);

            $encodeBufs['serverInfo'] = $structBuffer;

            $iVersion = 3;
            $iRequestId = 1;
            $servantName = $this->_objName;
            $funcName = __FUNCTION__;
            $cPacketType = 0;
            $iMessageType = 0;
            $tarsTimeout = 2000;
            $contexts = [];
            $statuses = [];

            $tarsRequestBuf = \TUPAPI::encode($iVersion, $iRequestId, $servantName,
                $funcName, $cPacketType, $iMessageType, $tarsTimeout, $contexts, $statuses, $encodeBufs);

            $client = new \swoole_client(SWOOLE_SOCK_TCP, SWOOLE_SOCK_ASYNC); //异步非阻塞

            $client->on('connect', function ($cli) use ($tarsRequestBuf) {
                $cli->send($tarsRequestBuf);
            });

            $client->on('receive', function ($cli, $data) {
                $cli->close();
            });

            $client->on('error', function ($cli) {
            });

            $client->on('close', function ($cli) {
            });

            $timeout = 2;
            $client->connect($this->_ip, $this->_port, $timeout);
        } catch (\Exception $e) {
            return array(
                'code' => $e->getCode(),
                'msg' => $e->getMessage(),
            );
        }
    }
}
