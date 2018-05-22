<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/4/16
 * Time: 下午6:18.
 */

namespace Tars;

class Conf
{
    private static $_swooleTable;

    const SWOOLE_TABLE_SET_FAILED = -1001;
    const SWOOLE_TABLE_GET_FAILED = -1002;

    const KEY = 'tars:php:tarsConf';

    private function __construct()
    {
        self::$_swooleTable = new \swoole_table(20000);
        self::$_swooleTable->column('tarsConfig', \swoole_table::TYPE_STRING, 10000);
        self::$_swooleTable->create();
    }

    public static function getInstance()
    {
        if (self::$_swooleTable) {
            return self::$_swooleTable;
        } else {
            new self();

            return self::$_swooleTable;
        }
    }

    public static function init($tarsConf)
    {
        self::getInstance()->set(self::KEY, ['tarsConfig' => serialize($tarsConf)]);
    }

    public static function get()
    {
        $data = self::getInstance()->get(self::KEY);
        $tarsConf = unserialize($data['tarsConfig']);

        return $tarsConf;
    }
}
