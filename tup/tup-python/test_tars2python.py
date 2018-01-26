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


class Fruit:
    orange = 1;
    apple = 2;

class B(tarscore.struct):
    __tars_class__ = "Test.B";
    
    def __init__(self):
        self.a = 0;
        self.f = 0;
    
    @staticmethod
    def writeTo(oos, value):
        oos.write(tarscore.int32, 1, value.a);
        oos.write(tarscore.float, 2, value.f);
    
    @staticmethod
    def readFrom(ios):
        value = B();
        value.a= ios.read(tarscore.int32, 1, True, value.a);
        value.f= ios.read(tarscore.float, 2, True, value.f);
        return value;

class A(tarscore.struct):
    __tars_class__ = "Test.A";
    
    def __init__(self):
        self.a = 0;
        self.b = B();
        self.fruit = 0;
    
    @staticmethod
    def writeTo(oos, value):
        oos.write(tarscore.int32, 1, value.a);
        oos.write(B, 2, value.b);
        oos.write(tarscore.int32, 3, value.fruit);
    
    @staticmethod
    def readFrom(ios):
        value = A();
        value.a= ios.read(tarscore.int32, 1, True, value.a);
        value.b= ios.read(B, 2, True, value.b);
        value.fruit= ios.read(tarscore.int32, 3, True, value.fruit);
        return value;

class TestInfo(tarscore.struct):
    __tars_class__ = "Test.TestInfo";
    vctcls_vi = tarscore.vctclass(tarscore.int32);
    mapcls_mi = tarscore.mapclass(tarscore.int32, tarscore.string);
    
    def __init__(self):
        self.ibegin = 0;
        self.b = True;
        self.si = 0;
        self.by = 0;
        self.ii = 0;
        self.li = 0;
        self.f = 0;
        self.d = 0;
        self.s = "";
        self.vi = TestInfo.vctcls_vi();
        self.mi = TestInfo.mapcls_mi();
        self.aa = A();
        self.iend = 0;
        self.vbc = bytes();
        self.uii = 0;
    
    @staticmethod
    def writeTo(oos, value):
        oos.write(tarscore.int32, 0, value.ibegin);
        oos.write(tarscore.boolean, 1, value.b);
        oos.write(tarscore.int16, 2, value.si);
        oos.write(tarscore.int8, 3, value.by);
        oos.write(tarscore.int32, 4, value.ii);
        oos.write(tarscore.int64, 5, value.li);
        oos.write(tarscore.float, 6, value.f);
        oos.write(tarscore.double, 7, value.d);
        oos.write(tarscore.string, 8, value.s);
        oos.write(value.vctcls_vi, 9, value.vi);
        oos.write(value.mapcls_mi, 10, value.mi);
        oos.write(A, 11, value.aa);
        oos.write(tarscore.int32, 12, value.iend);
        oos.write(tarscore.bytes, 13, value.vbc);
        oos.write(tarscore.uint32, 14, value.uii);
    
    @staticmethod
    def readFrom(ios):
        value = TestInfo();
        value.ibegin= ios.read(tarscore.int32, 0, True, value.ibegin);
        value.b= ios.read(tarscore.boolean, 1, True, value.b);
        value.si= ios.read(tarscore.int16, 2, True, value.si);
        value.by= ios.read(tarscore.int8, 3, True, value.by);
        value.ii= ios.read(tarscore.int32, 4, True, value.ii);
        value.li= ios.read(tarscore.int64, 5, True, value.li);
        value.f= ios.read(tarscore.float, 6, True, value.f);
        value.d= ios.read(tarscore.double, 7, True, value.d);
        value.s= ios.read(tarscore.string, 8, True, value.s);
        value.vi= ios.read(value.vctcls_vi, 9, True, value.vi);
        value.mi= ios.read(value.mapcls_mi, 10, True, value.mi);
        value.aa= ios.read(A, 11, True, value.aa);
        value.iend= ios.read(tarscore.int32, 12, True, value.iend);
        value.vbc= ios.read(tarscore.bytes, 13, True, value.vbc);
        value.uii= ios.read(tarscore.uint32, 14, True, value.uii);
        return value;


if __name__ == "__main__":
    a = A()
    a.a = 1
    a.b.a = 2
    a.b.f = 3.1415926
    a.fruit = Fruit.orange

    jos = tarscore.TarsOutputStream()
    A.writeTo(jos, a)

    jis = tarscore.TarsInputStream(jos.getBuffer())
    a2 = A.readFrom(jis)
    print "Should be 1  2  3.1415926 1"
    print a2.a
    print a2.b.a
    print a2.b.f
    print a2.fruit

    pass
