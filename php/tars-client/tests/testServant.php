<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2017/6/16
 * Time: 下午2:38.
 */
require_once './vendor/autoload.php';

// 指定主控ip
$config = new \Tars\client\CommunicatorConfig();
$config->setLocator('tars.tarsregistry.QueryObj@tcp -h 172.16.0.161 -p 17890');
$config->setModuleName('App.Server');
$config->setCharsetName('UTF-8');
$servant = new \PHPTest\PHPServer\obj\TestTafServiceServant($config);

echo "Locator specified with default socketmode 1\n";
$name = 'ted';
$intVal = $servant->sayHelloWorld($name, $greetings);
var_dump($greetings);

// 直接指定服务ip
$route['sIp'] = '172.16.0.161';
$route['iPort'] = 28888;
$routeInfo[] = $route;
$config = new \Tars\client\CommunicatorConfig();
$config->setRouteInfo($routeInfo);
$config->setSocketMode(2); //1标识socket 2标识swoole同步 3标识swoole协程
$config->setModuleName('App.Server');
$config->setCharsetName('UTF-8');

$servant = new \PHPTest\PHPServer\obj\TestTafServiceServant($config);

echo "Service ip and port specified with socket mode 2 (swoole client)\n";

$name = 'ted';
$intVal = $servant->sayHelloWorld($name, $greetings);
var_dump($greetings);
