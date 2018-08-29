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

from core import tarscore


class EndpointF(tarscore.struct):
    __tars_class__ = "register.EndpointF"

    def __init__(self):
        self.host = ""
        self.port = 0
        self.timeout = 0
        self.istcp = 0
        self.grid = 0
        self.groupworkid = 0
        self.grouprealid = 0
        self.setId = ""
        self.qos = 0
        self.bakFlag = 0
        self.weight = 0
        self.weightType = 0

    @staticmethod
    def writeTo(oos, value):
        oos.write(tarscore.string, 0, value.host)
        oos.write(tarscore.int32, 1, value.port)
        oos.write(tarscore.int32, 2, value.timeout)
        oos.write(tarscore.int32, 3, value.istcp)
        oos.write(tarscore.int32, 4, value.grid)
        oos.write(tarscore.int32, 5, value.groupworkid)
        oos.write(tarscore.int32, 6, value.grouprealid)
        oos.write(tarscore.string, 7, value.setId)
        oos.write(tarscore.int32, 8, value.qos)
        oos.write(tarscore.int32, 9, value.bakFlag)
        oos.write(tarscore.int32, 11, value.weight)
        oos.write(tarscore.int32, 12, value.weightType)

    @staticmethod
    def readFrom(ios):
        value = EndpointF()
        value.host = ios.read(tarscore.string, 0, True, value.host)
        value.port = ios.read(tarscore.int32, 1, True, value.port)
        value.timeout = ios.read(tarscore.int32, 2, True, value.timeout)
        value.istcp = ios.read(tarscore.int32, 3, True, value.istcp)
        value.grid = ios.read(tarscore.int32, 4, True, value.grid)
        value.groupworkid = ios.read(
            tarscore.int32, 5, False, value.groupworkid)
        value.grouprealid = ios.read(
            tarscore.int32, 6, False, value.grouprealid)
        value.setId = ios.read(tarscore.string, 7, False, value.setId)
        value.qos = ios.read(tarscore.int32, 8, False, value.qos)
        value.bakFlag = ios.read(tarscore.int32, 9, False, value.bakFlag)
        value.weight = ios.read(tarscore.int32, 11, False, value.weight)
        value.weightType = ios.read(
            tarscore.int32, 12, False, value.weightType)
        return value
