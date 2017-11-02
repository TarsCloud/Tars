# phptars
phptars是针对php使用tars二进制协议，以及tars平台整体运维、RPC等一系列能力的解决方案。它主要由如下的两个部分组成：

## tarsclient

tarsclient中提供了对tars服务进行调用的php能力，包括：

* 用来TUP协议打包解包、编码解码的php扩展及测试用例；
* 用来从tars文件生成php类文件的tars2php工具；
* 调用远程服务的实例；
* 详细的client使用说明README

## tarsserver
tarsserver正在建设中，敬请期待。将会支持如下的特性：

* 基于swoole2.0的高性能服务
* 支持tup协议和tars流两种协议模式
* 上报、监控、日志的集成
* tars平台发布支持
