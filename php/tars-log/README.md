
# tars-log  
------------------------

`tars-log` 是 `phptars` 远程日志模块

## 安装

使用`composer`进行安装
`composer install phptars/tars-log`

## 使用

### 配置

实例化CommunicatorConfig，可以逐个参数进行配置，也可以通过平台下发的配置文件统一配置
1. 单独配置某个参数
```php
$config  = new \Tars\client\CommunicatorConfig();  
$config->setLocator("tars.tarsregistry.QueryObj@tcp -h 172.16.0.161 -p 17890");  
$config->setModuleName("tedtest");  
$config->setCharsetName("UTF-8");
$config->setLogLevel("INFO");	//日志级别：`INFO`、`DEBUG`、`WARN`、`ERROR` 默认INFO
$config->setSocketMode(2);		//远程日志连接方式：1：socket，2：swoole tcp client 3: swoole coroutine tcp client
```
2. 配置文件初始化参数
```php
$config = new \Tars\client\CommunicatorConfig();
$sFilePath = '项目地址/src/conf'; //配置文件下发路径
$config->init($sFilePath);
```

### 输出日志
调用`LogServant`的`logger`方式输出远程日志
```php
$logServant  = new \Tars\log\LogServant($config);  
$appName = "App";	//应用名称
$serverName = "server";	//服务名称
$file = "test.log";	//文件名称
$format = "%Y%m%d";	//日志时间格式
$buffer = ["hahahahaha"];	//日志内容，数组，每个元素为一条日志
$result = $logServant->logger($appName,$serverName,$file,$format,$buffer);
```