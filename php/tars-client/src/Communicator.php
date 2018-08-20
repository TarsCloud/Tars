<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/4/29
 * Time: 下午12:55.
 */

namespace Tars\client;

use Tars\monitor\StatFWrapper;
use Tars\registry\QueryFWrapper;

class Communicator
{
    protected $_moduleName;
    protected $_servantName;

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

    // monitorHelper?
    protected $_statF;
    protected $_statServantName;
    // registrHelper?
    protected $_queryF;
    protected $_refreshEndpointInterval;

    public function __construct(CommunicatorConfig $config)
    {
        $this->_moduleName = $config->getModuleName();
        $this->_servantName = $config->getServantName();
        $this->_localIp = $config->getLocalip();
        $this->_locator = $config->getLocator();
        $this->_timeout = $config->getConnectTimeout();
        $this->_setdivision = $config->getSetDivision();
        $this->_enableset = $config->isEnableSet();

        $this->_socketMode = $config->getSocketMode();
        $this->_iVersion = $config->getIVersion();

        $this->_statServantName = $config->getStat();

        $this->_refreshEndpointInterval = empty($config->getRefreshEndpointInterval())
            ? 60000 : $config->getRefreshEndpointInterval();

        // 如果已经有配置的地址的话,直接选用
        if (!empty($config->getRouteInfo())) {
            $this->_routeInfo = $config->getRouteInfo();
        } else {
            // 完成服务的路由
            $this->_queryF = new QueryFWrapper($this->_locator, $this->_socketMode, $this->_refreshEndpointInterval);
            $this->_routeInfo = $this->_queryF->findObjectById($this->_servantName);
            // 初始化上报组件,只在指定了主控的前提下
            $reportInterval = empty($config->getReportInterval()) ? 60000 : $config->getReportInterval();
            $this->_statF = new StatFWrapper($this->_locator, $this->_socketMode,
                $this->_statServantName, $this->_moduleName, $reportInterval);
        }

    }

    // 同步的socket tcp收发
    public function invoke(RequestPacket $requestPacket, $timeout, $sIp = '', $iPort = 0)
    {
        // 转换成网络需要的timeout
        $timeout = $timeout / 1000;

        $startTime = $this->militime();
        $count = count($this->_routeInfo) - 1;
        if ($count === -1) {
            throw new \Exception('Rout fail', Code::ROUTE_FAIL);
        }
        $index = rand(0, $count);
        $ip = empty($sIp) ? $this->_routeInfo[$index]['sIp'] : $sIp;
        $port = empty($iPort) ? $this->_routeInfo[$index]['iPort'] : $iPort;
        $bTcp = isset($this->_routeInfo[$index]['bTcp']) ?
            $this->_routeInfo[$index]['bTcp'] : 1;

        try {
            $requestBuf = $requestPacket->encode();
            $responseBuf = '';
            if ($bTcp) {
                switch ($this->_socketMode) {
                    // 单纯的socket
                    case 1:{
                        $responseBuf = $this->socketTcp($ip, $port,
                            $requestBuf, $timeout);
                        break;
                    }
                    case 2:{
                        $responseBuf = $this->swooleTcp($ip, $port,
                            $requestBuf, $timeout);
                        break;
                    }
                    case 3:{
                        $responseBuf = $this->swooleCoroutineTcp($ip, $port,
                            $requestBuf, $timeout);
                        break;
                    }
                }
            } else {
                switch ($this->_socketMode) {
                    // 单纯的socket
                    case 1:{
                        $responseBuf = $this->socketUdp($ip, $port, $requestBuf, $timeout);
                        break;
                    }
                    case 2:{
                        $responseBuf = $this->swooleUdp($ip, $port, $requestBuf, $timeout);
                        break;
                    }
                    case 3:{
                        $responseBuf = $this->swooleCoroutineUdp($ip, $port, $requestBuf, $timeout);
                        break;
                    }
                }
            }

            $responsePacket = new ResponsePacket();
            $responsePacket->_responseBuf = $responseBuf;
            $responsePacket->iVersion = $this->_iVersion;
            $sBuffer = $responsePacket->decode();

            $endTime = $this->militime();

            if(!is_null($this->_locator))
            {
                $this->_statF->addStat($requestPacket->_servantName, $requestPacket->_funcName, $ip,
                    $port, ($endTime - $startTime), 0, 0);
            }

            return $sBuffer;
        } catch (\Exception $e) {
            $endTime = $this->militime();

            if(!is_null($this->_locator))
            {
                $this->_statF->addStat($requestPacket->_servantName, $requestPacket->_funcName, $ip,
                    $port, ($endTime - $startTime), $e->getCode(), $e->getCode());
            }
            throw $e;
        }
    }

