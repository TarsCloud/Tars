[View English](Deploy.zh.md)

# 目录
> * [部署简介](#chapter-1)
> * [源码编译方式部署](#chapter-2)
> * [框架Docker化部署](#chapter-3)
> * [K8s Docker部署](#chapter-4)
> * [K8s融合部署](#chapter-5)

# 1 <a id="chapter-1"></a>部署简介

需要了解Tars部署基础知识:

Tars由mysql+框架服务+业务服务组成:
- mysql, 用于存储各种服务信息
- 框架服务是由多个Tars实现好的服务(c++实现), web管理平台(nodejs)构成
- 业务服务是由开发团队根据产品需求自己实现的服务, 这些服务可以由c++, java, go, nodejs, php等语言实现

Tars完成部署后, 从服务器角度, 它由以下几部分组成:
- mysql集群(主从配置或者集群配置): 通常主从配置即可, 正常情况, 即使mysql挂了, 也不会影响业务服务的运行(但是会影响部署和发布)
- 框架服务: 是由多个c++实现的tars服务 + web管理平台组成, 通常部署在两台机器上, 其中一台会多部署web管理平台, tarspatch, tarsAdminRegistry, 运行过程中框架服务器都挂了, 也不会影响业务服务的运行(影响部署和发布)
- 多台节点服务器: 从1台至上万台, 每台节点服务器上都必须部署一个tarsnode进程, tarsnode要连接到框架服务上(连接tarsregistry)
- 业务自己实现的服务通过web管理平台发布到这些节点服务器上

Tars的部署工作包括:
- mysql的安装
- 框架服务部署(一般是两台服务器)
- 节点服务器部署(部署tarsnode), 一般可以通过web管理平台远程部署节点服务器

**注意:如果节点服务器上运行了不同语言实现的业务服务, 那么节点服务器的运行环境需要自己安装, 比如安装jdk, node等**

Tars部署方式有以下几种:
- 源码编译部署
- 框架Docker部署
- K8s Docker部署
- K8s 融合部署

# 2 <a id="chapter-2"></a>源码编译方式部署

源码部署方式是了解Tars非常好的途径, 源码部署细节方式请参见 [Install](https://github.com/TarsCloud/Tars/blob/master/Install.zh.md) 中的源码部署部分, 强烈建议阅读.

主要步骤如下:
- 安装mysql
- 下载源码(TarsFramework)
- 编译源码
- 下载管理平台(TarsWeb)
- 通过一键部署脚本完成部署

源码部署方式Tars框架服务都以独立的进程模式运行在服务器上, 可以手工启停, 每个服务都可以独立更新.

**注意:该方式建议对Tars比较熟悉的团队使用**

# 3 <a id="chapter-3"></a>框架Docker化部署

源码部署虽然可以独立更新, 但是也带来了不便, 毕竟每个模块更新还是比较麻烦的, 同时模块版本可能还有依赖, 更新维护就更麻烦了.

在这种情况下, 可以选择框架Docker化部署: 简单的说, 将框架服务以及web都docker容器化, 启动容器则框架服务都自动启动, 更新时也整体更新.

Tars框架的Docker制作也分三种模式:
- 源码制作,[参见](https://github.com/TarsCloud/Tars/blob/master/Install.zh.md) 中的docker制作部分.
- 自动制作(不包含运行环境), 具体[参见](https://github.com/TarsCloud/TarsDocker/blob/master/README.zh.md) 
- 自动制作(包含运行环境), 具体[参见](https://github.com/TarsCloud/TarsDocker/blob/master/README.zh.md) 

两者区别如下:
- 源码制作
>- 需要手工下载源码, 一步一步制作docker
>- 镜像都采用了国内镜像, 比较容易制作成功
>- 制作的镜像, 不包含运行环境, 即java, php等需要运行时的环境的业务服务, 无法发布到docker镜像内部

- 自动制作
>- 一键运行命令, 既可以完成镜像的制作
>- 采用的系统默认的源, 可能连接海外源, 较慢(但是在github上action制作的时候比较快)

# 4 <a id="chapter-4"></a>K8s Docker部署

框架Docker部署, 虽然极大方便了框架的部署, 但是对于使用k8s来管理容器的团队而言, 仍然有很多工作要做.

因此这里, 提供一种k8s上部署Tars的建议:
- 将框架服务容器化, 使用docker: tarscloud/framework 或 tarscloud/tars
- 将tarsnode节点也容器化, 使用docker: tarscloud/tars-node
- tars框架和tarsnode节点都作为pod部署在k8s上, pod运行的容器当成一台虚拟机
- 通过tars web发布服务到这些容器中运行

具体[参见](https://github.com/TarsCloud/TarsDocker/blob/master/README.zh.md) 

**注意:docker内部提供了各种语言的运行环境, 可以把业务服务发布到docker内部运行(相当于把docker当成虚拟机)**

虽然这种方式并没有把每个服务都做一个pod, 独立运行在k8s上, 但是也基本解决了tars和k8s结合的问题, 虽然不优雅, 当整体可用.

这种模式下需要解决的最核心问题是: 框架服务和tarsnode都作为pod, 可能会死掉并产生漂移, 导致ip会变化, 如何解决?

建议解决方式如下:
- 所有pod可以采用stateful headless模式部署, 每个都有独立的域名, 比如tars-0, tars-1, tarsnode-1, tarsnode-2等...
- 业务服务部署时, 业务节点都采用域名(不要采用ip)

# 5 <a id="chapter-5"></a>K8s融合部署

上面K8s的Docker部署, 虽然把Tars部署在k8s上运行起来, 但是实际发布, 扩容并没有使用K8s, 只是把k8s当成了一个容器管理平台了.

Tars和K8s的深度融合仍然在规划和开发中...
