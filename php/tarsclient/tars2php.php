<?php
/**
 *
 * 说明：
 *    1. } 要单独放在一行，如}后面有;号，要跟}放在一行
 *    2. 每行只能字义一个字段
 *    3. struct不能嵌套定义，如用到别的struct, 把对应的struct 拿出来定义即可
 *
 **/

class Utils {
    public static $preEnums;
    public static $preStructs;

    public static $wholeTypeMap = array(
        'bool' => '\TARS::BOOL',
        'byte' => '\TARS::CHAR',
        'char' => '\TARS::CHAR',
        'unsigned byte' => '\TARS::UINT8',
        'unsigned char' => '\TARS::UINT8',
        'short' => '\TARS::SHORT',
        'unsigned short' => '\TARS::UINT16',
        'int' => '\TARS::INT32',
        'unsigned int' => '\TARS::UINT32',
        'long' => '\TARS::INT64',
        'float' => '\TARS::FLOAT',
        'double' => '\TARS::DOUBLE',
        'string' => '\TARS::STRING',
        'vector' => 'new \TARS_VECTOR',
        'map' => 'new \TARS_MAP',
        'Bool' => '\TARS::BOOL',
        'Byte' => '\TARS::CHAR',
        'Char' => '\TARS::CHAR',
        'Unsigned byte' => '\TARS::UINT8',
        'Unsigned char' => '\TARS::UINT8',
        'Short' => '\TARS::SHORT',
        'Unsigned short' => '\TARS::UINT16',
        'Int' => '\TARS::INT32',
        'Unsigned int' => '\TARS::UINT32',
        'Long' => '\TARS::INT64',
        'Float' => '\TARS::FLOAT',
        'Double' => '\TARS::DOUBLE',
        'String' => '\TARS::STRING',
        'Vector' => 'new \TARS_VECTOR',
        'Map' => 'new \TARS_MAP',
    );

    public static $typeMap = array(
        'bool' => '\TARS::BOOL',
        'byte' => '\TARS::CHAR',
        'char' => '\TARS::CHAR',
        'unsigned byte' => '\TARS::UINT8',
        'unsigned char' => '\TARS::UINT8',
        'short' => '\TARS::SHORT',
        'unsigned short' => '\TARS::UINT16',
        'int' => '\TARS::INT32',
        'unsigned int' => '\TARS::UINT32',
        'long' => '\TARS::INT64',
        'float' => '\TARS::FLOAT',
        'double' => '\TARS::DOUBLE',
        'string' => '\TARS::STRING',
        'vector' => '\TARS::VECTOR',
        'map' => '\TARS::MAP',
        'enum' => '\TARS::UINT8',// 应该不会出现
        'struct' => '\TARS::STRUCT',// 应该不会出现
        'Bool' => '\TARS::BOOL',
        'Byte' => '\TARS::CHAR',
        'Char' => '\TARS::CHAR',
        'Unsigned byte' => '\TARS::UINT8',
        'Unsigned char' => '\TARS::UINT8',
        'Short' => '\TARS::SHORT',
        'Unsigned short' => '\TARS::UINT16',
        'Int' => '\TARS::INT32',
        'Unsigned int' => '\TARS::UINT32',
        'Long' => '\TARS::INT64',
        'Float' => '\TARS::FLOAT',
        'Double' => '\TARS::DOUBLE',
        'String' => '\TARS::STRING',
        'Vector' => '\TARS::VECTOR',
        'Map' => '\TARS::MAP',
        'Enum' => '\TARS::UINT8',// 应该不会出现
        'Struct' => '\TARS::STRUCT',// 应该不会出现
    );

    public static function getPackMethods($type) {
        $packMethods = [
            'bool' => 'putBool',
            'byte' => 'putChar',
            'char' => 'putChar',
            'unsigned byte' => 'putUInt8',
            'unsigned char' => 'putUInt8',
            'short' => 'putShort',
            'unsigned short' => 'putUInt16',
            'int' => 'putInt32',
            'unsigned int' => 'putUInt32',
            'long' => 'putInt64',
            'float' => 'putFloat',
            'double' => 'putDouble',
            'string' => 'putString',
            'enum' => 'putUInt8',
            'map' => 'putMap',
            'vector' => 'putVector',
            'Bool' => 'putBool',
            'Byte' => 'putChar',
            'Char' => 'putChar',
            'Unsigned byte' => 'putUInt8',
            'Unsigned char' => 'putUInt8',
            'Short' => 'putShort',
            'Unsigned short' => 'putUInt16',
            'Int' => 'putInt32',
            'Unsigned int' => 'putUInt32',
            'Long' => 'putInt64',
            'Float' => 'putFloat',
            'Double' => 'putDouble',
            'String' => 'putString',
            'Enum' => 'putUInt8',
            'Map' => 'putMap',
            'Vector' => 'putVector'
        ];

        if(isset($packMethods[$type]))
            return $packMethods[$type];
        else return 'putStruct';
    }

    public static function getUnpackMethods($type) {
        $unpackMethods = [
            'bool' => 'getBool',
            'byte' => 'getChar',
            'char' => 'getChar',
            'unsigned byte' => 'getUInt8',
            'unsigned char' => 'getUInt8',
            'short' => 'getShort',
            'unsigned short' => 'getUInt16',
            'int' => 'getInt32',
            'unsigned int' => 'getUInt32',
            'long' => 'getInt64',
            'float' => 'getFloat',
            'double' => 'getDouble',
            'string' => 'getString',
            'enum' => 'getUInt8',
            'map' => 'getMap',
            'vector' => 'getVector',
            'Bool' => 'getBool',
            'Byte' => 'getChar',
            'Char' => 'getChar',
            'Unsigned byte' => 'getUInt8',
            'Unsigned char' => 'getUInt8',
            'Short' => 'getShort',
            'Unsigned short' => 'getUInt16',
            'Int' => 'getInt32',
            'Unsigned int' => 'getUInt32',
            'Long' => 'getInt64',
            'Float' => 'getFloat',
            'Double' => 'getDouble',
            'String' => 'getString',
            'Enum' => 'getUInt8',
            'Map' => 'getMap',
            'Vector' => 'getVector'
        ];


        if(isset($unpackMethods[strtolower($type)]))
            return $unpackMethods[strtolower($type)];
        else return 'getStruct';
    }

    /**
     * @param $char
     * @return int
     * 判断是不是tag
     */
    public static function isTag($word) {
        if(!is_numeric($word)) {
            return false;
        }
        else return true;
    }

    /**
     * @param $word
     * @return bool
     * 判断收集到的word是不是
     */
    public static function isRequireType($word) {
        return in_array(strtolower($word),['require','optional']);
    }

    public static function isBasicType($word) {
        $basicTypes = [
            'bool','byte','char','unsigned byte', 'unsigned char', 'short','unsigned short',
            'int' ,'unsigned int','long','float','double','string', 'void'
        ];
        return in_array(strtolower($word),$basicTypes);
    }

    public static function isEnum($word,$preEnums) {
        return in_array($word,$preEnums);
    }

    public static function isMap($word) {
        return strtolower($word) == 'map';
    }

    public static function isStruct($word,$preStructs) {
        return in_array($word,$preStructs);
    }

    public static function isVector($word) {
        return strtolower($word) == 'vector';
    }

    public static function isSpace($char) {
        if($char == ' ' || $char == "\t")
            return true;
        else return false;
    }

    public static function paramTypeMap($paramType) {
        if(Utils::isBasicType($paramType) || Utils::isMap($paramType) || Utils::isVector($paramType)) {
            return "";
        }
        else {
            return $paramType;
        }
    }

    public static function getRealType($type) {
        if(isset(Utils::$typeMap[strtolower($type)])) return Utils::$typeMap[strtolower($type)];
        else return '\TARS::STRUCT';
    }

    public static function inIdentifier($char) {
        return ($char >= 'a' & $char <= 'z') |
        ($char >= 'A' & $char <= 'Z')|
        ($char >= '0' & $char <= '9') |
        ($char == '_');
    }


    public static function abnormalExit($level,$msg) {
        echo "[$level]$msg"."\n";
        exit;
    }

