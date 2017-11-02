/**
 * tars-rpc的框架导出类
 */
var utils=require("@tars/utils");

module.exports.promise      = utils.Promise;
module.exports.Communicator = require("./core/client/Communicator.js").Communicator;
module.exports.ServantProxy = require("./core/client/ServantProxy.js").ServantProxy;
module.exports.server       = require("./core/server/HeroServer.js").HeroServer;
module.exports.client       = require("./core/client/Communicator.js").Communicator.New();
module.exports.error        = require("./core/util/TarsError.js").TarsError;
module.exports.rpcTars      =require("./core/rpc-tars");
module.exports.rpcJson      =require("./core/rpc-json");
