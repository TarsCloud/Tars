/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
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

#include "util/tc_loki.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace tars;

class TestMember
{
public:
    void mem();
};

void testTypeTraits()
{
    bool b;
    b = TL::TypeTraits<vector<int>::iterator>::isPointer;
    cout << "vector<int>::iterator is " << b << endl;

    b = TL::TypeTraits<char*>::isPointer;
    cout << "char* is " << b << endl;

    b = TL::TypeTraits<char&>::isReference;
    cout << "char& is " << b << endl;
/*
    typedef void (TestMember::*TpMem)();
    b = TL::TypeTraits<TpMem>::isMemberPointer;
    cout << "(TestMember::*TpMem)() is " << b << endl;
    b = TL::TypeTraits<int>::isMemberPointer;
    cout << "int is " << b << endl;
*/
    b = TL::TypeTraits<const int>::isConst;
    cout << "const int is " << b << endl;

    b = TL::TypeTraits<int>::isConst;
    cout << "int is " << b << endl;

    b = TL::TypeTraits<volatile int>::isVolatile;
    cout << "volatile int is " << b << endl;

    b = TL::TypeTraits<int>::isVolatile;
    cout << "int is " << b << endl;

}

void testTypeSelect()
{
    cout << "int|char:" << sizeof(TL::TypeSelect<true, int, char>::Result) << endl;
}

struct A { };
struct B : public A { };

void testConversion()
{
//    cout << "conversion double to int:" << TL::Conversion<double, int>::exists << endl;
    cout << "conversion char to char*:" << TL::Conversion<char, char*>::exists << endl;
    cout << "conversion size_t to vector<size_t>:" << TL::Conversion<size_t, vector<size_t> >::exists << endl;

    cout << "conversion B to A:" << TL::Conversion<B, A>::exists << endl;
    cout << "conversion A to B:" << TL::Conversion<A, B>::exists << endl;

    cout << "SUPERSUBCLASS(B, A):" << SUPERSUBCLASS(B, A) << endl;
    cout << "SUPERSUBCLASS(A, B):" << SUPERSUBCLASS(A, B) << endl;
    cout << "SUPERSUBCLASS_STRICT(A, B):" << SUPERSUBCLASS_STRICT(A, B) << endl;
    cout << "SUPERSUBCLASS_STRICT(B, B):" << SUPERSUBCLASS_STRICT(B, B) << endl;
}

template<typename T>
struct Holder
{
    T _value;
};

void testScatterHierarchy()
{
    typedef TL::ScatterHierarchy<TL::TLMaker<int, char, string>::Result, Holder> TestHolder;
    cout << "size:" << sizeof(TestHolder) << endl;
    cout << "size:" << sizeof(Holder<string>) << endl;

    TestHolder th;

    //按照名字访问
    TL::field<int>(th)._value = 4;
    cout << TL::field<int>(th)._value << endl;
    TL::field<char>(th)._value = 'c';
    cout << TL::field<char>(th)._value << endl;
    TL::field<string>(th)._value = "abc";
    cout << TL::field<string>(th)._value << endl;

    //按索引访问
    TL::field<0>(th)._value = 5;
    cout << TL::field<0>(th)._value << endl;

    TL::field<1>(th)._value = 'a';
    cout << TL::field<1>(th)._value << endl;

    TL::field<2>(th)._value = "abcd";
    cout << TL::field<2>(th)._value << endl;

    const TestHolder cth = th;
    cout << TL::field<int>(cth)._value << endl;
    //下面这句应该编译不过
//    TL::field<int>(cth)._value = 7;

    //采用Tuple类描述
    typedef TL::Tuple<TL::TLMaker<int, string>::Result> TestTuple;
    TestTuple tt;

    TL::field<0>(tt) = 6;
    TL::field<1>(tt) = "abcde";
    cout << TL::field<0>(tt) << endl;
    cout << TL::field<1>(tt) << endl;

    const TestTuple ctt = tt;
    cout << TL::field<0>(ctt) << endl;
    //下面这句应该编译不过
//    TL::field<0>(ctt) = 6;
}

int main(int argc, char *argv[])
{
    try
    {
        testTypeTraits();
        testTypeSelect();
        testConversion();
        testScatterHierarchy();
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


