<?php

namespace TimerServer\servant\PHPTest\PHPServer\obj\classes;

class ComplicatedStruct extends \TARS_Struct
{
    const SS = 0;
    const RS = 1;
    const MSS = 2;
    const STR = 3;

    public $ss;
    public $rs;
    public $mss;
    public $str;

    protected static $_fields = array(
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
        self::STR => array(
            'name' => 'str',
            'required' => false,
            'type' => \TARS::STRING,
            ),
    );

    public function __construct()
    {
        parent::__construct('PHPTest_PHPServer_obj_ComplicatedStruct', self::$_fields);
        $this->rs = new SimpleStruct();
        $this->ss = new \TARS_Vector(new SimpleStruct());
        $this->mss = new \TARS_Map(\TARS::STRING, new SimpleStruct());
    }
}
