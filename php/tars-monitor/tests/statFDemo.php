<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/4/27
 * Time: 下午6:11.
 */

// 需要增加对网络的jianrong

require_once '../vendor/autoload.php';

$statfWrapper = new \Tars\monitor\StatFWrapper('tars.tarsregistry.QueryObj@tcp -h 172.16.0.161 -p 17890', 1);
$statfWrapper->monitorStat('PHPTest.helloTars.obj', 'test', '172.16.0.116', 51003, 200, 0, 0);
$statfWrapper->monitorStat('PHPTest.helloTars.obj', 'test', '172.16.0.116', 51003, 200, 0, 0);
$statfWrapper->monitorStat('PHPTest.helloTars.obj', 'test', '172.16.0.116', 51003, 200, 0, 0);

$statfWrapper = new \Tars\monitor\StatFWrapper('tars.tarsregistry.QueryObj@tcp -h 172.16.0.161 -p 17890', 2);
$statfWrapper->monitorStat('PHPTest.helloTars.obj', '11111', '172.16.0.116', 51003, 200, 2, 2);
$statfWrapper->monitorStat('PHPTest.helloTars.obj', '11111', '172.16.0.116', 51003, 200, 2, 2);
$statfWrapper->monitorStat('PHPTest.helloTars.obj', '11111', '172.16.0.116', 51003, 200, 2, 2);

/*
$inst = \Tars\monitor\StatF::getInstance();
$inst->init('172.16.0.161',10003,"tars.tarsstat.StatObj",'phpClient',60000,false);

// 进行一条一条的上报
$inst->monitorStat("PHPTest.helloTars.obj","test","172.16.0.116",51003,200,0,0);
$inst->monitorStat("PHPTest.helloTars.obj","test","172.16.0.116",51003,200,1,0);
$inst->monitorStat("PHPTest.helloTars.obj","test","172.16.0.116",51003,200,2,2);



// 使用swoole table
$inst = \Tars\monitor\StatF::getInstance();
$inst->init('172.16.0.161',10003,"tars.tarsstat.StatObj",'phpClient',1,true);

$inst->monitorStat("PHPTest.helloTars.obj","test","172.16.0.116",51003,200,0,0);
$inst->monitorStat("PHPTest.helloTars.obj","test","172.16.0.116",51003,200,1,0);
$inst->monitorStat("PHPTest.helloTars.obj","test","172.16.0.116",51003,200,2,2);
*/
