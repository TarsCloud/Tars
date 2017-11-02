/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016THL A29 Limited, a Tencent company. All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use this file except 
 * in compliance with the License. You may obtain a copy of the License at
 *
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed 
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the 
 * specific language governing permissions and limitations under the License.
 */

"use strict";

var util = require("util");
var assert = require("assert");
var Promise = require("bluebird");
var EventEmitter = require('events').EventEmitter;

var TarsClient = require("@tars/rpc").Communicator.New();
var ConfigParser = require('@tars/utils').Config;
var tars = require("./ConfigFProxy").tars;

var parseConf = function(content, configFormat) {
    var ret = content;
    if (configFormat === "c") {
        var configParser = new ConfigParser();
        configParser.parseText(content, 'utf8');
        ret = configParser.data;
    } else if (configFormat === "json") {
        ret = JSON.parse(content);
    }

    return ret;
}

function tarsConfigHelper(options) {
    var self = this;
    var proxyName = "tars.tarsconfig.ConfigObj";
    var confFilePath = null;

    if (process.env.TARS_CONFIG) {
        confFilePath = process.env.TARS_CONFIG;
    } else {
        if (options && options.confFile) {
            confFilePath = options.confFile;
        }
    }

    if (confFilePath) {
        TarsClient.initialize(confFilePath);
        var configParser = new ConfigParser();
        configParser.parseFile(confFilePath, 'utf8');

        this.app = configParser.get('tars.application.server.app', "");
        this.server = configParser.get('tars.application.server.server', "");
        this.localip = configParser.get('tars.application.server.localip', "");

        var enableset = configParser.get('tars.application.enableset', "");
        if (enableset.toLowerCase() === "y") {
            this.setdivision = configParser.get('tars.application.setdivision', "");
        }

        proxyName = configParser.get('tars.application.server.config', "");
    }

    if (!process.env.TARS_CONFIG) {
        if (options && options.tarsConfigObjProxyName) {
            proxyName = options.tarsConfigObjProxyName;
        } else {
            assert.fail('must be specify tars config obj name');
        }
    }

    if (options) {
        if (options.app) {
            this.app = options.app;
        }

        if (options.server) {
            this.server = options.server;
        }

        if (options.host) {
            this.localip = options.host;
        }
    }

    assert(this.app && this.server, "can not get app or server name");

    this.options = options;
    this.prx = TarsClient.stringToProxy(tars.ConfigProxy, proxyName);
    this.setTimeout();

    this.setMaxListeners(0);

    process.on("message", function(obj) {
        if (obj.cmd === "tars.loadconfig") {
            self.loadConfig(obj.data).then(function(configContent) {
                self.emit("configPushed", obj.data, configContent);
            }, function () {});
        }
    });
}
util.inherits(tarsConfigHelper, EventEmitter);

tarsConfigHelper.prototype.getConfigInfoObj = function (configOptions, isForList, isListAllFun) {
    var configInfo;

    if (isListAllFun && isForList) {
        if (configOptions instanceof tars.GetConfigListInfo) {
            return configOptions;
        }
        configInfo = new tars.GetConfigListInfo();
    } else {
        if (configOptions instanceof tars.ConfigInfo) {
            return configOptions;
        }
        configInfo = new tars.ConfigInfo();
    }

    configInfo.appname = this.app;

    var servername = this.server;
    var bAppOnly = false;
    var host = this.localip;
    var filename = "";

    if (configOptions) {
        if (typeof configOptions === "object") {
            filename = configOptions.filename || "";
            if (configOptions.bAppOnly) {
                bAppOnly = true;
            } else {
                if (configOptions.servername) {
                    servername = configOptions.servername;
                }
            }
            host = configOptions.host || "";
        } else {
            filename = configOptions;
        }
    }

    if (servername === "") {
        bAppOnly = true;
    }

    if (bAppOnly) {
        servername = "";
    }

    configInfo.servername = servername;
    configInfo.host = host || '';
    configInfo.setdivision = this.setdivision || '';
    configInfo.bAppOnly = bAppOnly;

    if (!isForList) {
        configInfo.filename = filename;
    }

    return configInfo;
}

/*
 @description:加载服务默认的配置项,并且转换为json
 @param：configFormat {string} "c":c++格式的配置项，默认值。"json"：配置项是json格式的
 */
tarsConfigHelper.prototype.loadServerObject = function (configFormat) {
    var _configFormat = configFormat || "c";
    return this.loadConfig(this.server + ".conf", _configFormat);
}

/*
 @description:获取节点的配置内容
 */
tarsConfigHelper.prototype.loadConfig = function (configOptions, configFormat) {
    var configInfo = this.getConfigInfoObj(configOptions, false);

    return this.prx.loadConfigByInfo(configInfo).then(function(retData) {
        return parseConf(retData.response.arguments.config, configFormat);
    });
}

/*
 @description:获取节点的配置列表
 */
tarsConfigHelper.prototype.getConfigList = function (configOptions) {
    var configInfo = this.getConfigInfoObj(configOptions, true, true);

    configInfo.host = "";

    return this.prx.ListAllConfigByInfo(configInfo).then(function(retData) {
        return retData.response.arguments.vf.value;
    });
}

/*
 @description:获取节点的所有配置项，promise返回的object，key是文件名，value是配置内容
 */
tarsConfigHelper.prototype.getAllConfigData = function (configOptions, configFormat) {
    var configInfoContent = this.getConfigInfoObj(configOptions,false);

    var self = this;

    return this.getConfigList(configOptions).then(function(configList) {
        var pActions = [];

        configList.forEach(function (cItem) {
            var fConfig = configInfoContent;
            fConfig.filename = cItem;
            pActions.push(self.loadConfig(fConfig).then(function(configContent) {
                return {filename: cItem, content: configContent};
            }));
        });

        return Promise.all(pActions).then(function(allData) {
            var retObj = {};

            allData.forEach(function(cItem) {
                retObj[cItem.filename] = parseConf(cItem.content, configFormat);
            });

            return retObj;
        });
    });
}

tarsConfigHelper.prototype.setTimeout = function (iTimeout) {
    var timeout = iTimeout || 30 * 1000;
    this.prx.setTimeout(timeout);
}

exports = module.exports = tarsConfigHelper;