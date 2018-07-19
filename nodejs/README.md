# Tars.js

它不是一个玩具！

它是一款在 [Tars 平台](http://tars.tencent.com) 上执行 Node.js 服务的基础架构，并且根据 Node.js 的语言特点与业务特性开发。

它可以帮助您在大型复杂异构系统中快速搭建（迁移） Node.js 服务。

## 特性

### 总览

`Tars.js` 包含下述特性：

* 内置负载均衡（通过 Cluster 模块实现）
* 异常退出的监控与拉起
* 日志搜集与处理
* 支持 TARS 平台的管理命令
* HTTP(s) 服务监控与用量自动上报
* 支持 Tars RPC 调用与染色（模调自动上报）
* 支持动态拉取服务配置

__我们强烈建议您阅读 [@tars/node-agent](https://github.com/tars-node/node-agent) README 了解详细说明。__

### 高自由度

* 兼容所有（≥0.12 即可）官方 Node.js 版本。
* 对 Node.js 源码无侵入无修改。
* 底层对上层完全透明，支持各种上层框架，无需变更。

也就是说：

* 您可以使用任何喜欢的第三方框架（如 [Express.js](https://expressjs.com/) / [Koa.js](https://koajs.com/) 等），也无需对框架进行任何修改（或引入任何中间件）。即可在 `Tars` 平台上运行，享受到平台提供的各种监控与管理特性。
* 您也可以自由选择您项目需要的 Node.js 版本。

### 高性能

* 默认的旁路上报与监控对服务性能的影响小于 5%
* 常用模块（RPC、日志等）性能位于业界前列。

模块以功能划分，业务只需依赖它所使用到的模块即可，无需全部载入。

### 可持续

* 项目由 2014 年开始开发，紧跟 Node.js 新版本特性进行优化。
* 响应及时，在腾讯内部服务于上百亿的流量。

## Hello World

1. 在 Tars 平台 -> 服务管理 -> 服务上线，上线一个新服务：
	* 服务基本信息：
		* 服务名称：HelloWorld
		* 服务类型：NODEJS
		* 模板名称：tars.default
	* OBJ 部署信息：
		* OBJ名称：HttpObj
		* 是TARS：选择否（移除复选框）
		* 端口：自动生成端口

2. 安装 [tars-deploy](https://github.com/tars-node/deploy) 打包工具。
``` bash
npm i -g @tars/deploy
```

3. 根据业务需求以及您的喜好选择框架（！Tars.js 不限制您使用的框架！），编写业务代码：
``` js
const http = require('http');

const hostname = process.env.IP || '127.0.0.1';
const port = process.env.PORT || 3000;

const server = http.createServer((req, res) => {
  res.statusCode = 200;
  res.setHeader('Content-Type', 'text/plain');
  res.end('Hello World\n');
});

server.listen(port, hostname, () => {
  console.log(`Server running at http://${hostname}:${port}/`);
});
```
上述为 Node.js 官网 [HTTP Server](https://nodejs.org/en/about/) 例子，在 `Tars.js` 平台上运行只需要修改 IP 和端口即可。
这里在环境变量中获得到的 IP 和 PORT 即为您在第一步配置的 OBJ 部署信息中的 IP 与端口。

4. 在项目根目录下执行打包工具，即可生成发布包 HelloWorld.tgz：
``` bash
tars-deploy HelloWorld
```

5. 在 Tars 平台上选择您刚刚创建的服务 -> 发布管理 -> 手动上传发布包，然后发布此版本即可。

6. 通过您在第一步配置的 IP 与 端口即可访问您的服务。

## 可用模块列表

* [@tars/rpc](https://github.com/tars-node/rpc) : RPC 通信模块。
* [@tars/stream](https://github.com/tars-node/stream) : Tars 协议编解码模块。
* [@tars/logs](https://github.com/tars-node/logs)：日志组件，包含滚动（大小、时间）与染色日志。
* [@tars/config](https://github.com/tars-node/config)：用于获取服务配置文件。
* [@tars/monitor](https://github.com/tars-node/monitor)：提供服务监控、特性监控与PP监控上报支持。
* [@tars/notify](https://github.com/tars-node/notify)：用于（告警）消息上报。
* [@tars/utils](https://github.com/tars-node/utils)：辅助工具集合，包含配置文件与 Endpoint 解析器。
* [@tars/dyeing](https://github.com/tars-node/dyeing)：染色工具。

每个模块都有详细的 README 敬请阅读。