<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/4/29
 * Time: 下午3:58.
 */

namespace Tars\monitor\client;

class Consts
{
    const SOCKET_MODE_UDP = 1;
    const SOCKET_MODE_TCP = 2;
    const SOCKET_TCP_MAX_PCK_SIZE = 65536; /* 64*1024 */

    const TARSVERSION = 0x01;
    const TUPVERSION = 0x03;
}
