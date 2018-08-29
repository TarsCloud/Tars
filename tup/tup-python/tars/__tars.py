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
from __util import util
from exception import *


class BinBuffer:
    def __init__(self, buff=bytes()):
        self.buffer = buff
        self.position = 0

    def writeBuf(self, buff):
        self.buffer += buff

    def getBuffer(self):
        return self.buffer

    def length(self):
        return len(self.buffer)


class DataHead:
    EN_INT8 = 0
    EN_INT16 = 1
    EN_INT32 = 2
    EN_INT64 = 3
    EN_FLOAT = 4
    EN_DOUBLE = 5
    EN_STRING1 = 6
    EN_STRING4 = 7
    EN_MAP = 8
    EN_LIST = 9
    EN_STRUCTBEGIN = 10
    EN_STRUCTEND = 11
    EN_ZERO = 12
    EN_BYTES = 13

    @staticmethod
    def writeTo(buff, tag, vtype):
        if tag < 15:
            helper = (tag << 4) | vtype
            buff.writeBuf(struct.pack('!B', helper))
        else:
            helper = (0xF0 | vtype) << 8 | tag
            buff.writeBuf(struct.pack('!H', helper))


class TarsOutputStream(object):
    def __init__(self):
        self.__buffer = BinBuffer()

    def __writeBoolean(self, tag, value):
        self.__writeInt8(tag, int(value))

    def __writeInt8(self, tag, value):
        if value == 0:
            DataHead.writeTo(self.__buffer, tag, DataHead.EN_ZERO)
        else:
            DataHead.writeTo(self.__buffer, tag, DataHead.EN_INT8)
            self.__buffer.writeBuf(struct.pack('!b', value))

    def __writeInt16(self, tag, value):
        if value >= -128 and value <= 127:
            self.__writeInt8(tag, value)
        else:
            DataHead.writeTo(self.__buffer, tag, DataHead.EN_INT16)
            self.__buffer.writeBuf(struct.pack('!h', value))

    def __writeInt32(self, tag, value):
        if value >= -32768 and value <= 32767:
            self.__writeInt16(tag, value)
        else:
            DataHead.writeTo(self.__buffer, tag, DataHead.EN_INT32)
            self.__buffer.writeBuf(struct.pack('!i', value))

    def __writeInt64(self, tag, value):
        if value >= (-2147483648) and value <= 2147483647:
            self.__writeInt32(tag, value)
        else:
            DataHead.writeTo(self.__buffer, tag, DataHead.EN_INT64)
            self.__buffer.writeBuf(struct.pack('!q', value))

    def __writeFloat(self, tag, value):
        DataHead.writeTo(self.__buffer, tag, DataHead.EN_FLOAT)
        self.__buffer.writeBuf(struct.pack('!f', value))

    def __writeDouble(self, tag, value):
        DataHead.writeTo(self.__buffer, tag, DataHead.EN_DOUBLE)
        self.__buffer.writeBuf(struct.pack('!d', value))

    def __writeString(self, tag, value):
        length = len(value)
        if length <= 255:
            DataHead.writeTo(self.__buffer, tag, DataHead.EN_STRING1)
            self.__buffer.writeBuf(struct.pack('!B', length))
            self.__buffer.writeBuf(str.encode(value))
        else:
            DataHead.writeTo(self.__buffer, tag, DataHead.EN_STRING4)
            self.__buffer.writeBuf(struct.pack('!I', length))
            self.__buffer.writeBuf(str.encode(value))

    def __writeBytes(self, tag, value):
        DataHead.writeTo(self.__buffer, tag, DataHead.EN_BYTES)
        DataHead.writeTo(self.__buffer, 0,   DataHead.EN_INT8)
        length = len(value)
        self.__writeInt32(0, length)
        self.__buffer.buffer += value
        self.__buffer.position += length

    def __writeMap(self, coder, tag, value):
        DataHead.writeTo(self.__buffer, tag, DataHead.EN_MAP)
        self.__writeInt32(0, len(value))
        for key in value:
            self.write(coder.ktype, 0, key)
            self.write(coder.vtype, 1, value.get(key))

    def __writeVector(self, coder, tag, value):
        DataHead.writeTo(self.__buffer, tag, DataHead.EN_LIST)
        n = len(value)
        self.__writeInt32(0, n)
        for i in range(0, n):
            self.write(value.vtype, 0, value[i])

    def __writeStruct(self, coder, tag, value):
        DataHead.writeTo(self.__buffer, tag, DataHead.EN_STRUCTBEGIN)
        value.writeTo(self, value)
        DataHead.writeTo(self.__buffer, 0,   DataHead.EN_STRUCTEND)

    def write(self, coder, tag, value):
        if coder.__tars_index__ == 999:
            self.__writeBoolean(tag, value)
        elif coder.__tars_index__ == 0:
            self.__writeInt8(tag, value)
        elif coder.__tars_index__ == 1:
            self.__writeInt16(tag, value)
        elif coder.__tars_index__ == 2:
            self.__writeInt32(tag, value)
        elif coder.__tars_index__ == 3:
            self.__writeInt64(tag, value)
        elif coder.__tars_index__ == 4:
            self.__writeFloat(tag, value)
        elif coder.__tars_index__ == 5:
            self.__writeDouble(tag, value)
        elif coder.__tars_index__ == 13:
            self.__writeBytes(tag, value)
        elif coder.__tars_index__ == 67:
            self.__writeString(tag, value)
        elif coder.__tars_index__ == 8:
            self.__writeMap(coder, tag, value)
        elif coder.__tars_index__ == 9:
            self.__writeVector(coder, tag, value)
        elif coder.__tars_index__ == 1011:
            self.__writeStruct(coder, tag, value)
        else:
            raise TarsTarsUnsupportType(
                "tars unsupport data type:" % coder.__tars_index__)

    def getBuffer(self):
        return self.__buffer.getBuffer()

    def printHex(self):
        util.printHex(self.__buffer.getBuffer())


