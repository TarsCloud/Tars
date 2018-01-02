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


import sys;
class util:
    @staticmethod
    def printHex(buff):
        count = 0;
        for c in buff:
            sys.stdout.write("0X%02X " % ord(c));
            count += 1;
            if count % 16 == 0:
                sys.stdout.write("\n");
        sys.stdout.write("\n");        
        sys.stdout.flush();

    @staticmethod
    def mapclass(ktype, vtype):
        class mapklass(dict): 
            def size(self): return len(self);
        setattr(mapklass, '__tars_index__', 8);
        setattr(mapklass, '__tars_class__', "map<" + ktype.__tars_class__ + "," + vtype.__tars_class__ + ">");
        setattr(mapklass, 'ktype', ktype);
        setattr(mapklass, 'vtype', vtype);
        return mapklass;

    @staticmethod
    def vectorclass(vtype):
        class klass(list):
            def size(self): return len(self);
        setattr(klass, '__tars_index__', 9);
        setattr(klass, '__tars_class__', "list<" + vtype.__tars_class__ + ">");
        setattr(klass, 'vtype', vtype);
        return klass;
    
    class boolean   : __tars_index__ = 999;  __tars_class__ = "bool";
    class int8      : __tars_index__ = 0;    __tars_class__ = "char";
    class uint8     : __tars_index__ = 1;    __tars_class__ = "short";
    class int16     : __tars_index__ = 1;    __tars_class__ = "short";
    class uint16    : __tars_index__ = 2;    __tars_class__ = "int32";
    class int32     : __tars_index__ = 2;    __tars_class__ = "int32";
    class uint32    : __tars_index__ = 3;    __tars_class__ = "int64";
    class int64     : __tars_index__ = 3;    __tars_class__ = "int64";
    class float     : __tars_index__ = 4;    __tars_class__ = "float";
    class double    : __tars_index__ = 5;    __tars_class__ = "double";
    class bytes     : __tars_index__ = 13;   __tars_class__ = "list<char>";
    class string    : __tars_index__ = 67;   __tars_class__ = "string";
    class struct    : __tars_index__ = 1011; 


from xml.etree import cElementTree as ET

def xml2dict(node, dic = {}):
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

