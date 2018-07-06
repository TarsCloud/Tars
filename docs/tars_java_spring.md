# Tars-Spring 使用说明

## 功能说明

Tars支持使用Spring配置servant，使用此功能需要依赖tars-spring.jar包，以及spring 4及以上版本。你可以将你的servant作为一个Spring bean，你可以自由的使用Spring的功能最后只需要通过Tars提供的标签告知Tars哪些些bean是servant即可。

## 依赖配置

使用此功能需要添加依赖jar包，在pom.xml中添加如下配置：

```xml
<dependency>
      <groupId>com.tencent.tars</groupId>
      <artifactId>tars-spring</artifactId>
      <version>1.4.0</version>
</dependency>
```

## 服务暴露配置

使用spring配置模式需要在resources目录下将tars原版配置文件servants.xml改为servants-spring.xml，如果两种配置文件均存在会优先读取servant.xml则不会启用spring模式。

### Servant配置

在Spring模式下，servants-spring.xml为spring配置文件，在配置文件中需要先引入Tars的xsd文件：

```xml
<?xml version="1.0" encoding="UTF-8"?>
<beans xmlns="http://www.springframework.org/schema/beans"
       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
       xmlns:tars="http://tars.tencent.com/schema/tars"
       xsi:schemaLocation="
       http://www.springframework.org/schema/beans http://www.springframework.org/schema/beans/spring-beans-4.0.xsd
       http://tars.tencent.com/schema/tars http://tars.tencent.com/schema/tars/tars.xsd">
</beans>
```

Tars中自定义了多个spring标签，配置servant时需要先将servant定义为一个bean，然后通过Tars标签定义为servant：

```xml
<tars:servant name="HelloObj" interface="com.qq.test.GreeterServant" ref="greeterServantImp"/>
<bean id="greeterServantImp" class="com.qq.test.impl.GreeterServantImp" />
```

servant标签通过name指定servant名称，ref指定对应的bean名，通过interface指定对应的接口名。当然你也可以通过添加@Component标签然后通过spring自动扫描获取到bean，同样只需要配置对应bean名即可：

```xml
<context:component-scan base-package="com.qq.tars.test"/>
<tars:servant name="HelloObj" interface="com.qq.test.GreeterServant" ref="greeterServantImp"/>
```

### Listener配置

listener的配置与servant配置相同，也需要将你的Listener定义为bean，然后通过tars标签将对应的bean指定为Listener：

```xml
<tars:listener ref="startListener"/>
<bean id="startListener" class="com.qq.test.ServiceStartListener" />
```
## 1.3.0版本升级指南

如需使用tars-spring等新功能需要将tars升级到1.3.0版本及以上版本，本次改动相对较大，附上版本升级指南：
1. 管理平台需要重新编译升级。
2. tars-node需要升级到新版本。
3. 在模版管理中修改tars.tarsjava.default模版的classpath配置项，修改为
> classpath=${basepath}/conf:${basepath}/WEB-INF/classes:${basepath}/WEB-INF/lib
4. 在构建tars-java项目时servants.xml需要放在resources目录下
