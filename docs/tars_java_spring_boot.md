# Tars-Spring 使用说明

## 功能说明

Tars支持使用通过spring boot的方式编写tars服务，使用此功能需要依赖tars-spring-boot-starter.jar包，以及spring boot 2.0及以上版本。你可以将你的servant作为一个Spring bean，注解暴露spring bean即可。

## demo工程
demo工程参考[demo工程](https://github.com/Tencent/Tars/tree/master/java/examples/tars-spring-boot-server)

## 依赖配置

使用此功能需要添加依赖，在pom.xml中添加如下配置：
```xml
    <properties>
        <spring-boot.version>2.0.3.RELEASE</spring-boot.version>
    </properties>

    <dependencyManagement>
        <dependencies>
            <dependency>
                <groupId>org.springframework.boot</groupId>
                <artifactId>spring-boot-dependencies</artifactId>
                <version>${spring-boot.version}</version>
                <type>pom</type>
                <scope>import</scope>
            </dependency>
        </dependencies>
    </dependencyManagement>

    <dependencies>
        <dependency>
            <groupId>com.tencent.tars</groupId>
            <artifactId>tars-spring-boot-starter</artifactId>
            <version>1.5.0</version>
        </dependency>
    </dependencies>
```

### Servant配置

在spring boot中，需要通过注解开启tars服务相关功能：
```java
@SpringBootApplication
@EnableTarsServer
public class QuickStartApplication {
    public static void main(String[] args) {
        SpringApplication.run(QuickStartApplication.class, args);
    }
}
```
通过注解@EnableTarsServer标识这是一个TARS服务，并开启服务相关功能。

编写tars协议文件，如：
```
module TestApp
{
	interface Hello
	{
	    string hello(int no, string name);
	};
};
```
并通过TARS提供的maven插件生成对应的接口代码：
```java
@Servant
public interface HelloServant {

	public String hello(int no, String name);
}

```
服务逻辑通过实现接口来编写：
```java
@TarsServant("HelloObj")
public class HelloServantImpl implements HelloServant {

    @Override
    public String hello(int no, String name) {
        return String.format("hello no=%s, name=%s, time=%s", no, name, System.currentTimeMillis());
    }
}
```
接口的实现类通过注解@TarsServant来暴露服务，其中填写的'HelloObj'为servant名，该名称与管理平台上的名称对应即可。

## 编写一个Http服务
此外如果你想使用spring-boot来编写一个http服务，而不使用taf接口的话也是可以的：
```java
@SpringBootApplication
@EnableTarsServer
@TarsHttpService("HttpObj")
@RestController
public class DemoApplication {
    public static void main(String[] args) {
        SpringApplication.run(DemoApplication.class, args);
    }
	
	@RequestMapping(path = "/test")
    public String test() {
        return "hello world";
    }
}
```
此时添加一个@TarsHttpService注解，这个注解中需要添加你希望绑定的非tars协议 servant的名称，框架会自动将spring-boot中嵌入的容器的端口绑定到对应Servant的端口上，这样你就可以方便的使用spring-mvc的各种注解来开发http和web服务了。

## 客户端注入
为了简化客户端的构造，spring-boot-starter提供了客户端自动注入的功能，在你需要注入客户端的属性上加上@TarsClient注解，框架会自动帮你构造并注入客户端：
```java
@TarsServant("HelloObj")
public class HelloWorldServantImpl implements HelloWordServant {
    @TarsClient("TarsJavaTest.SpringBootServer.HelloObj")
    HelloWordPrx prx;
}
```
如上述代码，通过@TarsClient("TarsJavaTest.SpringBootServer.HelloObj")即可注入HelloWordPrx客户端，如果只填写Obj名称则采用默认值注入客户端，当然你也可以在注解中自定义客户端配置：
```java
@TarsServant("HelloObj")
public class HelloWorldServantImpl implements HelloWordServant {
    @TarsClient(name = "TarsJavaTest.SpringBootServer.HelloObj", syncTimeout = 1000)
    HelloWordPrx prx;
}
```
这样就设置了客户端同步超时时间，该注解提供了所有常用配置的配置项：
```java
@Target({ ElementType.FIELD })
@Retention(RetentionPolicy.RUNTIME)
@Documented
public @interface TarsClient {
    @AliasFor("name")
    String value() default "";

    @AliasFor("value")
    String name() default "";

    String setDivision() default "";

    boolean isgray() default false;

    int connections() default Constants.default_connections;

    int connectTimeout() default Constants.default_connect_timeout;

    int syncTimeout() default Constants.default_sync_timeout;

    int asyncTimeout() default Constants.default_async_timeout;

    boolean enableSet() default false;

    boolean tcpNoDelay() default false;

    String charsetName() default "UTF-8";
}
```
包括连接数、同步异步超时时间，字符集等均可以在注解中配置。

## 服务发布

编写完成代码后，可以通过spring boot提供的spring-boot-maven-plugin插件进行打包，将服务打包为jar包后上传即可启动。

## 客户端注入



## 如何在本地启动和开发调试tars
拷贝node生成的模板文件到本地（在服务器 tasnode/data/服务名/conf 目录下），修改其中每个servant的启动ip和端口文本地ip端口
配置启动参数。-Dconfig=(模板路径)
通过ide启动MainClass

## 版本升级指南

如需使用tars-spring-boot的新功能需要将tars升级到1.5.0版本及以上版本，本次改动相对较大，附上版本升级指南：
1. 管理平台需要重新编译升级。
2. tars-node需要升级到新版本。
3. 模板选择需要选tars.tarsjava.springboot模版。如果不是重新搭建环境可自行添加模板，父模板选择tars.tarsjava.default，内容如下：
```
<tars>
<application>
<server>
packageFormat=jar
mainclass=-jar ${basepath}/${app}.${server}.jar
</server>
</application>
</tars>
```
