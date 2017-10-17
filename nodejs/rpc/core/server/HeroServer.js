var TarsProtocol  = require("../rpc-tars").server;
var Endpoint     = require("@tars/utils").Endpoint;
var Configure    = require("@tars/utils").Config;
var TarsMonitor   = require("@tars/monitor");
var Communicator = require("../client/Communicator.js").Communicator;
var BindAdapter  = require("./BindAdapter.js").BindAdapter;

///////////////////////////////////////////////封装标准tars服务//////////////////////////////////////////////////////////
var HeroServer = function () {
    this._adapters       = [];          //当前服务上的所有绑定端口
    this._servants       = {};          //当前服务上的所有绑定端口对应的处理类
    this._configure      = undefined;
    this._configureFile  = "";
    this._communicator   = undefined;   //框架已经初始化的通信器
    this.Application     = "";
    this.ServerName      = "";
};
module.exports.HeroServer = HeroServer;

//创建一个BindAdapter的参数
HeroServer.BindAdapterOption = function () {
    this.endpoint     = "";         //必选。当前BindAdapter的监听地址，比如tcp -h 127.0.0.1 -p 14002 -t 10000
    this.name         = "";         //可选。当前BindAdapter的名字
    this.servant      = "";         //可选。当前BindAdapter上的Servant的名字
    this.maxconns     = 1024;       //可选。当前BindAdapter上可以支持的最大链接数
    this.protocolName = "tars";      //可选。当前BindAdapter上的协议解析器的名字
    this.protocol     = undefined;  //可选。当前BindAdapter上的协议解析器，如果不设置或者为"tars"则默认为tars协议
    this.handleImp    = undefined;  //必选。当前BindAdapter上的请求处理类
};

//定义访问属性
HeroServer.prototype.__defineGetter__("communicator", function () { return this._communicator;});

HeroServer.prototype.__defineGetter__("configure", function () { return this._configure; });

//使用配置文件初始化服务
HeroServer.prototype.initialize = function ($sConfigFile, $initFunction) {
    this._configureFile = $sConfigFile;
    this._configure     = new Configure();
    this._configure.parseFile(this._configureFile);

    this._initializeClient(this._configureFile);

    this._initializeServer();

    TarsMonitor.stat.init(this._configure.json);

    $initFunction && $initFunction(this);
};

//为服务Servant增加处理类
HeroServer.prototype.addServant = function ($ServantHandle, $ServantName) {
    this._servants[$ServantName] = $ServantHandle;
};

//启动服务
HeroServer.prototype.start = function (option) { //option的类型为：BindAdapterOption
    option && this._addBindAdapter(option);
    this._waitForShutdown();
};

//停止服务
HeroServer.prototype.stop = function () {
    this._communicator && this._communicator.destroy(); //如果有初始化通信器，则把通信器关闭

    for (var i = 0; i < this._adapters.length; i++) {
        this._adapters[i].stop();
    }
    this._adapters.splice(0, this._adapters.length);
};

HeroServer.prototype._initializeClient = function ($sConfigFile) {
    this._communicator = Communicator.New();
    this._communicator.initialize($sConfigFile);
};

HeroServer.prototype._initializeServer = function () {
    this.Application = this._configure.get("tars.application.server.app");
    this.ServerName  = this._configure.get("tars.application.server.server");

    var svrList = this._configure.getDomain("tars.application.server");
    for (var i = 0, len = svrList.length; i < len; i++) {
        var config = this._configure.get("tars.application.server.<" + svrList[i] + ">");

        var option = new HeroServer.BindAdapterOption();
        option.name          = svrList[i];
        option.servant       = config.servant;
        option.endpoint      = config.endpoint;
        option.maxconns      = config.maxconns;
        option.protocolName  = config.protocol;
        option.protocol      = TarsProtocol;

        this._addBindAdapter(option);
    }
};

//向服务中注册BindAdapter
HeroServer.prototype._addBindAdapter = function ($options) {
    var adapter = new BindAdapter();
    adapter.name          = $options.name;
    adapter.servantName   = $options.servant;
    adapter.endpoint      = Endpoint.parse($options.endpoint);
    adapter.maxconns      = $options.maxconns?$options.maxconns:1024;
    adapter.protocolName  = $options.protocolName;
    adapter.protocol      = adapter.isTarsProtocol()?TarsProtocol:$options.protocol;

    this._servants[adapter.servantName] = $options.handleImp;
    this._adapters.push(adapter);
};

HeroServer.prototype._waitForShutdown = function () {
    for (var i = 0, len = this._adapters.length; i < len; i++) {
        var adapter = this._adapters[i];
        adapter.handleImp = new this._servants[adapter.servantName]();
        adapter.handleImp.application = this;
        adapter.handleImp.initialize();
        adapter.start();
    }
};

///////////////////////////////////////////////封装简单tars服务//////////////////////////////////////////////////////////
var SimpleServer = function ($HandleImp) {
    this._server    = new HeroServer();
    this._handleImp = $HandleImp;
};

SimpleServer.prototype.start = function ($options) {
    var svroption = new HeroServer.BindAdapterOption();
    svroption.endpoint      = $options.endpoint;
    svroption.servant       = $options.servant;
    svroption.maxconns      = $options.maxconns;
    svroption.protocolName  = $options.protocol;
    svroption.handleImp     = $options.handleImp?$options.handleImp:this._handleImp;

    process.env.TARS_CONFIG && this._server._initializeClient(process.env.TARS_CONFIG);
    this._server.start(svroption);
};

SimpleServer.prototype.stop = function () {
    this._server && this._server.stop();
};

///////////////////////////////////////////////对外提供的静态方法/////////////////////////////////////////////////////////
HeroServer.createServer = function ($HandleImp) {
    return $HandleImp?(new SimpleServer($HandleImp)):(new HeroServer());
};

HeroServer.getServant = function ($sConfigFile, $ServantName) {
    var configure = new Configure();
    configure.parseFile($sConfigFile);

    var svrList  = configure.getDomain("tars.application.server");
    var servants = [];
    for (var i = 0, len = svrList.length; i < len; i++) {
        var config = configure.get("tars.application.server.<" + svrList[i] + ">");
        if ($ServantName && config.servant === $ServantName) {
            servants.push(config);
            break;
        }

        if ($ServantName === undefined) {
            servants.push(config);
        }
    }

    return servants;
};
