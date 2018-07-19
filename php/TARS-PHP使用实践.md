# TARS-PHP使用实践

## 综合资料
- 源码 https://github.com/Tencent/Tars
- 目前支持的开发语言 C++ 、Java、Nodejs、PHP
- 详细介绍以及架构https://github.com/Tencent/Tars/blob/master/Introduction.md
- 问题交流QQ群：669339903


## 一、安装相关
1. 安装前请先按照此页面介绍准备好软件https://github.com/Tencent/Tars/tree/master/build
2. 建议先阅读下文件内容https://github.com/Tencent/Tars/blob/master/build/install.sh （因实际上有些软件依赖在某些脚本中会自动安装，避免重复劳动），熟悉大致的脚本安装过程
3. 参考前两部分内容，接下来按照安装说明进行安装https://github.com/Tencent/Tars/blob/master/Install.md
4. 注意：请使用服务器内网IP，不要使用127.0.0.1
5. 安装中遇到的常见问题和注意事项，可阅读https://github.com/Tencent/Tars/blob/master/Install_faq.md


### 安装环境依赖

软件 |软件要求|备注
------|--------|--------
linux内核版本:      |	2.6.18及以上版本（操作系统依赖）| centos6.9|
gcc版本:          	|   4.8.2及以上版本、glibc-devel（c++语言框架依赖）|
bison工具版本:      |	2.5及以上版本（c++语言框架依赖）|
flex工具版本:       |	2.5及以上版本（c++语言框架依赖）|
cmake版本：       	|   2.8.8及以上版本（c++语言框架依赖）|
resin版本：       	|   4.0.49及以上版本（web管理系统依赖）| 下载设置软链|
Java JDK版本：      | 	java语言框架（最低1.6），web管理系统（最低1.8）| 安装可能会报错，注意环境变量设置|
Maven版本：			|   2.2.1及以上版本（web管理系统、java语言框架依赖）|
mysql版本:          |   4.1.17及以上版本（框架运行依赖）| 注意账号和权限设置|
rapidjson版本:      |   1.0.2版本（c++语言框架依赖）| 可以不用单独安装，安装过程会自行下载|

### 安装Tars核心服务

1. 错误修正：

tars_install.sh和monitor.sh文件安装后没有的话，可手工复制文件：
从源码/cpp/build/framework/deploy/tars_install.sh、cpp/build/framework/deploy/tarsnode/util/monitor.sh复制

3. 其他按照文档说明和顺序照着执行。


### 安装web管理界面

1. 文档中“2.2. java语言框架开发环境安装”部分，构建web工程项目可直接到源码web目录下修改app.config.properties和tars.conf配置，然后直接打包即可！

2. 运行/usr/local/resin/bin/resin.sh start启动web管理界面，正常的话会看到初始化界面，默认配置好三个服务。

### 服务启动

```
service mysql start
/usr/local/app/tars/tars_install.sh
/usr/local/app/tars/tarspatch/util/init.sh
/usr/local/app/tars/tarsnode/bin/tarsnode --config=/usr/local/app/tars/tarsnode/conf/tarsnode.conf
/usr/local/resin/bin/resin.sh start

```

