# 目录 #
> * 服务端开发
> * 客户端开发
> * 业务配置
> * 服务日志
> * 服务管理
> * 异常上报
> * 属性统计

# 框架使用指南 #

## 服务端开发 ##

下面通过一个完整的hello world例子介绍如何实现自己的服务。

### 依赖配置 ###
在构建项目**pom.xml**中添加依赖jar包

- 框架依赖配置
```xml
<dependency>
    <groupId>com.tencent.tars</groupId>
    <artifactId>tars-server</artifactId>
    <version>1.4.0</version>
    <type>jar</type>
</dependency>
```
- 插件依赖配置
```xml
<plugin>
    <groupId>com.tencent.tars</groupId>
    <artifactId>tars-maven-plugin</artifactId>
    <version>1.4.0</version>
    <configuration>
	<tars2JavaConfig>
	    <tarsFiles>
		<tarsFile>${basedir}/src/main/resources/hello.tars</tarsFile>
	    </tarsFiles>
	    <tarsFileCharset>UTF-8</tarsFileCharset>
	    <servant>true</servant>
	    <srcPath>${basedir}/src/main/java</srcPath>
	    <charset>UTF-8</charset>
	    <packagePrefixName>com.qq.tars.quickstart.server.</packagePrefixName>
	</tars2JavaConfig>
    </configuration>
</plugin>
```
### 接口文件定义 ###

接口文件定义是通过Tars接口描述语言来定义，在src/main/resources目录下建立hello.tars文件，内容如下
```    	protobuf
module TestApp 
{
	interface Hello
	{
	    string hello(int no, string name);
	};
};
```
### 接口文件编译 ###

提供插件编译生成java代码，在tars-maven-plugin添加生成java文件配置

```xml
<plugin>
	<groupId>com.tencent.tars</groupId>
	<artifactId>tars-maven-plugin</artifactId>
	<version>1.4.0</version>
	<configuration>
		<tars2JavaConfig>
			<!-- tars文件位置 -->
			<tarsFiles>
				<tarsFile>${basedir}/src/main/resources/hello.tars</tarsFile>
			</tarsFiles>
			<!-- 源文件编码 -->
			<tarsFileCharset>UTF-8</tarsFileCharset>
			<!-- 生成服务端代码 -->
			<servant>true</servant>
			<!-- 生成源代码编码 -->
			<charset>UTF-8</charset>
			<!-- 生成的源代码目录 -->
			<srcPath>${basedir}/src/main/java</srcPath>
			<!-- 生成源代码包前缀 -->
			<packagePrefixName>com.qq.tars.quickstart.server.</packagePrefixName>
		</tars2JavaConfig>
	</configuration>
</plugin>
```

在工程根目录下执行mvn tars:tars2java
```			java
@Servant
public interface HelloServant {
    public String hello(int no, String name);
}	
```
### 服务接口实现 ###

新创建一个HelloServantImpl.java文件，实现HelloServant.java接口

```java
public class HelloServantImpl implements HelloServant {
```

```java
@Override
public String hello(int no, String name) {
    return String.format("hello no=%s, name=%s, time=%s", no, name, System.currentTimeMillis());
}
```


### 服务暴露配置 ###

在WEB-INF下创建一个servants.xml的配置文件，服务编写后需要进程启动时加载配置暴露服务，配置如下:
​			
```xml
<?xml version="1.0" encoding="UTF-8"?>
<servants>
	<servant name="HelloObj">
		<home-api>com.qq.tars.quickstart.server.testapp.HelloServant</home-api>
		<home-class>com.qq.tars.quickstart.server.testapp.impl.HelloServantImpl</home-class>
	</servant>
</servants>
```

说明：除了此方法之外，还可以采用spring模式来配置服务，详情见tars_java_spring.md。

### 服务配置ServerConfig

