--TEST--
vector: map in vector

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

$map = new \TARS_MAP(\TARS::INT32,\TARS::STRING);
$map->pushBack([1 => 'test yong1']);
$map->pushBack([2 => 'test yong2']);

$vector = new \TARS_VECTOR(new \TARS_MAP(\TARS::INT32,\TARS::STRING));

$vector->pushBack($map);


$buf = \TUPAPI::putVector("vec",$vector);

$encodeBufs['vec'] = $buf;

$requestBuf = \TUPAPI::encode($iVersion, $iRequestId, $servantName, $funcName, $cPacketType, $iMessageType, $iTimeout, $contexts,$statuses,$encodeBufs);

$decodeRet = \TUPAPI::decode($requestBuf);
assert($decodeRet['status'] == 0);

if($decodeRet['status'] !== 0) {
    echo "error";
} else {
    $respBuf = $decodeRet['buf'];
    $v = new \TARS_VECTOR(new \TARS_MAP(\TARS::INT32,\TARS::STRING));
    $out = \TUPAPI::getVector("vec",$v,$respBuf);

    $data = [
        [1 => 'test yong1', 2 => 'test yong2'],
    ];

    if ($data == $out) {
        echo "success";
    }
}

?>
--EXPECT--
success