var Registry     = require("@tars/registry");
var Endpoint     = require("@tars/utils").Endpoint;
var TimeProvider = require("@tars/utils").timeProvider;
var AdapterProxy = require("./AdapterProxy.js").AdapterProxy;

/////////////////////////////////////////////主控地址管理类/////////////////////////////////////////////////////////////
var EndpointManager = function ($RefObjectProxy, $RefCommunicator, $ObjectName, $SetName) {
    this._objname   = $ObjectName;      //分离之后ServantName的名字
    this._setname   = $SetName;         //SET的名称
    this._worker    = $RefObjectProxy;  //ObjectProxy引用
    this._comm      = $RefCommunicator; //通信器引用
    this._adapters  = [];               //当前服务下所有的通信代理类
    this._hash      = 0;                //Hash值
    this._direct    = false;            //是否是直连服务，否则需要走主控查询可用地址+端口
    this._bRequest  = false;            //是否正在请求中

    this._iRefreshTime          = 0;       //IPList的刷新时间
    this._iRefreshInterval      = 60000;   //请求列表的频率，单位毫秒
    this._iActiveEmptyInterval  = 10000;   //请求回来活跃列表为空的间隔时间，单位毫秒
    this._iRequestTimeout       = 0;       //请求的超时时间(绝对时间) 防止请求回调丢了。一直在正在请求状态
    this._iWaitTime             = 5000;    //请求主控的等待时间，单位毫秒
    this._loacator              = undefined; // 主控

    this._initialize($ObjectName);
};
module.exports.EndpointManager = EndpointManager;

//解析服务端名称，发现是否是直连端口
EndpointManager.prototype._initialize = function ($ObjectProxyName) {
    var options = $ObjectProxyName.split('@');
    if (options.length != 2) {
        return;
    }

    this._objname = options[0];
    this._direct  = true;
    var endpoints = options[1].split(":");
    for (var i = 0; i < endpoints.length; i++) {
        this._addEndpoint(Endpoint.parse(endpoints[i]));
    }
};

EndpointManager.prototype.getAllAdapters = function()
{
    return this._adapters || [];
};

EndpointManager.prototype._addEndpoint = function ($endpoint) {
    var adapter = new AdapterProxy();
    adapter.worker   = this._worker;
    adapter.endpoint = $endpoint;
    adapter.initialize();

    this._adapters.push(adapter);
    return adapter;
};

EndpointManager.prototype._doEndpointsException = function () {
    this._bRequest     = false;
    this._iRefreshTime = TimeProvider.nowTimestamp() + 2000; //频率控制获取主控失败 2秒钟再更新
};

//根据传入的协议、地址以及端口号查找是否已在列表中
EndpointManager.prototype._findEndpointByInfo = function ($endpoint) {
    for (var i = 0; i < this._adapters.length; i++) {
        var adapter = this._adapters[i];
        if (adapter._endpoint.sProtocol === $endpoint.sProtocol && adapter._endpoint.sHost === $endpoint.sHost && adapter._endpoint.iPort === $endpoint.iPort) {
            return adapter;
        }
    }

    return undefined;
};

