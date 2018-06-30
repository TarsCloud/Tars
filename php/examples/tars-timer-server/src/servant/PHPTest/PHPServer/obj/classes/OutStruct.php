<?php

namespace TimerServer\servant\PHPTest\PHPServer\obj\classes;

class OutStruct extends \TARS_Struct
{
    const ID = 0;
    const COUNT = 1;
    const PAGE = 2;
    const STR = 3;

    public $id;
    public $count;
    public $page;
    public $str;

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
    );

    public function __construct()
    {
        parent::__construct('PHPTest_PHPServer_obj_OutStruct', self::$_fields);
    }
}
