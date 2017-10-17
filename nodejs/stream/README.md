
# 00 - 安装
> $ npm install @tars/stream

# 01 - stream模块基本介绍和使用方法
stream模块用作Tars(tars/TUP)基础协议编解码库，使用该模块可以基于tars协议描述格式对数据流进行编解码，并能够与目前使用tars协议的TARS服务端以及终端进行无障碍通信。

tars编解码模块工作流方式一般有如下三种：

### 第一种，以tars文件作为调用方和服务方的通信桥梁（双方约定最终协议以tars文件为准）。
该tars文件也就是我们常说的以".tars"结尾的协议描述文件。

该tars文件一般由后台开发制定，前台开发需向后台开发索求经评审确认的tars文件，然后经工具转换成适用于NodeJS的编解码源代码文件。

```c++
module TRom
{
    struct User_t
    {
        0 optional int id = 0;
        1 optional float score = 0;
        2 optional string name = "";
    };

    struct Result_t
    {
        0 optional int id = 0;
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
比如，我们将如上内容保存为“Protocol.tars”后，可以使用如下的命令生成不同的文件：

> $ tars2node Protocol.tars

上述命令将忽略interface描述段，只转换文件中定义的“常量”、“枚举值”、“结构体”等数据类型，供开发者当不使用Tars框架作为调用工具时的编解码库文件。生成的文件名称为“Protocol.js”。


> $ tars2node Protocol.tars --client

上述命令不仅转换文件中定义的“常量”、“枚举值”、“结构体”等数据类型，同时将interface的描述段翻译成RPC调用框架。生成的文件名称为“ProtocolProxy.js”，该文件供调用方使用。开发者引入该文件之后，可以直接调用服务端的服务。具体的使用方法请参考“npm install rpc”模块的说明文档。


> $ tars2node Protocol.tars --server

上述命令不仅转换文件中定义的“常量”、“枚举值”、“结构体”等数据类型，同时将interface的描述段翻译成服务端的接口文件。生成的文件名称为“Protocol.js”以及“ProtocolImp.js”，开发者不要改动“Protocol.js”，只需要继续完善“ProtocolImp.js”，实现文件中具体的函数，即可作为Tars服务端提供服务。具体的使用方法请参考“npm install rpc”模块的说明文档。


### 第二种，没有协议描述文件，需要我们自己手工书写编解码代码时。
比如服务后台提供购买某件商品的功能，它需要“用户号码”、“用户昵称”、“商品编号”、“商品数量”等四个参数。
后台对这四个参数的编号（也就是tars中所指的tag）分别为0、1、2、3。
```javascript
//第一步，引入tars/TUP编解码库
var Tars = require("@tars/stream");

//第二步，客户端按照服务端要求，对输入参数进行编码
var ost = new Tars.OutputStream();
ost.writeUInt32(0, 155069599);		//写入“用户号码”；在服务端“0”代表“用户号码”。
ost.writeString(1, "KevinTian");	//写入“用户昵称”；在服务端“1”代表“用户昵称”。
ost.writeUInt32(2, 1002121);		//写入“商品编号”；在服务端“2”代表“商品编号”。
ost.writeUInt32(3, 10);				//写入“商品数量”；在服务端“3”代表“商品数量”。

//第三步，客户端将打包后的二进制Buffer发送给服务端
send ( ost.getBinBuffer().toNodeBuffer() ) to server

//第四步，服务端从客户端接收完整的请求二进制Buffer
recv ( var requestBuffer = new Buffer() ) from client

//第五步，将该请求进行解码反序列化
var ist = new Tars.InputStream(new Tars.BinBuffer(requestBuffer));

var uin  = ist.readUInt32(0, true);	//根据编号“0”读取“用户号码”。
var name = ist.readString(1, true);	//根据编号“1”读取“用户昵称”。
var gid  = ist.readUInt32(2, true);	//根据编号“2”读取“商品编号”。
var num  = ist.readUInt32(3, true);	//根据编号“3”读取“商品数量”。

//第六步，根据相关传入参数进行相应的逻辑操作
console.log("name:", name);
console.log("num :", num);
......
```
### 第三种，服务端接受TUP协议格式的数据。
```
//第一步，引入tars/TUP编解码库
var Tars = require("@tars/stream");

//第二步，客户端按照服务端要求，对输入参数进行编码
var tup_encode = new Tars.Tup();
tup_encode.writeUInt32("uin",  155069599);		//服务端接口函数“用户号码”的变量名称为“uin”。
tup_encode.writeString("name", "KevinTian");	//服务端接口函数“用户昵称”的变量名称为“name”。
tup_encode.writeUInt32("gid",  1002121);		//服务端接口函数“商品编号”的变量名称为“gid”。
tup_encode.writeUInt32("num",  10);				//服务端接口函数“商品数量”的变量名称为“uum”。

var BinBuffer = tup_encode.encode(true);

//第三步，客户端将打包后的二进制Buffer发送给服务端
send ( BinBuffer.toNodeBuffer() ) to server

//第四步，服务端从客户端接收完整的请求二进制Buffer
recv ( var requestBuffer = new Buffer() ) from client

//第五步，将该请求进行解码反序列化
var tup_decode = new Tars.Tup();
tup_decode.decode(new Tars.BinBuffer(requestBuffer));

var uin  = tup_decode.readUInt32("uin");		//服务端根据变量名“uin”读取“用户号码”。
var name = tup_decode.readString("name");		//服务端根据变量名“name”读取“用户昵称”。
var num  = tup_decode.readUInt32("num");		//服务端根据变量名“gid”读取“商品编号”。
var gid  = tup_decode.readUInt32("gid");		//服务端根据变量名“num”读取“商品数量”。

//第六步，根据相关传入参数进行相应的逻辑操作
console.log("name:", name);
console.log("num :", num);
......
```

# 02 - stream支持的数据类型以及使用方法
**基本数据类型**

| 数据类型 | 对应C++语言的数据类型 |
| ------------- | ------------- |
| 布尔值 | bool |
| 整型 | char(int8)、short(int16)、int(int32)、long long(int64) |
| 整型 | unsigned char(uint8)、unsigned short(uint16)、unsigned int(uint32) |
| 数值 | float(32位)、double(64位) |
| 字符串 | std::string |

**复杂数据类型**

| 数据类型 | 对应C++语言的数据类型 |
| ------------- | ------------- |
| 结构体 | struct（在Tars框架中需要使用tars2node根据tars文件来生成Javascript中的类）|
| 二进制Buffer | vector&lt;char&gt;（在NodeJs中使用[stream].BinBuffer类型来模拟）|
| 数组 | vector&lt;DataType&gt;（在NodeJs中使用[stream].List(vproto)类型来模拟）|
| 词典 | map&lt;KeyType, DataType&gt;（在NodeJs中使用[stream].Map(kproto, vproto)类型来模拟）|

**关于NodeJs中数据类型的特别说明**

**[1]：** “复杂数据类型”与“基本数据类型”，或者“复杂数据类型”与“复杂数据类型”组合使用可以组成其他高级数据类型。

**[2]：** 虽然NodeJS中支持Float和Double数据类型，但我们不推荐使用，因为在序列化和反序列化之后，数值存在精度损失，某些情况下会对业务逻辑造成伤害。

**[3]：** 我们这里实现的64位整形实际上是伪64位，在NodeJs中它的原形仍然是Number。

我们都知道Js中的Number类型采用IEEE754双精度浮点数标准来表示。IEEE754规定有效数字第一位默认为1，再加上后面的52位来表示数值。

也就是说IEEE754提供的有效数字的精度为53个二进制位，这就意味着NodeJs的Number数值或者说我们实现的Int64数据类型只能精确表示绝对值小于2的53次方的整数。

**[4]：** 在Javascript中String类型是Unicode编码，在tars编解码时我们将其转换成了UTF8编码格式；

后台服务程序接受到的字符串是UTF8编码，如果需要按照GBK编码的方式处理字符串，需要后台程序先做下转码（UTF8->GBK）；

后台服务程序如果使用的是GBK，发送字符串之前，需要将其转成UTF8编码。

# 03 - 基本类型使用方法
```javascript
//必须引入stream模块
var Tars = require("@tars/stream");

