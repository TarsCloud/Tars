<?php

namespace Tars\monitor\classes;

class StatPropMsgHead extends \TARS_Struct
{
    const MODULENAME = 0;
    const IP = 1;
    const PROPERTYNAME = 2;
    const SETNAME = 3;
    const SETAREA = 4;
    const SETID = 5;
    const SCONTAINER = 6;
    const IPROPERTYVER = 7;

    public $moduleName;
    public $ip;
    public $propertyName;
    public $setName;
    public $setArea;
    public $setID;
    public $sContainer;
    public $iPropertyVer = 1;

    protected static $_fields = array(
        self::MODULENAME => array(
            'name' => 'moduleName',
            'required' => true,
            'type' => \TARS::STRING,
            ),
        self::IP => array(
            'name' => 'ip',
            'required' => true,
            'type' => \TARS::STRING,
            ),
        self::PROPERTYNAME => array(
            'name' => 'propertyName',
            'required' => true,
            'type' => \TARS::STRING,
            ),
        self::SETNAME => array(
            'name' => 'setName',
            'required' => false,
            'type' => \TARS::STRING,
            ),
        self::SETAREA => array(
            'name' => 'setArea',
            'required' => false,
            'type' => \TARS::STRING,
            ),
        self::SETID => array(
            'name' => 'setID',
            'required' => false,
            'type' => \TARS::STRING,
            ),
        self::SCONTAINER => array(
            'name' => 'sContainer',
            'required' => false,
            'type' => \TARS::STRING,
            ),
        self::IPROPERTYVER => array(
            'name' => 'iPropertyVer',
            'required' => false,
            'type' => \TARS::INT32,
            ),
    );

    public function __construct()
    {
        parent::__construct('tars_tarsproperty_PropertyObj_StatPropMsgHead', self::$_fields);
    }
}
