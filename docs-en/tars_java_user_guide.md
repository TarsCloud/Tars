# contents #
> * Server development
> * Client development
> * Service configuration
> * Service logs
> * Service management
> * Abnormally report
> * Attribute statistics

# Guidelines for the use of the tars framework #

## Server development ##

The following is a complete Hello world example to show how to implement your own services.

### Dependency configuration ###
Add dependent jar packages in the build project **pom.xml**

- Framework dependency configuration
```xml
<dependency>
    <groupId>com.tencent.tars</groupId>
    <artifactId>tars-server</artifactId>
    <version>1.4.0</version>
    <type>jar</type>
</dependency>
```
- Plug-in unit dependency configuration
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
### Definition of interface files ###

The definition of the interface file is defined by the Tars interface description language, and the hello.tars file is set up under the src/main/resources directory. The contents are as follows
```    	protobuf
module TestApp 
{
	interface Hello
	{
	    string hello(int no, string name);
	};
};
```
### Compilation of interface files ###

Provide plug-in compile to generate java code, add java file configuration in tars-maven-plugin

```xml
<plugin>
	<groupId>com.tencent.tars</groupId>
	<artifactId>tars-maven-plugin</artifactId>
	<version>1.4.0</version>
	<configuration>
		<tars2JavaConfig>
			<!-- Tars file location -->
			<tarsFiles>
				<tarsFile>${basedir}/src/main/resources/hello.tars</tarsFile>
			</tarsFiles>
			<!-- Source file encoding -->
			<tarsFileCharset>UTF-8</tarsFileCharset>
			<!-- Generating server-side code -->
			<servant>true</servant>
			<!-- Generating source code coding -->
			<charset>UTF-8</charset>
			<!-- Generated source code directory -->
			<srcPath>${basedir}/src/main/java</srcPath>
			<!-- Generating the source code package prefix -->
			<packagePrefixName>com.qq.tars.quickstart.server.</packagePrefixName>
		</tars2JavaConfig>
	</configuration>
</plugin>
```

Executing mvn tars:tars2java in the project root directory
```			java
@Servant
public interface HelloServant {
    public String hello(int no, String name);
}	
```
### Service interface implementation ###

Create a new HelloServantImpl.java file and implement HelloServant.java interface.

```java
public class HelloServantImpl implements HelloServant {
```

```java
@Override
public String hello(int no, String name) {
    return String.format("hello no=%s, name=%s, time=%s", no, name, System.currentTimeMillis());
}
```


### Service exposure configuration ###

Create a servants.xml configuration file under WEB-INF. After the service is written, you need to load the configuration exposed service when the process starts. The configuration is as follows:
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

Note: in addition to this method, spring mode can also be used to configure services. For details, see tars_java_spring.md.

### Server configure ServerConfig

The service framework has a global structure, ServerConfig, which records the basic information of the service and initializes the parameters automatically from the service configuration file when the service framework is initialized. The parameters are described as follows：
> * Application：Application name，If the configuration file is not configured, the default is UNKNOWN；
> * ServerName：Service name；
> * BasePath：basic path, usually representing the path of an executable file；
> * DataPath：data file path, usually representing the data of the service itself；
> * LocalIp：Local IP, the default IP value is the first network card IP of non 127.0.0.1 in this machine；
> * LogPath：Log file path, how to write log files, please refer to the following content；
> * LogLevel：Rolling log level；
> * Local：A service can have a management port, which can be sent to a service through a management port, which represents the address of the bound management port, such as TCP -h 127.0.0.1 -p 8899, indicating no management port if it is not set；
> * Node：If the local NODE address is set, the heartbeat is sent to NODE regularly, otherwise the heartbeat is not transmitted, usually only the service that is posted to the frame has the parameter；
> * Log：Log center address，such as ：tars.tarslog.LogObj@tcp –h .. –p …，If this is not configured, the remote log is not recorded；
> * Config：Configuration center address，such as：tars.tarsconfig.ConfigObj@tcp –h … -p …，If it is not configured, the addConfig function is invalid and cannot be configured from the remote configuration center；
> * Notify：Information report center address，such as：tars.tarsnotify.NotifyObj@tcp –h … -p …，If there is no configuration, the reported information is discarded directly；
> * SessionTimeOut：Timeout settings to prevent free connections；
> * SessionCheckInterval：The inspection cycle is mainly to prevent free connection timeouts；

