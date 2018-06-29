<?php

namespace Tars\config\classes;

class GetConfigListInfo extends \TARS_Struct
{
    const APPNAME = 0;
    const SERVERNAME = 1;
    const BAPPONLY = 2;
    const HOST = 3;
    const SETDIVISION = 4;
    const CONTAINERNAME = 5;

    public $appname;
    public $servername;
    public $bAppOnly = false;
    public $host = '';
    public $setdivision = '';
    public $containername = '';

    protected static $_fields = array(
        self::APPNAME => array(
            'name' => 'appname',
            'required' => true,
            'type' => \TARS::STRING,
            ),
        self::SERVERNAME => array(
            'name' => 'servername',
            'required' => false,
            'type' => \TARS::STRING,
            ),
        self::BAPPONLY => array(
            'name' => 'bAppOnly',
            'required' => false,
            'type' => \TARS::BOOL,
            ),
        self::HOST => array(
            'name' => 'host',
            'required' => false,
            'type' => \TARS::STRING,
            ),
        self::SETDIVISION => array(
            'name' => 'setdivision',
            'required' => false,
            'type' => \TARS::STRING,
            ),
        self::CONTAINERNAME => array(
            'name' => 'containername',
            'required' => false,
            'type' => \TARS::STRING,
            ),
    );

    public function __construct()
    {
        parent::__construct('tars_tarsconfig_ConfigObj_GetConfigListInfo', self::$_fields);
    }
}
