--TEST--
vector: string

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

$shorts = ["test1","test2"];
$vector = new \TARS_VECTOR(\TARS::STRING);
foreach ($shorts as $short) {
    $vector->pushBack($short);
}

$buf = \TUPAPI::putVector("vec",$vector);

$encodeBufs['vec'] = $buf;

$requestBuf = \TUPAPI::encode($iVersion, $iRequestId, $servantName, $funcName, $cPacketType, $iMessageType, $iTimeout, $contexts,$statuses,$encodeBufs);

$decodeRet = \TUPAPI::decode($requestBuf);
assert($decodeRet['status'] == 0);

if($decodeRet['status'] !== 0) {
    echo "error";
} else {
    $respBuf = $decodeRet['buf'];
    $v = new \TARS_VECTOR(\TARS::STRING);
    $out = \TUPAPI::getVector("vec", $v,$respBuf);

    if ($shorts == $out) {
        echo "success";
    }
}

?>
--EXPECT--
success