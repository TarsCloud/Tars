--TEST--
map: map in map

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

$map = new \TARS_MAP(\TARS::UINT8, \TARS::BOOL);
$map->pushBack([1 => true]);
$bigMap = new \TARS_MAP(\TARS::SHORT, new \TARS_MAP(\TARS::UINT8, \TARS::BOOL));
$bigMap->pushBack([99 => $map]);

$buf = \TUPAPI::putMap("map", $bigMap);

$encodeBufs['map'] = $buf;

$requestBuf = \TUPAPI::encode($iVersion, $iRequestId, $servantName, $funcName, $cPacketType, $iMessageType, $iTimeout, $contexts,$statuses,$encodeBufs);

$decodeRet = \TUPAPI::decode($requestBuf);
assert($decodeRet['status'] == 0);

if($decodeRet['status'] !== 0) {
    echo "error";
} else {
    $respBuf = $decodeRet['buf'];

    $map = new \TARS_MAP(\TARS::SHORT, new \TARS_MAP(\TARS::UINT8, \TARS::BOOL));
    $out = \TUPAPI::getMap("map", $map,$respBuf);

    $data = [99 => [1 => true]];

    if ($data == $out) {
        echo "success";
    }
}

?>
--EXPECT--
success