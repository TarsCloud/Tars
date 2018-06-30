<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/4/27
 * Time: 下午6:11.
 */

// 需要增加对网络的jianrong

require_once '../vendor/autoload.php';

$statfWrapper = new \Tars\monitor\PropertyFWrapper('tars.tarsregistry.QueryObj@tcp -h 172.16.0.161 -p 17890', 1);
$statfWrapper->monitorProperty('127.0.0.1', 'userdefined', 'Sum', 2);
$statfWrapper->monitorProperty('127.0.0.1', 'userdefined', 'Count', 2);
$statfWrapper->monitorProperty('127.0.0.1', 'userdefined', 'Count', 1);

$statfWrapper = new \Tars\monitor\PropertyFWrapper('tars.tarsregistry.QueryObj@tcp -h 172.16.0.161 -p 17890', 2, 'ted');
$statfWrapper->monitorProperty('127.0.0.1', 'userdefined', 'Sum', 2);
$statfWrapper->monitorProperty('127.0.0.1', 'userdefined', 'Count', 2);
$statfWrapper->monitorProperty('127.0.0.1', 'userdefined', 'Count', 1);

/*
require_once "../src/PropertyF.php";
require_once "../src/StatPropInfo.php";
require_once "../src/StatPropMsgBody.php";
require_once "../src/StatPropMsgHead.php";

$inst = \Tars\monitor\PropertyF::getInstance();
$inst->init('172.16.0.161',10004,"tars.tarsproperty.PropertyObj",
    'PHPTest.helloTars',60000,false);

// 进行一条一条的上报
$inst->reportPropMsg("127.0.0.1","userdefined",'Sum',2);
$inst->reportPropMsg("127.0.0.1","userdefined",'Count',2);
$inst->reportPropMsg("127.0.0.1","userdefined",'Count',1);
$inst->reportPropMsg("127.0.0.1","userdefined",'Avg',2);

$inst->reportPropMsg("127.0.0.1","userdefined",'Max',9);
$inst->reportPropMsg("127.0.0.1","userdefined",'Min',1);


// 使用swoole table
$inst = \Tars\monitor\PropertyF::getInstance();
$inst->init('172.16.0.161',10004,"tars.tarsproperty.PropertyObj",
    'PHPTest.helloTars');

$inst->reportPropMsg("127.0.0.1","userdefined",'Max',999);
*/
