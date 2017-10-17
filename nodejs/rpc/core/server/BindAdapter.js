var assert         = require("assert");
var Endpoint       = require("@tars/utils").Endpoint;
var TimeProvider   = require("@tars/utils").timeProvider;
var TarsError       = require("../util/TarsError.js").TarsError;
var NetThread      = require("./NetThread.js").NetThread;
var TarsCurrent     = require("./TarsCurrent.js").TarsCurrent;

////////////////////
// /////////////////////////监听端口功能包裹类///////////////////////////////////////////////////////
var BindAdapter = function () {
    this._objname       = "";           //BindAdapter的名字
    this._servant       = "";           //当前端口上的servant名字
    this._endpoint      = undefined;    //当前端口监听地址

    this._protocolName  = "";
    this._protocol      = undefined;    //当前端口上的请求包协议解析器
    this._maxconns      = 1024;         //当前端口上最大的连接数

    this._netthread     = undefined;    //当前端口上的监听服务
    this._handleImp     = undefined;    //当前端口上的逻辑处理对象实例
};
module.exports.BindAdapter = BindAdapter;

//创建对外属性设置接口
BindAdapter.prototype.__defineGetter__("name", function () { return this._objname; });
BindAdapter.prototype.__defineSetter__("name", function (value) { this._objname = value; });

BindAdapter.prototype.__defineGetter__("servantName", function () { return this._servant; });
BindAdapter.prototype.__defineSetter__("servantName", function (value) { this._servant = value; });

BindAdapter.prototype.__defineGetter__("maxconns", function () { return this._maxconns; });
BindAdapter.prototype.__defineSetter__("maxconns", function (value) { this._maxconns = value; });

BindAdapter.prototype.__defineGetter__("endpoint", function () { return this._endpoint; });
BindAdapter.prototype.__defineSetter__("endpoint", function (value) { this._endpoint = value; });

BindAdapter.prototype.__defineGetter__("protocolName", function () { return this._protocolName; });
BindAdapter.prototype.__defineSetter__("protocolName", function (value) { this._protocolName = value; });

BindAdapter.prototype.__defineGetter__("protocol", function () { return this._protocol; });
BindAdapter.prototype.__defineSetter__("protocol", function (value) { this._protocol = value; });

BindAdapter.prototype.__defineGetter__("handleImp", function () { return this._handleImp; });
BindAdapter.prototype.__defineSetter__("handleImp", function (value) { this._handleImp = value; });

BindAdapter.prototype.isTarsProtocol = function () { //判断是否是tars协议
    return this._protocolName === "tars";
};

BindAdapter.prototype.start = function () {
    assert(this._endpoint.sProtocol === "tcp" || this._endpoint.sProtocol === "udp", "trans protocol must be tcp or udp");

    this._netthread = new NetThread(this, this._endpoint, this._protocol);
    this._netthread.initialize();
};

BindAdapter.prototype.stop = function () {
    this._netthread.stop();
};

//当传输端口上获得一个完整的请求包之后
BindAdapter.prototype.dispatch = function ($Transceiver, $protoMessage) {
    var current = new TarsCurrent();
    current._origin      = $protoMessage.origin;
    current._endpoint    = new Endpoint($Transceiver.socket.remoteAddress, $Transceiver.socket.remotePort);
    current._adapter     = this;
    current._transceiver = $Transceiver;
    current._timestamp   = TimeProvider.nowTimestamp();

    try {
        //处理非TARS协议
        if (this.isTarsProtocol() === false) {
            var Func = ($protoMessage.sFuncName && this.handleImp[$protoMessage.sFuncName]) || this.handleImp["doRequest"];
            if (Func) {
                Func.call(this.handle, current, $protoMessage.origin);
            } else {
                current.sendErrorResponse(TarsError.SERVER.FUNC_NOT_FOUND, "Not Found Func By Name:" + ($protoMessage.sFuncName?$protoMessage.sFuncName:"doRequest"));
            }
        }

        //处理tars协议
        if (this.isTarsProtocol() === true) {
            var iReturn = this._handleImp.onDispatch(current, $protoMessage.sFuncName, $protoMessage.origin.sBuffer);
            if (iReturn != TarsError.SUCCESS) {
                current.sendErrorResponse(iReturn, "");
            }
        }
    } catch (e) {
        current.sendErrorResponse(e.code != undefined?e.code:TarsError.SERVER.DECODE_ERROR, "BindAdapter Exception:" + e.message);
    }
};
