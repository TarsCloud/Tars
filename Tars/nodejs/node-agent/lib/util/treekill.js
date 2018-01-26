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

var childProcess = require('child_process');
var exec = childProcess.exec;
var isWindows = process.platform === 'win32';
var downgradePs = false;

module.exports = function (pid, signal, callback) {
	if (isWindows) {
			exec('taskkill /pid ' + pid + ' /T /F', callback);
	} else {
			var tree = {};
			var pidsToProcess = {};

			buildProcessTree(pid, tree, pidsToProcess, function () {
					try {
							killAll(tree, signal);
					}
					catch (err) {
							if (callback) {
									return callback(err);
  						} else {
									throw err;
							}
					}
					if (callback) {
							return callback();
					}
			});
	}
}

function killAll (tree, signal) {
	var killed = {};
	Object.keys(tree).forEach(function (pid) {
			tree[pid].forEach(function (pidpid) {
					if (!killed[pidpid]) {
							killPid(pidpid, signal);
							killed[pidpid] = 1;
					}
			});
			if (!killed[pid]) {
					killPid(pid, signal);
					killed[pid] = 1;
			}
	});
}

function killPid(pid, signal) {
	try {
			process.kill(parseInt(pid, 10), signal);
	}
	catch (err) {
			if (err.code !== 'ESRCH') throw err;
	}
}

function buildProcessTree(ppid, tree, pidsToProcess, cb) {
	pidsToProcess[ppid] = 1;
	tree[ppid] = [];

	function isFinish(){
		delete pidsToProcess[ppid];
		if(Object.keys(pidsToProcess).length == 0){
			return cb();
		}
	}

	var args = downgradePs ? '-eo pid,ppid | grep -w ' : '-o pid --no-headers --ppid ';
	var ps = exec('ps ' + args + ppid, function(err, stdout, stderr){
		if (err) {
			// illegal option --, try to use basic `ps` instead of it.
			if (/illegal/.test(err.message) && !downgradePs) {
				downgradePs = true;
				return buildProcessTree(ppid, tree, pidsToProcess, cb);
			}

			// Avoid pipe close error - dynamic self-closing process.
			if(/Command failed/.test(err.message)) {
				return isFinish();
			}
			throw err;
		}

		var pids = stdout.split('\n');

		// remove parentPid if necessary.
		downgradePs && pids.shift();

		pids = pids.filter(function(pid){
			return !!pid;
		}).map(function(pid){
			pid = pid.trim();
			return parseInt(downgradePs ? pid.slice(0, pid.search(/\s/)) : pid, 10);
		});

		if(pids.length > 0){
			tree[ppid] = tree[ppid].concat(pids);
			pids.forEach(function(pid){
				if(!tree[pid]) {
					buildProcessTree(pid, tree, pidsToProcess, cb);
				}else{
					delete pidsToProcess[pid];
				}
			});
		}

		isFinish();
	});
}