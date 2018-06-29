<?php

namespace Tars\monitor\classes;

class StatPropMsgBody extends \TARS_Struct
{
    const VINFO = 0;

    public $vInfo;

    protected static $_fields = array(
        self::VINFO => array(
            'name' => 'vInfo',
            'required' => true,
            'type' => \TARS::VECTOR,
            ),
    );

    public function __construct()
    {
        parent::__construct('tars_tarsproperty_PropertyObj_StatPropMsgBody', self::$_fields);
        $this->vInfo = new \TARS_Vector(new StatPropInfo());
    }
}