    public static function pregMatchByName($name='enum',$line) {
        // 处理第一行,正则匹配出classname
        $Tokens = preg_split("/$name/", $line);

        $mathName = $Tokens[1];
        $mathName = trim($mathName," \r\0\x0B\t\n{");

        preg_match('/[a-zA-Z][0-9a-zA-Z]/',$mathName,$matches);
        if(empty($matches)) {
            //Utils::abnormalExit('error',$name.'名称有误'.$line);
        }
        return $mathName;

    }

    public static function isReturn($char) {
        if($char == "\n" || $char == '\r' || bin2hex($char) == '0a' || bin2hex($char) == '0b' ||
            bin2hex($char) == '0c' || bin2hex($char) == '0d')
            return true;
        else return false;
    }
}

class FileConverter
{
    public $moduleName;
    public $uniqueName;
    public $interfaceName;
    public $fromFile;

    public $servantName;

    public $namespaceName;

    public $preStructs=[];
    public $preEnums=[];
    public $preConsts=[];
    public $preNamespaceStructs=[];
    public $preNamespaceEnums=[];

    public function __construct($fromFile, $servantName)
    {
        $this->fromFile = $fromFile;
        $this->servantName = $servantName;
        $this->initDir();
    }

    /**
     * 首先需要初始化一些文件目录
     * @return [type] [description]
     */
    public function initDir() {
        $moduleElements = explode(".",$this->servantName);

        $product = $moduleElements[0];
        $project = $moduleElements[1];
        $service = $moduleElements[2];
        if (strtolower(substr(php_uname('a'), 0, 3)) === 'win') {
            exec("mkdir " . $product);
            exec("mkdir " . $product . "\\" . $project);
            exec("DEL " . $product . "\\" . $project . "\\" . $service . "\\*.*");
            exec("mkdir " . $product . "\\" . $project . "\\" . $service);

            $this->moduleName = $product . "\\" . $project . "\\" . $service;

            exec("mkdir " . $this->moduleName . "\\classes");
            exec("mkdir " . $this->moduleName . "\\tars");
            exec("copy " . $this->fromFile . " " . $this->moduleName . "\\tars");

            $this->namespaceName = $product . "\\" . $project . "\\" . $service;

            $this->uniqueName = $product . "_" . $project . "_" . $service;
            return;
        }

        exec("mkdir ".$product);
        exec("mkdir ".$product."/".$project);
        exec("rm -rf ".$product."/".$project."/".$service);
        exec("mkdir ".$product."/".$project."/".$service);

        $this->moduleName = $product."/".$project."/".$service;

        exec("mkdir ".$this->moduleName."/classes");
        exec("mkdir ".$this->moduleName."/tars");
        exec("cp ./".$this->fromFile." ".$this->moduleName."/tars");

        $this->namespaceName = $product."\\".$project."\\".$service;

        $this->uniqueName = $product."_".$project."_".$service;
    }

    public function usage()
    {
        echo 'php tars2php.php $tars_file $servantName';
    }

    public function moduleScan()
    {
        $fp = fopen($this->fromFile, 'r');
        if (!$fp) {
            $this->usage();
            exit;
        }
        while (($line = fgets($fp, 1024)) !== false) {

            // 判断是否有module
            $moduleFlag = strpos($line,"module");
            if($moduleFlag !== false) {
                $name = Utils::pregMatchByName("module",$line);
                $currentModule = $name;
            }

            // 判断是否有include
            $includeFlag = strpos($line,"#include");
            if($includeFlag !== false) {
                // 找出jce对应的文件名
                $tokens = preg_split("/#include/", $line);
                $includeFile = trim($tokens[1],"\" \r\n");

                if (strtolower(substr(php_uname('a'), 0, 3)) === 'win') {
                    exec("copy " . $includeFile . " " . $this->moduleName . "\\tars");
                }else {
                    exec("cp " . $includeFile . " " . $this->moduleName . "/tars");
                }

                $includeParser = new IncludeParser();
                $includeParser->includeScan($includeFile,$this->preEnums,$this->preStructs,
                    $this->preNamespaceEnums,$this->preNamespaceStructs);
            }

            // 如果空行，或者是注释，就直接略过
            if (!$line || trim($line) == '' || trim($line)[0] === '/' || trim($line)[0] === '*' || trim($line) === '{') {
                continue;
            }

            // 正则匹配,发现是在enum中
            $enumFlag = strpos($line,"enum");
            if($enumFlag !== false) {
                $name = Utils::pregMatchByName("enum",$line);
                if(!empty($name)) {
                    $this->preEnums[] = $name;

                    // 增加命名空间以备不时之需
                    if(!empty($currentModule))
                        $this->preNamespaceEnums[] = $currentModule."::".$name;

                    while(($lastChar = fgetc($fp)) != '}') {
                        continue;
                    }
                }
            }

            // 正则匹配，发现是在结构体中
            $structFlag = strpos($line, "struct");
            // 一旦发现了struct，那么持续读到结束为止
            if ($structFlag !== false) {
                $name = Utils::pregMatchByName("struct",$line);

                if(!empty($name)) {
                    $this->preStructs[] = $name;
                    // 增加命名空间以备不时之需
                    if(!empty($currentModule))
                        $this->preNamespaceStructs[] = $currentModule."::".$name;
                }

            }
        }
        fclose($fp);
    }

    public function moduleParse()
    {
        $fp = fopen($this->fromFile, 'r');
        if (!$fp) {
            $this->usage();
            exit;
        }
        while (($line = fgets($fp, 1024)) !== false) {

            // 判断是否有include
            $includeFlag = strpos($line,"#include");
            if($includeFlag !== false) {
                // 找出jce对应的文件名
                $tokens = preg_split("/#include/", $line);
                $includeFile = trim($tokens[1],"\" \r\n");
                $includeParser = new IncludeParser();
                $includeParser->includeParse($includeFile,$this->preEnums,$this->preStructs,$this->uniqueName,
                    $this->moduleName,$this->namespaceName,$this->servantName,$this->preNamespaceEnums,$this->preNamespaceStructs);
            }

            // 如果空行，或者是注释，就直接略过
            if (!$line || trim($line) == '' || trim($line)[0] === '/' || trim($line)[0] === '*') {
                continue;
            }
            // 正则匹配,发现是在consts中
            $constFlag = strpos($line,"const");
            if($constFlag !== false) {
                // 直接进行正则匹配
                Utils::abnormalExit('warning','const is not supported, please make sure you deal with them yourself in this version!');
            }


            // 正则匹配，发现是在结构体中
            $structFlag = strpos($line, "struct");
            // 一旦发现了struct，那么持续读到结束为止
            if ($structFlag !== false) {
                $name = Utils::pregMatchByName("struct",$line);

                $structParser = new StructParser($fp,$line,$this->uniqueName,$this->moduleName,$name,$this->preStructs,
                    $this->preEnums,$this->namespaceName,$this->preNamespaceEnums,$this->preNamespaceStructs);
                $structClassStr = $structParser->parse();
                file_put_contents($this->moduleName."/classes/".$name.".php", $structClassStr);

            }

            // 正则匹配，发现是在interface中
            $interfaceFlag = strpos(strtolower($line), "interface");
            // 一旦发现了struct，那么持续读到结束为止
            if ($interfaceFlag !== false) {
                $name = Utils::pregMatchByName("interface",$line);

                if(in_array($name,$this->preStructs)) {
                    $name .= "Servant";
                }

                $interfaceParser = new InterfaceParser($fp,$line,$this->namespaceName,$this->moduleName,
                    $name,$this->preStructs,
                    $this->preEnums,$this->servantName,$this->preNamespaceEnums,$this->preNamespaceStructs);
                $interfaces = $interfaceParser->parse();

                // 需要区分同步和异步的两种方式
                file_put_contents($this->moduleName."/".$name.".php", $interfaces['syn']);
            }
        }
    }
}


