<?php
namespace App\Helper;
use Illuminate\Support\Facades\Log ;
use Illuminate\Support\Facades\Redis ;

defined('DYNAMIC_PASSWD_DB') OR define('DYNAMIC_PASSWD_DB', 10);
class Net {
    public static  function rpc( $url,  $args =null ){
        /*
        $args_str_arr=[];
        if (is_array($args)){
            foreach ($args as $key =>$value){
                $args_str_arr[]= "$key=".urlencode($value);
            }
        }
        */
        //$opt_url= $url."?".implode("&",$args_str_arr );
        $ret_str=static::send_post_data($url,$args);
        //dd($ret_str);
        return json_decode($ret_str,true);
    }
    public static  function rpc_get( $url,  $args =null ){
        $args_str_arr=[];
        if (is_array($args)){
            foreach ($args as $key =>$value){
                $args_str_arr[]= "$key=".urlencode($value);
            }
        }
        $opt_url= $url."?".implode("&",$args_str_arr );
        \App\Helper\Utils::logger("OPT_URL: $opt_url");
        $ret_str=file_get_contents($opt_url );
        return json_decode($ret_str,true);
    }


    static public function baidu_push( $userid, $messageid, $title, $message_content ){
        $url = Config::get_monitor_url()."/notice/baidu";
        $send_data = array(
            'userid'          => $userid,
            'title'           => $title,
            'messageid'       => $messageid,
            'message_content' => base64_encode(json_encode($message_content)),
        );
        $ret_send = self::send_post_data($url, $send_data);
    }

    /**
     * type   软件类型
     * device 设备类型
     */
    static public function baidu_push_all($type,$device,$messageid,$message_content){
        $url=Config::get_monitor_url()."/notice/baidu_all";
        $send_data = array(
            'type'            => $type,
            'device'          => $device,
            'messageid'       => $messageid,
            'message_content' => base64_encode(json_encode($message_content)),
        );
        return $ret_send = self::send_post_data($url, $send_data);
    }

    static public function  send_sms(  $phone , $message, $user_ip, $type=0 ){
        $url = Config::get_monitor_url()."/notice/sms";
        $send_data = array(
            'phone'   => $phone,
            'message' => $message,
            'user_ip' => $user_ip,
            'type'    => $type ,
        );

        $ret_send = self::send_post_data($url, $send_data);

    }

    static public function  send_sms_taobao(  $phone ,  $user_ip, $type, $args){
        $url = Config::get_monitor_new_url()."/notice/sms";

        $send_data = array(
            'phone'   => $phone,
            'type'    => $type ,
            'user_ip' => ip2long($user_ip),
            'args'    => json_encode($args),
        );

        $ret_send = self::send_post_data($url, $send_data);
    }



    static public  function send_post_data($url, $data)
    {
        // the data should be key-value, or something unexpectable may happen.
        $ch = curl_init();
        if ($ch === false) {
            Log::error("curl_init error");
            return false;
        }
        curl_setopt($ch, CURLOPT_URL, $url);
        //curl_setopt($ch, CURLOPT_HEADER, true);
        curl_setopt($ch, CURLOPT_POST, true);
        curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 5); // wait to connect
        curl_setopt($ch, CURLOPT_TIMEOUT, 3);//wait to execute


        { //Loger
            $args_str_arr=[];
            if (is_array($data)){
                foreach ($data as $key =>$value){
                    $args_str_arr[]= "$key=".urlencode($value);
                }
            }
            $opt_url= $url."?".implode("&",$args_str_arr );
            \App\Helper\Utils::logger("SEND URL $opt_url" );
        }

        $result = curl_exec($ch);

        /**
         * if ($result === false) {
         * return false;
         * }
         */

        curl_close($ch);
        return $result;
    }
    static function http_post_data($url, $data_string) {

        $ch = curl_init();
        curl_setopt($ch, CURLOPT_POST, 1);
        curl_setopt($ch, CURLOPT_URL, $url);
        curl_setopt($ch, CURLOPT_POSTFIELDS, $data_string);
        curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 30); // wait to connect

        curl_setopt($ch, CURLOPT_HTTPHEADER, array(
            'Content-Type: application/json; charset=utf-8',
            'Content-Length: ' . strlen($data_string))
        );
        ob_start();
        curl_exec($ch);
        $return_content = ob_get_contents();
        ob_end_clean();

        $return_code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
        return array($return_code, $return_content);
    }

    static public  function send_get_data($url, $data)
    {
        // the data should be key-value, or something unexpectable may happen.


        $args_str_arr=[];
        if (is_array($data)){
            foreach ($data as $key =>$value){
                $args_str_arr[]= "$key=".urlencode($value);
            }
        }
        $opt_url= $url."?".implode("&",$args_str_arr );

        $ch = curl_init();
        if ($ch === false) {
            Log::error("curl_init error");
            return false;
        }

        \App\Helper\Utils::logger("SEND URL $opt_url" );
        curl_setopt($ch, CURLOPT_URL, $opt_url);

        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 5); // wait to connect
        curl_setopt($ch, CURLOPT_TIMEOUT, 3);//wait to execute
        $result = curl_exec($ch);

        curl_close($ch);
        return $result;
        //return file_get_contents($opt_url);
    }

    static public function set_dynamic_passwd( $phone, $role, $passwd, $connection_conf="api" )
    {
        $redis = Redis::connection($connection_conf );

        if (!$redis->select(DYNAMIC_PASSWD_DB)) {
            return false;
        }

        $key = $phone.'_'.$role;
        $result = $redis->setex($key, 3600*2, $passwd);
        //$redis->close();
        return $result;
    }
}
