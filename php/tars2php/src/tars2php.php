<?php
/**
 * 说明：
 *    1. } 要单独放在一行，如}后面有;号，要跟}放在一行
 *    2. 每行只能字义一个字段
 *    3. struct不能嵌套定义，如用到别的struct, 把对应的struct 拿出来定义即可.
 **/
$configFile = $argv[1];
$config = require_once $configFile;

$fileConverter = new FileConverter($config);

$fileConverter->moduleScan();

$fileConverter->moduleParse();

class Utils
{
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
        'vector' => 'new \TARS_Vector',
        'map' => 'new \TARS_Map',
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
        'enum' => '\TARS::UINT8', // 应该不会出现
        'struct' => '\TARS::STRUCT', // 应该不会出现
    );

    public static function getPackMethods($type)
    {
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
            'Vector' => 'putVector',
        ];

        if (isset($packMethods[$type])) {
            return $packMethods[$type];
        } else {
            return 'putStruct';
        }
    }

    public static function getUnpackMethods($type)
    {
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
            'Vector' => 'getVector',
        ];

        if (isset($unpackMethods[strtolower($type)])) {
            return $unpackMethods[strtolower($type)];
        } else {
            return 'getStruct';
        }
    }

    /**
     * @param $char
     *
     * @return int
     *             判断是不是tag
     */
    public static function isTag($word)
    {
        if (!is_numeric($word)) {
            return false;
        } else {
            return true;
        }
    }

    /**
     * @param $word
     *
     * @return bool
     *              判断收集到的word是不是
     */
    public static function isRequireType($word)
    {
        return in_array(strtolower($word), ['require', 'optional']);
    }

    public static function isBasicType($word)
    {
        $basicTypes = [
            'bool', 'byte', 'char', 'unsigned byte', 'unsigned char', 'short', 'unsigned short',
            'int', 'unsigned int', 'long', 'float', 'double', 'string', 'void',
        ];

        return in_array(strtolower($word), $basicTypes);
    }

    public static function isEnum($word, $preEnums)
    {
        return in_array($word, $preEnums);
    }

    public static function isMap($word)
    {
        return strtolower($word) == 'map';
    }

    public static function isStruct($word, $preStructs)
    {
        return in_array($word, $preStructs);
    }

    public static function isVector($word)
    {
        return strtolower($word) == 'vector';
    }

    public static function isSpace($char)
    {
        if ($char == ' ' || $char == "\t") {
            return true;
        } else {
            return false;
        }
    }

    public static function paramTypeMap($paramType)
    {
        if (self::isBasicType($paramType) || self::isMap($paramType) || self::isVector($paramType)) {
            return '';
        } else {
            return $paramType;
        }
    }

    public static function getRealType($type)
    {
        if (isset(self::$typeMap[strtolower($type)])) {
            return self::$typeMap[strtolower($type)];
        } else {
            return '\TARS::STRUCT';
        }
    }

    public static function inIdentifier($char)
    {
        return ($char >= 'a' & $char <= 'z') |
        ($char >= 'A' & $char <= 'Z') |
        ($char >= '0' & $char <= '9') |
        ($char == '_');
    }

    public static function abnormalExit($level, $msg)
    {
        echo "[$level]$msg"."\n";
        exit;
    }

    public static function pregMatchByName($name, $line)
    {
        // 处理第一行,正则匹配出classname
        $Tokens = preg_split("/$name/", $line);

        $mathName = $Tokens[1];
        $mathName = trim($mathName, " \r\0\x0B\t\n{");

        preg_match('/[a-zA-Z][0-9a-zA-Z]/', $mathName, $matches);
        if (empty($matches)) {
            //Utils::abnormalExit('error',$name.'名称有误'.$line);
        }

        return $mathName;
    }

    public static function isReturn($char)
    {
        if ($char == "\n" || $char == '\r' || bin2hex($char) == '0a' || bin2hex($char) == '0b' ||
            bin2hex($char) == '0c' || bin2hex($char) == '0d') {
            return true;
        } else {
            return false;
        }
    }
}

class FileConverter
{
    public $moduleName;
    public $uniqueName;
    public $interfaceName;
    public $fromFile;
    public $outputDir;

    public $appName;
    public $serverName;
    public $objName;
    public $servantName;

    public $namespaceName;
    public $namespacePrefix;

    public $preStructs = [];
    public $preEnums = [];
    public $preConsts = [];
    public $preNamespaceStructs = [];
    public $preNamespaceEnums = [];

    public function __construct($config)
    {
        $this->fromFile = $config['tarsFiles'][0];
        if (empty($config['appName']) || empty($config['serverName']) || empty($config['objName'])) {
            Utils::abnormalExit('error', 'appName or serverName or objName empty!');
        }
        $this->servantName = $config['appName'].'.'.$config['serverName'].'.'.$config['objName'];

        $this->appName = $config['appName'];
        $this->serverName = $config['serverName'];
        $this->objName = $config['objName'];

        $this->outputDir = empty($config['dstPath']) ? './' : $config['dstPath'].'/';

        $pos = strrpos($this->fromFile, '/', -1);
        $inputDir = substr($this->fromFile, 0, $pos);
        $this->inputDir = $inputDir;

        $this->namespacePrefix = $config['namespacePrefix'];
        $this->withServant = $config['withServant'];

        $this->initDir();
    }

    /**
     * 首先需要初始化一些文件目录.
     *
     * @return [type] [description]
     */
    public function initDir()
    {
        if (strtolower(substr(php_uname('a'), 0, 3)) === 'win') {
            exec('mkdir '.$this->outputDir.$this->appName);
            exec('mkdir '.$this->outputDir.$this->appName.'\\'.$this->serverName);
            exec('DEL '.$this->outputDir.$this->appName.'\\'.$this->serverName.'\\'.$this->objName.'\\*.*');
            exec('mkdir '.$this->outputDir.$this->appName.'\\'.$this->serverName.'\\'.$this->objName);

            $this->moduleName = $this->appName.'\\'.$this->serverName.'\\'.$this->objName;

            exec('mkdir '.$this->outputDir.$this->moduleName.'\\classes');
            exec('mkdir '.$this->outputDir.$this->moduleName.'\\tars');
            exec('copy '.$this->fromFile.' '.$this->outputDir.$this->moduleName.'\\tars');
        } else {
            exec('mkdir '.$this->outputDir.$this->appName);
            exec('mkdir '.$this->outputDir.$this->appName.'/'.$this->serverName);
            exec('rm -rf '.$this->outputDir.$this->appName.'/'.$this->serverName.'/'.$this->objName);
            exec('mkdir '.$this->outputDir.$this->appName.'/'.$this->serverName.'/'.$this->objName);

            $this->moduleName = $this->appName.'/'.$this->serverName.'/'.$this->objName;

            exec('mkdir '.$this->outputDir.$this->moduleName.'/classes');
            exec('mkdir '.$this->outputDir.$this->moduleName.'/tars');
            exec('cp '.$this->fromFile.' '.$this->outputDir.$this->moduleName.'/tars');
        }

        $this->namespaceName = empty($this->namespacePrefix) ? $this->appName.'\\'.$this->serverName.'\\'.$this->objName
            : $this->namespacePrefix.'\\'.$this->appName.'\\'.$this->serverName.'\\'.$this->objName;

        $this->uniqueName = $this->appName.'_'.$this->serverName.'_'.$this->objName;
    }

