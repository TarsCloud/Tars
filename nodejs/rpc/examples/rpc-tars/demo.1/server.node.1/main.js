var Tars   = require("../../../../protal.js");
var TarsDemo  = require("./NodeTarsImp.js").tars;

var svr = new Tars.server();
svr.initialize(process.env.TARS_CONFIG || "./TARS.NodeTarsServer.config.conf", function (server){
    server.addServant(TarsDemo.NodeTarsImp, server.Application + "." + server.ServerName + ".NodeTarsObj");
    console.log("tars server started");
});

svr.start();