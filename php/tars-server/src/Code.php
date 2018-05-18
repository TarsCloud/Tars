<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/3/4
 * Time: 下午4:09.
 */

namespace Tars;

class Code
{
    // taf服务端可能返回的错误码
    const TARSSERVERSUCCESS = 0; //服务器端处理成功
    const TARSSERVERDECODEERR = -1; //服务器端解码异常
    const TARSSERVERENCODEERR = -2; //服务器端编码异常
    const TARSSERVERNOFUNCERR = -3; //服务器端没有该函数
    const TARSSERVERNOSERVANTERR = -4; //服务器端五该Servant对象
    const TARSSERVERRESETGRID = -5; //服务器端灰度状态不一致
    const TARSSERVERQUEUETIMEOUT = -6; //服务器队列超过限制
    const TARSASYNCCALLTIMEOUT = -7; //异步调用超时
    const TARSPROXYCONNECTERR = -8; //proxy链接异常
    const TARSSERVEROVERLOAD = -9; //服务器端超负载,超过队列长度
    const TARSADAPTERNULL = -10; //客户端选路为空，服务不存在或者所有服务down掉了
    const TARSINVOKEBYINVALIDESET = -11; //客户端按set规则调用非法
    const TARSCLIENTDECODEERR = -12; //客户端解码异常

    const TARSSERVERUNKNOWNERR = -99; //服务器端未知异常

    public static $msgs = [
        self::TARSSERVERSUCCESS => '服务器端处理成功',
        self::TARSSERVERDECODEERR => '服务器端解码异常',
        self::TARSSERVERENCODEERR => '服务器端编码异常',
        self::TARSSERVERNOFUNCERR => '服务器端没有该函数',
        self::TARSSERVERNOSERVANTERR => '服务器端无该Servant对象',
        self::TARSSERVERRESETGRID => '服务器端灰度状态不一致',
        self::TARSSERVERQUEUETIMEOUT => '服务器队列超过限制',
        self::TARSASYNCCALLTIMEOUT => '异步调用超时',
        self::TARSPROXYCONNECTERR => 'proxy链接异常',
        self::TARSSERVERUNKNOWNERR => '服务器端未知异常',
        self::TARSSERVEROVERLOAD => '服务器端超负载,超过队列长度',
        self::TARSADAPTERNULL => '客户端选路为空，服务不存在或者所有服务down掉了',
        self::TARSINVOKEBYINVALIDESET => '客户端按set规则调用非法',
        self::TARSCLIENTDECODEERR => '客户端解码异常',
    ];

    public static function getMsg($code)
    {
        return self::$msgs[$code];
    }
}
