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

'use strict';

var http = require('http'),
	https = require('https'),
	assert = require('assert');

var onFinished = require('on-finished');

var tarsMonitor = require('@tars/monitor').stat, 
	tarsConfigure = require('@tars/utils').Config;

var moduleName;
var slaveSetName, slaveSetArea, slaveSetID;
var createServer = {};

var pathname = function(url) {
	var hash, qs, path;

	if (!url || typeof url !== 'string') {
		return '/';
	}

	if (url.indexOf('http://') === 0) {
		path = url.indexOf('/', 7);
	} else if (url.indexOf('https://') === 0) {
		path = url.indexOf('/', 8);
	}

	if (path !== undefined) {
		if (path !== -1) {
			url = url.slice(path);
		} else {
			return '/';
		}
	}

	hash = url.indexOf('#');

	if (hash !== -1) {
		url = url.slice(0, hash);
	}

	qs = url.indexOf('?');

	if (qs !== -1) {
		url = url.slice(0, qs);
	}

	return url || '/';
};

var initConfig = function(obj) {
	var tarsConfig, setdivision;

	obj = obj || process.env.TARS_CONFIG;

	assert(obj, 'TARS_CONFIG is not in env and init argument is neither an Object nor a String.');

	if (typeof obj === 'string') {
		tarsConfig = new tarsConfigure;
		tarsConfig.parseFile(obj);
	} else {
		tarsConfig = obj;
	}

	moduleName = tarsConfig.get('tars.application.client.modulename') || 'NO_MODULE_NAME';
	setdivision = tarsConfig.get('tars.application.setdivision');

	if (tarsConfig.get('tars.application.enableset', '').toLowerCase() === 'y' && setdivision && typeof setdivision === 'string') {
		setdivision = setdivision.split('.');
		if (setdivision.length >= 3) {
			slaveSetName = setdivision[0];
			slaveSetArea = setdivision[1];
			slaveSetID = setdivision.slice(2).join('.');

			moduleName +=  '.' + slaveSetName + slaveSetArea + slaveSetID;
		}
	}
};

var shimming = function(type, options) {
	switch (type) {
		case 'http' : {
			createServer[type] = createServer[type] || http.createServer;
			break;
		}
		case 'https' : {
			createServer[type] = createServer[type] || https.createServer;
			break;
		}
	}

	return function(/*...args*/) {
		var reqHandler = function(req, res) {
			var startTime = process.uptime(),
				localInfo = req.socket.address(),
				remoteAddress = req.socket.remoteAddress || '',
				interfaceName = '';

			if (options.sep) {
				interfaceName = pathname(req.url);
			}

			/*
			 *	don't leak arguments to the other function
			 */
			var argsLen = arguments.length,
				args = new Array(argsLen);

			for (var i = 0; i < argsLen; i += 1) {
				args[i] = arguments[i];
			}

			onFinished(res, function(err) {
				var headers = {
						masterName : type + '_client',
						slaveName : moduleName,
						interfaceName : interfaceName,
						masterIp : remoteAddress,
						slaveIp : localInfo.address || '',
						slavePort : localInfo.port || 0,
						bFromClient : false
					};

				if (slaveSetName && slaveSetArea && slaveSetID) {
					headers.slaveSetName = slaveSetName;
					headers.slaveSetArea = slaveSetArea;
					headers.slaveSetID = slaveSetID;
				}

				if (err || (options.threshold !== 0 && res.statusCode >= options.threshold)) {
					tarsMonitor.report(headers, tarsMonitor.TYPE.ERROR);
				} else {
					tarsMonitor.report(headers, tarsMonitor.TYPE.SUCCESS, (process.uptime() - startTime) * 1000);
				}
			});

			requestListener && requestListener.apply(this, args);
		}, requestListener;

		switch (type) {
			case 'http' : {
				requestListener = arguments[0];
				return createServer[type].call(http, reqHandler);
			}
			case 'https' : {
				requestListener = arguments[1];
				return createServer[type].call(https, arguments[0], reqHandler);
			}
		}
	};
};

var restore = function(type) {
	if (!createServer[type]) {
		return;
	}

	switch (type) {
		case 'http' : {
			http.createServer = createServer[type];
		}
		case 'https' : {
			https.createServer = createServer[type];
		}
	}
	
	delete createServer[type];
};

exports.init = function(obj) {
	initConfig(obj);
	tarsMonitor.init(obj);
};
exports.bind = function(options) {
	var opt = {
		threshold : 400,
		sep : true
	};

	if (options) {
		if (options.threshold >= 0) {
			opt.threshold = options.threshold;
		}
		if (typeof options.sep === 'boolean') {
			opt.sep = options.sep;
		}
	}

	if (!moduleName) {
		exports.init();
	}

	http.createServer = shimming('http', opt);
	https.createServer = shimming('https', opt);
};
exports.unbind = function() {
	if (moduleName) {
		restore('http');
		restore('https');

		tarsMonitor.stop();
	}
};