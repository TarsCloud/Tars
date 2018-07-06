<?php

namespace Tars\monitor;

use Tars\monitor\classes\StatPropInfo;
use Tars\monitor\classes\StatPropMsgBody;
use Tars\monitor\classes\StatPropMsgHead;
use Tars\Utils;

class PropertyFWrapper
{
    protected $_propertyF;
    protected $_routeInfo;
    protected $_moduleName;

    const SWOOLE_STAT_SUCCESS = 0;
    const SWOOLE_STAT_TIMEOUT = 1;
    const SWOOLE_STAT_EXCEPTION = 2;

    protected $_masterName = '';

    public function __construct($locator, $socketMode, $moduleName = 'php',
                                $propertyName = 'tars.tarsproperty.PropertyObj')
    {
        $result = Utils::getLocatorInfo($locator);
        if (empty($result) || !isset($result['locatorName'])
            || !isset($result['routeInfo']) || empty($result['routeInfo'])) {
            throw new \Exception('Route Fail', -100);
        }

        $this->_moduleName = $moduleName;
        $this->_propertyF = new PropertyFServant($locator, $socketMode, $propertyName);
    }

    public function monitorProperty($ip, $propertyName, $policy, $value, $moduleName = '',
                                    $setName = '', $setArea = '', $setID = '',
                                    $sContainer = '', $iPropertyVer = 1)
    {
        $msgHead = new StatPropMsgHead();
        $msgHead->moduleName = empty($moduleName) ? $this->_moduleName : $moduleName;
        $msgHead->ip = empty($ip) ? '127.0.0.1' : $ip;
        $msgHead->propertyName = $propertyName;
        if (!empty($setName)) {
            $msgHead->setName = $setName;
        }
        if (!empty($setArea)) {
            $msgHead->setArea = $setArea;
        }
        if (!empty($setID)) {
            $msgHead->setID = $setID;
        }
        if (!empty($sContainer)) {
            $msgHead->sContainer = $sContainer;
        }
        $msgHead->iPropertyVer = $iPropertyVer;

        $msgBody = new StatPropMsgBody();

        $propInfo = new StatPropInfo();
        $propInfo->policy = $policy;
        $propInfo->value = $value;

        $msgBody->vInfo->pushBack($propInfo);
        $msg[] = ['key' => $msgHead, 'value' => $msgBody];

        $this->reportPropMsg($msg);
    }

    /**
     * @param $moduleName
     * @param $ip
     * @param $propertyName
     * @param $policies
     * @param $values
     * @param string $setName
     * @param string $setArea
     * @param string $setID
     * @param string $sContainer
     * @param int    $iPropertyVer
     *
     * @return bool
     */
    public function reportPropMsg($statmsg)
    {
        try {
            $this->_propertyF->reportPropMsg($statmsg);
        } catch (\Exception $e) {
            throw $e;
        }
    }
}
