<?php

namespace Tars\report;

class ServerFSync
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

            $time = microtime(true);

            $client = new \swoole_client(SWOOLE_SOCK_TCP | SWOOLE_KEEP);

            $timeout = 2;
            if (!$client->connect($this->_ip, $this->_port, $timeout)) {
                throw new \Exception();
            }

            if (!$client->send($tarsRequestBuf)) {
                $client->close();
                throw new \Exception();
            }

            $totalLen = 0;
            $tarsResponseBuf = null;
            while (true) {
                if (microtime(true) - $time > $timeout) {
                    $client->close();
                    throw new \Exception();
                }

                //读取最多32M的数据
                $data = $client->recv(65535);

                if (empty($data)) {
                    $client->close();
                    // 已经断开连接
                    throw new \Exception();
                } else {
                    //第一个包
                    if ($tarsResponseBuf === null) {
                        $tarsResponseBuf = $data;

                        //在这里从第一个包中获取总包长
                        $list = unpack('Nlen', substr($data, 0, 4));
                        $totalLen = $list['len'];
                    } else {
                        $tarsResponseBuf .= $data;
                    }

                    //check if all package is receved
                    if (strlen($tarsResponseBuf) >= $totalLen) {
                        $client->close();

                        return 0;
                    }
                }
            }
        } catch (\Exception $e) {
            return -1;
        }
    }
}
