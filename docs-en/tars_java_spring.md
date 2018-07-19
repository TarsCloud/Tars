# Tars-Spring introduction

## Functional description

Tars supports the use of Spring to configure servant, which needs to depend on the tars-spring.jar package, as well as the spring 4 and above. You can use your servant as a Spring bean, and you can use the function of Spring freely. Finally, you only need to tell Tars which bean is servant through the label provided by the Tars.

## Dependency configuration

Using this function requires adding a dependency jar package and adding the following configuration in pom.xml:

```xml
<dependency>
      <groupId>com.tencent.tars</groupId>
      <artifactId>tars-spring</artifactId>
      <version>1.4.0</version>
</dependency>
```

## Service exposure configuration

Using the spring configuration mode, you need to change the tars original configuration file servants.xml to servants-spring.xml under the resources directory.If all two configuration files exist, the spring mode will not be enabled if the servant.xml will be read first.

### Servant configuration

In the Spring mode, servants-spring.xml is spring configuration file. The Tars's XSD file needs to be introduced first in the configuration file as follow:

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

Multiple spring tags are defined in Tars. When configuring servant, you need to define servant as a bean, and then define it as servant by  the Tars tag.

```xml
<tars:servant name="HelloObj" interface="com.qq.test.GreeterServant" ref="greeterServantImp"/>
<bean id="greeterServantImp" class="com.qq.test.impl.GreeterServantImp" />
```

The servant tag specifies the name of the servant through the name. Ref specifies the corresponding bean name, and interface specifies the corresponding interface name. Of course, you can also get bean by adding a @Component tag and then bean is obtained by spring automatic scanning, as well as configuring a corresponding bean name：

```xml
<context:component-scan base-package="com.qq.tars.test"/>
<tars:servant name="HelloObj" interface="com.qq.test.GreeterServant" ref="greeterServantImp"/>
```

### Listener configuration

The configuration of listener is the same as that of the servant configuration, and you need to define your Listener as bean, and then the corresponding bean is specified as Listener through the Tars tag.

```xml
<tars:listener ref="startListener"/>
<bean id="startListener" class="com.qq.test.ServiceStartListener" />
```
## 1.3.0 Version update Guide

If you need new functions like tars-spring, you need to upgrade the tars to the 1.3.0 version and the above version. As the change is relatively large, the version update guide is as follows:
1. The management platform needs to be recompiled and upgraded.
2. Tars-node needs to be upgraded to a new version.
3. Modifying the classpath configuration item of the tars.tarsjava.default template in the template management to the following code:
> classpath=${basepath}/conf:${basepath}/WEB-INF/classes:${basepath}/WEB-INF/lib
4. When building a tars-java project, servants.xml needs to be placed under the resources directory.