//非直连的情况下，到主控查询活动列表
EndpointManager.prototype._dns = function () {
    var self = this;
    var nowTime = TimeProvider.nowTimestamp();

    //01 先判断是否可以查主控
    if (self._direct) { //如果是直连，不查主控
        return ;
    }
    if (self._bRequest && self._iRequestTimeout < nowTime) { //如果正在请求中，但已经超时，则重置
        self._doEndpointsException();
    }
    if (self._bRequest || self._iRefreshTime > nowTime) { //如果正在请求，或者还没到重试时间
        return ;
    }

    //02 定义请求主控的处理函数
    function doEndpoints (result) { //如果请求主控服务成功
        if (result.response.return != 0) {
            return self._doEndpointsException();
        }
        var nowTime = TimeProvider.nowTimestamp();
        self._bRequest = false;
        self._iRefreshTime = nowTime + (result.response.arguments.activeEp.value.length > 0?self._iRefreshInterval:self._iActiveEmptyInterval);

        //01 将新的节点接入到活动列表中
        var actives   = [], i = 0;
        var endpoints = result.response.arguments.activeEp;
        for (i = 0, len = endpoints.value.length; i < len; i++) {
            var newEndpoint = new Endpoint();
            newEndpoint.sProtocol = endpoints.value[i].istcp?"tcp":"udp";
            newEndpoint.sHost     = endpoints.value[i].host;
            newEndpoint.iPort     = endpoints.value[i].port;
            newEndpoint.setId     = endpoints.value[i].setId || '';

            if (self._findEndpointByInfo(newEndpoint) === undefined) { //如果不在已有列表中，则新加入
                self._addEndpoint(newEndpoint, true);
            }
            actives.push(newEndpoint);
        }
        //02 将已经去除的地址，从活动列表中删除
        for (i = 0; i < self._adapters.length; i++) {
            var bFound   = false;
            var endpoint = self._adapters[i]._endpoint;

            //查找该地址是否在活动列表中
            for (var ii = 0, len = actives.length; ii < len; ii++) {
                if (endpoint.sProtocol === actives[ii].sProtocol && endpoint.sHost === actives[ii].sHost && endpoint.iPort === actives[ii].iPort) {
                    bFound = true;
                    break;
                }
            }

            //如果在活动列表中没有找到，则说明该地址被停止了或者去除了，则从当前的队列中删除
            if (!bFound) {
                self._adapters[i].destroy();
                self._adapters.splice(i, 1);
                i--;
            }
        }
        //03 通知对应的ObjectProxy可以发送缓存中的请求了
        self._worker.doInvoke();
    }

    var doEndpointsFault = function (result) { //如果请求主控服务失败
        self._doEndpointsException();
    };

    //03 根据是否开起SET化查询不同的活动IPList
    self._bRequest = true;
    self._iRequestTimeout = nowTime + self._iWaitTime;
    var locator = self._comm.getProperty("locator");
    if(!self._loacator)
    {
        // 第一次加载主控
        self._loacator = locator;
        Registry.setLocator(locator);
    }
    else
    {
        // 非首次加载主控
        if(self._loacator != locator)
        {
            // 主控发生改变
            console.log('[locator] [old]' + self._loacator + ' [new] ' + locator);
            self._loacator = locator;
            Registry.resetLocator(locator);
        }
    }

    if (self._comm.ClientConfig.SetOpen || self._setname.length > 0) {
        Registry.findObjectByIdInSameSet(self._objname, self._setname.length > 0?self._setname:self._comm.ClientConfig.SetDivision).then(doEndpoints, doEndpointsFault);
    } else {
        Registry.findObjectByIdInSameGroup(self._objname).then(doEndpoints, doEndpointsFault);
    }
};

//按照一定的规则选取一个可用的服务连接
EndpointManager.prototype.selectAdapterProxy = function (reqMessage) {
    //01 如果是非直连，则进入请求主控处理逻辑
    !this._direct && this._dns();

    //02 hashcode 调用
    var adapter, adapters = [];
    var hashCode = +reqMessage.request.property['hashCode'];

    if (this._adapters && this._adapters.length > 0) {
        adapters = this._adapters.slice(0);
    }

    if (hashCode && !isNaN(hashCode) && adapters.length > 0) {
        // hash映射
        var hash;

        do {
            hash = hashCode % adapters.length;
            adapter = adapters[hash];
            if(adapter.checkActive()) {
                return adapter;
            }

            adapters.splice(hash, 1);
        } while (adapters.length > 0);

        adapters = this._adapters.slice(0);
        hash = hashCode % adapters.length;
        adapter = adapters[hash];
        //强制连接
        adapter.checkActive(true);
        //此处需要检查并赋值adapter属性和RemoteEndpoint属性，否则超时过久内存泄露、且节点重连时若请求被处理，doResponse会报错
        reqMessage.adapter=adapter;
        reqMessage.RemoteEndpoint=adapter._endpoint;
        adapter.pushTimeoutQueueN(reqMessage);
        return;
    }

    // 03 检查是否有可用的连接
    for (var i = 0; i < this._adapters.length; i++) {
        adapter = this._adapters[(this._hash++)%this._adapters.length];
        if (adapter.checkActive()) {
            return adapter;
        }
    }

    if (this._adapters.length > 0) { // 如果当前没有可用的连接，则随机选取一个
        adapter = this._adapters[(this._hash++)%this._adapters.length];
        adapter.checkActive(true); //强制连接
    }

    // 04 返回可用的连接代理类
    return;
};

//销毁
EndpointManager.prototype.destroy = function () {
    for (var i = 0; i < this._adapters.length; i++) {
        this._adapters[i].destroy();
    }
};
