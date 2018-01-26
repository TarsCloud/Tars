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

var timeProvider = require('../index').timeProvider;

var i = 0, count = 10000000;
var tt1, tt2, interval = 0;
var t1 = new Date().getTime();
var t2 = t1;

tt1 = timeProvider.nowTimestamp();
for(i = 0; i < count; i++)
{
    tt2 = timeProvider.diff(tt1);
}
t2 = new Date().getTime();
console.log('【hrTime】interval: ' + (t2 - t1));

t1 = new Date().getTime();
tt1 = timeProvider.dateTimestamp();
for(i = 0; i < count; i++)
{
    tt2 = timeProvider.dateTimestampDiff(tt1);
}
t2 = new Date().getTime();
console.log('【hrTime】interval: ' + (t2 - t1));

var tt3, tt4, tt5, tt6;
tt3 = timeProvider.nowTimestamp();
setTimeout(function(){
    tt4 = timeProvider.diff(tt3);
    console.log('diff: ', tt4);
}, 2000);

tt5 = timeProvider.dateTimestamp();
setTimeout(function(){
    tt6 = timeProvider.dateTimestampDiff(tt5);
    console.log('dateTimestampDiff: ', tt6);
}, 2000);