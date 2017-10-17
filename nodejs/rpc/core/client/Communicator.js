/**
 * tars框架的客户端通信器实现类
 */
var assert      = require("assert");
var events      = require('events');
var TarsMonitor  = require("@tars/monitor");
var Configure   = require("@tars/utils").Config;
var ObjectProxy = require("./ObjectProxy.js").ObjectProxy;

//定义通信器
var Communicator = function () {
    this._property      = {}; //通信器的配置项
    this._workers       = {}; //ObjectProxy的映射表
    this._configure     = undefined;
    this._initialized   = false;
    this._masterName    = "";
};
module.exports.Communicator = Communicator;

Communicator.prototype.__defineGetter__("configure", function () { return this._configure; });

Communicator.prototype.__defineGetter__("masterName", function () { return this._masterName; });

//全局函数，用于创建一个通信器
Communicator.New = function () {
    return new Communicator();
};

//通信器全局属性
Communicator.prototype.ClientConfig = {
    SetOpen     : false,
    SetDivision : "NULL",
    ModuleName  : "NO_MODULE_NAME",
    LocalIp     : ""
};

//设置通信器的属性值
Communicator.prototype.setProperty = function ($key, $value) { this._property[$key] = $value; this._initProperty();     };
Communicator.prototype.getProperty = function ($key, $value) { return this._property[$key]?this._property[$key]:$value; };

//使用配置文件初始化通信器
Communicator.prototype.initialize  = function ($sFilePath) {
    if (this._initialized || ($sFilePath === undefined && process.env.TARS_CONFIG === undefined)) { //如果已经初始化过，或者配置文件路径为空，则直接返回
        return ;
    }

    this.bindUpdateLocatorEvent();

    this._initialized = true;

    //读取配置文件，设置属性
    this._configure = new Configure();
    this._configure.parseFile($sFilePath || process.env.TARS_CONFIG);

    TarsMonitor.stat.init(this._configure.json);

    this.ClientConfig.ModuleName  = this._configure.get("tars.application.client.modulename", "NO_MODULE_NAME");
    this.ClientConfig.LocalIp     = this._configure.get("tars.application.server.localip", "");
    this._masterName              = this.ClientConfig.ModuleName;

    this.getProperty("locator") || this.setProperty("locator",     this._configure.get("tars.application.client.locator", undefined));            //设置主控地址
    this.getProperty("timeout") || this.setProperty("timeout",     this._configure.get("tars.application.client.async-invoke-timeout", 30000));   //设置请求调用超时时间，默认为30s
    this.getProperty("setdivision") || this.setProperty("setdivision", this._configure.get("tars.application.setdivision", "NULL"));
    this.getProperty("enableset") || this.setProperty("enableset",   this._configure.get("tars.application.enableset", "n"));
};

//生成代理类接口
Communicator.prototype.stringToProxy = function ($ProxyHandle, $objName, $SetName) {
    this.initialize();

    //先检查设置的objName是否格式正确
    assert($objName != undefined, "please specify the parameter value for objName");
    assert($objName.valueOf("@") != -1 || $objName.valueOf("@") == -1 && this.getProperty("locator") != undefined, "please specify the parameter value for registry locator");

    $objName = $objName.replace(/^[\s\t ]+|[\s\t ]+$/g, '');
    $objName = $objName.replace(/\s{2,}/g, ' ');

    //创建代理类
    var proxy = new $ProxyHandle();
    proxy._name   = $objName;
    proxy._worker = this._createObjectProxy($objName, $SetName?$SetName:""); //生成底层使用的ObjectProxy类实例
    proxy._rpc    = this._createRPC(proxy);                                  //当用在第三方协议的似乎，用来生成函数以及调用对应函数

    return proxy;
};

//析构当前的通信器
Communicator.prototype.destroy = function () {
    for (var key in this._workers) {
        this._workers[key].destroy();

        delete this._workers[key];
    }
};

Communicator.prototype.disconnect = function () { //兼容老版本的接口
    this.destroy();
};

//检查是否开起SET模型
Communicator.prototype._initProperty = function () {
    if (this.getProperty("enableset", "n") === "y" || this.getProperty("enableset", "n") === "Y") {
        this.ClientConfig.SetOpen     = true;
        this.ClientConfig.SetDivision = this.getProperty("setdivision", "NULL");

        // 检查SET格式是否正确
        var setDivision = this.ClientConfig.SetDivision.split(".");
        if (setDivision.length != 3 || setDivision[0] === "*" || setDivision[1] === "*") {
            this.ClientConfig.SetOpen     = false;
            this.ClientConfig.SetDivision = "NULL";
            this.setProperty("enableset", "n");
        }

        // 设置上报使用的MasterName
        if (this.ClientConfig.SetOpen) {
            var masterName   = this.ClientConfig.ModuleName;
            var setDivision  = this.ClientConfig.SetDivision.split(".");

            if (masterName.indexOf(".") !== -1) {
                masterName   = masterName.slice(masterName.indexOf('.') + 1);
            }

            this._masterName = setDivision[0] + setDivision[1] + setDivision[2] + "." + masterName;
        }
    }
};

//生成ObjectProxy类实例
Communicator.prototype._createObjectProxy = function ($ObjectName, $SetName) {
    var tmpObjName = $ObjectName + ":" + ($SetName === undefined?"":$SetName);
    if (this._workers.hasOwnProperty(tmpObjName)) {
        return this._workers[tmpObjName];
    }

    var worker = new ObjectProxy();
    worker.communicator = this;
    var timeout = this.getProperty("timeout"); //为ObjectProxy设置一个请求超时时间
    if(timeout)
    {
        worker.timeout      = timeout;
    }
    worker.initialize($ObjectName, $SetName);

    this._workers[tmpObjName] = worker;

    return worker;
};

//创建RPC调用的句柄
Communicator.prototype._createRPC = function (Handle) {
    return {
        createFunc : function (FuncName) {
            this[FuncName] = function () {
                var argsLen = arguments.length;
                var args = new Array(argsLen);
                for (var i = 0; i < argsLen; i += 1) {
                    args[i] = arguments[i];
                }
                
                return Handle.rpc_call.apply(Handle, Array.prototype.concat.apply([Handle._worker.genRequestId(), FuncName], args));
            }

            return this[FuncName];
        }
    };
};

// 更新locator
Communicator.prototype.bindUpdateLocatorEvent = function () {
    var self = this;
    function updateLocator()
    {
        var config = new Configure();
        config.parseFile(process.env.TARS_CONFIG);
        var locator = config.get("tars.application.client.locator", undefined);
        self.setProperty("locator", locator);
    }

    // 突破process.on的限制
    if(!global._tarsRpcEvents)
    {
        global._tarsRpcEvents = new events.EventEmitter();
        global._tarsRpcEvents.setMaxListeners(0);
        process.on('message', function(message) {
            global._tarsRpcEvents.emit('message', message);
        });
    }

    global._tarsRpcEvents.on('message', function(message){
        if (message && message.cmd === 'tars.loadproperty') {
            updateLocator();
        }
    });
};
