<?php

namespace Tars\monitor;

use Tars\monitor\classes\StatMicMsgBody;
use Tars\monitor\classes\StatMicMsgHead;
use Tars\monitor\contract\StoreCacheInterface;
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
    protected $_cache;
    /** @var  StoreCacheInterface */
    protected static $cacheInstance = null;


    public function __construct(
        $locator,
        $socketMode,
        $statServantName,
        $masterName = 'phpClient',
        $reportInterval = 60000
    ) {
        $result = Utils::getLocatorInfo($locator);
        if (empty($result) || !isset($result['locatorName'])
            || !isset($result['routeInfo']) || empty($result['routeInfo'])
        ) {
            throw new \Exception("Route Fail", -100);
        }

        $this->_masterName = $masterName;
        $this->_reportInterval = $reportInterval;
        $this->_statF = new StatFServant($locator, $socketMode, $statServantName);
        $this->_cache = self::$cacheInstance;
    }

    public static function initStoreCache(StoreCacheInterface $storeCache)
    {
        self::$cacheInstance = $storeCache;
    }

    public static function getStoreCache()
    {
        return self::$cacheInstance;
    }

    /**
     * @param string $slaveName
     * @param string $interfaceName
     * @param string $slaveIp
     * @param int $slavePort
     * @param float $rspTime
     * @param int $returnValue
     * @param int $status
     * @param string $masterName
     * @param string $masterIp
     * @param string $tarsVersion
     * @param string $slaveSetName
     * @param string $slaveSetArea
     * @param string $slaveSetID
     */
    public function addStat(
        $slaveName,
        $interfaceName,
        $slaveIp,
        $slavePort,
        $rspTime,
        $returnValue = 0,
        $status = self::SWOOLE_STAT_SUCCESS,
        $masterName = '',
        $masterIp = '',
        $tarsVersion = '',
        $slaveSetName = '',
        $slaveSetArea = '',
        $slaveSetID = ''
    ) {
        $table = $this->_cache;
        if (!$table instanceof StoreCacheInterface) {
            return $this->monitorStat(...func_get_args());
        }
        $timeSlice = $this->getTimeSlice($this->_reportInterval);
        $headInfo = compact('masterName', 'slaveName', 'interfaceName', 'masterIp', 'slaveIp', 'slavePort',
            'returnValue', 'slaveSetName', 'slaveSetArea', 'slaveSetID', 'tarsVersion', 'timeSlice');

        $key = implode('|', $headInfo);

        if ($status == self::SWOOLE_STAT_SUCCESS) {
            $table->incrField($key, 'count', 1);
        } else {
            if ($status == self::SWOOLE_STAT_TIMEOUT) {
                $table->incrField($key, 'timeoutCount', 1);
            } else {
                $table->incrField($key, 'execCount', 1);
            }
        }
        $table->incrField($key, 'totalRspTime', $rspTime);
        $stat = $table->get($key);
        if ($rspTime >= $stat['maxRspTime']) {
            $table->set($key, ['maxRspTime' => $rspTime]);
        }
        if ($rspTime <= $stat['minRspTime']) {
            $table->set($key, ['minRspTime' => $rspTime]);
        }
    }

    /**
     * 单次上报，来一次报一次
     *
     * @param $slaveName
     * @param $interfaceName
     * @param $slaveIp
     * @param $slavePort
     * @param $rspTime
     * @param int $returnValue
     * @param int $status
     * @param string $masterName
     * @param string $masterIp
     * @param string $tarsVersion
     * @param string $slaveSetName
     * @param string $slaveSetArea
     * @param string $slaveSetID
     * @return bool
     * @throws \Exception
     */
    public function monitorStat(
        $slaveName,
        $interfaceName,
        $slaveIp,
        $slavePort,
        $rspTime,
        $returnValue = 0,
        $status = self::SWOOLE_STAT_SUCCESS,
        $masterName = '',
        $masterIp = '',
        $tarsVersion = '',
        $slaveSetName = '',
        $slaveSetArea = '',
        $slaveSetID = ''
    ) {
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
            $msgHead->slaveSetArea = $slaveSetArea;
            $msgHead->slaveSetID = $slaveSetID;
            $msgHead->tarsVersion = $tarsVersion;

            $msgBody = new StatMicMsgBody();
            $successCount = $timeoutCount = $execCount = 0;
            if ($status == self::SWOOLE_STAT_SUCCESS) {
                $successCount = 1;
            } else {
                if ($status == self::SWOOLE_STAT_TIMEOUT) {
                    $timeoutCount = 1;
                } else {
                    $execCount = 1;
                }
            }
            $msgBody->count = $successCount;
            $msgBody->timeoutCount = $timeoutCount;
            $msgBody->execCount = $execCount;
            $msgBody->totalRspTime = $rspTime;
            $msgBody->maxRspTime = $rspTime;
            $msgBody->minRspTime = $rspTime;

            $msg[] = ["key" => $msgHead, "value" => $msgBody];
            $this->reportMicMsg($msg, true);
            return true;
        } catch (\Exception $e) {
            error_log("monitorStat failed");
            throw $e;
        }

    }

    public function sendStat()
    {
        $msg = [];
        $table = $this->_cache;
        $currentSlice = $this->getTimeSlice($this->_reportInterval);

        foreach ($table->getAll() as $key => $value) {

            list($masterName, $slaveName, $interfaceName, $masterIp, $slaveIp, $slavePort, $returnValue, $slaveSetName, $slaveSetArea, $slaveSetID, $tarsVersion, $timeSlice) = explode('|',
                $key);

            if ($timeSlice < $currentSlice) {
                $msgHead = new StatMicMsgHead();
                $msgHead->masterName = $masterName ? $masterName : $this->_masterName;
                $msgHead->slaveName = $slaveName;
                $msgHead->interfaceName = $interfaceName;
                $msgHead->masterIp = $masterIp ? $masterIp : '127.0.0.1';
                $msgHead->slaveIp = $slaveIp;
                $msgHead->slavePort = $slavePort;
                $msgHead->returnValue = $returnValue;
                $msgHead->slaveSetName = $slaveSetName;
                $msgHead->slaveSetArea = $slaveSetArea;
                $msgHead->slaveSetID = $slaveSetID;
                $msgHead->tarsVersion = $tarsVersion;

                $msgBody = new StatMicMsgBody();
                $msgBody->count = isset($value['count']) ? $value['count'] : 0;
                $msgBody->timeoutCount = isset($value['timeoutCount']) ? $value['timeoutCount'] : 0;
                $msgBody->execCount = isset($value['execCount']) ? $value['execCount'] : 0;
                $msgBody->totalRspTime = $value['totalRspTime'];
                $msgBody->maxRspTime = $value['maxRspTime'];
                $msgBody->minRspTime = $value['minRspTime'];

                $msg[] = ["key" => $msgHead, "value" => $msgBody];
                $table->del($key);
            }
        }

        if (count($msg) > 0) {
            $this->reportMicMsg($msg, true);
        }
        return true;
    }

    public function reportMicMsg($msg, $bFromClient = true)
    {
        try {
            $this->_statF->reportMicMsg($msg, $bFromClient);
        } catch (\Exception $e) {
            throw $e;
        }
    }

    /**
     * 获取是当天多少个$interval 秒（单位s）
     *
     * @param $interval
     * @return int
     */
    public function getTimeSlice($interval)
    {
        $time = time();
        return (int)($time / ($interval/1000));
    }
}

