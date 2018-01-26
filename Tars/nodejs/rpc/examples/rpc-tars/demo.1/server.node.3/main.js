var TarsServer = require("../../../../protal.js").server;
var TARS  = require("./NodeTarsImp.js").tars;

TarsServer.getServant("./TARS.NodeTarsServer.config.conf").forEach(function (config){
    var svr, map;
    map = {
        'TarsDemo.NodeTarsServer.NodeTarsObj' : TARS.NodeTarsImp
    };

    svr = TarsServer.createServer(map[config.servant]);
    svr.start(config);
});
