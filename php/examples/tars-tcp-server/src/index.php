<?php
/**
 * Created by PhpStorm.
 * User: dingpanpan
 * Date: 2017/12/2
 * Time: 17:00.
 */
require_once __DIR__.'/vendor/autoload.php';

use \Tars\cmd\Command;

//php tarsCmd.php  conf restart
$config_path = $argv[1];
$pos = strpos($config_path, '--config=');

$config_path = substr($config_path, $pos + 9);

$cmd = strtolower($argv[2]);

$class = new Command($cmd, $config_path);
$class->run();
