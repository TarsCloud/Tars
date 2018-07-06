<?php

namespace TimerServer\servant\PHPTest\PHPServer\obj\classes;

class LotofTags extends \TARS_Struct
{
    const ID = 0;
    const COUNT = 1;
    const PAGE = 2;
    const STR = 3;
    const SS = 4;
    const RS = 5;
    const MSS = 6;

    public $id;
    public $count;
    public $page;
    public $str;
    public $ss;
    public $rs;
    public $mss;

    protected static $_fields = array(
        self::ID => array(
            'name' => 'id',
            'required' => true,
            'type' => \TARS::INT64,
            ),
        self::COUNT => array(
            'name' => 'count',
            'required' => true,
            'type' => \TARS::INT32,
            ),
        self::PAGE => array(
            'name' => 'page',
            'required' => true,
            'type' => \TARS::SHORT,
            ),
        self::STR => array(
            'name' => 'str',
            'required' => false,
            'type' => \TARS::STRING,
            ),
        self::SS => array(
            'name' => 'ss',
            'required' => true,
            'type' => \TARS::VECTOR,
            ),
        self::RS => array(
            'name' => 'rs',
            'required' => true,
            'type' => \TARS::STRUCT,
            ),
        self::MSS => array(
            'name' => 'mss',
            'required' => true,
            'type' => \TARS::MAP,
            ),
    );

    public function __construct()
    {
        parent::__construct('PHPTest_PHPServer_obj_LotofTags', self::$_fields);
        $this->rs = new SimpleStruct();
        $this->ss = new \TARS_Vector(new SimpleStruct());
        $this->mss = new \TARS_Map(\TARS::STRING, new SimpleStruct());
    }
}
