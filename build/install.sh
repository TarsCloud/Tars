#!/bin/bash

PWD_DIR=`pwd`
MachineIp=
MachineName=
MysqlIncludePath=
MysqlLibPath=


##安装glibc-devel

yum install -y glibc-devel

##安装flex、bison

yum install -y flex bison

##安装cmake

tar zxvf cmake-2.8.8.tar.gz
cd cmake-2.8.8
./bootstrap
make
make install
cd -

## 安装mysql
yum install -y ncurses-devel
yum install -y zlib-devel

if [   ! -n "$MysqlIncludePath"  ] 
  then
	tar zxvf mysql-5.6.26.tar.gz
	cd mysql-5.6.26
	cmake . -DCMAKE_INSTALL_PREFIX=/usr/local/mysql-5.6.26 -DWITH_INNOBASE_STORAGE_ENGINE=1 -DMYSQL_USER=mysql -DDEFAULT_CHARSET=utf8 -DDEFAULT_COLLATION=utf8_general_ci
	make
	make install
	ln -s /usr/local/mysql-5.6.26 /usr/local/mysql
	cd -
  else
  	## 根据mysql 库路径 配置 设置framework/CMakeLists.txt tarscpp/CMakeList.txt
  	sed -i "s@/usr/local/mysql/include@${MysqlIncludePath}@g" ../framework/CMakeLists.txt
  	sed -i "s@/usr/local/mysql/lib@${MysqlLibPath}@g" ../framework/CMakeLists.txt
  	sed -i "s@/usr/local/mysql/include@${MysqlIncludePath}@g" ../framework/tarscpp/CMakeLists.txt
  	sed -i "s@/usr/local/mysql/lib@${MysqlLibPath}@g" ../framework/tarscpp/CMakeLists.txt

fi


yum install -y perl
cd /usr/local/mysql
useradd mysql
rm -rf /usr/local/mysql/data
mkdir -p /data/mysql-data
ln -s /data/mysql-data /usr/local/mysql/data
chown -R mysql:mysql /data/mysql-data /usr/local/mysql/data
cp support-files/mysql.server /etc/init.d/mysql

yum install -y perl-Module-Install.noarch
perl scripts/mysql_install_db --user=mysql
cd -

sed -i "s/192.168.2.131/${MachineIp}/g" `grep 192.168.2.131 -rl ./conf/*`
cp ./conf/my.cnf /usr/local/mysql/

##启动mysql
service mysql start
chkconfig mysql on

##添加mysql的bin路径
echo "PATH=\$PATH:/usr/local/mysql/bin" >> /etc/profile
echo "export PATH" >> /etc/profile
source /etc/profile

##修改mysql root密码
cd /usr/local/mysql/
./bin/mysqladmin -u root password 'root@appinside'
./bin/mysqladmin -u root -h ${MachineName} password 'root@appinside'
cd -

##添加mysql的库路径
echo "/usr/local/mysql/lib/" >> /etc/ld.so.conf
ldconfig

##下载C++基础服务框架
yum install -y git
cd ../
git submodule update --init --recursive framework
cd -

##安装c++语言框架
cd ../framework/build/
chmod u+x build.sh
./build.sh all
./build.sh install
cd -

##Tars数据库环境初始化
mysql -uroot -proot@appinside -e "grant all on *.* to 'tars'@'%' identified by 'tars2015' with grant option;"
mysql -uroot -proot@appinside -e "grant all on *.* to 'tars'@'localhost' identified by 'tars2015' with grant option;"
mysql -uroot -proot@appinside -e "grant all on *.* to 'tars'@'${MachineName}' identified by 'tars2015' with grant option;"
mysql -uroot -proot@appinside -e "flush privileges;"

cd ../framework/sql/
sed -i "s/192.168.2.131/${MachineIp}/g" `grep 192.168.2.131 -rl ./*`
sed -i "s/db.tars.com/${MachineIp}/g" `grep db.tars.com -rl ./*`
chmod u+x exec-sql.sh
./exec-sql.sh
cd -

##打包框架基础服务
cd ../framework/build/
make framework-tar

make tarsstat-tar
make tarsnotify-tar
make tarsproperty-tar
make tarslog-tar
make tarsquerystat-tar
make tarsqueryproperty-tar
cd -

##安装核心基础服务
mkdir -p /usr/local/app/tars/
cd ../framework/build/
cp framework.tgz /usr/local/app/tars/
cd -

cd /usr/local/app/tars
tar xzfv framework.tgz

sed -i "s/192.168.2.131/${MachineIp}/g" `grep 192.168.2.131 -rl ./*`
sed -i "s/db.tars.com/${MachineIp}/g" `grep db.tars.com -rl ./*`
sed -i "s/registry.tars.com/${MachineIp}/g" `grep registry.tars.com -rl ./*`
sed -i "s/web.tars.com/${MachineIp}/g" `grep web.tars.com -rl ./*`

chmod u+x tars_install.sh
./tars_install.sh

./tarspatch/util/init.sh

##安装nodejs环境
wget -qO- https://raw.githubusercontent.com/creationix/nvm/v0.33.11/install.sh | bash
source ~/.bashrc
nvm install v8.11.3

cd -

##安装web管理系统
cd ../
git submodule update --init --recursive web
cd web/
npm install -g pm2 --registry=https://registry.npm.taobao.org
sed -i "s/registry.tars.com/${MachineIp}/g" `grep registry1.tars.com -rl ./config/*`
sed -i "s/db.tars.com/${MachineIp}/g" `grep db.tars.com -rl ./config/*`
npm install --registry=https://registry.npm.taobao.org
npm run prd

cd -

mkdir -p /data/log/tars/

