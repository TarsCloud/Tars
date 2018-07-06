<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/4/27
 * Time: 下午3:51.
 */
require_once '../vendor/autoload.php';
$config = new \Tars\client\CommunicatorConfig();
$config->setLocator('tars.tarsregistry.QueryObj@tcp -h 172.16.0.161 -p 17890');
$config->setModuleName('tedtest');
$config->setCharsetName('UTF-8');

$conigServant = new \Tars\config\ConfigServant($config);
$result = $conigServant->loadConfig('PHPTest', 'helloTars', 'hhh.txt', $configtext);
var_dump($configtext);

$config->setSocketMode(2);
$conigServant = new \Tars\config\ConfigServant($config);
$result = $conigServant->loadConfig('PHPTest', 'helloTars', 'hhh.txt', $configtext);
var_dump($configtext);
