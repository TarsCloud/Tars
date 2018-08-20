<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/2/24
 * Time: 下午2:51.
 */

// 以namespace的方式,在psr4的框架下对代码进行加载
return array(
    'namespaceName' => 'HttpServer\\',
    'monitorStoreConf' => [
        //使用redis缓存主调上报信息
        //'className' => Tars\monitor\cache\RedisStoreCache::class,
        //'config' => [
        // 'host' => '127.0.0.1',
        // 'port' => 6379,
        // 'password' => ':'
        //],
        //使用swoole_table缓存主调上报信息（默认）
        'className' => Tars\monitor\cache\SwooleTableStoreCache::class,
        'config' => [
            'size' => 40960
        ]
    ]
);
