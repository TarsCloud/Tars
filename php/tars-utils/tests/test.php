<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/4/24
 * Time: 上午9:39.
 */
require_once '../src/Utils.php';
require_once '../src/Conf.php';

$conf = \Tars\Utils::parseFile('./QDPHP.TARSServer.config.conf');
var_dump($conf);

$conf2 = \Tars\Utils::getTarsConf();

echo "conf2:\n\n";

var_dump($conf2);
