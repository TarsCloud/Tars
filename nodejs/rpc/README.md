
## 00 - 安装

> $ npm install @tars/rpc

## 01 - tars简介

tars是Tars4NodeJS项目底层的RPC调用框架，提供了一个多服务器进程间进行RPC调用的基础设施。简单来说我们可以用这个模块做这些事情：

- 使用tars2node将Tars文件翻译成客户端代理类代码后，供客户端调用任意的Tars服务。

- 使用tars2node将Tars文件翻译成服务端代码后，可以实现标准的Tars服务，该服务可被任意使用TARS/TUP协议的客户端直接调用。

- 远程日志、染色日志、属性上报、告警上报、tarsnode与服务通信等框架内服务。

- 创建自定义通信协议的客户端代理类(比如使用JSON格式的协议)。

- 创建自定义通信协议的服务端(比如使用JSON格式的协议)。

- 模块：@tars/registry，功能：根据服务Obj名字到主控查询该服务可用的IP列表。

tars分为客户端和服务器端两个部分。 客户端部分提供了rpc代理生成，消息路由和网络通讯等功能。 服务器端提供了远程服务暴露，请求派发，网络通讯等功能。

## 02 - 关于协议、Tars文件以及翻译工具tars2node的说明

**在深入学习tars的相关知识之前，我们先理清`TARS编码协议`、`TUP组包协议`、`TARS组包协议`三者之间的关系：**

- TARS编码协议是一种数据编解码规则，它将整形、枚举值、字符串、序列、字典、自定义结构体等数据类型按照一定的规则编码到二进制数据流中。对端接收到二进制数据流之后，按照相应的规则反序列化可得到原始数值。

- TARS编码协议使用一种叫做TAG的整型值(unsigned char)来标识变量，比如某个变量A的TAG值为100(该值由开发者自定义)，我们将变量值编码的同时，也将该TAG值编码进去。对端需要读取变量A的数值时，就到数据流中寻找TAG值为100的数据段，找到后按规则读出数据部分即是变量A的数值。

- TARS编码协议的定位是一套编码规则。tars协议序列化之后的数据不仅可以进行网络传输，同时还可以存储到数据库或者DCache中。

- TUP组包协议是TARS编码协议的上层封装，定位为通信协议。它使用变量名作为变量的关键字，编码时，客户端将变量名打包到数据流中；解码时，对端根据变量名寻找对应的数据区，然后根据数据类型对该数据区进行反序列化得到原始数值。

- TUP组包协议内置一个TARS编码协议的Map类型，该Map的关键字就是变量名，Map的值是将变量的数据值经过TARS编码序列化的二进制数据。

- TUP组包协议封装的数据包可以直接发送给Tars服务端，而服务端可以直接反序列化得到原始值。

- TARS组包协议是对RequestPacket（请求结构体）和ResponsePacket（结果结构体）使用TARS编码协议封装的通信协议。结构体包含比如请求序列号、协议类型、RPC参数序列化之后二进制数据等重要信息。

