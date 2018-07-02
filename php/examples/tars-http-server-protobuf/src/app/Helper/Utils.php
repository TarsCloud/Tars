<?php
namespace App\Helper;
use App\Enums as  E;

class Utils  {

    public static function logger( $fmt_str  ) {
        $args=func_get_args();
        if (count($args)==1) {
            $message= $fmt_str;
        }else{
            $message= call_user_func_array("sprintf",$args );
        }
        $now=time(NULL);

        global $g_response ;
        /*
        if ( $g_response ) {
            $time_str=date('H:i:s', time(NULL));
           \ChromePhp::log($time_str, substr( $message,0,400));
        }
        */

        echo  date('Y-m-d H:i:s').  " P_".getmypid().":". $message."\n";
    }

    static function env( $key, $default="" ) {
        $v= getenv( $key );
        if ($v =="") {
            return $default;
        }else{
            return $v;
        }
    }

};
