/**
 * tars框架的客户端实现类
 */
var Promise         = require("@tars/utils").Promise;
var Protocol        = require("../rpc-tars").client;
var TarsError        = require("../util/TarsError.js").TarsError;
var TQueue          = require("../util/TQueue.js").TQueue;
var EndpointManager = require("./EndpointManager.js").EndpointManager;
var ReqMessage      = require("./ReqMessage.js").ReqMessage;

///////////////////////////////////////////////定义调用类///////////////////////////////////////////////////////////////
var ObjectProxy = function () {
    this._objname       = "";            //ObjectProxy的名称
    this._setname       = "";            //使用stringtoProxy时指定的set名称
    this._pTimeoutQueue = new TQueue();  //全局数据发送队列
    this._manager       = undefined;     //对端连接管理器
    this._requestId     = 1;             //当前服务队列的请求ID号
    this._iTimeout      = 3000;          //默认的调用超时时间
    this._protocol      = Protocol;      //当前端口上的协议解析器
    this._comm          = undefined;     //通信器实例
};
module.exports.ObjectProxy = ObjectProxy;

//定义ObjectProxy的属性函数
ObjectProxy.prototype.__defineSetter__("communicator", function (value){ this._comm = value; });
ObjectProxy.prototype.__defineGetter__("communicator", function (){ return this._comm; });

ObjectProxy.prototype.__defineGetter__("timeout", function () { return this._iTimeout; });
ObjectProxy.prototype.__defineSetter__("timeout", function (value) { this._iTimeout = value; });

ObjectProxy.prototype.__defineGetter__("name", function () { return this._objname; });
ObjectProxy.prototype.__defineSetter__("name", function (value) { this._objname = value; });

ObjectProxy.prototype.__defineGetter__("pTimeoutQueue", function () { return this._pTimeoutQueue; });
ObjectProxy.prototype.__defineSetter__("pTimeoutQueue", function (value) { this._pTimeoutQueue = value; });

//初始化ObjectProxy
ObjectProxy.prototype.initialize = function ($ObjName, $SetName) {
    this._manager = new EndpointManager(this, this._comm, $ObjName, $SetName);
    this._objname = this._manager._objname;
    this._setname = $SetName;
};

ObjectProxy.prototype.setProtocol = function ($protocol) {
    this._protocol = $protocol;
};

ObjectProxy.prototype.genRequestId = function () {
    return ++this._requestId;
};

//当重新连接或者第一次连接上服务端时，传输类回调当前函数
ObjectProxy.prototype.doInvoke = function () {
    var self = this;

    self._pTimeoutQueue.forEach(function (key) {
        var reqMessage = self._pTimeoutQueue.pop(key, true);

        var adapter    = self._manager.selectAdapterProxy(reqMessage);

        if(adapter) {
            adapter.invoke(reqMessage);
        } else {
            self.doInvokeException(reqMessage);
        }
    });
};

//在没有选取对端接口成功的情况下，框架会将请求放到对应ObjectProxy的请求队列中
//此时如果一直连接服务不成功，reqMessage中的定时器会调用当前超时处理函数
ObjectProxy.prototype.doTimeout = function($reqMessage) {
    this._pTimeoutQueue.erase($reqMessage.request.iRequestId);

    $reqMessage.clearTimeout();
    $reqMessage.promise.reject({request:$reqMessage, response:undefined, error:{code:TarsError.CLIENT.REQUEST_TIMEOUT, message:"call remote server timeout(no adapter selected)"}});
};

ObjectProxy.prototype.doInvokeException = function ($reqMessage) {
    $reqMessage.clearTimeout();
    $reqMessage.promise.reject({request:$reqMessage, response:undefined, error:{code:TarsError.SERVER.TARSADAPTERNULL, message:"select AdapterProxy is null"}});
};

ObjectProxy.prototype.invoke = function($reqMessage) {
    var adapter  = this._manager.selectAdapterProxy($reqMessage);
    if (adapter) {
        adapter.invoke($reqMessage);
    } else {
        var hashCode = $reqMessage.request.property['hashCode'];

        var adapters = this._manager.getAllAdapters() || [];

        if(!hashCode || adapters.length == 0) {
            // 第一次调用的时候，adapters长度为0
            this._pTimeoutQueue.push($reqMessage.request.iRequestId, $reqMessage);
        }
    }

    return $reqMessage.promise.promise;
};

ObjectProxy.prototype.tars_invoke = function ($FuncName, $BinBuffer, $Property) {
    //判断最后一个参数，是否是属性结构体
    var extProperty = {};
    if ($Property && $Property.hasOwnProperty("dyeing")) {
        extProperty.dyeing  = $Property.dyeing;
    }
    if ($Property && $Property.hasOwnProperty("context")) {
        extProperty.context = $Property.context;
    }
    if ($Property && $Property.hasOwnProperty("packetType")) {
        extProperty.packetType = $Property.packetType;
    }

    if ($Property && $Property.hasOwnProperty("hashCode")) {
        extProperty.hashCode = $Property.hashCode;
    }

    //构造请求
    var reqMessage = new ReqMessage();
    reqMessage.request.iRequestId   = this.genRequestId();
    reqMessage.request.sServantName = this._objname;
    reqMessage.request.sFuncName    = $FuncName;
    reqMessage.request.appBuffer    = $BinBuffer;
    reqMessage.request.property     = extProperty;
    reqMessage.request.configure    = this._comm.configure;
    reqMessage.promise              = Promise.defer();
    reqMessage.worker               = this;
    reqMessage.setTimeout(this._iTimeout);
    return this.invoke(reqMessage);
};

ObjectProxy.prototype.destroy = function () {
    this._manager.destroy();
};
