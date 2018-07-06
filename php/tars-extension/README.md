
# phptars 扩展使用说明

## php扩展能力说明
为了在扩展中实现tars打包解包和tup编码解码的全部体系,所以php扩展主要做了三件事情:
* 将tars的所有数据结构进行了扩展类型的映射
* 将tars的三种复杂类型进行了特殊的扩展类型的映射
* 提供了tup和tars协议的打包解包与编码解码的能力。

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

## 打包解包与编码解码
作为扩展的核心功能,就是提供tars的编解码和打包解包的能力:

### 打包解包
```
// 针对基本类型的打包和解包的方法,输出二进制buf
// iVersion只有1和3两个版本，1版本时$nameOrTagNum需要传入tagNum,方法里面第一个参数为1第二个参数为2以此类推
// 3版本时$nameOrTagNum需要传入name,参数名
$buf = \TASAPI::put*($nameOrTagNum, $value, $iVersion = 3)
$value = \TUPAPI::get*($nameOrTagNum, $buf, $isRequire = false, $iVersion = 3)

// 针对struct,传输对象,返回结果的时候,以数组的方式返回,其元素与类的成员变量一一对应
$buf = \TUPAPI::putStruct($nameOrTagNum, $clazz, $iVersion = 3)
$result = \TUPAPI::getStruct($nameOrTagNum, $clazz, $buf, $isRequire = false, $iVersion = 3)

// 针对vector,传入完成pushBack的vector
$buf = \TUPAPI::putVector($nameOrTagNum, TARS_Vector $clazz, $iVersion = 3)
$value = \TUPAPI::getVector($nameOrTagNum, TARS_Vector $clazz, $buf, $isRequire = false, $iVersion = 3)

// 针对map,传入完成pushBack的map
$buf = \TUPAPI::putMap($nameOrTagNum, TARS_Map $clazz, $iVersion = 3)
$value = \TUPAPI::getMap($nameOrTagNum, TARS_Map $clazz, $buf, $isRequire = false, $iVersion = 3)

// 需要将上述打好包的数据放在一起用来编码
$inbuf_arr[$nameOrTagNum] = $buf
```
### 编码解码
```
//针对tup协议(iVersion=3)的情况：
//这种情况下客户端发包用encode编码，服务端收包用decode解码，服务端回包用encode编码，客户端收包用decode解码
// 进行tup协议的编码,返回结果可以用来传输、持久化
$reqBuffer = \TUPAPI::encode(
                         $iVersion = 3,
                         $iRequestId,
                         $servantName,
                         $funcName,
                         $cPacketType=0,
                         $iMessageType=0,
                         $iTimeout,
                         $context=[],
                         $statuses=[],
                         $bufs)
// 进行tup协议的解码
$ret = \TUPAPI::decode($respBuffer, $iVersion = 3)
$code = $ret['iRet']
$buf = $ret['sBuffer']

//针对tars协议(iVersion=1)的情况：
//这种情况下客户端发包用encode编码，服务端收包用decodeReqPacket解码，服务端回包用encodeRspPacket编码，客户端收包用decode解码

//客户端发包
$reqBuffer = \TUPAPI::encode(
                         $iVersion = 1,
                         $iRequestId,
                         $servantName,
                         $funcName,
                         $cPacketType=0,
                         $iMessageType=0,
                         $iTimeout,
                         $context=[],
                         $statuses=[],
                         $bufs)
//服务端收包，解包
$ret = \TUPAPI::decodeReqPacket($respBuffer)
$code = $ret['iRet']
$buf = $ret['sBuffer']

//服务端回包，打包encodeRspPacket
$reqBuffer = \TUPAPI::encodeRspPacket(
                         $iVersion = 1,
                         $cPacketType,
                         $iMessageType,
                         $iRequestid,
                         $iRet=0,
                         $sResultDesc='',
                         $bufs,
                         $statuses=[])
                         
//客户端收包，解包
$ret = \TUPAPI::decode($respBuffer, $iVersion = 1)
$code = $ret['iRet']
$buf = $ret['sBuffer']
```

对于不同类型的结构的打包解包的更丰富的使用请参考tests/

## tars2php（自动生成php类工具）使用说明
请参见tars2php模块下的文档说明:
[详细说明](https://github.com/Tencent/Tars/blob/master/php/tars2php/README.md)

## 测试用例

### phpunite版本的测试用例
针对扩展的常见使用,增加了测试用例,位于/ext/testcases文件夹下,
测试时只需要执行`php phpunit-4.8.36.phar test.php` 即可完成所有测试用例的执行。其中覆盖到了:
* 所有基本类型的打包解包和编码的测试
* 简单struct类型打包解包和编码的测试
* 简单vector类型的打包解包和编码的测试
* 简单map类型的打包解包和编码的测试
* 复杂vector类型(包含非基本数据类型)的打包解包和编码的测试
* 复杂map类型(包含非基本数据类型)的打包解包和编码的测试
* 复杂struct类型(嵌套vector和map)的打包解包和编码的测试

另外testTARSClient.php和testTARSServer.php是tars协议（iVersion=1）情况下客户端发包，服务端解包 和 服务端回包，客户端解包的测试用例。

注意，需要自行下载phpunit的可执行文件，或直接使用预先安装好的phpunit工具，进行单元测试。

### 同时指出phpt版本的测试用例
安装完成扩展后，执行make test即可。