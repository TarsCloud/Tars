--TEST--
map: complicate map

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

$simpleStruct1 = new SimpleStruct();
$simpleStruct1->id = 1;
$simpleStruct1->count = 2;

$simpleStruct2 = new SimpleStruct();
$simpleStruct2->id = 2;
$simpleStruct2->count = 4;


$map = new \TARS_MAP(\TARS::STRING,new SimpleStruct());

$map->pushBack(["test1"=>$simpleStruct1]);
$map->pushBack(["test2"=>$simpleStruct2]);


$buf = \TUPAPI::putMap("map",$map);

$encodeBufs['map'] = $buf;

$requestBuf = \TUPAPI::encode($iVersion, $iRequestId, $servantName, $funcName, $cPacketType, $iMessageType, $iTimeout, $contexts,$statuses,$encodeBufs);

$decodeRet = \TUPAPI::decode($requestBuf);
assert($decodeRet['status'] == 0);

if($decodeRet['status'] !== 0) {
    echo "error";
} else {
    $respBuf = $decodeRet['buf'];
    $m = new \TARS_MAP(\TARS::STRING,new SimpleStruct());
    $out = \TUPAPI::getMap("map",$m,$respBuf);

    $data = [
        'test1' => [
            'id' => 1,
            'count' => 2,
            'page' => 1,
        ],
        'test2' => [
            'id' => 2,
            'count' => 4,
            'page' => 1,
        ],
    ];

    if ($data == $out) {
        echo "success";
    }
}

?>
--EXPECT--
success