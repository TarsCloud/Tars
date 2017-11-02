/**
 * tars框架的连接代理类类
 */
var assert         = require("assert");
var TarsMonitor     = require("@tars/monitor");
var TimeProvider   = require("@tars/utils").timeProvider;
var TarsError       = require("../util/TarsError.js").TarsError;
var TQueue         = require("../util/TQueue.js").TQueue;
var TCPTransceiver = require("./Transceiver.js").TCPTransceiver;
var UDPTransceiver = require("./Transceiver.js").UDPTransceiver;

/**
 * 超时一定比率后进行切换
 * 设置超时检查参数
 * 计算到某台服务器的超时率, 如果连续超时次数或者超时比例超过阀值
 * 默认60s内, 超时调用次数>=2, 超时比率0.5或者连续超时次数>5,
 * 则失效
 * 服务失效后, 请求将尽可能的切换到其他可能的服务器, 并每隔tryTimeInterval尝试一次, 如果成功则认为恢复
 * 如果其他服务器都失效, 则随机选择一台尝试
 * 如果是灰度状态的服务, 服务失效后如果请求切换, 也只会转发到相同灰度状态的服务
 * @uint16_t minTimeoutInvoke, 计算的最小的超时次数, 默认2次(在checkTimeoutInterval时间内超过了minTimeoutInvoke, 才计算超时)
 * @uint32_t frequenceFailInvoke, 连续失败次数
 * @uint32_t checkTimeoutInterval, 统计时间间隔, (默认60s, 不能小于30s)
 * @float radio, 超时比例 > 该值则认为超时了 ( 0.1<=radio<=1.0 )
 * @uint32_t tryTimeInterval, 重试时间间隔
 */
var CheckTimeoutInfo = function ()
{
    this.minTimeoutInvoke       = 2;        //计算的最小的超时次数, 默认2次(在checkTimeoutInterval时间内超过了minTimeoutInvoke, 才计算超时)
    this.checkTimeoutInterval   = 60000;    //统计时间间隔, (默认60s, 不能小于30s)
    this.frequenceFailInvoke    = 5;        //连续失败次数
    this.minFrequenceFailTime   = 5;        //
    this.radio                  = 0.5;      //超时比例 > 该值则认为超时了 (0.1 <= radio <= 1.0)
    this.tryTimeInterval        = 30000;    //重试时间间隔，单位毫秒
};

//////////////////////////////////////////////定义接口代理类////////////////////////////////////////////////////////////
var AdapterProxy = function () {
    this._worker                = undefined;        //所属的ObjectProxy
    this._endpoint              = undefined;        //服务端的IP地址以及端口
    this._pTrans                = undefined;        //连接服务端的套接口

    this._pTimeoutQueueY        = new TQueue();     //当前端口上的已发送调用队列
    this._pTimeoutQueueN        = new TQueue();     //当前端口上的未发送调用队列

    this._activeStatus          = true;                     //当前端口上的可用状态
    this._checkTimeoutInfo      = new CheckTimeoutInfo();
    this._nextFinishInvokeTime  = 0;
    this._nextRetryTime         = 0;
    this._frequenceFailTime     = 0;
    this._frequenceFailInvoke   = 0;
    this._totalInvoke           = 0;
    this._timeoutInvoke         = 0;
};
module.exports.AdapterProxy = AdapterProxy;

// 设置接口代理类对外暴露的属性
AdapterProxy.prototype.__defineGetter__("worker",   function () { return this._worker; });
AdapterProxy.prototype.__defineSetter__("worker",   function (value) { this._worker = value;   });

AdapterProxy.prototype.__defineGetter__("endpoint", function () { return this._endpoint; });
AdapterProxy.prototype.__defineSetter__("endpoint", function (value) { this._endpoint = value; });

AdapterProxy.prototype.pushTimeoutQueueN = function(reqMessage)
{
    this._pTimeoutQueueN.push(reqMessage.request.iRequestId, reqMessage);
};

AdapterProxy.prototype.pushTimeoutQueueY = function(reqMessage)
{
    this._pTimeoutQueueY.push(reqMessage.request.iRequestId, reqMessage);
};

