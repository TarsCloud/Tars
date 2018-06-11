--TEST--
struct: all type struct

--SKIPIF--
<?php require __DIR__ . "/../include/skipif.inc"; ?>
--INI--
zend.assertions=-1
assert.active=1
assert.warning=1
assert.bail=0
assert.quiet_eval=0

--FILE--
<?php
require_once __DIR__ . "/../include/config.inc";
require_once __DIR__ . "/../include/AllTypeStruct.php";
require_once __DIR__ . "/../include/functions.php";

$allTypeStruct = new AllTypeStruct();
$allTypeStruct->bool = true;
$allTypeStruct->char = 'a';
$allTypeStruct->uint8 = 255;
$allTypeStruct->short = -32768;
$allTypeStruct->uint16 = 65535;
$allTypeStruct->float = 3.56;
$allTypeStruct->double = 3.1231231212;
$allTypeStruct->int32 = -2147483648;
$allTypeStruct->uint32 = 4294967295;
$allTypeStruct->int64 = 9223372036854775807;
$allTypeStruct->string = 'test';
$shorts = ["t","a"];
foreach ($shorts as $short) {
    $allTypeStruct->vecchar->pushBack($short);
}

$buf = \TUPAPI::putStruct("struct",$allTypeStruct);

$encodeBufs['struct'] = $buf;

$requestBuf = \TUPAPI::encode($iVersion, $iRequestId, $servantName, $funcName, $cPacketType, $iMessageType, $iTimeout, $contexts,$statuses,$encodeBufs);

$decodeRet = \TUPAPI::decode($requestBuf);
assert($decodeRet['status'] == 0);

if($decodeRet['status'] !== 0) {
    echo "error";
} else {
    $decodeRet = \TUPAPI::decode($requestBuf);
    $respBuf = $decodeRet['buf'];

    $allTypeStruct->vecchar = 'ta';

    $outAllTypeStruct = new AllTypeStruct();
    $result = \TUPAPI::getStruct("struct",$outAllTypeStruct,$respBuf);
    fromArray($result,$outAllTypeStruct);


    if ($allTypeStruct->bool == $outAllTypeStruct->bool &&
        $allTypeStruct->char == $outAllTypeStruct->char &&
        $allTypeStruct->uint8 == $outAllTypeStruct->uint8 &&
        $allTypeStruct->short == $outAllTypeStruct->short &&
        floatComp($allTypeStruct->float, $outAllTypeStruct->float, 7) == 0 &&
        $allTypeStruct->double == $outAllTypeStruct->double &&
        $allTypeStruct->int32 == $outAllTypeStruct->int32 &&
        $allTypeStruct->uint32 == $outAllTypeStruct->uint32 &&
        $allTypeStruct->int64 == $outAllTypeStruct->int64 &&
        $allTypeStruct->string == $outAllTypeStruct->string &&
        "ta" == $outAllTypeStruct->vecchar
    ) {
        echo "success";
    } else {
        var_dump($outAllTypeStruct);
    }
}

?>
--EXPECT--
success