<?php
namespace App\Core;
class Tars {

    static $cmd_config =null ;
    static $error_config =null ;
    static $http_cmd_config=null;
    static $project_name= "yb_account";
    //session redis 配置
    static $session_redis_config=[];

    //是否在正式环境
    static $release_flag=false;


    static function init_shutdown() {

        $shutdown_function=  function  () {
            global  $g_response;
            $error = error_get_last();
            /*
              if (in_array($error['type'],array(E_ERROR,E_WARNING))) {//把你需要记录的错误类型修改下就行
              //这里可以调用你写的Log类或者方法记录错误信息。 也可以使用xhprof扩展什么的
              }
            */
            if ($g_response) {
                $ret_str="";
                $ret_str.="Error Type : " . $error["type"] . "<br>";
                $ret_str.= "Error Message : "  . preg_replace("/\n/", "<br/>", $error["message"]  ) . "<br>";
                $ret_str=json_encode(["ret"=> 1002,  "info"=>"系统出错", 'msg'=> $ret_str   ]);

                $g_response->end( $ret_str );
            }
        };
        register_shutdown_function( $shutdown_function);
    }
    static public function init(){
        //env init
        //$dotenv = new \Dotenv\Dotenv(__DIR__ ."/". "../../conf/env.conf");
        if (strpos( __DIR__ , "/usr/local/app/tars/") !== false ) {
            static::$release_flag = true;
            $env_file_name= __DIR__ ."/". "../../conf/";
            $dotenv = new \Dotenv\Dotenv($env_file_name, "env.conf");
            echo "====IN  RELEASE ======\n";
        }else{
            static::$release_flag = false;
            $env_file_name=  __DIR__ ."/". "../../../" ;
            $dotenv = new \Dotenv\Dotenv($env_file_name );
            echo "====IN DEV ======\n";
        }


        $dotenv->load();
        //加载自定义配置
        \App\Helper\Config::init();
        static::init_shutdown();

        $project_name= static::$project_name ;
        //加载可用的协议
        static::$cmd_config = require( __DIR__. "/". "../proto/$project_name/Proto/{$project_name}_cmd_list.php"  ) ;
        static::$error_config = require( __DIR__. "/". "../proto/$project_name/Proto/{$project_name}_error.php"  ) ;
        static::init_http_cmd_config();

    }

    public function init_http_cmd_config() {
        foreach ( static::$cmd_config as $item ) {
            if (preg_match( "/.*\\\\(.*)\$/",$item[0], $matches  )) {
                $url= "/".  strtolower( $matches[1] ) ."/" . strtolower( $item[1]);
                static::$http_cmd_config[$url] = $item;
            }
        }
    }


    static public function onInitServer( \swoole_server $sw, $tarsConfig ) {
        static::init();

        $sw->on('Request', "\\App\\Core\\Tars::onRequest" );
        if (!static::$release_flag ) {
            $project_name= $tarsConfig['tars']['application']['server']['app'].
                '.'.$tarsConfig['tars']['application']['server']['server'];
            echo " noitfy file:$project_name \n ";
            static::add_watch_process($sw ,$project_name);
        }
    }
    static public function  set_session( \swoole_http_request $request,
                                         \swoole_http_response $response ) {
        global $g_session;
        $g_session=[];
        if (static::$session_redis_config["enable"]==="true"){

            $sessionid=@$request->header["laravel_session"];
            if (!$sessionid)  {
                $sessionid=@$request->cookie["laravel_session"];
            }
            \App\Helper\Utils::logger("sessionid:$sessionid");


            try  {
                $redis= new \Redis ();
                $config=static::$session_redis_config;

                $redis->connect( $config["host"], $config["passwd"] );
                $redis->select( $config["database"] );
                $redis_key="laravel:$sessionid";
                $data_str = $redis->get($redis_key);
                $g_session= @unserialize( @unserialize($data_str ));
                \App\Helper\Utils::logger("session:".json_encode($g_session ));

            }catch(\Exception $e ) {
                $response->end( json_encode(['ret'=> 1002, "info"=> " 系统出错 ", "msg" =>$e->getMessage()   ]) );
                return false;
            } finally {
                try {
                    $redis->close();
                }catch(\Exception $e ){
                }
            }

        }else{
            $g_session=[];
            return true;
        }
        return true;
    }
    static public function  deal_default_uri($uri, \swoole_http_request $request, \swoole_http_response $response) {
        //得到协议文档
        if($uri=="/doc" ) {
            $project_name= static::$project_name ;
            $html=file_get_contents( __DIR__. "/". "../../../proto/show_proto.html"  ) ;
            $html=preg_replace('/__PROJECT_NAME__/',$project_name,$html );
            $response->end( $html);
            return true;

        }else if ( $uri=="/doc_json" ) {
            $project_name= static::$project_name ;
            $proto_json_1=json_decode( file_get_contents( __DIR__. "/". "../proto/$project_name/{$project_name}-info.json"  ),true) ;
            $proto_json_2= json_decode(file_get_contents( __DIR__. "/". "../proto/$project_name/{$project_name}-cmd.json"  ) ,true);
            $proto_json_1["tag_list"]= $proto_json_2["tag_list"];
            $proto_json_1["cmd_list"]= $proto_json_2["cmd_list"];

            $response->header('Content-Type',
                              'application/json');
            $response->end(json_encode($proto_json_1) );
            return true;
        }
        return false;
    }

