<?php

namespace App\Server\Servant\classes;

class NestedStruct extends \TARS_Struct {
	const CODE = 0;
	const NESTEDSTRUCT = 1;
	const STRUCTLIST = 2;
	const STRUCTMAP = 3;
	const EXT = 9;


	public $code; 
	public $nestedStruct; 
	public $structList; 
	public $structMap; 
	public $ext; 


	protected static $fields = array(
		self::CODE => array(
			'name'=>'code',
			'required'=>true,
			'type'=>\TARS::INT32,
			),
		self::NESTEDSTRUCT => array(
			'name'=>'nestedStruct',
			'required'=>true,
			'type'=>\TARS::STRUCT,
			),
		self::STRUCTLIST => array(
			'name'=>'structList',
			'required'=>true,
			'type'=>\TARS::VECTOR,
			),
		self::STRUCTMAP => array(
			'name'=>'structMap',
			'required'=>true,
			'type'=>\TARS::MAP,
			),
		self::EXT => array(
			'name'=>'ext',
			'required'=>false,
			'type'=>\TARS::MAP,
			),
	);

	public function __construct() {
		parent::__construct('App_Server_Servant_NestedStruct', self::$fields);
		$this->nestedStruct = new SimpleStruct();
		$this->structList = new \TARS_VECTOR(new SimpleStruct());
		$this->structMap = new \TARS_MAP(\TARS::STRING,new SimpleStruct());
		$this->ext = new \TARS_MAP(\TARS::STRING,new \TARS_VECTOR(new SimpleStruct()));
	}
}
