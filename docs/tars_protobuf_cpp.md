# tars支持protobuf service描述文件

如果当你了解到tars的时候，你已经有不少已有业务采用了protobuf协议；如果想要把这些业务迁移到tars，你还需要手动把proto文件翻译成tars文件,非常麻烦而且容易出错。
现在tars使用protoc的插件机制，提供了对proto文件的直接支持，能够生成tars rpc相关代码，使得迁移平滑省心。


## 使用方法


### 1. 准备proto文件
proto文件的语法是不限制的，你可以使用proto2或proto3；
但注意，一定加上**option cc_generic_services=false;**
因为我们的目标就是不使用protoc生成的pb rpc接口，而是要用tars插件接管，生成符合tars框架的rpc接口.
一个proto文件的示例如下:


```cpp
syntax = "proto2";

option cc_generic_services=false;

package TestApp;

message PbRequest {
    required int32 a = 1;
    required int32 b = 2;
}

message PbResponse { 
    required int32 c = 1;
}  

service Hello {
    rpc add(PbRequest) returns (PbResponse) {
    }
}
```


### 2. 直接执行make即可
因为调用tars pb插件的语句，已经内置在框架makefile.tars文件中。
由于protoc默认生成的文件名带有.pb.h后缀,tars插件也遵循这个命名规则，生成的文件后缀是.tars.h


