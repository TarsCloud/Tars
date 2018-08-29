#!/usr/bin/env python
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


import sys
from threading import Lock
import hashlib
from xml.etree import cElementTree as ET
from exception import TarsException


class util:
    @staticmethod
    def printHex(buff):
        count = 0
        for c in buff:
            sys.stdout.write("0X%02X " % ord(c))
            count += 1
            if count % 16 == 0:
                sys.stdout.write("\n")
        sys.stdout.write("\n")
        sys.stdout.flush()

    @staticmethod
    def mapclass(ktype, vtype):
        class mapklass(dict):
            def size(self): return len(self)
        setattr(mapklass, '__tars_index__', 8)
        setattr(mapklass, '__tars_class__', "map<" +
                ktype.__tars_class__ + "," + vtype.__tars_class__ + ">")
        setattr(mapklass, 'ktype', ktype)
        setattr(mapklass, 'vtype', vtype)
        return mapklass

    @staticmethod
    def vectorclass(vtype):
        class klass(list):
            def size(self): return len(self)
        setattr(klass, '__tars_index__', 9)
        setattr(klass, '__tars_class__', "list<" + vtype.__tars_class__ + ">")
        setattr(klass, 'vtype', vtype)
        return klass

    class boolean:
        __tars_index__ = 999
        __tars_class__ = "bool"

    class int8:
        __tars_index__ = 0
        __tars_class__ = "char"

    class uint8:
        __tars_index__ = 1
        __tars_class__ = "short"

    class int16:
        __tars_index__ = 1
        __tars_class__ = "short"

    class uint16:
        __tars_index__ = 2
        __tars_class__ = "int32"

    class int32:
        __tars_index__ = 2
        __tars_class__ = "int32"

    class uint32:
        __tars_index__ = 3
        __tars_class__ = "int64"

    class int64:
        __tars_index__ = 3
        __tars_class__ = "int64"

    class float:
        __tars_index__ = 4
        __tars_class__ = "float"

    class double:
        __tars_index__ = 5
        __tars_class__ = "double"

    class bytes:
        __tars_index__ = 13
        __tars_class__ = "list<char>"

    class string:
        __tars_index__ = 67
        __tars_class__ = "string"

    class struct:
        __tars_index__ = 1011


def xml2dict(node, dic={}):
    '''
    @brief: 将xml解析树转成字典
    @param node: 树的根节点
    @type node: cElementTree.Element
    @param dic: 存储信息的字典
    @type dic: dict
    @return: 转换好的字典
    @rtype: dict
    '''
    dic[node.tag] = ndic = {}
    [xml2dict(child, ndic) for child in node.getchildren() if child != node]
    ndic.update([map(str.strip, exp.split('=')[:2])
                 for exp in node.text.splitlines() if '=' in exp])
    return dic


def configParse(filename):
    '''
    @brief: 解析tars配置文件
    @param filename: 文件名
    @type filename: str
    @return: 解析出来的配置信息
    @rtype: dict
    '''
    tree = ET.parse(filename)
    return xml2dict(tree.getroot())


class NewLock(object):
    def __init__(self):
        self.__count = 0
        self.__lock = Lock()
        self.__lockForCount = Lock()
        pass

    def newAcquire(self):
        self.__lockForCount.acquire()
        self.__count += 1
        if self.__count == 1:
            self.__lock.acquire()
        self.__lockForCount.release()
        pass

    def newRelease(self):
        self.__lockForCount.acquire()
        self.__count -= 1
        if self.__count == 0:
            self.__lock.release()
        self.__lockForCount.release()


class LockGuard(object):
    def __init__(self, newLock):
        self.__newLock = newLock
        self.__newLock.newAcquire()

    def __del__(self):
        self.__newLock.newRelease()


class ConsistentHashNew(object):
    def __init__(self, nodes=None, nodeNumber=3):
        """
        :param nodes:           服务器的节点的epstr列表
        :param n_number:        一个节点对应的虚拟节点数量
        :return:
        """
        self.__nodes = nodes
        self.__nodeNumber = nodeNumber  # 每一个节点对应多少个虚拟节点，这里默认是3个
        self.__nodeDict = dict()  # 用于记录虚拟节点的hash值与服务器epstr的对应关系
        self.__sortListForKey = []  # 用于存放所有的虚拟节点的hash值，这里需要保持排序，以找出对应的服务器
        if nodes:
            for node in nodes:
                self.addNode(node)

    @property
    def nodes(self):
        return self.__nodes

    @nodes.setter
    def nodes(self, value):
        self.__nodes = value

    def addNode(self, node):
        """
        添加node，首先要根据虚拟节点的数目，创建所有的虚拟节点，并将其与对应的node对应起来
        当然还需要将虚拟节点的hash值放到排序的里面
        这里在添加了节点之后，需要保持虚拟节点hash值的顺序
        :param node:
        :return:
        """
        for i in xrange(self.__nodeNumber):
            nodeStr = "%s%s" % (node, i)
            key = self.__genKey(nodeStr)
            self.__nodeDict[key] = node
            self.__sortListForKey.append(key)
        self.__sortListForKey.sort()

    def removeNode(self, node):
        """
        这里一个节点的退出，需要将这个节点的所有的虚拟节点都删除
        :param node:
        :return:
        """
        for i in xrange(self.__nodeNumber):
            nodeStr = "%s%s" % (node, i)
            key = self.__genKey(nodeStr)
            del self.__nodeDict[key]
            self.__sortListForKey.remove(key)

    def getNode(self, key):
        """
        返回这个字符串应该对应的node，这里先求出字符串的hash值，然后找到第一个小于等于的虚拟节点，然后返回node
        如果hash值大于所有的节点，那么用第一个虚拟节点
        :param : hashNum or keyStr
        :return:
        """
        keyStr = ''
        if isinstance(key, int):
            keyStr = "the keyStr is %d" % key
        elif isinstance(key, type('a')):
            keyStr = key
        else:
            raise TarsException("the hash code has wrong type")
        if self.__sortListForKey:
            key = self.__genKey(keyStr)
            for keyItem in self.__sortListForKey:
                if key <= keyItem:
                    return self.__nodeDict[keyItem]
            return self.__nodeDict[self.__sortListForKey[0]]
        else:
            return None

    def __genKey(self, keyStr):
        """
        通过key，返回当前key的hash值，这里采用md5
        :param key:
        :return:
        """
        md5Str = hashlib.md5(keyStr).hexdigest()
        return long(md5Str, 16)
