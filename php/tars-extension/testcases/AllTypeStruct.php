<?php


class AllTypeStruct extends \TARS_Struct
{
    const BOOL = 0;
    const CHAR = 1;
    const UINT8 = 2;
    const SHORT = 3;
    const UINT16 = 4;
    const FLOAT = 5;
    const DOUBLE = 6;
    const INT32 = 7;
    const UINT32 = 8;
    const INT64 = 9;
    const STRING = 10;
    const VECCHAR = 11;

    public $bool;
    public $char;
    public $uint8;
    public $short;
    public $uint16;
    public $float;
    public $double;
    public $int32;
    public $uint32;
    public $int64;
    public $string;
    public $vecchar;

    protected static $fields = array(
        self::BOOL => array(
            'name' => 'bool',
            'required' => true,
            'type' => \TARS::BOOL,
        ),
        self::CHAR => array(
            'name' => 'char',
            'required' => true,
            'type' => \TARS::CHAR,
        ),
        self::UINT8 => array(
            'name' => 'uint8',
            'required' => true,
            'type' => \TARS::UINT8,
        ),
        self::SHORT => array(
            'name' => 'short',
            'required' => true,
            'type' => \TARS::SHORT,
        ),
        self::UINT16 => array(
            'name' => 'uint16',
            'required' => true,
            'type' => \TARS::UINT16,
        ),
        self::FLOAT => array(
            'name' => 'float',
            'required' => true,
            'type' => \TARS::FLOAT,
        ),
        self::DOUBLE => array(
            'name' => 'double',
            'required' => true,
            'type' => \TARS::DOUBLE,
        ),
        self::INT32 => array(
            'name' => 'int32',
            'required' => true,
            'type' => \TARS::INT32,
        ),
        self::UINT32 => array(
            'name' => 'uint32',
            'required' => true,
            'type' => \TARS::UINT32,
        ),
        self::INT64 => array(
            'name' => 'int64',
            'required' => true,
            'type' => \TARS::INT64,
        ),
        self::STRING => array(
            'name' => 'string',
            'required' => true,
            'type' => \TARS::STRING,
        ),
        self::VECCHAR => array(
            'name' => 'vecchar',
            'required' => true,
            'type' => \TARS::VECTOR,
        ),
    );

    public function __construct()
    {
        parent::__construct('App_Server_Servant.AllTypeStruct', self::$fields);
        $this->vecchar = new \TARS_Vector(\TARS::CHAR);
    }
}
