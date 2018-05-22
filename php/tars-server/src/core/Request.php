<?php
/**
 * Created by PhpStorm.
 * User: yuanyizhi
 * Date: 15/8/12
 * Time: 上午11:04.
 */

namespace Tars\core;

class Request
{
    public $reqBuf;
    public $servType;
    public $data;
    public $server = array();

    public $paramInfos;
    public $impl;
    public $namespaceName; //标识当前服务的namespacePrefix

    /**
     * 将原始请求信息转换到PHP超全局变量中.
     */
    public function setGlobal()
    {
        if ($this->server) {
            $_SERVER = $this->server;
        }
    }
    public function unsetGlobal()
    {
        $_SERVER = array();
    }
}
