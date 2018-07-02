# TARS-HTTP-SERVER-PROTOBUF的说明

使用proto buf 定义 接口

安装: cd src/ && composer install
cd ..

更新  支持 自定义的 onInitServer:

cp ./new_Server.php  ./src/vendor/phptars/tars-server/src/core/Server.php



运行 :./restart.sh


测试打开:
http://127.0.0.1:30000/account/login?passwd=101&phone=15600000000

![](https://github.com/xcwen/Tars/raw/php-http-extends/php/examples/tars-http-server-protobuf/images/2.png)

协议文档地址:
http://127.0.0.1:30000/doc

![](https://github.com/xcwen/Tars/raw/php-http-extends/php/examples/tars-http-server-protobuf/images/1.png)

进一步开发: 

安装 protobuf : 安装最新版本  protobuf  ,支持php

生成 protobuf 协议  对应的代码: 

./proto/gen_proto.sh 

当前项目名:yb_account

协议protobuf 文件在: proto/src/yb_account

每个协议单独一个文件

如
account__get_userid.proto

```protobuf

syntax = "proto3";

//__CMD: 0x1003 
//__DESC: 得到用户id
//__TAGS: for_app

message in  {
  int32  role = 1; // 角色
  string phone = 2; //电话
}

message out {
  int32  id = 1 ;
}


```
