<?php

namespace Tars\monitor\classes;

class ProxyInfo extends \TARS_Struct
{
    const BFROMCLIENT = 0;

    public $bFromClient;

    protected static $_fields = array(
        self::BFROMCLIENT => array(
            'name' => 'bFromClient',
            'required' => true,
            'type' => \TARS::BOOL,
            ),
    );

    public function __construct()
    {
        parent::__construct('tars_tarsstat_StatObj_ProxyInfo', self::$_fields);
    }
}
