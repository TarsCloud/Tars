--TEST--
basic: float

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
require_once __DIR__ . "/../include/functions.php";

$float = 3.141592;

$buf = \TUPAPI::putFloat("float",$float);

$encodeBufs['float'] = $buf;

$requestBuf = \TUPAPI::encode($iVersion, $iRequestId, $servantName, $funcName, $cPacketType, $iMessageType, $iTimeout, $contexts,$statuses,$encodeBufs);

$decodeRet = \TUPAPI::decode($requestBuf);
assert($decodeRet['status'] == 0);

if($decodeRet['status'] !== 0) {
    echo "error";
} else {
    $respBuf = $decodeRet['buf'];

    $out = \TUPAPI::getFloat("float",$respBuf);

    if (floatComp($float, $out, 7) == 0) {
        echo "success";
    }
}

?>
--EXPECT--
success