//使用Tars.OutputStream对数据进行序列化
var os = new Tars.OutputStream();

os.writeBoolean(0, false);
os.writeInt8(1, 10);
os.writeInt16(2, 32767);
os.writeInt32(3, 0x7FFFFFFE);
os.writeInt64(4, 8589934591);
os.writeUInt8(5, 200);
os.writeUInt16(6, 65535);
os.writeUInt32(7, 0xFFFFFFEE);
os.writeString(8, "我的测试程序");

//使用Tars.InputStream对数据进行反序列化
var is = new Tars.InputStream(os.getBinBuffer());

var tp0 = is.readBoolean(0, true, false);
console.log("BOOLEAN:", tp0);

var tp1 = is.readInt8(1, true, 0);
console.log("INT8:", tp1);

var tp2 = is.readInt16(2, true, 0);
console.log("INT16:", tp2);

var tp3 = is.readInt32(3, true, 0);
console.log("INT32:", tp3);

var tp4 = is.readInt64(4, true, 0);
console.log("INT64:", tp4);

var tp5 = is.readUInt8(5, true, 0);
console.log("UINT8:", tp5);

var tp6 = is.readUInt16(6, true, 0);
console.log("UINT16:", tp6);

var tp7 = is.readUInt32(7, true, 0);
console.log("UINT32:", tp7);

var tp8 = is.readString(8, true, "");
console.log("STRING:", tp8);
```

# 04 - 复杂类型前传 - 用于表示复杂类型的类型原
首先，我们理解下什么是 **类型原型**！

在C++中，我们可以按如下方法声明一个字符串的容器向量：
```cpp
#include <string>
#include <vector>

std::vector<std::string> vec;
vec.push_back("qzone");
vec.push_back("wechat");
```
其中std::vector<std::string>，std::vector表示容器类型，而std::string则表示该容器所容纳的 **类型原型** 。

那我们如何在NodeJs中表示该类型？并能使之与tars的编解码库无缝的融合？

为了解决这个问题，我们使用如下的方法对std::vector进行模拟，以达到上述C++代码所能完成的功能：
```javascript
var Tars = require("@tars/stream");

var abc = new Tars.List(Tars.String);
abc.push("qzone");
abc.push("wechat");
```

其中Tars.List(Tars.String)，Tars.List表示数组类型，而Tars.String则用来表示该容器所容纳的 **类型原型**。

**至此，我们明白类型原型主要是用来与复杂数据类型组合，表示更加复杂的数据类型。**

目前的版本中，我们支持如下的类型原型定义：

| 数据类型 | 描述 |
| ------------- | ------------- |
| 布尔值       | [stream].Boolean |
| 整型         | [stream].Int8, [stream].Int16, [stream].32, [stream].64, [stream].UInt8, [stream].UInt16, [stream].UInt32 |
| 数值         | [stream].Float, [stream].Double |
| 字符串       | [stream].String |
| 枚举值       | [stream].Enum |
| 数组         | [stream].List |
| 字典         | [stream].Map |
| 二进制Buffer | [stream].BinBuffer |

为了大家更加清晰的理解该概念，我们提前描述一部分复杂类型的在NodeJs中的表示方法。

数据类型的详细使用方法，请参考后续的详细说明。
```javascript
var Tars = require("@tars/stream");

//c++语法：std::vector<int>
var abc = new Tars.List(Tars.Int32)
abc.push(10000);
abc.push(10001);

//c++语法：std::vector<std::vector<std::string> >
var abc = new Tars.List(Tars.List(Tars.String));
var ta  = new Tars.List(Tars.String);
    ta.push("ta1");
    ta.push("ta2");
var tb  = new Tars.List(Tars.String);
    tb.push("tb1");
    tb.push("tb2");
abc.push(ta);
abc.push(tb);

//c++语法：std::map<std::string, std::string>
var abc = new Tars.Map(Tars.String, Tars.String);
abc.insert("key1", "value1");
abc.insert("key2", "value2");

//c++语法：std::map<std::string, std::vector<string> >
var abc = new Tars.Map(Tars.String, Tars.List(Tars.String));
var ta  = new Tars.List(Tars.String);
    ta.push("ta1");
    ta.push("ta2");
var tb  = new Tars.List(Tars.String);
    tb.push("tb1");
    tb.push("tb2");
abc.insert("key_a", ta);
abc.insert("key_b", tb);

//c++语法：std::vector<char>
var abc = new Tars.BinBuffer();
abc.writeInt32(10000);
abc.writeInt32(10001);
```

# 05 - 复杂类型 - struct（结构体）的使用方法说明
```c++
module Ext
{
    struct ExtInfo  {
        0 optional string sUserName;
        1 optional map<string, vector<byte> > data;
        2 optional map<string, map<string, vector<byte> > > cons;
    };
};
```
将上述内容保存为文件“Demo.tars”，然后使用命令“tars2node Demo.tars”生成编解码文件“Demo.js”。

“Demo.js”内容如下所示：
```javascript
var TarsStream = require("@tars/stream");

var Ext = Ext || {};
module.exports.Ext = Ext;

