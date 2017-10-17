'use strict';

var assert = require('assert');

var lag = require('event-loop-lag');
var pidusage = require('pidusage');

var tarsMonitor = require('@tars/monitor').property;
var tarsConfigure = require('@tars/utils').Config;

var eventloop;

var moduleName;

var memTimerId, lagTimerId, cpuTimerId, libuvTimerId;

var rss, heapTotal, heapUsed, eventloopLag, cpuUsage, activeHandles, activeRequests;

var initConfig = function(obj) {
	var tarsConfig, setdivision, slaveSetName, slaveSetArea, slaveSetID;

	obj = obj || process.env.TARS_CONFIG;

	assert(obj, 'TARS_CONFIG is not in env and init argument is neither an Object nor a String.');

	tarsMonitor.init(obj);

	if (typeof obj === 'string') {
		tarsConfig = new tarsConfigure;
		tarsConfig.parseFile(obj);
	} else {
		tarsConfig = obj;
	}

	moduleName = tarsConfig.get('tars.application.client.modulename', '');
	setdivision = tarsConfig.get('tars.application.setdivision');

	if (tarsConfig.get('tars.application.enableset', '').toLowerCase() === 'y' && setdivision && typeof setdivision === 'string') {
		setdivision = setdivision.split('.');
		if (setdivision.length >= 3) {
			slaveSetName = setdivision[0];
			slaveSetArea = setdivision[1];
			slaveSetID = setdivision.slice(2).join('.');
			if (moduleName.indexOf('.') !== -1) {
				moduleName = moduleName.slice(moduleName.indexOf('.') + 1);
			}
			moduleName = slaveSetName + slaveSetArea + slaveSetID + '.' + moduleName;
		}
	}
};

var initReporter = function() {
	rss = tarsMonitor.create(moduleName + '.rss', [new tarsMonitor.POLICY.Max, 
		new tarsMonitor.POLICY.Min,
		new tarsMonitor.POLICY.Avg]);

	heapTotal = tarsMonitor.create(moduleName + '.heapTotal', [new tarsMonitor.POLICY.Max, 
		new tarsMonitor.POLICY.Min,
		new tarsMonitor.POLICY.Avg]); 

	heapUsed = tarsMonitor.create(moduleName + '.heapUsed', [new tarsMonitor.POLICY.Max, 
		new tarsMonitor.POLICY.Min,
		new tarsMonitor.POLICY.Avg]);

	eventloopLag = tarsMonitor.create(moduleName + '.eventLoop', [new tarsMonitor.POLICY.Max, 
		new tarsMonitor.POLICY.Min,
		new tarsMonitor.POLICY.Avg]);

	cpuUsage = tarsMonitor.create(moduleName + '.cpuUsage', [new tarsMonitor.POLICY.Max, 
		new tarsMonitor.POLICY.Min,
		new tarsMonitor.POLICY.Avg]);

	activeHandles = tarsMonitor.create(moduleName + '.activeHandles', [new tarsMonitor.POLICY.Max, 
		new tarsMonitor.POLICY.Min,
		new tarsMonitor.POLICY.Avg]);

	activeRequests = tarsMonitor.create(moduleName + '.activeRequests', [new tarsMonitor.POLICY.Max, 
		new tarsMonitor.POLICY.Min,
		new tarsMonitor.POLICY.Avg]);
};

var initLag = function() {
	eventloop = lag(exports.lagInterval);
};

var reportCpu = function() {
	pidusage.stat(process.pid, function(err, stat) {
		if (!err) {
			cpuUsage.report(stat.cpu.toFixed(3));
		}
	});
};

var reportMem = function() {
	var mem = process.memoryUsage();

	rss.report(mem.rss);
	heapTotal.report(mem.heapTotal);
	heapUsed.report(mem.heapUsed);
};

var reportLag = function() {
	eventloopLag.report(eventloop().toFixed(3));
};

var reportLibuv = function() {
	activeHandles.report(process._getActiveHandles().length);
	activeRequests.report(process._getActiveRequests().length);
};

exports.init = function(obj) {
	initConfig(obj);
	initReporter()
	initLag();
};

exports.start = function() {
	if (!rss) {
		exports.init();
	}

	if (!memTimerId) {
		memTimerId = setInterval(reportMem, exports.memInterval);
		memTimerId.unref();
	}

	if (!lagTimerId) {
		lagTimerId = setInterval(reportLag, exports.lagInterval);
		lagTimerId.unref();
	}

	if (!cpuTimerId) {
		cpuTimerId = setInterval(reportCpu, exports.cpuInterval);
		cpuTimerId.unref();
	}

	if (!libuvTimerId) {
		libuvTimerId = setInterval(reportLibuv, exports.libuvInterval);
		libuvTimerId.unref();
	}
};

exports.stop = function() {
	if (memTimerId) {
		clearInterval(memTimerId);
		memTimerId = undefined;
	}

	if (lagTimerId) {
		clearInterval(lagTimerId);
		lagTimerId = undefined;
	}

	if (cpuTimerId) {
		clearInterval(cpuTimerId);
		cpuTimerId = undefined;
	}

	pidusage.unmonitor(process.pid);

	if (libuvTimerId) {
		clearInterval(libuvTimerId);
		libuvTimerId = undefined;
	}
};

exports.lagInterval = 2 * 1000;
exports.memInterval = 5 * 1000;
exports.cpuInterval = 5 * 1000;
exports.libuvInterval = 10 * 1000;