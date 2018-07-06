<?php
/**
 * Created by PhpStorm.
 * User: dingpanpan
 * Date: 2017/12/2
 * Time: 16:20.
 */

namespace Tars\cmd;

class Command
{
    public $cmd;
    public $confPath;

    public function __construct($cmd, $confPath)
    {
        $this->cmd = $cmd;
        $this->confPath = $confPath;
    }

    public function run()
    {
        $cmd = $this->cmd;
        $confPath = $this->confPath;

        if ($cmd === 'start') {
            $class = new Start($confPath);
            $class->execute();
        } elseif ($cmd === 'stop') {
            $class = new Stop($confPath);
            $class->execute();
        } elseif ($cmd === 'restart') {
            $class = new Restart($confPath);
            $class->execute();
        } else {
            // 默认其实就是start
            $class = new Start($confPath);
            $class->execute();
        }
    }
}