The configuration file format of the service is as follows：

	<tars>
	  <application>
	    <server>
	       #The ip:port of the local node
	       node=tars.tarsnode.ServerObj@tcp -h x.x.x.x -p 19386 -t 60000
	       #Application name
	       app=TestApp
	       #Server name
	       server=HelloServer
	       #Local ip
	       localip=x.x.x.x
	       #Management port
	       local=tcp -h 127.0.0.1 -p 20001 -t 3000
	       #Service executable files, configuration files, and so on
	       basepath=/usr/local/app/tars/tarsnode/data/TestApp.HelloServer/bin/
	       #Service data directory
	       datapath=/usr/local/app/tars/tarsnode/data/TestApp.HelloServer/data/
	       #Log path
	       logpath=/usr/local/app/tars/app_log/
	       #The address of the configuration center
	       config=tars.tarsconfig.ConfigObj
	       #The address of the report[Option]
	       notify=tars.tarsnotify.NotifyObj
	       #Remote log address[Option]
	       log=tars.tarslog.LogObj
	       #Timeout time of service stop
	       deactivating-timeout=2000
	       #Log level
	       logLevel=DEBUG
	       #Timeout settings to prevent free connections
		   sessionTimeOut=120000
		   #The inspection cycle is mainly to prevent free connection timeouts
	       sessionCheckInterval=60000
	    </server>
	  </application>
	</tars>

### Adapter

Adapter represents the binding port. A new binding port is added to the service, and a new Adapter is set up, and the relevant parameters and processing objects can be very convenient to complete the processing on this port, which is usually used to complete the support of other protocols.

For TARS services, the addition of adapter items in the service configuration file means that an additional Servant processing object can be completed.

The Adapter configuration is as follows:

	<tars>
	  <application>
	    <server>
	       #Configure the binding port
	       <TestApp.HelloServer.HelloObjAdapter>
	            #Allowed IP address
	            allow
	            #Monitor IP address
	            endpoint=tcp -h x.x.x.x -p 20001 -t 60000
	            #Handle group
	            handlegroup=TestApp.HelloServer.HelloObjAdapter
	            #Maximum connection
	            maxconns=200000
	            #protocol
	            protocol=tars
	            #Queue size
	            queuecap=10000
	            #Queue timeout time Millisecond
	            queuetimeout=60000
	            #Servant
	            servant=TestApp.HelloServer.HelloObj
	            #Current thread number
	            threads=5
	       </TestApp.HelloServer.HelloObjAdapter>
	    </server>
	  </application>
	</tars>

Adding servant items needs to be completed in servants.xml..xml中完成配置

### Service startup

