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

var path = require('path');
var util = require('util');
var cluster = require('cluster');
var os = require('os');
var net = require('net');

var God = require('./God');
var constants = require('./constants');
var convert = require('./util/convert');
var cpu = require('./util/cpu');
var deps = require('./util/lsdeps');
var Log = require('./log');
var pkg = require('../package.json');

var tarsReport = require('./tars/Report');
var tarsMessage = require('./tars/Message');
var tarsNotify = require('./tars/Notify');

var bindEvents = function() {
	var exception = false;

	cluster.on('fork', function(worker) {
		console.info('worker(%s), forked.', worker.process.pid);
	}).on('online', function(worker) {
		console.info('worker(%s), online.', worker.process.pid);
	}).on('listening', function(worker, address) {
		console.info('worker(%s), listening on %s:%s', worker.process.pid, address.address || '', address.port);
	}).on('fork', function(worker) {
		var procStd = function(pid, level) {
			return function(buf) {
				buf.toString().split('\n').forEach(function(line) {
					if (line.length > 0) {
						if (line[line.length - 1] === '\r') {
							line = line.slice(0, -1);
						}

						Log.append(null, {
							level : level,
							msg : line,
							meta : {
								pid : pid
							}
						});
					}
				});
			};
		};

		if (Log.isLogToFile()) {
			worker.process.stdout.on('data', procStd(worker.process.pid, 'info'));
			worker.process.stderr.on('data', procStd(worker.process.pid, 'error'));
		} else {
			worker.process.stdout.pipe(process.stdout);
			worker.process.stderr.pipe(process.stderr);
		}
	});

	God.events.on('message', function(code, worker, args) {
		switch(code) {
			case constants.GOD_MESSAGE.EXCEPTION_REACHED_COND : {
				console.error('exception occurred more than %s times within %s seconds, exiting ...', constants.EXCEPTION_TOTAL, constants.EXCEPTION_TIME / 1000);
				tarsNotify.report.error(util.format('exiting,exception occurred more than %s times within %s seconds', constants.EXCEPTION_TOTAL, constants.EXCEPTION_TIME / 1000), '');
				exception = true;
				break;
			}
			case constants.GOD_MESSAGE.KILLING_ALL_WORKERS : {
				console.info('killing all worker process ...');
				tarsReport.destroy();
				tarsMessage.destroy();
				Log.close();
				break;
			}
			case constants.GOD_MESSAGE.KILLING_WORKER : {
				console.info('killing worker(%s) ...', worker.process.pid);
				break;
			}
			case constants.GOD_MESSAGE.FORCE_KILL_WORKER : {
				console.error('exceeded the graceful timeout, force kill worker(%s) ...', worker.process.pid);
				tarsNotify.report.error('exceeded the graceful timeout, force kill worker', worker.process.pid);
				break;
			}
			case constants.GOD_MESSAGE.ALL_WORKERS_STOPPED : {
				console.info('all workers killed, really exiting now ...');
				setTimeout(function() {
					process.exit(exception ? constants.CODE_UNCAUGHTEXCEPTION : 0);
				}, 100).unref();
				break;
			}
			case constants.GOD_MESSAGE.STOP_ZOMBIE_WORKER : {
				console.error('detected zombie worker(%s).', worker.process.pid);
				tarsNotify.report.error('detected zombie worker.', worker.process.pid);
				break;
			}
			case constants.GOD_MESSAGE.KILL_ERROR : {
				console.error('kill worker(%s) failed, %s.', worker.process.pid, args || 'no error');
				tarsNotify.report.error('kill worker failed', worker.process.pid);
				break;
			}
		}
	}).on('exit', function(worker, error, code, signal) {
		if (error) {
			console.error('worker(%s), exit unexpected.', worker.process.pid);
			tarsNotify.report.error('worker exit unexpected', worker.process.pid);

			if (typeof error === 'string') {
				Log.append(null, {
					level : 'error',
					msg : error,
					meta : {
						pid : worker.process.pid
					}
				});
			}
		} else {
			console.info('worker(%s), exit normally%s.', worker.process.pid, convert.friendlyExit(code, signal, ' with'));
		}
	});

	process.once('SIGINT', function() {
		console.info('received kill or Ctrl-C signal.');
		tarsNotify.report.info('stop');
		God.killAll();
	}).on('exit', function(code) {
		console.info('exit%s.', convert.friendlyExit(code, null, ' with'));
	});

	tarsMessage.on('notify', function(command, data, callback) {
		var mesgObj = {
			cmd : command
		};

		if (data) {
			mesgObj.data = data;
		}

		// send to worker
		God.send(mesgObj);

		// send to master(itself)
		mesgObj.setRet = callback;
		process.emit('message', mesgObj); 
	}).on('shutdown', function() {
		console.info('received TARS shutdown signal.');
		tarsNotify.report.info('stop');
		God.killAll();
	});

	process.on('message', function(message) {
		if (message) {
			switch (message.cmd) {
				case 'tars.setloglevel' : {
					Log.setLevel(message.data, null);
					break;
				}
				case 'preheatCheck' : {
					if (God.getStatus().every(function (status) {
						return status === constants.WORKER_STATUS.ONLINE;
					})) {
						message.setRet('success');
					} else {
						message.setRet('not ready');
					}
					break;
				}
			}
		}
	});
};

