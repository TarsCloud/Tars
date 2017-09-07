<?php

namespace App\Server\Servant\classes;

class MiddleStruct extends \TARS_Struct {
	const ID = 0;
	const IP = 14;
	const DEVICEUID = 15;
	const CREATETIME = 17;


	public $id; 
	public $ip; 
	public $deviceUid; 
	public $createTime; 


	protected static $fields = array(
		self::ID => array(
			'name'=>'id',
			'required'=>true,
			'type'=>\TARS::INT64,
			),
		self::IP => array(
			'name'=>'ip',
			'required'=>true,
			'type'=>\TARS::STRING,
			),
		self::DEVICEUID => array(
			'name'=>'deviceUid',
			'required'=>true,
			'type'=>\TARS::MAP,
			),
		self::CREATETIME => array(
			'name'=>'createTime',
			'required'=>false,
			'type'=>\TARS::VECTOR,
			),
	);

	public function __construct() {
		parent::__construct('App_Server_Servant_MiddleStruct', self::$fields);
		$this->deviceUid = new \TARS_MAP(\TARS::STRING,\TARS::STRING);
		$this->createTime = new \TARS_VECTOR(\TARS::INT64);
	}
}