When the service starts, add the configuration file **-Dconfig=config.conf** to the boot command.
Note: config.conf is the configuration file, the configuration files of the server and the client must be merged into this file. The complete configuration file is as follows：

	<tars>
	  <application>
	    enableset=n
	    setdivision=NULL
	    <server>
	       #The ip:port of the local node
	       node=tars.tarsnode.ServerObj@tcp -h x.x.x.x -p 19386 -t 60000
	       #Application name
	       app=TestApp
	       #Server name
	       server=HelloServer
	       #Local ip
	       localip=x.x.x.x
	       #Management port
	       local=tcp -h 127.0.0.1 -p 20001 -t 3000
	       #Service executable files, configuration files, and so on
	       basepath=/usr/local/app/tars/tarsnode/data/TestApp.HelloServer/bin/
	       #Service data directory
	       datapath=/usr/local/app/tars/tarsnode/data/TestApp.HelloServer/data/
	       #Log path
	       logpath=/usr/local/app/tars/app_log/
	       #The address of the configuration center
	       config=tars.tarsconfig.ConfigObj
	       #The address of the report[Option]
	       notify=tars.tarsnotify.NotifyObj
	       #Remote log address[Option]
	       log=tars.tarslog.LogObj
	       #Timeout time of service stop
	       deactivating-timeout=2000
	       #Log level
	       logLevel=DEBUG
	        #Configure the binding port
	       <TestApp.HelloServer.HelloObjAdapter>
	            #Allowed IP address
	            allow
	            #Monitor IP address
	            endpoint=tcp -h x.x.x.x -p 20001 -t 60000
	            #Handle group
	            handlegroup=TestApp.HelloServer.HelloObjAdapter
	            #Maximum connection
	            maxconns=200000
	            #protocol
	            protocol=tars
	            #Queue size
	            queuecap=10000
	            #Queue timeout time Millisecond
	            queuetimeout=60000
	            #Servant
	            servant=TestApp.HelloServer.HelloObj
	            #Current thread number
	            threads=5
	       </TestApp.HelloServer.HelloObjAdapter>
	    </server>
	    <client>
	       #Master's address
	       locator=tars.tarsregistry.QueryObj@tcp -h x.x.x.x -p 17890
	       #Synchronous timeout time
	       sync-invoke-timeout=3000
	       #Asynchronous timeout time
	       async-invoke-timeout=5000
	       #Refresh the time interval of the IP list
	       refresh-endpoint-interval=60000
	       #The time interval of the reported data
	       report-interval=60000
	       #sampling rate
	       sample-rate=100000
	       #Maximum sampling number
	       max-sample-count=50
	       #Template name
	       modulename=TestApp.HelloServer
	    </client>
	  </application>
	</tars>

### Asynchronous nesting

Asynchronous nesting represents the following:

>  A invokes B asynchronously, B receives the request and then calls C asynchronously. After C returns, B returns the result back to A.

Normally, after receiving the request, B needs to return to A after the interface is processed, so it cannot be implemented if B also initiates an asynchronous request to C in the interface Therefore, it is necessary to declare startup asynchronism in the implementation interface method to achieve asynchronous calls across services.

​	
	//Declarations start asynchronous context
	AsyncContext context = AsyncContext.startAsync();
	//Interface implementation
	...
	
	//Back packet after asynchronous processing
	context.writeResult(...);

## Client development

The client can complete remote calls without writing any protocol communication code.

### communicator

after the server is completed, the client completes the receiving and sending operations to the server through the 
communicator **Communicator**.

The initialization of the communicator is as follows:

	CommunicatorConfig cfg = CommunicatorConfig.load("config.conf");
	//Construct communicator
	Communicator communicator = CommunicatorFactory.getInstance().getCommunicator(cfg);

Explain：
> * The configuration file format of communicator will be introduced later：
> * Communicator default does not use configuration files or can be used. All parameters have default values:
> * Communicator can also be initialized directly by attributes:
> * If you need to get the client invocation agent by name, you must set the locator parameter:

Communicator attributes:
> * locator: The address of the registry service must be IP port. If there is no need for registry to locate the service, no configuration is required；
> * connect-timeout：Network connection timeout time, milliseconds, without configuration, the default value is 3000.
> * connections；The number of connections, the default value is 4.
> * sync-invoke-timeout：The maximum timeout time (synchronization) of the invoke, milliseconds, without configuration, the default value is 3000
> * async-invoke-timeout：The maximum timeout time (asynchronous) of the invoke, milliseconds, without configuration, the default value is 5000
> * refresh-endpoint-interval：Go to registry to refresh the configuration time interval, milliseconds, if not configured, the default value is 1 minutes
> * stat：The address of the service is invoked between the modules,If there is no configuration, the reported data is discarded directly.
> * property：Address reported in property, if there is no configuration, the reported data will be discarded directly.
> * report-interval：The interval time of reporting to stat/property, default value is 60000 milliseconds
> * modulename：Module name, The default value is the executable program name.


