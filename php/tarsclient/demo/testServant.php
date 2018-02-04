<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2017/6/16
 * Time: 下午2:38
 */



require_once("./vendor/autoload.php");

// 指定主控ip
$_SERVER['LOCATOR_IP'] = "127.0.0.1";
$_SERVER['LOCATOR_PORT'] = 17890;
$servant = new \App\Server\Servant\servant();

// 直接指定服务ip
$ip = "127.0.0.1";
$port = 8080;


$servant = new \App\Server\Servant\servant();

$in1 = "test";

$ss1 = new SimpleStruct();
$ss1->id = 1;
$ss1->count = 2;
$ss1->page = 3;


$intVal = $servant->singleParam($in1,$out1);




