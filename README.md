

# TARS - A Linux Foundation Project

- [TARS Foundation Official Website](https://tarscloud.org/foundation/index)
- [TARS Project Official Website](http://tarscloud.org/)
- WeChat Group: TARS01
- WeChat Offical Account: TarsCloud
- QQ Group: 579079160 and 669339903
- Twitter: [@TarsCloud](https://twitter.com/TarsCloud)
- [Mailing List](https://groups.google.com/g/tars-foundation-information)
- [Contacts](https://tarscloud.org/about/contacts)

## What is TARS?

TARS is a Linux Foundation project. It is a high-performance RPC framework based on name service and Tars protocol, also integrated administration platform, and implemented hosting-service via flexible schedule.

Tars, aka TAF(Total Application Framework), has been used in Tencent since 2008. It supports C++, Java, Nodejs and PHP for now. This framework offers a set of solution for development, maintenance and testing, which making development, deployment and testing service efficiently.
It integrated extensible protocol for encoding/decoding, high-performance RPC communication framework, name service, monitor, statistics and configuration. You can use it to develop your reliable distributed application based on microservice fast, and reach fully efficient service management.

Nowadays it's used by hundreds of bussiness in Tencent, services that developed base on TAF run on 16 thousands of machines.

See the detailed introduction [SUMMARY.md](https://tarscloud.github.io/TarsDocs_en/).

## Supported platforms
For now it supports OS as below:

- Linux
- Mac(>=2.1.0 support)

## Supported languages

For now it supports following languages:

- C++
- Java
- Nodejs
- PHP
- Go
 
## Version Management
 
Tars is composed of many modules, scattered in many warehouses, and the basic framework version and language version can develop independently. In view of this, from version 2.1.0, the Framework version tag is printed on the tarsframework warehouse, no longer reflected in the tars warehouse

In addition, each component will have its own version. When there is a version dependency specification, each component will have its own version

## Installation 

1. If you are new to Tars, please read documentation [installation](https://tarscloud.github.io/TarsDocs_en/installation).
2. First deploy, please read documentation [source](https://tarscloud.github.io/TarsDocs_en/installation/source.html).
3. Install by docker, detail information: [docker](https://tarscloud.github.io/TarsDocs_en/installation/docker.html)。

## Submodule

Directory         |Features
------------------|----------------
framework         |Source code implementation of C++ language framework basic service
cpp               |C++ language framework rpc source code implementation
java              |java language framework rpc source code implementation
go                |go language framework rpc source code implementation
nodejs            |nodejs language framework rpc source code implementation
php               |php language framework rpc source code implementation
tup               |source code implementation of tup group protocol in each language
web               |manage tars web source implementation

## Developer's documentation

See [docs](https://github.com/TarsCloud/TarsDocs_en).

## License

The open-source protocol Tars used is BSD-3-Clause, see [LICENSE.md](https://github.com/TarsCloud/TarsDocs_en/blob/master/LICENSE).

## Chinese Version
[Read Chinese Version](README.zh.md)

