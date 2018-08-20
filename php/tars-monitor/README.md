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

#### 定时上报(默认)
> 使用定时上报需要 swoole_table支持，通过调用addStat将上报信息暂存，tars-server的task进程会搜集一段时间内（上报时间间隔由服务器下发，一般为60s）统一打包上报，可以减少上报请求
```php
$locator = "tars.tarsregistry.QueryObj@tcp -h 172.16.0.161 -p 17890";
$socketMode = 1;
$statfWrapper  = new \Tars\monitor\StatFWrapper($locator,$socketMode);  
$statfWrapper->addStat("PHPTest.helloTars.obj","test","172.16.0.116",51003,200,0,0);  
$statfWrapper->addStat("PHPTest.helloTars.obj","test","172.16.0.116",51003,200,0,0);  
$statfWrapper->addStat("PHPTest.helloTars.obj","test","172.16.0.116",51003,200,0,0);
```

上报数据可使用多种存储方式，cache中提供了`swoole_table`与`redis`的实现方式，用户也可以自己实现`contract/StoreCacheInterface`，配置存储方式参考demo `tars-http-server` 中`src/services.php` 的配置。
```php
return array(
    'namespaceName' => 'HttpServer\\',
    'monitorStoreConf' => [
        'className' => Tars\monitor\cache\SwooleTableStoreCache::class,
        'config' => []
    ]
);
```
`monitorStoreConf` 为存储方式的配置，其中`className`为实现类，`config` 为对应的配置，如使用redis存储方式时，config中需要配置redis的host、port、以及key的前缀等。
未配置`monitorStoreConf`时默认使用`SwooleTableStoreCache`进行存储。


#### 单次上报
> 同时`tars-monitor`也提供了单次上报的接口`monitorStat`，即每次tars请求调用均会进行一次上报，该方式不建议使用
```php
$locator = "tars.tarsregistry.QueryObj@tcp -h 172.16.0.161 -p 17890";
$socketMode = 1;
$statfWrapper  = new \Tars\monitor\StatFWrapper($locator,$socketMode);  
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