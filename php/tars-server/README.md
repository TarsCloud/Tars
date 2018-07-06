# TARS-SERVER说明文档

包名:phptars/tars-server
是TARS—PHP-SERVER的底层依赖。

## 如何使用
TARS-SERVER使用composer进行包管理,开发者只需要根据相应的版本,进行composer install即可。

具体的使用方式,请参考对应php/examples下面的http-server、timer-server和tcp-server。

## 框架说明
TARS-SERVER是以SWOOLE为底层的网络收发实现的,框架主要包含如下的目录:
* cmd: 负责框架的启动和停止命令的实现,现在支持start、stop和restart命令
* core: 框架的核心实现  
* protocol: 负责进行协议处理

### cmd层
针对cmd层,现在包含如下几个文件:
1. Command.php: 
负责在服务启动的时候,指定配置文件和启动命令

2. CommandBase
规定了一个Command所必须的实现,所有的诸如START,都是CommandBase的子类。其中提供了getProcess方法,来获取当前启动的服务进程。

3. Restart
重启命令,只是调用停止后,再调用启动 
 
4. Start
启动命令,会首先解析平台下发的配置,然后引入业务所必须的services.php文件。
接下来监测进程是否已经启动,从而避免重复启动;
最后将配置和预先定义的SwooleTable传入Server,进行服务的初始化和启动过程。


5. Stop
现在的服务停止方式比较暴力,会根据服务的名称拉出所有的进程,然后kill掉。后续会引入reload的方式进行服务的代码重新加载。


### core核心层
核心层主要由Event、Server、Request和Response组成。

1. Server.php
负责服务的启动前的初始化工作,包括:
* 判断是tcp还是http类型,从而注册对应的回调,启动对应的server
* 判断如果是timer,会启动对对应目录的timer扫描
* 将swoole的配置透传
* 注册通用的回调函数
* 传递server的swooletable
* 指定整个框架的启动文件,并强制require
* 指定框架的协议处理方式,是tars还是http

在完成服务的启动之后,首先会进入onMasterStart:
* 写入进程的名称
* 将pid写入文件中
* 进行服务的初始化上报

onManagerStart:
* 重命名进程

其次是onWorkerStart:
* 如果是tcp类型,需要先将interface中的注释转化为php的数据,方便路由的时候处理
* 如果是http的类型,需要指定对应的namespacename
* 设置对应的worker的名称
* 如果是timer,需要启动对应的timer
* 在workerId=0的时候(保证只触发一次),将服务的保活上报任务投递到TASK里面


onTask: 将服务的APPName serverName servantName进行上报。

分别需要关注onReceive和onRequest两个回调。

对于tcp的server,关注onReceive:
* 初始化Request对象,将sw对象,传入超全局变量$_SERVER
* 设置protocol为TARSProtocol
* 进行协议处理,并回包
* 清除全局变量

对于http的server,关注onRequest:
* 处理cookie、get、post请参数
* 初始化Request对象,将sw对象,传入超全局变量$_SERVER
* 进行协议处理,并回包
* 清除全局变量


2. Event.php
onReceive方法:
* tcp协议的请求,先会进入TARSProtocol的route方法,进行路由
* 完成路由后,进行实际的函数调用
* 打包回包
* 发送回包


onRequest方法:
* 提供一个默认的探测接口
* 进行基本的路由协议解析
* 调用对应的controller方法
* 发送回包

3. Request.php
储存一些必要的请求数据;
设置和去掉全局变量

4. Response.php
负责回包的一些工作


### 服务启动流程
整个服务启动由cmd下的Start发起,
之后调用Server对象的创建,
然后依次进行swoole的初始化工作,
完成启动服务之后,只需要处理onReceive或者onRequest的监听即可


## 框架依赖
框架依赖以下几个包:
* phptars/tars-client: 进行tars服务的调用
* phptars/tars-report: 负责服务本身运行状态的上报
* phptars/tars-config: 负责对于平台上传的配置的拉取