服务框架中有全局的结构ServerConfig，它记录了服务的基本信息，在服务框架初始化时会自动从服务配置文件中初始化这些参数。参数说明：
> * Application：应用名称，如果配置文件没有配置，默认为UNKNOWN；
> * ServerName：服务名称；
> * BasePath：基本路径，通常表示可执行文件的路径；
> * DataPath：数据文件路径，通常表示存在服务自己的数据；
> * LocalIp：本地ip，默认是本机非127.0.0.1的第一块网卡IP；
> * LogPath：日志文件路径，日志的写法请参考后续；
> * LogLevel：滚动log日志级别；
> * Local：服务可以有管理端口，可以通过管理端口发送命令给服务，该参数表示绑定的管理端口的地址，例如tcp -h 127.0.0.1 -p 8899，如果没有设置则没有管理端口；
> * Node：本地NODE地址，如果设置，则定时给NODE发送心跳，否则不发送心跳，通常只有发布到框架上面的服务才有该参数；
> * Log：日志中心地址，例如：tars.tarslog.LogObj@tcp –h .. –p …，如果没有配置，则不记录远程日志；
> * Config：配置中心地址，例如：tars.tarsconfig.ConfigObj@tcp –h … -p …，如果没有配置，则addConfig函数无效，无法从远程配置中心拉取配置；
> * Notify：信息上报中心地址，例如：tars.tarsnotify.NotifyObj@tcp –h … -p …，如果没有配置，则上报的信息直接丢弃；
> * SessionTimeOut：防空闲连接超时设置；
> * SessionCheckInterval：防空闲连接超时检查周期；

服务的配置文件格式如下：

	<tars>
	  <application>
	    <server>
	       #本地node的ip:port
	       node=tars.tarsnode.ServerObj@tcp -h x.x.x.x -p 19386 -t 60000
	       #应用名称
	       app=TestApp
	       #服务名称
	       server=HelloServer
	       #本机ip
	       localip=x.x.x.x
	       #管理端口
	       local=tcp -h 127.0.0.1 -p 20001 -t 3000
	       #服务可执行文件,配置文件等
	       basepath=/usr/local/app/tars/tarsnode/data/TestApp.HelloServer/bin/
	       #服务数据目录
	       datapath=/usr/local/app/tars/tarsnode/data/TestApp.HelloServer/data/
	       #日志路径
	       logpath=/usr/local/app/tars/app_log/
	       #配置中心的地址
	       config=tars.tarsconfig.ConfigObj
	       #通知上报的地址[可选]
	       notify=tars.tarsnotify.NotifyObj
	       #远程日志的地址[可选]
	       log=tars.tarslog.LogObj
	       #服务停止的超时时间
	       deactivating-timeout=2000
	       #日志等级
	       logLevel=DEBUG
	       #防空闲连接超时设置
		   sessionTimeOut=120000
		   #防空闲连接超时检查周期
	       sessionCheckInterval=60000
	    </server>
	  </application>
	</tars>

### Adapter

Adapter表示了绑定端口。服务新增一个绑定端口，则新建立一个Adapter，并设置相关的参数和处理对象则可以非常方便的完成对这个端口上的处理，通常用这个功能来完成在其他协议的支撑。

对于TARS服务而言，在服务配置文件中增加adapter项，即可以完成增加一个Servant处理对象。

Adapter配置如下：

	<tars>
	  <application>
	    <server>
	       #配置绑定端口
	       <TestApp.HelloServer.HelloObjAdapter>
	            #允许的IP地址
	            allow
	            #监听IP地址
	            endpoint=tcp -h x.x.x.x -p 20001 -t 60000
	            #处理组
	            handlegroup=TestApp.HelloServer.HelloObjAdapter
	            #最大连接数
	            maxconns=200000
	            #协议
	            protocol=tars
	            #队列大小
	            queuecap=10000
	            #队列超时时间毫秒
	            queuetimeout=60000
	            #处理对象
	            servant=TestApp.HelloServer.HelloObj
	            #当前线程个数
	            threads=5
	       </TestApp.HelloServer.HelloObjAdapter>
	    </server>
	  </application>
	</tars>

添加servant项需要在servants.xml中完成配置

### 服务启动

