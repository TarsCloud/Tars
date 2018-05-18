<?php
/**
 * Created by PhpStorm.
 * User: dingpanpan
 * Date: 2017/12/2
 * Time: 16:02.
 */

namespace Tars\cmd;

class Restart extends CommandBase
{
    public function __construct($configPath)
    {
        parent::__construct($configPath);
    }

    public function execute()
    {
        $configPath = $this->configPath;

        $class = new Stop($configPath);
        $class->execute();

        $class = new Start($configPath);
        $class->execute();
    }
}
