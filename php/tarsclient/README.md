# phptars使用说明

phptars作为tars在php侧的client方案,主要提供了如下的能力:
* 针对tars协议文件中的struct,自动生成对应的php结构体文件;
* 针对tars协议文件中的interface, 自动生成对应的php接口文件;
* 基于php扩展的高效打包与解包的接口;
* 基于tup的网络收发层的辅助类库;

## 代码结构
* demo -- 使用网络收发类库的实例
    * App -- 根据example.tars自动生成的文件夹,对应的servantName为App.Server.Servant
        * Server
            * Servant
                * classes
                    * SimpleStruct.php -- 由tars中的struct生成的php类文件
                * tars
                    * example.tars
                * servant.php -- 对应interface中的接口文件
    * vendor -- 使用vendor加载tars-assistant网络收发类库
    * composer.json -- 包管理文件
    * testServant.php
* ext -- php扩展源码与测试用例
    * include -- 头文件
    * tests -- 基本功能测试和内存泄露测试
    * auto -- 自动扩展编译脚本
    * config.m4 -- 扩展配置文件
    * tars_c.c -- tars协议描述文件
    * tupapi.c -- 提供给php调用的打包解包、编码解码文件
    * ttars.c -- 针对php扩展中的struct、vector和map对象的管理
    * tup_c.c -- tup协议的编码解码
* tars2php.php -- 代码自动生成工具
* README.md -- 文档

## php扩展能力说明
为了在扩展中实现tars打包解包和tup编码解码的全部体系,所以php扩展主要做了三件事情:
* 将tars的所有数据结构进行了扩展类型的映射
* 将tars的三种复杂类型进行了特殊的扩展类型的映射
* 提供了tup和tars协议的打包解包与编码解码的能力。

### 基本类型的映射
如下是我们对基本类型的映射:
```
    bool => \TARS::BOOL
    char => \TARS::CHAR
    uint8 => \TARS::UINT8
    short => \TARS::SHORT
    uint16 => \TARS::UINT16
    float => \TARS::FLOAT
    double => \TARS::DOUBLE
    int32 => \TARS::INT32
    uint32 => \TARS::UINT32
    int64 => \TARS::INT64
    string => \TARS::STRING
    vector => \TARS::VECTOR
    map => \TARS::MAP
    struct => \TARS::STRUCT
```
当我们需要标识具体的变量类型的时候,就需要用到这些基本的类型了,这些类型都是常量,从1-14。

### 复杂类型的映射
针对vector、map、struct三种基本的类型,有一些特殊的打包解包的机制,因此需要引入特别的数据类型:
```
    vector => \TARS_Vector
    它同时具有两个成员函数pushBack()和push_back()
    入参为取决于vector本身是包含什么类型的数组
```
map也是比较类型的:
```
    map => \TARS_Map
    它同时具有两个成员函数pushBack()和push_back()
    入参为取决于map本身包含什么类型
```

struct:
```
    struct => \TARS_Struct
    struct的构造函数比较特殊,接收classname和classfields两个参数
    第一个描述名称,第二个描述struct内的变量的信息
```

### 打包解包与编码解码
作为扩展的核心功能,就是提供tars的编解码和打包解包的能力:
```
// 针对基本类型的打包和解包的方法,输出二进制buf
$buf = \TASAPI::put*($name, $value)
$value = \TUPAPI::get*($name, $buf)

// 针对struct,传输对象,返回结果的时候,以数组的方式返回,其元素与类的成员变量一一对应
$buf = \TUPAPI::putStruct($name, $clazz)
$result = \TUPAPI::getStruct($name, $clazz, $buf)

// 针对vector,传入完成pushBack的vector
$buf = \TUPAPI::putVector($name, TARS_Vector $clazz)
$value = \TUPAPI::getVector($name, TARS_Vector $clazz, $buf)

// 针对map,传入完成pushBack的map
$buf = \TUPAPI::putMap($name, TARS_Map $clazz)
$value = \TUPAPI::getMap($name, TARS_Map $clazz, $buf)

// 需要将上述打好包的数据放在一起用来编码
$inbuf_arr[$name] = $buf
// 进行tup协议的编码,返回结果可以用来传输、持久化
$reqBuffer = \TUPAPI::encode(
                         $iVersion=3,
                         $iRequestId,
                         $servantName,
                         $funcName,
                         $cPacketType=0,
                         $iMessageType=0,
                         $iTimeout,
                         $context=[],
                         $statuses=[],
                         $inbuf_arr)
// 进行tup协议的解码
$ret = \TUPAPI::decode($respBuffer)
$code = $ret['code']
$buf = $ret['sBuffer']
```

对于不同类型的结构的打包解包的更丰富的使用请参考tests/

## 打包解包能力使用说明
如果用户只有使用打包解包需求的,那么使用流程如下:
0. 使用phpstorm的同学,请访问https://github.com/yuewenweb/tars-ide-helper,下载并引入到phpstorm的依赖库中,即可获得php扩展中的函数和代码的自动提示