正常启动，将会看到如下进程：
```
[root@centos data]# ps -ef|grep tars
root       5495      1  0 11:35 pts/0    00:00:03 /usr/local/app/tars/tarsregistry/bin/tarsregistry --config=/usr/local/app/tars/tarsregistry/conf/tarsregistry.conf
root       5504      1  0 11:35 pts/0    00:00:01 /usr/local/app/tars/tarsAdminRegistry/bin/tarsAdminRegistry --config=/usr/local/app/tars/tarsAdminRegistry/conf/adminregistry.conf
root       5514      1  0 11:35 pts/0    00:00:02 /usr/local/app/tars/tarsconfig/bin/tarsconfig --config=/usr/local/app/tars/tarsconfig/conf/tarsconfig.conf
root       5523      1  0 11:35 pts/0    00:00:01 /usr/local/app/tars/tarspatch/bin/tarspatch --config=/usr/local/app/tars/tarspatch/conf/tarspatch.conf
root       5610      1  0 11:35 ?        00:00:00 rsync --address=192.168.222.132 --daemon --config=/usr/local/app/tars/tarspatch/conf/rsync.conf
root       5621      1  2 11:35 ?        00:00:18 /usr/local/app/tars/tarsnode/bin/tarsnode --config=/usr/local/app/tars/tarsnode/conf/tarsnode.conf
root       6021   5621  0 11:36 ?        00:00:05 /usr/local/app/tars/tarsnode/data/tars.tarslog/bin/tarslog --config=/usr/local/app/tars/tarsnode/data/tars.tarslog/conf/tars.tarslog.config.conf
root       6022   5621  0 11:36 ?        00:00:05 /usr/local/app/tars/tarsnode/data/tars.tarsnotify/bin/tarsnotify --config=/usr/local/app/tars/tarsnode/data/tars.tarsnotify/conf/tars.tarsnotify.config.conf
root       6054   5621  0 11:36 ?        00:00:03 /usr/local/app/tars/tarsnode/data/tars.tarsqueryproperty/bin/tarsqueryproperty --config=/usr/local/app/tars/tarsnode/data/tars.tarsqueryproperty/conf/tars.tarsqueryproperty.config.conf
root       6108   5621  0 11:36 ?        00:00:03 /usr/local/app/tars/tarsnode/data/tars.tarsquerystat/bin/tarsquerystat --config=/usr/local/app/tars/tarsnode/data/tars.tarsquerystat/conf/tars.tarsquerystat.config.conf
root       6163   5621  0 11:37 ?        00:00:04 /usr/local/app/tars/tarsnode/data/tars.tarsstat/bin/tarsstat --config=/usr/local/app/tars/tarsnode/data/tars.tarsstat/conf/tars.tarsstat.config.conf
root       7097   5621  0 11:42 ?        00:00:01 /usr/local/app/tars/tarsnode/data/tars.tarsproperty/bin/tarsproperty --config=/usr/local/app/tars/tarsnode/data/tars.tarsproperty/conf/tars.tarsproperty.config.conf
```




### 安装基础服务

将安装文档中“4.1. 框架基础服务打包”相关基础包打包然后通过管理界面配置和上传发布

发布时需要在web管理的“运维管理模块”配置，具体参考“4.4. 安装框架普通基础服务”部分



### 其他
- php语言相关 建议安装php7+swoole2
- php需要安装扩展，请在 https://github.com/Tencent/Tars/tree/master/php/tars-extension 源码目录进行编译，然后将扩展加入到php.ini中



## 二、Tars使用相关

### tars协议
tars协议采用接口描述语言（Interface description language，缩写IDL）来实现，它是一种二进制、可扩展、代码自动生成、支持多平台的协议，使得在不同平台上运行的对象和用不同语言编写的程序可以用PRC远程调用的方式相互通信交流， 主要应用在后台服务之间的网络传输协议，以及对象的序列化和反序列化等方面。

协议支持的类型分两种，基本类型和复杂类型。

基本类型包括：void、bool、byte、short、int、long、float、double、string、unsigned byte、unsigned short、unsigned int；

复杂类型包括：enum、const、struct、vector、map，以及struct、vector、map的嵌套。

### 开发和打包
不同语言在开发上存在差异，但基本思路一致：
1. 编写tars文件xx.tars；
  比如
