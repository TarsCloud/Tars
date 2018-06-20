## 主控寻址的模块

## 1、模块说明
```
	tars平台的tarsregistry服务提供服务发现的功能。
        本模块为php提供主控寻址的能力(服务发现)。
```

## 2、文件说明：
```
├── composer.json
├── src
│   ├── client  //请求主控服务的client代码
│   │   ├── Code.php
│   │   ├── CodeRegistry.php
│   │   ├── CommunicatorConfig.php
│   │   ├── CommunicatorFactory.php
│   │   ├── Communicator.php
│   │   ├── CommunicatorRegistry.php
│   │   ├── Consts.php
│   │   ├── RequestPacket.php
│   │   ├── RequestPacketRegistry.php
│   │   ├── ResponsePacket.php
│   │   ├── ResponsePacketRegistry.php
│   │   ├── TUPAPIWrapper.php
│   │   └── TUPAPIWrapperRegistry.php
│   ├── EndpointF.php       //struct EndpointF 的php类
│   ├── QueryFServant.php   //直接请求主控服务
│   ├── QueryFWrapper.php   //优先从内存寻找服务地址，其次从主控寻址
│   ├── RouteTable.php      //在swoole table里保存服务地址
│   └── tars   //协议文件             
│       ├── EndpointF.tars 
│       └── QueryF.tars 
└── tests
    └── demo.php

```

## 3、使用示例：
```
        //从tarsregistry服务寻找服务地址
        $wrapper = new \Tars\registry\QueryFWrapper("tars.tarsregistry.QueryObj@tcp -h 172.16.0.161 -p 17890",1,60000);
        $result = $wrapper->findObjectById("PHPTest.PHPServer.obj");
        var_dump($result);

        //优先从内存寻找服务地址，其次从主控寻址
        \Tars\registry\RouteTable::getInstance();
        $result = \Tars\registry\RouteTable::getRouteInfo("PHPTest.PHPServer.obj");
        echo "result:\n";
        var_dump($result);
```