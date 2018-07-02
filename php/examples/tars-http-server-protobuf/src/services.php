<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/2/24
 * Time: 下午2:51.
 */

//\App\Core\Tars::onInitServer
// 以namespace的方式,在psr4的框架下对代码进行加载
return array(
    //'namespaceName' => 'HttpServer\\',
    "onInitServer" =>  [\App\Core\Tars::class, "onInitServer"],
);
