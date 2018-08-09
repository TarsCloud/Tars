# tars-client

## 简介
主要包含主控寻址网络收发能力、上报能力

tests目录提供了一个tars服务的测试用例。服务名为 **App.Server.Servant**

## 使用说明
参照提供的测试用例。  
tars-client端调用tars服务，在实例化时需要传入 **\Tars\client\CommunicatorConfig** 的实例，设定必要的配置信息。主要包含以下内容  
* 服务地址
    * 主控寻址
        * 有多台机器提供服务时，可以通过主控自动寻址方式发现服务
    * 指定服务地址
        * 灰度或者需要从特定地址获取服务时，采用此方式
* 网络传输
    * 包含三种模式：socket、swoole sync、swoole coroutine
* 上报名
    * 指定上报模块名称。主控寻址默认为 **tarsproxy** ，指定服务地址时可以根据业务自行填写
* 编码格式

### 服务寻址方式
结合测试用例testServant.php介绍不同服务寻址的代码规范。  
通过 **\Tars\client\CommunicatorConfig** 类设定相关配置。测试用例给出了两种寻址方式的示例代码  
1、主控寻址  
一旦指定locator，tars可以根据服务名自动抓取服务地址。locator配置的格式如下  
```php
$config = new \Tars\client\CommunicatorConfig();
$config->setLocator("tars.tarsregistry.QueryObj@tcp -h 172.16.0.161 -p 17890");
```
tars主控本身也是一个tars服务，服务名为 **tars.tarsregistry.QueryObj** ，传输协议为tcp，服务地址为 172.16.0.161，端口为 17890 。实际开发中请 **根据服务实际情况填写**  

上述主控服务确定后，可以根据需要指定上报的模块名及编码格式。默认的上报模块名为 **tarsproxy** ，为了便于跟踪业务，建议重新指定上报模块名
```
$config->setModuleName("App.Server");
$config->setCharsetName("UTF-8");
```

2、指定ip
指定服务方的地址。这种方式需要指定服务的ip、port，代码如下。
```php
$route['sIp'] = "127.0.0.1";
$route['iPort'] = 8080;
$routeInfo[] = $route;
$config = new \Tars\client\CommunicatorConfig();
$config->setRouteInfo($routeInfo);
```
其他用法同自动寻址相同
