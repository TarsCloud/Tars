#!/bin/bash
MachineIp=$1
CodePath=/usr/local/tarscode
MysqlDefaultPassword=tars2015

if [ ! -n "$1" ] ;then
    echo "NO Local IP address Input, please input IP address."
    exit
else
    echo "Input Local IP address is $MachineIp"
fi


time=$(date "+%Y%m%d-%H%M%S")
echo "Install Script Run Time : ${time}"

if false;then
##install basic tools
yum install -y glibc-devel
yum install -y flex bison
yum install -y cmake
yum install -y ncurses-devel
yum install -y zlib-devel
yum install -y perl
yum install -y wget
yum install -y net-tools
yum install -y gcc
yum install -y gcc-c++
yum install -y flex
yum install -y bison
yum install -y make
yum install -y git
yum install -y expect
yum install -y tar

#install mysql
wget https://repo.mysql.com//mysql80-community-release-el7-3.noarch.rpm
rpm -ivh mysql80-community-release-el7-3.noarch.rpm
yum-config-manager --disable mysql80-community
yum-config-manager --enable mysql57-community
yum install -y mysql-community-server
yum install -y mysql-devel


#check whether folder /usr/local/mysql/data exist; creat the folder if it does NOT exist
#if [ ! -d "/usr/local/mysql/data" ]; then
#echo "[Message]/usr/local/mysql/data does not exit"
#mkdir -p /usr/local/mysql/data
#echo "[Message]create a folder for /usr/local/mysql/data"
#fi

#backup mysql data,make sure no data loss in mysql
#datename=$(date +%Y%m%d-%H%M%S)
#mv /usr/local/mysql/data /usr/local/mysql/$datename
#mkdir -p /usr/local/mysql/data



#download the code
mkdir -p $CodePath
cd $CodePath
git clone https://github.com/TarsCloud/Tars.git
cd Tars
git submodule update --init --recursive


#compile tars code
cd $CodePath/Tars/framework
#modify the parameter in the CMake, make sure the compile run successfully
find . -name 'CMakeLists.txt' | xargs perl -pi -e 's|/usr/local/mysql/include|/usr/include/mysql|g'
find . -name 'CMakeLists.txt' | xargs perl -pi -e 's|/usr/local/mysql/lib|/usr/lib64/mysql|g'
cd $CodePath/Tars/framework/tarscpp
find . -name 'CMakeLists.txt' | xargs perl -pi -e 's|/usr/local/mysql/include|/usr/include/mysql|g'
find . -name 'CMakeLists.txt' | xargs perl -pi -e 's|/usr/local/mysql/lib|/usr/lib64/mysql|g'
cd $CodePath/Tars/framework/build
./build.sh all


#install tars
cd /usr/local/
mkdir tars
chown root:root ./tars/
cd $CodePath/Tars/framework/build
./build.sh install



#systemctl status mysqld.service
systemctl start mysqld.service
systemctl enable mysqld.service

fi

wget -qO- https://raw.githubusercontent.com/creationix/nvm/v0.33.11/install.sh | bash
source ~/.bashrc
nvm install v8.11.3
npm install -g pm2 --registry=https://registry.npm.taobao.org


#check whether mysqlOperation file exist
checkfile=$CodePath/Tars/framework/sql/mysqlOperation.sh
if [  -f "$checkfile" ]; then 
rm -rf $CodePath/Tars/framework/sql/mysqlOperation.sh
echo "get rid of the old file, copy an new file"
fi 

cp -rf mysqlOperation.sh  $CodePath/Tars/framework/sql 

cd  $CodePath/Tars/framework/sql
pwd

sed -i "s/192.168.2.131/$MachineIp/g" `grep 192.168.2.131 -rl ./*`
sed -i "s/db.tars.com/$MachineIp/g" `grep db.tars.com -rl ./*`
sed -i "s/10.120.129.226/$MachineIp/g" `grep 10.120.129.226 -rl ./*`


# Grab temp password
TempPassword=`cat /var/log/mysqld.log |grep "temporary password" |awk -F ":" '{print $NF}'`
echo "Temp Password:" $TempPassword
#input password to login 
#./mysqlOperation.sh root $TempPassword
./mysqlOperation.sh root tars2015


#import db_tars_web.sql
cp importTarsWebSql.sh $CodePath/Tars/web/sql
cd $CodePath/Tars/web/sql
sed -i 's/db.tars.com/$MachineIp/g' config/webConf.js
sed -i 's/registry.tars.com/$MachineIp/g' config/tars.conf
./importTarsWebSql.sh $MysqlDefaultPassword



#make the framework
cd $CodePath/Tars/framework/build
make framework-tar

#create install path
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

#pull all the tars process to run
cd /usr/local/app/tars/
chmod u+x tars_install.sh
./tars_install.sh
ps -aux |grep tars


service firewalld status
systemctl stop firewalld
systemctl disable firewalld

pm2 start 0