class IncludeParser {
    public function includeScan($includeFile,&$preEnums,&$preStructs,
                                &$preNamespaceEnums,&$preNamespaceStructs)
    {
        $fp = fopen($includeFile, 'r');
        if (!$fp) {
            echo "Include file not exit, please check";
            exit;
        }
        while (($line = fgets($fp, 1024)) !== false) {
            // 如果空行，或者是注释，就直接略过
            if (!$line || trim($line) == ''|| trim($line)[0] === '/' || trim($line)[0] === '*') {
                continue;
            }

            // 判断是否有module
            $moduleFlag = strpos($line,"module");
            if($moduleFlag !== false) {
                $name = Utils::pregMatchByName("module",$line);
                $currentModule = $name;
            }

            // 正则匹配,发现是在enum中
            $enumFlag = strpos($line,"enum");
            if($enumFlag !== false) {
                $name = Utils::pregMatchByName("enum",$line);
                $preEnums[] = $name;
                if(!empty($currentModule))
                    $preNamespaceEnums[] = $currentModule."::".$name;
                while(($lastChar = fgetc($fp)) != '}') {
                    continue;
                }
            }

            // 正则匹配，发现是在结构体中
            $structFlag = strpos($line, "struct");
            // 一旦发现了struct，那么持续读到结束为止
            if ($structFlag !== false) {
                $name = Utils::pregMatchByName("struct",$line);

                $preStructs[] = $name;
                if(!empty($currentModule))
                    $preNamespaceStructs[] = $currentModule."::".$name;
            }
        }
    }


    public function includeParse($includeFile,&$preEnums,&$preStructs,$uniqueName,$moduleName,$namespaceName,$servantName,
                                 &$preNamespaceEnums,&$preNamespaceStructs)
    {
        $fp = fopen($includeFile, 'r');
        if (!$fp) {
            echo "Include file not exit, please check";
            exit;
        }
        while (($line = fgets($fp, 1024)) !== false) {
            // 如果空行，或者是注释，就直接略过
            if (!$line || trim($line) == '' || trim($line)[0] === '/' || trim($line)[0] === '*') {
                continue;
            }

            // 正则匹配,发现是在consts中
            $constFlag = strpos($line,"const");
            if($constFlag !== false) {
                // 直接进行正则匹配
                echo 'CONST is not supported, please make sure you deal with them yourself in this version!';
            }

            // 正则匹配，发现是在结构体中
            $structFlag = strpos($line, "struct");
            // 一旦发现了struct，那么持续读到结束为止
            if ($structFlag !== false) {
                $name = Utils::pregMatchByName("struct",$line);

                $structParser = new StructParser($fp,$line,$uniqueName,$moduleName,$name,$preStructs,
                    $preEnums,$namespaceName,$preNamespaceEnums,$preNamespaceStructs);
                $structClassStr = $structParser->parse();
                file_put_contents($moduleName."/classes/".$name.".php", $structClassStr);

            }

            // 正则匹配，发现是在interface中
            $interfaceFlag = strpos(strtolower($line), "interface");
            // 一旦发现了struct，那么持续读到结束为止
            if ($interfaceFlag !== false) {
                $name = Utils::pregMatchByName("interface",$line);

                if(in_array($name,$preStructs)) {
                    $name .= "Servant";
                }

                $interfaceParser = new InterfaceParser($fp,$line,$namespaceName,$moduleName,
                    $name,$preStructs,
                    $preEnums,$servantName,$preNamespaceEnums,$preNamespaceStructs);
                $interfaces = $interfaceParser->parse();

                // 需要区分同步和异步的两种方式
                file_put_contents($moduleName."/".$name.".php", $interfaces['syn']);
            }
        }
    }
}


class InterfaceParser {

    public $namespaceName;
    public $moduleName;
    public $interfaceName;
    public $asInterfaceName;

    public $state;

    // 这个结构体,可能会引用的部分,包括其他的结构体、枚举类型、常量
    public $useStructs=[];
    public $extraUse;
    public $preStructs;
    public $preEnums;

    public $preNamespaceStructs;
    public $preNamespaceEnums;

    public $returnSymbol = "\n";
    public $doubleReturn = "\n\n";
    public $tabSymbol = "\t";
    public $doubleTab = "\t\t";
    public $tripleTab = "\t\t\t";
    public $quardupleTab = "\t\t\t\t";


    public $extraContructs = '';
    public $extraExtInit = '';

    public $consts='';
    public $variables = '';
    public $fields = '';

    public $funcSet = '';

    public $servantName;


    public function __construct($fp,$line,$namespaceName,$moduleName,
                                $interfaceName,$preStructs,
                                $preEnums,$servantName,$preNamespaceEnums,$preNamespaceStructs)
    {
        $this->fp = $fp;
        $this->namespaceName = $namespaceName;
        $this->moduleName = $moduleName;
        $this->preStructs = $preStructs;
        $this->preEnums = $preEnums;
        $this->interfaceName = $interfaceName;
        $this->servantName = $servantName;

        $this->extraUse = '';
        $this->useStructs = [];

        $this->preNamespaceEnums = $preNamespaceEnums;
        $this->preNamespaceStructs = $preNamespaceStructs;

    }

    public function copyAnnotation() {
        // 再读入一个字符
        $nextChar = fgetc($this->fp);
        // 第一种
        if($nextChar == '/') {
            while (1) {
                $tmpChar = fgetc($this->fp);

                if($tmpChar == "\n") {
                    $this->state = 'lineEnd';
                    break;
                }
            }
            return;
        }
        else if($nextChar == '*') {
            while (1) {
                $tmpChar =fgetc($this->fp);

                if($tmpChar === false) {
                    Utils::abnormalExit('error','注释换行错误,请检查'.$tmpChar);
                }
                else if($tmpChar === "\n") {

                }
                else if(($tmpChar) === '*') {
                    $nextnextChar = fgetc($this->fp);
                    if($nextnextChar == '/') {
                        return;
                    }
                    else{
                        $pos = ftell($this->fp);
                        fseek($this->fp,$pos - 1);
                    }
                }
            }
        }
        // 注释不正常
        else {
            Utils::abnormalExit('error','注释换行错误,请检查'.$nextChar);
        }
    }

    public function getFileHeader($prefix="") {
        return "<?php\n\nnamespace ".$this->namespaceName.$prefix.";".
        $this->doubleReturn;
    }

    public function getInterfaceBasic() {

        return $this->tabSymbol."private \$_tarsAssistant;".$this->returnSymbol.
        $this->tabSymbol."private \$_servantName = \"$this->servantName\";".$this->doubleReturn.
        $this->tabSymbol."private \$_ip;".$this->returnSymbol.
        $this->tabSymbol."private \$_port;".$this->doubleReturn.
        $this->tabSymbol."public function __construct(\$ip=\"\",\$port=\"\") {".$this->returnSymbol.
        $this->doubleTab."\$this->_tarsAssistant = new \phptars\TarsAssistant();".$this->returnSymbol.
        $this->doubleTab."\$this->_ip = \$ip;".$this->returnSymbol.
        $this->doubleTab."\$this->_port = \$port;".$this->returnSymbol.
        $this->tabSymbol."}".$this->doubleReturn;
    }

    public function parse() {

        while ($this->state != 'end') {
            $this->state = 'init';
            $this->InterfaceFuncParseLine();
        }

        $interfaceClass = $this->getFileHeader("").$this->extraUse."class ".$this->interfaceName." {".$this->returnSymbol;

        $interfaceClass .= $this->getInterfaceBasic();

        $interfaceClass .= $this->funcSet;

        $interfaceClass .= "}".$this->doubleReturn;

        return [
            'syn' => $interfaceClass,
        ];
    }