var initLog = function(name, dir) {
	Log.prepare(name, dir);

	Log.init(null, 'TarsRotate', {
		maxFiles : constants.APPLOG_MAX_FILES,
		maxSize : constants.APPLOG_MAX_SIZE
	});

	Log.init('agent', 'TarsRotate', {
		maxFiles : constants.APPLOG_MAX_FILES,
		maxSize : constants.APPLOG_MAX_SIZE
	});

	Log.setLevel(constants.APPLOG_LEVEL, null);
};

var outRedirect = function() {
	var register = function(level) {
		return function() {
			Log.append('agent', {
				level : level,
				msg : util.format.apply(util, arguments),
				meta : {
					pid : process.pid
				}
			});
		};
	};

	console.info = register('info');
	console.warn = register('warn');
	console.error = register('error');
};

var getWorkerArgs = function(script, opts) {
	var args = {}, obj;

	args['script'] = script;

	if (opts.scriptArgs) {
		args['script_args'] = opts.scriptArgs;
	}
	
	if (opts.nodeArgs) {
		args['node_args'] = opts.nodeArgs;
	}

	//worker process title
	args['name'] = typeof opts.name === 'string' ? opts.name : path.basename(script, path.extname(script));

	//pass custom env to worker
	if (opts.env) {
		args['env'] = opts.env;
	}

	if (opts.httpAddress) {
		obj = convert.extractAddress(opts.httpAddress);
		if (obj) {
			args['http_ip'] = obj.ip;
			args['http_port'] = obj.port;
		}
	}

	//specify worker uid and gid, if not set it's equal to Master 
	if (opts.runAsUser) {
		args['run_as_user'] = opts.runAsUser;
	}
	if (opts.runAsGroup) {
		args['run_as_group'] = opts.runAsGroup;
	}

	if (!isNaN(opts.maxMemoryRestart)) {
		if (!Array.isArray(args['node_args'])) {
			args['node_args'] = [];
		}
		args['node_args'].push('--max-old-space-size=' + opts.maxMemoryRestart);
	}

	if (opts.config) {
		args['config'] = opts.config;
	}

	if (!isNaN(opts.keepaliveTime)) {
		args['keepaliveTime'] = opts.keepaliveTime;
	}

	if (typeof opts.tarsMonitor === 'boolean') {
		args['tarsMonitor'] = opts.tarsMonitor;
	}

	if (opts.log) {
		args['log'] = path.join(opts.log, args['name'].replace('.', '/') + '/');
	}

	return args;
};