// 初始化代理类，主要生成网络传输类的实例
AdapterProxy.prototype.initialize = function () {
    assert(this._endpoint.sProtocol === "tcp" || this._endpoint.sProtocol === "udp", "trans protocol must be tcp or udp");

    if (this._endpoint.sProtocol === "tcp") {
        this._pTrans = new TCPTransceiver(this, this._endpoint);
    } else {
        this._pTrans = new UDPTransceiver(this, this._endpoint);
    }
};

//将队列返还给ObjectProxy，并调用worker的doInvoke，以选择其他节点重新发送
//如果不返还请求数据，请求量很大，干掉某个节点的时候，此节点中的请求可能会超时
AdapterProxy.prototype._resetQueue=function(){
    var self=this;
    var worker=self.worker;
    self._pTimeoutQueueY.forEach(function (key) {
        var reqMessage = self._pTimeoutQueueY.pop(key, true);
        delete reqMessage.adapter;
        delete reqMessage.RemoteEndpoint;
        worker.pTimeoutQueue.push(reqMessage.request.iRequestId, reqMessage);
    });
    self._pTimeoutQueueN.forEach(function (key) {
        var reqMessage = self._pTimeoutQueueN.pop(key, true);
        delete reqMessage.adapter;
        delete reqMessage.RemoteEndpoint;
        worker.pTimeoutQueue.push(reqMessage.request.iRequestId, reqMessage);
    });
    process.nextTick(function(){
        worker.doInvoke();
    });
}

// 销毁当前的连接代理类，在如下的时机：
// 第一，该服务端被从主控去除之后
// 第二，关闭当前的连接
AdapterProxy.prototype.destroy = function () {
    this._resetQueue();
    this._pTrans.close();
};

//请求返回了，并且Transceiver获得了一个完整的请求，由Transceiver回调该接口
AdapterProxy.prototype.doResponse = function ($rspMessage) {
    var reqMessage = this._pTimeoutQueueY.pop($rspMessage.iRequestId, true);
    //如果在当前已发送队列中没有发现该请求，有可能是因为超时被删除了,或者是单向请求压根没加入队列
    if (reqMessage === undefined) {
        return ;
    }

    reqMessage.LocalEndpoint  = this._pTrans.LocalEndpoint;
    reqMessage.clearTimeout();
    reqMessage.adapter.finishInvoke($rspMessage.iResultCode, reqMessage);

    if ($rspMessage.iResultCode === 0) {
        reqMessage.promise.resolve({request:reqMessage, response:$rspMessage.origin});
    } else {
        reqMessage.promise.reject ({request:reqMessage, response:$rspMessage.origin, error:{code:$rspMessage.iResultCode, message:$rspMessage.sResultDesc}});
    }
};

// 当在AdapterProxy内队列上的请求超时的时候，调用当前函数
AdapterProxy.prototype.doTimeout = function ($reqMessage) {
    // 有可能在任一队列上，无论那种情况尝试删除即可
    this._pTimeoutQueueY.erase($reqMessage.request.iRequestId);
    this._pTimeoutQueueN.erase($reqMessage.request.iRequestId);

    $reqMessage.clearTimeout();
    $reqMessage.adapter.finishInvoke(TarsError.CLIENT.REQUEST_TIMEOUT, $reqMessage);

    $reqMessage.promise.reject({request:$reqMessage, response:undefined, error:{code:TarsError.CLIENT.REQUEST_TIMEOUT, message:"call remote server timeout(remote server no response)"}});
};

// 发送积压的数据
// 当前代理类重新连接或者第一次连接上对端服务之后，回调该函数
AdapterProxy.prototype.doInvoke = function () {
    // 遍历未发送的请求
    var self = this;
    self._pTimeoutQueueN.forEach(function (key) {
        var reqMessage = self._pTimeoutQueueN.pop(key, true);
        if (self._pTrans.sendRequest(reqMessage.request)) {
            //单向请求不需要向请求成功队列里边加入数据，而是立即reject
            if(reqMessage.request.property && reqMessage.request.property.packetType === 1){
                reqMessage.adapter._doOneWayEnd(reqMessage);
            }
            else{
                self._pTimeoutQueueY.push(reqMessage.request.iRequestId, reqMessage);
            }
        } else {
            self._doInvokeException(reqMessage);
        }
    });

    // 看ObjectProxy上是否有数据需要发送
    self._worker.doInvoke();
};
//单向请求发送成功后即调用此函数，避免请求数据长时间在_pTimeoutQueueY队列中直到超时，占用内存
AdapterProxy.prototype._doOneWayEnd = function (reqMessage) {
    reqMessage.clearTimeout();
    //标记一下调用成功，避免触发节点失活策略
    reqMessage.adapter.finishInvoke(0, reqMessage);
    reqMessage.promise.reject({request:reqMessage, response:undefined, error:{code:reqMessage.iResultCode, message:"one way call do not need response"}});
}
// 从当前积压队列中再次发送数据时失败，则不再尝试，直接作为异常处理
AdapterProxy.prototype._doInvokeException = function ($reqMessage) {
    $reqMessage.clearTimeout();
    $reqMessage.adapter.finishInvoke(TarsError.CLIENT.ADAPTER_NOT_FOUND, $reqMessage);
    $reqMessage.promise.reject({request:$reqMessage, response:undefined, error:{code:TarsError.CLIENT.ADAPTER_NOT_FOUND, message:"call remote server error(send again error)"}});
};

