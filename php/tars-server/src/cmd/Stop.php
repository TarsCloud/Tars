<?php
/**
 * Created by PhpStorm.
 * User: dingpanpan
 * Date: 2017/12/2
 * Time: 16:02.
 */

namespace Tars\cmd;

class Stop extends CommandBase
{
    public function __construct($configPath)
    {
        parent::__construct($configPath);
    }

    public function execute()
    {
        $tarsConfig = $this->tarsConfig;

        //判断master进程是否存在
        if (empty($tarsConfig['tars']['application']['server']['app'])
            || empty($tarsConfig['tars']['application']['server']['server'])) {
            echo 'AppName or ServerName empty! Please check config!'.PHP_EOL;
            exit;
        }

        $name = $tarsConfig['tars']['application']['server']['app'].
            '.'.$tarsConfig['tars']['application']['server']['server'];
        $ret = $this->getProcess($name);
        if ($ret['exist'] === false) {
            echo "{$name} stop  \033[34;40m [FAIL] \033[0m process not exists".PHP_EOL;

            return;
        }

        $masterPid = implode(' ', $ret['masterPid']);
        $cmd = "kill -15 {$masterPid}";
        exec($cmd, $output, $r);

        if ($r === false) { // kill失败时
            echo "{$name} stop  \033[34;40m [FAIL] \033[0m posix exec fail".PHP_EOL;
            exit;
        }

        echo "{$name} stop  \033[32;40m [SUCCESS] \033[0m".PHP_EOL;
    }
}