服务启动时需要在启动命令中添加配置文件**-Dconfig=config.conf**，
注意：config.conf为配置文件，服务端的配置文件和客户端的配置文件必须合并到这一个文件中。完整的如下：

	<tars>
	  <application>
	    enableset=n
	    setdivision=NULL
	    <server>
	       #本地node的ip:port
	       node=tars.tarsnode.ServerObj@tcp -h x.x.x.x -p 19386 -t 60000
	       #应用名称
	       app=TestApp
	       #服务名称
	       server=HelloServer
	       #本机ip
	       localip=x.x.x.x
	       #管理端口
	       local=tcp -h 127.0.0.1 -p 20001 -t 3000
	       #服务可执行文件,配置文件等
	       basepath=/usr/local/app/tars/tarsnode/data/TestApp.HelloServer/bin/
	       #服务数据目录
	       datapath=/usr/local/app/tars/tarsnode/data/TestApp.HelloServer/data/
	       #日志路径
	       logpath=/usr/local/app/tars/app_log/
	       #配置中心的地址
	       config=tars.tarsconfig.ConfigObj
	       #通知上报的地址[可选]
	       notify=tars.tarsnotify.NotifyObj
	       #远程日志的地址[可选]
	       log=tars.tarslog.LogObj
	       #服务停止的超时时间
	       deactivating-timeout=2000
	       #日志等级
	       logLevel=DEBUG
	        #配置绑定端口
	       <TestApp.HelloServer.HelloObjAdapter>
	            #允许的IP地址
	            allow
	            #监听IP地址
	            endpoint=tcp -h x.x.x.x -p 20001 -t 60000
	            #处理组
	            handlegroup=TestApp.HelloServer.HelloObjAdapter
	            #最大连接数
	            maxconns=200000
	            #协议
	            protocol=tars
	            #队列大小
	            queuecap=10000
	            #队列超时时间毫秒
	            queuetimeout=60000
	            #处理对象
	            servant=TestApp.HelloServer.HelloObj
	            #当前线程个数
	            threads=5
	       </TestApp.HelloServer.HelloObjAdapter>
	    </server>
	    <client>
	       #主控的地址
	       locator=tars.tarsregistry.QueryObj@tcp -h x.x.x.x -p 17890
	       #同步超时时间
	       sync-invoke-timeout=3000
	       #异步超时时间
	       async-invoke-timeout=5000
	       #刷新ip列表的时间间隔
	       refresh-endpoint-interval=60000
	       #上报数据的时间间隔
	       report-interval=60000
	       #采样率
	       sample-rate=100000
	       #最大采样数
	       max-sample-count=50
	       #模版名称
	       modulename=TestApp.HelloServer
	    </client>
	  </application>
	</tars>

### 异步嵌套

异步嵌套代表如下情况：

>  A 异步调用B，B接收到请求后再异步调用C，等C返回后，B再将结果返回A。

通常情况下面，B接收到请求后，在接口处理完毕以后就需要返回应答给A，因此如果B在接口中又发起异步请求到C，则无法实现。

因此需要在实现接口方法中，声明启动异步来实现跨服务的异步调用，
​	
	//声明启动异步上下文
	AsyncContext context = AsyncContext.startAsync();
	//接口实现
	...
	
	//在异步处理后回包
	context.writeResult(...);

## 客户端开发

客户端可以不用写任何协议通信代码即可完成远程调用。

### 通信器

完成服务端以后，客户端对服务端完成收发包的操作是通过通信器**Communicator**来实现的。

通信器的初始化如下：

	CommunicatorConfig cfg = CommunicatorConfig.load("config.conf");
	//构建通信器
	Communicator communicator = CommunicatorFactory.getInstance().getCommunicator(cfg);

说明：
> * 通信器的配置文件格式后续会介绍；
> * 通信器缺省不采用配置文件也可以使用，所有参数都有默认值；
> * 通信器也可以直接通过属性来完成初始化；
> * 如果需要通过名字来获取客户端调用代理，则必须设置locator参数；

通信器属性说明：
> * locator: registry服务的地址，必须是有ip port的，如果不需要registry来定位服务，则不需要配置；
> * connect-timeout：网络连接超时时间，毫秒，没有配置缺省为3000
> * connections；连接数，默认为4；
> * sync-invoke-timeout：调用最大超时时间（同步），毫秒，没有配置缺省为3000
> * async-invoke-timeout：调用最大超时时间（异步），毫秒，没有配置缺省为5000
> * refresh-endpoint-interval：定时去registry刷新配置的时间间隔，毫秒，没有配置缺省为1分钟
> * stat：模块间调用服务的地址，如果没有配置，则上报的数据直接丢弃；
> * property：属性上报地址，如果没有配置，则上报的数据直接丢弃；
> * report-interval：上报给stat/property的时间间隔，默认为60000毫秒；
> * modulename：模块名称，默认为可执行程序名称；