    public function usage()
    {
        echo 'php tars2php.php tars.proto.php';
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
            $moduleFlag = strpos($line, 'module');
            if ($moduleFlag !== false) {
                $name = Utils::pregMatchByName('module', $line);
                $currentModule = $name;
            }

            // 判断是否有include
            $includeFlag = strpos($line, '#include');
            if ($includeFlag !== false) {
                // 找出tars对应的文件名
                $tokens = preg_split('/#include/', $line);
                $includeFile = trim($tokens[1], "\" \r\n");

                if (strtolower(substr(php_uname('a'), 0, 3)) === 'win') {
                    exec('copy '.$includeFile.' '.$this->moduleName.'\\tars');
                } else {
                    exec('cp '.$includeFile.' '.$this->moduleName.'/tars');
                }

                $includeParser = new IncludeParser();
                $includeParser->includeScan($includeFile, $this->preEnums, $this->preStructs,
                    $this->preNamespaceEnums, $this->preNamespaceStructs);
            }

            // 如果空行，或者是注释，就直接略过
            if (!$line || trim($line) == '' || trim($line)[0] === '/' || trim($line)[0] === '*' || trim($line) === '{') {
                continue;
            }

            // 正则匹配,发现是在enum中
            $enumFlag = strpos($line, 'enum');
            if ($enumFlag !== false) {
                $name = Utils::pregMatchByName('enum', $line);
                if (!empty($name)) {
                    $this->preEnums[] = $name;

                    // 增加命名空间以备不时之需
                    if (!empty($currentModule)) {
                        $this->preNamespaceEnums[] = $currentModule.'::'.$name;
                    }

                    while (($lastChar = fgetc($fp)) != '}') {
                        continue;
                    }
                }
            }

            // 正则匹配，发现是在结构体中
            $structFlag = strpos($line, 'struct');
            // 一旦发现了struct，那么持续读到结束为止
            if ($structFlag !== false) {
                $name = Utils::pregMatchByName('struct', $line);

                if (!empty($name)) {
                    $this->preStructs[] = $name;
                    // 增加命名空间以备不时之需
                    if (!empty($currentModule)) {
                        $this->preNamespaceStructs[] = $currentModule.'::'.$name;
                    }
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
            $includeFlag = strpos($line, '#include');
            if ($includeFlag !== false) {
                // 找出tars对应的文件名
                $tokens = preg_split('/#include/', $line);
                $includeFile = trim($tokens[1], "\" \r\n");
                $includeParser = new IncludeParser();
                $includeParser->includeParse($includeFile, $this->preEnums, $this->preStructs, $this->uniqueName,
                    $this->moduleName, $this->namespaceName, $this->servantName, $this->preNamespaceEnums, $this->preNamespaceStructs,
                    $this->outputDir);
            }

            // 如果空行，或者是注释，就直接略过
            if (!$line || trim($line) == '' || trim($line)[0] === '/' || trim($line)[0] === '*') {
                continue;
            }

            // 正则匹配,发现是在enum中
            $enumFlag = strpos($line, 'enum');
            if ($enumFlag !== false) {
                // 处理第一行,正则匹配出classname
                $enumTokens = preg_split('/enum/', $line);

                $enumName = $enumTokens[1];
                $enumName = trim($enumName, " \r\0\x0B\t\n{");

                // 判断是否是合法的structName
                preg_match('/[a-zA-Z][0-9a-zA-Z]/', $enumName, $matches);
                if (empty($matches)) {
                    Utils::abnormalExit('error', 'Enum名称有误');
                }

                $this->preEnums[] = $enumName;
                while (($lastChar = fgetc($fp)) != '}') {
                    continue;
                }
            }

            // 正则匹配,发现是在consts中
            $constFlag = strpos($line, 'const');
            if ($constFlag !== false) {
                // 直接进行正则匹配
                Utils::abnormalExit('warning', 'const is not supported, please make sure you deal with them yourself in this version!');
            }

            // 正则匹配，发现是在结构体中
            $structFlag = strpos($line, 'struct');
            // 一旦发现了struct，那么持续读到结束为止
            if ($structFlag !== false) {
                $name = Utils::pregMatchByName('struct', $line);

                $structParser = new StructParser($fp, $line, $this->uniqueName, $this->moduleName, $name, $this->preStructs,
                    $this->preEnums, $this->namespaceName, $this->preNamespaceEnums, $this->preNamespaceStructs);
                $structClassStr = $structParser->parse();
                file_put_contents($this->outputDir.$this->moduleName.'/classes/'.$name.'.php', $structClassStr);
            }

            // 正则匹配，发现是在interface中
            $interfaceFlag = strpos(strtolower($line), 'interface');
            // 一旦发现了struct，那么持续读到结束为止
            if ($interfaceFlag !== false) {
                $name = Utils::pregMatchByName('interface', $line);
                $interfaceName = $name.'Servant';

                // 需要区分一下生成server还是client的代码
                if ($this->withServant) {
                    $servantParser = new ServantParser($fp, $line, $this->namespaceName, $this->moduleName,
                        $interfaceName, $this->preStructs,
                        $this->preEnums, $this->servantName, $this->preNamespaceEnums, $this->preNamespaceStructs);
                    $servant = $servantParser->parse();
                    file_put_contents($this->outputDir.$this->moduleName.'/'.$interfaceName.'.php', $servant);
                } else {
                    $interfaceParser = new InterfaceParser($fp, $line, $this->namespaceName, $this->moduleName,
                        $interfaceName, $this->preStructs,
                        $this->preEnums, $this->servantName, $this->preNamespaceEnums, $this->preNamespaceStructs);
                    $interfaces = $interfaceParser->parse();

                    // 需要区分同步和异步的两种方式
                    file_put_contents($this->outputDir.$this->moduleName.'/'.$interfaceName.'.php', $interfaces['syn']);
                }
            }
        }
    }
}

class IncludeParser
{
    public function includeScan($includeFile, &$preEnums, &$preStructs,
                                &$preNamespaceEnums, &$preNamespaceStructs)
    {
        $fp = fopen($includeFile, 'r');
        if (!$fp) {
            echo 'Include file not exit, please check';
            exit;
        }
        while (($line = fgets($fp, 1024)) !== false) {
            // 如果空行，或者是注释，就直接略过
            if (!$line || trim($line) == '' || trim($line)[0] === '/' || trim($line)[0] === '*') {
                continue;
            }

            // 判断是否有module
            $moduleFlag = strpos($line, 'module');
            if ($moduleFlag !== false) {
                $name = Utils::pregMatchByName('module', $line);
                $currentModule = $name;
            }

            // 正则匹配,发现是在enum中
            $enumFlag = strpos($line, 'enum');
            if ($enumFlag !== false) {
                $name = Utils::pregMatchByName('enum', $line);
                $preEnums[] = $name;
                if (!empty($currentModule)) {
                    $preNamespaceEnums[] = $currentModule.'::'.$name;
                }
                while (($lastChar = fgetc($fp)) != '}') {
                    continue;
                }
            }

            // 正则匹配，发现是在结构体中
            $structFlag = strpos($line, 'struct');
            // 一旦发现了struct，那么持续读到结束为止
            if ($structFlag !== false) {
                $name = Utils::pregMatchByName('struct', $line);

                $preStructs[] = $name;
                if (!empty($currentModule)) {
                    $preNamespaceStructs[] = $currentModule.'::'.$name;
                }
            }
        }
    }

    public function includeParse($includeFile, &$preEnums, &$preStructs, $uniqueName, $moduleName, $namespaceName, $servantName,
                                 &$preNamespaceEnums, &$preNamespaceStructs, $outputDir)
    {
        $fp = fopen($includeFile, 'r');
        if (!$fp) {
            echo 'Include file not exit, please check';
            exit;
        }
        while (($line = fgets($fp, 1024)) !== false) {
            // 如果空行，或者是注释，就直接略过
            if (!$line || trim($line) == '' || trim($line)[0] === '/' || trim($line)[0] === '*') {
                continue;
            }

            // 正则匹配,发现是在consts中
            $constFlag = strpos($line, 'const');
            if ($constFlag !== false) {
                // 直接进行正则匹配
                echo 'CONST is not supported, please make sure you deal with them yourself in this version!';
            }

            // 正则匹配，发现是在结构体中
            $structFlag = strpos($line, 'struct');
            // 一旦发现了struct，那么持续读到结束为止
            if ($structFlag !== false) {
                $name = Utils::pregMatchByName('struct', $line);

                $structParser = new StructParser($fp, $line, $uniqueName, $moduleName, $name, $preStructs,
                    $preEnums, $namespaceName, $preNamespaceEnums, $preNamespaceStructs);
                $structClassStr = $structParser->parse();
                file_put_contents($outputDir.$moduleName.'/classes/'.$name.'.php', $structClassStr);
            }

            // 正则匹配，发现是在interface中
            $interfaceFlag = strpos(strtolower($line), 'interface');
            // 一旦发现了struct，那么持续读到结束为止
            if ($interfaceFlag !== false) {
                $name = Utils::pregMatchByName('interface', $line);

                if (in_array($name, $preStructs)) {
                    $name .= 'Servant';
                }

                $interfaceParser = new InterfaceParser($fp, $line, $namespaceName, $moduleName,
                    $name, $preStructs,
                    $preEnums, $servantName, $preNamespaceEnums, $preNamespaceStructs);
                $interfaces = $interfaceParser->parse();

                // 需要区分同步和异步的两种方式
                file_put_contents($outputDir.$moduleName.'/'.$name.'.php', $interfaces['syn']);
            }
        }
    }
}

class InterfaceParser
{
    public $namespaceName;
    public $moduleName;
    public $interfaceName;
    public $asInterfaceName;

    public $state;

    // 这个结构体,可能会引用的部分,包括其他的结构体、枚举类型、常量
    public $useStructs = [];
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

    public $consts = '';
    public $variables = '';
    public $fields = '';

    public $funcSet = '';

    public $servantName;

    public function __construct($fp, $line, $namespaceName, $moduleName,
                                $interfaceName, $preStructs,
                                $preEnums, $servantName, $preNamespaceEnums, $preNamespaceStructs)
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

    public function copyAnnotation()
    {
        // 再读入一个字符
        $nextChar = fgetc($this->fp);
        // 第一种
        if ($nextChar == '/') {
            while (1) {
                $tmpChar = fgetc($this->fp);

                if ($tmpChar == "\n") {
                    $this->state = 'lineEnd';
                    break;
                }
            }

            return;
        } elseif ($nextChar == '*') {
            while (1) {
                $tmpChar = fgetc($this->fp);

                if ($tmpChar === false) {
                    Utils::abnormalExit('error', '注释换行错误,请检查'.$tmpChar);
                } elseif ($tmpChar === "\n") {
                } elseif (($tmpChar) === '*') {
                    $nextnextChar = fgetc($this->fp);
                    if ($nextnextChar == '/') {
                        return;
                    } else {
                        $pos = ftell($this->fp);
                        fseek($this->fp, $pos - 1);
                    }
                }
            }
        }
        // 注释不正常
        else {
            Utils::abnormalExit('error', '注释换行错误,请检查'.$nextChar);
        }
    }

    public function getFileHeader($prefix = '')
    {
        return "<?php\n\nnamespace ".$this->namespaceName.$prefix.';'.$this->doubleReturn.
        'use Tars\\client\\CommunicatorConfig;'.$this->returnSymbol.
        'use Tars\\client\\Communicator;'.$this->returnSymbol.
        'use Tars\\client\\RequestPacket;'.$this->returnSymbol.
        'use Tars\\client\\TUPAPIWrapper;'.$this->returnSymbol.
        $this->returnSymbol;
    }

    public function getInterfaceBasic()
    {
        return $this->tabSymbol.'protected $_communicator;'.$this->returnSymbol.
        $this->tabSymbol.'protected $_iVersion;'.$this->returnSymbol.
        $this->tabSymbol.'protected $_iTimeout;'.$this->returnSymbol.
        $this->tabSymbol."public \$_servantName = \"$this->servantName\";".$this->doubleReturn.
        $this->tabSymbol.'public function __construct(CommunicatorConfig $config) {'.$this->returnSymbol.

        $this->doubleTab.'try {'.$this->returnSymbol.
        $this->tripleTab.'$config->setServantName($this->_servantName);'.$this->returnSymbol.
        $this->tripleTab.'$this->_communicator = new Communicator($config);'.$this->returnSymbol.
        $this->tripleTab.'$this->_iVersion = $config->getIVersion();'.$this->returnSymbol.
        $this->tripleTab.'$this->_iTimeout = empty($config->getAsyncInvokeTimeout())?2:$config->getAsyncInvokeTimeout();'.$this->returnSymbol.
        $this->doubleTab.'} catch (\\Exception $e) {'.$this->returnSymbol.
        $this->tripleTab.'throw $e;'.$this->returnSymbol.
        $this->doubleTab.'}'.$this->returnSymbol.
        $this->tabSymbol.'}'.$this->doubleReturn;
    }

    public function parse()
    {
        while ($this->state != 'end') {
            $this->state = 'init';
            $this->InterfaceFuncParseLine();
        }

        $interfaceClass = $this->getFileHeader('').$this->extraUse.'class '.$this->interfaceName.' {'.$this->returnSymbol;

        $interfaceClass .= $this->getInterfaceBasic();

        $interfaceClass .= $this->funcSet;

        $interfaceClass .= '}'.$this->doubleReturn;

        return [
            'syn' => $interfaceClass,
        ];
    }