    private function socketTcp($sIp, $iPort, $requestBuf, $timeout = 2)
    {
        $time = microtime(true);

        $sock = \socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

        if (false === $sock) {
            throw new \Exception(Code::getErrMsg(Code::TARS_SOCKET_CREATE_FAILED), Code::TARS_SOCKET_CREATE_FAILED);
        }

        if (!\socket_connect($sock, $sIp, $iPort)) {
            \socket_close($sock);
            throw new \Exception();
        }

        if (!\socket_write($sock, $requestBuf, strlen($requestBuf))) {
            \socket_close($sock);
            throw new \Exception();
        }

        $totalLen = 0;
        $responseBuf = null;
        while (true) {
            if (microtime(true) - $time > $timeout) {
                \socket_close($sock);
                throw new \Exception();
            }
            //读取最多32M的数据
            $data = \socket_read($sock, 65536, PHP_BINARY_READ);

            if (empty($data)) {
                // 已经断开连接
                return '';
            } else {
                //第一个包
                if ($responseBuf === null) {
                    $responseBuf = $data;
                    //在这里从第一个包中获取总包长
                    $list = unpack('Nlen', substr($data, 0, 4));
                    $totalLen = $list['len'];
                } else {
                    $responseBuf .= $data;
                }

                //check if all package is receved
                if (strlen($responseBuf) >= $totalLen) {
                    \socket_close($sock);
                    break;
                }
            }
        }

        return $responseBuf;
    }

    private function socketUdp($sIp, $iPort, $requestBuf, $timeout = 2)
    {
        $time = microtime(true);
        $sock = \socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);

        if (false === $sock) {
            throw new \Exception(Code::getErrMsg(Code::TARS_SOCKET_CREATE_FAILED), Code::TARS_SOCKET_CREATE_FAILED);
        }

        if (!\socket_set_nonblock($sock)) {
            \socket_close($sock);
            $code = Code::TARS_SOCKET_SET_NONBLOCK_FAILED; // 设置socket非阻塞失败
            throw new \Exception(Code::getErrMsg($code), $code);
        }

        $len = strlen($requestBuf);
        if (\socket_sendto($sock, $requestBuf, $len, 0x100, $sIp, $iPort) != $len) {
            \socket_close($sock);
            $code = Code::TARS_SOCKET_SEND_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }

        if (0 == $timeout) {
            \socket_close($sock);

            return ''; // 无回包的情况，返回成功
        }

        $read = array($sock);
        $second = floor($timeout);
        $usecond = ($timeout - $second) * 1000000;
        $ret = \socket_select($read, $write, $except, $second, $usecond);

        if (false === $ret) {
            \socket_close($sock);
            $code = Code::TARS_SOCKET_RECEIVE_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        } elseif ($ret != 1) {
            \socket_close($sock);
            $code = Code::TARS_SOCKET_SELECT_TIMEOUT;
            throw new \Exception(Code::getErrMsg($code), $code);
        }

