<?php
namespace App\Controllers;
class Controller extends ControllerEx
{

    var $check_login_flag = true;
    public  function __construct() {

    }

    public function get_account_id() {
        global $g_session;
        return  @$g_session["adminid"];
    }

    public function get_account() {
        global $g_session;
        return  @$g_session["acc"];
    }


    public function __get( $name ){
        if (substr($name ,0,2  ) == "t_" ) {
            $reflectionObj = new \ReflectionClass( "App\\Models\\$name");
            $this->$name= $reflectionObj->newInstanceArgs();
            return $this->$name;
        }else{
            throw new \Exception() ;
        }
    }

}
