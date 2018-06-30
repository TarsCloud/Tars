<?php

namespace Tars\monitor\classes;

class StatMicMsgBody extends \TARS_Struct
{
    const COUNT = 0;
    const TIMEOUTCOUNT = 1;
    const EXECCOUNT = 2;
    const INTERVALCOUNT = 3;
    const TOTALRSPTIME = 4;
    const MAXRSPTIME = 5;
    const MINRSPTIME = 6;

    public $count;
    public $timeoutCount;
    public $execCount;
    public $intervalCount;
    public $totalRspTime;
    public $maxRspTime;
    public $minRspTime;

    protected static $_fields = array(
        self::COUNT => array(
            'name' => 'count',
            'required' => true,
            'type' => \TARS::INT32,
            ),
        self::TIMEOUTCOUNT => array(
            'name' => 'timeoutCount',
            'required' => true,
            'type' => \TARS::INT32,
            ),
        self::EXECCOUNT => array(
            'name' => 'execCount',
            'required' => true,
            'type' => \TARS::INT32,
            ),
        self::INTERVALCOUNT => array(
            'name' => 'intervalCount',
            'required' => true,
            'type' => \TARS::MAP,
            ),
        self::TOTALRSPTIME => array(
            'name' => 'totalRspTime',
            'required' => true,
            'type' => \TARS::INT64,
            ),
        self::MAXRSPTIME => array(
            'name' => 'maxRspTime',
            'required' => true,
            'type' => \TARS::INT32,
            ),
        self::MINRSPTIME => array(
            'name' => 'minRspTime',
            'required' => true,
            'type' => \TARS::INT32,
            ),
    );

    public function __construct()
    {
        parent::__construct('tars_tarsstat_StatObj_StatMicMsgBody', self::$_fields);
        $this->intervalCount = new \TARS_Map(\TARS::INT32, \TARS::INT32);
    }
}
