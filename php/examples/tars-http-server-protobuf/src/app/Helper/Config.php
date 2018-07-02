<?php
namespace  App\Helper;
class Config {
    static  $config_map;
    static  function init () {
        $config_path= realpath( __DIR__ . "/"."../../config/") ;

        \App\Helper\Utils::logger( "$config_path" );
        $file_list=scandir($config_path) ;


        foreach ($file_list as $file_name){
            if( preg_match("/(.*).php/",$file_name,$matches )) {
                $bucket_name=$matches[1];
                \App\Helper\Utils::logger("load config : $bucket_name ");
                static::$config_map[$bucket_name]=require_once(  $config_path . "/$bucket_name.php"  );

            }
        }

        //print_r(static::$config_map);
        //exit;
    }



    static function get_config( $key,  $bucket_name ="admin" )  {
        return static::$config_map[$bucket_name][$key];
    }


    static public function  get_session_redis () {
        return  self::get_config("session_redis");
    }


}