1. 将example.tars文件放入与tars2php同级文件夹

2. 执行php tars2php.php example.tars "App.Server.Servant",其后后两个参数分别为tars文件的文件名和tars服务的servantName

3. 生成的文件的目录结构,如上一部分所示,其中的classes部分,即为你需要的php依赖的文件。
如example.tars中的struct:
```
struct SimpleStruct
{
    0 require long id;
    1 require EnumType count;
    2 optional short page =1;
};
```
转变成classes/SimpleStruct.php

```
<?php
class SimpleStruct extends \TARS_Struct {
	// 标识变量的tag
	const ID = 0;
	const COUNT = 1;
	const PAGE = 2;

    // 标识变量本身
	public $id;
	public $count;
	public $page=1;

    // 标识TAG和其对应的名称、是否必选、TARS的类型
	protected static $fields = array(
		self::ID => array(
			'name'=>'id',
			'required'=>true,
			'type'=>\TARS::INT64,
			),
		self::COUNT => array(
			'name'=>'count',
			'required'=>true,
			'type'=>\TARS::UINT8,
			),
		self::PAGE => array(
			'name'=>'page',
			'required'=>false,
			'type'=>\TARS::SHORT,
			),
	);

    // 构造函数
	public function __construct() {
		parent::__construct('App_Server_Servant.SimpleStruct', self::$fields);
	}
}
```

4. 以example.tars中的`int singleParam(string in1, SimpleStruct ss1, out double out1);`为例,打包解包的代码如下:
```
<?php

    // 进行string类型的打包
    $in1 = "test";
    $strBuffer = \TUPAPI::putString("in1",$in1);
    $inbufs["in1"] = $str_buf;

    // 进行struct结构体的打包
    $ss1 = new SimpleStruct();
    $ss1->id = 1;
    $ss1->count = 2;
    $ss1->page = 3;
    $structBuffer = \TUPAPI::putStruct("ss1",$ss1);
    $inbufs["ss1"] = $structBuffer;

    // tup编码
    $reqBuf = \TUPAPI::encode(
                                1,
                                1,
                                "App.Server.Servant",
                                "singleParam",
                                0,
                                0,
                                2,
                                [],
                                [],
                                $inbufs);
    // 这里略去了网络收发包的过程
    $respBuf = sendandreceive();

    // tup解码
    $decodeRet = \TUPAPI::decode($respBuf);
    if($decodeRet['code'] !== 0) {
        // 错误处理
    }
    $buf = $decodeRet['sBuffer'];

    // 完成参数的解包
    $out1 = \TUPAPI::getDouble("out1",$buf);

```

## 网络收发能力使用说明
如果使用我们的网络收发能力基础类库,那么会更加方便一些,使用方式如下:
0. 使用phpstorm的同学,请访问https://github.com/yuewenweb/tars-ide-helper,下载并引入到phpstorm的依赖库中,即可获得php扩展中的函数和代码的自动提示

1. 将example.tars文件放入与tars2php同级文件夹

2. 执行php tars2php.php example.tars "App.Server.Servant",其后后两个参数分别为tars文件的文件名和tars服务的servantName


3. 在composer.json中指定require类库:
```
    "phptars/tars-assistant" : "x.x.x"
```

4. 执行composer install命令安装类库,此时会出现vendor目录

5. 开始写业务代码
```
<?php

    require_once "./vendor/autoload.php";
    // 直接指定服务ip
    $ip = "";// taf服务ip
    $port = 0;// taf服务端口
    $servant = new App\Server\Servant\servant($ip,$port);
    
    // 指定主控ip和端口,无需再指定服务的地址
    $_SERVER['LOCATOR_IP'] = "127.0.0.1";
    $_SERVER['LOCATOR_PORT'] = 17890;
    $servant = new App\Server\Servant\servant();
    

    $in1 = "test";

    $ss1 = new SimpleStruct();
    $ss1->id = 1;
    $ss1->count = 2;
    $ss1->page = 3;

    try {
        $intVal = $servant->singleParam($in1,$ss1,$out1);
    }
    catch(phptars\TarsException $e) {
        // 错误处理
    }
```

相比于不借助我们底层库的写法,这个写法显然简单很多,而且也非常符合rpc的调用思想。所以非常推荐大家使用。


## 测试用例
针对扩展的常见使用,增加了测试用例,位于/ext/testcases文件夹下,
测试时只需要执行`php phpunit-4.8.36.phar test.php` 即可完成所有测试用例的执行。其中覆盖到了:
* 所有基本类型的打包解包和编码的测试
* 简单struct类型打包解包和编码的测试
* 简单vector类型的打包解包和编码的测试
* 简单map类型的打包解包和编码的测试
* 复杂vector类型(包含非基本数据类型)的打包解包和编码的测试
* 复杂map类型(包含非基本数据类型)的打包解包和编码的测试
* 复杂struct类型(嵌套vector和map)的打包解包和编码的测试

注意，需要自行下载phpunit的可执行文件，或直接使用预先安装好的phpunit工具，进行单元测试。
