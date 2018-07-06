<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/5/7
 * Time: 下午4:05.
 */

namespace Tars\protocol;

class HTTPProtocol implements Protocol
{
    // 决定是否要提供一个口子出来,让用户自定义启动服务之前的初始化的动作
    // 这里需要对参数进行规定

    public function packRsp($paramInfo, $unpackResult, $args, $returnVal)
    {
    }

    public function packErrRsp($unpackResult, $code, $msg)
    {
    }
    public function route(\Tars\core\Request $request, \Tars\core\Response $response, $tarsConfig = [])  //默认为
    {
        $uri = $request->data['server']['request_uri'];
        $verb = $request->data['server']['request_method'];
        $list = explode('/', $uri);

        // 这里的大小写和autoload需要确定一个规则
        return [
            'class' => ucwords($list[1]).'Controller',
            'action' => 'action'.ucwords($list[2]),
        ];
    }

    public function parseAnnotation($docblock)
    {
    }
}
