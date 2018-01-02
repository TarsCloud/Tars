# Tars框架Protobuf支持依赖

## 使用方法
安装Tars-java后，在pom.xml中添加tars-proto依赖:
```xml
<dependency>
      <groupId>qq-cloud-central</groupId>
      <artifactId>tars-protobuf</artifactId>
      <version>1.0.1</version>
</dependency>
```
然后使用java-protobuf-plugin插件配合pb生成代码，详细使用方式参见
> plugins/java-protobuf-plugin