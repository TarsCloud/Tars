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
	util = require('util'),
	assert = require('assert'),
	cluster = require('cluster');

var TarsConfigure = require('@tars/utils').Config,
	TarsDyeing = require('@tars/dyeing');

var winston = require('winston'),
	winstonTars = require('@tars/winston-tars');

var callsite = require('callsite');

var cycle = require('cycle');

var tarsConfig, initialized = false, listened = false, instances = {};

var lineno = function() {
	var stack = callsite()[3];
	return path.basename(stack.getFileName()) + ':' + stack.getLineNumber();
};

var getDyeingObj = function() {
	var val = arguments[0], key = arguments[1];

	if (!val) {
		return TarsDyeing.gen(false);
	}

	if (TarsDyeing.is(val)) {
		return val;
	}

	switch(typeof val) {
		case 'boolean' :  {
			return TarsDyeing.gen(true);
		}
		case 'string' : {
			if (key && typeof key === 'string') {
				return TarsDyeing.gen(true, key, val);
			} else {
				return TarsDyeing.gen(true, null, val);
			}
		}
		case 'object' : {
			if (typeof val.getDyeingObj === 'function') {
				return val.getDyeingObj();
			}
			break;
		}
	}

	return TarsDyeing.gen(false);
};

var setConfig = function(obj) {
	initialized = true;

	obj = obj || process.env.TARS_CONFIG;

	if (!obj) {
		return;
	}

	if (obj) {
		if (typeof obj === 'string') {
			tarsConfig = new TarsConfigure;
			tarsConfig.parseFile(obj);
		} else if (obj instanceof TarsConfigure) {
			tarsConfig = obj;
		}
	}
};

var listenMessage = function() {
	listened = true;

	if (cluster.isWorker) {
		process.on('message', listenMessage.listener);
	}
};
listenMessage.listener = function(message) {
	if (message.cmd === 'tars.setloglevel' && message.data) {
		Object.getOwnPropertyNames(instances).forEach(function(key) {
			var instance = instances[key].obj;
			if (instance.type === 'TarsRotate') {
				instance.setLevel(message.data);
			}
		});
	}
};

var setLogger = function(logger) {
	logger.setLevels(winston.config.tars.levels);
	logger.emitErrs = true;
};

var buildConsole = function(type, options) {
	var formatter;

	if (type === 'TarsDate') {
		formatter = options.formatter || winstonTars.Formatter.Simple({
			separ : options.separ
		});
	} else {
		formatter = options.formatter || winstonTars.Formatter.Detail({
			separ : options.separ
		});
	}

	return new (winston.Logger)({transports : [new winston.transports.Console({
		formatter : formatter
	})]});
};

var tarsLogs = function(type, name, options) {
	var ths = this;

	if (!(this instanceof tarsLogs)) {
		return new tarsLogs(type, name, options);
	}

	assert(winston.transports[type], 'argument type not found in winston.transports');

	assert(!name || typeof name === 'string', 'argument name MUST BE an string');

	if (instances[type + name]) {
		instances[type + name].ref += 1;
		return instances[type + name].obj;
	}

	if (!initialized) {
		setConfig();
	}

	if (!listened) {
		listenMessage();
	}

	options = options || {};

	if (!tarsConfig) {
		this._commonLogger = buildConsole(type, options);
		setLogger(this._commonLogger);
		this.setLevel('INFO');
		return;
	} else if (!options.tarsConfig) {
		options.tarsConfig = tarsConfig;
	}

	if (typeof options.hasSufix !== 'boolean') {
		options.hasSufix = true;
	}
	if (typeof options.hasAppNamePrefix !== 'boolean') {
		options.hasAppNamePrefix = true;
	}
	options.concatStr = options.concatStr || '_';

	assert(name || options.hasAppNamePrefix, 'name and options.hasAppNamePrefix can\'t be empty at the same time');

	if (options.hasAppNamePrefix) {
		options.filename = util.format('%s.%s', tarsConfig.get('tars.application.server.app'), 
			tarsConfig.get('tars.application.server.server'));
	} else {
		options.filename = '';
	}

	if (name) {
		options.filename += (options.hasAppNamePrefix ? options.concatStr : '') + name;
	}

	if (options.hasSufix) {
		options.filename += '.log';
	}

	options.filename = path.join(
		tarsConfig.get('tars.application.server.logpath'), 
		tarsConfig.get('tars.application.server.app'),
		tarsConfig.get('tars.application.server.server'),
		options.filename
	);

	this.type = type;
	this.name = name;
	this.options = options;

	if (type === 'TarsDate') {
		this._initTarsDate();
	} else {
		this._initCommon();
	}

	this._initDyeing();
	
	if (type === 'TarsRotate') {
		this.setLevel(tarsConfig.get('tars.application.server.logLevel', 'DEBUG'));
	} else {
		this.setLevel('INFO');
	}

	instances[type + name] = {
		ref : 1,
		obj : this
	};
};

