--TEST--
map: uint16 & float

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

$data = [123 => 1.1];
$map = new \TARS_MAP(\TARS::UINT16, \TARS::FLOAT);
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

    $map = new \TARS_MAP(\TARS::UINT16,\TARS::FLOAT);
    $out = \TUPAPI::getMap("map", $map,$respBuf);

    $sData = [123 => 1.1];
    $sKey = key($sData);
    $sValue = $sData[$sKey];
    $rKey = key($out);
    $rValue = $out[$rKey];
    if ($sKey == $rKey && (floatComp($sValue, $rValue, 7) == 0)) {
        echo "success";
    }
}

?>
--EXPECT--
success