通信器配置文件格式如下：

	<tars>
	  <application>
		#set调用
		enableset                      = N
		setdivision                    = NULL 
	    #proxy需要的配置
	    <client>
	        #地址
	        locator                     = tars.tarsregistry.QueryObj@tcp -h 127.0.0.1 -p 17890
	        #同步最大超时时间(毫秒)
	        connect-timeout             = 3000
	        #网络连接数
	        connections                 = 4
	        #网络连接超时时间(毫秒)
	        sync-invoke-timeout         = 3000
	        #异步最大超时时间(毫秒)
	        async-invoke-timeout        = 5000
	        #刷新端口时间间隔(毫秒)
	        refresh-endpoint-interval   = 60000
	        #模块间调用
	        stat                        = tars.tarsstat.StatObj
	        #属性上报地址
	        property                    = tars.tarsproperty.PropertyObj
	        #report time interval
	        report-interval             = 60000
	        #模块名称
	        modulename                  = TestApp.HelloServer
	    </client>
	  </application>
	</tars>


使用说明：
> * 当使用Tars框架做服务端使用时，通信器不要自己创建，直接采用服务框架中的通信器就可以了，例如：CommunicatorFactory.getInstance().getCommunicator().stringToProxy(...)，对于纯客户端情形，则要用户自己定义个通信器并生成代理（proxy）；
> * getCommunicator()是框架初始化，随时可以获取；
> * 对于通信器创建出来的代理，也不需要每次需要的时候都stringToProxy，初始化时建立好，后面直接使用就可以了；
> * 代理的创建和使用，请参见下面几节；
> * 对同一个Obj名称，多次调用stringToProxy返回的Proxy其实是一个，多线程调用是安全的，且不会影响性能；

### 超时控制

超时控制是对客户端proxy（代理）而言。上节中通信器的配置文件中有记录：

	#同步最大超时时间(毫秒)
	sync-invoke-timeout          = 3000
	#异步最大超时时间(毫秒)
	async-invoke-timeout         = 5000

上面的超时时间对通信器生成的所有proxy都有效，如果需要单独设置超时时间，设置如下：

针对proxy设置(ServantProxyConfig与CommunicatorConfig类似)
​	
```java
//设置该代理单独初始化配置
public <T> T stringToProxy(Class<T> clazz, ServantProxyConfig servantProxyConfig)
```


### 调用

本节会详细阐述远程调用的方式。

首先简述tars客户端的寻址方式，其次会介绍客户端的调用方式，包括但不限于单向调用、同步调用、异步调用、hash调用等。

#### 寻址方式简介

Tars服务的寻址方式通常可以分为如下两种方式，即服务名在主控注册和不在主控注册，主控是指专用于注册服务节点信息的名字服务（路由服务）。
名字服务中的服务名添加则是通过操作管理平台实现的。

对于没有在主控注册的服务，可以归为直接寻址方式，即在服务的obj后面指定要访问的ip地址。客户端在调用的时候需要指定HelloObj对象的具体地址：

即：TestApp.HelloServer.HelloObj@tcp -h 127.0.0.1 -p 9985

TestApp.HelloServer.HelloObj：对象名称

tcp：tcp协议

-h：指定主机地址，这里是127.0.0.1 

-p：端口地址，这里是9985

如果HelloServer在两台服务器上运行，则HelloPrx初始化方式如下：
​	
```java
HelloPrx prx = c.stringToProxy("TestApp.HelloServer.HelloObj@tcp -h 127.0.0.1 -p 9985:tcp -h 192.168.1.1 -p 9983");
```

即，HelloObj的地址设置为两台服务器的地址。此时请求会分发到两台服务器上（分发方式可以指定，这里不做介绍），如果一台服务器down，则自动将请求分到另外一台，并定时重试开始down的那一台服务器。

对于在主控中注册的服务，服务的寻址方式是基于服务名进行的，客户端在请求服务端的时候则不需要指定HelloServer的具体地址，但是需要在生成通信器或初始化通信器的时候指定registry(主控中心)的地址。

```java
HelloPrx prx = c.stringToProxy<HelloPrx>("TestApp.HelloServer.HelloObj");
```

#### 单向调用

