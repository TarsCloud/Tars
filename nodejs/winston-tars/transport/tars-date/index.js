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
	path = require('path');

var strftime = require('strftime'),
	winston = require('winston');

var TarsBase = require('../tars-base'),
	TarsDateFormat = require('../util/date-format');

var Formatter = require('../../lib/formatter');

var TarsDate = exports.TarsDate = function (options) {
	var instance = TarsBase.call(this, options);

	if (instance) {
		return instance;
	}

	this.options.concatStr = options.concatStr || '_';
	this.options.formatter = options.formatter || Formatter.Simple();
	this.options.format = options.format || TarsDateFormat.LogByDay;

	if (typeof this.options.format === 'function') {
		this.options.format = new this.options.format();
	}
};

util.inherits(TarsDate, TarsBase);

TarsDate.prototype.name = 'tarsDate';

TarsDate.prototype._checkfile = function(cb) {
	var now = new Date, regenerate = true, nowTime;

	if (this.options.format.name !== 'custom') {
		nowTime = parseInt(strftime(this.options.format.timePattern, now));

		if (this._lastTime && nowTime - this._lastTime < this.options.format.interval) {
			regenerate = false;
		} else {
			this._lastTime = nowTime;
		}
	}

	if (regenerate) {
		this.filename = path.join(this._dirname, this._basename + '_' + strftime(this.options.format.logPattern, now) + this._extname);
	}

	fs.stat(this.filename, function(err, stats) {
		if (err && err.code !== 'ENOENT') {
			cb(err);
			return;
		}

		cb();
	});
};

TarsDate.FORMAT = TarsDateFormat;

winston.transports.TarsDate = TarsDate;