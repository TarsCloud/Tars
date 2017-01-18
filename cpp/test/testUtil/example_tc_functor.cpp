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
#include "util/tc_functor.h"

#include <iostream>
#include <vector>
#include <cassert>

using namespace std;
using namespace tars;

////////////////////////////////////////////////////////////////////
// C风格的函数:同一个名称的函数只能有一个,不能重载
void TestFunction1()
{
    cout << "TestFunction1()" << endl;
}

void TestFunction1(char c)
{
    cout << "TestFunction1(" << c << ")" << endl;
}

void TestFunction2(char c)
{
    cout << "TestFunction2('" << c << "')" << endl;
}

void TestFunction3(const string &s, int i)
{
    cout << "TestFunction3('" << s << "', '" << i << "')" << endl;
}

void TestFunction4(string &s, int i)
{
    cout << "TestFunction4('" << s << "', '" << i << "')" << endl;
    s = "abc";
}

void TestFunction5(string s)
{
    cout << "TestFunction5('" << s << "')" << endl;
}

struct A
{
    A()
    {
        s = "A";
        cout << "A()" << endl;
    }

    A(const A &a)
    {
        s = a.s;
        cout << "A(A)" << endl;
    }
    string s;
};

void TestFunction6(A s)
{
    cout << "TestFunction6('" << s.s << "')" << endl;
}

void TestFunction7(A& s)
{
    cout << "TestFunction7('" << s.s << "')" << endl;
}

void TestFunction8(A *pa)
{
    cout << "TestFunction8('" << pa->s << "')" << endl;
}

//封装调用示例代码
void testFunction()
{
    //////////////////////////////////////////////////////////////////////////////
    typedef void (*TpFunc)();
    TC_Functor<void> cmd1(static_cast<TpFunc>(TestFunction1));
    cmd1();

    //封装成wrapper
    TC_Functor<void>::wrapper_type fwrapper1(cmd1);
    fwrapper1();
    TC_Functor<void>::wrapper_type fwrapper1c(fwrapper1);
    fwrapper1c();

    typedef void (*TpFunc1)(char);
    TC_Functor<void, TL::TLMaker<char>::Result> cmd11(static_cast<TpFunc1>(TestFunction1));
    cmd11('c');
    TC_Functor<void, TL::TLMaker<char>::Result>::wrapper_type fwrapper11(cmd11, 'c');
    fwrapper11();

    //////////////////////////////////////////////////////////////////////////////
    TC_Functor<void, TL::TLMaker<char>::Result> cmd2(TestFunction2);
    cmd2('c');
    TC_Functor<void, TL::TLMaker<char>::Result>::wrapper_type fwrapper2(cmd2, 'c');
    fwrapper2();

    //////////////////////////////////////////////////////////////////////////////
    TC_Functor<void, TL::TLMaker<const string&, int>::Result> cmd3(TestFunction3);
    string s3("s3");
    cmd3(s3, 10);
    TC_Functor<void, TL::TLMaker<const string&, int>::Result>::wrapper_type fwrapper3(cmd3, s3, 10);
    fwrapper3();

    //////////////////////////////////////////////////////////////////////////////
    TC_Functor<void, TL::TLMaker<string&, int>::Result> cmd4(TestFunction4);
    string s4("s4");
    cmd4(s4, 10);
    assert(s4 == "abc");
    cout << s4 << endl;
    s4 = "s";
    TC_Functor<void, TL::TLMaker<string&, int>::Result>::wrapper_type fwrapper4(cmd4, s4, 10);
    fwrapper4();
    cout << s4 << endl;
    TC_Functor<void, TL::TLMaker<string&, int>::Result>::wrapper_type fwrapper44(fwrapper4);
    fwrapper44();

    //////////////////////////////////////////////////////////////////////////////
    TC_Functor<void, TL::TLMaker<string>::Result> cmd5(TestFunction5);
    string s5("s5");
    cmd5(s5);
    TC_Functor<void, TL::TLMaker<string>::Result>::wrapper_type fwrapper5(cmd5, s5);
    fwrapper5();

    //////////////////////////////////////////////////////////////////////////////
    TC_Functor<void, TL::TLMaker<A>::Result> cmd6(TestFunction6);
    A a;
    cmd6(a);
    TC_Functor<void, TL::TLMaker<A>::Result>::wrapper_type fwrapper6(cmd6, a);
    fwrapper6();

    //////////////////////////////////////////////////////////////////////////////
    TC_Functor<void, TL::TLMaker<A&>::Result> cmd7(TestFunction7);
    cmd7(a);

    //////////////////////////////////////////////////////////////////////////////
    TC_Functor<void, TL::TLMaker<A*>::Result> cmd8(TestFunction8);
    cmd8(&a);
}

///////////////////////////////////////////////////////////////////////////////////////
//测试仿函数

struct TestFunctor
{
    void operator()()
    {
        cout << "TestFunctor::operator() called" << endl;
    }

    void operator()(char c)
    {
        cout << "TestFunctor::operator('" << c << "') called" << endl;
    }

    void operator()(const string &s, int i)
    {
        cout << "TestFunctor::operator(" << s << "," << i << ") called" << endl;
    }

    void operator()(string &s, int i)
    {
        cout << "TestFunctor::operator(" << s << "," << i << ") called" << endl;
        s = "abc";
    }

    void operator()(string s)
    {
        cout << "TestFunctor::operator(" << s << ") called" << endl;
    }

