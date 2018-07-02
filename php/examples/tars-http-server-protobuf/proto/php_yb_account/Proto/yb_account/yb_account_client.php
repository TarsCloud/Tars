<?php
namespace Proto\yb_account;

class yb_db_client extends \App\Core\proto_base {
    function __construct( $ip,$port){
        parent::__construct($ip,$port);
    }


    /**
     * 用户登录
     */
    public function  account__login(  $role=0,$phone="",$passwd="" ) {

        if ($role instanceof  account__login\in    ) {
            $in = $role;
        }else{
            $in = new account__login\in ();
            $in->setRole(intval($role));
            $in->setPhone(strval($phone));
            $in->setPasswd(strval($passwd));

        }


        $out=  new account__login\out();
        return $this->send_cmd( 0x1001 ,$in,$out );
    }

    /**
     * 得到用户id
     */
    public function  account__get_userid(  $role=0,$phone="" ) {

        if ($role instanceof  account__get_userid\in    ) {
            $in = $role;
        }else{
            $in = new account__get_userid\in ();
            $in->setRole(intval($role));
            $in->setPhone(strval($phone));

        }


        $out=  new account__get_userid\out();
        return $this->send_cmd( 0x1003 ,$in,$out );
    }

}