        $out = null;
        $responseBuf = null;
        while (true) {
            if (microtime(true) - $time > $timeout) {
                \socket_close($sock);
                $code = Code::TARS_SOCKET_TIMEOUT;
                throw new \Exception(Code::getErrMsg($code), $code);
            }

            // 32k：32768 = 1024 * 32
            $outLen = @\socket_recvfrom($sock, $out, 32768, 0, $ip, $port);
            if (!($outLen > 0 && $out != '')) {
                continue;
            }
            $responseBuf = $out;
            \socket_close($sock);

            return $responseBuf;
        }
    }
    private function swooleTcp($sIp, $iPort, $requestBuf, $timeout = 2)
    {
        $client = new \swoole_client(SWOOLE_SOCK_TCP | SWOOLE_KEEP);

        $client->set(array(
            'open_length_check' => 1,
            'package_length_type' => 'N',
            'package_length_offset' => 0,       //第N个字节是包长度的值
            'package_body_offset' => 0,       //第几个字节开始计算长度
            'package_max_length' => 2000000,  //协议最大长度
        ));

        if (!$client->connect($sIp, $iPort, $timeout)) {
            $code = Code::TARS_SOCKET_CONNECT_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }

        if (!$client->send($requestBuf)) {
            $client->close();
            $code = Code::TARS_SOCKET_SEND_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
        //读取最多32M的数据
        $tarsResponseBuf = $client->recv();

        if (empty($tarsResponseBuf)) {
            $client->close();
            // 已经断开连接
            $code = Code::TARS_SOCKET_RECEIVE_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }

        return $tarsResponseBuf;
    }

    private function swooleUdp($sIp, $iPort, $requestBuf, $timeout = 2)
    {
        $client = new \swoole_client(SWOOLE_SOCK_UDP);

        $client->set(array(
            'open_length_check' => 1,
            'package_length_type' => 'N',
            'package_length_offset' => 0,       //第N个字节是包长度的值
            'package_body_offset' => 0,       //第几个字节开始计算长度
            'package_max_length' => 2000000,  //协议最大长度
        ));

        if (!$client->connect($sIp, $iPort, $timeout)) {
            $code = Code::TARS_SOCKET_CONNECT_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }

        if (!$client->send($requestBuf)) {
            $client->close();
            $code = Code::TARS_SOCKET_SEND_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
        //读取最多32M的数据
        $tarsResponseBuf = $client->recv();

        if (empty($tarsResponseBuf)) {
            $client->close();
            // 已经断开连接
            $code = Code::TARS_SOCKET_RECEIVE_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }

        return $tarsResponseBuf;
    }

    private function swooleCoroutineTcp($sIp, $iPort, $requestBuf, $timeout = 2)
    {
        $client = new \Swoole\Coroutine\Client(SWOOLE_SOCK_TCP);

//        $client->set(array(
//            'open_length_check'     => 1,
//            'package_length_type'   => 'N',
//            'package_length_offset' => 0,       //第N个字节是包长度的值
//            'package_body_offset'   => 0,       //第几个字节开始计算长度
//            'package_max_length'    => 2000000,  //协议最大长度
//        ));

        if (!$client->connect($sIp, $iPort, $timeout)) {
            $code = Code::TARS_SOCKET_CONNECT_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }

        if (!$client->send($requestBuf)) {
            $client->close();
            $code = Code::TARS_SOCKET_SEND_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }
        $firstRsp = true;
        $curLen = 0;
        $responseBuf = '';
        $packLen = 0;
        $isConnected = true;
        while ($isConnected) {
            if ($client->errCode) {
                throw new \Exception('socket recv falied', $client->errCode);
            }
            $data = $client ? $client->recv() : '';
            if ($firstRsp) {
                $firstRsp = false;
                $list = unpack('Nlen', substr($data, 0, 4));
                $packLen = $list['len'];
                $responseBuf = $data;
                $curLen += strlen($data);
                if ($curLen == $packLen) {
                    $isConnected = false;
                    $client->close();
                }
            } else {
                if ($curLen < $packLen) {
                    $responseBuf .= $data;
                    $curLen += strlen($data);
                    if ($curLen == $packLen) {
                        $isConnected = false;
                        $client->close();
                    }
                } else {
                    $isConnected = false;
                    $client->close();
                }
            }
        }

        //读取最多32M的数据
        //$responseBuf = $client->recv();

        if (empty($responseBuf)) {
            $client->close();
            // 已经断开连接
            $code = Code::TARS_SOCKET_RECEIVE_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }

        return $responseBuf;
    }

    private function swooleCoroutineUdp($sIp, $iPort, $requestBuf, $timeout = 2)
    {
        $client = new \Swoole\Coroutine\Client(SWOOLE_SOCK_UDP);

        $client->set(array(
            'open_length_check' => 1,
            'package_length_type' => 'N',
            'package_length_offset' => 0,       //第N个字节是包长度的值
            'package_body_offset' => 0,       //第几个字节开始计算长度
            'package_max_length' => 2000000,  //协议最大长度
        ));

        if (!$client->connect($sIp, $iPort, $timeout)) {
            $code = Code::TARS_SOCKET_CONNECT_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }

        if (!$client->send($requestBuf)) {
            $client->close();
            $code = Code::TARS_SOCKET_SEND_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }

        //读取最多32M的数据
        $responseBuf = $client->recv();

        if (empty($responseBuf)) {
            $client->close();
            // 已经断开连接
            $code = Code::TARS_SOCKET_RECEIVE_FAILED;
            throw new \Exception(Code::getErrMsg($code), $code);
        }

        return $responseBuf;
    }

    private function militime()
    {
        list($msec, $sec) = explode(' ', microtime());
        $miliseconds = (float) sprintf('%.0f', (floatval($msec) + floatval($sec)) * 1000);

        return $miliseconds;
    }
}
