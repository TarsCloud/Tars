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
	fs = require('fs'),
	assert = require('assert'),
	util = require('util');

var winston = require('winston'),
	common = require('winston/lib/winston/common');

var mkdirp = require('./util/mkdirp');

var	Formatter = require('../lib/formatter');

var defaultMaxListeners = require('events').EventEmitter.defaultMaxListeners || 10;

var instances = {};

var TarsBase = function (options) {
	assert(options.filename, 'Cannot log to file without filename');

	if (instances[options.filename]) {
		instances[options.filename].ref += 1;
		instances[options.filename].obj.setMaxListeners(instances[options.filename].ref * defaultMaxListeners);
		return instances[options.filename].obj;
	}

	winston.Transport.call(this, options);

	this.filename = options.filename;
	this.options = {
		interval : options.interval || 5 * 1000,
		bufferSize : options.bufferSize || 10000,
		formatter : options.formatter || Formatter.Detail(),
		prefix : options.prefix
	};

	this._opening = false;
	this._dirchecked = false;
	this._buffer = [];
	this._bufferIndex = 0;

	this._orgname = this.filename;
	this._dirname = path.dirname(this.filename);
	this._extname = path.extname(this.filename);
	this._basename = path.basename(this.filename, this._extname);

	instances[this._orgname] = {
		ref : 1,
		obj : this
	};
};

util.inherits(TarsBase, winston.Transport);

TarsBase.prototype.name = 'tarsBase';

TarsBase.prototype.log = function (level, msg, meta, callback) {
	var output = common.log({
		level : level,
		message : msg,
		meta : meta,
		formatter : this.options.formatter,
		prefix : this.options.prefix
	}) + '\n';

	if (this._getFileStream()) {
		this._write(output, callback);
	} else {
		this._buffer[this._bufferIndex++] = {
			output : output,
			callback : callback
		};
		if (this._bufferIndex > this.options.bufferSize) {
			this._bufferIndex = 0;
		}
	}
};

TarsBase.prototype._write = function(data, callback) {
	var ths = this;

	this._stream.write(data);

	if (!this._timerid) {
		this._timerid = setTimeout(function() {
			ths._closeStream();

			ths._timerid = undefined;
		}, this.options.interval);
	}

	callback(null, true);
};

TarsBase.prototype._checkfile = function(cb) {
	this.emit('checkfile');
	
	fs.stat(this.filename, function(err, stats) {
		if (err) {
			if (err.code !== 'ENOENT') {
				cb(err);
				return;
			}
		}

		cb();
	});
};

TarsBase.prototype._checkdir = function(cb) {
	var ths = this;

	if (this._dirchecked) {
		cb();
	} else {
		mkdirp(this._dirname, function(err) {
			ths._dirchecked = !err;
			cb(err);
		});
	}
};

TarsBase.prototype._getFileStream = function() {
	var ths = this;

	if (this._stream) {
		return true;
	}

	if (this._opening) {
		return false;
	}

	this._opening = true;

	this._checkdir(function(err) {
		if (err) {
			ths.emit('error', err);
			return;
		}

		ths._checkfile(function(err) {
			var i = 0;

			if (err) {
				ths.emit('error', err);
				return;
			}

			setImmediate(function() {
				ths._stream = fs.createWriteStream(ths.filename, {
					flags : 'a'
				});
				ths._stream.on('error', function(err) {
					ths.emit('error', err);
				});

				for (i = ths._bufferIndex; i < ths._buffer.length; i += 1) {
					ths._write(ths._buffer[i].output, ths._buffer[i].callback);
				}
				for (i = 0; i < ths._bufferIndex; i += 1) {
					ths._write(ths._buffer[i].output, ths._buffer[i].callback);
				}
				ths._bufferIndex = 0;
				ths._buffer = [];
				
				ths._opening = false;
			});
		});
	});

	return false;
};

TarsBase.prototype._closeStream = function() {
	if (this._stream) {
		this._stream.removeAllListeners('error');
		
		this._stream.end(function(_stream) {
			return function() {
				_stream.destroy();
				_stream = undefined;
			}
		}(this._stream));

		this._stream = undefined;
	}
};

TarsBase.prototype.close = function() {
	if (this._timerid) {
		clearTimeout(this._timerid);
		this._timerid = undefined;
	}

	this._buffer = [];
	this._bufferIndex = 0;

	this._closeStream();

	if (instances[this._orgname]) {
		if (instances[this._orgname].ref > 1) {
			instances[this._orgname].ref -= 1;
			instances[this._orgname].obj.setMaxListeners(instances[this._orgname].ref * defaultMaxListeners);
		} else {
			delete instances[this._orgname];
		}
	}
};

module.exports = winston.transports.TarsBase = TarsBase;