<?php

namespace Tars\deploy;

class Deploy
{
    public static $serverName = '';

    public static function run()
    {
        self::handle();
    }

    public static function handle()
    {
        $tarsPHPRoot = dirname(dirname(dirname(dirname(dirname(dirname(__FILE__))))));
        if (!is_dir($tarsPHPRoot)) {
            self::log('is not dir : '.$tarsPHPRoot);
            exit();
        }

        $serverName = self::getServerName($tarsPHPRoot);

        //复制项目代码到tmp/serverName 目录下
        $tmpDir = $tarsPHPRoot.DIRECTORY_SEPARATOR.'tmp';
        $tmpProjectDir = $tmpDir.DIRECTORY_SEPARATOR.$serverName;

        if (is_dir($tmpDir)) {
            self::rmDir($tmpDir);
        } elseif (is_file($tmpDir)) {
            @unlink($tmpDir);
        }
        mkdir($tmpDir, 0700);
        mkdir($tmpProjectDir, 0700);
        self::copyDir($tarsPHPRoot, $tmpProjectDir, $tmpDir);

        //打包
        $tgzFile = $tarsPHPRoot.DIRECTORY_SEPARATOR.'src'.DIRECTORY_SEPARATOR.$serverName.'_'.date('YmdHis').'.tar.gz';
        $phar = new \PharData($tgzFile);
        $phar->compress(\Phar::GZ);
        $phar->buildFromDirectory($tmpDir);

        self::log('tgzFile : '.$tgzFile);
        self::log('[SUCCESS]');

        //删除tmp目录
        self::rmDir($tmpDir);

        return true;
    }

    public static function log($info)
    {
        print_r('>>>> '.$info."\r\n");
    }

    //根据jce.proto.php 获取服务名
    public static function getServerName($tarsPHPRoot)
    {
        $jceProtoFile = $tarsPHPRoot.DIRECTORY_SEPARATOR.'tars'.DIRECTORY_SEPARATOR.'tars.proto.php';
        if (!is_file($jceProtoFile)) {
            self::log('is not file : '.$jceProtoFile);
            exit();
        }
        self::log('use jce.proto.php : '.$jceProtoFile);

        $jceProto = require_once $jceProtoFile;
        $serverName = $jceProto['serverName'];
        self::$serverName = $serverName;

        self::log('ServerName : '.$serverName);

        return $serverName;
    }

    public static function copyDir($source, $dest, $tmpDir)
    {
        $git = dirname($tmpDir).DIRECTORY_SEPARATOR.'.git';
        $srcZipPre = dirname($tmpDir).DIRECTORY_SEPARATOR.'src'.DIRECTORY_SEPARATOR.self::$serverName;

        if (!file_exists($dest)) {
            mkdir($dest);
        }
        $handle = opendir($source);
        while (($item = readdir($handle)) !== false) {
            if ($item == '.' || $item == '..') {
                continue;
            }
            $_source = $source.DIRECTORY_SEPARATOR.$item;
            $_dest = $dest.DIRECTORY_SEPARATOR.$item;
            if (is_file($_source)) {
                if (strpos($_source, $srcZipPre) !== false) {
                    if (strpos($_source, '.zip') !== false || strpos($_source, '.tgz') !== false || strpos($_source, '.tar.gz') !== false) {
                        continue; //排除生成的zip包
                    }
                }
                copy($_source, $_dest);
            }
            if (is_dir($_source)
                && strpos($_source, $tmpDir) === false //排除tmp目录
                && $_source != $git //排除.git目录
                ) {
                self::copyDir($_source, $_dest, $tmpDir);
            }
        }
        @closedir($handle);
    }

    public static function rmDir($path)
    {
        $handle = opendir($path);
        while (($item = readdir($handle)) !== false) {
            if ($item == '.' || $item == '..') {
                continue;
            }
            $_path = $path.DIRECTORY_SEPARATOR.$item;
            if (is_file($_path)) {
                @unlink($_path);
            }
            if (is_dir($_path)) {
                self::rmDir($_path);
            }
        }
        @closedir($handle);
        rmdir($path);
    }
}
