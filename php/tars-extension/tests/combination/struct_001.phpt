--TEST--
struct: simple struct

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
require_once __DIR__ . "/../include/SimpleStruct.php";
require_once __DIR__ . "/../include/functions.php";

$simpleStruct = new SimpleStruct();
$simpleStruct->id = 1;
$simpleStruct->count = 2;

$buf = \TUPAPI::putStruct("struct",$simpleStruct);

$encodeBufs['struct'] = $buf;

$requestBuf = \TUPAPI::encode($iVersion, $iRequestId, $servantName, $funcName, $cPacketType, $iMessageType, $iTimeout, $contexts,$statuses,$encodeBufs);

$decodeRet = \TUPAPI::decode($requestBuf);
assert($decodeRet['status'] == 0);

if($decodeRet['status'] !== 0) {
    echo "error";
} else {
    $decodeRet = \TUPAPI::decode($requestBuf);
    $respBuf = $decodeRet['buf'];

    $outSimpleStruct = new SimpleStruct();
    $result = \TUPAPI::getStruct("struct",$outSimpleStruct,$respBuf);
    fromArray($result,$outSimpleStruct);

    if ($simpleStruct == $outSimpleStruct) {
        echo "success";
    }
}

?>
--EXPECT--
success