Ext.ExtInfo = function() {
    this.sUserName = "";
    this.data = new TarsStream.Map(TarsStream.String, TarsStream.BinBuffer);
    this.cons = new TarsStream.Map(TarsStream.String, TarsStream.Map(TarsStream.String, TarsStream.BinBuffer));
};
Ext.ExtInfo._write = function (os, tag, value) { os.writeStruct(tag, value); }
Ext.ExtInfo._read  = function (is, tag, def) { return is.readStruct(tag, true, def); }
Ext.ExtInfo._readFrom = function (is) {
    var tmp = new Ext.ExtInfo();
    tmp.sUserName = is.readString(0, false, "");
    tmp.data = is.readMap(1, false, TarsStream.Map(TarsStream.String, TarsStream.BinBuffer));
    tmp.cons = is.readMap(2, false, TarsStream.Map(TarsStream.String, TarsStream.Map(TarsStream.String, TarsStream.BinBuffer)));
    return tmp;
};
Ext.ExtInfo.prototype._writeTo = function (os) {
    os.writeString(0, this.sUserName);
    os.writeMap(1, this.data);
    os.writeMap(2, this.cons);
};
Ext.ExtInfo.prototype._equal = function (anItem) {
    return anItem.sUserName === this.sUserName
    && anItem.data === this.data
    && anItem.cons === this.cons;
}
Ext.ExtInfo.prototype._genKey = function () {
    if (!this._proto_struct_name_) {
        this._proto_struct_name_ = 'STRUCT' + Math.random();
    }
    return this._proto_struct_name_;
}
Ext.ExtInfo.prototype.toBinBuffer = function () {
    var os = new TarsStream.OutputStream();
    this._writeTo(os);
    return os.getBinBuffer();
}
Ext.ExtInfo.create = function (is) {
    return Ext.ExtInfo._readFrom(is);
}
```

**对“module Ext”的说明**

Ext在C++中就是命名空间，在Javascript中我们将它翻译成一个Object，该命名空间下所有的“常量”、“枚举值”、“结构体”、“函数”都挂接在该Object之下。

**tars文件中描述的结构体的表示方法**

首先，结构体翻译成一个Object。翻译程序根据数据类型以及tars文件中定义的默认值，生成数据成员。除tars中定义的数据成员之外，根据编解码的需要，翻译程序为结构体添加了若干辅助函数。这些函数如_writeTo，在需要将结构体序列化成数据流的地方，被编解码库调用，该函数逐个将数据成员写入数据流中。

**翻译程序默认添加的辅助函数**

| 方法  | 限制 | 描述 |
| ------------- | ------------- | ------------- |
| \_write | 开发者不可用 | 静态函数。当结构体用作类型原型时使用。|
| \_read | 开发者不可用 | 静态函数。当结构体用作类型原型时使用。|
| \_readFrom | 开发者不可用 | 静态函数。从数据流中读取结构体的数据成员值，并生成一个权限的结构体示例返回。|
| \_writeTo | 开发者不可用 | 成员函数。将当前结构体的数据成员写入指定的数据流中。|
| \_equal | 开发者不可用 | 成员函数。将当前结构体用作字典类型Key值时的比较函数。|
| \_genKey | 开发者不可用 | 成员函数。将当前结构体用作字典类型Key值时，内部使用该函数获得当前结构体的别名。|
| toBinBuffer | 开发者可用 | 成员函数。将当前结构体序列化成二进制Buffer，返回值类型为require("@tars/stream").BinBuffer。|
| create | 开发者可用 | 成员函数。从数据流中返回一个全新的结构体。|

**结构体的使用示例**

我们演示结构体在三个典型场景的使用方法：

**第一种场景：** 当结构体用作RPC函数的参数时。

由于rpc框架会自动对参数进行序列化，所以我们无需关心编解码，只需要按照普通的类一样，先new后赋值，然后传入参数直接调用RPC函数即可。

假如服务端有个RPC如下定义：

```c++
module TRom
{
    struct Param  {
        0 optional string sUserName;
        1 optional int iId;
    };

	interface process {
		int getUserLevel(Param userInfo, out int iLevel);
	};
};}
```

安装上述方法生成tars编解码文件（生成文件名称为：Protocol.js）之后，按如下方法调用对端服务：

```javascript
var Tars  = require("@tars/rpc").client;
var TRom = require("./Protocol.js").TRom;

var prx = Tars.stringToProxy(TRom.NodeJsCommProxy, "TRom.NodeJsTestServer.NodeJsCommObj@tcp -h 10.12.22.13 -p 8080  -t 60000");

var usr = new TRom.Param();
usr.sUserName = "KevinTian";
usr.iId       = 10000;

prx.getUserLevel(usr).then(function (result) {
	console.log("success:", result);
}, function (result) {
	console.log("error:", result);
}).done();
```

**第二种场景：** 对端非标准rpc框架，接受序列化的数据流作为参数。

在这种场景下需要我们自己对结构体进行序列化。还是以上面的tars文件作为例子，一般的方法如下：

```
//客户端安装如下方法进行打包，然后将打包后的二进制数据流发送到服务端
var Tars  = require("@tars/stream");
var TRom = require("./Protocol.js").TRom;

var usr  = new TRom.Param();
usr.sUserName = "KevinTian";
usr.iId       = 10000;

var os = new Tars.OutputStream();
os.writeStruct(1, usr);

//打包并得到发送的二进制数据流
var toSendBuffer = os.getBinBuffer().toNodeBuffer();
```

客户端将toSendBuffer发送给服务端，并且服务端接受完毕之后按如下方法进行解码：

```javascript
var Tars  = require("@tars/stream");
var TRom = require("./Protocol.js").TRom;

var is   = new Tars.InputStream(new Tars.BinBuffer(toSendBuffer));
var usr  = is.readStruct(1, true, TRom.Param);

console.log("TRom.Param.sUserName:", usr.sUserName);
console.log("TRom.Param.iId:", usr.iId);
```

**第三种场景：** 对方服务要求数据流使用Tup协议，并且已经约定好了各个变量的名字。我们可以按如下的方法进行编解码：

```javascript
//客户端根据约定的名字，将结构体放入Tup中
var Tars  = require("@tars/stream");
var TRom = require("./Protocol.js").TRom;

var usr  = new TRom.Param();
usr.sUserName = "KevinTian";
usr.iId       = 10000;

var tup_encode = new Tars.Tup();
tup_encode.writeStruct("userInfo",  usr);

//打包并得到发送的二进制数据流
var toSendBuffer = tup_encode.encode(true).toNodeBuffer();
```

客户端将toSendBuffer发送给服务端，并且服务端接受完毕之后按如下方法进行解码：

```javascript
var Tars  = require("@tars/stream");
var TRom = require("./Protocol.js").TRom;

var tup_decode = new Tars.Tup();
tup_decode.decode(new Tars.BinBuffer(toSendBuffer));

var usr  = tup_decode.readStruct("userInfo", TRom.Param);

console.log("TRom.Param.sUserName:", usr.sUserName);
console.log("TRom.Param.iId:", usr.iId);
```

# 06 - 复杂类型 - vector（数组）的使用方法说明
由于Javascript原生的Array不支持tars中的一些特殊化操作，所以我们对它进行了一次封装。开发者可按下述的代码理解：

```javascript
[stream].List = function(proto)
{
    this.proto = proto;
    this.value = new Array();
    this.push  = function (value) { this.value.push(value); }
    ......
}
```

#### [stream].List 对象属性
| 属性  | 描述 |
| ------------- | ------------- |
| value  | Js中的Array数据类型。Tars.List实际是基于该Array进行的上层封装。|
| length | 返回数组中元素的数目。|

#### [stream].List 对象方法
| 方法  | 描述 |
| ------------- | ------------- |
| at  | 返回数组中指定位置的元素。 |
| push | 向数组的末尾添加一个元素。|
| forEach | 当前数组的遍历方法，具体使用方法请参考后面的示例。 |
| toObject | 将List实例转化成基本的数据对象，具体使用方法请参考后面的示例。 |
| readFromObject | 将传入的数组处理后push到List实例中，具体使用方法请参考后面的示例。 |

proto是Vector的类型原型（类型原型决定了在对Vector编解码时采用的方法，所以声明Vector的时候必须传入正确的类型原型）。

#### [stream].List的声明示例

```javascript
var Tars = require("@tars/stream");

