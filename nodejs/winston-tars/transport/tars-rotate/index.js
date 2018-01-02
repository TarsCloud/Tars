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

var util = require('util'),
	fs = require('fs'),
	path = require('path'),
	cluster = require('cluster');

var winston = require('winston');

var TarsBase = require('../tars-base'),
	TarsRotateCore = require('./core'),
	TarsRotateMaster = require('./master');

var TarsRotate = exports.TarsRotate = function (options) {
	var instance = TarsBase.call(this, options);

	if (instance) {
		return instance;
	}

	this.options.maxFiles = options.maxFiles || 10;
	this.options.maxSize = options.maxSize || 10 * 1024 * 1024; //10m
	this.options.concatStr = options.concatStr || '_';

	if ('isWorker' in options) {
		this._isWorker = !!options.isWorker;
	} else {
		this._isWorker = cluster.isWorker;
	}
};

util.inherits(TarsRotate, TarsBase);

TarsRotate.prototype.name = 'tarsRotate';

TarsRotate.prototype._checkfile = function(cb) {
	if (!this._isWorker) {
		TarsRotateCore.rotate(this._dirname,
			this._basename,
			this._extname,
			this.filename,
			this.options.maxFiles,
			this.options.maxSize,
			this.options.concatStr,
			cb);
	} else {
		try {
			process.send({
				cmd : 'log:rotate',
				data : {
					filename : this.filename,
					interval : this.options.interval,
					maxFiles : this.options.maxFiles,
					maxSize : this.options.maxSize,
					concatStr : this.options.concatStr
				}
			});
		} catch(e) {}
		
		cb();
	}
};

TarsRotate.Master = TarsRotateMaster;

winston.transports.TarsRotate = TarsRotate;