The communicator configuration file format is as follows:

	<tars>
	  <application>
		#set invoking
		enableset                      = N
		setdivision                    = NULL 
	    #configuration required by proxy
	    <client>
	        #address
	        locator                     = tars.tarsregistry.QueryObj@tcp -h 127.0.0.1 -p 17890
	        #Maximum timeout time at synchronous invoke(millisecond)
	        connect-timeout             = 3000
	        #Network connection number
	        connections                 = 4
	        #Network connection timeout time(millisecond)
	        sync-invoke-timeout         = 3000
	        #Maximum timeout time at asynchronous invoke(millisecond)
	        async-invoke-timeout        = 5000
	        #Time interval when a port is refreshed(millisecond)
	        refresh-endpoint-interval   = 60000
	        #Invokes between each module
	        stat                        = tars.tarsstat.StatObj
	        #Address of the reported property
	        property                    = tars.tarsproperty.PropertyObj
	        #report time interval
	        report-interval             = 60000
	        #Module name
	        modulename                  = TestApp.HelloServer
	    </client>
	  </application>
	</tars>


Instruction:
> * When using the Tars framework as the server, communicator does not need to create itself, you can directly use the communicator in the service framework, such as: CommunicatorFactory.getInstance().getCommunicator().stringToProxy(...),
in the case of pure client, users need to define their communicator and generate agent (proxy) themselves.
> * getCommunicator()is the initialization of the framework and can be obtained at any time.
> * For agents created by communicators, it is not necessary to use stringToProx to create it every time when you need it. Once established, you can use it all the time.
> * For the creation and use of the agent, see the following sections.
> * For the same Obj name, the Proxy returned after repeated stringToProxy invokes is the same. Multithread invocation is secure and does not affect performance.

### Timeout control

Timeout control is for the client proxy (agent). There is a record in the configuration file of the communicator in the upper section:

	#Maximum timeout time at synchronous invoke(millisecond)
	sync-invoke-timeout          = 3000
	#Maximum timeout time at asynchronous invoke(millisecond)
	async-invoke-timeout         = 5000

The above timeout time is valid for all proxy generated by communicator. If you need to set the timeout separately, set it as follows:

For proxy settings (ServantProxyConfig is similar to CommunicatorConfig)
​	
```java
//Set the agent to initialize the configuration
public <T> T stringToProxy(Class<T> clazz, ServantProxyConfig servantProxyConfig)
```


### invoking

This section describes the way of remote calls in detail.

First of all, it describes the addressing mode of tars client, and then introduces the mode of calling of the client, including but not limited to one way call, synchronous call, asynchronous call, hash call and so on.

#### A brief introduction to the way of addressing

The addressing modes of Tars services are usually divided into two ways: the name of the service is registered in the master control and the master control is not registered, and the master control refers to the name service (routing service) dedicated to the information of the registered service node.

The service name in the name service is added through the operation management platform.

For non registered services, it can be classified as direct addressing, which is to specify the IP address to be accessed behind the obj of the service. The client needs to specify the specific address of the HelloObj object when invoking.

That is: TestApp.HelloServer.HelloObj@tcp -h 127.0.0.1 -p 9985

TestApp.HelloServer.HelloObj：Object name

tcp：tcp protocol

-h：Specify the host address, Here is 127.0.0.1

-p：Port address, here is 9985

If HelloServer runs on two servers, the HelloPrx initialization is as follows:
​	
```java
HelloPrx prx = c.stringToProxy("TestApp.HelloServer.HelloObj@tcp -h 127.0.0.1 -p 9985:tcp -h 192.168.1.1 -p 9983");
```

That is, the address of HelloObj is set to the address of two servers. At this point, the request will be distributed to two servers (the distribution can be specified without introduction), and if one server down, it automatically divides the request into another and retries the down server at regular intervals.

For services registered in the master control, the service is addressed based on the name of the service, and the client does not need to specify the specific address of the HelloServer when the server is requested, but the registry needs to be specified when the communicator is generated or the communicator is initialized.

```java
HelloPrx prx = c.stringToProxy<HelloPrx>("TestApp.HelloServer.HelloObj");
```

