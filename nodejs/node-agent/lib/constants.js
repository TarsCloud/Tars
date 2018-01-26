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

var conf = {
	ENTRY_POINT_NAME : ['server.js', 'app.js', 'start.js', 'index.js'],
	GRACEFUL_TIMEOUT : 1000 * 8,
	EXCEPTION_TOTAL : 2,
	EXCEPTION_TIME : 1000 * 10,
	APPLOG_MAX_FILES : 10,
	APPLOG_MAX_SIZE : 1024 * 1024 * 10,
	APPLOG_LEVEL : 'DEBUG',
	WORKER_STATUS : {
		ONLINE : 1,
		STOPPED : 2,
		STOPPING : 3,
		LAUNCHING : 4,
		ERRORED : 5
	},
	GOD_MESSAGE : {
		EXCEPTION_REACHED_COND : 1,
		KILLING_ALL_WORKERS : 2,
		KILLING_WORKER : 3,
		FORCE_KILL_WORKER : 4,
		ALL_WORKERS_STOPPED : 5,
		STOP_ZOMBIE_WORKER : 6,
		KILL_ERROR : 7
	},
	CAN_START_WORKER : {
		OK : 1,
		NEED_TO_KILLALL : 2,
		ALREADY_SEND_CMD : 3
	},
	TARS_DEFAULT_APP : 'DEFAULT_APP',
	TARS_HEART_BEAT_INTERVAL : 10 * 1000,
	TARS_MONITOR : true,
	TARS_MONITOR_HTTP_THRESHOLD : 400,
	TARS_MONITOR_HTTP_SEPPATH : true,
	WORKER_HEART_BEAT_TIMES : 5,
	WORKER_DETECT_INTERVAL : 5 * 60,
	CODE_UNCAUGHTEXCEPTION : 100
};

module.exports = conf;