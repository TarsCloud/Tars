<?php


class SimpleStruct extends \TARS_Struct
{
    const ID = 0;
    const COUNT = 1;
    const PAGE = 2;

    public $id;
    public $count;
    public $page = 1;

    protected static $fields = array(
        self::ID => array(
            'name' => 'id',
            'required' => true,
            'type' => \TARS::INT64,
            ),
        self::COUNT => array(
            'name' => 'count',
            'required' => true,
            'type' => \TARS::UINT8,
            ),
        self::PAGE => array(
            'name' => 'page',
            'required' => false,
            'type' => \TARS::SHORT,
            ),
    );

    public function __construct()
    {
        parent::__construct('App_Server_Servant.SimpleStruct', self::$fields);
    }
}