AdapterProxy.prototype.invoke = function ($reqMessage) {
    $reqMessage.adapter = this;
    $reqMessage.RemoteEndpoint = this._endpoint;

    if (this._pTrans.sendRequest($reqMessage.request)) {
        if($reqMessage.request.property && $reqMessage.request.property.packetType === 1){
            //单向请求不需要向请求成功队列里边加入数据
            $reqMessage.adapter._doOneWayEnd($reqMessage);
        }
        else{
            //如果非单向请求发送成功，则放入发送成功队列
            this._pTimeoutQueueY.push($reqMessage.request.iRequestId, $reqMessage);
        }
    } else {
        this._pTimeoutQueueN.push($reqMessage.request.iRequestId, $reqMessage); //如果发送失败，则放入发送失败队列，稍后尝试再次发送
    }
};

// 设置当前代理类为无效
AdapterProxy.prototype._setInactive = function () {
    this._activeStatus  = false;
    this._nextRetryTime = TimeProvider.nowTimestamp() + this._checkTimeoutInfo.tryTimeInterval;
    this._pTrans.close();
};

// 判断当前的接口代理实例是否可用
AdapterProxy.prototype.checkActive = function ($bForceConnect) {
    if (this._pTrans.hasConnected() || this._pTrans.isConnecting()) {
        return true;
    }

    var nowTime = TimeProvider.nowTimestamp();

    //如果是需要强制连接
    if ($bForceConnect) {
        this._nextRetryTime = nowTime + this._checkTimeoutInfo.tryTimeInterval;
        if (!this._pTrans.isValid()) {
            this._pTrans.reconnect();
        }
        return this._pTrans.hasConnected() || this._pTrans.isConnecting();
    }
    //失效且没有到下次重试时间, 直接返回不可用
    if (!this._activeStatus && nowTime < this._nextRetryTime) {
        return false;
    }

    //如果还没有连接过，那么就重新连接
    if (!this._pTrans.isValid()) {
        this._pTrans.reconnect();
    }

    if (!this._activeStatus) {
        this._nextRetryTime = nowTime + this._checkTimeoutInfo.tryTimeInterval;
    }

    //返回当前AdapterProxy是否可用
    return this._pTrans.hasConnected() || this._pTrans.isConnecting();
};

//处理当前代理类有效性的检查等功能
AdapterProxy.prototype._doFinishInvoke = function ($iResultCode) {
    var nowTime = TimeProvider.nowTimestamp();

    //处于异常状态
    if (this._activeStatus === false) {
        if ($iResultCode === 0) { //调用成功的情况下，重置统计数据
            this._activeStatus          = true;
            this._frequenceFailInvoke   = 0;
            this._totalInvoke           = 1;
            this._timeoutInvoke         = 0;
            this._nextFinishInvokeTime  = nowTime + this._checkTimeoutInfo.checkTimeoutInterval;
        } else {
            console.info("[TARS][AdapterProxy::finishInvoke(bool), ", this._worker.name, ", " ,this._endpoint.toString(), ", retry fail]");
        }
        return ;
    }

    //处于正常状态
    this._totalInvoke++;

    if ($iResultCode !== 0) { //调用失败
        this._timeoutInvoke++;
        if (this._frequenceFailInvoke === 0) {
            this._frequenceFailTime = nowTime + this._checkTimeoutInfo.minFrequenceFailTime;
        }
        this._frequenceFailInvoke++;

        //检查是否到了连续失败次数,且至少在5s以上
        if (this._frequenceFailInvoke >= this._checkTimeoutInfo.frequenceFailInvoke && nowTime >= this._frequenceFailTime) {
            this._setInactive();
        }
    } else { //调用成功
        this._frequenceFailInvoke = 0;
    }

    //判断一段时间内的超时比例
    if (nowTime > this._nextFinishInvokeTime) {
        this._nextFinishInvokeTime = nowTime + this._checkTimeoutInfo.checkTimeoutInterval;
        if ($iResultCode !== 0 && this._timeoutInvoke >= this._checkTimeoutInfo.minTimeoutInvoke && this._timeoutInvoke >= parseInt(this._checkTimeoutInfo.radio * this._totalInvoke)) {
            this._setInactive();
        } else {
            this._totalInvoke   = 0;
            this._timeoutInvoke = 0;
        }
    }
};

