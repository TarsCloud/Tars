--TEST--
map: string & string

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

$strings = [["test1"=>1],["test2"=>2]];
$map = new \TARS_MAP(\TARS::STRING,\TARS::STRING);
foreach ($strings as $string) {
    $map->pushBack($string);
}
$buf = \TUPAPI::putMap("map",$map);
$encodeBufs['map'] = $buf;

$requestBuf = \TUPAPI::encode($iVersion, $iRequestId, $servantName, $funcName, $cPacketType, $iMessageType, $iTimeout, $contexts,$statuses,$encodeBufs);

$decodeRet = \TUPAPI::decode($requestBuf);
assert($decodeRet['status'] == 0);

if($decodeRet['status'] !== 0) {
    echo "error";
} else {
    $respBuf = $decodeRet['buf'];

    $map = new \TARS_MAP(\TARS::STRING,\TARS::STRING);
    $out = \TUPAPI::getMap("map", $map,$respBuf);

    $data = ["test1" => "1", "test2" => "2"];

    if ($data == $out) {
        echo "success";
    }
}

?>
--EXPECT--
success