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

import struct
import string
from __util import util
from __tars import TarsOutputStream
from __tars import TarsInputStream
from __packet import RequestPacket


class TarsUniPacket(object):
    def __init__(self):
        self.__mapa = util.mapclass(util.string, util.bytes)
        self.__mapv = util.mapclass(util.string, self.__mapa)
        self.__buffer = self.__mapv()
        self.__code = RequestPacket()

    # @property
    # def version(self):
    #     return self.__code.iVersion

    # @version.setter
    # def version(self, value):
    #     self.__code.iVersion = value

    @property
    def servant(self):
        return self.__code.sServantName

    @servant.setter
    def servant(self, value):
        self.__code.sServantName = value

    @property
    def func(self):
        return self.__code.sFuncName

    @func.setter
    def func(self, value):
        self.__code.sFuncName = value

    @property
    def requestid(self):
        return self.__code.iRequestId

    @requestid.setter
    def requestid(self, value):
        self.__code.iRequestId = value

    @property
    def result_code(self):
        if self.__code.status.has_key("STATUS_RESULT_CODE") == False:
            return 0

        return string.atoi(self.__code.status["STATUS_RESULT_CODE"])

    @property
    def result_desc(self):
        if self.__code.status.has_key("STATUS_RESULT_DESC") == False:
            return ''

        return self.__code.status["STATUS_RESULT_DESC"]

    def put(self, vtype, name, value):
        oos = TarsOutputStream()
        oos.write(vtype, 0, value)
        self.__buffer[name] = {vtype.__tars_class__: oos.getBuffer()}

    def get(self, vtype, name):
        if self.__buffer.has_key(name) == False:
            raise Exception("UniAttribute not found key:%s,type:%s" %
                            (name, vtype.__tars_class__))

        t = self.__buffer[name]
        if t.has_key(vtype.__tars_class__) == False:
            raise Exception("UniAttribute not found type:" +
                            vtype.__tars_class__)

        o = TarsInputStream(t[vtype.__tars_class__])
        return o.read(vtype, 0, True)

    def encode(self):
        oos = TarsOutputStream()
        oos.write(self.__mapv, 0, self.__buffer)

        self.__code.iVersion = 2
        self.__code.sBuffer = oos.getBuffer()

        sos = TarsOutputStream()
        RequestPacket.writeTo(sos, self.__code)

        return struct.pack('!i', 4 + len(sos.getBuffer())) + sos.getBuffer()

    def decode(self, buf):
        ois = TarsInputStream(buf[4:])
        self.__code = RequestPacket.readFrom(ois)

        sis = TarsInputStream(self.__code.sBuffer)
        self.__buffer = sis.read(self.__mapv, 0, True)

    def clear(self):
        self.__code.__init__()

    def haskey(self, name):
        return self.__buffer.has_key(name)