class TarsInputStream(object):
    def __init__(self, buff):
        self.__buffer = BinBuffer(buff)

    def __peekFrom(self):
        helper, = struct.unpack_from(
            '!B', self.__buffer.buffer, self.__buffer.position)
        t = (helper & 0xF0) >> 4
        p = (helper & 0x0F)
        l = 1
        if t >= 15:
            l = 2
            t, = struct.unpack_from(
                '!B', self.__buffer.buffer, self.__buffer.position + 1)
        return (t, p, l)

    def __readFrom(self):
        t, p, l = self.__peekFrom()
        self.__buffer.position += l
        return (t, p, l)

    def __skipToStructEnd(self):
        t, p, l = self.__readFrom()
        while p != DataHead.EN_STRUCTEND:
            self.__skipField(p)
            t, p, l = self.__readFrom()

    def __skipField(self, p):
        if p == DataHead.EN_INT8:
            self.__buffer.position += 1
        elif p == DataHead.EN_INT16:
            self.__buffer.position += 2
        elif p == DataHead.EN_INT32:
            self.__buffer.position += 4
        elif p == DataHead.EN_INT64:
            self.__buffer.position += 8
        elif p == DataHead.EN_FLOAT:
            self.__buffer.position += 4
        elif p == DataHead.EN_DOUBLE:
            self.__buffer.position += 8
        elif p == DataHead.EN_STRING1:
            length, = struct.unpack_from(
                '!B', self.__buffer.buffer, self.__buffer.position)
            self.__buffer.position += length + 1
        elif p == DataHead.EN_STRING4:
            length, = struct.unpack_from(
                '!i', self.__buffer.buffer, self.__buffer.position)
            self.__buffer.position += length + 4
        elif p == DataHead.EN_MAP:
            size = self.__readInt32(0, True)
            for i in range(0, size * 2):
                ti, pi, li = self.__readFrom()
                self.__skipField(pi)
        elif p == DataHead.EN_LIST:
            size = self.__readInt32(0, True)
            for i in range(0, size):
                ti, pi, li = self.__readFrom()
                self.__skipField(pi)
        elif p == DataHead.EN_BYTES:
            ti, pi, li = self.__readFrom()
            if pi != DataHead.EN_INT8:
                raise TarsTarsDecodeInvalidValue(
                    "skipField with invalid type, type value: %d, %d." % (p, pi))
            size = self.__readInt32(0, True)
            self.__buffer.position += size
        elif p == DataHead.EN_STRUCTBEGIN:
            self.__skipToStructEnd()
        elif p == DataHead.EN_STRUCTEND:
            pass
            #self.__buffer.position += length + 1;
        elif p == DataHead.EN_ZERO:
            pass
            #self.__buffer.position += length + 1;
        else:
            raise TarsTarsDecodeMismatch(
                "skipField with invalid type, type value:%d" % p)

    def __skipToTag(self, tag):
        length = self.__buffer.length()
        while self.__buffer.position < length:
            t, p, l = self.__peekFrom()
            if tag <= t or p == DataHead.EN_STRUCTEND:
                return False if (p == DataHead.EN_STRUCTEND) else (t == tag)

            self.__buffer.position += l
            self.__skipField(p)
        return False

    def __readBoolean(self, tag, require, default=None):
        v = self.__readInt8(tag, require)
        if v is None:
            return default
        else:
            return (v != 0)

    def __readInt8(self, tag, require, default=None):
        if self.__skipToTag(tag):
            t, p, l = self.__readFrom()
            if p == DataHead.EN_ZERO:
                return 0
            elif p == DataHead.EN_INT8:
                value, = struct.unpack_from(
                    '!b', self.__buffer.buffer, self.__buffer.position)
                self.__buffer.position += 1
                return value
            else:
                raise TarsTarsDecodeMismatch(
                    "read 'Char' type mismatch, tag: %d , get type: %d." % (tag, p))
        elif require:
            raise TarsTarsDecodeRequireNotExist(
                "require field not exist, tag: %d" % tag)
        return default

    def __readInt16(self, tag, require, default=None):
        if self.__skipToTag(tag):
            t, p, l = self.__readFrom()
            if p == DataHead.EN_ZERO:
                return 0
            elif p == DataHead.EN_INT8:
                value, = struct.unpack_from(
                    '!b', self.__buffer.buffer, self.__buffer.position)
                self.__buffer.position += 1
                return value
            elif p == DataHead.EN_INT16:
                value, = struct.unpack_from(
                    '!h', self.__buffer.buffer, self.__buffer.position)
                self.__buffer.position += 2
                return value
            else:
                raise TarsTarsDecodeMismatch(
                    "read 'Short' type mismatch, tag: %d , get type: %d." % (tag, p))
        elif require:
            raise TarsTarsDecodeRequireNotExist(
                "require field not exist, tag: %d" % tag)
        return default

    def __readInt32(self, tag, require, default=None):
        if self.__skipToTag(tag):
            t, p, l = self.__readFrom()
            if p == DataHead.EN_ZERO:
                return 0
            elif p == DataHead.EN_INT8:
                value, = struct.unpack_from(
                    '!b', self.__buffer.buffer, self.__buffer.position)
                self.__buffer.position += 1
                return value
            elif p == DataHead.EN_INT16:
                value, = struct.unpack_from(
                    '!h', self.__buffer.buffer, self.__buffer.position)
                self.__buffer.position += 2
                return value
            elif p == DataHead.EN_INT32:
                value, = struct.unpack_from(
                    '!i', self.__buffer.buffer, self.__buffer.position)
                self.__buffer.position += 4
                return value
            else:
                raise TarsTarsDecodeMismatch(
                    "read 'Int32' type mismatch, tag: %d, get type: %d." % (tag, p))
        elif require:
            raise TarsTarsDecodeRequireNotExist(
                "require field not exist, tag: %d" % tag)
        return default

    def __readInt64(self, tag, require, default=None):
        if self.__skipToTag(tag):
            t, p, l = self.__readFrom()
            if p == DataHead.EN_ZERO:
                return 0
            elif p == DataHead.EN_INT8:
                value, = struct.unpack_from(
                    '!b', self.__buffer.buffer, self.__buffer.position)
                self.__buffer.position += 1
                return value
            elif p == DataHead.EN_INT16:
                value, = struct.unpack_from(
                    '!h', self.__buffer.buffer, self.__buffer.position)
                self.__buffer.position += 2
                return value
            elif p == DataHead.EN_INT32:
                value, = struct.unpack_from(
                    '!i', self.__buffer.buffer, self.__buffer.position)
                self.__buffer.position += 4
                return value
            elif p == DataHead.EN_INT64:
                value, = struct.unpack_from(
                    '!q', self.__buffer.buffer, self.__buffer.position)
                self.__buffer.position += 8
                return value
            else:
                raise TarsTarsDecodeMismatch(
                    "read 'Int64' type mismatch, tag: %d, get type: %d." % (tag, p))
        elif require:
            raise TarsTarsDecodeRequireNotExist(
                "require field not exist, tag: %d" % tag)
        return default

    def __readString(self, tag, require, default=None):
        if self.__skipToTag(tag):
            t, p, l = self.__readFrom()
            if p == DataHead.EN_STRING1:
                length, = struct.unpack_from(
                    '!B', self.__buffer.buffer, self.__buffer.position)
                self.__buffer.position += 1
                value, = struct.unpack_from(
                    str(length) + "s", self.__buffer.buffer, self.__buffer.position)
                self.__buffer.position += length
                return value
            elif p == DataHead.EN_STRING4:
                length, = struct.unpack_from(
                    '!i', self.__buffer.buffer, self.__buffer.position)
                self.__buffer.position += 4
                value, = struct.unpack_from(
                    str(length) + "s", self.__buffer.buffer, self.__buffer.position)
                self.__buffer.position += length
                return value
            else:
                raise TarsTarsDecodeMismatch(
                    "read 'string' type mismatch, tag: %d, get type: %d." % (tag, p))
        elif require:
            raise TarsTarsDecodeRequireNotExist(
                "require field not exist, tag: %d" % tag)
        return default

    def __readBytes(self, tag, require, default=None):
        if self.__skipToTag(tag):
            t, p, l = self.__readFrom()
            if p == DataHead.EN_BYTES:
                ti, pi, li = self.__readFrom()
                if pi != DataHead.EN_INT8:
                    raise TarsTarsDecodeMismatch(
                        "type mismatch, tag: %d, type: %d, %d" % (tag, p, pi))
                size = self.__readInt32(0, True)
                value, = struct.unpack_from(
                    str(size) + 's', self.__buffer.buffer, self.__buffer.position)
                self.__buffer.position += size
                return value
            else:
                raise TarsTarsDecodeMismatch(
                    "type mismatch, tag: %d, type: %d" % (tag, p))
        elif require:
            raise TarsTarsDecodeRequireNotExist(
                "require field not exist, tag: %d" % tag)
        return default

    def __readFloat(self, tag, require, default=None):
        if self.__skipToTag(tag):
            t, p, l = self.__readFrom()
            if p == DataHead.EN_ZERO:
                return 0
            elif p == DataHead.EN_FLOAT:
                value, = struct.unpack_from(
                    '!f', self.__buffer.buffer, self.__buffer.position)
                self.__buffer.position += 4
                return value
            else:
                raise TarsTarsDecodeMismatch(
                    "read 'Float' type mismatch, tag: %d, get type: %d." % (tag, p))
        elif require:
            raise TarsTarsDecodeRequireNotExist(
                "require field not exist, tag: %d" % tag)
        return default

    def __readDouble(self, tag, require, default=None):
        if self.__skipToTag(tag):
            t, p, l = self.__readFrom()
            if p == DataHead.EN_ZERO:
                return 0
            elif p == DataHead.EN_FLOAT:
                value, = struct.unpack_from(
                    '!f', self.__buffer.buffer, self.__buffer.position)
                self.__buffer.position += 4
                return value
            elif p == DataHead.EN_DOUBLE:
                value, = struct.unpack_from(
                    '!d', self.__buffer.buffer, self.__buffer.position)
                self.__buffer.position += 8
                return value
            else:
                raise TarsTarsDecodeMismatch(
                    "read 'Double' type mismatch, tag: %d, get type: %d." % (tag, p))
        elif require:
            raise TarsTarsDecodeRequireNotExist(
                "require field not exist, tag: %d" % tag)
        return default

    def __readStruct(self, coder, tag, require, default=None):
        if self.__skipToTag(tag):
            t, p, l = self.__readFrom()
            if p != DataHead.EN_STRUCTBEGIN:
                raise TarsTarsDecodeMismatch(
                    "read 'struct' type mismatch, tag: %d, get type: %d." % (tag, p))
            value = coder.readFrom(self)
            self.__skipToStructEnd()
            return value
        elif require:
            raise TarsTarsDecodeRequireNotExist(
                "require field not exist, tag: %d" % tag)
        return default

    def __readMap(self, coder, tag, require, default=None):
        if self.__skipToTag(tag):
            t, p, l = self.__readFrom()
            if p == DataHead.EN_MAP:
                size = self.__readInt32(0, True)
                omap = coder()
                for i in range(0, size):
                    k = self.read(coder.ktype, 0, True)
                    v = self.read(coder.vtype, 1, True)
                    omap[k] = v
                return omap
            else:
                raise TarsTarsDecodeMismatch(
                    "read 'map' type mismatch, tag: %d, get type: %d." % (tag, p))
        elif require:
            raise TarsTarsDecodeRequireNotExist(
                "require field not exist, tag: %d" % tag)
        return default

    def __readVector(self, coder, tag, require, default=None):
        if self.__skipToTag(tag):
            t, p, l = self.__readFrom()
            if p == DataHead.EN_LIST:
                size = self.__readInt32(0, True)
                value = coder()
                for i in range(0, size):
                    k = self.read(coder.vtype, 0, True)
                    value.append(k)
                return value
            else:
                raise TarsTarsDecodeMismatch(
                    "read 'vector' type mismatch, tag: %d, get type: %d." % (tag, p))
        elif require:
            raise TarsTarsDecodeRequireNotExist(
                "require field not exist, tag: %d" % tag)
        return default

    def read(self, coder, tag, require, default=None):
        if coder.__tars_index__ == 999:
            return self.__readBoolean(tag, require, default)
        elif coder.__tars_index__ == 0:
            return self.__readInt8(tag, require, default)
        elif coder.__tars_index__ == 1:
            return self.__readInt16(tag, require, default)
        elif coder.__tars_index__ == 2:
            return self.__readInt32(tag, require, default)
        elif coder.__tars_index__ == 3:
            return self.__readInt64(tag, require, default)
        elif coder.__tars_index__ == 4:
            return self.__readFloat(tag, require, default)
        elif coder.__tars_index__ == 5:
            return self.__readDouble(tag, require, default)
        elif coder.__tars_index__ == 13:
            return self.__readBytes(tag, require, default)
        elif coder.__tars_index__ == 67:
            return self.__readString(tag, require, default)
        elif coder.__tars_index__ == 8:
            return self.__readMap(coder, tag, require, default)
        elif coder.__tars_index__ == 9:
            return self.__readVector(coder, tag, require, default)
        elif coder.__tars_index__ == 1011:
            return self.__readStruct(coder, tag, require, default)
        else:
            raise TarsTarsUnsupportType(
                "tars unsupport data type:" % coder.__tars_index__)

    def printHex(self):
        util.printHex(self.__buffer.buffer)
