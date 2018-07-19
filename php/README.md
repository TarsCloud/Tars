# TARS-PHP 

TARS-PHP是针对php使用tars二进制协议，以及tars平台整体运维、RPC等一系列能力的解决方案。它主要由如下的几个部分组成：

如果你想要快速的体验tars-server,请进入examples目录,里面有详尽的三种服务的部署guideline和开发guideline
* [tcp server](https://github.com/Tencent/Tars/blob/master/php/examples/tars-tcp-server/README.md)
* [http server](https://github.com/Tencent/Tars/blob/master/php/examples/tars-http-server/README.md)
* [timer server ](https://github.com/Tencent/Tars/blob/master/php/examples/tars-timer-server/README.md)

## tars-client

tars-client中提供了对tars服务进行调用的php能力，包括：

* 调用远程服务的实例；
* 主调上报
* 自动寻址模块

[详细说明](https://github.com/Tencent/Tars/blob/master/php/tars-client/README.md)

## tars-server
tars-server提供了底层的server框架,同时支持如下特性

* 基于swoole1.x/2.x的高性能服务
* 支持tup协议和tars流两种协议模式
* 支持http、tcp、timer三种server
* 上报、监控、日志的集成
* tars平台发布支持

[详细说明](https://github.com/Tencent/Tars/blob/master/php/tars-server/README.md)

## tars-config 

从tars平台的配置服务拉取配置文件的能力模块。

[详细说明](https://github.com/Tencent/Tars/blob/master/php/tars-config/README.md)

## tars-deploy 

进行tars-server业务代码打包的模块。

[详细说明](https://github.com/Tencent/Tars/blob/master/php/tars-deploy/README.md)

## tars-extension

tars底层依赖的php扩展代码
[详细说明](https://github.com/Tencent/Tars/blob/master/php/tars-extension/README.md)
 
## tars-log

tars进行远程日志写入的模块
[详细说明](https://github.com/Tencent/Tars/blob/master/php/tars-log/README.md)
 
## tars-monitor

tars进行主调上报和特性上报的功能模块
[详细说明](https://github.com/Tencent/Tars/blob/master/php/tars-monitor/README.md)
 
## tars-registry
 
tars进行主控寻址的功能模块
[详细说明](https://github.com/Tencent/Tars/blob/master/php/tars-registry/README.md)
 
## tars-report
 
tars进行keep-alive服务保活上报的模块
[详细说明](https://github.com/Tencent/Tars/blob/master/php/tars-report/README.md)

## tars-utils
 
tars进行配置文件解析的模块
[详细说明](https://github.com/Tencent/Tars/blob/master/php/tars-utils/README.md)

## examples

三个实际的tcp、http、timer的server实例和说明:
* [tcp-server](https://github.com/Tencent/Tars/tree/master/php/examples/tars-tcp-server)
* [http-server](https://github.com/Tencent/Tars/tree/master/php/examples/tars-http-server)
* [timer-server](https://github.com/Tencent/Tars/tree/master/php/examples/tars-timer-server) 

## tars2php 

代码自动生成的工具,可以自动生成server和client端的代码。
[详细说明](https://github.com/Tencent/Tars/blob/master/php/tars2php/README.md)
 
## tars平台针对php修改说明

与默认模板tars.default相比，php新增了tars.tarsphp.default, 主要添加了如下部分:

```
    <tars>
        ...
        <application>
            <server>
                ...
                php=/data/env/runtime/php-7.1.7/bin/php
                buffer_output_size=12582912
                open_tcp_nodelay=1
                open_eof_check=0
                open_eof_split=0
                task_worker_num=1
                dispatch_mode=2
                daemonize=1
                ...
            </server>
        </application>
        ...
    </tars>
```
* 支持自定义php执行文件（/tars/application/server< php >），默认/usr/bin/php。
* 支持自定义tars_php框架启动文件配置（/tars/application/server< entrance >），默认项目目录下的src/index.php。
* tars平台生成的启动脚本会以entrance为入口文件来执行启停脚本。

在tars-server模块中,三个不同的http、timer、tcp server都需要使用不同的模块,请务必进行新的模板的添加和修改工作!!

    
