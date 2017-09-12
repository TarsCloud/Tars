var TarsServer  = require("../../../../protal.js").server;
var TarsDemo = require("./NodeSecondImp.js").tars;

var svr = new TarsServer();
svr.initialize(process.env.TARS_CONFIG || "./TARS.NodeTarsServer.config.conf", function (server){
    server.addServant(TarsDemo.NodeSecondImp, server.Application + "." + server.ServerName + ".NodeSecondObjAdapetr");
    console.log("tars server started");
});
svr.start();
