
# tars-config使用说明

## 简介
tars-config用来获取从tars管理平台下发的配置。通常使用在下发环境对应的业务配置，例如mysql，redis地址，端口等信息

## 使用说明
```
<?php

	require_once "../vendor/autoload.php";
	$config = new \Tars\client\CommunicatorConfig();
	$config->setLocator("tars.tarsregistry.QueryObj@tcp -h 172.16.0.161 -p 17890"); //这里配置的是tars主控地址
	$config->setModuleName("tedtest"); //主调名字用于显示再主调上报中。
	$config->setCharsetName("UTF-8"); //字符集

	$conigServant = new \Tars\config\ConfigServant($config); 
	$result = $conigServant->loadConfig("PHPTest",'helloTars','hhh.txt',$configtext); //参数分别为 appName(servant name 第一部分)，server name(servant name第二部分)，文件名，最后一个是引用传参，是输出的配置文件内容。
	var_dump($configtext);

	$config->setSocketMode(2); //设置socket model为2 swoole tcp client，1为socket，3为swoole 协程 client
	$conigServant = new \Tars\config\ConfigServant($config);
	$result = $conigServant->loadConfig("PHPTest",'helloTars','hhh.txt',$configtext);
	var_dump($configtext);
```