<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/5/17
 * Time: 下午4:15.
 */

namespace HttpServer\conf;

class ENVConf
{
    public static $locator
        = 'tars.tarsregistry.QueryObj@tcp -h 172.16.0.161 -p 17890';

    public static $socketMode = 2;

    public static function getTarsConf()
    {
        $table = $_SERVER->table;
        $result = $table->get('tars:php:tarsConf');
        $tarsConf = unserialize($result['tarsConfig']);

        return $tarsConf;
    }
}