    /**
     * @param $fp
     * @param $line
     * 这里必须要引入状态机了
     */
    public function InterfaceFuncParseLine() {

        $line = '';
        $this->state = 'init';
        while(1) {

            if($this->state == 'init') {
                $char =fgetc($this->fp);

                // 有可能是换行
                if($char == '{' || Utils::isReturn($char)) {
                    continue;
                }
                // 遇到了注释会用贪婪算法全部处理完,同时填充到struct的类里面去
                else if($char == '/') {
                    $this->copyAnnotation();
                    break;
                }
                else if(Utils::inIdentifier($char)) {
                    $this->state = 'identifier';
                    $line .= $char;
                }
                // 终止条件之1,宣告struct结束
                else if($char == '}') {
                    // 需要贪心的读到"\n"为止
                    while(($lastChar=fgetc($this->fp)) != "\n") {
                        continue;
                    }
                    $this->state = 'end';
                    break;
                }
            }
            else if($this->state == 'identifier') {
                $char =fgetc($this->fp);

                if($char == '/') {
                    $this->copyAnnotation();
                }
                else if ($char == ';') {
                    $line .= $char;
                    break;
                }
                // 终止条件之2,同样宣告interface结束
                else if($char == '}') {
                    // 需要贪心的读到"\n"为止
                    while(($lastChar=fgetc($this->fp)) != "\n") {
                        continue;
                    }
                    $this->state = 'end';
                }
                else if(Utils::isReturn($char)){
                    continue;
                }
                else if($char == ')') {
                    $line .= $char;
                    // 需要贪心的读到"\n"为止
                    while(($lastChar=fgetc($this->fp)) != "\n") {
                        continue;
                    }
                    $this->state = 'lineEnd';
                }
                else $line .= $char;
            }
            else if($this->state == 'lineEnd') {
                $char =fgetc($this->fp);
                if($char == '}'){
                    // 需要贪心的读到"\n"为止
                    while(($lastChar=fgetc($this->fp)) != "\n") {
                        continue;
                    }
                    $this->state = 'end';
                }
                break;
            }
            else if($this->state == 'end') {
                break;
            }
        }

        if(empty($line)) return;

        $line = trim($line);
        // 如果空行，或者是注释，或者是大括号就直接略过
        if (!$line || $line[0] === '/' || $line[0] === '*' || $line === '{') {
            return;
        }

        $endFlag = strpos($line, "};");
        if ($endFlag !== false) {
            $this->state = 'end';
            return;
        }

        $endFlag = strpos($line, "}");
        if ($endFlag !== false) {
            $this->state = 'end';
            return;
        }

        // 有必要先分成三个部分,返回类型、接口名、参数列表
        $tokens = preg_split('/\(/',$line,2);
        $mix = $tokens[0];
        $rest = $tokens[1];

        $pices = preg_split('/\s+/', $mix);

        $funcName = $pices[count($pices) - 1];

        $returnType = implode("",array_slice($pices,0,count($pices) - 1));

        $this->state = 'init';
        $word = '';

        $params = [];

        for($i = 0; $i < strlen($rest); $i++) {
            $char = $rest[$i];

            if($this->state == 'init') {
                // 有可能是换行
                if($char == '(' || Utils::isSpace($char)) {
                    continue;
                }
                else if($char == "\n") {
                    break;
                }

                else if(Utils::inIdentifier($char)) {
                    $this->state = 'identifier';
                    $word .= $char;
                }
                // 终止条件之1,宣告interface结束
                else if($char == ')') {
                    break;
                }
                else {
                    Utils::abnormalExit('error','Interface:'.$this->interfaceName.'内格式错误,请更正tars');
                }
            }
            else if($this->state == 'identifier') {
                if($char == ',') {
                    $params[] = $word;
                    $this->state = 'init';
                    $word = '';
                    continue;
                }
                // 标志着map和vector的开始,不等到'>'的结束不罢休
                // 这时候需要使用栈来push,然后一个个对应的pop,从而达到type的遍历
                else if($char == '<') {
                    $mapVectorStack = [];
                    $word .= $char;
                    array_push($mapVectorStack,'<');
                    while(!empty($mapVectorStack)) {
                        $moreChar = $rest[$i+1];
                        $word .= $moreChar;
                        if($moreChar == '<') {
                            array_push($mapVectorStack,'<');
                        }
                        else if($moreChar == '>') {
                            array_pop($mapVectorStack);
                        }
                        $i++;
                    }
                    continue;
                }
                else if($char == ")"){
                    $params[] = $word;
                    break;
                }
                else if ($char == ';') {
                    continue;
                }
                // 终止条件之2,同样宣告struct结束
                else if($char == '}') {
                    $this->state = 'end';
                }
                else if($char == "\n"){
                    break;
                }
                else $word .= $char;
            }
            else if($this->state == 'lineEnd') {
                break;
            }
            else if($this->state == 'end') {
                break;
            }
        }

        $this->writeInterfaceLine($returnType,$funcName,$params);

    }

    /**
     * @param $wholeType
     * 通过完整的类型获取vector的扩展类型
     * vector<CateObj> => new \TARS_VECTOR(new CateObj())
     * vector<string> => new \TARS_VECTOR(\TARS::STRING)
     * vector<map<string,CateObj>> => new \TARS_VECTOR(new \TARS_MAP(\TARS_MAP,new CateObj()))
     */
    public function getExtType($wholeType,$valueName) {
        $state = 'init';
        $word = '';
        $extType = '';

        for($i = 0; $i < strlen($wholeType);$i++) {
            $char = $wholeType[$i];
            if($state == 'init') {
                // 如果遇到了空格
                if(Utils::isSpace($char)) {
                    continue;
                }
                // 回车是停止符号
                else if(Utils::inIdentifier($char)) {
                    $state = 'indentifier';
                    $word .= $char;
                }
                else if(Utils::isReturn($char)) {
                    break;
                }
                else if($char == '>') {
                    $extType .= ")";
                    continue;
                }
            }
            else if($state == 'indentifier') {
                if($char == '<') {
                    // 替换word,替换< 恢复初始状态
                    $tmp = $this->VecMapReplace($word);
                    $extType .= $tmp;
                    $extType .= "(";
                    $word = '';
                    $state = 'init';
                }
                else if($char == '>') {
                    // 替换word,替换> 恢复初始状态
                    // 替换word,替换< 恢复初始状态
                    $tmp = $this->VecMapReplace($word);
                    $extType .= $tmp;
                    $extType .= ")";
                    $word = '';
                    $state = 'init';
                }
                else if($char == ',') {
                    // 替换word,替换, 恢复初始状态
                    // 替换word,替换< 恢复初始状态
                    $tmp = $this->VecMapReplace($word);
                    $extType .= $tmp;
                    $extType .= ",";
                    $word = '';
                    $state = 'init';
                }
                else {
                    $word .= $char;
                    continue;
                }
            }
        }

        return $extType;
    }


    public function VecMapReplace($word) {
        $word = trim($word);
        // 遍历所有的类型
        foreach (Utils::$wholeTypeMap as $key => $value) {
            if(Utils::isStruct($word,$this->preStructs)) {
                if(!in_array($word,$this->useStructs)) {
                    $this->extraUse .= "use ".$this->namespaceName."\\classes\\".$word.";".$this->returnSymbol;
                    $this->useStructs[] = $word;
                }

                $word = "new " . $word . "()";
            }
            else if(in_array($word,$this->preNamespaceStructs)) {
                $words = explode("::",$word);
                $word = $words[1];
                if(!in_array($word,$this->useStructs)) {
                    $this->extraUse .= "use protocol\\".$this->namespaceName."\\classes\\".$word.";".$this->returnSymbol;
                    $this->useStructs[] = $word;
                }

                $word = "new " . $word . "()";
                break;
            }
            else $word = str_replace($key,$value,$word);
        }

        return $word;
    }


