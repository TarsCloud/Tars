<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/2/12
 * Time: 下午2:44.
 */

namespace Tars\protocol;

use Tars\core\Request;
use Tars\core\Response;

interface Protocol
{
    // 决定是否要提供一个口子出来,让用户自定义启动服务之前的初始化的动作
    // 这里需要对参数进行规定

    public function route(Request $request, Response $response, $tarsConfig = []);

    public function packRsp($paramInfo, $unpackResult, $args, $returnVal);

    public function packErrRsp($unpackResult, $code, $msg);

    public function parseAnnotation($docblock);
}
