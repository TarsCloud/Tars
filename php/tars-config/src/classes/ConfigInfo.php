<?php

namespace Tars\config\classes;

class ConfigInfo extends \TARS_Struct
{
    const APPNAME = 0;
    const SERVERNAME = 1;
    const FILENAME = 2;
    const BAPPONLY = 3;
    const HOST = 4;
    const SETDIVISION = 5;

    public $appname;
    public $servername;
    public $filename;
    public $bAppOnly = false;
    public $host;
    public $setdivision;

    protected static $_fields = array(
        self::APPNAME => array(
            'name' => 'appname',
            'required' => true,
            'type' => \TARS::STRING,
            ),
        self::SERVERNAME => array(
            'name' => 'servername',
            'required' => true,
            'type' => \TARS::STRING,
            ),
        self::FILENAME => array(
            'name' => 'filename',
            'required' => true,
            'type' => \TARS::STRING,
            ),
        self::BAPPONLY => array(
            'name' => 'bAppOnly',
            'required' => true,
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
    );

    public function __construct()
    {
        parent::__construct('tars_tarsconfig_ConfigObj_ConfigInfo', self::$_fields);
    }
}
