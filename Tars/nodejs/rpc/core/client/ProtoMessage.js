//框架 ---> 客户端编码器
var ProtoMessageRequest = function () {
    this.iRequestId   = 0;
    this.sFuncName    = undefined;
    this.appBuffer    = undefined;      //业务需要发送的数据
    this.property     = undefined;      //业务附加数据
};

//客户端解码器 ---> 框架
var ProtoMessageResponse = function () {
    this.origin       = undefined;      //原始的请求结构体,从协议中解出来的第一层结构体
    this.iRequestId   = 0;              //
    this.iResultCode  = 0;              //客户端解码时后的错误编码，为0时框架认为成功，其他错误码将强制关闭连接
    this.sResultDesc  = "";             //客户端解码时后的错误消息
};

module.exports.Request  = ProtoMessageRequest;
module.exports.Response = ProtoMessageResponse;
