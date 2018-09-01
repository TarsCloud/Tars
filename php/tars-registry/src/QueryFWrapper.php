<?php

namespace Tars\registry;

use Tars\Utils;

class QueryFWrapper
{
    protected $_queryF;
    protected $_refreshInterval;

    public function __construct($locator, $socketMode, $refreshInterval = 60000)
    {
        $result = Utils::getLocatorInfo($locator);
        if (empty($result) || !isset($result['locatorName'])
            || !isset($result['routeInfo']) || empty($result['routeInfo'])) {
            throw new \Exception('Route Fail', -100);
        }

        $locatorName = $result['locatorName'];
        $routeInfo = $result['routeInfo'];
        $this->_refreshInterval = $refreshInterval;

        $this->_queryF = new QueryFServant($routeInfo, $socketMode, $locatorName);
    }

    public function findObjectById($id)
    {
        try {
            if (class_exists('swoole_table')) {
                RouteTable::getInstance();
                $result = RouteTable::getRouteInfo($id);
                $routeInfo = $result['routeInfo'];

                if (!empty($routeInfo)) {
                    $timestamp = $result['timestamp'];
                    if (time() - $timestamp < $this->_refreshInterval / 1000) {
                        return $routeInfo;
                    }
                }
            }

            $endpoints = $this->_queryF->findObjectById($id);
            $routeInfo = [];
            foreach ($endpoints as $endpoint) {
                $route['sIp'] = $endpoint['host'];
                $route['iPort'] = $endpoint['port'];
                $route['timeout'] = $endpoint['timeout'];
                $route['bTcp'] = $endpoint['istcp'];
                $routeInfo[] = $route;
            }

            // 这里你能起一个定时器么,i think not, 但是可以起swooletable
            // 然后在server里面轮询,再去刷swooletable里面缓存的数据
            if (class_exists('swoole_table') && php_sapi_name() !== "apache" && php_sapi_name() !== "fpm-fcgi") {
                RouteTable::getInstance();
                RouteTable::setRouteInfo($id, $routeInfo);
            }

            return $routeInfo;
        } catch (\Exception $e) {
            throw $e;
        }
    }
}
