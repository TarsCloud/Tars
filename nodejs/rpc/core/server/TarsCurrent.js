var Dyeing    = require("@tars/dyeing");
var TimeProvider = require("@tars/utils").timeProvider;
var Monitor   = require("@tars/monitor");
var ProtoMessage = require("./ProtoMessage.js");
var Configure    = require("@tars/utils").Config;


///////////////////////////////////////////////程序运行的上下文信息/////////////////////////////////////////////////////
var TarsCurrent = function () {
    this._origin         = undefined;    //原始的请求结构体
    this._endpoint       = undefined;    //客户端的IP地址
    this._adapter        = undefined;    //BindAdapter的指向
    this._servant        = undefined;    //处理类
    this._transceiver    = undefined;    //连接
    this._timestamp      = 0;            //接受到完整请求包的时间
    this._rspcontext     = {};
    this._configure      = undefined;    //配置
};
module.exports.TarsCurrent = TarsCurrent;

/***
 * 对外提供的获取接口
 ***/
TarsCurrent.prototype.__defineGetter__("application",  function () { return this._adapter.handleImp.application; });

TarsCurrent.prototype.__defineGetter__("communicator", function () { return this._adapter.handleImp.application.communicator; });

//获取客户端的IP和端口
TarsCurrent.prototype.getEndpoint = function () {
    return this._endpoint;
};

//仅tars协议有效.请求的协议的版本号
TarsCurrent.prototype.getRequestVersion = function () {
    return this._origin?this._origin.iVersion:undefined;
};

//请求ID
TarsCurrent.prototype.getRequestId = function () {
    return this._origin?this._origin.iRequestId:undefined;
};

//仅tars协议有效.获取扩展map
TarsCurrent.prototype.getContext = function () {
    return this._origin?this._origin.context:undefined;
};

//仅tars协议有效.保存状态信息，比如灰度、染色等
TarsCurrent.prototype.getRequestStatus = function () {
    return this._origin?this._origin.status:undefined;
};

//设置返回的context
TarsCurrent.prototype.setResponseContext = function ($context) {
    this._rspcontext = $context;
};

//获取原始的请求数据
TarsCurrent.prototype.getRequest = function () {
    return this._origin;
};

//适用于第三份协议时的回包函数
TarsCurrent.prototype.sendResponse = function () {
    var response = new ProtoMessage.Response();
    response.origin       = this._origin;
    response.appBuffer    = Array.prototype.slice.call(arguments, 0);
    response.iResultCode  = 0;
    response.sResultDesc  = "";

    this._transceiver.send(response);
};

// 仅tars协议有效.开发者调用sendResponse，编码之后，由框架调用该函数返回数据
TarsCurrent.prototype.doResponse = function ($BinBuffer) {
    // 处理三种需要主动上报的数据[tup_client,one_way_client,not_tars_client]
    this._report();

    // 如果是tars协议并且是单向调用则直接返回
    if (this._adapter.isTarsProtocol() && this._origin.cPacketType == 1) { //1表示tarsONEWAY，单向调用
        return ;
    }

    // 生成一个向协议解析器传递的结构体
    var response = new ProtoMessage.Response();
    response.origin       = this._origin;
    response.appBuffer    = $BinBuffer;
    response.iResultCode  = 0;
    response.sResultDesc  = "";
    response.property     = {context:this._rspcontext};

    this._transceiver.send(response);
};

// 当出现错误时，需要给个客户端返回一个错误消息是调用
TarsCurrent.prototype.sendErrorResponse = function ($iRet, $sMessage) {
    // 如果是tars协议并且是单向调用则直接返回
    if (this._adapter.isTarsProtocol() && this._origin.cPacketType == 1) { //1表示tarsONEWAY，单向调用
        return ;
    }

    // 生成一个向协议解析器传递的结构体
    var response = new ProtoMessage.Response();
    response.origin       = this._origin;
    response.appBuffer    = undefined;
    response.iResultCode  = $iRet;
    response.sResultDesc  = $sMessage;

    this._transceiver.send(response);
};

//获取原始请求的染色情况
TarsCurrent.prototype.getDyeingObj = function () {
    var bDyeing    = this._origin.iMessageType & 0x04;
    var sDyeingKey = this._origin.status.get("STATUS_DYED_KEY") || '';

    return Dyeing.gen(bDyeing, sDyeingKey);
};

// 处理三种需要主动上报的数据[tup_client,one_way_client,not_tars_client]
TarsCurrent.prototype._report = function () {
    // 判断是否在测试环境中，非正式环境不上报
    if (process.env.TARS_MONITOR === undefined) {
        return ;
    }
    if (this.communicator === undefined) { //如果服务端没有初始化通信器的情况下，则不上报
        return ;
    }

    var sReportType = "";
    if (this._origin.iVersion === 2 || this._origin.iVersion === 3) { //如果是tup协议调用
        sReportType = "tup_client";
    } else if (this._origin.cPacketType === 1) { //如果是单向调用
        sReportType = "one_way_client";
    } else if (this._adapter.isTarsProtocol() === false) { //如果是第三方调用方式
        sReportType = "not_tars_client";
    } else { //除上述三种情况之外，直接返回
        return ;
    }

    var slaveSetName = '', slaveSetArea = '', slaveSetID = '', slaveSetString = '', slaveName = '';

    if(process.env.TARS_CONFIG)
    {
        // 读取配置文件
        if(!this._configure)
        {
            this._configure = new Configure();
            this._configure.parseFile(process.env.TARS_CONFIG);
        }
    }

    if(this._configure)
    {
        var enableSet = this._configure.get("tars.application.enableset", 'n');
        slaveName = this._configure.get("tars.application.client.modulename", '');

        if(enableSet == 'y' || enableSet == 'Y')
        {
            var setDivision = this._configure.get("tars.application.setdivision", undefined);
            if(setDivision)
            {
                slaveSetString = setDivision;
                var arr = slaveSetString.split('.');
                slaveSetName = arr[0] || '';
                slaveSetArea = arr[1] || '';
                slaveSetID = arr[2] || '';

                slaveName += '.' + arr.join('');
            }
        }

    }

    var headers = {
        masterName    : sReportType,
        slaveName     : slaveName,
        interfaceName : this._origin.sFuncName,
        masterIp      : this._endpoint.sHost,
        slaveIp       : "",
        slavePort     : 0,
        bFromClient   : false
    };

    if(slaveSetName)
        headers.slaveSetName = slaveSetName;

    if(slaveSetArea)
        headers.slaveSetArea = slaveSetArea;

    if(slaveSetID)
        headers.slaveSetID = slaveSetID;

    Monitor.stat.report(headers, Monitor.stat.TYPE.SUCCESS, TimeProvider.nowTimestamp() - this._timestamp);
};
