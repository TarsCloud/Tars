该工程是Tars框架C++语言的源代码

目录名称 |功能
------------------|----------------
build        |C++语言框架编译和安装的目录
framework    |C++语言框架基础服务的源码实现
servant      |C++语言框架rpc的源码实现
tools        |C++语言框架IDL工具的源码实现
util         |C++语言框架基础工具库的源码实现
examples     |C++语言框架的示例代码，包括：快速入门示例、promise编程入门示例、压测程序示例
test         |C++语言框架各个部分的测试程序
doc          |文档说明
thirdparty   |依赖第三方组件或者库的存放目录，目前框架基础服务QueryStatServer用到了rapidjson，需要下载放到这个目录

依赖环境

软件 |软件要求
------|--------
linux内核版本:      |	2.6.18及以上版本
gcc版本:          	|   4.1.2及以上版本、glibc-devel
bison工具版本:      |	2.5及以上版本
flex工具版本:       |	2.5及以上版本
cmake版本：       	|   2.8.8及以上版本
mysql版本：         |	4.1.17及以上版本
rapidjson版本：     |   1.0.2版本

编译安装

```
cd ./build
./build.sh all
./build.sh install
```
详细安装参见Install.md