    public function paramParser($params)
    {

        // 输入和输出的参数全部捋一遍
        $inParams = [];
        $outParams = [];
        foreach ($params as $param) {
            $this->state = 'init';
            $word = '';
            $wholeType = '';
            $paramType = 'in';
            $type = '';
            $mapVectorState = false;

            for ($i = 0; $i < strlen($param) ; $i++) {
                $char = $param[$i];
                if($this->state == 'init') {
                    // 有可能是换行
                    if(Utils::isSpace($char)) {
                        continue;
                    }
                    else if($char == "\n") {
                        break;
                    }
                    else if(Utils::inIdentifier($char)) {
                        $this->state = 'identifier';
                        $word .= $char;
                    }
                    else {
                        Utils::abnormalExit('error','Interface:'.$this->interfaceName.'内格式错误,请更正tars');
                    }
                }
                else if($this->state == 'identifier') {
                    // 如果遇到了space,需要检查是不是在map或vector的类型中,如果当前积累的word并不合法
                    // 并且又不是处在vector或map的前置状态下的话,那么就是出错了
                    if(Utils::isSpace($char)) {
                        if($word == 'out') {
                            $paramType = $word;
                            $this->state = 'init';
                            $word = '';
                        }
                        else if(Utils::isBasicType($word)) {
                            $type = $word;
                            $this->state = 'init';
                            $word = '';
                        }
                        else if(Utils::isStruct($word,$this->preStructs)) {

                            // 同时要把它增加到本Interface的依赖中
                            if(!in_array($word,$this->useStructs)) {
                                $this->extraUse .= "use ".$this->namespaceName."\\classes\\".$word.";".$this->returnSymbol;
                                $this->useStructs[] = $word;
                            }

                            $type = $word;
                            $this->state = 'init';
                            $word = '';
                        }
                        else if(Utils::isEnum($word,$this->preEnums)) {
                            $type = 'unsigned byte';
                            $this->state = 'init';
                            $word = '';
                        }
                        else if(in_array($word,$this->preNamespaceStructs)) {
                            $word = explode("::",$word);
                            $word = $word[1];
                            // 同时要把它增加到本Interface的依赖中
                            if(!in_array($word,$this->useStructs)) {
                                $this->extraUse .= "use ".$this->namespaceName."\\classes\\".$word.";".$this->returnSymbol;
                                $this->useStructs[] = $word;
                            }

                            $type = $word;
                            $this->state = 'init';
                            $word = '';
                        }
                        else if(in_array($word,$this->preNamespaceEnums)) {
                            $type = 'unsigned byte';
                            $this->state = 'init';
                            $word = '';
                        }
                        else if(Utils::isMap($word)) {
                            $mapVectorState = true;
                        }
                        else if(Utils::isVector($word)) {
                            $mapVectorState = true;
                        }
                        else {
                            // 读到了vector和map中间的空格,还没读完
                            if($mapVectorState) {
                                continue;
                            }
                            // 否则剩余的部分应该就是值和默认值
                            else {
                                if(!empty($word))
                                    $valueName = $word;
                                $this->state = 'init';
                                $word = '';
                            }
                        }
                    }
                    // 标志着map和vector的开始,不等到'>'的结束不罢休
                    // 这时候需要使用栈来push,然后一个个对应的pop,从而达到type的遍历
                    else if($char == '<') {
                        // 贪婪的向后,直到找出所有的'>'
                        $type = $word;
                        // 还会有一个wholeType,表示完整的部分
                        $mapVectorStack = [];
                        $wholeType = $type;
                        $wholeType .= '<';
                        array_push($mapVectorStack,'<');
                        while(!empty($mapVectorStack)) {
                            $moreChar = $param[$i+1];
                            $wholeType .= $moreChar;
                            if($moreChar == '<') {
                                array_push($mapVectorStack,'<');
                            }
                            else if($moreChar == '>') {
                                array_pop($mapVectorStack);
                            }
                            $i++;
                        }

                        $this->state = 'init';
                        $word = '';
                    }
                    else $word .= $char;
                }
            }


            if(!empty($word)) {
                $valueName = $word;
            }

            if($paramType == 'in') {
                $inParams[] = [
                    'type' => $type,
                    'wholeType' => $wholeType,
                    'valueName' => $valueName
                ];
            }
            else {
                $outParams[] = [
                    'type' => $type,
                    'wholeType' => $wholeType,
                    'valueName' => $valueName
                ];
            }
        }

        return [
            'in' => $inParams,
            'out' => $outParams
        ];

    }

    public function returnParser($returnType)
    {
        if(Utils::isStruct($returnType,$this->preStructs)) {
            if(!in_array($returnType,$this->useStructs)) {
                $this->extraUse .= "use ".$this->namespaceName."\\classes\\".$returnType.";".$this->returnSymbol;
                $this->useStructs[] = $returnType;
            }
            $returnInfo = [
                'type' => $returnType,
                'wholeType' => $returnType,
                'valueName' => $returnType
            ];
            return $returnInfo;
        }
        else if( Utils::isBasicType($returnType)) {
            $returnInfo = [
                'type' => $returnType,
                'wholeType' => $returnType,
                'valueName' => $returnType
            ];
            return $returnInfo;
        }

        $this->state = 'init';
        $word = '';
        $wholeType = '';
        $type = '';
        $mapVectorState = false;
        $valueName = '';

        for ($i = 0; $i < strlen($returnType) ; $i++) {
            $char = $returnType[$i];
            if($this->state == 'init') {
                // 有可能是换行
                if(Utils::isSpace($char)) {
                    continue;
                }
                else if($char == "\n") {
                    break;
                }
                else if(Utils::inIdentifier($char)) {
                    $this->state = 'identifier';
                    $word .= $char;
                }
                else {
                    Utils::abnormalExit('error','Interface内格式错误,请更正jce');
                }
            }
            else if($this->state == 'identifier') {
                // 如果遇到了space,需要检查是不是在map或vector的类型中,如果当前积累的word并不合法
                // 并且又不是处在vector或map的前置状态下的话,那么就是出错了
                //echo "[debug][state={$this->state}]word:".$word."\n";
                if(Utils::isSpace($char)) {
                    if(Utils::isBasicType($word)) {
                        $type = $word;
                        $this->state = 'init';
                        $word = '';
                    }
                    else if(Utils::isStruct($word,$this->preStructs)) {

                        // 同时要把它增加到本Interface的依赖中
                        if(!in_array($word,$this->useStructs)) {
                            $this->extraUse .= "use ".$this->namespaceName."\\classes\\".$word.";".$this->returnSymbol;
                            $this->useStructs[] = $word;
                        }


                        $type = $word;
                        $this->state = 'init';
                        $word = '';
                    }
                    else if(Utils::isEnum($word,$this->preEnums)) {
                        $type = 'unsigned byte';
                        $this->state = 'init';
                        $word = '';
                    }
                    else if(in_array($word,$this->preNamespaceStructs)) {
                        $word = explode("::",$word);
                        $word = $word[1];
                        // 同时要把它增加到本Interface的依赖中
                        if(!in_array($word,$this->useStructs)) {
                            $this->extraUse .= "use ".$this->namespaceName."\\classes\\".$word.";".$this->returnSymbol;
                            $this->useStructs[] = $word;
                        }

                        $type = $word;
                        $this->state = 'init';
                        $word = '';
                    }
                    else if(in_array($word,$this->preNamespaceEnums)) {
                        $type = 'unsigned byte';
                        $this->state = 'init';
                        $word = '';
                    }
                    else if(Utils::isMap($word)) {
                        $mapVectorState = true;
                    }
                    else if(Utils::isVector($word)) {
                        $mapVectorState = true;
                    }
                    else {
                        // 读到了vector和map中间的空格,还没读完
                        if($mapVectorState) {
                            continue;
                        }
                        // 否则剩余的部分应该就是值和默认值
                        else {
                            if(!empty($word))
                                $valueName = $word;
                            $this->state = 'init';
                            $word = '';
                        }
                    }
                }
                // 标志着map和vector的开始,不等到'>'的结束不罢休
                // 这时候需要使用栈来push,然后一个个对应的pop,从而达到type的遍历
                else if($char == '<') {
                    // 贪婪的向后,直到找出所有的'>'
                    $type = $word;
                    // 还会有一个wholeType,表示完整的部分
                    $mapVectorStack = [];
                    $wholeType = $type;
                    $wholeType .= '<';
                    array_push($mapVectorStack,'<');
                    while(!empty($mapVectorStack)) {
                        $moreChar = $returnType[$i+1];
                        $wholeType .= $moreChar;
                        if($moreChar == '<') {
                            array_push($mapVectorStack,'<');
                        }
                        else if($moreChar == '>') {
                            array_pop($mapVectorStack);
                        }
                        $i++;
                    }

                    $this->state = 'init';
                    $word = '';
                }
                else $word .= $char;
            }
        }

        $returnInfo = [
            'type' => $type,
            'wholeType' => $wholeType,
            'valueName' => $valueName
        ];


        return $returnInfo;

    }
    /**
     * @param $tag
     * @param $requireType
     * @param $type
     * @param $name
     * @param $wholeType
     * @param $defaultValue
     */
    public function writeInterfaceLine($returnType,$funcName,$params) {

        $result = $this->paramParser($params);
        $inParams = $result['in'];
        $outParams = $result['out'];

        // 处理通用的头部
        $funcHeader = $this->generateFuncHeader($funcName,$inParams,$outParams);

        $returnInfo = $this->returnParser($returnType);

        $funcBodyArr = $this->generateFuncBody($inParams,$outParams,$returnInfo);
        $synFuncBody = $funcBodyArr['syn'];


        $funcTail = $this->tabSymbol."}".$this->doubleReturn;

        $this->funcSet .= $funcHeader.$synFuncBody.$funcTail;

    }


