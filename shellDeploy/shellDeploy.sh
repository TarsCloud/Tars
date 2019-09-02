#!/bin/bash

##入参MachineIP为本机的IP Address
##Parameter IP Address is local IP Address
MachineIp=$1

##CodePath 为代码下载和编译路径
##CodePath is path for code download and compilation
CodePath=/usr/local/tarscode

##MysqlDefaultPassword为Tars使用的默认密码, 初次安装不建议修改。修改该密码，涉及到很多配置改动。
##MysqlDefaultPassword is the initial password for mysql opertation. Would suggest keep default password.
##Some configurations are required to be modified once MysqlDefaultPassword is changed.   
MysqlDefaultPassword=tars2015

##设置时区为北京时间，避免云主机位于不同时区引发的错误
##Set Beijing Time
TZ='Asia/Shanghai'; 
export TZ

##显示脚本的运行时间
##Display the runtime 
time=$(date "+%Y%m%d-%H%M%S")
echo "Deploy Script Run Time（Beijing Time） : ${time}">>$CodePath/Tars/shellDeploy/deploy_log

touch $CodePath/Tars/shellDeploy/deploy_log

##检查是否输入IP地址，如果没有输入，则退出脚本
##check whether IP address is inputed, exit if not
if [ ! -n "$1" ] ;then
    echo "NO Local IP address Input, please input IP address.">>$CodePath/Tars/shellDeploy/deploy_log
    exit
else
    echo "Input Local IP address is $MachineIp">>$CodePath/Tars/shellDeploy/deploy_log
fi

## 安装部署过程中需要的软件
## install basic tools
# if false;then
yum install -y glibc-devel
yum install -y flex
yum install -y bison
yum install -y cmake
yum install -y ncurses-devel
yum install -y zlib-devel
yum install -y perl
yum install -y wget
yum install -y net-tools
yum install -y gcc
yum install -y gcc-c++
yum install -y flex
yum install -y make
yum install -y git
yum install -y expect
yum install -y tar
yum install -y epel-release
yum install -y nodejs
yum install -y npm
#npm install -g n
npm i -g pm2
yum install -y lrzsz
echo "Finish basic tool installation">>$CodePath/Tars/shellDeploy/deploy_log


## 安装mysql 5.7
## install mysql 5.7
wget https://repo.mysql.com//mysql80-community-release-el7-3.noarch.rpm
rpm -ivh mysql80-community-release-el7-3.noarch.rpm
yum-config-manager --disable mysql80-community
yum-config-manager --enable mysql57-community
yum install -y mysql-community-server
yum install -y mysql-devel
echo "Finish mysql5.7 installation">>$CodePath/Tars/shellDeploy/deploy_log

#disable password for mysql
#echo "# disable password">>/etc/my.cnf
#echo "skip-grant-tables">>/etc/my.cnf
#echo "disable password for mysql">>$CodePath/Tars/shellDeploy/deploy_log

## 启动mysql,并设置为开机自启动
## start mysql
systemctl start mysqld.service
systemctl enable mysqld.service
systemctl status mysqld.service
echo "start mysql5.7 ...">>$CodePath/Tars/shellDeploy/deploy_log


## 安装nvm
## install nvm
wget -qO- https://raw.githubusercontent.com/creationix/nvm/v0.33.11/install.sh | bash
source ~/.bashrc
nvm install v8.11.3
npm install -g pm2 --registry=https://registry.npm.taobao.org
echo "install nvm && pm2">>$CodePath/Tars/shellDeploy/deploy_log


## 编译Tars框架代码
## compile Tars framework
cd $CodePath/Tars/framework
## 修改Cmake中mysql配置参数，确保编译通过
## modify the parameter in the CMake, make sure the compile run successfully
find . -name 'CMakeLists.txt' | xargs perl -pi -e 's|/usr/local/mysql/include|/usr/include/mysql|g'
find . -name 'CMakeLists.txt' | xargs perl -pi -e 's|/usr/local/mysql/lib|/usr/lib64/mysql|g'
cd $CodePath/Tars/framework/tarscpp
find . -name 'CMakeLists.txt' | xargs perl -pi -e 's|/usr/local/mysql/include|/usr/include/mysql|g'
find . -name 'CMakeLists.txt' | xargs perl -pi -e 's|/usr/local/mysql/lib|/usr/lib64/mysql|g'
cd $CodePath/Tars/framework/build
./build.sh all
echo "Finish framework compilation">>$CodePath/Tars/shellDeploy/deploy_log


## Tars安装
## Install Tars
cd /usr/local/
mkdir tars
chown root:root ./tars/
cd $CodePath/Tars/framework/build
./build.sh install
echo "Finish framework installation">>$CodePath/Tars/shellDeploy/deploy_log
#fi

