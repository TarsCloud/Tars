
# tars2php（自动生成php类工具）使用说明

## 简介
tars2php主要功能是通过tars协议文件，自动生成client端和server端php代码，方法大家使用。（server端主要为框架代码，实际业务逻辑需要自己补充实现）

## 基本类型的映射
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

## 复杂类型的映射
针对vector、map、struct三种基本的类型,有一些特殊的打包解包的机制,因此需要引入特别的数据类型:
vector:
```
    vector => \TARS_VECTOR
    它同时具有两个成员函数pushBack()和push_back()
    入参为取决于vector本身是包含什么类型的数组

	例如：
    $shorts = ["test1","test2"];
    $vector = new \TARS_VECTOR(\TARS::STRING); //定义一个string类型的vector
    foreach ($shorts as $short) {
        $vector->pushBack($short); //依次吧test1，test2两个元素，压入vector中
    }
```
map:
```
    map => \TARS_MAP
    它同时具有两个成员函数pushBack()和push_back()
    入参为取决于map本身包含什么类型

    例如：
    $strings = [["test1"=>1],["test2"=>2]];
    $map = new \TARS_MAP(\TARS::STRING,\TARS::INT64); //定义一个key为string,value是int64的map
    foreach ($strings as $string) {
        $map->pushBack($string); //依次把两个元素压入map中，注意pushBack接收一个array，且array只有一个元素
    }
```

struct:
```
    struct => \TARS_Struct
    struct的构造函数比较特殊,接收classname和classfields两个参数
    第一个描述名称,第二个描述struct内的变量的信息

   例如：
	class SimpleStruct extends \TARS_Struct {
		const ID = 0; //TARS文件中每个struct的tag
		const COUNT = 1;

		public $id; //strcut中每个元素的值保存在这里
		public $count; 

		protected static $fields = array(
			self::ID => array(
				'name'=>'id',//struct中每个元素的名称
				'required'=>true,//struct中每个元素是否必须，对应tars文件中的require和optional
				'type'=>\TARS::INT64,//struct中每个元素的类型
				),
			self::COUNT => array(
				'name'=>'count',
				'required'=>true,
				'type'=>\TARS::UINT8,
				),
		);

		public function __construct() {
			parent::__construct('App_Server_Servant.SimpleStruct', self::$fields);
		}
	}
   
```

## tars2php使用方法
如果用户只有使用打包解包需求的,那么使用流程如下:

0. 准备一份tars协议文件，例如example.tars

1. 编写一个tars.proto.php文件，这是用来生成代码的配置文件。
```
//本范例的servant name为PHPTest.PHPServer.obj
return array(
    'appName' => 'PHPTest', //tars服务servant name 的第一部分
    'serverName' => 'PHPServer', //tars服务servant name 的第二部分
    'objName' => 'obj', //tars服务servant name 的第三部分
    'withServant' => true,//决定是服务端,还是客户端的自动生成
    'tarsFiles' => array(
        './example.tars' //tars文件的地址
    ),
    'dstPath' => './server/', //生成php文件的位置
    'namespacePrefix' => 'Server\servant', //生成php文件的命名空间前缀
);
```
2. 执行php ./tars2php.php ./tars.proto.php

3. 工具会根据servant name自动生成三级目录结构，demo中会在./server目录下生成PHPTest/PHPServer/obj/目录，obj目录下的classers是struct对应的php对象，tars目录是tars协议文件本身 。

如example.tars中的struct:
```
struct SimpleStruct {
    0 require long id=0;
    1 require unsigned int count=0;
    2 require short page=0;
};
```
转变成classes/SimpleStruct.php

```
<?php

namespace Server\servant\PHPTest\PHPServer\obj\classes;

class SimpleStruct extends \TARS_Struct {
	const ID = 0; //tars协议中的tag
	const COUNT = 1;
	const PAGE = 2;
	
	public $id; //元素的实际值
	public $count; 
	public $page; 
	
	protected static $_fields = array(
		self::ID => array(
			'name'=>'id', //tars协议中没个元素的name
			'required'=>true, //tars协议中是require或者optional
			'type'=>\TARS::INT64, //类型
			),
		self::COUNT => array(
			'name'=>'count',
			'required'=>true,
			'type'=>\TARS::UINT32,
			),
		self::PAGE => array(
			'name'=>'page',
			'required'=>true,
			'type'=>\TARS::SHORT,
			),
	);

	public function __construct() {
		parent::__construct('PHPTest_PHPServer_obj_SimpleStruct', self::$_fields);
	}
}
```

4. 以example.tars中的interface部分会自动生成单独的已interface命名的php文件。
例如`int testLofofTags(LotofTags tags, out LotofTags outtags);`接口生成的方法如下

server部分
```
<?php
    //注意生成文件中的注释部分会在server启动的时候转换为php代码，如非必要，请勿修改.
    //server部分具体实现需要自己继承完成,注释说明依次如下
    
    //参数为struct类型，对应$tags变量，对应的php对象在\Server\servant\PHPTest\PHPServer\obj\classes\LotofTags
    //参数为struct类型，对应$outtags变量，对应的php对象在\Server\servant\PHPTest\PHPServer\obj\classes\LotofTags，是输出参数
    //接口防止为int
	/**
	 * @param struct $tags \Server\servant\PHPTest\PHPServer\obj\classes\LotofTags
	 * @param struct $outtags \Server\servant\PHPTest\PHPServer\obj\classes\LotofTags =out=
	 * @return int 
	 */
	public function testLofofTags(LotofTags $tags,LotofTags &$outtags);
```
client部分
```
<?php
	try {
		$requestPacket = new RequestPacket(); //构建请求包需要的参数
		$requestPacket->_iVersion = $this->_iVersion;
		$requestPacket->_funcName = __FUNCTION__;
		$requestPacket->_servantName = $this->_servantName;
		
		$encodeBufs = [];

		$__buffer = TUPAPIWrapper::putStruct("tags",1,$tags,$this->_iVersion); //打包第一个参数tags
		$encodeBufs['tags'] = $__buffer;
		
		$requestPacket->_encodeBufs = $encodeBufs; //在请求包中设置请求bufs

		$sBuffer = $this->_communicator->invoke($requestPacket,$this->_iTimeout); //发送请求包，接收返回包

		$ret = TUPAPIWrapper::getStruct("outtags",2,$outtags,$sBuffer,$this->_iVersion); //从返回包中解出第一个输出参数outtags
		return TUPAPIWrapper::getInt32("",0,$sBuffer,$this->_iVersion); //解出返回参数 返回参数 name是空，tag为0

	}
	catch (\Exception $e) {
		throw $e;
	}
```