# TARS-HTTP-SERVER-PROTOBUF的说明

使用proto buf 定义 接口

安装: cd src/ && composer install

更新  支持 自定义的 onInitServer:

cp ./new_Server.php  ./src/vendor/phptars/tars-server/src/core/Server.php



运行 :./restart.sh


测试打开:
http://127.0.0.1:30000/account/login?passwd=101&phone=15600000000

协议文档地址:
http://127.0.0.1:30000/doc


进一步开发: 

安装 protobuf : 安装最新版本  protobuf  ,支持php

生成 protobuf 协议  对应的代码: 

./proto/gen_proto.sh 
