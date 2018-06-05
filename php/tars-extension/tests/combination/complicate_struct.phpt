--TEST--
map: complicate struct

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
require_once __DIR__ . "/../include/NestedStruct.php";
require_once __DIR__ . "/../include/functions.php";

$simpleStruct1 = new SimpleStruct();
$simpleStruct1->id = 1;
$simpleStruct1->count = 2;

$nestedStruct = new NestedStruct();
$nestedStruct->structMap->pushBack(["test1"=>$simpleStruct1]);
$nestedStruct->structMap->pushBack(["test4"=>$simpleStruct1]);

$nestedStruct->structList->pushBack($simpleStruct1);
$nestedStruct->structList->pushBack($simpleStruct1);

$nestedStruct->nestedStruct = $simpleStruct1;

$structList = new \TARS_VECTOR(new SimpleStruct());
$structList->pushBack($simpleStruct1);

$nestedStruct->ext->pushBack(["test2" => $structList]);
$nestedStruct->ext->pushBack(["test3" => $structList]);

$nestedStruct->vecstruct->pushBack($simpleStruct1);

$nestedStruct->vecchar->pushBack("a");

$buf = \TUPAPI::putStruct("struct",$nestedStruct);

$encodeBufs['struct'] = $buf;

$requestBuf = \TUPAPI::encode($iVersion, $iRequestId, $servantName, $funcName, $cPacketType, $iMessageType, $iTimeout, $contexts,$statuses,$encodeBufs);

$decodeRet = \TUPAPI::decode($requestBuf);
assert($decodeRet['status'] == 0);

if($decodeRet['status'] !== 0) {
    echo "error";
} else {
    $respBuf = $decodeRet['buf'];
    $nestedStructOut = new NestedStruct();
    $result = \TUPAPI::getStruct("struct", $nestedStructOut,$respBuf);
    fromArray($result, $nestedStructOut);

    if ($simpleStruct1 == $nestedStructOut->nestedStruct) {
        echo "success";
    }
}

?>
--EXPECT--
success