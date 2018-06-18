--TEST--
basic: string with int

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

$string  = 222;

$buf = \TUPAPI::putString("string",$string);

$encodeBufs['string'] = $buf;

$requestBuf = \TUPAPI::encode($iVersion, $iRequestId, $servantName, $funcName, $cPacketType, $iMessageType, $iTimeout, $contexts,$statuses,$encodeBufs);

$decodeRet = \TUPAPI::decode($requestBuf);
assert($decodeRet['status'] == 0);

if($decodeRet['status'] !== 0) {
    echo "error";
} else {
    $respBuf = $decodeRet['buf'];

    $out = \TUPAPI::getString("string",$respBuf);

    if ($string == $out) {
        echo "success";
    }
}

?>
--EXPECT--
success