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
	http = require('http');

var winston = require('winston'),
	winstonTars = require('@tars/winston-tars');

var constants = require('./constants');

var callsite = require('callsite');

var httpStat = require('./tars/HttpStats');
var usageStat = require('./tars/UsageStats');

var agent_args = JSON.parse(process.env.agent_args);
delete process.env.agent_args;

var exec_script = agent_args.exec_script;

var logger, currLogLevel;

var lineno = function() {
	var stack = callsite()[2];
	return path.basename(stack.getFileName()) + ':' + stack.getLineNumber();
};

var errorToString = function(err) {
	if (typeof err === 'undefined') {
		return 'undefined';
	}

	if (typeof err !== 'object') {
		return err.toString();
	}

	if (!err) {
		return 'null';
	}

	return err.stack ? err.stack : err.toString();
};

// set constants
if (parseInt(agent_args.process_keepalive) >= 0) {
	constants.WORKER_DETECT_INTERVAL = parseInt(agent_args.process_keepalive);
}

// [en|dis]able tars monitor
if (!process.env.TARS_CONFIG) {
	constants.TARS_MONITOR = false;
} else if (process.env.TARS_MONITOR) {
	constants.TARS_MONITOR = (process.env.TARS_MONITOR === 'true');
}
process.env.TARS_MONITOR = constants.TARS_MONITOR;

// set process title
if (exec_script) {
	process.title = util.format('%s: worker process', exec_script);
}

// fixed process script file path
process.argv[1] = exec_script;

// if we've been told to run as a different user or group (e.g. because they have fewer
// privileges), switch to that user before importing any third party application code.
if (agent_args.process_group) {
  process.setgid(agent_args.process_group);
}

if (agent_args.process_user) {
  process.setuid(agent_args.process_user);
}

// Handle Ctrl+C signal
process.on('SIGINT', function() {});

// if script not listen on disconnect event, program will be exit
process.on('disconnect', function disconnect() {
	if (constants.TARS_MONITOR) {
		httpStat.unbind();
		usageStat.stop();
	}

	if (!process.listeners('disconnect').filter(function(listener) {
		return listener !== disconnect;
	}).length) {
		process.removeListener('disconnect', disconnect);
		process.exit();
	}
});

// Notify master that an uncaughtException has been catched
process.on('uncaughtException', function uncaughtListener(err) {
	if (!process.listeners('uncaughtException').filter(function (listener) {
		return listener !== uncaughtListener;
	}).length) {
		process.removeListener('uncaughtListener', uncaughtListener);
		try {
			process.send({
				cmd : 'god:err',
				data : errorToString(err)
			});
		} catch(e) {}
		setTimeout(function() {
			process.exit(constants.CODE_UNCAUGHTEXCEPTION);
		}, 100);
	}
});

// Main log settings
if (!isNaN(parseInt(agent_args.log_maxsize))) {
	constants.APPLOG_MAX_SIZE = parseInt(agent_args.log_maxsize);
}
if (!isNaN(parseInt(agent_args.log_maxfiles))) {
	constants.APPLOG_MAX_FILES = parseInt(agent_args.log_maxfiles);
}
if (agent_args.log_level) {
	constants.APPLOG_LEVEL = agent_args.log_level;
}

// init logger
if (agent_args.log_main) {
	logger = new (winston.Logger)({
		transports: [new (winston.transports.TarsRotate)({
			filename : agent_args.log_main,
			maxSize : constants.APPLOG_MAX_SIZE,
			maxFiles : constants.APPLOG_MAX_FILES
		})]
	});
} else {
	logger = new (winston.Logger)({
		transports : [new (winston.transports.Console)({
			formatter : winstonTars.Formatter.Detail()
		})]
	});
}

logger.setLevels(winston.config.tars.levels);
logger.emitErrs = true;

// Redirect console to master
console.log = function() {
	if (currLogLevel >= winston.config.tars.levels.debug) {
		var argsLen = arguments.length;
		var args = new Array(argsLen);
		for (var i = 0; i < argsLen; i += 1) {
			args[i] = arguments[i];
		}

		logger.log('debug', util.format.apply(util, args), {
			lineno : lineno()
		});
	}
};
console.info = function() {
	if (currLogLevel >= winston.config.tars.levels.info) {
		var argsLen = arguments.length;
		var args = new Array(argsLen);
		for (var i = 0; i < argsLen; i += 1) {
			args[i] = arguments[i];
		}

		logger.log('info', util.format.apply(util, args), {
			lineno : lineno()
		});
	}
};
console.warn = function() {
	if (currLogLevel >= winston.config.tars.levels.warn) {
		var argsLen = arguments.length;
		var args = new Array(argsLen);
		for (var i = 0; i < argsLen; i += 1) {
			args[i] = arguments[i];
		}

		logger.log('warn', util.format.apply(util, args), {
			lineno : lineno()
		});
	}
};
console.error = function() {
	if (currLogLevel >= winston.config.tars.levels.error) {
		var argsLen = arguments.length;
		var args = new Array(argsLen);
		for (var i = 0; i < argsLen; i += 1) {
			args[i] = arguments[i];
		}

		logger.log('error', util.format.apply(util, args), {
			lineno : lineno()
		});
	}
};

var setLevel = function(level) {
	if (typeof level !== 'string') {
		return;
	}
	level = level.toLowerCase();
	if (Object.getOwnPropertyNames(winston.config.tars.levels).indexOf(level) === -1) {
		return;
	}
	Object.getOwnPropertyNames(logger.transports).forEach(function(name) {
		logger.transports[name].level = level;
	});
	currLogLevel = winston.config.tars.levels[level];
};

setLevel(constants.APPLOG_LEVEL);

// process log level change
process.on('message', function(message) {
	if (message) {
		switch(message.cmd) {
			case 'tars.setloglevel' : {
				setLevel(message.data);
				break;
			}
			case 'agent.shutdown' : {
				process.disconnect();
				break;
			}
		}
	}
});

// send heartbeat to master
if (constants.WORKER_DETECT_INTERVAL > 0) {
	setInterval(function() {
		try {
			process.send({
				cmd : 'god:alive'
			}, undefined, function() {});
		} catch(e) {}
	}, Math.ceil(constants.WORKER_DETECT_INTERVAL * 1000 / constants.WORKER_HEART_BEAT_TIMES)).unref();
}

// monitor http & https svr
if (constants.TARS_MONITOR) {
	httpStat.bind({
		threshold : parseInt(agent_args.http_threshold),
		sep : (agent_args.http_seppath === 'false') ? false : true
	});
	usageStat.start();
}

// Change dir to fix process.cwd
process.chdir(path.dirname(exec_script));

// Get the script & exec as main
require('module')._load(exec_script, null, true);