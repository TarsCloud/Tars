<?php
/**
 * Created by PhpStorm.
 * User: dingpanpan
 * Date: 2017/12/2
 * Time: 16:20.
 */

namespace Tars\cmd;

use Tars\Utils;

class CommandBase
{
    public $configPath;
    public $tarsConfig;

    public function __construct($configPath)
    {
        $this->configPath = $configPath;

        $tarsConfig = Utils::parseFile($configPath);
        $this->tarsConfig = $tarsConfig;
    }

    /**
     * @param $processName
     *
     * @return array
     */
    public function getProcess($processName)
    {
        $cmd = "ps aux | grep '".$processName."' | grep master | grep -v grep  | awk '{ print $2}'";
        exec($cmd, $ret);

        if (empty($ret)) {
            return [
                'exist' => false,
            ];
        } else {
            return [
                'exist' => true,
                'masterPid' => $ret,
            ];
        }
    }
}