TARS编码协议的编解码规则以及Tars文件的编写方法，请参考 [@tars/steam文档](https://github.com/tars-node/stream/blob/master/README.md)。


**由Tars文件生成客户端或者服务端的代码的方法：**

tars2node工具可由cpp/tools/tars2node源码编译得到

通过`tars2node xxxx.tars --client `命令得到client端代理类

通过`tars2node xxxx.tars --server `命令得到server端实现类

**tars2node工具简介：**

学习Tars文件的编写方法之后，我们就可以自己来定义通信描述文件，然后使用tars2node的不同命令行选项生成不同的代码文件：

>$ tars2node Protocol.tars

上述命令将忽略interface描述段，只转换文件中定义的“常量”、“枚举值”、“结构体”等数据类型，供开发者当不使用Tars框架作为调用工具时的编解码库文件。生成的文件名称为“ProtocolTars.js”。

> $ tars2node Protocol.tars --client

上述命令不仅转换文件中定义的“常量”、“枚举值”、“结构体”等数据类型，同时将interface的描述段翻译成RPC调用框架。生成的文件名称为“ProtocolProxy.js”，该文件供调用方使用。开发者引入该文件之后，可以直接调用服务端的服务。具体的使用方法请参考“npm install tars”模块的说明文档。

> $ tars2node Protocol.tars --server

上述命令不仅转换文件中定义的“常量”、“枚举值”、“结构体”等数据类型，同时将interface的描述段翻译成服务端的接口文件。生成的文件名称为“Protocol.js”以及“ProtocolImp.js”，开发者不要改动“Protocol.js”，只需要继续完善“ProtocolImp.js”，实现文件中具体的函数，即可作为Tars服务端提供服务。具体的使用方法请参考“npm install tars”模块的说明文档。

tars2node支持的命令行参数及其作用：

| 选项 | 作用 |
| ------------- | ------------- |
| --tars-lib-path=\<DIRECTORY\>  | 指定@tars/stream模块的路径，默认使用NodeJS的目录。 |
| --with-tars | 是否允许“tars”作为命名空间（因为tars这个命名空间主要用于框架服务的tars文件定义）。 |
| --dir=\<DIRECTORY\> | 生成文件的输出目录。 |
| --relative | 限定所有的Tars文件都在当前目录寻找。|
| --tarsBase=\<DIRECTORY\> | 指定Tars文件的搜索目录。|
| --r | 转换嵌套的Tars文件（比如在A.tars中包含了B.tars和C.tars，使用该参数，在翻译A.tars的同时，也将B.tars和C.tars翻译成JS代码。|
| --client | 生成客户端的调用类代码。|
| --server | 生成服务端的框架代码。|

## 03 - tars示例和开发步骤

**文档看不下去了，马上动手实测！**

**第一步**，[下载rpc模块代码](https://github.com/tars-node/rpc)：

**第二步**，在rpc模块根目录

> $ npm install

**第三步**，在`/rpc/examples/rpc-tars/demo.1/server.node.1`目录下

> $ node main.js

启动rpc 服务端程序

**第四步**，在`/rpc/examples/rpc-tars/demo.1/client.node.proxy`目录下

> $ node main.js

启动rpc 客户端程序


**使用tars模块的开发步骤**

**第一步**，编写tars文件，定义客户端与服务端通信用到的常量、枚举值、结构体、函数等通信协议。我们使用如下tars文件作为示例：

> 一般而言Tars文件通常由`服务端开发`制定、维护和提供。

```c++
module TRom
{
    struct User_t
    {
        0 optional int id = 0;
        1 optional int score = 0;
        2 optional string name = "";
    };

    struct Result_t
    {
        0 optional int id = 0;
        1 optional int iLevel = 0;
    };

    interface NodeJsComm
    {
        int test();

        int getall(User_t stUser, out Result_t stResult);

        int getUsrName(string sUsrName, out string sValue1, out string sValue2);

        int secRequest(vector<byte> binRequest, out vector<byte> binResponse);
    };
};
```
将上述内容保存为：NodeJsComm.tars。


**第二步**，根据tars文件生成客户端的调用代码

> $ tars2node --client NodeJsComm.tars

**第三步**，客户端程序

```javascript
//STEP01 引入系统模块以及工具生成的代码
var Tars  = require("@tars/rpc").client;
var TRom = require("./NodeJsCommProxy.js").TRom;

//STEP02 初始化Tars客户端
//       该步骤非必选项，后续文档将介绍[tars].client.initialize函数在什么情况下需要调用以及它做了哪些工作
//       initialize函数只需调用一次，初始化之后全局可用
//       在演示程序中我们不需要使用过多的特性，所以先将其注释
//Tars.initialize("./config.conf");

//STEP03 生成服务端调用代理类实例
var prx = Tars.stringToProxy(TRom.NodeJsCommProxy, "TRom.NodeJsTestServer.NodeJsCommObj@tcp -h 127.0.0.1 -p 14002 -t 60000");

//STEP04 客户端调用采用Promise机制进行回调，这里编写成功以及失败的回调函数
var success = function (result) {
	console.log("result.response.costtime:",    		result.response.costtime);
	console.log("result.response.return:",      		result.response.return);
	console.log("result.response.arguments.stResult:",  result.response.arguments.stResult);
}

var error = function (result) {
	console.log("result.response.costtime:",			result.response.costtime);
    console.log("result.response.error.code:",         	result.response.error.code);
    console.log("result.response.error.message:",       result.response.error.message);
}

//STEP05 初始化接口参数，开始调用RPC接口
var stUser = new TRom.User_t();
stUser.name = "tencent-mig";

prx.getall(stUser).then(success, error).done();
```
将上述代码保存为client.js，使用如下命令即可调用服务端。

> $ node client.js

>result.response.costtime: 7

>result.response.return: 200

>result.response.arguments.stResult: { id: 10000, iLevel: 10001 }

如果我们只是调用方，写到这里已经足矣。按照刚才的示例，拿到相应Tars文件我们就可以调用C++的Tars服务、Java的Tars服务或者NodeJS的Tars服务。

**第四步**，实现一个NodeJS版本的Tars服务。

首先，完形填空。完成Tars文件中定义的RPC函数，实现自己的业务逻辑。

tars2node的`--erver`选项将Tars文件生成服务端的代码。使用该选项翻译工具不仅转换文件中定义的“常量”、“枚举值”、“结构体”等数据类型，同时将interface描述段翻译成服务端的接口文件。主要生成两个文件，比如在当前例子中会生成`NodeJsComm.js`和`NodeJsCommImp.js`。开发者 **不需要也尽量不要** 改动`NodeJsComm.js`，该文件主要实现了结构体编解码、函数参数编解码、函数分发等功能。`NodeJsCommImp.js`继承与`NodeJsComm.js`，该文件主要供开发者填补定义的RPC函数，实现业务逻辑。

```javascript
var TRom = require('./NodeJsComm.js').TRom;
module.exports.TRom = TRom;

TRom.NodeJsCommImp.prototype.initialize = function ( ) {
    //TODO::

}

TRom.NodeJsCommImp.prototype.test = function (current) {
    //TODO::

}

TRom.NodeJsCommImp.prototype.getall = function (current, stUser, stResult) {
    //TODO::
	//初始时，每个RPC函数都为空，需要开发者自己完形填空，补齐这里缺失的业务逻辑。
	//补齐业务逻辑之后，开发者调用current的sendResponse函数，返回数据给调用方。
	//需要注意:每个函数的sendResponse都是不一样的，它的参数与当前函数的 返回值 和 出参 相对应。
	//         如果当前函数有返回值，那么current.sendResponse的第一个参数应该是该返回。示例中当前函数的返回值为int类型，我们返回200作为示例。
	//         解决返回值的问题之后，我们按顺序写入当前的出参即可。参数的编解码和网络传输由框架解决。

	stResult.id		= 10000;
	stResult.iLevel	= 10001;

	current.sendResponse(200, stResult);
}

TRom.NodeJsCommImp.prototype.getUsrName = function (current, sUsrName, sValue1, sValue2) {
    //TODO::

}

TRom.NodeJsCommImp.prototype.secRequest = function (current, binRequest, binResponse) {
    //TODO::

}
```

接下来，创建一个服务入口文件。它主要负责读取配置文件、配置端口、设置协议解析器、启动服务等等工作。

```javascript
var Tars  = require("@tars/rpc").server;
var TRom = require("./NodeJsCommImp.js").TRom;

var svr  = Tars.createServer(TRom.NodeJsCommImp);
svr.start({
    name     : "TRom.NodeJsTestServer.NodeJsCommObjAdapetr",
    servant  : "TRom.NodeJsTestServer.NodeJsCommObj",
    endpoint : "tcp -h 127.0.0.1 -p 14002 -t 10000",
    protocol : "tars",
	maxconns : 200000
});

console.log("server started.");
```
将上述代码保存为server.js，使用如下命令启动。

> $ node server.js

>server started.


## 04 - 客户端的初始化函数[tars].client.initialize

在演示代码中我们提到initialize不一定要显示调用，我们用其他方式同样可以设置我们需要的参数。

首先我们看下配置文件的格式和必要参数：

```
<tars>
    <application>
        <client>
            locator = tars.tarsregistry.QueryObj@tcp -h 172.27.208.171 -p 17890 ##定义主控地址
            async-invoke-timeout=60000										  ##异步调用的超时时间（ms）
        </client>
    </application>
</tars>
```

这个配置文件正是由tarsnode生成的，我们主要使用"tars.application.client.locator"和"tars.application.client.async-invoke-timeout"这个两个配置项。

> 什么情况下可以不用调用initialize函数？

> 如果我们在生成服务端代理时，每个服务端都使用直连的模式，也就是在stringToProxy中指定IP地址就可以不用初始化了。

除了使用配置文件设置这两个参数之外，我们可以调用[tars].client对外暴露的函数进行设置：

```
var Tars  = require("@tars/rpc").client;

Tars.set("locator", "tars.tarsregistry.QueryObj@tcp -h 172.27.208.171 -p 17890");
Tars.set("timeout", 60000);
```

上述的调用方法，与使用initialize+配置文件的方式等价。

## 05 - Tars服务的创建方法

tars有三种方法创建一个标准的Tars服务：

**第一种**，使用tarsnode生成的配置文件。

使用这种方法与TARS4C++的使用方式一样。

首先需要我们在TARS管理平台配置服务的Obj，然后在启动程序时由tarsnode生成包含监听端口的配置文件，然后服务框架再依赖该配置绑定端口+启动服务。

![tars服务创建](http://3gimg.qq.com/trom_s/tarsdoc/2.png)

tarsnode生成的配置文件类似与如下：

```
<tars>
	<application>
		enableset=n
		setdivision=NULL
		<server>
			node=tars.tarsnode.ServerObj@tcp -h 127.0.0.1 -p 19386 -t 60000
			app=TRom
			server=NodeJsTestServer
			localip=127.0.0.1
			netthread=2
			local=tcp -h 127.0.0.1 -p 10002 -t 3000
			basepath=/usr/local/app/tars/tarsnode/data/MTT.NodeJSTest/bin/
			datapath=/usr/local/app/tars/tarsnode/data/MTT.NodeJSTest/data/
			logpath=/usr/local/app/tars/app_log//
			logsize=15M
			config=tars.tarsconfig.ConfigObj
			notify=tars.tarsnotify.NotifyObj
			log=tars.tarslog.LogObj
			deactivating-timeout=3000
			openthreadcontext=0
			threadcontextnum=10000
			threadcontextstack=32768
			closeout=0
			<TRom.NodeJsTestServer.NodeJsCommObjAdapter>
				allow
				endpoint=tcp -h 127.0.0.1 -p 14002 -t 60000
				handlegroup=TRom.NodeJsTestServer.NodeJsCommObjAdapter
				maxconns=200000
				protocol=tars
				queuecap=10000
				queuetimeout=60000
				servant=TRom.NodeJsTestServer.NodeJsCommObj
				shmcap=0
				shmkey=0
				threads=5
			</TRom.NodeJsTestServer.NodeJsCommObjAdapter>
		</server>
		<client>
			locator=tars.tarsregistry.QueryObj@tcp -h 127.0.0.1 -p 17890:tcp -h 127.0.0.1 -p 17890
			refresh-endpoint-interval=60000
			stat=tars.tarsstat.StatObj
			property=tars.tarsproperty.PropertyObj
			report-interval=60000
			sample-rate=1000
			max-sample-count=100
			sendthread=1
			recvthread=1
			asyncthread=3
			modulename=TRom.NodeJsTestServer
			async-invoke-timeout=60000
			sync-invoke-timeout=3000
		</client>
	</application>
</tars>
```

我们使用该配置文件创建一个服务，代码如下：

```javascript
//STEP01 引入关键模块
var Tars       = require("@tars/rpc");
var TRom      = require("./NodeJsCommImp.js");

//STEP02 创建一个服务的实例
//注意这里的配置，在正式环境时，用 process.env.TARS_CONFIG 来表示配置文件的路径
//也就是：svr.initialize(process.env.TARS_CONFIG, function (server){ ... });
var svr = new Tars.server();
svr.initialize("./TRom.NodeJsTestServer.config.conf", function (server){
    server.addServant(TRom.NodeJsCommImp, server.Application + "." + server.ServerName + ".NodeJsCommObj");
});

//STEP03 上步初始化服务之后，开始启动服务
svr.start();
```

**第二种**，显示化服务端信息

```
//STEP01 引入关键模块
var Tars  = require("@tars/rpc").server;
var TRom = require("./NodeJsCommImp.js").TRom;

//STEP02 创建一个服务的实例
//注意这里的“endpoint”和“protocol”为必选项，格式必须如下示例相同
var svr  = Tars.createServer(TRom.NodeJsCommImp);
svr.start({
    name     : "TRom.NodeJsTestServer.AdminObjAdapetr",
    servant  : "TRom.NodeJsTestServer.AdminObj",
    endpoint : "tcp -h 127.0.0.1 -p 14002 -t 10000",
    maxconns : 200000,
    protocol : "tars"
});

console.log("server started.");
```

**第三种**，从tarsnode生成的配置文件中，选取部分服务来启动

```
//STEP01 引入关键模块
var Tars   = require("@tars/rpc");
var TRom  = require("./NodeJsCommImp.js");

Tars.server.getServant("./TRom.NodeJsTestServer.config.conf").forEach(function (config){
    var svr, map;
    map = {
        'TRom.NodeJsTestServer.NodeJsCommObj' : TRom.NodeJsCommImp
    };

    svr = Tars.server.createServer(map[config.servant]);
    svr.start(config);
});

```

## 06 - Tars客户端的实现原理

![客户端系统架构](http://3gimg.qq.com/trom_s/tarsdoc/client.png)


## 07 - Tars服务端的实现原理

![服务端系统架构](http://3gimg.qq.com/trom_s/tarsdoc/server.png)


## 08 - tars作为客户端调用第三方协议服务的示例

首先我们先定一个双方认可的通信协议，比如我们以Json格式作为通信协议，格式假定：

```javascript
//客户端 --> 服务端
{
	P_RequestId : 0, 					//本次调用的序列号
	P_FuncName  : 'test'				//本次调用的函数名称
	P_Arguments : ['aa', 'bb'.....]		//本次调用的函数参数
}

//客户端 <-- 服务端
{
	P_RequestId : 0, 					//本次调用的序列号
	P_FuncName  : 'test'				//本次调用的函数名称
	P_Arguments : ['ee', 'ff'.....]		//本次调用的返回参数
}
```

实现协议解析类：

```javascript
//将文件保存为Protocol.js
var EventEmitter = require("events").EventEmitter;
var util         = require("util");

var stream = function () {
    EventEmitter.call(this);
    this._data = undefined;
    this._name = "json";
}
util.inherits(stream, EventEmitter);

stream.prototype.__defineGetter__("name", function () { return this._name; });

module.exports = stream;

/**
 * 根据传入数据进行打包的方法
 * @param request
 * request.iRequestId : 框架生成的请求序列号
 * request.sFuncName  : 函数名称
 * request.Arguments  : 函数的参数列表
 */
stream.prototype.compose = function (data) {
    var str = JSON.stringify({
		P_RequestId : data.iRequestId,
		P_FuncName  : data.sFuncName,
		P_Arguments : data.Arguments
	});

    var len = 4 + Buffer.byteLength(str);
    var buf = new Buffer(len);
    buf.writeUInt32BE(len, 0);
    buf.write(str, 4);

    return buf;
}

/**
 *
 * 网络收取包之后，填入数据判断是否完整包
 * @param data 传入的data数据可能是TCP的各个分片，不一定是一个完整的数据请求，协议解析类内部做好数据缓存工作
 *
 * 当有一个完整的请求时，解包函数抛出事件，需按照如下格式补充事件的数据成员：
 *
 * {
 *     iRequestId :  0,	//本次请求的序列号
 *     sFuncName  : "",	//本次请求的函数名称
 *     Arguments  : []	//本次请求的参数列表
 * }
 *
 */
stream.prototype.feed = function (data) {
    var BinBuffer = data;
    if (this._data != undefined) {
        var temp = new Buffer(this._data.length + data.length);
        this._data.copy(temp, 0);
        data.copy(temp, this._data.length);
        this._data = undefined;
        BinBuffer = temp;
    }

    for (var pos = 0; pos < BinBuffer.length; ) {
        if (BinBuffer.length - pos < 4) {
            break;
        }
        var Length = BinBuffer.readUInt32BE(pos);
        if (pos + Length > BinBuffer.length) {
            break;
        }
        var result   = JSON.parse(BinBuffer.slice(pos + 4, pos + Length).toString());
        var request  =
        {
            iRequestId : result.P_RequestId,
			sFuncName  : result.P_FuncName,
            Arguments  : result.P_Arguments
        };

        this.emit("message", request);
        pos += Length;
    }

    if (pos != BinBuffer.length) {
        this._data = new Buffer(BinBuffer.length - pos);
        BinBuffer.copy(this._data, 0, pos);
    }
}

/**
 * 重置当前协议解析器
 */
stream.prototype.reset = function () {
    delete this._data;
    this._data = undefined;
}
```

客户端使用该协议解析器，调用服务端的代码：

```
var Tars      = require("@tars/rpc").client;
var Protocol = require("./ProtocolClient.js");

var prx      = Tars.stringToProxy(Tars.ServantProxy, "test@tcp -h 127.0.0.1 -p 12306 -t 60000");
prx.setProtocol(Protocol);
prx.rpc.createFunc("echo");

var success = function (result) {
    console.log("success");
    console.log("result.response.costtime:",  result.response.costtime);
    console.log("result.response.arguments:", result.response.arguments);
}

var error = function (result) {
    console.log("error");
    console.log("result.response.error.code:",    result.response.error.code);
    console.log("result.response.error.message:", result.response.error.message);
}

prx.rpc.echo("tencent", "mig", "abc").then(success, error);

```

另外，如果想要请求根据某个特征来发到某台固定的机器，可以使用如下方法：
``` javascript
prx.getUsrName(param,{
    hashCode:userId
}).then(success, error).done();

```
获得客户端代理对象之后，调用服务端接口函数，此时可以传入传入hashCode参数，tars会根据hashCode来把请求分配到连接列表中固定的一台机器。
需要注意的是：
- 这里的userId是一个数字，二进制、八进制、十六进制都可以，但是转换成10进制的数字最好在16位数以下。javascript处理高精度数字会有精度丢失的问题。
- 服务端机器列表固定时，同一hashCode会被分配到固定的一台机器，当服务端机器列表发生变化时，会重新分配hashCode对应的机器。

## 09 - tars作为第三方协议服务的示例

首先实现RPC函数处理类，注意框架的分发逻辑：

A.如果客户端传来的函数名，是处理类的函数，那么框架有限调用对应函数

B.如果客户端传来的函数不是处理的函数，那么调用该处理类的 **`onDispatch`**函数，由该函数负责处理该请求

C.如果也没有onDispatch函数，则报错

```
//将该文件保存为：EchoHandle.js
var Handle = function () {

}

Handle.prototype.initialize = function () { }
Handle.prototype.echo = function (current, v1, v2, v3) {
    console.log("EchoHandle.echo::", v1, v2, v3);

    current.sendResponse("TX", "TX-MIG");
}

Handle.prototype.onDispatch = function (v1, v2, v3) {
    console.log("EchoHandle.onDispatch::", v1, v2, v3);
}

module.exports = Handle;
```

服务端启动函数的代码示例：
```
var Tars         = require("@tars/rpc").server;
var Protocol    = require("./ProtocolClient.js");
var Handle      = require("./EchoHandle.js");

var svr = Tars.createServer(Handle);
svr.start({
    endpoint : "tcp -h 127.0.0.1 -p 12306 -t 10000",
    protocol : Protocol
});

```

## 09 - tars 客户端请求参数
tars客户端代理对象调用协议接口函数时，最后一个参数可以传入一个对象，配置一些请求参数，目前支持4种请求参数。
### dyeing
请求染色对象。染色对象生成方式详见[@tars/dyeing](https://github.com/tars-node/dyeing "@tars/dyeing")。
### context
请求上下文对象。
### packetType
请求类型参数，1为单向请求，其他为普通请求
### hashCode
请求hash，必须填入js精度安全范围内的数字（Math.pow(2, 53) - 1）

示例：
``` javascript
prx.getUsrName(param,{
	dyeing:dyeingObj,
	context:{xxx:xxx},
	packetType:1,
    hashCode:userId
}).then(success, error);

```
