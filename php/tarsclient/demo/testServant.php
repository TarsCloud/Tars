<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2017/6/16
 * Time: 下午2:38
 */

require_once "./vendor/autoload.php";

$servant = new App\Server\Servant\servant("",0);

$servant->ping();