tarsLogs.prototype._initDyeing = function() {
	this.options.filename = path.join(
		tarsConfig.get('tars.application.server.logpath'),
		'tars_dyeing',
		'dyeing' + (this.options.hasSufix ? '.log' : '')
	);
	this.options.prefix = util.format('%s.%s', tarsConfig.get('tars.application.server.app'), 
			tarsConfig.get('tars.application.server.server'));

	this._dyeingLogger = new (winston.Logger)({
		transports : [
			new (winston.transports[this.type])(this.options)
		]
	});

	setLogger(this._dyeingLogger);
};

tarsLogs.prototype._initCommon = function() {
	this.options.formatter = this.options.formatter || winstonTars.Formatter.Detail({
		separ : this.options.separ
	});

	this._commonLogger = new (winston.Logger)({
		transports : [
			new (winston.transports[this.type])(this.options)
		]
	});

	setLogger(this._commonLogger);
};

tarsLogs.prototype._initTarsDate = function() {
	var transports = [];

	this.options.formatter = this.options.formatter || winstonTars.Formatter.Simple({
		separ : this.options.separ
	});

	transports.push(new (winston.transports.TarsDate)(this.options));
	
	this.options.filename = this.name;

	this._commonLogger = new (winston.Logger)({transports : transports});

	setLogger(this._commonLogger);
};

tarsLogs.prototype._log = function(level, args) {
	var dyeingObj, meta, addedLineno = false;

	if (args.length === 0) {
		return false;
	}

	dyeingObj = args[args.length - 1];

	if (TarsDyeing.is(dyeingObj)) {
		args = Array.prototype.slice.call(args, 0, -1);

		if (args.length === 0) {
			return false;
		}

		if (!tarsConfig || !dyeingObj.dyeing) {
			dyeingObj = undefined;
		}
	} else {
		args = Array.prototype.slice.call(args);
		dyeingObj = undefined;
	}

	meta = args[args.length - 1];
	if (meta && typeof meta === 'object') {
		if (meta.lineno !== false && this.type === 'TarsRotate') {
			if (typeof meta.lineno !== 'string' || meta.lineno === '') {
				meta.lineno = lineno();
				addedLineno = true;
			}
		}

		if (!(meta instanceof Error) && !util.isDate(meta) && !Array.isArray(meta) && !util.isRegExp(meta) && !Buffer.isBuffer(meta)) {
			args[args.length - 1] = cycle.decycle(meta);
		}
	} else if (this.type === 'TarsRotate') {
		args.push({
			lineno : lineno()
		});
	}

	if (dyeingObj) {
		this._dyeingLogger[level].apply(this._dyeingLogger, args);
	}

	this._commonLogger[level].apply(this._commonLogger, args);

	if (addedLineno) {
		delete meta.lineno;
	}

	return true;
};

tarsLogs.prototype.debug = function() {
	var argsLen = arguments.length;
	var args = new Array(argsLen);
	for (var i = 0; i < argsLen; i += 1) {
		args[i] = arguments[i];
	}

	return this._log('debug', args);
};

tarsLogs.prototype.info = function() {
	var argsLen = arguments.length;
	var args = new Array(argsLen);
	for (var i = 0; i < argsLen; i += 1) {
		args[i] = arguments[i];
	}

	return this._log('info', args);
};

tarsLogs.prototype.warn = function() {
	var argsLen = arguments.length;
	var args = new Array(argsLen);
	for (var i = 0; i < argsLen; i += 1) {
		args[i] = arguments[i];
	}

	return this._log('warn', args);
};

tarsLogs.prototype.error = function() {
	var argsLen = arguments.length;
	var args = new Array(argsLen);
	for (var i = 0; i < argsLen; i += 1) {
		args[i] = arguments[i];
	}

	return this._log('error', args);
};

tarsLogs.prototype.setLevel = function(level) {
	var ths = this;

	if (typeof level !== 'string') {
		return false;
	}

	level = level.toLowerCase();

	if (Object.getOwnPropertyNames(winston.config.tars.levels).indexOf(level) === -1) {
		return false;
	}

	Object.getOwnPropertyNames(this._commonLogger.transports).forEach(function(name) {
		ths._commonLogger.transports[name].level = level;
	});
};

tarsLogs.prototype.close = function() {
	this._commonLogger.close();

	if (this._dyeingLogger) {
		this._dyeingLogger.close();
	}

	if (instances[this.type + this.name]) {
		if (instances[this.type + this.name].ref > 1) {
			instances[this.type + this.name].ref -= 1;
		} else {
			delete instances[this.type + this.name];
		}
	}

	if (Object.getOwnPropertyNames(instances).length === 0) {
		if (cluster.isWorker) {
			process.removeListener('message', listenMessage.listener);
		}
		listened = false;
	}
};

tarsLogs.prototype.getDyeingObj = getDyeingObj;

tarsLogs.DateFormat = winstonTars.DateFormat;
tarsLogs.Formatter = winstonTars.Formatter;

tarsLogs.setConfig = setConfig;

tarsLogs.getDyeingObj = getDyeingObj;

module.exports = exports = tarsLogs;