    /**
     * @param $fp
     * @param $line
     * 这里必须要引入状态机了
     */
    public function InterfaceFuncParseLine()
    {
        $line = '';
        $this->state = 'init';
        while (1) {
            if ($this->state == 'init') {
                $char = fgetc($this->fp);

                // 有可能是换行
                if ($char == '{' || Utils::isReturn($char)) {
                    continue;
                }
                // 遇到了注释会用贪婪算法全部处理完,同时填充到struct的类里面去
                elseif ($char == '/') {
                    $this->copyAnnotation();
                    break;
                } elseif (Utils::inIdentifier($char)) {
                    $this->state = 'identifier';
                    $line .= $char;
                }
                // 终止条件之1,宣告struct结束
                elseif ($char == '}') {
                    // 需要贪心的读到"\n"为止
                    while (($lastChar = fgetc($this->fp)) != "\n") {
                        continue;
                    }
                    $this->state = 'end';
                    break;
                }
            } elseif ($this->state == 'identifier') {
                $char = fgetc($this->fp);

                if ($char == '/') {
                    $this->copyAnnotation();
                } elseif ($char == ';') {
                    $line .= $char;
                    break;
                }
                // 终止条件之2,同样宣告interface结束
                elseif ($char == '}') {
                    // 需要贪心的读到"\n"为止
                    while (($lastChar = fgetc($this->fp)) != "\n") {
                        continue;
                    }
                    $this->state = 'end';
                } elseif (Utils::isReturn($char)) {
                    continue;
                } elseif ($char == ')') {
                    $line .= $char;
                    // 需要贪心的读到"\n"为止
                    while (($lastChar = fgetc($this->fp)) != "\n") {
                        continue;
                    }
                    $this->state = 'lineEnd';
                } else {
                    $line .= $char;
                }
            } elseif ($this->state == 'lineEnd') {
                $char = fgetc($this->fp);
                if ($char == '}') {
                    // 需要贪心的读到"\n"为止
                    while (($lastChar = fgetc($this->fp)) != "\n") {
                        continue;
                    }
                    $this->state = 'end';
                }
                break;
            } elseif ($this->state == 'end') {
                break;
            }
        }
        if (empty($line)) {
            return;
        }

        $line = trim($line);
        // 如果空行，或者是注释，或者是大括号就直接略过
        if (!$line || $line[0] === '/' || $line[0] === '*' || $line === '{') {
            return;
        }

        $endFlag = strpos($line, '};');
        if ($endFlag !== false) {
            $this->state = 'end';

            return;
        }

        $endFlag = strpos($line, '}');
        if ($endFlag !== false) {
            $this->state = 'end';

            return;
        }

        // 有必要先分成三个部分,返回类型、接口名、参数列表
        $tokens = preg_split('/\(/', $line, 2);
        $mix = $tokens[0];
        $rest = $tokens[1];

        $pices = preg_split('/\s+/', $mix);

        $funcName = $pices[count($pices) - 1];

        $returnType = implode('', array_slice($pices, 0, count($pices) - 1));

        $state = 'init';
        $word = '';

        $params = [];

        for ($i = 0; $i < strlen($rest); ++$i) {
            $char = $rest[$i];

            if ($state == 'init') {
                // 有可能是换行
                if ($char == '(' || Utils::isSpace($char)) {
                    continue;
                } elseif ($char == "\n") {
                    break;
                } elseif (Utils::inIdentifier($char)) {
                    $state = 'identifier';
                    $word .= $char;
                }
                // 终止条件之1,宣告interface结束
                elseif ($char == ')') {
                    break;
                } else {
                    Utils::abnormalExit('error', 'Interface:'.$this->interfaceName.'内格式错误,请更正tars');
                }
            } elseif ($state == 'identifier') {
                if ($char == ',') {
                    $params[] = $word;
                    $state = 'init';
                    $word = '';
                    continue;
                }
                // 标志着map和vector的开始,不等到'>'的结束不罢休
                // 这时候需要使用栈来push,然后一个个对应的pop,从而达到type的遍历
                elseif ($char == '<') {
                    $mapVectorStack = [];
                    $word .= $char;
                    array_push($mapVectorStack, '<');
                    while (!empty($mapVectorStack)) {
                        $moreChar = $rest[$i + 1];
                        $word .= $moreChar;
                        if ($moreChar == '<') {
                            array_push($mapVectorStack, '<');
                        } elseif ($moreChar == '>') {
                            array_pop($mapVectorStack);
                        }
                        ++$i;
                    }
                    continue;
                } elseif ($char == ')') {
                    $params[] = $word;
                    break;
                } elseif ($char == ';') {
                    continue;
                }
                // 终止条件之2,同样宣告struct结束
                elseif ($char == '}') {
                    $state = 'end';
                } elseif ($char == "\n") {
                    break;
                } else {
                    $word .= $char;
                }
            } elseif ($state == 'lineEnd') {
                break;
            } elseif ($state == 'end') {
                break;
            }
        }
        $this->writeInterfaceLine($returnType, $funcName, $params);
    }

    /**
     * @param $wholeType
     * 通过完整的类型获取vector的扩展类型
     * vector<CateObj> => new \TARS_VECTOR(new CateObj())
     * vector<string> => new \TARS_VECTOR(\TARS::STRING)
     * vector<map<string,CateObj>> => new \TARS_VECTOR(new \TARS_MAP(\TARS_MAP,new CateObj()))
     */
    public function getExtType($wholeType, $valueName)
    {
        $state = 'init';
        $word = '';
        $extType = '';

        for ($i = 0; $i < strlen($wholeType); ++$i) {
            $char = $wholeType[$i];
            if ($state == 'init') {
                // 如果遇到了空格
                if (Utils::isSpace($char)) {
                    continue;
                }
                // 回车是停止符号
                elseif (Utils::inIdentifier($char)) {
                    $state = 'indentifier';
                    $word .= $char;
                } elseif (Utils::isReturn($char)) {
                    break;
                } elseif ($char == '>') {
                    $extType .= ')';
                    continue;
                }
            } elseif ($state == 'indentifier') {
                if ($char == '<') {
                    // 替换word,替换< 恢复初始状态
                    $tmp = $this->VecMapReplace($word);
                    $extType .= $tmp;
                    $extType .= '(';
                    $word = '';
                    $state = 'init';
                } elseif ($char == '>') {
                    // 替换word,替换> 恢复初始状态
                    // 替换word,替换< 恢复初始状态
                    $tmp = $this->VecMapReplace($word);
                    $extType .= $tmp;
                    $extType .= ')';
                    $word = '';
                    $state = 'init';
                } elseif ($char == ',') {
                    // 替换word,替换, 恢复初始状态
                    // 替换word,替换< 恢复初始状态
                    $tmp = $this->VecMapReplace($word);
                    $extType .= $tmp;
                    $extType .= ',';
                    $word = '';
                    $state = 'init';
                } else {
                    $word .= $char;
                    continue;
                }
            }
        }

        return $extType;
    }

    public function VecMapReplace($word)
    {
        $word = trim($word);
        // 遍历所有的类型
        foreach (Utils::$wholeTypeMap as $key => $value) {
            if (Utils::isStruct($word, $this->preStructs)) {
                if (!in_array($word, $this->useStructs)) {
                    $this->extraUse .= 'use '.$this->namespaceName.'\\classes\\'.$word.';'.$this->returnSymbol;
                    $this->useStructs[] = $word;
                }

                $word = 'new '.$word.'()';
            } elseif (in_array($word, $this->preNamespaceStructs)) {
                $words = explode('::', $word);
                $word = $words[1];
                if (!in_array($word, $this->useStructs)) {
                    $this->extraUse .= 'use protocol\\'.$this->namespaceName.'\\classes\\'.$word.';'.$this->returnSymbol;
                    $this->useStructs[] = $word;
                }

                $word = 'new '.$word.'()';
                break;
            } else {
                $word = preg_replace('/\b'.$key.'\b/', $value, $word);
            }
        }

        return $word;
    }

    public function paramParser($params)
    {

        // 输入和输出的参数全部捋一遍
        $inParams = [];
        $outParams = [];
        foreach ($params as $param) {
            $state = 'init';
            $word = '';
            $wholeType = '';
            $paramType = 'in';
            $type = '';
            $mapVectorState = false;

            for ($i = 0; $i < strlen($param); ++$i) {
                $char = $param[$i];
                if ($state == 'init') {
                    // 有可能是换行
                    if (Utils::isSpace($char)) {
                        continue;
                    } elseif ($char == "\n") {
                        break;
                    } elseif (Utils::inIdentifier($char)) {
                        $state = 'identifier';
                        $word .= $char;
                    } else {
                        Utils::abnormalExit('error', 'Interface:'.$this->interfaceName.'内格式错误,请更正tars');
                    }
                } elseif ($state == 'identifier') {
                    // 如果遇到了space,需要检查是不是在map或vector的类型中,如果当前积累的word并不合法
                    // 并且又不是处在vector或map的前置状态下的话,那么就是出错了
                    if (Utils::isSpace($char)) {
                        if ($word == 'out') {
                            $paramType = $word;
                            $state = 'init';
                            $word = '';
                        } elseif (Utils::isBasicType($word)) {
                            $type = $word;
                            $state = 'init';
                            $word = '';
                        } elseif (Utils::isStruct($word, $this->preStructs)) {

                            // 同时要把它增加到本Interface的依赖中
                            if (!in_array($word, $this->useStructs)) {
                                $this->extraUse .= 'use '.$this->namespaceName.'\\classes\\'.$word.';'.$this->returnSymbol;
                                $this->useStructs[] = $word;
                            }

                            $type = $word;
                            $state = 'init';
                            $word = '';
                        } elseif (Utils::isEnum($word, $this->preEnums)) {
                            $type = 'unsigned byte';
                            $state = 'init';
                            $word = '';
                        } elseif (in_array($word, $this->preNamespaceStructs)) {
                            $word = explode('::', $word);
                            $word = $word[1];
                            // 同时要把它增加到本Interface的依赖中
                            if (!in_array($word, $this->useStructs)) {
                                $this->extraUse .= 'use '.$this->namespaceName.'\\classes\\'.$word.';'.$this->returnSymbol;
                                $this->useStructs[] = $word;
                            }

                            $type = $word;
                            $state = 'init';
                            $word = '';
                        } elseif (in_array($word, $this->preNamespaceEnums)) {
                            $type = 'unsigned byte';
                            $state = 'init';
                            $word = '';
                        } elseif (Utils::isMap($word)) {
                            $mapVectorState = true;
                        } elseif (Utils::isVector($word)) {
                            $mapVectorState = true;
                        } else {
                            // 读到了vector和map中间的空格,还没读完
                            if ($mapVectorState) {
                                continue;
                            }
                            // 否则剩余的部分应该就是值和默认值
                            else {
                                if (!empty($word)) {
                                    $valueName = $word;
                                }
                                $state = 'init';
                                $word = '';
                            }
                        }
                    }
                    // 标志着map和vector的开始,不等到'>'的结束不罢休
                    // 这时候需要使用栈来push,然后一个个对应的pop,从而达到type的遍历
                    elseif ($char == '<') {
                        // 贪婪的向后,直到找出所有的'>'
                        $type = $word;
                        // 还会有一个wholeType,表示完整的部分
                        $mapVectorStack = [];
                        $wholeType = $type;
                        $wholeType .= '<';
                        array_push($mapVectorStack, '<');
                        while (!empty($mapVectorStack)) {
                            $moreChar = $param[$i + 1];
                            $wholeType .= $moreChar;
                            if ($moreChar == '<') {
                                array_push($mapVectorStack, '<');
                            } elseif ($moreChar == '>') {
                                array_pop($mapVectorStack);
                            }
                            ++$i;
                        }

                        $state = 'init';
                        $word = '';
                    } else {
                        $word .= $char;
                    }
                }
            }

            if (!empty($word)) {
                $valueName = $word;
            }

            if ($paramType == 'in') {
                $inParams[] = [
                    'type' => $type,
                    'wholeType' => $wholeType,
                    'valueName' => $valueName,
                ];
            } else {
                $outParams[] = [
                    'type' => $type,
                    'wholeType' => $wholeType,
                    'valueName' => $valueName,
                ];
            }
        }

        return [
            'in' => $inParams,
            'out' => $outParams,
        ];
    }