所谓单向调用，表示客户端只管发送数据，而不接收服务端的响应，也不管服务端是否接收到请求。
​	
```java
HelloPrx prx = c.stringToProxy("TestApp.HelloServer.HelloObj");
//发起远程调用
prx.async_hello(null, 1000, "hello word");
```

#### 同步调用

请看如下调用示例：
​	
```java
HelloPrx prx = c.stringToProxy("TestApp.HelloServer.HelloObj");
//发起远程调用
prx.hello(1000, "hello word");
```

#### 异步调用

请看如下调用示例：

```java
HelloPrx prx = c.stringToProxy("TestApp.HelloServer.HelloObj");
//发起远程调用
prx.async_hello(new HelloPrxCallback() {
        
        @Override
        public void callback_expired() {
        }
        
        @Override
        public void callback_exception(Throwable ex) {
        }
        
        @Override
        public void callback_hello(String ret) {
            System.out.println(ret);
        }
    }, 1000, "hello word");
```


注意：
> * 当接收到服务端返回时，HelloPrxCallback的callback_hello会被响应。
> * 如果调用返回异常或超时，则callback_exception会被调用，ret的值定义如下：

#### set方式调用

目前框架已经支持业务按set方式进行部署，按set部署之后，各个业务之间的调用对开业务发来说是透明的。但是由于有些业务有特殊需求，需要在按set部署之后，客户端可以指定set名称来调用服务端，因此框架则按set部署的基础上增加了客户端可以指定set名称去调用业务服务的功能。

详细使用规则如下，

假设业务服务端HelloServer部署在两个set上，分别为Test.s.1和Test.n.1。那么客户端指定set方式调用配置
​	
		enableset                      = Y
		setdivision                    = Test.s.1


## 业务配置

Tars服务框架提供了从tarsconfig拉取服务的配置到本地目录的功能。

使用方法很简单，服务在启动通过在注册监听器里面加载到服务conf目录。

以HelloServer为例：

```java
public class AppStartListener implements AppContextListener {
```

```java
    @Override
    public void appContextStarted(AppContextEvent event) {
        ConfigHelper.getInstance().loadConfig("helloServer.conf");
    }

    @Override
    public void appServantStarted(AppServantEvent event) {
    }
}
```

在servant.xml中注册配置
​	
```xml
<listener>
	<listener-class>com.qq.tars.quickstart.server.AppStartListener</listener-class>
</listener>
```


说明：
> * HelloServer.conf配置文件可以在管理平台上配置；
> * HelloServer.conf拉取到本地后，业务只要通过classloader就可以加载；
> * 配置文件的管理都在web管理平台上，同时管理平台可以主动push配置文件到Server；
> * 配置中心支持ip级别的配置，即一个服务部署在多台服务上，只有部分不同（与IP相关），这种情况下，配置中心可以支持配置文件的合并，同时支持在web管理平台查看和修改；

注意：
> * 对于没有发布到管理平台上的服务，需要在服务的配置文件中指定Config的地址，否则不能使用远程配置。

## 服务日志

框架支持本地和远程日志，获取日志Logger对象如下
​	
```java
private final static Logger FLOW_LOGGER = Logger.getLogger("flow", LogType.LOCAL);
```

说明：打远程日志前需要预先申请远程日志服务
> * LogType.LOCAL：只打本地日志
> * LogType.REMOTE只打远程日志
> * LogType.All 打本地和远程日志

## 服务管理

服务框架可以支持动态接收命令，来处理相关的业务逻辑，例如：动态更新配置等。

- 发送管理命令

服务的管理命令的发送方式：通过管理平台，将服务发布到平台上，通过管理平台发送命令；
​	
TARS服务框架目前内置命令：

> * tars.help    		//查看所有管理命令
> * tars.loadconfig     //从配置中心, 拉取配置下来: tars.loadconfig filename
> * tars.setloglevel    //设置滚动日志的等级: tars.setloglevel [NONE, ERROR, WARN, DEBUG]
> * tars.viewstatus     //查看服务状态
> * tars.connection     //查看当前链接情况

- 自定义命令

服务的自定义命令发送方式，通过管理平台自定义命令发送；
只需注册相关命令以及命令处理类，如下

```java
CustemCommandHelper.getInstance().registerCustemHandler("cmdName",new CommandHandler() {
```

