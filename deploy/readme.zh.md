# **步骤:**
    
## 1. 在机器上安装mysql, 并修改文件comm.properties中的db信息.

建议使用Mysql `5.6.26`, 以上的版本也是可以的.
```
[tarscomm]
mysql.host=127.0.0.1
mysql.port=3306
mysql.root.password=tars12345
```


大于5.6版本的Mysql增加了密码验证插件， 导致太简单的密码是无法创建的, 可以通过以下命令修改数据库密码验证配置来创建简单密码:

```
set global validate_password_policy=0;
set global validate_password_length=1;
```
同时确保mysql路径是正确的, 请看到 [注意事项 6](#attention-6)
## 2. 使用脚本部署

可以使用以下命令运行脚本部署项目
```sh
cd /data
git clone https://github.com/TarsCloud/Tars.git --recursive
cd /data/Tars/deploy
python ./deploy.py all
```
# 参数配置
```sh
python ./deploy.py [option]

[option]
    all:
        check()     //检查需要的工具包是否已经安装
        build()     //编译并构建框架
        deploy()    //初始化数据库并部署框架和web管理系统
        test()      //测试所有服务是否已经成功启动
    check:
        check()
    build:
        build()
    deploy:
        deploy()
    test:
        test()
```
# 注意事项

1. 安装mysql而不是mariadb （centos7 命令 `yum install mysql` 会自动安装mariadb)
2. 为了能够成功构建, 确保机器内存RAM >= 2G, 虚拟内存也是支持的.
3. 支持的 python 版本为 2.7
4. 确保网络无障碍
5. 你需要安装: gcc,gcc-c++, cmake, yasm, glibc-devel, flex, bison, ncurses-devel, zlib-devel, autoconf, 如果未安装, 脚本会自动安装, but 但有可能会失败.
6. <a id="attention-6"></a>Tars 使用 `/usr/local/mysql/` 作为默认路径. 如果你的机器mysql路径不同, 需要在编译之前编辑文件 CMakeLists.txt (`framework/tarscpp/CMakeLists.txt`, `framework/CMakeLists.txt`). 
7. 一部分CentOS7的网卡名字不是 `eth0` 而是 `ens33`, 需要更改以下文件中的网卡名:
`Tars/deploy/comm/tarsUtil.py :line 24: getAddress()`
8. `Error: npm command not found`, 删除 `source /etc/profile` 位于 `Tars/ deploy/comm/tarsDeploy.py: line 70: deployWeb()` 并重试.
9.  `Error: tarsDeploy.py: line 51 :shutil.copy: no such file or directory`, 可能是由于build失败导致的, 重新看注意事项 `2`.