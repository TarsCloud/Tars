<?php

namespace Tars\monitor;

use Tars\monitor\classes\StatMicMsgBody;
use Tars\monitor\classes\StatMicMsgHead;
use Tars\Utils;

class StatFWrapper
{
    protected $_statF;
    protected $_routeInfo;

    const SWOOLE_STAT_SUCCESS = 0;
    const SWOOLE_STAT_TIMEOUT = 1;
    const SWOOLE_STAT_EXCEPTION = 2;

    protected $_masterName = '';
    protected $_reportInterval;

    public function __construct($locator, $socketMode, $statServantName,
                                $masterName = 'phpClient', $reportInterval = 60000)
    {
        $result = Utils::getLocatorInfo($locator);
        if (empty($result) || !isset($result['locatorName'])
            || !isset($result['routeInfo']) || empty($result['routeInfo'])) {
            throw new \Exception('Route Fail', -100);
        }

        $this->_masterName = $masterName;
        $this->_reportInterval = $reportInterval;
        $this->_statF = new StatFServant($locator, $socketMode, $statServantName);
    }

    /**
     * @param $slaveName
     * @param $interfaceName
     * @param $slaveIp
     * @param $slavePort
     * @param $rspTime
     * @param int    $returnValue
     * @param int    $status
     * @param string $masterName
     * @param string $masterIp
     * @param string $tarsVersion
     * @param string $slaveSetName
     * @param string $slaveSetID
     * @param string $sMasterSetInfo
     * @param string $sSlaveContainer
     * @param string $sMasterContainer
     * @param string $iStatVer
     */
    public function addStat($slaveName, $interfaceName, $slaveIp, $slavePort,
     $rspTime, $returnValue = 0,
     $status = self::SWOOLE_STAT_SUCCESS,
     $masterName = '', $masterIp = '', $tarsVersion = '',
     $slaveSetName = '', $slaveSetID = '',
     $sMasterSetInfo = '', $sSlaveContainer = '',
     $sMasterContainer = '', $iStatVer = '')
    {
    }

    /**
     * 单次上报，来一次报一次
     *
     * @param $slaveName
     * @param $interfaceName
     * @param $slaveIp
     * @param $slavePort
     * @param $rspTime
     * @param int    $returnValue
     * @param int    $status
     * @param string $masterName
     * @param string $masterIp
     * @param string $tarsVersion
     * @param string $slaveSetName
     * @param string $slaveSetID
     * @param string $sMasterSetInfo
     * @param string $sSlaveContainer
     * @param string $sMasterContainer
     * @param string $iStatVer
     *
     * @return bool
     */
    public function monitorStat($slaveName, $interfaceName, $slaveIp, $slavePort,
     $rspTime, $returnValue = 0,
     $status = self::SWOOLE_STAT_SUCCESS,
     $masterName = '', $masterIp = '', $tarsVersion = '',
     $slaveSetName = '', $slaveSetID = '',
     $sMasterSetInfo = '', $sSlaveContainer = '',
     $sMasterContainer = '', $iStatVer = '')
    {
        try {
            $msgHead = new StatMicMsgHead();
            $msgHead->masterName = $masterName ? $masterName : $this->_masterName;
            $msgHead->slaveName = $slaveName;
            $msgHead->interfaceName = $interfaceName;
            $msgHead->masterIp = $masterIp ? $masterIp : '127.0.0.1';
            $msgHead->slaveIp = $slaveIp;
            $msgHead->slavePort = $slavePort;
            $msgHead->returnValue = $returnValue;
            $msgHead->slaveSetName = $slaveSetName;
            $msgHead->slaveSetID = $slaveSetID;
            $msgHead->tarsVersion = $tarsVersion;
            $msgHead->sMasterSetInfo = $sMasterSetInfo;
            $msgHead->sSlaveContainer = $sSlaveContainer;
            $msgHead->sMasterContainer = $sMasterContainer;
            $msgHead->iStatVer = $iStatVer;

            $msgBody = new StatMicMsgBody();
            $successCount = $timeoutCount = $execCount = 0;
            if ($status == self::SWOOLE_STAT_SUCCESS) {
                $successCount = 1;
            } elseif ($status == self::SWOOLE_STAT_TIMEOUT) {
                $timeoutCount = 1;
            } else {
                $execCount = 1;
            }
            $msgBody->count = $successCount;
            $msgBody->timeoutCount = $timeoutCount;
            $msgBody->execCount = $execCount;
            $msgBody->totalRspTime = $rspTime;
            $msgBody->maxRspTime = $rspTime;
            $msgBody->minRspTime = $rspTime;

            $msg[] = ['key' => $msgHead, 'value' => $msgBody];
            $this->reportMicMsg($msg, true);

            return true;
        } catch (\Exception $e) {
            error_log('monitorStat failed');
            throw $e;
        }
    }

    public function reportMicMsg($msg, $bFromClient = true)
    {
        try {
            $this->_statF->reportMicMsg($msg, $bFromClient);
        } catch (\Exception $e) {
            throw $e;
        }
    }
}