    /**
     * @param $funcName
     * @param $inParams
     * @param $outParams
     * @return string
     */
    public function generateFuncHeader($funcName,$inParams,$outParams) {
        $paramsStr = "";
        foreach ($inParams as $param) {
            $paramPrefix = Utils::paramTypeMap($param['type']);
            $paramSuffix = "$".$param['valueName'];
            $paramsStr .= !empty($paramPrefix)?$paramPrefix." ".$paramSuffix.",":$paramSuffix.",";

        }

        foreach ($outParams as $param) {
            $paramPrefix = Utils::paramTypeMap($param['type']);
            $paramSuffix = "&$".$param['valueName'];
            $paramsStr .= !empty($paramPrefix)?$paramPrefix." ".$paramSuffix.",":$paramSuffix.",";
        }

        $paramsStr = trim($paramsStr,",");
        $paramsStr .= ") {".$this->returnSymbol;

        $funcHeader = $this->tabSymbol."public function ".$funcName."(".$paramsStr;

        return $funcHeader;
    }

    /**
     * @param $funcName
     * @param $inParams
     * @param $outParams
     * 生成函数的包体
     */
    public function generateFuncBody($inParams,$outParams,$returnInfo) {
        $bodyPrefix = $this->doubleTab."\$this->_tarsAssistant->setRequest(\$this->_servantName,__FUNCTION__,\$this->_ip,\$this->_port);".$this->doubleReturn.
            $this->doubleTab."try {".$this->returnSymbol;

        $bodySuffix = $this->doubleTab."catch (\\Exception \$e) {".$this->returnSymbol.
            $this->tripleTab."throw new \phptars\TarsException(\$e->getMessage(),\$e->getCode());".$this->returnSymbol.
            $this->doubleTab."}".$this->returnSymbol;


        $bodyMiddle = "";


        $commonPrefix = "\$this->_tarsAssistant->";
        foreach ($inParams as $param) {
            $type = $param['type'];

            $packMethod = Utils::getPackMethods($type);
            $valueName = $param['valueName'];

            // 判断如果是vector需要特别的处理
            if(Utils::isVector($type)) {
                $vecFill = $this->tripleTab."\$".$valueName."_vec = ".$this->getExtType($param['wholeType'],$valueName).";".$this->returnSymbol.
                    $this->tripleTab."foreach(\$".$valueName." as "."\$single".$valueName.") {".$this->returnSymbol.
                    $this->quardupleTab."\$".$valueName."_vec->pushBack(\$single".$valueName.");".$this->returnSymbol.
                    $this->tripleTab."}".$this->returnSymbol;
                $bodyMiddle .= $vecFill;
                $bodyMiddle .= $this->tripleTab.$commonPrefix.$packMethod."(\"".$valueName."\",\$".$valueName."_vec);".$this->returnSymbol;

            }

            // 判断如果是map需要特别的处理
            else if(Utils::isMap($type)) {

                $mapFill = $this->tripleTab."\$".$valueName."_map = ".$this->getExtType($param['wholeType'],$valueName).";".$this->returnSymbol.
                    $this->tripleTab."foreach(\$".$valueName." as "."\$key => \$value) {".$this->returnSymbol.
                    $this->quardupleTab."\$".$valueName."_map->pushBack([\$key => \$value]);".$this->returnSymbol.
                    $this->tripleTab."}".$this->returnSymbol;
                $bodyMiddle .= $mapFill;
                $bodyMiddle .= $this->tripleTab.$commonPrefix.$packMethod."(\"".$valueName."\",\$".$valueName."_map);".$this->returnSymbol;
            }
            // 针对struct,需要额外的use过程
            else if(Utils::isStruct($type,$this->preStructs)) {
                if(!in_array($type,$this->useStructs)) {
                    $this->extraUse .= "use ".$this->namespaceName."\\classes\\".$param['type'].";".$this->returnSymbol;
                    $this->useStructs[] = $param['type'];
                }
                $bodyMiddle .= $this->tripleTab.$commonPrefix.$packMethod."(\"".$valueName."\",\$".$valueName.");".$this->returnSymbol;
            }

            else    $bodyMiddle .= $this->tripleTab.$commonPrefix.$packMethod."(\"".$valueName."\",\$".$valueName.");".$this->returnSymbol;

        }

        $bodyMiddle .= $this->doubleReturn;


        // 判断是否是异步的,进行分别的处理
        $bodyMiddle .= $this->tripleTab."\$this->_tarsAssistant->sendAndReceive();".$this->doubleReturn;


        foreach ($outParams as $param) {

            $type = $param['type'];

            $unpackMethods = Utils::getUnpackMethods($type);
            $name = $param['valueName'];

            if(Utils::isBasicType($type)) {
                $bodyMiddle .= $this->tripleTab."\$$name = ".$commonPrefix.$unpackMethods."(\"".$name."\");".$this->returnSymbol;
            }
            else {
                // 判断如果是vector需要特别的处理
                if(Utils::isVector($type) || Utils::isMap($type)) {

                    $bodyMiddle .= $this->tripleTab."\$$name = ".$commonPrefix.$unpackMethods."(\"".$name."\",".$this->getExtType($param['wholeType'],$name).");".$this->returnSymbol;
                }
                // 如果是struct
                else if(Utils::isStruct($type,$this->preStructs)) {
                    $bodyMiddle .= $this->tripleTab."\$ret = ".$commonPrefix.$unpackMethods."(\"".$name."\",\$$name);".$this->returnSymbol;

                    if (!in_array($type, $this->useStructs)) {
                        $this->extraUse .= "use " . $this->namespaceName . "\\classes\\" . $param['type'] . ";" . $this->returnSymbol;
                        $this->useStructs[] = $param['type'];
                    }
                }
            }
        }

        // 还要尝试去获取一下接口的返回码哦
        $returnUnpack = Utils::getUnpackMethods($returnInfo['type']);
        $valueName = $returnInfo['valueName'];

        if($returnInfo['type'] !== 'void') {
            if(Utils::isVector($returnInfo['type']) || Utils::isMap($returnInfo['type'])) {
                $bodyMiddle .= $this->tripleTab."return \$this->_tarsAssistant->".$returnUnpack."(\"\","
                    .$this->getExtType($returnInfo['wholeType'],$valueName).");".$this->doubleReturn.
                    $this->doubleTab."}".$this->returnSymbol;

            }
            else if(Utils::isStruct($returnInfo['type'],$this->preStructs)) {
                $bodyMiddle .= $this->tripleTab."\$returnVal = new $valueName();".$this->returnSymbol;
                $bodyMiddle .= $this->tripleTab."\$this->_tarsAssistant->".$returnUnpack."(\"\",\$returnVal);".$this->returnSymbol;
                $bodyMiddle .= $this->tripleTab."return \$returnVal;".$this->doubleReturn.
                    $this->doubleTab."}".$this->returnSymbol;


                if (!in_array($returnInfo['type'], $this->useStructs)) {
                    $this->extraUse .= "use " . $this->namespaceName . "\\classes\\" . $returnInfo['type'] . ";" . $this->returnSymbol;
                    $this->useStructs[] = $returnInfo['type'];
                }
            }
            else {
                $bodyMiddle .= $this->tripleTab."return \$this->_tarsAssistant->".$returnUnpack."(\"\");".$this->doubleReturn.
                    $this->doubleTab."}".$this->returnSymbol;

            }
        }
        else {
            $bodyMiddle .= $this->doubleTab."}".$this->returnSymbol;
        }

        $bodyStr = $bodyPrefix.$bodyMiddle.$bodySuffix;

        return [
            'syn' => $bodyStr
        ];
    }

}

class StructParser {

    public $uniqueName;
    public $moduleName;
    public $structName;
    public $state;

    // 这个结构体,可能会引用的部分,包括其他的结构体、枚举类型、常量
    public $preStructs;
    public $preEnums;
    public $preNamespaceEnums;
    public $preNamespaceStructs;

    public $returnSymbol = "\n";
    public $doubleReturn = "\n\n";
    public $tabSymbol = "\t";
    public $doubleTab = "\t\t";
    public $tripleTab = "\t\t\t";
    public $quardupleTab = "\t\t\t\t";


