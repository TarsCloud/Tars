说明：
1）该脚本用于快速部署Tars环境
2）该脚本仅仅使用于物理机部署和虚拟机部署（包括云主机），不适用于容器部署
3) 操作系统 CentOS 7
4）硬件要求：最低2核2G，建议4核8G
5）该脚本运行时候，需要使用本机的IP地址做为入参，请先查看本机IP地址。如果是云环境的话，请输入小网IP地址。
6) 该脚本中使用Mysql安装环境中的默认密码tars2015作为默认密码，仅供学习和演示使用。实际应用场景，请大家自行修改密码。 
7）文件夹中的三个脚本的说明：
   7.1  shellDeploy.sh 为主入口脚本， 进行绝大部分安装操作。 shellDeploy.sh调用了mysqlConfig.sh以及importTarsWebSql.sh
   7.2  mysqlConfig.sh 主要用于mysql数据库的配置，包括：用户名，密码，权限，数据库表项导入等等
   7.3  importTarsWebSql.sh 用于导入TarsWeb的数据库表项