#### Unidirectional invoke

The unidirectional invoke means that the client simply sends data without receiving the response from the server, regardless of whether the server receives the request.
​	
```java
HelloPrx prx = c.stringToProxy("TestApp.HelloServer.HelloObj");
//making remote invoke
prx.async_hello(null, 1000, "hello word");
```

#### Synchronous invoke

Please look at the following invoke for the example:
​	
```java
HelloPrx prx = c.stringToProxy("TestApp.HelloServer.HelloObj");
//making remote invoke
prx.hello(1000, "hello word");
```

#### asynchronous invoke

Please look at the following invoke for the example:

```java
HelloPrx prx = c.stringToProxy("TestApp.HelloServer.HelloObj");
//making remote invoke
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


Note:
> * When the server returns, the callback_hello of HelloPrxCallback will be responded.
> * If the call returns an exception or timeout, the callback_exception will be called, and the value of RET is defined as follows.

#### Set mode invoke

At present, the framework has already supported the deployment of business in set mode. After set deployment, the calls between businesses are transparent to the business deveploment. But because some services have special needs, after deploying set, the client can specify the set name to invoke the server, so the framework increases the function of the client to specify the set name to invoke the service on the basis of the set deployment.

The detailed use of the rules is as follows.

Suppose the business server HelloServer is deployed on two set, namely Test.s.1 and Test.n.1. Then the client specifies the set mode to call configuration
​	
		enableset                      = Y
		setdivision                    = Test.s.1


## Business configuration

The Tars service framework provides the function of pulling the service from tarsconfig to the local directory.

The method used is simple, and the service is loaded into the service conf directory through the information registered in the listener at startup.

Take HelloServer as an example:

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

Registration configuration in servant.xml
​	
```xml
<listener>
	<listener-class>com.qq.tars.quickstart.server.AppStartListener</listener-class>
