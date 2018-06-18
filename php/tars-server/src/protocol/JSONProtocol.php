<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/3/9
 * Time: 下午2:46.
 */

namespace Tars\protocol;

class JSONProtocol
{
    // 决定是否要提供一个口子出来,让用户自定义启动服务之前的初始化的动作
    public function route(\Tars\core\Request $request, \Tars\core\Response $response, $tarsConfig = [])  //默认为
    {
    }

    public function packRsp($paramInfo, $unpackResult, $args, $returnVal)
    {
    }

    public function packErrRsp($unpackResult, $code, $msg)
    {
        // 需要前四位的打包函数
        return json_encode(
            [
                'code' => $code,
                'msg' => $msg,
            ], JSON_UNESCAPED_UNICODE
        );
    }
    public function parseAnnotation($docblock)
    {
    }
}
