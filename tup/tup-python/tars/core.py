#! /usr/bin/env python
# -*- coding: utf-8 -*-


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

__version__ = "0.0.1"

from __util import util
from __tars import TarsInputStream
from __tars import TarsOutputStream
from __tup import TarsUniPacket


class tarscore:
    class TarsInputStream(TarsInputStream):
        pass

    class TarsOutputStream(TarsOutputStream):
        pass

    class TarsUniPacket(TarsUniPacket):
        pass

    class boolean(util.boolean):
        pass

    class int8(util.int8):
        pass

    class uint8(util.uint8):
        pass

    class int16(util.int16):
        pass

    class uint16(util.uint16):
        pass

    class int32(util.int32):
        pass

    class uint32(util.uint32):
        pass

    class int64(util.int64):
        pass

    class float(util.float):
        pass

    class double(util.double):
        pass

    class bytes(util.bytes):
        pass

    class string(util.string):
        pass

    class struct(util.struct):
        pass

    @staticmethod
    def mapclass(ktype, vtype): return util.mapclass(ktype, vtype)

    @staticmethod
    def vctclass(vtype): return util.vectorclass(vtype)

    @staticmethod
    def printHex(buff): util.printHex(buff)


# 被用户引用
from __util import configParse
from __rpc import Communicator
from exception import *
from __logger import tarsLogger