var setConstants = function(opts) {
	if (!isNaN(opts.gracefulShutdown)) {
		constants.GRACEFUL_TIMEOUT = opts.gracefulShutdown;
	}
	if (opts.config) {
		if (constants.GRACEFUL_TIMEOUT > 1000) {
			constants.GRACEFUL_TIMEOUT -= 1000;
		} else {
			constants.GRACEFUL_TIMEOUT = 0;
		}
	}

	if (!isNaN(opts.exceptionMax)) {
		constants.EXCEPTION_TOTAL = opts.exceptionMax;
	}
	
	if (!isNaN(opts.exceptionTime)) {
		constants.EXCEPTION_TIME = opts.exceptionTime;
	}

	if (!isNaN(opts.keepaliveTime)) {
		constants.WORKER_DETECT_INTERVAL = opts.keepaliveTime;
	}

	if (!isNaN(opts.applogMaxFiles)) {
		constants.APPLOG_MAX_FILES = opts.applogMaxFiles;
	}

	if (!isNaN(opts.applogMaxSize)) {
		constants.APPLOG_MAX_SIZE = opts.applogMaxSize;
	}

	if (opts.applogLevel) {
		constants.APPLOG_LEVEL = opts.applogLevel;
	}

	if (typeof opts.tarsMonitor === 'boolean') {
		constants.TARS_MONITOR = opts.tarsMonitor;
	}

	if (!isNaN(opts.tarsMonitorHttpThreshold)) {
		constants.TARS_MONITOR_HTTP_THRESHOLD = opts.tarsMonitorHttpThreshold;
	}

	if (typeof opts.tarsMonitorHttpSeppath === 'boolean') {
		constants.TARS_MONITOR_HTTP_SEPPATH = opts.tarsMonitorHttpSeppath;
	}
};

var initTarsComponent = function(args, opts) {
	if (opts.tarsNode) {
		console.info('tars node:', opts.tarsNode);
		tarsReport.init(args['name'], opts.tarsNode, opts.config);
		tarsReport.reportVersion(pkg.version || process.version);
		tarsReport.keepAlive();
	}

	if (opts.tarsLocal) {
		console.info('local interface:', opts.tarsLocal);
		tarsMessage.startServer(args['name'], opts.tarsLocal);
	}

	if (opts.config) {
		tarsNotify.init(opts.config);
	}
};

var startWorker = function(opts) {
	var instances;

	if (!isNaN(opts.instances) && opts.instances > 0) {
		instances = opts.instances;
	} else {
		if (opts.instances === -1) { // instances = max
			instances = cpu.totalCores;
		} else { // instances = auto
			if (cpu.physicalCores > 0 && cpu.totalCores > cpu.physicalCores) { //physicalCores correct
				instances = cpu.physicalCores;
			} else {
				instances = cpu.totalCores;
			}
		}
	}

	instances = instances || 1;

	console.info('forking %s workers ...', instances);

	God.startWorker(instances);
};

var deviceInfo = function() {
	if (cpu.physicalCores !== 0) {
		return util.format('%s arch, %d cpus, %d physical cores, %s platform, %s', os.arch(), cpu.totalCores, cpu.physicalCores, os.platform(), os.hostname());
	} else {
		return util.format('%s arch, %d cpus, %s platform, %s', os.arch(), cpu.totalCores, os.platform(), os.hostname());
	}
};

exports.start = function(script, opts) {
	var args = getWorkerArgs(script, opts);

	setConstants(opts);

	process.title = util.format('%s: master process', path.resolve(process.cwd(), script));

	initLog(args['name'], args['log']);
	outRedirect();

	console.info('starting agent ...');
	console.info('node:', process.version);
	console.info('version:', 'v' + pkg.version);

	deps.list(function(err, depslist) {
		if (!err) {
			console.info('dependencies:', depslist);
		}

		console.info('options:', util.inspect(args).replace(/[\n|\r]/g, ''));

		cpu.init(function(err) {
			if (err) {
				console.warn('%s, fallback to use os.cpus()', err);
			}

			console.info('device:', deviceInfo());

			bindEvents();
			God.prepare(args);

			initTarsComponent(args, opts);

			startWorker(opts);

			tarsNotify.report.info('restart');
		});
	});
};