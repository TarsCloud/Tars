<?php
/**
 * Created by PhpStorm.
 * User: liangchen
 * Date: 2018/3/9
 * Time: 下午3:06.
 */

namespace Tars\core;

use Tars\protocol\Protocol;
use Tars\Code;

class Event
{
    protected $protocol;
    protected $basePath;
    protected $tarsConfig;

    public function setProtocol(Protocol $protocol)
    {
        $this->protocol = $protocol;
    }

    public function setBasePath($basePath)
    {
        $this->basePath = $basePath;
    }

    public function setTarsConfig($tarsConfig)
    {
        $this->tarsConfig = $tarsConfig;
    }

    // 我希望别人在这个之前和之后,都可以通过自己的代码来介入开发
    // 应该是request对象和response对象
    // 对unpackResult的结构要求:

    public function onReceive(Request $request, Response &$response)
    {
        $impl = $request->impl;
        $paramInfos = $request->paramInfos;

        try {
            // 这里通过protocol先进行unpack
            $result = $this->protocol->route($request, $response, $this->tarsConfig);

            if (is_null($result)) {
                return;
            } else {
                $sFuncName = $result['sFuncName'];
                $args = $result['args'];
                $unpackResult = $result['unpackResult'];
                if (method_exists($impl, $sFuncName)) {
                    $returnVal = $impl->$sFuncName(...$args);
                } else {
                    throw new \Exception(Code::TARSSERVERNOFUNCERR);
                }
                $paramInfo = $paramInfos[$sFuncName];

                $rspBuf = $this->protocol->packRsp($paramInfo, $unpackResult, $args, $returnVal);
                $response->send($rspBuf);

                return;
            }
        } catch (\Exception $e) {
            $unpackResult['iVersion'] = 1;
            $rspBuf = $this->protocol->packErrRsp($unpackResult, $e->getCode(), $e->getMessage());
            $response->send($rspBuf);

            return;
        }
    }

    /**
     * @param Request  $request
     * @param Response $response
     *                           针对http进行处理
     */
    public function onRequest(Request $request, Response $response)
    {
        if ($request->data['server']['request_uri'] == '/monitor/monitor') {
            $response->header('Content-Type', 'application/json');
            $response->send("{'code':0}");

            return;
        }
        $namespaceName = $request->namespaceName;

        $route = $this->protocol->route($request, $response);
        if (!$route) {
            $class = $namespaceName.'controller\IndexController';
            $fun = 'actionIndex';
        } else {
            $class = $namespaceName.'controller\\'.$route['class'];
            $fun = $route['action'];
        }

        if ((!class_exists($class) || !method_exists(($class), ($fun)))) {
            if ($response->servType == 'http') {
                $response->status(404);
            }
            $response->send('not found');

            return;
        }
        $obj = new $class($request, $response);
        $obj->$fun();
    }
}
