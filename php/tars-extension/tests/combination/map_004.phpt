--TEST--
map: uint32 & double

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

$data = [123 => 1.1];
$map = new \TARS_MAP(\TARS::UINT32, \TARS::DOUBLE);
$map->pushBack($data);

$buf = \TUPAPI::putMap("map",$map);

$encodeBufs['map'] = $buf;

$requestBuf = \TUPAPI::encode($iVersion, $iRequestId, $servantName, $funcName, $cPacketType, $iMessageType, $iTimeout, $contexts,$statuses,$encodeBufs);

$decodeRet = \TUPAPI::decode($requestBuf);
assert($decodeRet['status'] == 0);

if($decodeRet['status'] !== 0) {
    echo "error";
} else {
    $respBuf = $decodeRet['buf'];

    $map = new \TARS_MAP(\TARS::UINT32,\TARS::DOUBLE);
    $out = \TUPAPI::getMap("map", $map,$respBuf);

    if ($data == $out) {
        echo "success";
    }
}

?>
--EXPECT--
success