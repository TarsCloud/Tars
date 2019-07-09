[Click me switch to English version](README.md)

# Tars

Tars这个名字取自于电影"星际穿越"中的机器人，它是基于名字服务使用Tars协议的高性能RPC开发框架，配套一体化的运营管理平台，并通过伸缩调度，实现运维半托管服务。

Tars是腾讯从2008年到今天一直在使用的后台逻辑层的统一应用框架TAF（Total Application Framework），目前支持C++,Java,PHP,Nodejs,Go语言。该框架为用户提供了涉及到开发、运维、以及测试的一整套解决方案，帮助一个产品或者服务快速开发、部署、测试、上线。
它集可扩展协议编解码、高性能RPC通信框架、名字路由与发现、发布监控、日志统计、配置管理等于一体，通过它可以快速用微服务的方式构建自己的稳定可靠的分布式应用，并实现完整有效的服务治理。

目前该框架在腾讯内部，各大核心业务都在使用，颇受欢迎，基于该框架部署运行的服务节点规模达到上万个。

Tars详细介绍参见[Introduction.md](Introduction.md)。

## 支持平台

目前运行的操作系统平台如下：

- Linux

## 支持语言

目前支持的开发语言如下：

- C++
- Java
- Nodejs
- PHP
- Go

## 安装说明

1. 初次接触时，建议参考安装说明文档手工[Install.md](Install.md)进行安装。
2. 对安装比较熟后，可以参考[deploy](deploy)目录下的python脚本进行安装（必要时需要修改）。
3. 也可以选择采用Docker镜像安装的方式进行快捷安装，详细安装说明见[TarsDocker仓库](https://github.com/TarsCloud/TarsDocker)。

## 性能数据

参见[ docs/tars_performce.md](docs/tars_performce.md)。

## 子项目

| 目录名称  | 功能                          |
| --------- | ----------------------------- |
| framework | C++语言框架基础服务的源码实现 |
| cpp       | C++语言框架rpc的源码实现      |
| java      | java语言框架rpc的源码实现     |
| go        | go语言框架rpc的源码实现       |
| nodejs    | nodejs语言框架rpc的源码实现   |
| php       | php语言框架rpc的源码实现      |
| tup       | tup组包协议各个语言的源码实现 |
| web       | 管理tars的web源码实现         |

## 开发技术文档

参见[docs](docs)目录。

## License

Tars的开源协议为BSD-3-Clause，详情参见[LICENSE.TXT](LICENSE.TXT)。

## 联系方式

qq技术交流群1群：579079160（已满）。

qq技术交流群2群：669339903。

微信号：TARS01

