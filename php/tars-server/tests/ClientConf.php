<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/4/16
 * Time: 下午6:18.
 */
class ClientConf
{
    private static $_swooleTable;

    const SWOOLE_TABLE_SET_FAILED = -1001;
    const SWOOLE_TABLE_GET_FAILED = -1002;

    const KEY = 'taf_swoole_key';

    private function __construct()
    {
        //100个服务,每个长度30 需要3000个字节,这里申请64k
        self::$_swooleTable = new \swoole_table(65536);
        self::$_swooleTable->column('locator', \swoole_table::TYPE_STRING, 200);
        self::$_swooleTable->column('sync-invoke-timeout', \swoole_table::TYPE_INT, 4);
        self::$_swooleTable->column('async-invoke-timeout', \swoole_table::TYPE_INT, 4);
        self::$_swooleTable->column('refresh-endpoint-interval', \swoole_table::TYPE_INT, 4);
        self::$_swooleTable->column('stat', \swoole_table::TYPE_STRING, 50);
        self::$_swooleTable->column('property', \swoole_table::TYPE_STRING, 50);
        self::$_swooleTable->column('report-interval', \swoole_table::TYPE_INT, 4);
        self::$_swooleTable->column('sendthread', \swoole_table::TYPE_INT, 4);
        self::$_swooleTable->column('recvthread', \swoole_table::TYPE_INT, 4);
        self::$_swooleTable->column('asyncthread', \swoole_table::TYPE_INT, 4);
        self::$_swooleTable->column('modulename', \swoole_table::TYPE_STRING, 50);
        self::$_swooleTable->column('sample-rate', \swoole_table::TYPE_INT, 4);
        self::$_swooleTable->column('max-sample-count', \swoole_table::TYPE_INT, 4);
        self::$_swooleTable->column('propertyplus', \swoole_table::TYPE_STRING, 50);
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

    public static function init($clientConf)
    {
        self::getInstance()->set(self::KEY, $clientConf);
    }

    public static function get($key)
    {
        $data = self::getInstance()->get(self::KEY);

        return $data[$key];
    }
}
