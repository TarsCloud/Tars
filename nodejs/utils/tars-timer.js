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

var timeProvider = module.exports = {};

// dateTimestamp和dateTimestampDiff配套使用
timeProvider.dateTimestamp = function() {
    var now = {};
    now.hrtime      = process.hrtime();
    now.timestamp   = parseInt(now.hrtime[0] * 1e3 + now.hrtime[1] * 1e-6);

    return now;
};

timeProvider.dateTimestampDiff = function(oTime) {
    var hrtime = process.hrtime(oTime.hrtime);

    return parseInt(hrtime[0] * 1e3 + hrtime[1] * 1e-6);
};

// nowTimestamp和diff配套使用
timeProvider.nowTimestamp = function() {
    return process.uptime() * 1000;
};

timeProvider.diff = function(oTime) {
    return process.uptime() * 1000 - oTime;
};