```
module MTT
{
	enum MYE1
	{
		EM_CAT,
		EM_DOG,
	};
	struct A
	{
		0 require int b;
	};
    struct HelloWorld
    {
        0 require int nId;
        1 require string sMsg;
        2 require vector<string> vNews;
		///4 optional string sNew="client new";
		5 require map<int,int> mAddr;
		6 require A a;
    };

	struct HelloPerson
	{
		0 require int nId;
		1 require string sName;
		2 optional vector<string> vFavor;
		3 optional MYE1 ePet;
		///4 optional byte aPetAge[5];
		5 optional map<int,string> mAddr;
		6 optional string sPhone;
		7 optional bool bMan;
		9 optional HelloWorld shello;
	};

    interface Hello
    {
        int testHello(string s,vector<short> vsh,out string r);
		int testPerson(HelloPerson stPersonIn,out HelloWorld stPersonOut);
    };
};
```

2. 利用tars官方转换工具根据tars文件内容生成客户端或服务端代码(在配置文件中指定生成客户端还是服务端，注意命名规范)；

3. 完善代码逻辑（完形填空）；
4. 打包代码；
5. 通过web管理界面添加应用信息（与tars定义保持一致）并上传以上步骤最终的打包代码并发布。
6. 代码打包文件上传位置/data/tars/patchs/tars.upload/



### 发布
代码打包同时会执行发布过程，还可以在服务管理列表中重启或关闭处理。

打包程序上传后，会放置在应用服务器的/usr/local/app/tars/tarsnode/data目录下，——应用名+服务名，服务名PHPTest，服务名PHPHttpServer则会将代码解压放置在PHPTest.PHPHttpServer目录下

每个完整的目录下会放置bin、conf、data文件夹，基本作用为：
- bin目录——放置解压后的程序文件，tars同时自动生成对应的启动命令文件和关闭命令文件 tars_start.sh、tars_stop.sh （也可以跳过web管理界面直接使用此文件开启/关闭服务）
- conf目录下，则存放此应用相关的服务模版配置文件——结合应用发布填写的配置项生成的最终配置文件；
- data目录下放置服务的缓存信息文件，以及进程id文件等


### 日志查看

日志目录位置 /usr/local/app/tars/app_log/，在此目录下的应用名称目录下找到对应的日志文件，如 /usr/local/app/tars/app_log/PHPTest/PHPHttpServer/PHPTest.PHPHttpServer.log

服务打的日志路径在/usr/local/app/tars/app_log/服务的应用名/服务的服务名/目录下，例如:/usr/local/app/tars/app_log/Test/HelloServer/

服务的可执行文件在/usr/local/app/tars/tarsnode/data/服务的应用名.服务的服务名/bin/下,例如:/usr/local/app/tars/tarsnode/data/Test.HelloServer/bin/

服务的模版配置文件在/usr/local/app/tars/tarsnode/data/服务的应用名.服务的服务名/conf/下,例如:/usr/local/app/tars/tarsnode/data/Test.HelloServer/conf/

服务的缓存信息文件在/usr/local/app/tars/tarsnode/data/服务的应用名.服务的服务名/data/下,例如:/usr/local/app/tars/tarsnode/data/Test.HelloServer/data/




## 三、Tars PHP开发相关

### PHP服务模版配置

每个Tars服务启动运行时，必须指定一个模版配置文件，在Tars web管理系统中部署的服务的模版配置由node进行组织生成，若不是在web管理系统上，则需要自己创建一个模版文件。具体https://github.com/Tencent/Tars/blob/master/docs/tars_template.md


对php开发，首先在web管理界面中->运维管理->模板管理，找到tars.tarsphp.default模板，根据实际php安装位置修改，如:
```
php=/usr/bin/php/bin/php
```

同时，将tars.tarsphp.default内容复制，新建tcp、http、timer版本的模板
相比较将tars.tarsphp.default,http模板，差异为：

http模板在server节点增加：
```
protocolName=http
type=http
```

TCP模板在server节点增加：
```
package_length_type=N
open_length_check=1
package_length_offset=0
package_body_offset=0
package_max_length=2000000
protocolName=tars
type=tcp
```

timer模板在server节点增加：
```
protocolName=http
type=http
isTimer=1
```

注：
- 父模板名均选择tars.default即可
- 文档中protocolName、type的说明缺失，实际使用中发现会报错，保险起见按照以上说明配置