//例子1：声明vector<int32>
var va = new Tars.List(Tars.Int32);

//例子2：声明vector<string>
var vb = new Tars.List(Tars.String);

//例子3：声明vector<map<uint32, string> >
var vc = new Tars.List(Tars.Map(Tars.UInt32, Tars.String));

//例子4：声明vector<struct>，假设结构体名称为TRom.Param
var vd = new Tars.Vector(TRom.Param);
```

#### [stream].List的操作示例

```javascript
var Tars = require("@tars/stream");

var ve  = new Tars.List(Tars.String);

//向数组中添加元素
ve.push("TENCENT-MIG");
ve.push("TENCENT-SNG");
ve.push("TENCENT-IEG");
ve.push("TENCENT-TEG");

//获取数组的长度
console.log("Length:", ve.length);

//获取指定位置的元素
console.log("Array[1]:", ve.at(1));

//遍历方法1：
ve.forEach(function (value, index, oArray) {
	console.log("Array[" + index + "]:", value);
});

// 遍历方法2：
for (var index = 0, len = ve.length; index < len; index++) {
	console.log("Array[" + index + "]:", ve.at(index));
}

// toObject方法和readFromObject方法的详细例子可以参照sample/list路径下的test-list-c3.js文件
var user1 = new TRom.User_t();
user1.id = 1;
user1.name = 'x1';
user1.score = 1;

var user2 = new TRom.User_t();
user2.id = 2;
user2.name = 'x2';
user2.score = 2;

var user3 = new TRom.User_t();
user3.id = 3;
user3.name = 'x3';
user3.score = 3;

var userList1 = new Tars.List(TRom.User_t);

console.log('user1: ', user1);
console.log('user2: ', user2);

userList1.push(user1);
userList1.push(user2);

//toObject方法
console.log('userList1: ', userList1.toObject());

var userList2 = new Tars.List(TRom.User_t);
//readFromObject方法
userList2.readFromObject([user1, user2, user3]);
console.log('userList2: ', userList2.toObject());
```

# 07 - 复杂类型 - map（字典）的使用方法说明
由于Javascript原生的Object不支持tars中的一些特殊化操作，所以我们对它进行了一次封装。开发者可按下述的代码理解：

```javascript
[stream].Map = function(kproto, vproto) {
    var Map = function() {
        this._kproto = kproto;
        this._vproto = vproto;
        this.value   = new Object();
        this.put     = function(key, value) { this.insert(key, value); }
        ......
	}

	return new Map();
}
```

#### [stream].Map 对象属性
| 属性  | 描述 |
| ------------- | ------------- |
| value  | Js中的Object数据类型。[stream].Map实际是基于该Object进行的上层封装。|

#### [stream].Map 方法属性
| 方法  | 描述 |
| ------------- | ------------- |
| insert  | 向字典中添加一个元素。|
| set  | 同insert。|
| put  | 同insert。|
| remove | 根据指定的key，从字典中删除对应的数值。|
| clear | 清空当前字典。|
| has  | 根据指定的key，判断字典中是否包含对应的数值。|
| size | 返回当前字典中元素的数目。|
| forEach | 当前数组的遍历方法，具体使用方法请参考后面的示例。 |
| toObject | 将Map实例转化成基本的数据对象，具体使用方法请参考后面的示例。 |
| readFromObject | 将传入的对象处理后insert到Map实例中，具体使用方法请参考后面的示例。 |

#### [stream].Map的声明示例

```javascript
var Tars = require("@tars/stream");

//例子1：声明map<int32, int32>
var ma = new Tars.Map(Tars.Int32, Tars.Int32);

//例子2：声明map<uint32, string>
var mb = new Tars.Map(Tars.Int32, Tars.String);

//例子3：声明map<string, string>的方法
var mc = new Tars.Map(Tars.String, Tars.String);

//例子4：声明map<string, vector<int32> >
var md = new Tars.Map(Tars.String, Tars.List(Tars.Int32));

//例子5：声明map<string, map<int32, vector<string> > >
var me = new Tars.Map(Tars.String, Tars.Map(Tars.Int32, Tars.List(Tars.String)));

//例子6：声明map<string, struct>的方法，假设结构体名称为TRom.Param
var mf = new Tars.map(Tars.String, TRom.Param);
```

#### [stream].Map的操作示例

```javascript
var Tars = require("@tars/stream");

var mc = new Tars.Map(Tars.String, Tars.String);

//向字典中添加元素
mc.insert("KEY-00", "TENCENT-MIG");
mc.insert("KEY-01", "TENCENT-IEG");
mc.insert("KEY-02", "TENCENT-TEG");
mc.insert("KEY-03", "TENCENT-SNG");

//获取字典元素大小
console.log("SIZE:", mc.size());

//判断字典中是否有指定的值
console.log("Has:", mc.has("KEY-04"));

//字典遍历
mc.forEach(function (key, value) {
	console.log("KEY:", key);
	console.log("VALUE:", value);
});

// toObject方法和readFromObject方法的详细例子可以参照sample/map路径下的test-map-c5.js文件
var user1 = new TRom.User_t();
user1.id = 1;
user1.name = 'x1';
user1.score = 1;

var user2 = new TRom.User_t();
user2.id = 2;
user2.name = 'x2';
user2.score = 2;

var user3 = new TRom.User_t();
user3.id = 3;
user3.name = 'x3';
user3.score = 3;

var userMap1 = new Tars.Map(Tars.String, TRom.User_t);

userMap1.insert('user1', user1);
userMap1.insert('user2', user2);

//toObject方法
console.log('userMap1: ', userMap1.toObject());

var userMap2 = new Tars.Map(Tars.String, TRom.User_t);
//readFromObject方法
userMap2.readFromObject({
    'user1': user1,
    'user2': user2,
    'user3': user3
});
console.log('userMap2: ', userMap2.toObject());

```
#### 支持MultiMap类型
支持MultiMap类型，此类型允许以一个结构体作为Map的key。javascript原生对象没有办法表示此数据类型，因此此类型没有实现普通Map支持的toObject和readFromObject方法。

其操作实例如下：

```javascript
//构造Map类型
var msg = new Tars.Map(Test.StatMicMsgHead, Test.StatMicMsgBody);
msg.put(StatMicMsgHead1, StatMicMsgBody1);
msg.put(StatMicMsgHead2, StatMicMsgBody2);

//tars编码
var os = new Tars.OutputStream();
os.writeMap(1, msg);

