# tars-monitor
------------------

`tars-monitor` 是 `phptars` 服务与特性监控上报模块

它由两个子模块组成：
* 服务监控
* 特性监控

## 使用方式

### 安装

使用composer进行安装
`composer install phptars/tars-monitor`

### 调用

#### 服务监控上报

* locator  为上报地址，一般由服务器下发
* socketMode 设置为 1 使用 socket 方式进行上报
   socketMode 设置为 2 使用 swoole tcp client方式进行上报（需要swoole支持）
   socketMode 设置为 3 使用 swoole tcp coroutine client协程方式进行上报（需要swoole2.0以上支持）
* reportInterval 为 定时上报间隔
* enableTimer 为 是否开启定时上报

> 启用定时上报需要 swoole_table支持，开启定时上报后，调用addStat上报信息会暂存到swoole_table中，服务会搜集一段时间内统一打包上报，可以减少上报请求
```php
$locator = "tars.tarsregistry.QueryObj@tcp -h 172.16.0.161 -p 17890";
$socketMode = 1;
$statfWrapper  = new \Tars\monitor\StatFWrapper($locator,$socketMode);  
$statfWrapper->monitorStat("PHPTest.helloTars.obj","test","172.16.0.116",51003,200,0,0);  
$statfWrapper->monitorStat("PHPTest.helloTars.obj","test","172.16.0.116",51003,200,0,0);  
$statfWrapper->monitorStat("PHPTest.helloTars.obj","test","172.16.0.116",51003,200,0,0);
```

#### 特性监控

参数与服务监控类似

```php
$statfWrapper  = new \Tars\monitor\PropertyFWrapper("tars.tarsregistry.QueryObj@tcp -h 172.16.0.161 -p 17890",1);  
$statfWrapper->monitorProperty("127.0.0.1","userdefined",'Sum',2);  
$statfWrapper->monitorProperty("127.0.0.1","userdefined",'Count',2);  
$statfWrapper->monitorProperty("127.0.0.1","userdefined",'Count',1);
```

### 监控查看
数据上报后，用户可在服务监控/特性监控选项卡中查看上报的数据。

## 其他
因为其他模块已经集成了本模块， __所以一般情况下，服务脚本无需显式使用此模块。__