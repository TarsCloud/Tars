//服务端编码器 ---> 框架
var ProtoMessageRequest = function () {
    this.origin       = undefined;
    this.sFuncName    = undefined;
    this.iResultCode  =  0;
    this.sResultDesc  = "";
};

//框架 ---> 服务端编码器
var ProtoMessageResponse = function () {
    this.origin       = undefined;   //原始的请求结构体,从协议中解出来的第一层结构体
    this.iResultCode  = 0;           //tars框架的错误代码
    this.sResultDesc  = "";          //tars框架的错误消息
    this.appBuffer    = undefined;   //业务返回给客户端的数据
    this.property     = undefined;
};

module.exports.Request  = ProtoMessageRequest;
module.exports.Response = ProtoMessageResponse;
