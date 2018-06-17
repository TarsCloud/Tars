<?php
/**
 * Created by PhpStorm.
 * User: dingpanpan
 * Date: 2017/12/2
 * Time: 16:02.
 */

namespace Tars\cmd;

use Tars\Conf;
use Tars\core\Server;

class Start extends CommandBase
{
    public function __construct($configPath)
    {
        parent::__construct($configPath);
    }

    public function execute()
    {
        $configPath = $this->configPath;
        $tarsConfig = $this->tarsConfig;

        //检查必须的服务名是否存在
        if (empty($tarsConfig['tars']['application']['server']['app'])
            || empty($tarsConfig['tars']['application']['server']['server']))
        {
            echo 'AppName or ServerName empty! Please check config!'.PHP_EOL;
            exit;
        }

        // 检查一下业务必须的配置是否存在
        $basePath = $tarsConfig['tars']['application']['server']['basepath'];

        $servicesInfo = require $basePath.'src/services.php';
        if ($tarsConfig['tars']['application']['server']['servType'] === 'tcp')
        {
            if (!isset($servicesInfo['home-class']) || !isset($servicesInfo['home-api']))
            {
                echo 'home-class or home-api not exist, please chech services.php!'.PHP_EOL;
                exit;
            }
            $tarsConfig['tars']['application']['server']['servicesInfo'] = $servicesInfo;
        } else {
            $tarsConfig['tars']['application']['server']['servicesInfo'] = $servicesInfo;
        }

        $name = $tarsConfig['tars']['application']['server']['app']
            .'.'.$tarsConfig['tars']['application']['server']['server'];
        $ret = $this->getProcess($name);
        if ($ret['exist'] === true)
        {
            echo "{$name} start  \033[34;40m [FAIL] \033[0m process already exists".PHP_EOL;
            exit;
        }

        // 把配置对应的swooleTable对象设置到swoole serv里,从而全局使用
        $table = Conf::getInstance();
        $server = new Server($tarsConfig, $table);

        //创建成功
        echo "{$name} start  \033[32;40m [SUCCESS] \033[0m".PHP_EOL;

        $server->start();
    }
}