    public $extraContructs = '';
    public $extraExtInit = '';

    public $consts='';
    public $variables = '';
    public $fields = '';

    public $namespaceName;

    public function __construct($fp,$line,$uniqueName,$moduleName,$structName,$preStructs,$preEnums,$namespaceName,
                                $preNamespaceEnums,$preNamespaceStructs)
    {
        $this->fp = $fp;
        $this->uniqueName = $uniqueName;
        $this->moduleName = $moduleName;
        $this->preStructs = $preStructs;
        $this->preEnums = $preEnums;
        $this->structName = $structName;
        $this->namespaceName = $namespaceName;

        $this->consts = '';
        $this->variables = '';
        $this->fields = '';

        $this->preNamespaceEnums = $preNamespaceEnums;
        $this->preNamespaceStructs = $preNamespaceStructs;
    }




    public function parse() {

        while ($this->state != 'end') {
            $this->structBodyParseLine();
        }

        // 先把积累下来的三个部分处理掉
        $structClassStr = $this->getStructClassHeader("\\classes").
            "class ".$this->structName." extends \TARS_Struct {".$this->returnSymbol;

        $structClassStr  .= $this->consts.$this->doubleReturn;
        $structClassStr .= $this->variables.$this->doubleReturn;
        $fieldsPrefix = $this->tabSymbol."protected static \$fields = array(".$this->returnSymbol;
        $fieldsSuffix = $this->tabSymbol.");".$this->doubleReturn;

        $structClassStr .= $fieldsPrefix;
        $structClassStr .= $this->fields;
        $structClassStr .= $fieldsSuffix;

        // 处理最后一行

        $construct = $this->tabSymbol."public function __construct() {".$this->returnSymbol.
            $this->doubleTab."parent::__construct('".$this->uniqueName."_".$this->structName."', self::\$fields);".$this->returnSymbol
            .$this->extraContructs
            .$this->extraExtInit
            .$this->tabSymbol."}".$this->returnSymbol;

        $structClassStr .= $construct."}".$this->returnSymbol;

        return $structClassStr;
    }

    /**
     * @param $startChar
     * @param $lineString
     * @return string
     * 专门处理注释
     */
    public function copyAnnotation($startChar,$lineString) {
        $lineString .= $startChar;
        // 再读入一个字符
        $nextChar = fgetc($this->fp);
        // 第一种
        if($nextChar == '/') {
            $lineString .= $nextChar;
            while (1) {
                $tmpChar = fgetc($this->fp);
                if(Utils::isReturn($tmpChar)) {

                    $this->state = 'lineEnd';
                    break;
                }
                $lineString .= $tmpChar;
            }
            return $lineString;
        }
        else if($nextChar == '*') {
            $lineString .= $nextChar;
            while (1) {
                $tmpChar =fgetc($this->fp);
                $lineString .= $tmpChar;

                if($tmpChar === false) {
                    Utils::abnormalExit('error','注释换行错误,请检查');
                }
                else if(Utils::isReturn($tmpChar)) {

                }
                else if(($tmpChar) === '*') {
                    $nextnextChar = fgetc($this->fp);
                    if($nextnextChar == '/') {
                        $lineString .= $nextnextChar;
                        return $lineString;
                    }
                    else{
                        $pos = ftell($this->fp);
                        fseek($this->fp,$pos - 1);
                    }
                }
            }
        }
        // 注释不正常
        else {
            Utils::abnormalExit('error','注释换行错误,请检查');
        }
    }

    /**
     * @param $fp
     * @param $line
     * 这里必须要引入状态机了
     */
    public function structBodyParseLine() {

        $validLine = false;

        $this->state = 'init';

        $lineString = '';
        $word = '';

        $mapVectorState = false;
        while (1) {
            $char =fgetc($this->fp);

            if($this->state == 'init') {
                // 有可能是换行
                if($char == '{' || Utils::isSpace($char) || $char == '\r'
                    || $char == '\x0B' || $char == '\0') {
                    continue;
                }
                else if($char == "\n") {
                    break;
                }
                // 遇到了注释会用贪婪算法全部处理完,同时填充到struct的类里面去
                else if($char == '/') {
                    $this->copyAnnotation($char,$lineString);
                    break;
                }
                else if(Utils::inIdentifier($char)) {
                    $this->state = 'identifier';
                    $word .= $char;
                }
                // 终止条件之1,宣告struct结束
                else if($char == '}') {
                    // 需要贪心的读到"\n"为止
                    while(($lastChar=fgetc($this->fp)) != "\n") {
                        continue;
                    }
                    $this->state = 'end';
                    break;
                }
                else if($char == '=') {
                    //遇到等号,可以贪婪的向后,直到遇到;或者换行符
                    if(!empty($word))
                        $valueName = $word;
                    $moreChar = fgetc($this->fp);

                    $defaultValue = '';

                    while($moreChar != '\n' && $moreChar != ';' && $moreChar != '}') {
                        $defaultValue .= $moreChar;

                        $moreChar = fgetc($this->fp);
                    }
                    //if(empty($defaultValue)) {
                    //    Utils::abnormalExit('error','结构体'.$this->structName.'内默认值格式错误,请更正tars');
                    //}

                    if($moreChar == '}'){
                        // 需要贪心的读到"\n"为止
                        while(($lastChar=fgetc($this->fp)) != "\n") {
                            continue;
                        }
                        $this->state = 'end';
                    }
                    else $this->state = 'init';

                }
                else {
                    //echo "char:".var_export($char,true);
                    //Utils::abnormalExit('error','结构体'.$this->structName.'内格式错误,请更正tars');
                    continue;
                }
            }
            else if($this->state == 'identifier') {
                $validLine = true;
                // 如果遇到了space,需要检查是不是在map或vector的类型中,如果当前积累的word并不合法
                // 并且又不是处在vector或map的前置状态下的话,那么就是出错了
                if(Utils::isSpace($char)) {
                    if(Utils::isTag($word)) {
                        $tag = $word;
                        $this->state = 'init';
                        $word = '';
                    }
                    else if(Utils::isRequireType($word)) {
                        $requireType = $word;
                        $this->state = 'init';
                        $word = '';
                    }
                    else if(Utils::isBasicType($word)) {
                        $type = $word;
                        $this->state = 'init';
                        $word = '';
                    }
                    else if(Utils::isStruct($word,$this->preStructs)) {
                        $type = $word;
                        $this->state = 'init';
                        $word = '';
                    }
                    else if(Utils::isEnum($word,$this->preEnums)) {
                        $type = 'unsigned byte';
                        $this->state = 'init';
                        $word = '';
                    }
                    // 增加对namespace的支持
                    else if(in_array($word,$this->preNamespaceStructs)) {
                        $type = explode("::",$word);
                        $type = $type[1];
                        $this->state = 'init';
                        $word = '';
                    }
                    // 增加对namespace的支持
                    else if(in_array($word,$this->preNamespaceEnums)) {
                        $type = 'unsigned byte';
                        $this->state = 'init';
                        $word = '';
                    }
                    else {
                        // 读到了vector和map中间的空格,还没读完
                        if($mapVectorState) {
                            continue;
                        }
                        // 否则剩余的部分应该就是值和默认值
                        else {
                            if(!empty($word))
                                $valueName = $word;
                            $this->state = 'init';
                            $word = '';
                        }
                    }
                }
                // 标志着map和vector的开始,不等到'>'的结束不罢休
                // 这时候需要使用栈来push,然后一个个对应的pop,从而达到type的遍历
                else if($char == '<') {
                    // 贪婪的向后,直到找出所有的'>'
                    $type = $word;
                    // 还会有一个wholeType,表示完整的部分
                    $mapVectorStack = [];
                    $wholeType = $type;
                    $wholeType .= '<';
                    array_push($mapVectorStack,'<');
                    while(!empty($mapVectorStack)) {
                        $moreChar = fgetc($this->fp);
                        $wholeType .= $moreChar;
                        if($moreChar == '<') {
                            array_push($mapVectorStack,'<');
                        }
                        else if($moreChar == '>') {
                            array_pop($mapVectorStack);
                        }
                    }

                    $this->state = 'init';
                    $word = '';
                }
                else if($char == '=') {
                    //遇到等号,可以贪婪的向后,直到遇到;或者换行符
                    if(!empty($word))
                        $valueName = $word;
                    $moreChar = fgetc($this->fp);

                    $defaultValue = '';

                    while($moreChar != '\n' && $moreChar != ';' && $moreChar != '}') {
                        $defaultValue .= $moreChar;

                        $moreChar = fgetc($this->fp);
                    }
                    //if(empty($defaultValue)) {
                    //    Utils::abnormalExit('error','结构体'.$this->structName.'内默认值格式错误,请更正tars');
                    //}

                    if($moreChar == '}') {
                        // 需要贪心的读到"\n"为止
                        while(($lastChar=fgetc($this->fp)) != "\n") {
                            continue;
                        }
                        $this->state = 'end';
                    }
                    else $this->state = 'init';
                }
                else if ($char == ';') {
                    if(!empty($word)) {
                        $valueName = $word;
                    }
                    continue;
                }
                // 终止条件之2,同样宣告struct结束
                else if($char == '}') {
                    // 需要贪心的读到"\n"为止
                    while(($lastChar=fgetc($this->fp)) != "\n") {
                        continue;
                    }
                    $this->state = 'end';
                }
                else if($char == '/') {
                    $lineString = $this->copyAnnotation($char,$lineString);
                }
                else if($char == "\n"){
                    break;
                }
                else $word .= $char;
            }
            else if($this->state == 'lineEnd') {
                if($char == '}'){
                    // 需要贪心的读到"\n"为止
                    while(($lastChar=fgetc($this->fp)) != "\n") {
                        continue;
                    }
                    $this->state = 'end';
                }
                break;
            }
            else if($this->state == 'end') {
                break;
            }
        }

        if(!$validLine) {
            return;
        }

        // 完成了这一行的词法解析,需要输出如下的字段
//        echo "RAW tag:".$tag." requireType:".$requireType." type:".$type.
//            " valueName:".$valueName. " wholeType:".$wholeType.
//            " defaultValue:".$defaultValue." lineString:".$lineString."\n\n";

        if(!isset($tag) || empty($requireType) || empty($type) || empty($valueName)) {
            Utils::abnormalExit('error','结构体'.$this->structName.'内格式错误,请更正tars');
        }
        else if($type == 'map' && empty($wholeType)) {
            Utils::abnormalExit('error','结构体'.$this->structName.'内map格式错误,请更正tars');
        }
        else if($type == 'vector' && empty($wholeType)) {
            Utils::abnormalExit('error','结构体'.$this->structName.'内vector格式错误,请更正tars');
        }
        else {
            $this->writeStructLine($tag,$requireType,$type,$valueName,$wholeType,$defaultValue);
        }
    }

