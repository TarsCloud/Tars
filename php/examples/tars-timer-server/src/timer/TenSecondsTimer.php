<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/5/13
 * Time: 上午10:31.
 */

namespace TimerServer\timer;

class TenSecondsTimer
{
    public $interval;

    public function __construct()
    {
        $this->interval = 10000;
    }

    public function execute()
    {
        error_log('Timer execute with timer:'.time());

        $config = new \Tars\client\CommunicatorConfig();
        $config->setLocator('tars.tarsregistry.QueryObj@tcp -h 172.16.0.161 -p 17890');
        $config->setModuleName('PHPTest.PHPHttpServer');
        $config->setSocketMode(2);

        $servant = new \TimerServer\servant\PHPTest\PHPServer\obj\TestTafServiceServant($config);

        $cs = new \TimerServer\servant\PHPTest\PHPServer\obj\classes\ComplicatedStruct();
        $cs->str = '111';
        $b = new \TimerServer\servant\PHPTest\PHPServer\obj\classes\SimpleStruct();
        $b->count = 1098;
        $cs->mss->pushBack(['a' => $b]);
        $cs->rs = $b;

        $mcs = ['test' => $cs];
        $result = $servant->testComplicatedMap($mcs, $omcs);

        error_log('TenSeconds timer testComplatedMap result:'.var_export($result, true));
    }
}