//tars解码
var data = os.getBinBuffer().toNodeBuffer();

var is = new Tars.InputStream(new Tars.BinBuffer(data));
var ta = is.readMap(1, true, Tars.Map(Test.StatMicMsgHead, Test.StatMicMsgBody));

//遍历Map结果集
ta.forEach(function (key, value){
    console.log("KEY:", key.masterName, "VALUE.totalRspTime", value.totalRspTime);
});

//根据值去获取
var tb = ta.get(StatMicMsgHead2);
if (tb == undefined) {
    console.log("not found by name : StatMicMsgHead2");
} else {
    console.log(tb.totalRspTime);
}
```
# 08 - 复杂类型 - 二进制Buffer的使用方法说明

在浏览器中我们可以使用“DataView”和“ArrayBuffer”来存储和操作二进制数据。NodeJS为了提升性能，自身提供了一个Buffer类。为了方便Tars的编解码，我们对Buffer类进行了一层封装。开发者可按下述的代码理解：

```javascript
[stream].BinBuffer = function (buffer) {
    this._buffer    = (buffer != undefined && buffer instanceof Buffer)?buffer:null;
    this._length    = (buffer != undefined && buffer instanceof Buffer)?buffer.length:0;
    this._capacity  = this._length;
    this._position  = 0;
}
```

#### [stream].BinBuffer 对象属性
| 属性  | 描述 |
| ------------- | ------------- |
| length  | 获取该二进制Buffer的数据长度 |
| capacity  | 获取该二进制Buffer在不重新分配内存的情况下，可容纳数据的最大长度 |
| position  | 获取或者设置当前二进制Buffer的访问指针 |

>length和capacity的区别：

>假如我们向BinBuffer中写入一个Int32类型的数据。写成功之后，length和capacity的区别：

>由于BinBuffer类在第一次分配时使用默认的512长度来申请内存，此时 capacity 的值为 512

>length表示当前Buffer中存在真实数据的大小，此时 length 的值为 4

#### [stream].BinBuffer 方法属性

**toNodeBuffer**

>函数定义；[stream].BinBuffer.toNodeBuffer()

>函数作用：返回当前二进制Buffer的数据，该值为深拷贝的类型为NodeJS.Buffer的数据

>输入参数：无

>返回数据：NodeJS.Buffer类型

**print**

>函数定义：[stream].BinBuffer.print()

>函数作用：以每行16个字节，并16进制的方式打印当前的Buffer

**writeNodeBuffer**

>函数定义：[stream].BinBuffer.writeNodeBuffer(srcBuffer, offset, byteLength)

>函数作用：向二进制Buffer中写入NodeJS.Buffer类数据

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| srcBuffer | NodeJS.Buffer | 原始的Buffer数据 |
>| offset | UInt32 | 表示拷贝srcBuffer的起始位置 |
>| byteLength | UInt32 | 表示从offset开始，从srcBuffer中拷贝的数据量 |

>函数说明：

>[1]当前BinBuffer的 `length = length(原Buffer数据长度) + byteLength`

>[2]当前BinBuffer的 `position = position(原Buffer的位置指针) + byteLength`

**writeBinBuffer**

>函数定义：[stream].BinBuffer.writeBinBuffer(value)

>函数作用：向二进制Buffer中写入[stream].BinBuffer类数据

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| value | [stream].BinBuffer | 表示二进制Buffer |

>函数说明：

>[1]当前BinBuffer的 `length = length(原Buffer数据长度) + value.length`

>[2]当前BinBuffer的 `position = position(原Buffer的位置指针) + value.length`

**writeInt8**

>函数定义：[stream].BinBuffer.writeInt8(value)

>函数作用：向二进制Buffer中写入Int8类数据

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| value | Int8 | 8位的整型数据 |

>函数说明：

>[1]当前BinBuffer的 `length = length(原Buffer数据长度) + 1`

>[2]当前BinBuffer的 `position = position(原Buffer的位置指针) + 1`

**writeInt16**

>函数定义：[stream].BinBuffer.writeInt16(value)

>函数作用：向二进制Buffer中写入Int16类数据

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| value | Int16 | 16位的整型数据 |

>函数说明：

>[1]当前BinBuffer的 `length = length(原Buffer数据长度) + 2`

>[2]当前BinBuffer的 `position = position(原Buffer的位置指针) + 2`

>[3]数据存储采用网络字节序

**writeInt32**

>函数定义：[stream].BinBuffer.writeInt32(value)

>函数作用：向二进制Buffer中写入Int32类数据

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| value | Int32 | 32位的整型数据 |

>函数说明：

>[1]当前BinBuffer的 `length = length(原Buffer数据长度) + 4`

>[2]当前BinBuffer的 `position = position(原Buffer的位置指针) + 4`

>[3]数据存储采用网络字节序

**writeInt64**

>函数定义：[stream].BinBuffer.writeInt64(value)

>函数作用：向二进制Buffer中写入Int64类数据

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| value | Int64 | 64位的整型数据 |

>函数说明：

>[1]当前BinBuffer的 `length = length(原Buffer数据长度) + 8`

>[2]当前BinBuffer的 `position = position(原Buffer的位置指针) + 8`

>[3]数据存储采用网络字节序

**writeUInt8**

>函数定义：[stream].BinBuffer.writeUInt8(value)

>函数作用：向二进制Buffer中写入UInt8类数据

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| value | UInt8 | 8位的整型数据 |

>函数说明：

>[1]当前BinBuffer的 `length = length(原Buffer数据长度) + 1`

>[2]当前BinBuffer的 `position = position(原Buffer的位置指针) + 1`

**writeUInt16**

>函数定义：[stream].BinBuffer.writeUInt16(value)

>函数作用：向二进制Buffer中写入UInt16类数据

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| value | UInt16 | 16位的整型数据 |

>函数说明：

>[1]当前BinBuffer的 `length = length(原Buffer数据长度) + 2`

>[2]当前BinBuffer的 `position = position(原Buffer的位置指针) + 2`

>[3]数据存储采用网络字节序

**writeUInt32**

>函数定义：[stream].BinBuffer.writeUInt32(value)

>函数作用：向二进制Buffer中写入UInt32类数据

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| value | UInt32 | 32位的整型数据 |

>函数说明：

>[1]当前BinBuffer的 `length = length(原Buffer数据长度) + 4`

>[2]当前BinBuffer的 `position = position(原Buffer的位置指针) + 4`

>[3]数据存储采用网络字节序

**writeFloat**

>函数定义：[stream].BinBuffer.writeFloat(value)

>函数作用：向二进制Buffer中写入Float(32位，单精度浮点数)类数据

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| value | Float | 32位的单精度浮点数 |

>函数说明：

>[1]当前BinBuffer的 `length = length(原Buffer数据长度) + 4`

>[2]当前BinBuffer的 `position = position(原Buffer的位置指针) + 4`

>[3]数据存储采用网络字节序

**writeDouble**

>函数定义：[stream].BinBuffer.writeDouble(value)

>函数作用：向二进制Buffer中写入Double(64位，双精度浮点数)类数据

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| value | Double | 64位的双精度浮点数 |

>函数说明：

>[1]当前BinBuffer的 `length = length(原Buffer数据长度) + 8`

>[2]当前BinBuffer的 `position = position(原Buffer的位置指针) + 8`

>[3]数据存储采用网络字节序

**writeString**

>函数定义：[stream].BinBuffer.writeString(value)

>函数作用：向二进制Buffer中写入String(UTF8编码)类数据

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| value | String | UTF8编码的字符串 |

>函数说明：

>[1]当前BinBuffer的 `length = length(原Buffer数据长度) + 字符串的字节长度`

>[2]当前BinBuffer的 `position = position(原Buffer的位置指针) + 字符串的字节长度`

**readInt8**

>函数定义：[stream].BinBuffer.readInt8()

>函数作用：从二进制Buffer中，根据当前数据指针读取一个Int8类型的变量

>输入参数：无

>函数说明：

>[1]当前BinBuffer的 `position = position(原Buffer的位置指针) + 1`

**readInt16**

>函数定义：[stream].BinBuffer.readInt16()

>函数作用：从二进制Buffer中，根据当前数据指针读取一个Int16类型的变量

>输入参数：无

>函数说明：

>[1]当前BinBuffer的 `position = position(原Buffer的位置指针) + 2`

**readInt32**

>函数定义：[stream].BinBuffer.readInt32()

>函数作用：从二进制Buffer中，根据当前数据指针读取一个Int32类型的变量

>输入参数：无

>函数说明：

>[1]当前BinBuffer的 `position = position(原Buffer的位置指针) + 4`

**readInt64**

>函数定义：[stream].BinBuffer.readInt64()

>函数作用：从二进制Buffer中，根据当前数据指针读取一个Int64类型的变量

>输入参数：无

>函数说明：

>[1]当前BinBuffer的 `position = position(原Buffer的位置指针) + 8`

**readUInt8**

>函数定义：[stream].BinBuffer.readUInt8()

>函数作用：从二进制Buffer中，根据当前数据指针读取一个UInt8类型的变量

>输入参数：无

>函数说明：

>[1]当前BinBuffer的 `position = position(原Buffer的位置指针) + 1`

**readUInt16**

>函数定义：[stream].BinBuffer.readUInt16()

>函数作用：从二进制Buffer中，根据当前数据指针读取一个UInt16类型的变量

>输入参数：无

>函数说明：

>[1]当前BinBuffer的 `position = position(原Buffer的位置指针) + 2`

**readUInt32**

>函数定义：[stream].BinBuffer.readUInt32()

>函数作用：从二进制Buffer中，根据当前数据指针读取一个UInt32类型的变量

>输入参数：无

>函数说明：

>[1]当前BinBuffer的 `position = position(原Buffer的位置指针) + 4`

**readFloat**

>函数定义：[stream].BinBuffer.readFloat()

>函数作用：从二进制Buffer中，根据当前数据指针读取一个Float(32位的单精度浮点数)类型的变量

>输入参数：无

>函数说明：

>[1]当前BinBuffer的 `position = position(原Buffer的位置指针) + 4`

**readDouble**

>函数定义：[stream].BinBuffer.readDouble()

>函数作用：从二进制Buffer中，根据当前数据指针读取一个Double(64位的双精度浮点数)类型的变量

>输入参数：无

>函数说明：

>[1]当前BinBuffer的 `position = position(原Buffer的位置指针) + 8`

**readString**

>函数定义：[stream].BinBuffer.readString(byteLength)

>函数作用：从二进制Buffer中，根据当前数据指针读取一个String(UTF8编码)类型的变量

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| byteLength | UInt32 | 字符串的字节长度 |

>函数说明：

>[1]当前BinBuffer的 `position = position(原Buffer的位置指针) + 字符串的字节长度`

>[2]后台对字符串的编码需要使用UTF8字符集

**readBinBuffer**

>函数定义：[stream].BinBuffer.readBinBuffer(byteLength)

>函数作用：从二进制Buffer中，根据当前数据指针读取一个[stream].BinBuffer类型的变量

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| byteLength | UInt32 | 二进制Buffer的字节长度 |

>函数说明：

>[1]当前BinBuffer的 `position = position(原Buffer的位置指针) + 二进制Buffer的字节长度`

# 09 - 编码工具 - OutputStream的使用方法说明

**构造函数**
>函数定义：[stream].OutputStram()

>函数作用：声明一个输出流对象

>输入参数：无

>使用示例：var os = new [stream].OutputStream()

**getBinBuffer**

>函数定义：var buffer = [stream].OutputStream.getBinBuffer()

>函数作用：调用该函数获得打包后的二进制数据流

>输入参数：无

>返回数据：返回打包后的二进制数据流，该返回值类型为[stream].BinBuffer

**writeBoolean**

>函数定义：[stream].OutputStream.writeBoolean(tag, value)

>函数作用：向数据流中写一个Boolean类型的变量

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示该变量的数字标识，取值范围[0, 255] |
>| value | Boolean | 表示该变量的值，取值范围{false, true} |

>返回数据：void

**writeInt8**

>函数定义：[stream].OutputStream.writeInt8(tag, value)

>函数作用：向数据流中写一个int8类型的变量

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示该变量的数字标识，取值范围[0, 255] |
>| value | int8(Number) | 表示该变量的值，取值范围[-128, 127] |

>返回数据：void

**writeInt16**

>函数定义：[stream].OutputStream.writeInt16(tag, value)

>函数作用：向数据流中写一个Int16类型的变量

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示该变量的数字标识，取值范围[0, 255] |
>| value | int16(Number) | 表示该变量的值，取值范围[-32768, 32767] |

>返回数据：void

**writeInt32**

>函数定义：[stream].OutputStream.writeInt32(tag, value)

>函数作用：向数据流中写一个Int32类型的变量

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示该变量的数字标识，取值范围[0, 255] |
>| value | int32(Number) | 表示该变量的值，取值范围[-2147483648, 2147483647] |

>返回数据：void

**writeInt64**

>函数定义：[stream].OutputStream.writeInt64(tag, value)

>函数作用：向数据流中写一个Int64类型的变量

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示该变量的数字标识，取值范围[0, 255] |
>| value | int64(Number) | 表示该变量的值，取值范围[-9223372036854775808, 9223372036854775807] |

>返回数据：void

**writeUInt8**

>函数定义：[stream].OutputStream.writeUInt8(tag, value)

>函数作用：向数据流中写一个UInt8类型的变量

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示该变量的数字标识，取值范围[0, 255] |
>| value | UInt8(Number) | 表示该变量的值，取值范围[0, 255] |

>返回数据：void

**writeUInt16**

>函数定义：[stream].OutputStream.writeUInt16(tag, value)

>函数作用：向数据流中写一个UInt16类型的变量

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示该变量的数字标识，取值范围[0, 255] |
>| value | UInt16(Number) | 表示该变量的值，取值范围[0, 65535] |

>返回数据：void

**writeUInt32**

>函数定义：[stream].OutputStream.writeUInt32(tag, value)

>函数作用：向数据流中写一个UInt32类型的变量

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示该变量的数字标识，取值范围[0, 255] |
>| value | UInt32(Number) | 表示该变量的值，取值范围[0, 4294967295] |

>返回数据：void

**writeFloat**

>函数定义：[stream].OutputStream.writeFloat(tag, value)

>函数作用：向数据流中写一个float(32位)类型的变量

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示该变量的数字标识，取值范围[0, 255] |
>| value | Float(Number) | 单精度浮点数，因为有精度损失问题，不推荐使用该类型 |

>返回数据：void

**writeDouble**

>函数定义：[stream].OutputStream.writeDouble(tag, value)

>函数作用：向数据流中写一个double(64位)类型的变量

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示该变量的数字标识，取值范围[0, 255] |
>| value | Double(Number) | 双精度浮点数，因为有精度损失问题，不推荐使用该类型 |

>返回数据：void

**writeString**

>函数定义：[stream].OutputStream.writeString(tag, value)

>函数作用：向数据流中写一个String类型的变量

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示该变量的数字标识，取值范围[0, 255] |
>| value | String | 表示该变量的值，字符串编码字符集为UTF8 |

>返回数据：void

**writeStruct**

>函数定义：writeStruct(tag, value)

>函数作用：向数据流中写一个自定义结构体的变量

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示该变量的数字标识，取值范围[0, 255] |
>| value | 自定义结构体 | 结构体必须是使用tars2node转换而成的，否则可能会因缺少辅助函数而导致编解码失败 |

>返回数据：void

**writeBytes**

>函数定义：[stream].OutputStream.writeBytes(tag, value)

>函数作用：向数据流中写一个类型为 `char *` 或者 `vector<char>` 的变量

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示该变量的数字标识，取值范围[0, 255] |
>| value | [stream].BinBuffer | BinBuffer是对NodeJs中的Buffer类的封装，同时集成了编解码需要用到的辅助函数 |

>返回数据：void

**writeList**

>函数定义：[stream].OutputStream.writeList(tag, value)

>函数作用：向数据流中写一个类型为 `vector<T>`（T不可为byte）的变量

>函数参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示该变量的数字标识，取值范围[0, 255] |
>| value | [stream].List(T) | 该变量的类型原型 |

>返回数据：void

**writeMap**

>函数定义：[stream].OutputStream.writeMap(tag, value)

>函数作用：向数据流中写一个类型为 `map<T, V>` 类型的字段。

>函数参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示该变量的数字标识，取值范围[0, 255] |
>| value | [stream].Map(T, V) | 该变量的类型原型 |

>返回数据：void

# 10 - 解码工具 - InputStream的使用方法说明

**构造函数**

>函数定义：[stream].InputStream(binBuffer)

>函数作用：声明一个输入流对象

>输入参数：

>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;binBuffer 欲解码的二进制数据流，该值类型必须为[stream].BinBuffer，而不能是NodeJs中实现的Buffer类。

>使用示例：var is = new [stream].InputStream(new [stream].BinBuffer(Node.Buffer))

**readBoolean**

>函数定义：var value = [stream].InputStream.readBoolean(tag, require, default)

>函数作用：从数据流读取一个Boolean类型的数值

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示欲读取变量的数字标识，取值范围[0, 255] |
>| require | Boolean | 表示当前变量是否为必须值，取值范围{false, true} |
>| default | Boolean | 表示读取变量不成功时的返回值，取值范围{false, true} |

>>对require的说明：

>>当 `require === true` &nbsp;&nbsp;时， 如果当前变量不在数据流中，系统将抛出一个读取数据不存在的异常；

>>当 `require === false` 时，如果当前变量不在数据流中，系统将返回变量的默认值default；

>返回数据：Boolean，取值范围{false, true}

**readInt8**

>函数定义：[stream].InputStream.readInt8(tag, require, default)

>函数作用：从数据流读取一个Int8类型的数值

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示欲读取变量的数字标识，取值范围[0, 255] |
>| require | Boolean | 表示当前变量是否为必须值，取值范围{false, true} |
>| default | Int8 | 表示读取变量不成功时的返回值，取值范围[-128, 127] |

>>对require的说明：

>>当 `require === true` &nbsp;&nbsp;时， 如果当前变量不在数据流中，系统将抛出一个读取数据不存在的异常；

>>当 `require === false` 时，如果当前变量不在数据流中，系统将返回变量的默认值default；

>返回数据：Int8，取值范围[-128, 127]

**readInt16**

>函数定义：[stream].InputStream.readInt16(tag, require, default)

>函数作用：从数据流读取一个Int16类型的数值

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示欲读取变量的数字标识，取值范围[0, 255] |
>| require | Boolean | 表示当前变量是否为必须值，取值范围{false, true} |
>| default | Int16 | 表示读取变量不成功时的返回值，取值范围[-32768, 32767] |

>>对require的说明：

>>当 `require === true` &nbsp;&nbsp;时， 如果当前变量不在数据流中，系统将抛出一个读取数据不存在的异常；

>>当 `require === false` 时，如果当前变量不在数据流中，系统将返回变量的默认值default；

>返回数据：Int16，取值范围[-32768, 32767]

**readInt32**

>函数定义：[stream].InputStream.readInt32(tag, require, default)

>函数作用：从数据流读取一个Int32类型的数值

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示欲读取变量的数字标识，取值范围[0, 255] |
>| require | Boolean | 表示当前变量是否为必须值，取值范围{false, true} |
>| default | Int32 | 表示读取变量不成功时的返回值，取值范围[-2147483648, 2147483647] |

>>对require的说明：

>>当 `require === true` &nbsp;&nbsp;时， 如果当前变量不在数据流中，系统将抛出一个读取数据不存在的异常；

>>当 `require === false` 时，如果当前变量不在数据流中，系统将返回变量的默认值default；

>返回数据：Int32，取值范围[-2147483648, 2147483647]

**readInt64**

>函数定义：[stream].InputStream.readInt64(tag, require, default)

>函数作用：从数据流读取一个Int64类型的数值

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示欲读取变量的数字标识，取值范围[0, 255] |
>| require | Boolean | 表示当前变量是否为必须值，取值范围{false, true} |
>| default | Int64 | 表示读取变量不成功时的返回值，取值范围[-9223372036854775808, 9223372036854775807] |

>>对require的说明：

>>当 `require === true` &nbsp;&nbsp;时， 如果当前变量不在数据流中，系统将抛出一个读取数据不存在的异常；

>>当 `require === false` 时，如果当前变量不在数据流中，系统将返回变量的默认值default；

>返回数据：Int64(Number)，取值范围[-9223372036854775808, 9223372036854775807]

**readUInt8**

>函数定义：[stream].InputStream.readUInt8(tag, require, default)

>函数作用：从数据流读取一个UInt8类型的数值

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示欲读取变量的数字标识，取值范围[0, 255] |
>| require | Boolean | 表示当前变量是否为必须值，取值范围{false, true} |
>| default | UInt8 | 表示读取变量不成功时的返回值，取值范围[0, 255] |

>>对require的说明：

>>当 `require === true` &nbsp;&nbsp;时， 如果当前变量不在数据流中，系统将抛出一个读取数据不存在的异常；

>>当 `require === false` 时，如果当前变量不在数据流中，系统将返回变量的默认值default；

>返回数据：UInt8(Number)，取值范围[0, 255]

**readUInt16**

>函数定义：[stream].InputStream.readUInt16(tag, require, default)

>函数作用：从数据流读取一个UInt16类型的数值

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示欲读取变量的数字标识，取值范围[0, 255] |
>| require | Boolean | 表示当前变量是否为必须值，取值范围{false, true} |
>| default | UInt8 | 表示读取变量不成功时的返回值，取值范围[0, 65535] |

>>对require的说明：

>>当 `require === true` &nbsp;&nbsp;时， 如果当前变量不在数据流中，系统将抛出一个读取数据不存在的异常；

>>当 `require === false` 时，如果当前变量不在数据流中，系统将返回变量的默认值default；

>返回数据：UInt16(Number)，取值范围[0, 65535]

**readUInt32**

>函数定义：[stream].InputStream.readUInt32(tag, require, default)

>函数作用：从数据流读取一个UInt32类型的数值

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示欲读取变量的数字标识，取值范围[0, 255] |
>| require | Boolean | 表示当前变量是否为必须值，取值范围{false, true} |
>| default | UInt8 | 表示读取变量不成功时的返回值，取值范围[0, 4294967295] |

>>对require的说明：

>>当 `require === true` &nbsp;&nbsp;时， 如果当前变量不在数据流中，系统将抛出一个读取数据不存在的异常；

>>当 `require === false` 时，如果当前变量不在数据流中，系统将返回变量的默认值default；

>返回数据：UInt32(Number)，取值范围[0, 4294967295]

**readFloat**

>函数定义：[stream].InputStream.readFloat(tag, require, default)

>函数作用：从数据流读取一个Float（32位，单精度浮点数）类型的数值

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示欲读取变量的数字标识，取值范围[0, 255] |
>| require | Boolean | 表示当前变量是否为必须值，取值范围{false, true} |
>| default | Float | 表示读取变量不成功时的返回值 |

>>对require的说明：

>>当 `require === true` &nbsp;&nbsp;时， 如果当前变量不在数据流中，系统将抛出一个读取数据不存在的异常；

>>当 `require === false` 时，如果当前变量不在数据流中，系统将返回变量的默认值default；

>返回数据：Float(Number)

**readDouble**

>函数定义：[stream].InputStream.readFloat(tag, require, default)

>函数作用：从数据流读取一个Double（64位，双精度浮点数）类型的数值

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示欲读取变量的数字标识，取值范围[0, 255] |
>| require | Boolean | 表示当前变量是否为必须值，取值范围{false, true} |
>| default | Double | 表示读取变量不成功时的返回值 |

>>对require的说明：

>>当 `require === true` &nbsp;&nbsp;时， 如果当前变量不在数据流中，系统将抛出一个读取数据不存在的异常；

>>当 `require === false` 时，如果当前变量不在数据流中，系统将返回变量的默认值default；

>返回数据：Double(Number)

**readString**

>函数定义：[stream].InputStream.readString(tag, require, default)

>函数作用：从数据流读取一个String（UTF8编码）类型的数值

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示欲读取变量的数字标识，取值范围[0, 255] |
>| require | Boolean | 表示当前变量是否为必须值，取值范围{false, true} |
>| default | String | 表示读取变量不成功时的返回值 |

>>对require的说明：

>>当 `require === true` &nbsp;&nbsp;时， 如果当前变量不在数据流中，系统将抛出一个读取数据不存在的异常；

>>当 `require === false` 时，如果当前变量不在数据流中，系统将返回变量的默认值default；

>返回数据：String（UTF8编码）

**readStruct**

>函数定义：[stream].InputStream.readStruct(tag, require, TYPE_T)

>函数作用：从数据流读取一个自定义结构体类型的数值

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示欲读取变量的数字标识，取值范围[0, 255] |
>| require | Boolean | 表示当前变量是否为必须值，取值范围{false, true} |
>| TYPE_T | 自定义结构体的类型原型 | 表示该变量的类型原型 |

>>对require的说明：

>>当 `require === true` &nbsp;&nbsp;时， 如果当前变量不在数据流中，系统将抛出一个读取数据不存在的异常；

>>当 `require === false` 时，如果当前变量不在数据流中，系统将返回一个空的结构体的实例；

>返回数据：自定义结构体的实例

**readBytes**

>函数定义：[stream].InputStream.readBytes(tag, require, TYPE_T)

>函数作用：从数据流读取一个 `[stream].BinBuffer` 类型的数值

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示欲读取变量的数字标识，取值范围[0, 255] |
>| require | Boolean | 表示当前变量是否为必须值，取值范围{false, true} |
>| TYPE_T | [stream].BinBuffer | 表示该变量的类型原型 |

>>对require的说明：

>>当 `require === true` &nbsp;&nbsp;时， 如果当前变量不在数据流中，系统将抛出一个读取数据不存在的异常；

>>当 `require === false` 时，如果当前变量不在数据流中，系统将返回一个空的[stream].BinBuffer的实例；

>返回数据：[stream].BinBuffer

**readList**

>函数定义：[stream].InputStream.readList(tag, require, TYPE_T)

>函数作用：从数据流读取一个 `[stream].List<T>` 类型的数值

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示欲读取变量的数字标识，取值范围[0, 255] |
>| require | Boolean | 表示当前变量是否为必须值，取值范围{false, true} |
>| TYPE_T | [stream].List<T> | 表示该变量的类型原型 |

>>对require的说明：

>>当 `require === true` &nbsp;&nbsp;时， 如果当前变量不在数据流中，系统将抛出一个读取数据不存在的异常；

>>当 `require === false` 时，如果当前变量不在数据流中，系统将返回一个空的[stream].List(T)的实例；

>返回数据：[stream].List(T)

**readMap**

>函数定义：[stream].InputStream.readMap(tag, require, TYPE_T)

>函数作用：从数据流读取一个 `[stream].Map<T, V>` 类型的数值

>输入参数：

>| 参数  | 数据类型 | 描述 |
>| ------------- | ------------- | ------------- |
>| tag | UInt8 | 表示欲读取变量的数字标识，取值范围[0, 255] |
>| require | Boolean | 表示当前变量是否为必须值，取值范围{false, true} |
>| TYPE_T | [stream].Map(T, V) | 表示该变量的类型原型 |

>>对require的说明：

>>当 `require === true` &nbsp;&nbsp;时， 如果当前变量不在数据流中，系统将抛出一个读取数据不存在的异常；

>>当 `require === false` 时，如果当前变量不在数据流中，系统将返回一个空的[stream].Map(T, V)的实例；

>返回数据：[stream].Map(T, V)
