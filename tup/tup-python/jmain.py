# Tencent is pleased to support the open source community by making Tars available.
#
# Copyright (C) 2016THL A29 Limited, a Tencent company. All rights reserved.
#
# Licensed under the BSD 3-Clause License (the "License"); you may not use this file except 
# in compliance with the License. You may obtain a copy of the License at
#
# https://opensource.org/licenses/BSD-3-Clause
#
# Unless required by applicable law or agreed to in writing, software distributed 
# under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
# CONDITIONS OF ANY KIND, either express or implied. See the License for the 
# specific language governing permissions and limitations under the License.
#

from tars.core import tarscore;

try:
    #STEP01 typedef
    mapcls = tarscore.mapclass(tarscore.string, tarscore.string);

    #STEP02 evaluate
    url = mapcls();
    url["google"] = "http://www.google.com.hk";
    url["baidu"]  = "http://www.baidu.com";
    url["sina"]   = "http://www.sina.com.cn";

    #STEP03 write vars
    oos = tarscore.TarsOutputStream();
    oos.write(tarscore.string, 0, "abcdefg");
    oos.write(tarscore.int32, 1, 0x12345678);
    oos.write(mapcls, 2, url);

    #STEP04 decode
    ios = tarscore.TarsInputStream(oos.getBuffer());
    nam = ios.read(tarscore.string, 0, True);
    num = ios.read(tarscore.int32, 1, True);
    mul = ios.read(mapcls, 2, True); 

    #STEP05 print vars
    print "name:", nam;
    print "num:", num;
    print "map:", mul.size();

    for key in mul:
        print "key:", key, ", value:", mul[key];

except Exception as e:
    print(e);
