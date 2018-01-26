# 安装说明

安装前，需要下载以下软件到此目录下（如果Install.md中所说的某些依赖的工具或者软件已经安装，可以修改install.sh脚本，进行注释掉）

apache-maven-3.3.9-bin.tar.gz

cmake-2.8.8.tar.gz

jdk-8u111-linux-x64.tar.gz

mysql-5.6.26.tar.gz

resin-4.0.49.tar.gz

安装前，需要确保能联网，以便下载java框架的依赖包和c++框架依赖的rapadjson，其中安装java语言的框架时，最好用国内的maven镜像源(这个比较重要)。

详细安装说明参见Install.md。

依赖软件下载完毕后，在执行install.sh前，需要编辑此脚本，对脚本中的MachineIp和MachineName进行赋值，MachineIp是部署机器的ip，MachineName是部署机器的主机名，可以通过查看/etc/host知道。

另外install.sh的运行，需要root权限。

若有问题，可以加入tars的qq技术交流群：579079160，进行咨询。


