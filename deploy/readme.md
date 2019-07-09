
# **Tars Installation via Script**
    
## 1. Install mysql, and change the db information in comm.properties

We recommend to use a mysql version later than `5.6.26`. 

You can download mysql using wget. Below is an example downloading version 5.7, but you can change it to the desired version.

```  bash
wget -i -c http://dev.mysql.com/get/mysql57-community-release-el7-10.noarch.rpm  
yum -y install mysql57-community-release-el7-10.noarch.rpm  
yum -y install mysql-community-server  
yum -y install mysql-devel  
```  
If you have problems to install mysql with the above step, add the new mysql repository to local server with this yum command and then re-run the previous commands.  

```  bash
sudo yum localinstall https://dev.mysql.com/get/mysql57-community-release-el7-10.noarch.rpm  
```  

**PS:** Be aware that if you install mysql via `yum install mysql` on Centos 7, Mariadb will be installed instead. Since that's not what we want, we recommend you to follow the procedure described above.

If your mysql version is later than 5.6, you need to configure strong password rules as follows:
  
```  bash
set global validate_password_policy=0;  
set global validate_password_length=1;  
```

Make sure that the path of mysql source is the default. Tars uses `/usr/local/mysql/` as default path. If yours is not this, please modify the file CMakeLists.txt (`framework/tarscpp/CMakeLists.txt`, `framework/CMakeLists.txt`) before compiling. 

After, you can change the tarscomm properties as follows.

```sql
[tarscomm]
mysql.host=127.0.0.1
mysql.port=3306
mysql.root.password=tars12345
```



## 2. Checkout and deploy the source code

```bash
cd /data
git clone https://github.com/TarsCloud/Tars.git --recursive
cd /data/Tars/deploy
python ./deploy.py all
```

## 3. Build, deploy and test

You can check, build, deploy and test Tars with the commands below.

```bash
python ./deploy.py [option]

[option]
    all:
        check()     //check if the needed tools is installed
        build()     //compile and build framework
        deploy()    //initial db and deploy framework and web
        test()      //test if the servers successfully started
    check:
        check()
    build:
        build()
    deploy:
        deploy()
    test:
        test()
```
# To consider

1. In order to successfully build Tars, make sure your machine has at least 2 Gb of RAM (It's ok to have virtual RAM).
2. Make sure your machine supports Python 2.7.
3. Make sure your network is unobstructed.
4. You have to install: gcc,gcc-c++, cmake, yasm, glibc-devel, flex, bison, ncurses-devel, zlib-devel, autoconf.
5.  The *NIC name* in some CentOS7 is `ens33` instead of `eth0`. If that is your case, you need to change the following file: `Tars/deploy/comm/tarsUtil.py :line 24: getAddress()`.
7.  If you face this error:  `Error: npm command not found`, you should delete `source /etc/profile` in `Tars/ deploy/comm/tarsDeploy.py: line 70: deployWeb()` and retry.
8.  The error`Error: tarsDeploy.py: line 51 :shutil.copy: no such file or directory` might be caused by build error. In this case, please verify item 1.