</listener>
```


Instruction:
> * The HelloServer.conf configuration file can be configured on the management platform.
> * After HelloServer.conf is pulled to local, business can be loaded through classloader.
> * The configuration files are managed on the web management platform, and the management platform can take the initiative to configure push files to Server.
> * The configuration center supports IP level configuration, that is, a service is deployed on multiple services, only partially different (IP related). In this case, the configuration center can support configuration file consolidation, while supporting the view and modification of the web management platform.

Note:
> * For services that have not been released to the management platform, the address of the Config should be specified in the configuration file of the service, otherwise remote configuration can not be used.

## Service log

The framework supports local and remote logs, and gets the log Logger object as follows
​	
```java
private final static Logger FLOW_LOGGER = Logger.getLogger("flow", LogType.LOCAL);
```

Illustration: remote log service must be pre applied before long log.
> * LogType.LOCAL：Only print local logs
> * LogType.REMOTE: Only print remote logs
> * LogType.All : Print local and remote logs

## Service management

The service framework can support dynamic receiving commands to handle related business logic, such as dynamic update configuration, etc.

- Send management command

The sending way of service management commands is to distribute the service to the platform through the management platform and send commands through the management platform.
​	
The TARS service framework is currently built in commands:

> * tars.help    		//View all management commands
> * tars.loadconfig     //From the configuration center, pull the configuration down: tars.loadconfig filename
> * tars.setloglevel    //Set the level of the rolling log: tars.setloglevel [NONE, ERROR, WARN, DEBUG]
> * tars.viewstatus     //View the state of the service
> * tars.connection     //View the current link situation

- Custom command

The custom command sending mode of the service is sent through the custom command of the management platform.
Just register related commands and command processing classes, as follows:

```java
CustemCommandHelper.getInstance().registerCustemHandler("cmdName",new CommandHandler() {
```

```java
	@Override
	public void handle(String cmdName, String params) {
		
	}
});
```

## Abnormally Report
To better monitor, the framework supports direct reporting of exceptions to tarsnotify in the program and can be viewed on the management platform page.

The framework provides an exception reporting tool, which is used as follows:
​	
```java
NotifyHelper.getInstance().notifyNormal(info);
NotifyHelper.getInstance().notifyWarn(info);
NotifyHelper.getInstance().notifyError(info);
```

 Illustration:
> * notifyNormal: Report common information
> * notifyWarn: Report warning information
> * notifyError: Report error information


## Attribute statistics

In order to facilitate business statistics, the framework also supports information presentation that can be reported on the management platform.

The statistical types that are currently supported include the following:
> * Summing (sum)
> * Mean (AVG)
> * Distribution (distr)
> * Maximum value (max)
> * Minimum value (min)
> * Count (count)

The example code is as follows:
​	
	PropertyReportHelper.getInstance().createPropertyReporter("queue_size");
	PropertyReportHelper.getInstance().reportPropertyValue("queue_size", 100);

Illustration:
> * The report of the data is regularly reported and can be set in the configuration of communicator. It is now 1 minutes.
> * Notice that when the createPropertyReport is called, you must create and save the created objects after the service is started, and then take the object report, and do not use create every time.

## Dyed log

In debug, in order to facilitate the real-time view of a user's log of subsequent related call message flows after calling an interface of a service, the framework supports a separate print of all the logs triggered by the user to a specified log file.
There are two ways to dye logs: active opening and passive opening.

- Active staining:

> - If the requested client explicitly invokes the dyed opening interface in the framework, the intermediate logs are printed as the dyed logs from the opening switch to the dyed off interface in the explicit call frame.
> - When the dyeing function is opened, all the initiated TAF requests automatically pass the dyeing state, and the log generated after the request is received will also be printed as a dyed log. After the request is completed, the dyed switch of the transferred service will be automatically closed.

The example code is as follows:

```
DyeingSwitch.enableActiveDyeing("helloServer");   //Open the switch interface automatically, and the parameter indicates the name of the dyed log.
...Business processing
logger.info("hello world");   //At this time, because of the color opening state, the log will print an extra copy to the dyed log.
...Business processing
DyeingSwitch.closeActiveDyeing();    //Active closure of dyed switch interface
```

Illustration:

> - When the dyed log is opened, the parameter passed is recommended to fill in the service name. If it is null, the default name is default.
> - The log level and log type of the dyed log are the same as the original log. If the original log is only local, the dyed log is only local, and the original log will be remote to play the long distance dyed log.

- Passive staining:

  > - When the requesting server presupposes the coloring condition, if the received request satisfies the dyeing condition, the server framework will automatically open the dye switch.
  > - The mechanism of color state transfer is the same as active staining, but there is no need to turn off the dye switch explicitly in the business layer.

The use of the method is as follows:

First, the routeKey is defined on the Tars interface that needs to be dyed. This value is the variable that determines whether to turn on the coloring. The example is as follows:

```java
@Servant
public interface HelloServant {
     public String sayHello(@TarsRouteKey int no, String name);  //Use annotation routeKey to represent parameter variables for opening dyeing
}
```

After defining the dyed routeKey, you can set the user identity of the dyed user (the value corresponding to the routeKey annotation), the remote object name and the interface function name (optional) by managing the command "tars.setdyeing".

The format of the management command is as follows:

```
tars.setdyeing dyeingKey dyeingServant [dyeingInterface]  //The three parameters correspond to the values mentioned above, and the interface names are optional
```

If the name of the remote object is TestApp.HelloServer.HelloObj, the request interface name is sayHello, the user number that needs to be dyed is 12345, and the corresponding management command is as follows:

```
tars.setdyeing 12345 TestApp.HelloServer.HelloObj sayHello
```

- The query of the dyed log:

  > - Local dyed log: the log default directory is /usr/local/app/tars/app_log/tars_dyeing/, for active dyeing, the log name is the fixed suffix when the switch is opened, the log name is the Server name of the staining entry service and the fixed suffix for the passive stain. The suffix is _dyeing.
  > - Remote log: under the /usr/local/app/tars/remote_app_log/tars_dyeing/dyeing/ directory on the machine where the tarslog service print log is located, the log is named tars_dyeing.dyeing_{with the local dyed log name}.
