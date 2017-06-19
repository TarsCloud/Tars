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
    #STEP01 evaluate
    etup = tarscore.TarsUniPacket();
    etup.servant = "tars.registry.queryobj";
    etup.func = "getendpoints";
    etup.put(tarscore.int16, "aa", 12345);
    etup.put(tarscore.string, "bb", "abcdefghitssssssh");

    #STEP02 encode 
    buff = etup.encode();

    #STEP03 decode
    dtup = tarscore.TarsUniPacket();
    dtup.decode(buff);

    #STEP04 get var
    aa = dtup.get(tarscore.int16, "aa");
    bb = dtup.get(tarscore.string, "bb");

    #STEP05 print vars
    print "servant:", dtup.servant;
    print "func:", dtup.func;
    print "aa:", aa;
    print "bb:", bb;
except Exception as e:
    print e
