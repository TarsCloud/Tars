[Click me switch to English version](README.md)

# Readme.md

## Tars

Tars这个名字取自于电影"星际穿越"中的机器人，它是基于名字服务使用Tars协议的高性能RPC开发框架，配套一体化的运营管理平台，并通过伸缩调度，实现运维半托管服务。

Tars是腾讯从2008年到今天一直在使用的后台逻辑层的统一应用框架TAF（Total Application Framework），目前支持C++,Java,PHP,Nodejs,Go语言。该框架为用户提供了涉及到开发、运维、以及测试的一整套解决方案，帮助一个产品或者服务快速开发、部署、测试、上线。 它集可扩展协议编解码、高性能RPC通信框架、名字路由与发现、发布监控、日志统计、配置管理等于一体，通过它可以快速用微服务的方式构建自己的稳定可靠的分布式应用，并实现完整有效的服务治理。

目前该框架在腾讯内部，各大核心业务都在使用，颇受欢迎，基于该框架部署运行的服务节点规模达到上万个。

Tars详细介绍参见 [目录](https://tarscloud.github.io/TarsDocs/SUMMARY.html)。

## 安装 

1. 如果你是新手安装Tars, 请务必阅读部署的整体的说明 [installation](https://tarscloud.github.io/TarsDocs/installation)
2. 如果你是第一次部署, 建议采用源码部署熟悉系统 [source](https://tarscloud.github.io/TarsDocs/installation/source.html)
3. 如果你比较熟悉, 可以采用docker部署 [docker](https://tarscloud.github.io/TarsDocs/installation/docker.html)


### 支持平台

目前运行的操作系统平台如下：

* Linux
* Mac(>=2.1.0 support)

### 支持语言

目前支持的开发语言如下：

* C++
* Java
* Nodejs
* PHP
* Go

### 版本管理

Tars由多种模块组成, 分散在多个仓库中, 并且基础框架版本和语言版本可以独立发展, 鉴于此, 从2.1.0版本开始, 框架的版本TAG打在TarsFramework仓库上, 不再体现在Tars这个仓库上.

另外各个组件会有自己独立的版本, 当有版本依赖说明时, 各组件会独立说明.


### License

Tars的开源协议为BSD-3-Clause，详情参见 [LICENSE](https://tarscloud.github.io/TarsDocs/license.html)。

### 联系方式

qq技术交流群群：579079160、669339903。

微信公众号：TARS星球

推特地址：[@TarsCLoud](https://twitter.com/TarsCloud)

