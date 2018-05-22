<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/4/27
 * Time: 下午3:18.
 */
require_once '../vendor/autoload.php';
$config = new \Tars\client\CommunicatorConfig();
$config->setLocator('tars.tarsregistry.QueryObj@tcp -h 172.16.0.161 -p 17890');
$config->setModuleName('tedtest');
$config->setCharsetName('UTF-8');

$logServant = new \Tars\log\LogServant($config);
$result = $logServant->logger('PHPTest', 'helloTars', 'ted.log', '%Y%m%d', ['hahahahaha']);

$config->setSocketMode(2);
$logServant = new \Tars\log\LogServant($config);
$result = $logServant->logger('PHPTest', 'helloTars', 'ted2.log', '%Y%m%d', ['huohuohuo']);
