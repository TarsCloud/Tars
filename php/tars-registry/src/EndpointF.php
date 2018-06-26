<?php

namespace Tars\registry;

class EndpointF extends \TARS_Struct
{
    const HOST = 0;
    const PORT = 1;
    const TIMEOUT = 2;
    const ISTCP = 3;
    const GRID = 4;
    const GROUPWORKID = 5;
    const GROUPREALID = 6;
    const SETID = 7;
    const QOS = 8;
    const BAKFLAG = 9;
    const GRIDFLAG = 10;
    const WEIGHT = 11;
    const WEIGHTTYPE = 12;
    const CPULOAD = 13;
    const SAMPLETIME = 14;
    const CONTAINERNAME = 15;
    const GRAYFLAG = 16;

    public $host;
    public $port;
    public $timeout;
    public $istcp;
    public $grid;
    public $groupworkid;
    public $grouprealid;
    public $setId;
    public $qos;
    public $bakFlag;
    public $gridFlag;
    public $weight;
    public $weightType;
    public $cpuload;
    public $sampletime;
    public $containerName;
    public $grayFlag;

    protected static $fields = array(
        self::HOST => array(
            'name' => 'host',
            'required' => true,
            'type' => \TARS::STRING,
            ),
        self::PORT => array(
            'name' => 'port',
            'required' => true,
            'type' => \TARS::INT32,
            ),
        self::TIMEOUT => array(
            'name' => 'timeout',
            'required' => true,
            'type' => \TARS::INT32,
            ),
        self::ISTCP => array(
            'name' => 'istcp',
            'required' => true,
            'type' => \TARS::INT32,
            ),
        self::GRID => array(
            'name' => 'grid',
            'required' => true,
            'type' => \TARS::INT32,
            ),
        self::GROUPWORKID => array(
            'name' => 'groupworkid',
            'required' => false,
            'type' => \TARS::INT32,
            ),
        self::GROUPREALID => array(
            'name' => 'grouprealid',
            'required' => false,
            'type' => \TARS::INT32,
            ),
        self::SETID => array(
            'name' => 'setId',
            'required' => false,
            'type' => \TARS::STRING,
            ),
        self::QOS => array(
            'name' => 'qos',
            'required' => false,
            'type' => \TARS::INT32,
            ),
        self::BAKFLAG => array(
            'name' => 'bakFlag',
            'required' => false,
            'type' => \TARS::INT32,
            ),
        self::GRIDFLAG => array(
            'name' => 'gridFlag',
            'required' => false,
            'type' => \TARS::INT32,
            ),
        self::WEIGHT => array(
            'name' => 'weight',
            'required' => false,
            'type' => \TARS::INT32,
            ),
        self::WEIGHTTYPE => array(
            'name' => 'weightType',
            'required' => false,
            'type' => \TARS::INT32,
            ),
        self::CPULOAD => array(
            'name' => 'cpuload',
            'required' => false,
            'type' => \TARS::INT32,
            ),
        self::SAMPLETIME => array(
            'name' => 'sampletime',
            'required' => false,
            'type' => \TARS::INT64,
            ),
        self::CONTAINERNAME => array(
            'name' => 'containerName',
            'required' => false,
            'type' => \TARS::STRING,
            ),
        self::GRAYFLAG => array(
            'name' => 'grayFlag',
            'required' => false,
            'type' => \TARS::INT32,
            ),
    );

    public function __construct()
    {
        parent::__construct('tars_tarsregistry_QueryObj_EndpointF', self::$fields);
    }
}
