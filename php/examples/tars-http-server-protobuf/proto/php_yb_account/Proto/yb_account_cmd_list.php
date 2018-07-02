<?php
return [
	0x1001 => [ \App\Controllers\account::class, "login", \Proto\yb_account\account__login\in::class , \Proto\yb_account\account__login\out::class   ,"用户登录"   ],
	0x1003 => [ \App\Controllers\account::class, "get_userid", \Proto\yb_account\account__get_userid\in::class , \Proto\yb_account\account__get_userid\out::class   ,"得到用户id"   ],
];
