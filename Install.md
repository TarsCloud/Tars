[Click me switch to English version](Install.en.md)

# 目录
> * [依赖环境](#chapter-1)
> * [Tars开发环境安装介绍](#chapter-2)
> * [Tars数据库环境初始化](#chapter-3)
> * [Tars框架运行环境搭建](#chapter-4)

本安装文档仅描述了在一台服务器上安装搭建整个Tars框架的过程，目的是为了让用户对Tars框架的部署搭建、运行、测试等有个整体的认识。

如要用于线上环境，部署安装的原理是一样，不过需要更多考虑分布式系统下服务的部署需要有容错、容灾等的能力。若有需要，可以加入tars的qq技术交流群：579079160。

# 1. <a id="chapter-1"></a>依赖环境

软件 |软件要求
------|--------
linux内核版本:      |	2.6.18及以上版本（操作系统依赖）
gcc版本:          	|   4.1.2及以上版本、glibc-devel（c++语言框架依赖）
bison工具版本:      |	2.5及以上版本（c++语言框架依赖）
flex工具版本:       |	2.5及以上版本（c++语言框架依赖）
cmake版本：       	|   2.8.8及以上版本（c++语言框架依赖）
resin版本：       	|   4.0.49及以上版本（web管理系统依赖）
Java JDK版本：      | 	java语言框架（最低1.6），web管理系统（最低1.8）
Maven版本：			|   2.2.1及以上版本（web管理系统、java语言框架依赖）
mysql版本:          |   4.1.17及以上版本（框架运行依赖）
rapidjson版本:      |   1.0.2版本（c++语言框架依赖）

运行服务器要求：1台普通安装linux系统的机器即可。

## 1.1. glibc-devel安装介绍

如果没有安装glibc的开发库，需要先安装。

例如，在Centos下，执行：
```
yum install glibc-devel
```

## 1.2. cmake安装介绍
cmake是tars框架服务依赖的编译环境。

下载cmake-2.8.8源码包，解压：
```
tar zxvf cmake-2.8.8.tar.gz
```
进入目录：
```
cd cmake-2.8.8
```
进行如下操作：（选择适合自己的操作步骤）
```
./bootstrap(如果系统还没有安装CMake，源码中提供了一个 bootstrap 脚本)
make
make install(如果make install失败，一般是权限不够，切换root进行安装)
```

## 1.3. resin安装介绍
resin是Tars管理系统**推荐**的运行环境（如果没有安装java jdk，先需要安装，具体可以参见2.1章节）。
> 以resin-4.0.49，安装在/usr/local/resin下为例
```
cd /usr/local/
tar zxvf resin-4.0.49.tar.gz
ln -s resin-4.0.49 resin
```

## 1.4. mysql 安装介绍
安装前，确定系统是否安装了ncurses、zlib，若没有，可以执行：
```
yum install ncurses-devel
yum install zlib-devel
```

设置安装目录，切换至root用户
```
cd /usr/local
mkdir mysql-5.6.26
chown ${普通用户}:${普通用户} ./mysql-5.6.26
ln -s /usr/local/mysql-5.6.26 /usr/local/mysql
```

用utf8的安装方式
下载mysql源码（这里使用的是mysql-5.6.26）,用utf8的安装方式mysql，解压后编译：
```
cmake . -DCMAKE_INSTALL_PREFIX=/usr/local/mysql-5.6.26 -DWITH_INNOBASE_STORAGE_ENGINE=1 -DMYSQL_USER=mysql -DDEFAULT_CHARSET=utf8 -DDEFAULT_COLLATION=utf8_general_ci
make
make install
```
**注意，对于用Tars的c++进行开发编译的服务，mysql建议采用静态库，源码编译，避免所有服务器都要安装mysql的动态库。**


对于在服务器用Tars的c++进行开发编译服务代码而言，经过上面步骤就可以进行编译安装Tars开发框架了。

若要是搭建Tars框架的运行环境，需要以下步骤，切换至root用户，对mysql进行配置。
```shell
yum install perl
cd /usr/local/mysql
useradd mysql
rm -rf /usr/local/mysql/data
mkdir -p /data/mysql-data
ln -s /data/mysql-data /usr/local/mysql/data
chown -R mysql:mysql /data/mysql-data /usr/local/mysql/data
cp support-files/mysql.server /etc/init.d/mysql
**如果/etc/目录下有my.cnf存在，需要把这个配置删除了**
yum install -y perl-Module-Install.noarch
perl scripts/mysql_install_db --user=mysql
vim /usr/local/mysql/my.cnf
```
给一个my.cnf配置实例：

```cnf
[mysqld]

# Remove leading # and set to the amount of RAM for the most important data
# cache in MySQL. Start at 70% of total RAM for dedicated server, else 10%.
innodb_buffer_pool_size = 128M

# Remove leading # to turn on a very important data integrity option: logging
# changes to the binary log between backups.
log_bin

# These are commonly set, remove the # and set as required.
basedir = /usr/local/mysql
datadir = /usr/local/mysql/data
# port = .....
# server_id = .....
socket = /tmp/mysql.sock

bind-address={$your machine ip}

# Remove leading # to set options mainly useful for reporting servers.
# The server defaults are faster for transactions and fast SELECTs.
# Adjust sizes as needed, experiment to find the optimal values.
join_buffer_size = 128M
sort_buffer_size = 2M
read_rnd_buffer_size = 2M

sql_mode=NO_ENGINE_SUBSTITUTION,STRICT_TRANS_TABLES

```
**注意将bind-address改为部署机器的IP**

启动mysql
```
service mysql start
chkconfig mysql on
```
结束mysql
```
service mysql stop
```
添加mysql的bin路径
```
vim /etc/profile
PATH=$PATH:/usr/local/mysql/bin
export PATH
```
修改root密码(采用root密码)
```
./bin/mysqladmin -u root password 'root@appinside'
./bin/mysqladmin -u root -h ${主机名} password 'root@appinside'
```
**注意${主机名}需要修改成自身机器的名称，可以通过查看/etc/hosts**


添加mysql库路径
```
vim /etc/ld.so.conf
/usr/local/mysql/lib/
ldconfig
```
========================

mysql主从配置可以参考网上教程

master赋予权限:
```
GRANT REPLICATION SLAVE ON *.* to 'mysql-sync'@'%' identified by 'sync@appinside'
```
slave设置主备同步项
```
change master to master_host='${备机Ip}',master_user='mysql-sync',master_password='sync@appinside' ,master_log_file='iZ94orl0ix4Z-bin.000004',master_log_pos=611;
stop slave
start slave
show master status\G;
show slave status\G;
```
**注意${备机Ip}需要修改成备机数据库的Ip**

# 2. <a id="chapter-2"></a>Tars开发环境安装介绍
## 2.1. web管理系统开发环境安装
以linux环境为例：

下载java jdk，解压安装

配置环境环境
```
vim /etc/profile
```
添加如下内容：
```
export JAVA_HOME=${jdk source dir}
CLASSPATH=$JAVA_HOME/lib/dt.jar:$JAVA_HOME/lib/tools.jar
PATH=$JAVA_HOME/bin:$PATH
export PATH JAVA_HOME CLASSPATH
```
执行命令
```
source /etc/profile
```
测试
```
java -version
```

maven安装
下载maven，解压安装
配置环境变量
```
vim /etc/profile
```
添加如下内容：
```
export MAVEN_HOME=${maven source dir}
export PATH=$PATH:$MAVEN_HOME/bin
```
执行命令
```
source /etc/profile
```
测试
```
mvn -v
```
**注意，如有需要，修改本地仓库的位置和maven的源**

## 2.2. java语言框架开发环境安装

java jdk和maven环境配置与上面web管理系统的类似

**注意，java框架安装前，需要保证机器能联网，同时修改maven安装目录下的conf目录的setting.xml，尽量使用国内的maven镜像，具体操作可以网上查看**

下载tars源码，进入java源码目录，并安装在本地仓库
```
mvn clean install 
mvn clean install -f core/client.pom.xml 
mvn clean install -f core/server.pom.xml

```
构建web工程项目
通过IDE或者maven创建一个maven web项目，
这里以eclipse为例，File -> New -> Project -> Maven Project -> maven-archetype-webapp，再输入groupId、artifactId，生成完成之后可以通过eclipse进行导入，目录结构如下

![tars](docs/images/tars_java_evn1.png)


增加Maven依赖配置
使用Tars框架时，需要依赖框架提供的jar包依赖，以及工具插件。在工程项目pom.xml文件中增加如下依赖配置。
框架依赖配置

![tars](docs/images/tars_java_evn2.png)

插件依赖配置

![tars](docs/images/tars_java_evn3.png)

## 2.3. c++ 开发环境安装
下载tars源码，首先进入cpp/thirdparty目录，执行thirdparty.sh脚本，下载依赖的rapidjson

然后进入cpp/build源码目录
```
cd {$source_folder}/cpp/build
chmod u+x build.sh
./build.sh all
```

**编译时默认使用的mysql开发库路径：include的路径为/usr/local/mysql/include，lib的路径为/usr/local/mysql/lib/，若mysql开发库的安装路径不在默认路径，则需要修改build目录下CMakeLists.txt文件中的mysql相关的路径，再编译**

如果需要重新编译
```
./build.sh cleanall
./build.sh all
```

切换至root用户，创建安装目录
```
cd /usr/local
mkdir tars
chown ${普通用户}:${普通用户} ./tars/
```

安装
```
cd {$source_folder}/cpp/build
./build.sh install或者make install
```
**默认的安装路径为/usr/local/tars/cpp。**

**如要修改安装路径：**
```
**需要修改build目录下CMakeLists.txt文件中的安装路径。**
**需要修改servant/makefile/makefile.tars文件中的TARS_PATH的路径**
**需要修改servant/script/create_tars_server.sh文件中的DEMO_PATH的路径**
```

# 3. <a id="chapter-3"></a>Tars数据库环境初始化
## 3.1. 添加用户
```sql
grant all on *.* to 'tars'@'%' identified by 'tars2015' with grant option;
grant all on *.* to 'tars'@'localhost' identified by 'tars2015' with grant option;
grant all on *.* to 'tars'@'${主机名}' identified by 'tars2015' with grant option;
flush privileges;
```
**注意${主机名}需要修改成自身机器的名称，可以通过查看/etc/hosts

## 3.2. 创建数据库
sql脚本在cpp/framework/sql目录下，修改部署的ip信息
```
sed -i "s/192.168.2.131/${your machine ip}/g" `grep 192.168.2.131 -rl ./*`
sed -i "s/db.tars.com/${your machine ip}/g" `grep db.tars.com -rl ./*`
```
**注意，192.168.2.131这个ip是tars开发团队当时部署服务测试的ip信息，替换成自己数据库的部署地址即可,不要是127.0.0.1**

**注意，db.tars.com是tars框架数据库部署的地址信息，替换成自己数据库的部署地址即可**

执行.
```
chmod u+x exec-sql.sh
./exec-sql.sh
```
**注意将${your machine ip}改为部署机器的IP**

**如果exec-sql.sh脚本执行出错，需要脚本里修改数据库用户名root对应的密码**

脚本执行后，会创建3个数据库，分别是db_tars、tars_stat、tars_property。

其中db_tars是框架运行依赖的核心数据库，里面包括了服务部署信息、服务模版信息、服务配置信息等等；

tars_stat是服务监控数据存储的数据库；

tars_property是服务属性监控数据存储的数据库；

# 4. <a id="chapter-4"></a>Tars框架运行环境搭建

## 4.1. 框架基础服务打包

框架服务的安装分两种：

一种是核心基础服务(必须的)，必须手工部署的，

一种是普通基础服务(可选的)，可以通过管理平台发布的(和普通服务一样）。

```
手工部署的核心基础服务：tarsAdminRegistry, tarsregistry, tarsnode, tarsconfig, tarspatch
 
通过管理平台部署的普通基础服务：tarsstat, tarsproperty,tarsnotify, tarslog，tarsquerystat，tarsqueryproperty
```

首先准备第一种服务的安装包，在cpp/build/目录下输入：
```
make framework-tar
```
会在当前目录生成framework.tgz 包
这个包包含了 tarsAdminRegistry, tarsregistry, tarsnode, tarsconfig, tarspatch 部署相关的文件

第二种服务安装包可以单独准备：
```
make tarsstat-tar
make tarsnotify-tar
make tarsproperty-tar
make tarslog-tar
make tarsquerystat-tar
make tarsqueryproperty-tar
```
生成的发布包，在管理平台部署发布完成后，进行部署发布，具体参见4.4章节。

**注意在管理平台进行部署时，选择正确的服务模板即可（默认是有的，若没有相应的模版，可以在管理平台上创建，具体服务的模版内容可以tars_template.md）!**

## 4.2. 安装框架核心基础服务

## 4.2.1. 安装核心基础服务

切换至root用户，创建基础服务的部署目录，如下：
```  shell
cd /usr/local/app
mkdir tars
chown ${普通用户}:${普通用户} ./tars/
```

将已打好的框架服务包复制到/usr/local/app/tars/，然后解压，如下：
``` shell
cp build/framework.tgz /usr/local/app/tars/
cd /usr/local/app/tars
tar xzfv framework.tgz
```

修改各个服务对应conf目录下配置文件，注意将配置文件中的ip地址修改为本机ip地址，如下：
``` shell
cd /usr/local/app/tars
sed -i "s/192.168.2.131/${your_machine_ip}/g" `grep 192.168.2.131 -rl ./*`
sed -i "s/db.tars.com/${your_machine_ip}/g" `grep db.tars.com -rl ./*`
sed -i "s/registry.tars.com/${your_machine_ip}/g" `grep registry.tars.com -rl ./*`
sed -i "s/web.tars.com/${your_machine_ip}/g" `grep web.tars.com -rl ./*`
```
**注意，192.168.2.131这个ip是tars开发团队当时部署服务测试的ip信息，替换成本机的部署地址即可，不要是127.0.0.1**

**注意，db.tars.com是tars框架数据库部署的地址信息，替换成自己数据库的部署地址即可**

**注意，registry.tars.com是tars框架主控tarsregistry服务部署的地址信息，替换成自己主控tarsregistry符的部署地址即可**

**注意，web.tars.com是rsync使用的地址信息，替换成自己的部署机器地址即可**

然后在/usr/local/app/tars/目录下，执行脚本，启动tars框架服务
```
chmod u+x tars_install.sh
./tars_install.sh
```
**注意如果几个服务不是部署在同一台服务器上，需要自己手工copy以及处理tars_install.sh脚本**

部署管理平台并启动web管理平台(管理平台必须和tarspatch部署在同一台服务器上)部署tarspatch，切换至root用户，并执行
```
tarspatch/util/init.sh
```
**注意，上面脚本执行后，看看rsync进程是否起来了，若没有看看rsync使用的配置中的ip是否正确（即把web.tars.com替换成本机ip）

在管理平台上面配置tarspatch，注意需要配置服务的可执行目录(/usr/local/app/tars/tarspatch/bin/tarspatch)

在管理平台上面配置tarsconfig，注意需要配置服务的可执行目录(/usr/local/app/tars/tarsconfig/bin/tarsconfig)

tarsnode需要配置监控，避免不小心挂了以后会启动，需要在crontab里面配置
```
* * * * * /usr/local/app/tars/tarsnode/util/monitor.sh
```

## 4.2.2. 服务扩容前安装tarsnode

核心基础服务的安装成功后，如果需要在其他机器也能部署基于tars框架的服务，那么在管理平台扩容部署前，需要安装tarsnode服务。

如果只是在一台机器部署服务进行测试，这一步可以先忽略，等到需要扩容时再执行。

具体步骤跟上一节很像，如下：

切换至root用户，创建基础服务的部署目录，如下：
```  shell
cd /usr/local/app
mkdir tars
chown ${普通用户}:${普通用户} ./tars/
```

将已打好的框架服务包复制到/usr/local/app/tars/，然后解压，如下：
``` shell
cp build/framework.tgz /usr/local/app/tars/
cd /usr/local/app/tars
tar xzfv framework.tgz
```

修改各个服务对应conf目录下配置文件，注意将配置文件中的ip地址修改为本机ip地址，如下：
``` shell
cd /usr/local/app/tars
sed -i "s/192.168.2.131/${your_machine_ip}/g" `grep 192.168.2.131 -rl ./*`
sed -i "s/db.tars.com/${your_machine_ip}/g" `grep db.tars.com -rl ./*`
sed -i "s/registry.tars.com/${your_machine_ip}/g" `grep registry.tars.com -rl ./*`
sed -i "s/web.tars.com/${your_machine_ip}/g" `grep web.tars.com -rl ./*`
```
**注意，192.168.2.131这个ip是tars开发团队当时测试的ip信息，替换成自己扩容机器的ip地址即可，不要是127.0.0.1**

**注意，db.tars.com是tars框架数据库部署的地址信息，替换成数据库的部署ip地址即可**

**注意，registry.tars.com是tars框架主控tarsregistry服务部署的地址信息，替换成自己主控tarsregistry的部署地址即可**

**注意，web.tars.com是rsync使用的地址信息，替换成web的部署机器地址即可**

然后在/usr/local/app/tars/目录下，执行脚本，启动tars框架服务
```
chmod u+x tarsnode_install.sh
./tarsnode_install.sh
```

配置监控，避免不小心挂了以后会启动，需要在crontab里面配置
```
* * * * * /usr/local/app/tars/tarsnode/util/monitor.sh
```

## 4.3. 安装web管理系统

>**执行之前先安装tars java语言框架到本地仓库，参见2.2章节**
>管理系统源代码目录名称为**web**

修改配置文件，文件存放的路径在web/src/main/resources/目录下。
- app.config.properties  
  修改为实际的数据库配置

```ini
# 数据库(db_tars)
tarsweb.datasource.tars.addr={$your_db_ip}:3306
tarsweb.datasource.tars.user=tars
tarsweb.datasource.tars.pswd=tars2015

# 发布包存储路径
upload.tgz.path=/usr/local/app/patchs/tars.upload/
```

- tars.conf  
  替换registry1.tars.com，registry2.tars.com为实际IP。可以只配置一个地址，多个地址用冒号“:”连接

```xml
<tars>
    <application>
        #proxy需要的配置
        <client>
            #地址
            locator = tars.tarsregistry.QueryObj@tcp -h registry1.tars.com -p 17890:tars.tarsregistry.QueryObj@tcp -h registry2.tars.com -p 17890
            sync-invoke-timeout = 30000
            #最大超时时间(毫秒)
            max-invoke-timeout = 30000
            #刷新端口时间间隔(毫秒)
            refresh-endpoint-interval = 60000
            #模块间调用[可选]
            stat = tars.tarsstat.StatObj
            #网络异步回调线程个数
            asyncthread = 3
            modulename = tars.system
        </client>
    </application>
</tars>
```

打包，在web目录下执行命令，会在web/target目录下生成tars.war
```
mvn clean package
```

web发布
将tars.war放置到/usr/local/resin/webapps/中
```
cp ./target/tars.war /usr/local/resin/webapps/
```

创建日志目录
```
mkdir -p /data/log/tars
```

修改Resin安装目录下的conf/resin.xml配置文件
将默认的配置
```xml
<host id="" root-directory=".">
    <web-app id="/" root-directory="webapps/ROOT"/>
</host>
```
修改为
```xml
<host id="" root-directory=".">
    <web-app id="/" document-directory="webapps/tars"/>
</host>
```

启动resin
/usr/local/resin/bin/resin.sh start

访问站点
浏览器输入${your machine ip}:8080，即可看到，如下：

![tars](docs/images/tars_web_system_index.png)

## 4.4. 安装框架普通基础服务
**平台部署的端口号仅供参考，保证端口无冲突即可**
### 4.4.1 tarsnotify部署发布

默认tarsnotify在安装核心基础服务时，部署信息已初始化了，安装完管理平台后，就可以看到，如下：

![tars](docs/images/tars_tarsnotify_bushu.png)

上传发布包后，发布如下：

![tars](docs/images/tars_tarsnotify_patch.png)

### 4.4.2 tarsstat部署发布

部署信息如下：

![tars](docs/images/tars_tarsstat_bushu.png)

上传发布包后，发布如下：

![tars](docs/images/tars_tarsstat_patch.png)

### 4.4.3 tarsproperty部署发布

部署信息如下：

![tars](docs/images/tars_tarsproperty_bushu.png)

上传发布包后，发布如下：

![tars](docs/images/tars_tarsproperty_patch.png)

### 4.4.4 tarslog部署发布

部署信息如下：

![tars](docs/images/tars_tarslog_bushu.png)

上传发布包后，发布如下：

![tars](docs/images/tars_tarslog_patch.png)

### 4.4.5 tarsquerystat部署发布

部署信息如下：

![tars](docs/images/tars_tarsquerystat_bushu.png)

**注意在服务部署时，选择非Tars协议，因为Web是以json协议来获取服务监控的数据的**

上传发布包后，发布如下：

![tars](docs/images/tars_tarsquerystat_patch.png)

### 4.4.5 tarsqueryproperty部署发布

部署信息如下：

![tars](docs/images/tars_tarsqueryproperty_bushu.png)

**注意在服务部署时，选择非Tars协议，因为Web是以json协议来获取属性监控的数据的**

上传发布包后，发布如下：

![tars](docs/images/tars_tarsqueryproperty_patch.png)
