<?php

namespace Tars\monitor\classes;

class StatMicMsgHead extends \TARS_Struct
{
    const MASTERNAME = 0;
    const SLAVENAME = 1;
    const INTERFACENAME = 2;
    const MASTERIP = 3;
    const SLAVEIP = 4;
    const SLAVEPORT = 5;
    const RETURNVALUE = 6;
    const SLAVESETNAME = 7;
    const SLAVESETAREA = 8;
    const SLAVESETID = 9;
    const TARSVERSION = 10;

    public $masterName;
    public $slaveName;
    public $interfaceName;
    public $masterIp;
    public $slaveIp;
    public $slavePort;
    public $returnValue;
    public $slaveSetName;
    public $slaveSetArea;
    public $slaveSetID;
    public $tarsVersion;

    protected static $_fields = array(
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
        self::SLAVEPORT => array(
            'name' => 'slavePort',
            'required' => true,
            'type' => \TARS::INT32,
            ),
        self::RETURNVALUE => array(
            'name' => 'returnValue',
            'required' => true,
            'type' => \TARS::INT32,
            ),
        self::SLAVESETNAME => array(
            'name' => 'slaveSetName',
            'required' => false,
            'type' => \TARS::STRING,
            ),
        self::SLAVESETAREA => array(
            'name' => 'slaveSetArea',
            'required' => false,
            'type' => \TARS::STRING,
            ),
        self::SLAVESETID => array(
            'name' => 'slaveSetID',
            'required' => false,
            'type' => \TARS::STRING,
            ),
        self::TARSVERSION => array(
            'name' => 'tarsVersion',
            'required' => false,
            'type' => \TARS::STRING,
            ),
    );

    public function __construct()
    {
        parent::__construct('tars_tarsstat_StatObj_StatMicMsgHead', self::$_fields);
    }
}
