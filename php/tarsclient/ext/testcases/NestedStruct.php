<?php


class NestedStruct extends \TARS_Struct {
	const NESTEDSTRUCT = 1;
	const STRUCTLIST = 2;
	const STRUCTMAP = 3;
	const EXT = 9;
    const VECSTRING = 10;
    const MAPSTRING = 11;


	public $nestedStruct; 
	public $structList; 
	public $structMap; 
	public $ext;
    public $vecstring;
    public $mapstring;


	protected static $fields = array(
		self::NESTEDSTRUCT => array(
			'name'=>'nestedStruct',
			'required'=>1,
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
			'required'=>0,
			'type'=>\TARS::MAP,
			),
        self::VECSTRING => array(
            'name'=>'vecstring',
            'required'=>false,
            'type'=>\TARS::VECTOR,
        ),
        self::MAPSTRING => array(
            'name'=>'mapstring',
            'required'=>false,
            'type'=>\TARS::MAP,
        ),
	);

	public function __construct() {
		parent::__construct('App_Server_Servant.NestedStruct', self::$fields);
		$this->nestedStruct = new SimpleStruct();
		$this->structList = new \TARS_VECTOR(new SimpleStruct());
		$this->structMap = new \TARS_MAP(\TARS::STRING,new SimpleStruct());
		$this->ext = new \TARS_MAP(\TARS::STRING,new \TARS_VECTOR(new SimpleStruct()));
	    $this->vecstring = new \TARS_Vector(TARS::STRING);
        $this->mapstring = new \TARS_Map(\TARS::STRING,TARS::STRING);
	}
}
