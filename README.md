[点我查看中文版](README.zh.md)

# Tars

Tars comes from the robot in Interstellar movie. Tars is a high-performance RPC framework based on name service and Tars protocol, also integrated administration platform, and implemented hosting-service via flexible schedule.

Tars, aka TAF(Total Application Framework), has been used in Tencent since 2008. It supports C++,Java,Nodejs and php for now. This framework offers a set of solution for development, maintenance and testing, which making develop, deploy and testing service efficiently.
It integrated extensible protocol for encoding/decoding, high-performance RPC communication framework, name service, monitor, statistics and configuration. You can use it to develop your reliable distributed application based on microservice fast, and reach fully efficient service management.

Nowadays it's used by hundreds of bussiness in Tencent, services that developed base on TAF run on 16 thousands of machines.

See the detailed introduction [Introduction.md](Introduction.md).

## Supported platforms
For now it supports OS as below:

- Linux

## Supported languages

For now it supports following languages:

- C++
- Java
- Nodejs
- PHP
- Go
 
## Installation 

If you are new to Tars, please read documentation [Install.md](Install.md).
If you are familar to Tars, refer to scripts in directory [build](build), may some modification needed.

## Performance

See [tars_performce.md](docs/tars_performce.md).

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

See [docs](docs).

## License

The open-source protocol Tars used is BSD-3-Clause, see [LICENSE.TXT](LICENSE.TXT).

## Contacts

qq group code：579079160