    static public  function onRequest ( \swoole_http_request $request, \swoole_http_response $response)  {

        //\ChromePhp::getInstance()->clean();


        //设置session
        if(static::set_session($request, $response)===false ) {
            return;
        }

        $uri=$request->server['request_uri'];
        if ( static::deal_default_uri( $uri,  $request, $response ) ) {
            //处理了
            return;
        }
        $response->header('Content-Type',
                          'application/json');




        $cmd_info=@static::$http_cmd_config["$uri"];
        //保存到全局
        global $g_response;
        global $g_request;
        $g_response= $response;
        $g_request= $request;



       \App\Helper\Utils::logger("cmd:$uri");
        $out_arr=[];

        if ($cmd_info) {
            $get=[];
            if ( is_array($request->get) ) {
                $get= $request->get  ;
            }

            $post=[];
            if ( is_array($request->post) ) {
                $post= $request->post  ;
            }

            $data=array_merge($get, $post ) ;
            $json_data= json_encode($data );

            $ctrl_name= $cmd_info[0];
            $action=$cmd_info[1];
            $ctrl=new  $ctrl_name( );
            global $g_session;
            //检查是否要登录
            if ($ctrl->check_login_flag) {
                if (!@$g_session["acc"]) {
                    $out_arr=[
                        "ret" =>  103 ,
                        "info" => "未登录" ,
                    ];
                    $response->end(json_encode( $out_arr) );
                    return;
                }
            }

            $in_class= $cmd_info[2];
            $out_class= $cmd_info[3];
            $in=null;
            $out=null;

            \App\Helper\Utils::logger("IN:$json_data");

            if ($in_class) {
                $in=new $in_class();
                if ($json_data=="[]") {
                    $json_data="{}";
                }
                try {
                    $in->mergeFromJsonString($json_data );
                }catch( \Exception $e ){ //有异常参数
                    $class= new \ReflectionClass($in);
                    $arr=$class-> getProperties();
                    $in_args_name_map=[];
                    foreach ( $arr as $k => $v) {
                        $in_args_name_map[ $v->getName() ] =true;
                    }
                    foreach( $data as $key => $value ) {
                        if ( !isset($in_args_name_map[$key] ) ) {
                            \App\Helper\Utils::logger(" nofind args : $key=>$value ");
                            unset($data[$key] );
                        }
                    }
                    $json_data= json_encode($data );
                    if ($json_data=="[]") {
                        $json_data="{}";
                    }
                    $in->mergeFromJsonString($json_data );

                }
            }

            if ($out_class) {
                $out=new $out_class();
            }


            $out_json="";
            $ret= $ctrl->$action( $in, $out );
            if ($ret  <>0  ) {
                $out_arr=[
                    "ret" => $ret,
                    "info" => @static::$error_config[$ret ],
                ];
            }else{
                $ret=0;
                $out_json=$out->serializeToJsonString();
                $out_arr=[
                    "ret" => 0,
                    "info" => "succ",
                    "out" =>  json_decode($out_json ),
                ];
            }

        }else{
            $out_arr=[
                "ret" => -1,
                "info" => "url not find ",
            ];
        }


        $response->end(json_encode( $out_arr) );

    }

    static public  function add_watch_process($serv, $project_name) {
        $process1 = new \swoole_process(function ($worker) use ($serv,$project_name) {

            $process_name=$project_name.': task notify';
            echo " set name: $process_name \n";
            \swoole_set_process_name( $process_name );

            $include_arr= get_included_files();
            foreach( $include_arr as $include_file ){
                \App\Helper\Utils::logger( "include:".  $include_file  );
            }

            $watcher= new DirWatcher ();
            $file_list=[
                "./app/Core",
                "./app/Controllers",
                "./app/Models",
                "./app/Models/Zgen",
            ];

            $pwd=realpath( __DIR__. "/../../");


            foreach ( $file_list as $file ) {

                $abs_path="$pwd/$file" ;
                \App\Helper\Utils::logger( "abs_path:$abs_path ");

                $check_file=realpath( $abs_path );
                \App\Helper\Utils::logger( "add check :$check_file ");
                $watcher->addDirectory( $check_file );

            }
            $watcher->addCallback(function() use($serv ){
                \App\Helper\Utils::logger( "find new file , reload ");
                $serv->reload();
            });
            \App\Helper\Utils::logger( "start watcher ...");
            $watcher->startWatch();
            /*
              swoole_timer_tick(2000, function ($interval) use ($worker, $serv) {
              //\App\Helper\Utils::logger("DO TIMER  ");
              });
            */
        }, false);

        $serv->addprocess($process1);
    }

}
