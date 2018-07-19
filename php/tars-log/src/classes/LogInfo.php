<?php

namespace Tars\log\classes;

class LogInfo extends \TARS_Struct
{
    const APPNAME = 0;
    const SERVERNAME = 1;
    const SFILENAME = 2;
    const SFORMAT = 3;
    const SETDIVISION = 4;
    const BHASSUFIX = 5;
    const BHASAPPNAMEPREFIX = 6;
    const BHASSQUAREBRACKET = 7;
    const SCONCATSTR = 8;
    const SSEPAR = 9;
    const SLOGTYPE = 10;

    public $appname;
    public $servername;
    public $sFilename;
    public $sFormat;
    public $setdivision;
    public $bHasSufix = true;
    public $bHasAppNamePrefix = true;
    public $bHasSquareBracket = false;
    public $sConcatStr = '_';
    public $sSepar = '|';
    public $sLogType = '';

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
        self::SFILENAME => array(
            'name' => 'sFilename',
            'required' => true,
            'type' => \TARS::STRING,
            ),
        self::SFORMAT => array(
            'name' => 'sFormat',
            'required' => true,
            'type' => \TARS::STRING,
            ),
        self::SETDIVISION => array(
            'name' => 'setdivision',
            'required' => false,
            'type' => \TARS::STRING,
            ),
        self::BHASSUFIX => array(
            'name' => 'bHasSufix',
            'required' => false,
            'type' => \TARS::BOOL,
            ),
        self::BHASAPPNAMEPREFIX => array(
            'name' => 'bHasAppNamePrefix',
            'required' => false,
            'type' => \TARS::BOOL,
            ),
        self::BHASSQUAREBRACKET => array(
            'name' => 'bHasSquareBracket',
            'required' => false,
            'type' => \TARS::BOOL,
            ),
        self::SCONCATSTR => array(
            'name' => 'sConcatStr',
            'required' => false,
            'type' => \TARS::STRING,
            ),
        self::SSEPAR => array(
            'name' => 'sSepar',
            'required' => false,
            'type' => \TARS::STRING,
            ),
        self::SLOGTYPE => array(
            'name' => 'sLogType',
            'required' => false,
            'type' => \TARS::STRING,
            ),
    );

    public function __construct()
    {
        parent::__construct('tars_tarslog_LogObj_LogInfo', self::$_fields);
    }
}
