<?php
return [

    "session_redis" => [
        'enable'     => \App\Helper\Utils:: env('SESSION_REDIS_ENABLE', 'true'),
        'host'     => \App\Helper\Utils:: env('SESSION_REDIS_HOST', 'localhost'),
        'password' => \App\Helper\Utils:: env('SESSION_REDIS_PASSWORD', null),
        'port'     => \App\Helper\Utils:: env('SESSION_REDIS_PORT', 6379),
        "database" =>  \App\Helper\Utils:: env('SESSION_REDIS_PORT',0),
    ],




    "database" => [
        "default" => "mysql",
        'connections' => [
            'mysql' => [
                'driver'    => 'mysql',
                'host'      => \App\Helper\Utils:: env('DB_HOST', 'localhost'),
                'database'  => \App\Helper\Utils:: env('DB_DATABASE', 'db_xx'),
                'username'  => \App\Helper\Utils:: env('DB_USERNAME', 'xx'),
                'password'  => \App\Helper\Utils:: env('DB_PASSWORD', 'xxdrd'),
                'charset'   => 'latin1',
                'collation' => 'latin1_bin',
                'prefix'    => '',
                'strict'    => false,
                'engine'    => null,
            ],

        ],
    ],


];
