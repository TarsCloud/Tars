<?php
namespace App\Controllers;

use  \Proto\yb_account as P;
use  \Proto\yb_account\error\enum_error as ERR;

use App\Helper\Utils;
use Gregwar\Captcha\CaptchaBuilder;
use App\Enums as E;



class  account extends Controller {

    var $check_login_flag=false;
    public  function  login (P\account__login\in  $in,
                             P\account__login\out &$out)
    {
        $passwd=$in->getPasswd();
        $phone=$in->getPhone();
        if (!$passwd) {
            return ERR::ERR_PASSWD;
        }


        if (strlen($phone)!=11 || empty($phone)) {
            return ERR::ERR_PHONE;
        }

        $out->setId( 11 );
        return ;
    }


    public  function  get_userid (
        P\account__get_userid\in  $in,
        P\account__get_userid\out &$out)
    {
        $phone=$in->getPhone();
        $role=$in->getRole();

        $out->setId($phone*2 );

    }





}