AdapterProxy.prototype.finishInvoke = function ($iResultCode, $reqMessage) {
    $reqMessage = $reqMessage || {};
    this._doFinishInvoke($iResultCode);

    var packetType = 0;
    if($reqMessage.request.property && $reqMessage.request.property.packetType === 1)
        packetType = 1;

    //如果是在正式环境，则上报统计信息(单向调用不需要上报)
    if (process.env.TARS_MONITOR && packetType !== 1) {
        //正式环境需要上报统计数据

        var masterSetInfo = '', masterModuleName = '';
        var arr = [], slaveSetName = '', slaveSetArea = '', slaveSetID = '', servantArr;
        var communicator = this._worker._comm;
        var masterConfig = communicator.configure;

        masterModuleName = masterConfig.get("tars.application.client.modulename", '');

        var enableSet = masterConfig.get("tars.application.enableset", 'n');

        if(enableSet == 'y' || enableSet == 'Y')
        {
            var setDivision = masterConfig.get("tars.application.setdivision", undefined);
            if(setDivision)
            {
                masterSetInfo = setDivision;
                arr = setDivision.split(".");
                masterModuleName += '.' + arr.join('');
            }
        }

        var servantName = $reqMessage.request.sServantName;
        arr = servantName.split('.');
        arr = arr.slice(0, 2);
        servantName = arr.join('.');

        var setId       = $reqMessage.request.setId;

        var headers = {
            masterName    : masterModuleName,
            slaveName     : servantName,
            interfaceName : $reqMessage.request.sFuncName,
            masterIp      : this._worker.communicator.ClientConfig.LocalIp,
            slaveIp       : this._endpoint.sHost,
            slavePort     : this._endpoint.iPort,
            bFromClient   : true
        };

        if(setId)
        {
            servantArr = servantName.split('.');

            arr = setId.split(".");
            if(arr && arr.length >= 3)
            {
                slaveSetName         = arr[0];
                slaveSetArea         = arr[1];
                slaveSetID           = arr[2];

                headers.slaveName    = servantArr[0] + '.'+ servantArr[1] + '.' + arr.join('');

                headers.slaveSetName = slaveSetName;
                headers.slaveSetArea = slaveSetArea;
                headers.slaveSetID   = slaveSetID;
            }
        }


        if(masterSetInfo)
            headers.masterSetInfo = masterSetInfo;

        if(!($reqMessage.costtime >= 0) || !isFinite($reqMessage.costtime))
        {
            $reqMessage.costtime = 0;
        }

        if ($iResultCode === 0 && $reqMessage === undefined) {
            TarsMonitor.stat.report(headers, TarsMonitor.stat.TYPE.SUCCESS, 0);
        }

        if ($iResultCode === 0 && $reqMessage !== undefined) {
            TarsMonitor.stat.report(headers, TarsMonitor.stat.TYPE.SUCCESS, $reqMessage.costtime);
        }

        if ($iResultCode !== 0 && $reqMessage === undefined) {
            TarsMonitor.stat.report(headers, TarsMonitor.stat.TYPE.ERROR, 0);
        }

        if ($iResultCode !== 0 && $reqMessage !== undefined) {
            TarsMonitor.stat.report(headers, $iResultCode === TarsError.CLIENT.REQUEST_TIMEOUT?TarsMonitor.stat.TYPE.TIMEOUT:TarsMonitor.stat.TYPE.ERROR, $reqMessage.costtime);
        }
    }
};