    void operator()(A s)
    {
        cout << "TestFunctor::operator(" << s.s << ") called" << endl;
    }
};

void testFunctor()
{
    TestFunctor f;

    TC_Functor<void> cmd1(f);
    TC_Functor<void, TL::TLMaker<char>::Result> cmd2(f);
    TC_Functor<void, TL::TLMaker<const string&, int>::Result> cmd3(f);
    TC_Functor<void, TL::TLMaker<string&, int>::Result> cmd4(f);
    TC_Functor<void, TL::TLMaker<string, int>::Result> cmd5(f);
    TC_Functor<void, TL::TLMaker<A>::Result> cmd6(f);

    cmd1();
    cmd2('c');
    cmd3("s3", 5);
    string s4("s4");
    cmd4(s4, 5);
    cout << s4 << endl;
    cmd5(s4, 10);
    A a;
    cmd6(a);

    TC_Functor<void>::wrapper_type fwrapper1(cmd1);
    TC_Functor<void, TL::TLMaker<char>::Result>::wrapper_type fwrapper2(cmd2, 'c');
    TC_Functor<void, TL::TLMaker<const string&, int>::Result>::wrapper_type fwrapper3(cmd3, s4, 3);
    TC_Functor<void, TL::TLMaker<string&, int>::Result>::wrapper_type fwrapper4(cmd4, s4, 333);
    TC_Functor<void, TL::TLMaker<string, int>::Result>::wrapper_type fwrapper5(cmd5, s4, 22);
    TC_Functor<void, TL::TLMaker<A>::Result>::wrapper_type fwrapper6(cmd6,a);

    fwrapper1();
    fwrapper2();
    fwrapper3();
    fwrapper4();
    fwrapper5();
    fwrapper6();
}

///////////////////////////////////////////////////////////////////////////////////
//测试类成员函数指针
class TestMember
{
public:
    void mem1()
    {
        cout << "TestMember::mem1() called" << endl;
    }

    void mem1(char c)
    {
        cout << "TestMember::mem1('" << c << "') called" << endl;
    }

    void mem2(char c)
    {
        cout << "TestMember::mem2('" << c << "') called" << endl;
    }

    void mem3(const string &s, int i)
    {
        cout << "TestMember::mem3(" << s << "," << i << ") called" << endl;
    }

    void mem4(string &s, int i)
    {
        cout << "TestMember::mem4(" << s << "," << i << ") called" << endl;
        s = "abc";
    }

    void mem5(string s)
    {
        cout << "TestMember::mem5(" << s << ") called" << endl;
    }

    void mem6(A s)
    {
        cout << "TestMember::mem6(" << s.s << ") called" << endl;
    }

public:
    int i;
};

void testMemFunction()
{
    TestMember tm;
    TestMember *p = &tm;

    //定义成员函数指针类型
    typedef void (TestMember::*TpMem)();
    //定义指向成员函数的指针
    TpMem ptm = static_cast<TpMem>(&TestMember::mem1);

    //对象的调用方式
    (tm.*ptm)();
    //指针的调用方式
    (p->*ptm)();

    //封装成functor的函数调用
    TC_Functor<void> cmd1(&tm, static_cast<TpMem>(&TestMember::mem1));
    cmd1();
    TC_Functor<void>::wrapper_type fwrapper1(cmd1);
    fwrapper1();

    typedef void (TestMember::*TpMem1)(char);
    TC_Functor<void, TL::TLMaker<char>::Result> cmd11(&tm, static_cast<TpMem1>(&TestMember::mem1));
    cmd11('c');
    TC_Functor<void, TL::TLMaker<char>::Result>::wrapper_type fwrapper11(cmd11, 'c');
    fwrapper11();

    TC_Functor<void, TL::TLMaker<char>::Result> cmd2(&tm, &TestMember::mem2);
    cmd2('c');
    TC_Functor<void, TL::TLMaker<char>::Result>::wrapper_type fwrapper2(cmd2, 'c');
    fwrapper2();

    string ta ="a";
    TC_Functor<void, TL::TLMaker<const string&, int>::Result> cmd3(&tm, &TestMember::mem3);
    cmd3(ta, 33);
    TC_Functor<void, TL::TLMaker<const string&, int>::Result>::wrapper_type fwrapper3(cmd3, ta, 33);
    fwrapper3();

    TC_Functor<void, TL::TLMaker<string&, int>::Result> cmd4(&tm, &TestMember::mem4);
    string s= "s";
    cmd4(s, 33);
    cout << s << endl;
    TC_Functor<void, TL::TLMaker<string&, int>::Result>::wrapper_type fwrapper4(cmd4, s, 33);
    fwrapper4();

    TC_Functor<void, TL::TLMaker<string>::Result> cmd5(&tm, &TestMember::mem5);
    cmd5(s);
    TC_Functor<void, TL::TLMaker<string>::Result>::wrapper_type fwrapper5(cmd5, s);
    fwrapper5();

    TC_Functor<void, TL::TLMaker<A>::Result> cmd6(&tm, &TestMember::mem6);
    A a;
    cmd6(a);
    TC_Functor<void, TL::TLMaker<A>::Result>::wrapper_type fwrapper6(cmd6, a);
    fwrapper6();
}

int main(int argc, char *argv[])
{
    try
    {
        cout << "*************testFunction*************" << endl;
        testFunction();
        cout << "*************testFunctor*************" << endl;
        testFunctor();
        cout << "*************testMemFunction*************" << endl;
        testMemFunction();
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


