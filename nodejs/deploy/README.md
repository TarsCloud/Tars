# @tars/deploy

TARS 框架服务打包工具，用于打包服务生成适合 TARS 框架的发布包。

## 安装

`npm install -g @tars/deploy`  

> 由于 `tars-deploy` 是一个 CLI 程序，所以需要使用 __-g__ 参数来安装

## 用法

`tars-deploy name [options]`  

* name 为服务的"服务名" ，如您的服务名为 Server，那么填写 "Server"
* [options] 可选配置，详见 [选项](#options) 节

__打包时：请切换当前目录到服务的根目录（也就是服务 `package.json` 所在的目录）后执行此程序__

## 选项

> Options:

>   -h, --help     output usage information  
>   -V, --version  output the version number  
>   -f, --force    Force to Build Package

### -f, --force

由于工具会打包当前的运行环境（如 node 可执行的二进制文件、在当前架构上重新编译 C/C++ addon 等）所以请在与目标运营架构相同的环境（linux）上执行打包工具。

打开此开关，可跳过此限制。但同时我们强烈建议您，不要这么做！