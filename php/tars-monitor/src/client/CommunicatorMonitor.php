<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/4/29
 * Time: 下午12:55.
 */

namespace Tars\monitor\client;

use Tars\registry\QueryFWrapper;

class CommunicatorMonitor
{
    protected $_moduleName;
    protected $_localIp;
    protected $_locator;
    protected $_sIp;
    protected $_iPort;

    protected $_timeout;
    protected $_setdivision;
    protected $_enableset;

    protected $_routeInfo;

    protected $_socketMode; //1 socket ,2 swoole sync ,3 swoole coroutine
    protected $_iVersion;

    public function __construct($locator, $servantName,
                                $socketMode)
    {
        $this->_locator = $locator;
        $this->_timeout = 2;

        $this->_servantName = $servantName;

        $this->_socketMode = $socketMode;
        $this->_iVersion = 3;

        // 完成服务的路由
        $this->_queryF = new QueryFWrapper($this->_locator, $this->_socketMode, 60000);
        $this->_routeInfo = $this->_queryF->findObjectById($this->_servantName);
    }

    // 同步的socket tcp收发
    public function invoke(RequestPacketMonitor $requestPacket, $timeout)
    {
        try {
            $requestBuf = $requestPacket->encode();

            $count = count($this->_routeInfo) - 1;
            $index = rand(0, $count);
            $sIp = $this->_routeInfo[$index]['sIp'];
            $iPort = $this->_routeInfo[$index]['iPort'];

            switch ($this->_socketMode) {
                // 单纯的socket
                case 1:{
                    $this->socketTcp($sIp, $iPort,
                        $requestBuf, $timeout);
                    break;
                }
                case 2:{
                    $this->swooleTcp($sIp, $iPort,
                        $requestBuf, $timeout);
                    break;
                }
                case 3:{
                    $this->swooleCoroutineTcp($sIp, $iPort,
                        $requestBuf, $timeout);
                    break;
                }
            }

            return;
        } catch (\Exception $e) {
            throw $e;
        }
    }

    private function socketTcp($sIp, $iPort, $requestBuf, $timeout = 2)
    {
        $sock = \socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

        if (false === $sock) {
            throw new \Exception(CodeMonitor::getErrMsg(CodeMonitor::TARS_SOCKET_CREATE_FAILED), CodeMonitor::TARS_SOCKET_CREATE_FAILED);
        }

        if (!\socket_connect($sock, $sIp, $iPort)) {
            \socket_close($sock);
            throw new \Exception();
        }

        if (!\socket_write($sock, $requestBuf, strlen($requestBuf))) {
            \socket_close($sock);
            throw new \Exception();
        }
        \socket_close($sock);

        return;
    }

    private function swooleTcp($sIp, $iPort, $requestBuf, $timeout = 2)
    {
        $client = new \swoole_client(SWOOLE_SOCK_TCP | SWOOLE_KEEP);

        if (!$client->connect($sIp, $iPort, $timeout)) {
            $code = CodeMonitor::TARS_SOCKET_CONNECT_FAILED;
            throw new \Exception(CodeMonitor::getErrMsg($code), $code);
        }

        if (!$client->send($requestBuf)) {
            $client->close();
            $code = CodeMonitor::TARS_SOCKET_SEND_FAILED;
            throw new \Exception(CodeMonitor::getErrMsg($code), $code);
        }
        $client->close();

        return;
    }

    private function swooleCoroutineTcp($sIp, $iPort, $requestBuf, $timeout = 2)
    {
        $client = new \Swoole\Coroutine\Client(SWOOLE_SOCK_TCP);

        if (!$client->connect($sIp, $iPort, $timeout)) {
            $code = CodeMonitor::TARS_SOCKET_CONNECT_FAILED;
            throw new \Exception(CodeMonitor::getErrMsg($code), $code);
        }

        if (!$client->send($requestBuf)) {
            $client->close();
            $code = CodeMonitor::TARS_SOCKET_SEND_FAILED;
            throw new \Exception(CodeMonitor::getErrMsg($code), $code);
        }
        $client->close();

        return;
    }
}
