<?php

namespace Tars\monitor\classes;

class StatSampleMsg extends \TARS_Struct
{
    const UNID = 0;
    const MASTERNAME = 1;
    const SLAVENAME = 2;
    const INTERFACENAME = 3;
    const MASTERIP = 4;
    const SLAVEIP = 5;
    const DEPTH = 6;
    const WIDTH = 7;
    const PARENTWIDTH = 8;

    public $unid;
    public $masterName;
    public $slaveName;
    public $interfaceName;
    public $masterIp;
    public $slaveIp;
    public $depth;
    public $width;
    public $parentWidth;

    protected static $_fields = array(
        self::UNID => array(
            'name' => 'unid',
            'required' => true,
            'type' => \TARS::STRING,
            ),
        self::MASTERNAME => array(
            'name' => 'masterName',
            'required' => true,
            'type' => \TARS::STRING,
            ),
        self::SLAVENAME => array(
            'name' => 'slaveName',
            'required' => true,
            'type' => \TARS::STRING,
            ),
        self::INTERFACENAME => array(
            'name' => 'interfaceName',
            'required' => true,
            'type' => \TARS::STRING,
            ),
        self::MASTERIP => array(
            'name' => 'masterIp',
            'required' => true,
            'type' => \TARS::STRING,
            ),
        self::SLAVEIP => array(
            'name' => 'slaveIp',
            'required' => true,
            'type' => \TARS::STRING,
            ),
        self::DEPTH => array(
            'name' => 'depth',
            'required' => true,
            'type' => \TARS::INT32,
            ),
        self::WIDTH => array(
            'name' => 'width',
            'required' => true,
            'type' => \TARS::INT32,
            ),
        self::PARENTWIDTH => array(
            'name' => 'parentWidth',
            'required' => true,
            'type' => \TARS::INT32,
            ),
    );

    public function __construct()
    {
        parent::__construct('tars_tarsstat_StatObj_StatSampleMsg', self::$_fields);
    }
}