```java
	@Override
	public void handle(String cmdName, String params) {
		
	}
});
```

## 异常上报
为了更好监控，框架支持在程序中将异常直接上报到tarsnotify，并可以在管理平台页面上查看到。

框架提供异常上报工具，使用如下
​	
```java
NotifyHelper.getInstance().notifyNormal(info);
NotifyHelper.getInstance().notifyWarn(info);
NotifyHelper.getInstance().notifyError(info);
```

说明：
> * notifyNormal 上报普通的信息
> * notifyWarn 上报警告信息
> * notifyError 上报错误信息


## 属性统计

为了方便业务做统计，框架中也支持能够在管理平台看上报的信息展示。

目前支持的统计类型包括以下几种：
> * 求和（sum）
> * 平均（avg）
> * 分布（distr）
> * 最大值（max）
> * 最小值（min）
> * 计数（count）

示例代码如下：
​	
	PropertyReportHelper.getInstance().createPropertyReporter("queue_size");
	PropertyReportHelper.getInstance().reportPropertyValue("queue_size", 100);

说明：
> * 上报数据是定时上报的，可以在通信器的配置中设置，目前是1分钟一次;
> * 注意调用createPropertyReport时，必须在服务启动以后创建并保存好创建的对象，后续拿这个对象report即可，不要每次使用的时候create;

## 染色日志

为了方便在debug时实时查看某个用户在调用某服务某接口后引起的后续相关调用消息流的日志，框架中支持将该用户触发的所有日志单独打印一份到一个指定日志文件中。

染色日志有主动打开和被动打开两种方法：

- 主动染色：

> - 在发起请求的客户端显式调用框架中的染色开启接口，从打开开关到显式调用框架中染色关闭接口，中间的日志都会额外打印为染色日志。
> - 在开启染色功能时，所有的发起的taf请求都会自动传递染色状态，被调服务接收到请求后产生的日志也会打印为染色日志，在该请求处理完成后，会自动关闭被调服务的染色开关。

示例代码如下：

```
DyeingSwitch.enableActiveDyeing("helloServer");   //主动打开开关接口，参数表示染色日志名称
...业务处理
logger.info("hello world");   //此时出于染色开启状态，该条日志会额外打印一份到染色日志中
...业务处理
DyeingSwitch.closeActiveDyeing();    //主动关闭染色开关接口
```

说明：

> - 开启染色日志时传递的参数推荐填写服务名，如果填写为null则默认名称为default;
> - 染色日志的日志级别和日志类型与原本日志相同，如果原本日志只打本地，那么染色日志也只打本地，原本日志要打远程染色日志才会打远程;

- 被动染色：

  > - 在请求的服务端预设预先设定染色条件，如果接收到的请求满足染色条件，那么服务端框架层会自动打开染色开关;
  > - 染色状态传递的机制和主动染色相同，但是不需要在业务层显式关闭染色开关;

使用方法如下：

首先在需要染色的Tars接口上定义染色routeKey，这个值就是判断是否开启染色的变量，示例如下：

```java
@Servant
public interface HelloServant {
     public String sayHello(@TarsRouteKey int no, String name);  //使用注释routeKey来表示开启染色的参数变量
}
```

在定义染色routeKey之后，可以通过管理命令"tars.setdyeing"来设置需要染色用户标识（就是routeKey注释对应的值），远程对象名称和接口函数名（可选）。

管理命令格式如下：

```
tars.setdyeing dyeingKey dyeingServant [dyeingInterface]  //三个参数分别对应上文所提值，接口名可选填
```

假设远程对象名称是TestApp.HelloServer.HelloObj,请求接口名为sayHello,需要染色的用户号码为12345，对应的管理命令如下：

```
tars.setdyeing 12345 TestApp.HelloServer.HelloObj sayHello
```

- 染色日志查询：

  > - 本地染色日志：日志默认所在目录为/usr/local/app/tars/app_log/tars_dyeing/，对于主动染色，日志名为打开开关时传入的参数加固定后缀；对于被动染色，日志名为染色入口服务的Server名称加固定后缀。后缀为_dyeing。
  > - 远程日志：在tarslog服务打日志所在机器上 的/usr/local/app/tars/remote_app_log/tars_dyeing/dyeing/目录下，日志名为tars_dyeing.dyeing_{此部分同本地染色日志名}。
