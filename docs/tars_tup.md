# 目录
> * [TUP概述] (#main-chapter-1)
> * [Tars语言] (#main-chapter-2)
> * [Tars协议] (#main-chapter-3)
> * [TUP使用] (#main-chapter-4)
> * [TUP各版本接口介绍] (#main-chapter-5)

# 1. TUP概述 <a id="main-chapter-1"></a>

## 1.1. TUP是什么 

TUP（Tars Uni-Protocol的简称），Tars统一协议，是基于Tars编码的命令字（Command）层协议的封装。

## 1.2. TUP能做什么

1.支持java、c++等语言

2.支持对象的序列化和反序列化

3.支持协议动态扩展

4.提供put/get泛型接口，快速实现客户端/服务端的编解码

5.序列化的数据可用于网络传输或者持久化存储

6.支持直接调用Tars的服务端

## 1.3. TUP不能做什么

1.只做协议封装，不包含网络层

2.不支持数据压缩（可在业务层处理）

3.不支持加密协议（可在业务层处理）

## 1.4. 依赖和约束

1.依赖Tars协议，TUP用到的结构体对象必须通过Tars定义后生成

2.依赖各个语言的代码生成工具，如：tars2cpp/tars2java等

3.TarsUniPacket中封装的Tars相关的接口（如getTars.../setTars...），只有在调用Tars服务是需要用到

4.使用过程中，使用UniPacket完成请求与相应的数据传递，其中ServantName（服务对象名） 与FuncName（接口名）为必须设定的参数，否则编码失败

5.不建议get/put太多数据，如果有比较多数据，建议在tars文件中组成一个struct，然后put/get到UniPacket中，便于提高效率和减少网络包大小

6.UniPacket编码后的结果在包头包含了4个字节网络字节序的包长信息，长度包括包头，接收方收到包后需根据包头的内容，判断包长，确保包完整后，传入解码接口进行解码（无需去掉包头）

7.Tars c++语言的string 类型接口不能含二进制数据，二进制数据用vector<char>传输

# 2. Tars语言 <a id="main-chapter-2"></a>

## 2.1. 接口文件

Tars语言是一种类c++标识符的语言，用于生成具体的服务接口文件

Tars文件是Tars框架中客户端和服务端的通信接口，通过Tars的映射实现远程对象调用

Tars文件的扩展名必须以.tars为扩展名

对于结构定义，可以支持扩展字段，即可以增加字段而不影响原有结构的解析，可以在存储/协议等地方单独使用

大小写敏感

## 2.2. 词法规则

### 2.2.1. 注释

采用c++的注释规范。
```
//表示注释一行，/**/表示注释范围中的所有代码。
```
### 2.2.2. 关键字
```
void,struct,bool,byte,short,int,double,float,long,string,vector,map,key,routekey,module,interface,out,require,optional,false,true,enum,const
```
### 2.2.3. 标识符

所有标识符不能带有'tars_’符号，且必须以字母开头，同时不能和关键字冲突。

## 2.3. 基本类型

支持的基本类型包括以下：

void	：只能在函数的返回值表示

bool	：布尔类型，映射到tars::Bool

byte	：有符号字符，映射到 tars::Char

short	：有符号短整型，映射到 tars::Short

int		：有符号整型，映射到 tars::Int32

long	：有符号长整型，映射到 tars::Int64

float	：映射到tars::Float

double	：映射到tars::Double

string	：映射到 std::string，java:String

unsigned byte ：无符号字符,c++映射到 unsigend char 其它版本tars::Short

unsigned short：无符号短整形c++映射到 unsigned short其它版本 tars::Int32 

Unsigned int:无符号整形c++映射到 unsigned int其它版本 tars::Int64

## 2.4. 复杂类型

### 2.4.1. 枚举

枚举类型的定义如下：
```
enum TE
{
    E1,
    E2,
    E3
};
```
说明：

> * 枚举类型支持在指定枚举变量的值，例如支持：E1 = 1这种定义方式；

> * 第一个定义的枚举类型值为0，这里E1的值为0；

> * 枚举类型在tars文件定义后，通过tars2cpp生成以后，除了会生成相应的enum定义之外，会生成etos和stoe函数，将枚举值转换成字符串，以及将字符串转换成枚举值，在代码调试时会非常方便。

> * 建议在c++的tars文件中，所有接口都以int返回，且返回值在tars文件中以枚举来定义。


### 2.4.2. 常量

Tars文件中可以定义常量，例如：
```
const int a = 0;

const string s = “abc”;
```
说明：

> * 由于map，vector没有描述常量的值，因此不支持map，vector的定义；

### 2.4.3. 结构

结构定义如下：
```
struct Test
{
    0  require  string s;
    1  optional int  i = 23;
};

key[Test, s, i];
```
说明：

> * 第一列数字表示该字段的标识（tag），无论结构增减字段，该字段得值都不变，必须和响应的字段对应；

> * Tag的值必须要>=0且<=255；

> * require表示该字段必选；

> * optional表示该字段可选；

> * 对于optional字段，可以有一个缺省值，缺省值在编码时默认不打包；

key说明：

> * 表示结构的小于比较符号，缺省时Struct是没有小于操作的，如果定义了key，则生成小于比较符。

key详细说明：

> * key[Stuct, member…]：

> * Struct：表示结构的名称

> * Member：表示该结构的成员变量，可以有多个；

> * 生成的小于比较操作符，按照key中成员变量定义的顺序进行优先<比较；

> * 生成小于比较操作符以后，该结构就可以作为map的key；

其他说明：

> * 在Tars的c++语言中，对于结构而言，提供两个成员函数用于直接打印出结构的内容，可以用于调试和记录日志：

> * ostream& display(ostream& _os, int _level=0)：直接打印结构的详细内容，主要用于调试；

> * ostream& displaySimple(ostream& _os, int _level=0)：所有成员变量自动按照顺序以|分隔打印出来，用于记录日志；

### 2.4.4. 序列

序列用vector来定义，如下：
```
vector<int> vi;
```
### 2.4.5. 字典

字典用map来定义，如下：
```
map<int, string> m;
```
说明：

> * 对于struct，通常不能作为map的key，因此struct没有大小比较符号；

> * 如果需要struct能够作为map的key，需要用less定义struct中成员的比较顺序；

### 2.4.6. 数组

结构中可以定义数组类型，数组用[]来定义，如下：
```
byte m[5];
```
说明：
> * 对数组类型，在C++生成代码中会同时生成数组长度mLen

> * 对数组赋值后必须同时对数组长度赋值

> * 在非c++版本中数组类型将翻译为vector<类型>

> * byte m[5] 等价于定义vector<byte>:5

### 2.4.7 指针

结构中可以定义byte指针类型，指针用*来定义，如下：
```
byte *m;
```
指针类型使用时需要提前预分配内存块，指针需要内存时通过偏移指向预分配内存块，减少解码过程中的内存申请。

说明：

> * 对于指针类型，在c++代码中会同时生成mLen，用来指定指针长度。

> * 对指针赋值后必须对长度mLen赋值

> * 在非c++版本中指针类型将翻译为vector<类型>

> * 含有指针类型的数据读取时BufferReader必须用MapBufferReader,同时需要提前设定指针指向内存的buffer

### 2.4.8 嵌套

任何struct，map，vector都可以嵌套；

## 2.5. 接口

接口定义如下，例如：
```
interface Demo
{
    int get(out vector<map<int, string>> v);
    
    int set(vector<map<int, string>> v);
};

```
说明：
> * 表示输出参数
> * 接口定义后，通过自动代码生成工具（如：tars2cpp)会生成同步接口和异步接口等代码

## 2.6. 名字空间

所有的struct，interface必须在名字空间中，例如：
```
module MemCache
{
    struct Key
    {
        0 require string s;
    };

    struct Value
    {
        0 require string s;
    };

    interface MemCacheI
    {
        int get(Key k, out Value v);

        int set(Key k, Value v);
    };
};
```
说明：
> * 名字空间不能嵌套;
> * 可以引用其他名字空间,例如:Demo1::Key

# 3. Tars协议 <a id="main-chapter-3"></a>

## 3.1. 数据编码

### 3.1.1. 基本结构

每一个数据由两个部分组成，如下图：
```
| 头信息 | 实际数据 |
```
而其中头信息包括以下几个部分：
```
| Type(4 bits) | Tag 1(4 bits) | Tag 2(1 byte) |
```
Tag 2是可选的，当Tag的值不超过14时，只需要用Tag 1就可以表示；当Tag的值超过14而小于256时，Tag 1固定为15，而用Tag 2表示Tag的值。Tag不允许大于255。

Type表示类型，用4个二进制位表示，取值范围是0~15，用来标识该数据的类型。不同类型的数据，其后紧跟着的实际数据的长度和格式都是不一样的，详见一下的类型表。

Tag由Tag 1和Tag 2一起表示。取值范围是0~255，即该数据在结构中的字段ID，用来区分不同的字段。

### 3.1.2. 编码类型表

注意，这里的类型与tars文件定义的类型是两个不同的概念，这里的类型只是标识数据存储的类型，而不是数据定义的类型。

取值 |类型|备注
------|----|----
0 |int1|紧跟1个字节整型数据
1|int2|紧跟2个字节整型数据
2|int4|紧跟4个字节整型数据
3|int8|紧跟8个字节整型数据
4|float|紧跟4个字节浮点型数据
5|double|紧跟8个字节浮点型数据
6|String1|紧跟1个字节长度，再跟内容
7|String4|紧跟4个字节长度，再跟内容
8|Map|紧跟一个整型数据表示Map的大小，再跟[key, value]对列表
9|List|紧跟一个整型数据表示List的大小，再跟元素列表
10|自定义结构开始|自定义结构开始标志
11|自定义结构结束|自定义结构结束标志，Tag为0
12|数字0|表示数字0，后面不跟数据
13|SimpleList|简单列表（目前用在byte数组），紧跟一个类型字段（目前只支持byte），紧跟一个整型数据表示长度，再跟byte数据

### 3.1.3. 各类型详细描述

1.基本类型（包括int1、int2、int4、int8、float、double）

头信息后紧跟数值数据。char、bool也被看作整型。所有的整型数据之间不做区分，也就是说一个short的值可以赋值给一个int。

2.数字0

头信息后不跟数据，表示数值0。所有基本类型的0值都可以这样来表示。

这是考虑到数字0出现的概率比较大，所以单独提一个类型，以节省空间。

3.字符串（包括String1、String4）

String1跟一个字节的长度（该长度数据不包括头信息），接着紧跟内容。

String4与之类似。

4.Map

紧跟一个整形数据（包括头信息）表示Map的大小，然后紧跟[Key数据（Tag为0），Value数据（Tag为1）]对列表。

5.List

紧跟一个整形数据（包括头信息）表示List的大小，然后紧跟元素列表（Tag为0）

6.自定义结构开始

自定义结构开始标志，后面紧跟字段数据，字段按照tag升序顺序排列

7.自定义结构结束

自定义结构结束标志，Tag为0


### 3.1.4 对象持久化

对于自定义结构的持久化，由开始标志与结束标志来标识。

比如如下结构定义：
```
struct TestInfo
{
    1  require  int    ii  = 34;
    2  optional string s   = "abc";
};

struct TestInfo2
{
    1  require TestInfo  t;
    2  require int       a = 12345;
};

```
其中，默认的TestInfo2结构编码后结果为：

![tars](images/tars_tupstruct.png)

## 3.2. 消息格式

TUP底层协议完全采用Tars定义，与Tars的底层数据包定义一致，其中require的字段为TUP必须的字段，optional为访问Tars服务时额外需要用到的字段。

### 3.2.1. 请求包
```
//请求包体
struct RequestPacket
{
    1  require short        iVersion;         //版本号
    2  optional byte        cPacketType;      //包类型
    3  optional int         iMessageType;     //消息类型
    4  require int          iRequestId;       //请求ID
    5  require string       sServantName;     //servant名字
    6  require string       sFuncName;        //函数名称
    7  require vector<byte> sBuffer;          //二进制buffer
    8  optional int         iTimeout;         //超时时间（毫秒）
    9  optional map<string, string> context;  //业务上下文
    10 optional map<string, string> status;   //框架协议上下文
};
```
### 3.2.2. 响应包
```
//响应包体
struct ResponsePacket
{
    1 require short         iVersion;       //版本号
    2 optional byte         cPacketType;    //包类型
    3 require int           iRequestId;     //请求ID
    4 optional int          iMessageType;   //消息类型
    5 optional int          iRet;           //返回值
    6 require vector<byte>  sBuffer;        //二进制流
    7 optional map<string, string> status;  //协议上下文
    8 optional string       sResultDesc;    //结果描述
};

//返回值
const int TAFSERVERSUCCESS       = 0;       //服务器端处理成功
const int TAFSERVERDECODEERR     = -1;      //服务器端解码异常
const int TAFSERVERENCODEERR     = -2;      //服务器端编码异常
const int TAFSERVERNOFUNCERR     = -3;      //服务器端没有该函数
const int TAFSERVERNOSERVANTERR  = -4;      //服务器端没有该Servant对象
const int TAFSERVERRESETGRID     = -5;      //服务器端灰度状态不一致
const int TAFSERVERQUEUETIMEOUT  = -6;      //服务器队列超过限制
const int TAFASYNCCALLTIMEOUT    = -7;      //异步调用超时
const int TAFINVOKETIMEOUT       = -7;      //调用超时
const int TAFPROXYCONNECTERR     = -8;      //proxy链接异常
const int TAFSERVEROVERLOAD      = -9;      //服务器端超负载,超过队列长度
const int TAFADAPTERNULL         = -10;     //客户端选路为空，服务不存在或者所有服务down掉了
const int TAFINVOKEBYINVALIDESET = -11;     //客户端按set规则调用非法
const int TAFCLIENTDECODEERR     = -12;     //客户端解码异常
const int TAFSERVERUNKNOWNERR    = -99;     //服务器端位置异常
```

# 4. TUP使用 <a id="main-chapter-4"></a>

## 4.1. 类结构图

![tars](images/tars_tupclass.png)

## 4.2. 类的使用

1.RequestPacket：请求包及回应包的基类，通过tars文件定义生成，和Tars服务的基础包一致，一般不直接使用。

2.UniAttribute：属性类，用户可以通过对该类的对象进行操作，添加属性和获取属性，类提供了put/get泛型接口，并可实现编解码。编码序列化后的字节流可用于压缩、加密，网络传输或持久化存储，在需要的时候反序列化出原对象。

3.UniPacket：请求回应包类，继承于UniAttribute，可以添加请求的属性值，设置需要请求远程对象及方法名，编码后发送到服务端，服务端解码后可获取属性参数进行处理。服务端处理完请求后同样通过该类的对象返回结果，客户端解码获取处理结果。

4.TarsUniPacket：Tars请求回应包类，继承于UniPacket，调用Tars远程服务的时候使用，用户添加属性及设置相关属性后，进行编码，组成请求包通过网络发到Tars服务进行处理。Tars服务端收到TUP协议的请求，处理完后会以该类的对象组返回包返回给客户端。客户端收包后使用该类进行解码处理，获取结果。

## 4.3. 使用TUP协议调用Tars服务

1.客户端调用时，使用TarsUniPacket对象进行请求包的参数设置及输入参数赋值，其中必须指定的请求参数信息包括：
```
setRequestId();           设置消息id，递增

setServantName("");       设置远程对象名称

setFuncName("");          设置远程接口名称

setTarsPacketType();       包类型版本，TUP协议默认为3
```
针对特定远程接口的调用，只需通过put接口对输入参数进行赋值，属性名称为tars接口定义的参数名称，比如对于接口：
```
int testFunc(string inputString, int inputInt, out string outputString);
```
输入参数赋值的方式是：
```
TarsUniPacket<> req;

req.put<string>("inputString", "testInput");

req.put<Int32>("inputInt", 12345);

req.encode(buff);
```
TarsUniPacket对象必须为tars定义的每个输入参数设置属性值，否则服务端处理请求时会返回缺少某个属性值的异常错误，输出参数也可以作为输入，但是不是必选。

put接口的模板类型选用tars参数定义的对应的类型，但枚举类型例外，需换用Int32作为模板类型赋属性值。

2、TUP返回包同样使用TarsUniPacket对象进行解码，解码后使用getTarsResultCode()接口获取tars服务的处理结果，0为成功，非0为失败，失败的原因可以通过getTarsResultDesc()接口获取错误描述。

返回成功的结果包的各输出参数使用tars定义的输出参数名称作为属性名称去获取，接口的返回值使用空字符串的属性名去获取。

如上述接口获取返回结果的方式是：
```
TarsUniPacket<> rsp;

rsp.decode(recvBuff, recvLen);

if(rsp.getTarsResultCode() == 0)
{
    int ret = rsp.get<int32_t>("");						//获取返回值
    string retString = rsp.get<string>("outputString"); //获取输出参数
}
else
{
    cout <<　rsp.getTarsResultDesc() << endl;
}
``` 
# 5. TUP各版本接口介绍 <a id="main-chapter-5"></a>

## 5.1. Linux c++

### 5.1.1. 类接口

UniAttribute类

公共接口 |功能描述
------|--------
template<typename T> void put(const string& name, const T& t) |添加属性值
template<typename T> void get(const string& name, T& t)|获取属性值
template<typename T> T get(const string& name)|获取属性值
template<typename T> void getByDefault(const string& name, T& t, const T& def)|获取属性值(忽略异常，def为缺省值)
template<typename T> T getByDefault(const string& name, const T& def)|获取属性值(忽略异常，def为缺省值)
void clear()|清除全部属性值
void encode(string& buff)|将属性对象编码到字节流
void encode(vector<char>& buff)|将属性对象编码到字节流
void encode(char* buff, size_t & len)|将属性对象编码到字节流
void decode(const char* buff, size_t len)|将字节流解码
void decode(const vector<char>& buff)|将字节流解码
const map<string, vector<char> >& getData() const|获取已有的属性
bool isEmpty()|判断属性集合是否为空
size_t size()|获取属性集合大小
bool containsKey(const string & key)|判断属性是否存在

UniPacket类

公共接口 |功能描述
------|--------
void setVersion(short iVer) |设置协议版本号
UniPacket createResponse()|通过请求包生成回应包，生成过程会从请求包获取请求ID、对象名称、方法名等回填到回应包中
void encode(string& buff)|将对象编码到字节流
void encode(vector<char>& buff)|将对象编码到字节流
void encode(char* buff, size_t & len)|将对象编码到字节流
void decode(const char* buff, size_t len)|将字节流解码，其中len传入buffer长度，输出解码结果的长度
tars::Short getVersion() const|获取协议版本号
tars::Int32 getRequestId() const|获取消息ID
void setRequestId(tars::Int32 value)|设置请求ID
const std::string& getServantName() const|获取对象名称
void setServantName(const std::string& value)|设置对象名称(编码时对象名不能为空，否则编码失败)
const std::string& getFuncName() const|获取方法名
void setFuncName(const std::string& value)|设置方法名(编码时方法名不能为空，否则编码失败)

TarsUniPacket类

公共接口 |功能描述
------|--------
void setTarsVersion(tars::Short value) |设置协议版本
void setTarsPacketType(tars::Char value)|设置调用类型
void setTarsMessageType(tars::Int32 value)|设置消息类型
void setTarsTimeout(tars::Int32 value)|设置超时时间
void setTarsBuffer(const vector<tars::Char>& value)|设置参数编码内容
void setTarsContext(const map<std::string, std::string>& value)|设置上下文
void setTarsStatus(const map<std::string, std::string>& value)|设置特殊消息的状态值
tars::Short getTarsVersion() const|获取协议版本
tars::Char getTarsPacketType() const|获取调用类型
tars::Int32 getTarsMessageType() const|获取消息类型
tars::Int32 getTarsTimeout() const|获取超时时间
const vector<tars::Char>& getTarsBuffer() const|获取参数编码后内容
const map<std::string, std::string>& getTarsContext() const|获取上下文
const map<std::string, std::string>& getTarsStatus() const|获取特殊消息的状态值
tars::Int32 getTarsResultCode() const|获取Tars服务处理结果码，0为成功，非0为失败
string getTarsResultDesc() const|获取Tars服务处理结果描述

### 5.1.2. 使用注意

以上接口调用出错将抛出runtime_error 异常。

### 5.1.3. 使用例子

参见cpp/test/testServant/testTup/下的示例程序

## 5.2. Java

### 5.2.1. 类接口

UniAttribute类

UniPacket类

TarsUniPacket类

### 5.2.2. 使用注意

1.目前TUP支持基本类型, TarsStruct，已经存放基本类型或TarsStruct的map和list。对数组只支持byte[]，放入别的类型会抛IllegalArgumentException异常；

2.put和get方法调用出错将抛出ObjectCreateException 异常；

### 5.2.3. 使用例子
