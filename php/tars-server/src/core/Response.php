<?php
/**
 * Created by PhpStorm.
 * User: yuanyizhi
 * Date: 15/8/12
 * Time: 下午2:40.
 */

namespace Tars\core;

class Response
{
    public $fd;
    public $fromFd;
    public $server;
    public $rspBuf;
    public $servType;
    public $resource;

    public function sendto($ip, $port, $data, $ipv6 = false)
    {
        $this->server->sendto($ip, $port, $data, $ipv6);
    }

    //http
    public function header($key, $value)
    {
        $this->resource->header($key, $value);
    }

    public function status($http_status_code)
    {
        $this->resource->status($http_status_code);
    }

    public function cookie($key, $value = '', $expire = 0, $path = '/', $domain = '', $secure = false, $httponly = false)
    {
        $this->resource->cookie($key, $value, $expire, $path, $domain, $secure, $httponly);
    }

    public function send($data)
    {
        switch ($this->servType) {
            //tcp
            case 'http':
                $this->resource->end($data);
                break;
            default:
                $this->server->send($this->fd, $data);
        }
    }

    public function addTask(Task $task)
    {
        $this->server->addTask($task);
    }
}