    /**
     * @param $wholeType
     * 通过完整的类型获取vector的扩展类型
     * vector<CateObj> => new \TARS_VECTOR(new CateObj())
     * vector<string> => new \TARS_VECTOR(\TARS::STRING)
     * vector<map<string,CateObj>> => new \TARS_VECTOR(new \TARS_MAP(\TARS_MAP,new CateObj()))
     */
    public function getExtType($wholeType,$valueName) {
        $state = 'init';
        $word = '';
        $extType = '';

        for($i = 0; $i < strlen($wholeType);$i++) {
            $char = $wholeType[$i];
            if($state == 'init') {
                // 如果遇到了空格
                if(Utils::isSpace($char)) {
                    continue;
                }
                // 回车是停止符号
                else if(Utils::inIdentifier($char)) {
                    $state = 'indentifier';
                    $word .= $char;
                }
                else if(Utils::isReturn($char)) {
                    break;
                }
                else if($char == '>') {
                    $extType .= ")";
                    continue;
                }
            }
            else if($state == 'indentifier') {
                if($char == '<') {
                    // 替换word,替换< 恢复初始状态
                    $tmp = $this->VecMapReplace($word);
                    $extType .= $tmp;
                    $extType .= "(";
                    $word = '';
                    $state = 'init';
                }
                else if($char == '>') {
                    // 替换word,替换> 恢复初始状态
                    // 替换word,替换< 恢复初始状态
                    $tmp = $this->VecMapReplace($word);
                    $extType .= $tmp;
                    $extType .= ")";
                    $word = '';
                    $state = 'init';
                }
                else if($char == ',') {
                    // 替换word,替换, 恢复初始状态
                    // 替换word,替换< 恢复初始状态
                    $tmp = $this->VecMapReplace($word);
                    $extType .= $tmp;
                    $extType .= ",";
                    $word = '';
                    $state = 'init';
                }
                else {
                    $word .= $char;
                    continue;
                }
            }
        }
        return $extType;
    }


    public function VecMapReplace($word) {
        $word = trim($word);
        // 遍历所有的类型
        foreach (Utils::$wholeTypeMap as $key => $value) {
            $word = str_replace($key,$value,$word);
        }

        if(Utils::isStruct($word,$this->preStructs)) {
            $word = "new " . $word . "()";
        }

        return $word;
    }

    /**
     * @param $tag
     * @param $requireType
     * @param $type
     * @param $name
     * @param $wholeType
     * @param $defaultValue
     */
    public function writeStructLine($tag,$requireType,$type,$valueName,$wholeType,$defaultValue) {
        if($requireType === 'require')
            $requireFlag = 'true';
        else $requireFlag = 'false';

        $this->consts .= $this->tabSymbol."const ".strtoupper($valueName)." = ".$tag.";".$this->returnSymbol;
        if(!empty($defaultValue)) {
            $this->variables .= $this->tabSymbol."public $".$valueName."=".$defaultValue.";"." ".$this->returnSymbol;
        }
        else $this->variables .= $this->tabSymbol."public $".$valueName.";"." ".$this->returnSymbol;

        // 基本类型,直接替换
        if(Utils::isBasicType($type)) {
            $this->fields .= $this->doubleTab."self::".strtoupper($valueName)." => array(".$this->returnSymbol.
                $this->tripleTab."'name'=>'".$valueName."',".$this->returnSymbol.
                $this->tripleTab."'required'=>".$requireFlag.",".$this->returnSymbol.
                $this->tripleTab."'type'=>".Utils::getRealType($type).",".$this->returnSymbol.
                $this->tripleTab."),".$this->returnSymbol;

        }
        else if(Utils::isStruct($type,$this->preStructs)) {
            $this->fields .= $this->doubleTab."self::".strtoupper($valueName)." => array(".$this->returnSymbol.
                $this->tripleTab."'name'=>'".$valueName."',".$this->returnSymbol.
                $this->tripleTab."'required'=>".$requireFlag.",".$this->returnSymbol.
                $this->tripleTab."'type'=>".Utils::getRealType($type).",".$this->returnSymbol.
                $this->tripleTab."),".$this->returnSymbol;
            $this->extraContructs .= $this->doubleTab."\$this->$valueName = new $type();".$this->returnSymbol;

        }
        else if(Utils::isVector($type) || Utils::isMap($type)) {

            $extType = $this->getExtType($wholeType,$valueName);
            $this->extraExtInit .= $this->doubleTab."\$this->".$valueName." = ".$extType.";".$this->returnSymbol;

            $this->fields .= $this->doubleTab."self::".strtoupper($valueName)." => array(".$this->returnSymbol.
                $this->tripleTab."'name'=>'".$valueName."',".$this->returnSymbol.
                $this->tripleTab."'required'=>".$requireFlag.",".$this->returnSymbol.
                $this->tripleTab."'type'=>".Utils::getRealType($type).",".$this->returnSymbol.
                $this->tripleTab."),".$this->returnSymbol;

        }
        else {
            Utils::abnormalExit('error','结构体struct'.$this->structName.'内类型有误,请更正tars');
        }
    }

    public function getStructClassHeader($prefix="getFileHea") {
        return "<?php\n\nnamespace ".$this->namespaceName.$prefix.";".
        $this->doubleReturn;
    }
}


$fromFile = $argv[1];
$servantName = $argv[2];

if(empty($servantName)) {
    echo "Please input serantName!!";
    exit;
}

$fileConverter = new FileConverter($fromFile,$servantName);
$fileConverter->moduleScan();

$fileConverter->moduleParse();












