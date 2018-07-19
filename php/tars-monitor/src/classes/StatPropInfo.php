<?php

namespace Tars\monitor\classes;

class StatPropInfo extends \TARS_Struct
{
    const POLICY = 0;
    const VALUE = 1;

    public $policy;
    public $value;

    protected static $_fields = array(
        self::POLICY => array(
            'name' => 'policy',
            'required' => true,
            'type' => \TARS::STRING,
            ),
        self::VALUE => array(
            'name' => 'value',
            'required' => true,
            'type' => \TARS::STRING,
            ),
    );

    public function __construct()
    {
        parent::__construct('tars_tarsproperty_PropertyObj_StatPropInfo', self::$_fields);
    }
}
