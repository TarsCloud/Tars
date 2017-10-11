/**
 * tars框架的客户端实现类
 */
var Promise      = require("@tars/utils").Promise;
var ReqMessage   = require("./ReqMessage.js").ReqMessage;

var ServantProxy = function () {
    this._name   = undefined;
    this._worker = undefined;
    this._rpc    = undefined;
};
module.exports.ServantProxy = ServantProxy;

ServantProxy.prototype.__defineGetter__("rpc", function () { return this._rpc; });

ServantProxy.prototype.setTimeout  = function (iTimeout) { //设置请求超时时间
    this._worker.timeout = iTimeout;
};

ServantProxy.prototype.setProtocol = function ($protocol) { //设置协议解析器
    this._worker.setProtocol($protocol);
};

ServantProxy.prototype.rpc_call = function ($RequestId, $FuncName, $AppBuffer) {
    var argsLen = arguments.length, args;
    if (argsLen > 2) {
        args = new Array(argsLen - 2);
        for (var i = 2; i < argsLen; i += 1) {
            args[i - 2] = arguments[i];
        }
    } else {
        args = [];
    }

    var reqMessage = new ReqMessage();
    reqMessage.request.iRequestId = $RequestId;
    reqMessage.request.sFuncName  = $FuncName;
    reqMessage.request.appBuffer  = args;
    reqMessage.request.property   = {};
    reqMessage.promise            = Promise.defer();
    reqMessage.worker             = this._worker;
    reqMessage.setTimeout(this._worker._iTimeout);

    return this._worker.invoke(reqMessage).then(function (data) {
        var response = {
            costtime: 0
        };

        if(data.request && data.request.costtime)
            response.costtime = data.request.costtime;
        response.appBuffer = data.response;

        return {request:data.request, response:response};
    }, function (data) {
        data = data || {};
        var response = {
            costtime: 0
        };
        if(data.request && data.request.costtime)
            response.costtime = data.request.costtime;
        response.error    = data.error;

        throw  {request:data.request, response:response};
    });
};
