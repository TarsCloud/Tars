<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/4/27
 * Time: 下午2:50.
 */
require_once '../vendor/autoload.php';

$wrapper = new \Tars\registry\QueryFWrapper('tars.tarsregistry.QueryObj@tcp -h 172.16.0.161 -p 17890', 1, 60000);
$result = $wrapper->findObjectById('PHPTest.PHPServer.obj');
var_dump($result);

$wrapper = new \Tars\registry\QueryFWrapper('tars.tarsregistry.QueryObj@tcp -h 172.16.0.161 -p 17890', 2, 60000);
$result = $wrapper->findObjectById('PHPTest.PHPServer.obj');
var_dump($result);

\Tars\registry\RouteTable::getInstance();
$result = \Tars\registry\RouteTable::getRouteInfo('PHPTest.PHPServer.obj');
echo "result:\n";
var_dump($result);

/*
require_once '../src/QueryF.php';
require_once '../src/EndpointF.php';
// socket同步版本
$inst = \Tars\registry\QueryF::getInstance();
$inst->init("10.121.107.80",17890,"taf.tafregistry.QueryObj",false);
$result = $inst->findObjectById("QDPHP.TARSServer.obj");
var_dump($result);


// 默认为swoole同步版本
$inst = \Tars\registry\QueryF::getInstance();
$inst->init("10.121.107.80",17890,"taf.tafregistry.QueryObj");

$result = $inst->findObjectById("QDPHP.TARSServer.obj");
var_dump($result);

// swoole2.0协程版本
$inst = \Tars\registry\QueryF::getInstance();

$inst->init("10.121.107.80",17890,"taf.tafregistry.QueryObj",true,true);

$result = $inst->findObjectById("QDPHP.TARSServer.obj");
var_dump($result);
*/
