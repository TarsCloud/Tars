--TEST--
map: vector in map with key obj

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

$keyS = new SimpleStruct();
$keyS->id = 1;
$keyS->count = 2;

$valS = new SimpleStruct();
$valS->id = 11;
$valS->count = 12;

$map = new \TARS_MAP(new SimpleStruct(), new SimpleStruct(), 1);
$map->pushBack(['key' => $keyS, 'value' => $valS]);

$buf = \TUPAPI::putMap("map",$map);

$encodeBufs['map'] = $buf;

$requestBuf = \TUPAPI::encode($iVersion, $iRequestId, $servantName, $funcName, $cPacketType, $iMessageType, $iTimeout, $contexts,$statuses,$encodeBufs);

$decodeRet = \TUPAPI::decode($requestBuf);
assert($decodeRet['status'] == 0);

if($decodeRet['status'] !== 0) {
    echo "error";
} else {
    $respBuf = $decodeRet['buf'];

    $map = new \TARS_MAP(new SimpleStruct(), new SimpleStruct(), 1);
    $out = \TUPAPI::getMap("map", $map, $respBuf);

    $retKeyS = new SimpleStruct();
    $retValS = new SimpleStruct();
    fromArray($out[0]['key'], $retKeyS);
    fromArray($out[0]['value'], $retValS);

    if ($retKeyS == $keyS && $retValS == $valS) {
        echo "success";
    }
}

?>
--EXPECT--
success