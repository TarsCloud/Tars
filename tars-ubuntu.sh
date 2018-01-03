#!/bin/sh

PWD_DIR=/home/ubuntu/tars/build
MachineIp=10.135.24.205
##"ifconfig eth0 | grep "inet addr" | awk '{ print $2}' | awk -F: '{print $2}'"
MachineName='hostname'

##git
apt install -y git lrzsz

##clone code
cd /home/ubuntu
git clone https://github.com/tencent/tars.git

cd /home/ubuntu/tars/build

##安装glibc-devel
apt install -y libc-dev-bin 

##安装flex、bison
apt install -y flex bison

##安装cmake
apt install -y cmake

##安装java jdk
apt install -y openjdk-8-jdk
java -version

##安装maven
wget http://archive.apache.org/dist/maven/maven-3/3.3.9/binaries/apache-maven-3.3.9-bin.tar.gz
tar zxvf apache-maven-3.3.9-bin.tar.gz
##替换阿里云maven
sed -i "s?http://my.repository.com/repo/path?http://maven.aliyun.com/nexus/content/groups/public?g"  ./apache-maven-3.3.9/conf/settings.xml
sed -i "s?<id>mirrorId</id>?<id>nexus-aliyun</id>?g"  ./apache-maven-3.3.9/conf/settings.xml
sed -i "s?<mirrorOf>repositoryId</mirrorOf>?<mirrorOf>*</mirrorOf>?g"  ./apache-maven-3.3.9/conf/settings.xml
sed -i "s?<name>Human Readable Name for this Mirror.</name>?<name>Nexus aliyun</name>?g"  ./apache-maven-3.3.9/conf/settings.xml
#apt install -y maven

echo "export MAVEN_HOME=/home/ubuntu/tars/build/apache-maven-3.3.9/" >> /etc/profile
echo "export PATH=\$PATH:\$MAVEN_HOME/bin" >> /etc/profile

source /etc/profile

mvn -v

##安装resin
wget http://caucho.com/download/debian/dists/trusty/multiverse/binary-amd64/resin-pro_4.0.54-amd64.deb
dpkg -i resin-pro_4.0.54-amd64.deb
cd /home/ubuntu/tars/build
#ln -s /usr/local/resin-4.0.55 /usr/local/resin

##安装rapidjson
git clone https://github.com/Tencent/rapidjson.git

cp -r ./rapidjson ../cpp/thirdparty/

##安装mysql
rm -rf /etc/my.cnf

apt install -y libncurses5-dev
apt install -y zlib1g   zlib1g.dev

#下载mysql 
wget https://dev.mysql.com/get/Downloads/MySQL-5.6/mysql-5.6.38.tar.gz
tar zxvf mysql-5.6.38.tar.gz
cd mysql-5.6.38
cmake . -DCMAKE_INSTALL_PREFIX=/usr/local/mysql-5.6.38 -DWITH_INNOBASE_STORAGE_ENGINE=1 -DMYSQL_USER=mysql -DDEFAULT_CHARSET=utf8 -DDEFAULT_COLLATION=utf8_general_ci
make
make install
ln -s /usr/local/mysql-5.6.38 /usr/local/mysql
cd -

apt install -y perl
cd /usr/local/mysql
useradd mysql
rm -rf /usr/local/mysql/data
mkdir -p /data/mysql-data
ln -s /data/mysql-data /usr/local/mysql/data
chown -R mysql:mysql /data/mysql-data /usr/local/mysql/data
cp support-files/mysql.server /etc/init.d/mysql

##apt install -y perl-Module-Install.noarch
perl scripts/mysql_install_db --user=mysql
cd -

sed -i "s/192.168.2.131/10.135.24.205/g" `grep 192.168.2.131 -rl ./conf/*`
cp ./conf/my.cnf /usr/local/mysql/

##启动mysql
#service mysql start
#chkconfig mysql on
#/etc/init.d/mysql start
systemctl enable mysql.service
systemctl start mysql.service
#systemctl stop mysql.service
#systemctl start mysql.service

##添加mysql的bin路径
echo "PATH=\$PATH:/usr/local/mysql/bin" >> /etc/profile
echo "export PATH" >> /etc/profile
source /etc/profile

##修改mysql root密码
cd /usr/local/mysql/
./bin/mysqladmin -u root password 'root@appinside'
./bin/mysqladmin -u root -h VM-24-205-ubuntu password 'root@appinside'
cd -

##添加mysql的库路径
echo "/usr/local/mysql/lib/" >> /etc/ld.so.conf
ldconfig

cd /home/ubuntu/tars/build/
##替换阿里云maven
#sed -i "s?http://my.repository.com/repo/path?http://maven.aliyun.com/nexus/content/groups/public?g"  ./apache-maven-3.3.9/conf/settings.xml

#source /etc/profile

##安装java语言框架
cd ../java/
mvn clean install 
mvn clean install -f core/client.pom.xml 
mvn clean install -f core/server.pom.xml
cd -

##安装c++语言框架
cd ../cpp/build/
chmod u+x build.sh
./build.sh all
./build.sh install
cd -

##Tars数据库环境初始化
mysql -uroot -proot@appinside -e "grant all on *.* to 'tars'@'%' identified by 'tars2015' with grant option;"
mysql -uroot -proot@appinside -e "grant all on *.* to 'tars'@'localhost' identified by 'tars2015' with grant option;"
mysql -uroot -proot@appinside -e "grant all on *.* to 'tars'@'VM-24-205-ubuntu' identified by 'tars2015' with grant option;"
mysql -uroot -proot@appinside -e "flush privileges;"

cd ../cpp/framework/sql/
sed -i "s/192.168.2.131/10.135.24.205/g" `grep 192.168.2.131 -rl ./*`
sed -i "s/db.tars.com/10.135.24.205/g" `grep db.tars.com -rl ./*`
chmod u+x exec-sql.sh
./exec-sql.sh
cd -

##打包框架基础服务
cd ../cpp/build/
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
cd ../cpp/build/
cp framework.tgz /usr/local/app/tars/
cd /usr/local/app/tars
tar xzfv framework.tgz

sed -i "s/192.168.2.131/10.135.24.205/g" `grep 192.168.2.131 -rl ./*`
sed -i "s/db.tars.com/10.135.24.205/g" `grep db.tars.com -rl ./*`
sed -i "s/registry.tars.com/10.135.24.205/g" `grep registry.tars.com -rl ./*`

chmod u+x tars_install.sh
./tars_install.sh

./tarspatch/util/init.sh

##安装web管理系统
cd /home/ubuntu/tars/build
cd ../web/
sed -i "s/db.tars.com/10.135.24.205/g" `grep db.tars.com -rl ./src/main/resources/*`
sed -i "s/registry1.tars.com/10.135.24.205/g" `grep registry1.tars.com -rl ./src/main/resources/*`
sed -i "s/registry2.tars.com/10.135.24.205/g" `grep registry2.tars.com -rl ./src/main/resources/*`


mvn clean package
#cp ./target/tars.war /usr/local/resin/webapps/
cp ./target/tars.war /var/resin/webapps/

cd -

mkdir -p /data/log/tars/
#cp ./conf/resin.xml /usr/local/resin/conf/
mv /etc/resin/resin.xml /etc/resin/resin.xml.bak
cp ./conf/resin.xml /etc/resin/

#/usr/local/resin/bin/resin.sh start
/usr/local/share/resin-4.0.54/bin/resinctl stop
/usr/local/share/resin-4.0.54/bin/resinctl start
