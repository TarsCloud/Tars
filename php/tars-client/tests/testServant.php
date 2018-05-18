<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2017/6/16
 * Time: 下午2:38
 */



require_once("./vendor/autoload.php");

// 指定主控ip
$config = new \Tars\client\CommunicatorConfig();
$config->setLocator("tars.tarsregistry.QueryObj@tcp -h 172.16.0.161 -p 17890");
$config->setModuleName("App.Server");
$config->setCharsetName("UTF-8");

$servant = new \App\Server\Servant\TestTafServiceServant($config);

// 直接指定服务ip
$route['sIp'] = "127.0.0.1";
$route['iPort'] = 8080;
$routeInfo[] = $route;
$config = new \Tars\client\CommunicatorConfig();
$config->setRouteInfo($routeInfo);
$config->setSocketMode(2);//1标识socket 2标识swoole同步 3标识swoole协程
$config->setModuleName("App.Server");
$config->setCharsetName("UTF-8");

$servant = new \App\Server\Servant\TestTafServiceServant($config);

$name = "ted";
$intVal = $servant->sayHelloWorld($name,$greetings);