    public function returnParser($returnType)
    {
        if (Utils::isStruct($returnType, $this->preStructs)) {
            if (!in_array($returnType, $this->useStructs)) {
                $this->extraUse .= 'use '.$this->namespaceName.'\\classes\\'.$returnType.';'.$this->returnSymbol;
                $this->useStructs[] = $returnType;
            }
            $returnInfo = [
                'type' => $returnType,
                'wholeType' => $returnType,
                'valueName' => $returnType,
            ];

            return $returnInfo;
        } elseif (Utils::isBasicType($returnType)) {
            $returnInfo = [
                'type' => $returnType,
                'wholeType' => $returnType,
                'valueName' => $returnType,
            ];

            return $returnInfo;
        }

        $state = 'init';
        $word = '';
        $wholeType = '';
        $type = '';
        $mapVectorState = false;
        $valueName = '';

        for ($i = 0; $i < strlen($returnType); ++$i) {
            $char = $returnType[$i];
            if ($state == 'init') {
                // 有可能是换行
                if (Utils::isSpace($char)) {
                    continue;
                } elseif ($char == "\n") {
                    break;
                } elseif (Utils::inIdentifier($char)) {
                    $state = 'identifier';
                    $word .= $char;
                } else {
                    Utils::abnormalExit('error', 'Interface内格式错误,请更正tars');
                }
            } elseif ($state == 'identifier') {
                // 如果遇到了space,需要检查是不是在map或vector的类型中,如果当前积累的word并不合法
                // 并且又不是处在vector或map的前置状态下的话,那么就是出错了
                //echo "[debug][state={$this->state}]word:".$word."\n";
                if (Utils::isSpace($char)) {
                    if (Utils::isBasicType($word)) {
                        $type = $word;
                        $state = 'init';
                        $word = '';
                    } elseif (Utils::isStruct($word, $this->preStructs)) {

                        // 同时要把它增加到本Interface的依赖中
                        if (!in_array($word, $this->useStructs)) {
                            $this->extraUse .= 'use '.$this->namespaceName.'\\classes\\'.$word.';'.$this->returnSymbol;
                            $this->useStructs[] = $word;
                        }

                        $type = $word;
                        $state = 'init';
                        $word = '';
                    } elseif (Utils::isEnum($word, $this->preEnums)) {
                        $type = 'unsigned byte';
                        $state = 'init';
                        $word = '';
                    } elseif (in_array($word, $this->preNamespaceStructs)) {
                        $word = explode('::', $word);
                        $word = $word[1];
                        // 同时要把它增加到本Interface的依赖中
                        if (!in_array($word, $this->useStructs)) {
                            $this->extraUse .= 'use '.$this->namespaceName.'\\classes\\'.$word.';'.$this->returnSymbol;
                            $this->useStructs[] = $word;
                        }

                        $type = $word;
                        $state = 'init';
                        $word = '';
                    } elseif (in_array($word, $this->preNamespaceEnums)) {
                        $type = 'unsigned byte';
                        $state = 'init';
                        $word = '';
                    } elseif (Utils::isMap($word)) {
                        $mapVectorState = true;
                    } elseif (Utils::isVector($word)) {
                        $mapVectorState = true;
                    } else {
                        // 读到了vector和map中间的空格,还没读完
                        if ($mapVectorState) {
                            continue;
                        }
                        // 否则剩余的部分应该就是值和默认值
                        else {
                            if (!empty($word)) {
                                $valueName = $word;
                            }
                            $state = 'init';
                            $word = '';
                        }
                    }
                }
                // 标志着map和vector的开始,不等到'>'的结束不罢休
                // 这时候需要使用栈来push,然后一个个对应的pop,从而达到type的遍历
                elseif ($char == '<') {
                    // 贪婪的向后,直到找出所有的'>'
                    $type = $word;
                    // 还会有一个wholeType,表示完整的部分
                    $mapVectorStack = [];
                    $wholeType = $type;
                    $wholeType .= '<';
                    array_push($mapVectorStack, '<');
                    while (!empty($mapVectorStack)) {
                        $moreChar = $returnType[$i + 1];
                        $wholeType .= $moreChar;
                        if ($moreChar == '<') {
                            array_push($mapVectorStack, '<');
                        } elseif ($moreChar == '>') {
                            array_pop($mapVectorStack);
                        }
                        ++$i;
                    }

                    $state = 'init';
                    $word = '';
                } else {
                    $word .= $char;
                }
            }
        }

        $returnInfo = [
            'type' => $type,
            'wholeType' => $wholeType,
            'valueName' => $valueName,
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
    public function writeInterfaceLine($returnType, $funcName, $params)
    {
        $result = $this->paramParser($params);
        $inParams = $result['in'];
        $outParams = $result['out'];

        // 处理通用的头部
        $funcHeader = $this->generateFuncHeader($funcName, $inParams, $outParams);
        $returnInfo = $this->returnParser($returnType);

        $funcBodyArr = $this->generateFuncBody($inParams, $outParams, $returnInfo);
        $synFuncBody = $funcBodyArr['syn'];

        $funcTail = $this->tabSymbol.'}'.$this->doubleReturn;

        $this->funcSet .= $funcHeader.$synFuncBody.$funcTail;
    }

    /**
     * @param $funcName
     * @param $inParams
     * @param $outParams
     *
     * @return string
     */
    public function generateFuncHeader($funcName, $inParams, $outParams)
    {
        $paramsStr = '';
        foreach ($inParams as $param) {
            $paramPrefix = Utils::paramTypeMap($param['type']);
            $paramSuffix = '$'.$param['valueName'];
            $paramsStr .= !empty($paramPrefix) ? $paramPrefix.' '.$paramSuffix.',' : $paramSuffix.',';
        }

        foreach ($outParams as $param) {
            $paramPrefix = Utils::paramTypeMap($param['type']);
            $paramSuffix = '&$'.$param['valueName'];
            $paramsStr .= !empty($paramPrefix) ? $paramPrefix.' '.$paramSuffix.',' : $paramSuffix.',';
        }

        $paramsStr = trim($paramsStr, ',');
        $paramsStr .= ') {'.$this->returnSymbol;

        $funcHeader = $this->tabSymbol.'public function '.$funcName.'('.$paramsStr;

        return $funcHeader;
    }

    /**
     * @param $funcName
     * @param $inParams
     * @param $outParams
     * 生成函数的包体
     */
    public function generateFuncBody($inParams, $outParams, $returnInfo)
    {
        $bodyPrefix = $this->doubleTab.'try {'.$this->returnSymbol;

        $bodySuffix = $this->doubleTab.'catch (\\Exception $e) {'.$this->returnSymbol.
            $this->tripleTab.'throw $e;'.$this->returnSymbol.
            $this->doubleTab.'}'.$this->returnSymbol;

        $bodyMiddle = $this->tripleTab.'$requestPacket = new RequestPacket();'.$this->returnSymbol.
            $this->tripleTab.'$requestPacket->_iVersion = $this->_iVersion;'.$this->returnSymbol.
            $this->tripleTab.'$requestPacket->_funcName = __FUNCTION__;'.$this->returnSymbol.
            $this->tripleTab.'$requestPacket->_servantName = $this->_servantName;'.$this->returnSymbol.
            $this->tripleTab.'$encodeBufs = [];'.$this->doubleReturn;

        $commonPrefix = '$__buffer = TUPAPIWrapper::';

        $index = 0;
        foreach ($inParams as $param) {
            ++$index;
            $type = $param['type'];

            $packMethod = Utils::getPackMethods($type);
            $valueName = $param['valueName'];

            // 判断如果是vector需要特别的处理
            if (Utils::isVector($type)) {
                $vecFill = $this->tripleTab.'$'.$valueName.'_vec = '.$this->getExtType($param['wholeType'], $valueName).';'.$this->returnSymbol.
                    $this->tripleTab.'foreach($'.$valueName.' as '.'$single'.$valueName.') {'.$this->returnSymbol.
                    $this->quardupleTab.'$'.$valueName.'_vec->pushBack($single'.$valueName.');'.$this->returnSymbol.
                    $this->tripleTab.'}'.$this->returnSymbol;
                $bodyMiddle .= $vecFill;
                $bodyMiddle .= $this->tripleTab.$commonPrefix.$packMethod.'("'.$valueName."\",{$index},\$".$valueName.'_vec,$this->_iVersion);'.$this->returnSymbol;
            }

            // 判断如果是map需要特别的处理
            elseif (Utils::isMap($type)) {
                $mapFill = $this->tripleTab.'$'.$valueName.'_map = '.$this->getExtType($param['wholeType'], $valueName).';'.$this->returnSymbol.
                    $this->tripleTab.'foreach($'.$valueName.' as '.'$key => $value) {'.$this->returnSymbol.
                    $this->quardupleTab.'$'.$valueName.'_map->pushBack([$key => $value]);'.$this->returnSymbol.
                    $this->tripleTab.'}'.$this->returnSymbol;
                $bodyMiddle .= $mapFill;
                $bodyMiddle .= $this->tripleTab.$commonPrefix.$packMethod.'("'.$valueName."\",{$index},\$".$valueName.'_map,$this->_iVersion);'.$this->returnSymbol;
            }
            // 针对struct,需要额外的use过程
            elseif (Utils::isStruct($type, $this->preStructs)) {
                if (!in_array($type, $this->useStructs)) {
                    $this->extraUse .= 'use '.$this->namespaceName.'\\classes\\'.$param['type'].';'.$this->returnSymbol;
                    $this->useStructs[] = $param['type'];
                }
                $bodyMiddle .= $this->tripleTab.$commonPrefix.$packMethod.'("'.$valueName."\",{$index},\$".$valueName.',$this->_iVersion);'.$this->returnSymbol;
            } else {
                $bodyMiddle .= $this->tripleTab.$commonPrefix.$packMethod.'("'.$valueName."\",{$index},\$".$valueName.',$this->_iVersion);'.$this->returnSymbol;
            }

            $bodyMiddle .= $this->tripleTab."\$encodeBufs['{$valueName}'] = \$__buffer;".$this->returnSymbol;
        }

        $bodyMiddle .= $this->tripleTab.'$requestPacket->_encodeBufs = $encodeBufs;'.
            $this->doubleReturn;

        $bodyMiddle .= $this->tripleTab.'$sBuffer = $this->_communicator->invoke($requestPacket,$this->_iTimeout);'.$this->doubleReturn;

        foreach ($outParams as $param) {
            ++$index;

            $type = $param['type'];

            $unpackMethods = Utils::getUnpackMethods($type);
            $name = $param['valueName'];

            if (Utils::isBasicType($type)) {
                $bodyMiddle .= $this->tripleTab."\$$name = TUPAPIWrapper::".$unpackMethods.'("'.$name."\",{$index},\$sBuffer,\$this->_iVersion);".$this->returnSymbol;
            } else {
                // 判断如果是vector需要特别的处理
                if (Utils::isVector($type) || Utils::isMap($type)) {
                    $bodyMiddle .= $this->tripleTab."\$$name = TUPAPIWrapper::".$unpackMethods.'("'.$name."\",{$index},".$this->getExtType($param['wholeType'], $name).',$sBuffer,$this->_iVersion);'.$this->returnSymbol;
                }
                // 如果是struct
                elseif (Utils::isStruct($type, $this->preStructs)) {
                    $bodyMiddle .= $this->tripleTab.'$ret = TUPAPIWrapper::'.$unpackMethods.'("'.$name."\",{$index},\$$name,\$sBuffer,\$this->_iVersion);".$this->returnSymbol;

                    if (!in_array($type, $this->useStructs)) {
                        $this->extraUse .= 'use '.$this->namespaceName.'\\classes\\'.$param['type'].';'.$this->returnSymbol;
                        $this->useStructs[] = $param['type'];
                    }
                }
            }
        }

        // 还要尝试去获取一下接口的返回码哦
        $returnUnpack = Utils::getUnpackMethods($returnInfo['type']);
        $valueName = $returnInfo['valueName'];

        if ($returnInfo['type'] !== 'void') {
            if (Utils::isVector($returnInfo['type']) || Utils::isMap($returnInfo['type'])) {
                $bodyMiddle .= $this->tripleTab.'return TUPAPIWrapper::'.$returnUnpack.'("",0,'
                    .$this->getExtType($returnInfo['wholeType'], $valueName).',$sBuffer,$this->_iVersion);'.$this->doubleReturn.
                    $this->doubleTab.'}'.$this->returnSymbol;
            } elseif (Utils::isStruct($returnInfo['type'], $this->preStructs)) {
                $bodyMiddle .= $this->tripleTab."\$returnVal = new $valueName();".$this->returnSymbol;
                $bodyMiddle .= $this->tripleTab.'TUPAPIWrapper::'.$returnUnpack.'("",0,$returnVal,$sBuffer,$this->_iVersion);'.$this->returnSymbol;
                $bodyMiddle .= $this->tripleTab.'return $returnVal;'.$this->doubleReturn.
                    $this->doubleTab.'}'.$this->returnSymbol;

                if (!in_array($returnInfo['type'], $this->useStructs)) {
                    $this->extraUse .= 'use '.$this->namespaceName.'\\classes\\'.$returnInfo['type'].';'.$this->returnSymbol;
                    $this->useStructs[] = $returnInfo['type'];
                }
            } else {
                $bodyMiddle .= $this->tripleTab.'return TUPAPIWrapper::'.$returnUnpack.'("",0,$sBuffer,$this->_iVersion);'.$this->doubleReturn.
                    $this->doubleTab.'}'.$this->returnSymbol;
            }
        } else {
            $bodyMiddle .= $this->doubleTab.'}'.$this->returnSymbol;
        }

        $bodyStr = $bodyPrefix.$bodyMiddle.$bodySuffix;

        return [
            'syn' => $bodyStr,
        ];
    }
}

class StructParser
{
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

    public $consts = '';
    public $variables = '';
    public $fields = '';

    public $namespaceName;

    public function __construct($fp, $line, $uniqueName, $moduleName, $structName, $preStructs, $preEnums, $namespaceName,
                                $preNamespaceEnums, $preNamespaceStructs)
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

    public function parse()
    {
        while ($this->state != 'end') {
            $this->structBodyParseLine();
        }

        // 先把积累下来的三个部分处理掉
        $structClassStr = $this->getStructClassHeader('\\classes').
            'class '.$this->structName." extends \TARS_Struct {".$this->returnSymbol;

        $structClassStr .= $this->consts.$this->doubleReturn;
        $structClassStr .= $this->variables.$this->doubleReturn;
        $fieldsPrefix = $this->tabSymbol.'protected static $_fields = array('.$this->returnSymbol;
        $fieldsSuffix = $this->tabSymbol.');'.$this->doubleReturn;

        $structClassStr .= $fieldsPrefix;
        $structClassStr .= $this->fields;
        $structClassStr .= $fieldsSuffix;

        // 处理最后一行

        $construct = $this->tabSymbol.'public function __construct() {'.$this->returnSymbol.
            $this->doubleTab."parent::__construct('".$this->uniqueName.'_'.$this->structName."', self::\$_fields);".$this->returnSymbol
            .$this->extraContructs
            .$this->extraExtInit
            .$this->tabSymbol.'}'.$this->returnSymbol;

        $structClassStr .= $construct.'}'.$this->returnSymbol;

        return $structClassStr;
    }

    /**
     * @param $startChar
     * @param $lineString
     *
     * @return string
     *                专门处理注释
     */
    public function copyAnnotation($startChar, $lineString)
    {
        $lineString .= $startChar;
        // 再读入一个字符
        $nextChar = fgetc($this->fp);
        // 第一种
        if ($nextChar == '/') {
            $lineString .= $nextChar;
            while (1) {
                $tmpChar = fgetc($this->fp);
                if (Utils::isReturn($tmpChar)) {
                    $this->state = 'lineEnd';
                    break;
                }
                $lineString .= $tmpChar;
            }

            return $lineString;
        } elseif ($nextChar == '*') {
            $lineString .= $nextChar;
            while (1) {
                $tmpChar = fgetc($this->fp);
                $lineString .= $tmpChar;

                if ($tmpChar === false) {
                    Utils::abnormalExit('error', '注释换行错误,请检查');
                } elseif (Utils::isReturn($tmpChar)) {
                } elseif (($tmpChar) === '*') {
                    $nextnextChar = fgetc($this->fp);
                    if ($nextnextChar == '/') {
                        $lineString .= $nextnextChar;

                        return $lineString;
                    } else {
                        $pos = ftell($this->fp);
                        fseek($this->fp, $pos - 1);
                    }
                }
            }
        }
        // 注释不正常
        else {
            Utils::abnormalExit('error', '注释换行错误,请检查');
        }
    }

    /**
     * @param $fp
     * @param $line
     * 这里必须要引入状态机了
     */
    public function structBodyParseLine()
    {
        $validLine = false;

        $this->state = 'init';

        $lineString = '';
        $word = '';
        $wholeType = '';
        $defaultValue = null;


        $mapVectorState = false;
        while (1) {
            $char = fgetc($this->fp);

            if ($this->state == 'init') {
                // 有可能是换行
                if ($char == '{' || Utils::isSpace($char) || $char == '\r'
                    || $char == '\x0B' || $char == '\0') {
                    continue;
                } elseif ($char == "\n") {
                    break;
                }
                // 遇到了注释会用贪婪算法全部处理完,同时填充到struct的类里面去
                elseif ($char == '/') {
                    $this->copyAnnotation($char, $lineString);
                    break;
                } elseif (Utils::inIdentifier($char)) {
                    $this->state = 'identifier';
                    $word .= $char;
                }
                // 终止条件之1,宣告struct结束
                elseif ($char == '}') {
                    // 需要贪心的读到"\n"为止
                    while (($lastChar = fgetc($this->fp)) != "\n") {
                        continue;
                    }
                    $this->state = 'end';
                    break;
                } elseif ($char == '=') {
                    //遇到等号,可以贪婪的向后,直到遇到;或者换行符
                    if (!empty($word)) {
                        $valueName = $word;
                    }
                    $moreChar = fgetc($this->fp);

                    $defaultValue = '';

                    while ($moreChar != '\n' && $moreChar != ';' && $moreChar != '}') {
                        $defaultValue .= $moreChar;

                        $moreChar = fgetc($this->fp);
                    }
                    //if(empty($defaultValue)) {
                    //    Utils::abnormalExit('error','结构体'.$this->structName.'内默认值格式错误,请更正tars');
                    //}

                    if ($moreChar == '}') {
                        // 需要贪心的读到"\n"为止
                        while (($lastChar = fgetc($this->fp)) != "\n") {
                            continue;
                        }
                        $this->state = 'end';
                    } else {
                        $this->state = 'init';
                    }
                } else {
                    //echo "char:".var_export($char,true);
                    //Utils::abnormalExit('error','结构体'.$this->structName.'内格式错误,请更正tars');
                    continue;
                }
            } elseif ($this->state == 'identifier') {
                $validLine = true;
                // 如果遇到了space,需要检查是不是在map或vector的类型中,如果当前积累的word并不合法
                // 并且又不是处在vector或map的前置状态下的话,那么就是出错了
                if (Utils::isSpace($char)) {
                    if (Utils::isTag($word)) {
                        $tag = $word;
                        $this->state = 'init';
                        $word = '';
                    } elseif (Utils::isRequireType($word)) {
                        $requireType = $word;
                        $this->state = 'init';
                        $word = '';
                    } elseif ($word == 'unsigned') {
                        $word = $word.' ';
                        continue;
                    } elseif (Utils::isBasicType($word)) {
                        $type = $word;
                        $this->state = 'init';
                        $word = '';
                    } elseif (Utils::isStruct($word, $this->preStructs)) {
                        $type = $word;
                        $this->state = 'init';
                        $word = '';
                    } elseif (Utils::isEnum($word, $this->preEnums)) {
                        $type = 'int';
                        $this->state = 'init';
                        $word = '';
                    }
                    // 增加对namespace的支持
                    elseif (in_array($word, $this->preNamespaceStructs)) {
                        $type = explode('::', $word);
                        $type = $type[1];
                        $this->state = 'init';
                        $word = '';
                    }
                    // 增加对namespace的支持
                    elseif (in_array($word, $this->preNamespaceEnums)) {
                        $type = 'int';
                        $this->state = 'init';
                        $word = '';
                    } elseif ($word == 'unsigned') {
                        $word = $word.' ';
                        continue;
                    } else {
                        // 读到了vector和map中间的空格,还没读完
                        if ($mapVectorState) {
                            continue;
                        }
                        // 否则剩余的部分应该就是值和默认值
                        else {
                            if (!empty($word)) {
                                $valueName = $word;
                            }
                            $this->state = 'init';
                            $word = '';
                        }
                    }
                }
                // 标志着map和vector的开始,不等到'>'的结束不罢休
                // 这时候需要使用栈来push,然后一个个对应的pop,从而达到type的遍历
                elseif ($char == '<') {
                    // 贪婪的向后,直到找出所有的'>'
                    $type = $word;
                    // 还会有一个wholeType,表示完整的部分
                    $mapVectorStack = [];
                    $wholeType = $type;
                    $wholeType .= '<';
                    array_push($mapVectorStack, '<');
                    while (!empty($mapVectorStack)) {
                        $moreChar = fgetc($this->fp);
                        $wholeType .= $moreChar;
                        if ($moreChar == '<') {
                            array_push($mapVectorStack, '<');
                        } elseif ($moreChar == '>') {
                            array_pop($mapVectorStack);
                        }
                    }

                    $this->state = 'init';
                    $word = '';
                } elseif ($char == '=') {
                    //遇到等号,可以贪婪的向后,直到遇到;或者换行符
                    if (!empty($word)) {
                        $valueName = $word;
                    }
                    $moreChar = fgetc($this->fp);

                    $defaultValue = '';

                    while ($moreChar != '\n' && $moreChar != ';' && $moreChar != '}') {
                        $defaultValue .= $moreChar;

                        $moreChar = fgetc($this->fp);
                    }
                    //if(empty($defaultValue)) {
                    //    Utils::abnormalExit('error','结构体'.$this->structName.'内默认值格式错误,请更正tars');
                    //}

                    if ($moreChar == '}') {
                        // 需要贪心的读到"\n"为止
                        while (($lastChar = fgetc($this->fp)) != "\n") {
                            continue;
                        }
                        $this->state = 'end';
                    } else {
                        $this->state = 'init';
                    }
                } elseif ($char == ';') {
                    if (!empty($word)) {
                        $valueName = $word;
                    }
                    continue;
                }
                // 终止条件之2,同样宣告struct结束
                elseif ($char == '}') {
                    // 需要贪心的读到"\n"为止
                    while (($lastChar = fgetc($this->fp)) != "\n") {
                        continue;
                    }
                    $this->state = 'end';
                } elseif ($char == '/') {
                    $lineString = $this->copyAnnotation($char, $lineString);
                } elseif ($char == "\n") {
                    break;
                } else {
                    $word .= $char;
                }
            } elseif ($this->state == 'lineEnd') {
                if ($char == '}') {
                    // 需要贪心的读到"\n"为止
                    while (($lastChar = fgetc($this->fp)) != "\n") {
                        continue;
                    }
                    $this->state = 'end';
                }
                break;
            } elseif ($this->state == 'end') {
                break;
            }
        }

        if (!$validLine) {
            return;
        }

        // 完成了这一行的词法解析,需要输出如下的字段
//        echo "RAW tag:".$tag." requireType:".$requireType." type:".$type.
//            " valueName:".$valueName. " wholeType:".$wholeType.
//            " defaultValue:".$defaultValue." lineString:".$lineString."\n\n";

        if (!isset($tag) || empty($requireType) || empty($type) || empty($valueName)) {
            Utils::abnormalExit('error', '结构体'.$this->structName.'内格式错误,请更正tars');
        } elseif ($type == 'map' && empty($wholeType)) {
            Utils::abnormalExit('error', '结构体'.$this->structName.'内map格式错误,请更正tars');
        } elseif ($type == 'vector' && empty($wholeType)) {
            Utils::abnormalExit('error', '结构体'.$this->structName.'内vector格式错误,请更正tars');
        } else {
            $this->writeStructLine($tag, $requireType, $type, $valueName, $wholeType, $defaultValue);
        }
    }

    /**
     * @param $wholeType
     * 通过完整的类型获取vector的扩展类型
     * vector<CateObj> => new \TARS_VECTOR(new CateObj())
     * vector<string> => new \TARS_VECTOR(\TARS::STRING)
     * vector<map<string,CateObj>> => new \TARS_VECTOR(new \TARS_MAP(\TARS_MAP,new CateObj()))
     */
    public function getExtType($wholeType, $valueName)
    {
        $state = 'init';
        $word = '';
        $extType = '';

        for ($i = 0; $i < strlen($wholeType); ++$i) {
            $char = $wholeType[$i];
            if ($state == 'init') {
                // 如果遇到了空格
                if (Utils::isSpace($char)) {
                    continue;
                }
                // 回车是停止符号
                elseif (Utils::inIdentifier($char)) {
                    $state = 'indentifier';
                    $word .= $char;
                } elseif (Utils::isReturn($char)) {
                    break;
                } elseif ($char == '>') {
                    $extType .= ')';
                    continue;
                }
            } elseif ($state == 'indentifier') {
                if ($char == '<') {
                    // 替换word,替换< 恢复初始状态
                    $tmp = $this->VecMapReplace($word);
                    $extType .= $tmp;
                    $extType .= '(';
                    $word = '';
                    $state = 'init';
                } elseif ($char == '>') {
                    // 替换word,替换> 恢复初始状态
                    // 替换word,替换< 恢复初始状态
                    $tmp = $this->VecMapReplace($word);
                    $extType .= $tmp;
                    $extType .= ')';
                    $word = '';
                    $state = 'init';
                } elseif ($char == ',') {
                    // 替换word,替换, 恢复初始状态
                    // 替换word,替换< 恢复初始状态
                    $tmp = $this->VecMapReplace($word);
                    $extType .= $tmp;
                    $extType .= ',';
                    $word = '';
                    $state = 'init';
                } else {
                    $word .= $char;
                    continue;
                }
            }
        }

        return $extType;
    }

    public function VecMapReplace($word)
    {
        $word = trim($word);
        // 遍历所有的类型
        foreach (Utils::$wholeTypeMap as $key => $value) {
            $word = preg_replace('/\b'.$key.'\b/', $value, $word);
        }

        if (Utils::isStruct($word, $this->preStructs)) {
            $word = 'new '.$word.'()';
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
    public function writeStructLine($tag, $requireType, $type, $valueName, $wholeType, $defaultValue)
    {
        if ($requireType === 'require') {
            $requireFlag = 'true';
        } else {
            $requireFlag = 'false';
        }

        $this->consts .= $this->tabSymbol.'const '.strtoupper($valueName).' = '.$tag.';'.$this->returnSymbol;
        if (!empty($defaultValue)) {
            $this->variables .= $this->tabSymbol.'public $'.$valueName.'='.$defaultValue.';'.' '.$this->returnSymbol;
        } else {
            $this->variables .= $this->tabSymbol.'public $'.$valueName.';'.' '.$this->returnSymbol;
        }

        // 基本类型,直接替换
        if (Utils::isBasicType($type)) {
            $this->fields .= $this->doubleTab.'self::'.strtoupper($valueName).' => array('.$this->returnSymbol.
                $this->tripleTab."'name'=>'".$valueName."',".$this->returnSymbol.
                $this->tripleTab."'required'=>".$requireFlag.','.$this->returnSymbol.
                $this->tripleTab."'type'=>".Utils::getRealType($type).','.$this->returnSymbol.
                $this->tripleTab.'),'.$this->returnSymbol;
        } elseif (Utils::isStruct($type, $this->preStructs)) {
            $this->fields .= $this->doubleTab.'self::'.strtoupper($valueName).' => array('.$this->returnSymbol.
                $this->tripleTab."'name'=>'".$valueName."',".$this->returnSymbol.
                $this->tripleTab."'required'=>".$requireFlag.','.$this->returnSymbol.
                $this->tripleTab."'type'=>".Utils::getRealType($type).','.$this->returnSymbol.
                $this->tripleTab.'),'.$this->returnSymbol;
            $this->extraContructs .= $this->doubleTab."\$this->$valueName = new $type();".$this->returnSymbol;
        } elseif (Utils::isVector($type) || Utils::isMap($type)) {
            $extType = $this->getExtType($wholeType, $valueName);
            $this->extraExtInit .= $this->doubleTab.'$this->'.$valueName.' = '.$extType.';'.$this->returnSymbol;

            $this->fields .= $this->doubleTab.'self::'.strtoupper($valueName).' => array('.$this->returnSymbol.
                $this->tripleTab."'name'=>'".$valueName."',".$this->returnSymbol.
                $this->tripleTab."'required'=>".$requireFlag.','.$this->returnSymbol.
                $this->tripleTab."'type'=>".Utils::getRealType($type).','.$this->returnSymbol.
                $this->tripleTab.'),'.$this->returnSymbol;
        } else {
            Utils::abnormalExit('error', '结构体struct'.$this->structName.'内类型有误,请更正tars');
        }
    }

    public function getStructClassHeader($prefix = 'getFileHea')
    {
        return "<?php\n\nnamespace ".$this->namespaceName.$prefix.';'.
        $this->doubleReturn;
    }
}

class ServantParser
{
    public $namespaceName;
    public $moduleName;
    public $interfaceName;

    public $state;

    // 这个结构体,可能会引用的部分,包括其他的结构体、枚举类型、常量
    public $useStructs = [];
    public $extraUse;
    public $preStructs;
    public $preEnums;

    public $preNamespaceEnums = [];
    public $preNamespaceStructs = [];

    public $firstLine;

    public $returnSymbol = "\n";
    public $doubleReturn = "\n\n";
    public $tabSymbol = "\t";
    public $doubleTab = "\t\t";
    public $tripleTab = "\t\t\t";
    public $quardupleTab = "\t\t\t\t";

    public $extraContructs = '';
    public $extraExtType = '';
    public $extraExtInit = '';

    public $consts = '';
    public $variables = '';
    public $fields = '';

    public $funcSet = '';

    public $servantName;

    public function __construct($fp, $line, $namespaceName, $moduleName,
                                $interfaceName, $preStructs,
                                $preEnums, $servantName, $preNamespaceEnums, $preNamespaceStructs)
    {
        $this->fp = $fp;
        $this->firstLine = $line;
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

    public function isEnum($word)
    {
        return in_array($word, $this->preEnums);
    }

    public function isStruct($word)
    {
        return in_array($word, $this->preStructs);
    }

    public function getFileHeader($prefix = '')
    {
        return "<?php\n\nnamespace ".$this->namespaceName.$prefix.';'.
        $this->doubleReturn;
    }

    public function parse()
    {
        while ($this->state != 'end') {
            $this->InterfaceFuncParseLine();
        }

        // todo serverName+servant
        $interfaceClass = $this->getFileHeader('').$this->extraUse.'interface '.$this->interfaceName.' {'.$this->returnSymbol;

        $interfaceClass .= $this->funcSet;

        $interfaceClass .= '}'.$this->doubleReturn;

        return $interfaceClass;
    }

    /**
     * @param $startChar
     * @param $lineString
     *
     * @return string
     *                专门处理注释
     */
    public function copyAnnotation()
    {
        // 再读入一个字符
        $nextChar = fgetc($this->fp);
        // 第一种
        if ($nextChar == '/') {
            while (1) {
                $tmpChar = fgetc($this->fp);
                if (Utils::isReturn($tmpChar)) {
                    $this->state = 'lineEnd';
                    break;
                }
            }

            return;
        } elseif ($nextChar == '*') {
            while (1) {
                $tmpChar = fgetc($this->fp);

                if ($tmpChar === false) {
                    Utils::abnormalExit('error', $this->interfaceName.'注释换行错误,请检查');
                } elseif (Utils::isReturn($tmpChar)) {
                } elseif (($tmpChar) === '*') {
                    $nextnextChar = fgetc($this->fp);
                    if ($nextnextChar == '/') {
                        return;
                    } else {
                        $pos = ftell($this->fp);
                        fseek($this->fp, $pos - 1);
                    }
                }
            }
        }
        // 注释不正常
        else {
            Utils::abnormalExit('error', $this->interfaceName.'注释换行错误,请检查');
        }
    }

    /**
     * @param $fp
     * @param $line
     * 这里必须要引入状态机了
     * 这里并不一定要一个line呀,应该找)作为结束符
     */
    public function InterfaceFuncParseLine()
    {
        $line = '';
        $this->state = 'init';
        while (1) {
            $char = fgetc($this->fp);

            if ($this->state == 'init') {
                // 有可能是换行
                if ($char == '{' || Utils::isReturn($char)) {
                    continue;
                }
                // 遇到了注释会用贪婪算法全部处理完,同时填充到struct的类里面去
                elseif ($char == '/') {
                    $this->copyAnnotation();
                    break;
                } elseif (Utils::inIdentifier($char)) {
                    $this->state = 'identifier';
                    $line .= $char;
                }
                // 终止条件之1,宣告struct结束
                elseif ($char == '}') {
                    // 需要贪心的读到"\n"为止
                    while (($lastChar = fgetc($this->fp)) != "\n") {
                        continue;
                    }
                    $this->state = 'end';
                    break;
                }
            } elseif ($this->state == 'identifier') {
                if ($char == '/') {
                    $this->copyAnnotation();
                } elseif ($char == ';') {
                    $line .= $char;
                    break;
                }
                // 终止条件之2,同样宣告interface结束
                elseif ($char == '}') {
                    // 需要贪心的读到"\n"为止
                    while (($lastChar = fgetc($this->fp)) != "\n") {
                        continue;
                    }
                    $this->state = 'end';
                } elseif (Utils::isReturn($char)) {
                    continue;
                } elseif ($char == ')') {
                    $line .= $char;
                    // 需要贪心的读到"\n"为止
                    while (($lastChar = fgetc($this->fp)) != "\n") {
                        continue;
                    }
                    $this->state = 'lineEnd';
                } else {
                    $line .= $char;
                }
            } elseif ($this->state == 'lineEnd') {
                if ($char == '}') {
                    // 需要贪心的读到"\n"为止
                    while (($lastChar = fgetc($this->fp)) != "\n") {
                        continue;
                    }
                    $this->state = 'end';
                }
                break;
            } elseif ($this->state == 'end') {
                break;
            }
        }

        if (empty($line)) {
            return;
        }

        $line = trim($line);

        // 如果空行，或者是注释，或者是大括号就直接略过
        if (!trim($line) || trim($line)[0] === '/' || trim($line)[0] === '*' || trim($line) === '{') {
            return;
        }

        $endFlag = strpos($line, '};');
        if ($endFlag !== false) {
            $this->state = 'end';

            return;
        }

        $endFlag = strpos($line, '}');
        if ($endFlag !== false) {
            $this->state = 'end';

            return;
        }

        // 有必要先分成三个部分,返回类型、接口名、参数列表 todo
        $tokens = preg_split('/\(/', $line, 2);
        $mix = $tokens[0];
        $rest = $tokens[1];

        $pices = preg_split('/\s+/', $mix);

        $funcName = $pices[count($pices) - 1];

        $returnType = implode('', array_slice($pices, 0, count($pices) - 1));

        $state = 'init';
        $word = '';

        $params = [];

        for ($i = 0; $i < strlen($rest); ++$i) {
            $char = $rest[$i];

            if ($state == 'init') {
                // 有可能是换行
                if ($char == '(' || Utils::isSpace($char)) {
                    continue;
                } elseif (Utils::isReturn($char)) {
                    break;
                } elseif (Utils::inIdentifier($char)) {
                    $state = 'identifier';
                    $word .= $char;
                }
                // 终止条件之1,宣告interface结束
                elseif ($char == ')') {
                    break;
                } else {
                    Utils::abnormalExit('error', 'Interface'.$this->interfaceName.'内格式错误,请更正tars in line:'.__LINE__);
                }
            } elseif ($state == 'identifier') {
                if ($char == ',') {
                    $params[] = $word;
                    $state = 'init';
                    $word = '';
                    continue;
                }
                // 标志着map和vector的开始,不等到'>'的结束不罢休
                // 这时候需要使用栈来push,然后一个个对应的pop,从而达到type的遍历
                elseif ($char == '<') {
                    $mapVectorStack = [];
                    $word .= $char;
                    array_push($mapVectorStack, '<');
                    while (!empty($mapVectorStack)) {
                        $moreChar = $rest[$i + 1];
                        $word .= $moreChar;
                        if ($moreChar == '<') {
                            array_push($mapVectorStack, '<');
                        } elseif ($moreChar == '>') {
                            array_pop($mapVectorStack);
                        }
                        ++$i;
                    }
                    continue;
                } elseif ($char == ')') {
                    $params[] = $word;
                    break;
                } elseif ($char == ';') {
                    continue;
                }
                // 终止条件之2,同样宣告struct结束
                elseif ($char == '}') {
                    $state = 'end';
                } elseif (Utils::isReturn($char)) {
                    break;
                } else {
                    $word .= $char;
                }
            } elseif ($state == 'lineEnd') {
                break;
            } elseif ($state == 'end') {
                break;
            }
        }

        $this->writeInterfaceLine($returnType, $funcName, $params);
    }

    /**
     * @param $wholeType
     * 通过完整的类型获取vector的扩展类型
     */
    public function getExtType($wholeType)
    {
        $state = 'init';
        $word = '';
        $extType = '';

        for ($i = 0; $i < strlen($wholeType); ++$i) {
            $char = $wholeType[$i];
            if ($state == 'init') {
                // 如果遇到了空格
                if (Utils::isSpace($char)) {
                    continue;
                }
                // 回车是停止符号
                elseif (Utils::inIdentifier($char)) {
                    $state = 'indentifier';
                    $word .= $char;
                } elseif (Utils::isReturn($char)) {
                    break;
                } elseif ($char == '>') {
                    $extType .= ')';
                    continue;
                }
            } elseif ($state == 'indentifier') {
                if ($char == '<') {
                    // 替换word,替换< 恢复初始状态
                    $tmp = $this->VecMapReplace($word);
                    $extType .= $tmp;
                    $extType .= '(';
                    $word = '';
                    $state = 'init';
                } elseif ($char == '>') {
                    // 替换word,替换> 恢复初始状态
                    // 替换word,替换< 恢复初始状态
                    $tmp = $this->VecMapReplace($word);
                    $extType .= $tmp;
                    $extType .= ')';
                    $word = '';
                    $state = 'init';
                } elseif ($char == ',') {
                    // 替换word,替换, 恢复初始状态
                    // 替换word,替换< 恢复初始状态
                    $tmp = $this->VecMapReplace($word);
                    $extType .= $tmp;
                    $extType .= ',';
                    $word = '';
                    $state = 'init';
                } else {
                    $word .= $char;
                    continue;
                }
            }
        }

        return $extType;
    }

    public function VecMapReplace($word)
    {
        $word = trim($word);
        // 遍历所有的类型
        foreach (Utils::$wholeTypeMap as $key => $value) {
            if ($this->isStruct($word)) {
                if (!in_array($word, $this->useStructs)) {
                    $this->useStructs[] = $word;
                }
                $word = '\\'.$this->namespaceName.'\\classes\\'.$word;
                break;
            } elseif (in_array($word, $this->preNamespaceStructs)) {
                $words = explode('::', $word);
                $word = $words[1];
                if (!in_array($word, $this->useStructs)) {
                    $this->useStructs[] = $word;
                }
                $word = '\\'.$this->namespaceName.'\\classes\\'.$word;
                break;
            } else {
                $word = preg_replace('/\b'.$key.'\b/', $value, $word);
            }
        }

        $word = trim($word, 'new ');

        return $word;
    }

    public function paramParser($params)
    {

        // 输入和输出的参数全部捋一遍
        $inParams = [];
        $outParams = [];
        foreach ($params as $param) {
            $state = 'init';
            $word = '';
            $wholeType = '';
            $paramType = 'in';
            $type = '';
            $mapVectorState = false;

            for ($i = 0; $i < strlen($param); ++$i) {
                $char = $param[$i];
                if ($state == 'init') {
                    // 有可能是换行
                    if (Utils::isSpace($char)) {
                        continue;
                    } elseif (Utils::isReturn($char)) {
                        break;
                    } elseif (Utils::inIdentifier($char)) {
                        $state = 'identifier';
                        $word .= $char;
                    } else {
                        Utils::abnormalExit('error', 'Interface内格式错误,请更正tars in line:'.__LINE__);
                    }
                } elseif ($state == 'identifier') {
                    // 如果遇到了space,需要检查是不是在map或vector的类型中,如果当前积累的word并不合法
                    // 并且又不是处在vector或map的前置状态下的话,那么就是出错了
                    if (Utils::isSpace($char)) {
                        if ($word == 'out') {
                            $paramType = $word;
                            $state = 'init';
                            $word = '';
                        } elseif (Utils::isBasicType($word)) {
                            $type = $word;
                            $state = 'init';
                            $word = '';
                        } elseif ($this->isStruct($word)) {

                            // 同时要把它增加到本Interface的依赖中
                            if (!in_array($word, $this->useStructs)) {
                                $this->extraUse .= 'use '.$this->namespaceName.'\\classes\\'.$word.';'.$this->returnSymbol;
                                $this->useStructs[] = $word;
                            }

                            $type = $word;
                            $state = 'init';
                            $word = '';
                        } elseif ($this->isEnum($word)) {
                            $type = 'int';
                            $state = 'init';
                            $word = '';
                        } elseif (in_array($word, $this->preNamespaceStructs)) {
                            $word = explode('::', $word);
                            $word = $word[1];
                            // 同时要把它增加到本Interface的依赖中
                            if (!in_array($word, $this->useStructs)) {
                                $this->extraUse .= 'use '.$this->namespaceName.'\\classes\\'.$word.';'.$this->returnSymbol;
                                $this->useStructs[] = $word;
                            }

                            $type = $word;
                            $state = 'init';
                            $word = '';
                        } elseif (in_array($word, $this->preNamespaceEnums)) {
                            $type = 'int';
                            $state = 'init';
                            $word = '';
                        } elseif (Utils::isMap($word)) {
                            $mapVectorState = true;
                        } elseif (Utils::isVector($word)) {
                            $mapVectorState = true;
                        } else {
                            // 读到了vector和map中间的空格,还没读完
                            if ($mapVectorState) {
                                continue;
                            }
                            // 否则剩余的部分应该就是值和默认值
                            else {
                                if (!empty($word)) {
                                    $valueName = $word;
                                }
                                $state = 'init';
                                $word = '';
                            }
                        }
                    }
                    // 标志着map和vector的开始,不等到'>'的结束不罢休
                    // 这时候需要使用栈来push,然后一个个对应的pop,从而达到type的遍历
                    elseif ($char == '<') {
                        // 贪婪的向后,直到找出所有的'>'
                        $type = $word;
                        // 还会有一个wholeType,表示完整的部分
                        $mapVectorStack = [];
                        $wholeType = $type;
                        $wholeType .= '<';
                        array_push($mapVectorStack, '<');
                        while (!empty($mapVectorStack)) {
                            $moreChar = $param[$i + 1];
                            $wholeType .= $moreChar;
                            if ($moreChar == '<') {
                                array_push($mapVectorStack, '<');
                            } elseif ($moreChar == '>') {
                                array_pop($mapVectorStack);
                            }
                            ++$i;
                        }

                        $state = 'init';
                        $word = '';
                    } else {
                        $word .= $char;
                    }
                }
            }

            if (!empty($word)) {
                $valueName = $word;
            }

            if ($paramType == 'in') {
                $inParams[] = [
                    'type' => $type,
                    'wholeType' => $wholeType,
                    'valueName' => $valueName,
                ];
            } else {
                $outParams[] = [
                    'type' => $type,
                    'wholeType' => $wholeType,
                    'valueName' => $valueName,
                ];
            }
        }

        return [
            'in' => $inParams,
            'out' => $outParams,
        ];
    }

    public function returnParser($returnType)
    {
        if ($this->isStruct($returnType)) {
            if (!in_array($returnType, $this->useStructs)) {
                $this->useStructs[] = $returnType;
            }
            $returnInfo = [
                'type' => $returnType,
                'wholeType' => $returnType,
                'valueName' => $returnType,
            ];

            return $returnInfo;
        } elseif ($this->isEnum($returnType)) {
            $returnInfo = [
                'type' => $returnType,
                'wholeType' => $returnType,
                'valueName' => $returnType,
            ];

            return $returnInfo;
        } elseif (Utils::isBasicType($returnType)) {
            $returnInfo = [
                'type' => $returnType,
                'wholeType' => $returnType,
                'valueName' => $returnType,
            ];

            return $returnInfo;
        }

        $state = 'init';
        $word = '';
        $wholeType = '';
        $type = '';
        $mapVectorState = false;
        $valueName = '';

        for ($i = 0; $i < strlen($returnType); ++$i) {
            $char = $returnType[$i];
            if ($state == 'init') {
                // 有可能是换行
                if (Utils::isSpace($char)) {
                    continue;
                } elseif ($char == "\n") {
                    break;
                } elseif (Utils::inIdentifier($char)) {
                    $state = 'identifier';
                    $word .= $char;
                } else {
                    Utils::abnormalExit('error', 'Interface内格式错误,请更正tars');
                }
            } elseif ($state == 'identifier') {
                // 如果遇到了space,需要检查是不是在map或vector的类型中,如果当前积累的word并不合法
                // 并且又不是处在vector或map的前置状态下的话,那么就是出错了
                if (Utils::isSpace($char)) {
                    if (Utils::isBasicType($word)) {
                        $type = $word;
                        $state = 'init';
                        $word = '';
                    } elseif ($this->isStruct($word)) {

                        // 同时要把它增加到本Interface的依赖中
                        if (!in_array($word, $this->useStructs)) {
                            $this->extraUse .= 'use '.$this->namespaceName.'\\classes\\'.$word.';'.$this->returnSymbol;
                            $this->useStructs[] = $word;
                        }

                        $type = $word;
                        $state = 'init';
                        $word = '';
                    } elseif ($this->isEnum($word)) {
                        $type = 'int';
                        $state = 'init';
                        $word = '';
                    } elseif (in_array($word, $this->preNamespaceStructs)) {
                        $word = explode('::', $word);
                        $word = $word[1];
                        // 同时要把它增加到本Interface的依赖中
                        if (!in_array($word, $this->useStructs)) {
                            $this->extraUse .= 'use '.$this->namespaceName.'\\classes\\'.$word.';'.$this->returnSymbol;
                            $this->useStructs[] = $word;
                        }

                        $type = $word;
                        $state = 'init';
                        $word = '';
                    } elseif (in_array($word, $this->preNamespaceEnums)) {
                        $type = 'int';
                        $state = 'init';
                        $word = '';
                    } elseif (Utils::isMap($word)) {
                        $mapVectorState = true;
                    } elseif (Utils::isVector($word)) {
                        $mapVectorState = true;
                    } else {
                        // 读到了vector和map中间的空格,还没读完
                        if ($mapVectorState) {
                            continue;
                        }
                        // 否则剩余的部分应该就是值和默认值
                        else {
                            if (!empty($word)) {
                                $valueName = $word;
                            }
                            $state = 'init';
                            $word = '';
                        }
                    }
                }
                // 标志着map和vector的开始,不等到'>'的结束不罢休
                // 这时候需要使用栈来push,然后一个个对应的pop,从而达到type的遍历
                elseif ($char == '<') {
                    // 贪婪的向后,直到找出所有的'>'
                    $type = $word;
                    // 还会有一个wholeType,表示完整的部分
                    $mapVectorStack = [];
                    $wholeType = $type;
                    $wholeType .= '<';
                    array_push($mapVectorStack, '<');
                    while (!empty($mapVectorStack)) {
                        $moreChar = $returnType[$i + 1];
                        $wholeType .= $moreChar;
                        if ($moreChar == '<') {
                            array_push($mapVectorStack, '<');
                        } elseif ($moreChar == '>') {
                            array_pop($mapVectorStack);
                        }
                        ++$i;
                    }

                    $state = 'init';
                    $word = '';
                } else {
                    $word .= $char;
                }
            }
        }

        $returnInfo = [
            'type' => $type,
            'wholeType' => $wholeType,
            'valueName' => $valueName,
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
    public function writeInterfaceLine($returnType, $funcName, $params)
    {
        $result = $this->paramParser($params);
        $inParams = $result['in'];
        $outParams = $result['out'];

        $returnInfo = $this->returnParser($returnType);

        $funcAnnotation = $this->generateFuncAnnotation($inParams, $outParams, $returnInfo);

        // 函数定义恰恰是要放在最后面了
        $funcDefinition = $this->generateFuncHeader($funcName, $inParams, $outParams);

        $this->funcSet .= $funcAnnotation.$funcDefinition;
    }

    private function paramTypeMap($paramType)
    {
        if (Utils::isBasicType($paramType) || Utils::isMap($paramType) || Utils::isVector($paramType)) {
            return '';
        } else {
            return $paramType;
        }
    }
    /**
     * @param $funcName
     * @param $inParams
     * @param $outParams
     *
     * @return string
     */
    public function generateFuncHeader($funcName, $inParams, $outParams)
    {
        $paramsStr = '';
        foreach ($inParams as $param) {
            $paramPrefix = $this->paramTypeMap($param['type']);
            $paramSuffix = '$'.$param['valueName'];
            $paramsStr .= !empty($paramPrefix) ? $paramPrefix.' '.$paramSuffix.',' : $paramSuffix.',';
        }

        foreach ($outParams as $param) {
            $paramPrefix = $this->paramTypeMap($param['type']);
            $paramSuffix = '&$'.$param['valueName'];
            $paramsStr .= !empty($paramPrefix) ? $paramPrefix.' '.$paramSuffix.',' : $paramSuffix.',';
        }

        $paramsStr = trim($paramsStr, ',');
        $paramsStr .= ');'.$this->returnSymbol;

        $funcHeader = $this->tabSymbol.'public function '.$funcName.'('.$paramsStr;

        return $funcHeader;
    }

    /**
     * @param $funcName
     * @param $inParams
     * @param $outParams
     * 生成函数的包体
     */
    public function generateFuncAnnotation($inParams, $outParams, $returnInfo)
    {
        $bodyPrefix = $this->tabSymbol.'/**'.$this->returnSymbol;

        $bodyMiddle = '';

        foreach ($inParams as $param) {
            $annotation = $this->tabSymbol.' * @param ';
            $type = $param['type'];
            $valueName = $param['valueName'];
            $wholeType = $param['wholeType'];

            // 判断如果是vector需要特别的处理
            if (Utils::isVector($type)) {
                $annotation .= 'vector'.' $'.$valueName.' '.$this->getExtType($wholeType);
            }

            // 判断如果是map需要特别的处理
            elseif (Utils::isMap($type)) {
                $annotation .= 'map'.' $'.$valueName.' '.$this->getExtType($wholeType);
            }
            // 针对struct,需要额外的use过程
            elseif ($this->isStruct($type)) {
                $annotation .= 'struct'.' $'.$valueName.' \\'.$this->namespaceName.'\\classes\\'.$type;
            } else {
                $annotation .= $type.' $'.$valueName.' ';
            }
            $bodyMiddle .= $annotation.$this->returnSymbol;
        }

        foreach ($outParams as $param) {
            $annotation = $this->tabSymbol.' * @param ';
            $type = $param['type'];
            $valueName = $param['valueName'];
            $wholeType = $param['wholeType'];

            if (Utils::isBasicType($type)) {
                $annotation .= $type.' $'.$valueName;
            } else {
                // 判断如果是vector需要特别的处理
                if (Utils::isVector($type)) {
                    $annotation .= 'vector'.' $'.$valueName.' '.$this->getExtType($wholeType);
                } elseif (Utils::isMap($type)) {
                    $annotation .= 'map'.' $'.$valueName.' '.$this->getExtType($wholeType);
                }
                // 如果是struct
                elseif ($this->isStruct($type)) {
                    $annotation .= 'struct'.' $'.$valueName.' \\'.$this->namespaceName.'\\classes\\'.$type;
                }
            }

            $annotation .= ' =out='.$this->returnSymbol;
            $bodyMiddle .= $annotation;
        }

        // 还要尝试去获取一下接口的返回码哦
        $type = $returnInfo['type'];
        $valueName = $returnInfo['valueName'];
        $wholeType = $returnInfo['wholeType'];

        $annotation = $this->tabSymbol.' * @return ';

        if ($type !== 'void') {
            if (Utils::isVector($type)) {
                $annotation .= 'vector '.$this->getExtType($wholeType);
            } elseif (Utils::isMap($type)) {
                $annotation .= 'map '.$this->getExtType($wholeType);
            } elseif ($this->isStruct($type)) {
                $annotation .= 'struct \\'.$this->namespaceName.'\\classes\\'.$type;
            } else {
                $annotation .= $type;
            }
        } else {
            $annotation .= 'void';
        }

        $bodyMiddle .= $annotation.$this->returnSymbol.$this->tabSymbol.' */'.$this->returnSymbol;

        $bodyStr = $bodyPrefix.$bodyMiddle;

        return  $bodyStr;
    }
}
