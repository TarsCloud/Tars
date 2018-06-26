<?php

namespace Tars\report;

class ServerInfo extends \TARS_Struct
{
    const APPLICATION = 0;
    const SERVERNAME = 1;
    const PID = 2;
    const ADAPTER = 3;

    public $application;
    public $serverName;
    public $pid;
    public $adapter;

    protected static $fields = array(
        self::APPLICATION => array(
            'name' => 'application',
            'required' => true,
            'type' => \TARS::STRING,
            ),
        self::SERVERNAME => array(
            'name' => 'serverName',
            'required' => true,
            'type' => \TARS::STRING,
            ),
        self::PID => array(
            'name' => 'pid',
            'required' => true,
            'type' => \TARS::INT32,
            ),
        self::ADAPTER => array(
            'name' => 'adapter',
            'required' => false,
            'type' => \TARS::STRING,
            ),
    );

    public function __construct()
    {
        parent::__construct('tars_tarsnode_ServerObj_ServerInfo', self::$fields);
    }
}
