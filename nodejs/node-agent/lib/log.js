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

var path = require('path'),
	cluster = require('cluster');

var winston = require('winston'),
	winstonTars = require('@tars/winston-tars');

var servername, dirname;

var isLogToFile = function() {
	return !!(servername && dirname);
};

var initLogger = function(name, type, options) {
	var transport, logger;

	if (!isLogToFile()) {
		logger = winston.loggers.add(name, {transports : [new winston.transports.Console({
			formatter : winstonTars.Formatter.Detail()
		})]});
		logger.emitErrs = true;
		logger.setLevels(winston.config.tars.levels);
		return true;
	}

	transport = winston.transports[type === 'TarsRotate' ? 'TarsBase' : type];

	if (!transport) {
		return false;
	}

	if (name !== '_global') {
		options.filename = path.join(dirname, servername + '_' + name + '.log');
	} else {
		options.filename = path.join(dirname, servername + '.log');
	}

	transport = new transport(options);

	if (type === 'TarsRotate') {
		transport.on('checkfile', function() {
			winston.transports.TarsRotate.Master.start(this.filename, this.interval, options.maxFiles, options.maxSize, '_');
		});
	}

	logger = winston.loggers.add(name, {transports : [transport]});
	logger.emitErrs = true;
	logger.setLevels(winston.config.tars.levels);

	return true;
};

exports.prepare = function(name, dir) {
	dirname = dir;
	servername = name;

	winston.addColors(winston.config.tars.colors);

	cluster.on('worker_message', function(worker, message) {
		var data = message.data;
		if (message.cmd === 'log:rotate') {
			winston.transports.TarsRotate.Master.start(data.filename, data.interval, data.maxFiles, data.maxSize, data.concatStr);
		}
	});
};

exports.init = function(name, type, options) {
	name = name || '_global';

	if (winston.loggers.has(name)) {
		return true;
	}

	return initLogger(name, type, options);
};

exports.append = function(name, data) {
	name = name || '_global';
	
	var logger = winston.loggers.get(name);
	if (logger && typeof logger[data.level] === 'function') {
		logger.log(data.level, data.msg, data.meta);
	}
};

exports.setLevel = function(level, name) {
	var keys;

	if (typeof level !== 'string') {
		return false;
	}
	level = level.toLowerCase();

	if (Object.getOwnPropertyNames(winston.config.tars.levels).indexOf(level) === -1) {
		return false;
	}

	switch(typeof name) {
		case 'object' : {
			keys = ['_global'];
			break;
		}
		case 'string' : {
			keys = [name];
			break;
		}
		default : {
			keys = Object.getOwnPropertyNames(winston.loggers.loggers);
			break;
		}
	}
	
	keys.forEach(function(key) {
		var logger = winston.loggers.loggers[key];
		Object.getOwnPropertyNames(logger.transports).forEach(function(name) {
			logger.transports[name].level = level;
		});
	});

	return true;
};

exports.close = function() {
	winston.transports.TarsRotate.Master.close();
};

exports.isLogToFile = isLogToFile;