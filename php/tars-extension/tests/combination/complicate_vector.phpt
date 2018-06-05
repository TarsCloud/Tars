--TEST--
vector: complicate vector

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


$vector = new \TARS_VECTOR(new SimpleStruct());

$vector->pushBack($simpleStruct1);
$vector->pushBack($simpleStruct2);


$buf = \TUPAPI::putVector("vec",$vector);

$encodeBufs['vec'] = $buf;

$requestBuf = \TUPAPI::encode($iVersion, $iRequestId, $servantName, $funcName, $cPacketType, $iMessageType, $iTimeout, $contexts,$statuses,$encodeBufs);

$decodeRet = \TUPAPI::decode($requestBuf);
assert($decodeRet['status'] == 0);

if($decodeRet['status'] !== 0) {
    echo "error";
} else {
    $respBuf = $decodeRet['buf'];
    $v = new \TARS_VECTOR(new SimpleStruct());
    $out = \TUPAPI::getVector("vec",$v,$respBuf);

    $data = [
        [
            'id' => 1,
            'count' => 2,
            'page' => 1,
        ],
        [
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