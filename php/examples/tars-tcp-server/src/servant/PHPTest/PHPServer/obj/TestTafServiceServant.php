<?php

namespace Server\servant\PHPTest\PHPServer\obj;

use Server\servant\PHPTest\PHPServer\obj\classes\LotofTags;
use Server\servant\PHPTest\PHPServer\obj\classes\SimpleStruct;
use Server\servant\PHPTest\PHPServer\obj\classes\OutStruct;
use Server\servant\PHPTest\PHPServer\obj\classes\ComplicatedStruct;
interface TestTafServiceServant {
	/**
	 * @return void
	 */
	public function testTafServer();
	/**
	 * @param struct $tags \Server\servant\PHPTest\PHPServer\obj\classes\LotofTags
	 * @param struct $outtags \Server\servant\PHPTest\PHPServer\obj\classes\LotofTags =out=
	 * @return int
	 */
	public function testLofofTags(LotofTags $tags,LotofTags &$outtags);
	/**
	 * @param string $name 
	 * @param string $outGreetings =out=
	 * @return void
	 */
	public function sayHelloWorld($name,&$outGreetings);
	/**
	 * @param bool $a 
	 * @param int $b 
	 * @param string $c 
	 * @param bool $d =out=
	 * @param int $e =out=
	 * @param string $f =out=
	 * @return int
	 */
	public function testBasic($a,$b,$c,&$d,&$e,&$f);
	/**
	 * @param long $a 
	 * @param struct $b \Server\servant\PHPTest\PHPServer\obj\classes\SimpleStruct
	 * @param struct $d \Server\servant\PHPTest\PHPServer\obj\classes\OutStruct =out=
	 * @return string
	 */
	public function testStruct($a,SimpleStruct $b,OutStruct &$d);
	/**
	 * @param short $a 
	 * @param struct $b \Server\servant\PHPTest\PHPServer\obj\classes\SimpleStruct
	 * @param map $m1 \TARS_Map(\TARS::STRING,\TARS::STRING)
	 * @param struct $d \Server\servant\PHPTest\PHPServer\obj\classes\OutStruct =out=
	 * @param map $m2 \TARS_Map(\TARS::INT32,\Server\servant\PHPTest\PHPServer\obj\classes\SimpleStruct) =out=
	 * @return string
	 */
	public function testMap($a,SimpleStruct $b,$m1,OutStruct &$d,&$m2);
	/**
	 * @param int $a 
	 * @param vector $v1 \TARS_Vector(\TARS::STRING)
	 * @param vector $v2 \TARS_Vector(\Server\servant\PHPTest\PHPServer\obj\classes\SimpleStruct)
	 * @param vector $v3 \TARS_Vector(\TARS::INT32) =out=
	 * @param vector $v4 \TARS_Vector(\Server\servant\PHPTest\PHPServer\obj\classes\OutStruct) =out=
	 * @return string
	 */
	public function testVector($a,$v1,$v2,&$v3,&$v4);
	/**
	 * @return struct \Server\servant\PHPTest\PHPServer\obj\classes\SimpleStruct
	 */
	public function testReturn();
	/**
	 * @return map \TARS_Map(\TARS::STRING,\TARS::STRING)
	 */
	public function testReturn2();
	/**
	 * @param struct $cs \Server\servant\PHPTest\PHPServer\obj\classes\ComplicatedStruct
	 * @param vector $vcs \TARS_Vector(\Server\servant\PHPTest\PHPServer\obj\classes\ComplicatedStruct)
	 * @param struct $ocs \Server\servant\PHPTest\PHPServer\obj\classes\ComplicatedStruct =out=
	 * @param vector $ovcs \TARS_Vector(\Server\servant\PHPTest\PHPServer\obj\classes\ComplicatedStruct) =out=
	 * @return vector \TARS_Vector(\Server\servant\PHPTest\PHPServer\obj\classes\SimpleStruct)
	 */
	public function testComplicatedStruct(ComplicatedStruct $cs,$vcs,ComplicatedStruct &$ocs,&$ovcs);
	/**
	 * @param map $mcs \TARS_Map(\TARS::STRING,\Server\servant\PHPTest\PHPServer\obj\classes\ComplicatedStruct)
	 * @param map $omcs \TARS_Map(\TARS::STRING,\Server\servant\PHPTest\PHPServer\obj\classes\ComplicatedStruct) =out=
	 * @return map \TARS_Map(\TARS::STRING,\Server\servant\PHPTest\PHPServer\obj\classes\ComplicatedStruct)
	 */
	public function testComplicatedMap($mcs,&$omcs);
	/**
	 * @param short $a 
	 * @param bool $b1 =out=
	 * @param int $in2 =out=
	 * @param struct $d \Server\servant\PHPTest\PHPServer\obj\classes\OutStruct =out=
	 * @param vector $v3 \TARS_Vector(\Server\servant\PHPTest\PHPServer\obj\classes\OutStruct) =out=
	 * @param vector $v4 \TARS_Vector(\TARS::INT32) =out=
	 * @return int
	 */
	public function testEmpty($a,&$b1,&$in2,OutStruct &$d,&$v3,&$v4);
	/**
	 * @return int
	 */
	public function testSelf();
	/**
	 * @return int
	 */
	public function testProperty();
}