##配置Mysql参数
##Config Mysql 
cd $CodePath/Tars/shellDeploy
##安装过成，需要cp mysqlConfig.sh到目标路径，先检查是否已经存在，如果已经存在，先删除，再cp
##check whether mysqlConfig file exist
checkfile=$CodePath/Tars/framework/sql/mysqlConfig.sh
if [  -f "$checkfile" ]; then 
rm -rf $CodePath/Tars/framework/sql/mysqlConfig.sh
echo "delete previous file, copy an new file">>$CodePath/Tars/shellDeploy/deploy_log
fi 

cp $CodePath/Tars/shellDeploy/mysqlConfig.sh  $CodePath/Tars/framework/sql 

cd $CodePath/Tars/framework/sql

##替换本机IP地址
##Replace the previous configureation parameter with local IP address
sed -i "s/192.168.2.131/$MachineIp/g" `grep 192.168.2.131 -rl ./*`
sed -i "s/db.tars.com/$MachineIp/g" `grep db.tars.com -rl ./*`
sed -i "s/10.120.129.226/$MachineIp/g" `grep 10.120.129.226 -rl ./*`

## 登入mysql, 配置新的password,参数，数据库以及表项
## login mysql, configure new password, parameters,database as well as table
var=`cat /var/log/mysqld.log |grep "temporary password"`
echo $var>>$CodePath/Tars/shellDeploy/deploy_log
TempPassword=${var#*root@localhost:}
echo "Temp Password:"$TempPassword>>$CodePath/Tars/shellDeploy/deploy_log

#TempPassword=`cat /var/log/mysqld.log |grep "temporary password" |awk -F ":" '{print $NF}'`
#echo "Temp Password:" $TempPassword>>$CodePath/Tars/shellDeploy/deploy_log
./mysqlConfig.sh root $TempPassword
if [ $? -ne 0 ]; then
    echo "config mysql database for tars framework failed">>$CodePath/Tars/shellDeploy/deploy_log
    exit
else
    echo "config mysql database for tars framework successfully">>$CodePath/Tars/shellDeploy/deploy_log
fi


##安装核心基础服务
##install the service of tars framework
cd $CodePath/Tars/framework/build
make framework-tar
cd /usr/local
mkdir app
cd /usr/local/app/
mkdir tars
chown root:root ./tars/
cd tars/
cp $CodePath/Tars/framework/build/framework.tgz /usr/local/app/tars/
tar -zxvf framework.tgz

sed -i "s/192.168.2.131/$MachineIp/g" `grep 192.168.2.131 -rl ./*`
sed -i "s/db.tars.com/$MachineIp/g" `grep db.tars.com -rl ./*`
sed -i "s/registry.tars.com/$MachineIp/g" `grep registry.tars.com -rl ./*`
sed -i "s/web.tars.com/$MachineIp/g" `grep web.tars.com -rl ./*`
##拉起核心基础服务
cd /usr/local/app/tars/
chmod u+x tars_install.sh
./tars_install.sh
echo "Finish tars framework service installatin and pull">>$CodePath/Tars/shellDeploy/deploy_log


##配置tarsweb数据库以及表项
##Config Tarsweb database
cd $CodePath/Tars/web/
sed -i "s/db.tars.com/$MachineIp/g" config/webConf.js
sed -i "s/registry.tars.com/$MachineIp/g" config/tars.conf

## 安装tarsweb使用软件
## install tarsweb software
npm install --registry=https://registry.npm.taobao.org
npm run prd
##创建log日志目录
## create log folder
mkdir -p /data/log/tars

cp $CodePath/Tars/shellDeploy/importTarsWebSql.sh $CodePath/Tars/web/sql
cd $CodePath/Tars/web/sql
./importTarsWebSql.sh root $MysqlDefaultPassword
if [ $? -ne 0 ]; then
    echo "config mysql database for web failed">>$CodePath/Tars/shellDeploy/deploy_log
    exit
else
    echo "config mysql database for web successfully">>$CodePath/Tars/shellDeploy/deploy_log
fi

cd $CodePath/Tars/web/
pm2 start 0
echo "start tars web">>$CodePath/Tars/shellDeploy/deploy_log

##关闭防火墙
##shutdown firewall
service firewalld status
systemctl stop firewalld
systemctl disable firewalld
echo "shutdown and disable firewall">>$CodePath/Tars/shellDeploy/deploy_log

##非框架核心服务编译和发布
##None Core Service Compiple
cd $CodePath/Tars/framework/build
make tarsstat-tar
make tarsnotify-tar
make tarsproperty-tar
make tarslog-tar
make tarsquerystat-tar
make tarsqueryproperty-tar
echo "compile none-core services of framework">>$CodePath/Tars/shellDeploy/deploy_log
