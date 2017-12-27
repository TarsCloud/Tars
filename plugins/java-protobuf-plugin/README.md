tars Java Codegen Plugin for Protobuf
==============================================

插件可以和`protoc`一起生成`.proto`文件定义的接口；

需要自己编译；

**插件编译需要依赖Protobuf**

```shell
export PROTOBUF_HOME='protobuf安装目录'
export CXXFLAGS='-I$PROTOBUF_HOME/src" LDFLAGS="-L$PROTOBUF_HOME/src/.libs'
```

## System requirement

* Linux、 Mac OS X with Clang
* Java 7 or up
* [Protobuf](https://github.com/google/protobuf) 3.0.0-beta-3 or up

## Compiling and testing the codegen
[protobuf](https://github.com/google/protobuf)的编译

```shell
git clone https://github.com/google/protobuf.git
./autogen.sh
./configure
make
export PROTOBUF_HOME=`pwd`
```

插件的编译

```shell
git clone git@github.com:Tencent/Tars.git
cd Tars/plugins/java-protobuf-plugin
export CXXFLAGS="-I$PROTOBUF_HOME/src" LDFLAGS="-L$PROTOBUF_HOME/src/.libs"
./gradlew java_pluginExecutable
```

## Usage

### Mac

#### 单文件

```shell
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$PROTOBUF_HOME/src/.libs
cd usage/single
$PROTOBUF_HOME/src/.libs/protoc --plugin=protoc-gen-tars-java=../../build/exe/java_plugin/protoc-gen-tars-java --tars-java_out=./ --java_out=./ flight.proto
```

查看生成结果:

```shell
➜  single git:(master) ✗ ll com/iflytek/grpc/flight 
total 96K
-rw-r--r-- 1 scguo staff  79K Oct 10 14:30 Flight.java
-rw-r--r-- 1 scguo staff  749 Oct 10 14:30 FlightServicePrx.java
-rw-r--r-- 1 scguo staff  397 Oct 10 14:30 FlightServicePrxCallback.java
-rw-r--r-- 1 scguo staff  490 Oct 10 14:30 FlightServiceServant.java
-rw-r--r-- 1 scguo staff 1.8K Oct 10 14:30 ProtoCodec.java
```

#### 多文件

```shell
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$PROTOBUF_HOME/src/.libs
cd usage/multi
$PROTOBUF_HOME/src/.libs/protoc --java_out=./ base.proto
$PROTOBUF_HOME/src/.libs/protoc --plugin=protoc-gen-tars-java=../../build/exe/java_plugin/protoc-gen-tars-java --tars-java_out=./ --java_out=./ service.proto 
```

查看生成结果:

```shell
➜  multi git:(master) ✗ ll com/iflytek/grpc/flight/dto 
total 68K
-rw-r--r-- 1 scguo staff 66K Oct 10 14:42 Base.java
➜  multi git:(master) ✗ ll com/iflytek/grpc/flight/service 
total 32K
-rw-r--r-- 1 scguo staff  769 Oct 10 14:43 FlightServicePrx.java
-rw-r--r-- 1 scguo staff  409 Oct 10 14:43 FlightServicePrxCallback.java
-rw-r--r-- 1 scguo staff  506 Oct 10 14:43 FlightServiceServant.java
-rw-r--r-- 1 scguo staff 1.8K Oct 10 14:43 ProtoCodec.java
```

### Linux

#### 单文件

```shell
export LD_LIBRARY_PATH="$PROTOBUF_HOME/src/.libs"
cd usage/single
$PROTOBUF_HOME/src/.libs/protoc --plugin=protoc-gen-tars-java=../../build/exe/java_plugin/protoc-gen-tars-java --tars-java_out=./ flight.proto
```

产看生成的结果:

```shell
H0045170 : ➜  single  ll com/iflytek/grpc/flight/
总用量 96
-rw-r--r-- 1 root root 81040 10月 10 14:47 Flight.java
-rw-r--r-- 1 root root   397 10月 10 14:47 FlightServicePrxCallback.java
-rw-r--r-- 1 root root   749 10月 10 14:47 FlightServicePrx.java
-rw-r--r-- 1 root root   490 10月 10 14:47 FlightServiceServant.java
-rw-r--r-- 1 root root  1820 10月 10 14:47 ProtoCodec.java
```

#### 多文件

```shell
export LD_LIBRARY_PATH="$PROTOBUF_HOME/src/.libs"
cd usage/multi
$PROTOBUF_HOME/src/.libs/protoc --java_out=./ base.proto
$PROTOBUF_HOME/src/.libs/protoc --plugin=protoc-gen-tars-java=../../build/exe/java_plugin/protoc-gen-tars-java --tars-java_out=./ service.proto
```

查看生成结果:

```shell
H0045170 : ➜  multi  ll com/iflytek/grpc/flight/dto
总用量 68
-rw-r--r-- 1 root root 67409 10月 10 14:48 Base.java
H0045170 : ➜  multi  ll com/iflytek/grpc/flight/service
总用量 32
-rw-r--r-- 1 root root   409 10月 10 14:49 FlightServicePrxCallback.java
-rw-r--r-- 1 root root   769 10月 10 14:49 FlightServicePrx.java
-rw-r--r-- 1 root root   506 10月 10 14:49 FlightServiceServant.java
-rw-r--r-- 1 root root  1828 10月 10 14:49 ProtoCodec.java
```

## 可能的问题

1、编译找不到-lstdc++

```shell
/bin/ld: cannot find -lstdc++
collect2: 错误：ld 返回 1
```

```shell
yum install libstdc++-static.x86_64
```