# TARS-HTTP-SERVER-PROTOBUF的说明

使用proto buf 定义 接口

安装: composer install

安装 protobuf :


生成 protobuf 协议  对应的代码: 
./proto/gen_proto.sh 

运行 :./restart.sh


测试打开:
http://127.0.0.1:30000/account/login?passwd=101&phone=15600000000

协议文档地址:
http://127.0.0.1:30000/doc
