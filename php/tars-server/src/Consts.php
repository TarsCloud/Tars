<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/5/2
 * Time: 下午12:29.
 */

namespace Tars;

class Consts
{
    const TARSVERSION = 0x01;
    const TUPVERSION = 0x03;

    // 普通调用
    const TARSNORMAL = 0x00;

    // 单向调用
    const TARSONEWAY = 0x01;

    // 定义按位的消息状态类型,可复合

    const TARSMESSAGETYPENULL = 0x00;    //无状态
    const TARSMESSAGETYPEHASH = 0x01;    //HASH消息
    const TARSMESSAGETYPEGRID = 0x02;    //灰度消息
    const TARSMESSAGETYPEDYED = 0x04;    //染色消息
    const TARSMESSAGETYPESAMPLE = 0x08;    //stat采样消息
    const TARSMESSAGETYPEASYNC = 0x10;    //异步调用程序
    const TARSMESSAGETYPESETNAME = 0x80;     //按setname规则调用类型
}
