var TimeProvider = require("@tars/utils").timeProvider;
var ProtoMessage = require("./ProtoMessage.js");

//////////////////////////////////////////////////请求队列//////////////////////////////////////////////////////////////
var ReqMessage = function () {
    this.request        = new ProtoMessage.Request();
    this.worker         = undefined;
    this.promise        = undefined;
    this.timer          = undefined;    //请求超时定时器
    this.adapter        = undefined;
    this.property       = undefined;
    this.costtime       = 0;            //本次请求完成的时间，单位毫秒
    this.startTime      = 0;            //本次请求开始的内部时间点
    this.LocalEndpoint  = undefined;    //本次调用本地地址
    this.RemoteEndpoint = undefined;    //本次调用远端地址
};
module.exports.ReqMessage = ReqMessage;

ReqMessage.prototype.setTimeout = function (iTimeout) {
    var self = this;

    self.startTime = TimeProvider.nowTimestamp();
    self.timer     = setTimeout(function () {
        self.adapter?self.adapter.doTimeout(self):self.worker.doTimeout(self);
    }, iTimeout);
};

ReqMessage.prototype.clearTimeout = function () {
    clearTimeout(this.timer);
    this.timer    = undefined;
    this.costtime = TimeProvider.nowTimestamp() - this.startTime;
};
