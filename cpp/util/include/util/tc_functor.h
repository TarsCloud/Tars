#ifndef __TC_FUNCTOR_H
#define __TC_FUNCTOR_H

#include <memory>
#include <iostream>
#include "util/tc_loki.h"

using namespace std;

namespace tars
{
///////////////////////////////////////////////////////////////////////////////////////////////////
/** 
 * @file tc_functor.h
 * @brief 通用仿函数类
 
 * 参考C++设计新思维, loki库的设计 
 * Functor对调用的封装,可以对以下几种调用封装:
 * C风格的函数调用
 * C风格的函数指针
 * C++风格的函数对象
 * 指向类成员函数的指针调用
 * 封装以后的调用形式可以变成两种:
 * A 仿函数对象调用方式, 即对上述的几种方式都可以在右侧添加一对圆括号,并在括号内部放一组合适的参数来调用
 * B 把整个调用过程封装一个函数对象, 随时调用, 调用的时候不用传入参数, 而在在对象建立时就传入了参数
 * C函数调用:
 *  void TestFunction3(const string &s, int i)
 *
 *  {
 *
 *       cout << "TestFunction3('" << s << "', '" << i << "')" << endl;
 *
 *  }
 *
 * TC_Functor<void, TL::TLMaker<const string&, int>::Result> cmd3(TestFunction3);
 * string s3("s3");
 * cmd3(s3, 10);
 * C函数调用用TC_FunctorWrapper封装:*
 *  TC_FunctorWrapper<TC_Functor<void, TL::TLMaker<const string&, int>::Result> > fwrapper3(cmd3, s3, 10);
 *  fwrapper3();
 * 说明:
 * void : 函数的返回值
 * C++函数对象调用:
 *  struct TestFunctor
 *  {
 *
 *      void operator()(const string &s, int i)
 *
 *      {
 *
 *          cout << "TestFunctor::operator(" << s << "," << i << ") called" << endl;
 *
 *      }
 *
 *  }
 *
 *  TestFunctor f;
 *
 *  TC_Functor<void, TL::TLMaker<const string&, int>::Result> cmd3(f);
 *
 *  string s3("s3");
 *
 *  cmd3(s3, 10);
 *
 * C++函数对象调用用TC_FunctorWrapper封装:
 *
 *  TC_FunctorWrapper<TC_Functor<void, TL::TLMaker<const string&, int>::Result> > fwrapper3(cmd3, s3, 10);
 *
 *  fwrapper3();
 *
 * 指向类成员函数的调用
 *  struct TestMember
 * {
 *     void mem3(const string &s, int i)
 *     {
 *          cout << "TestMember::mem3(" << s << "," << i << ") called" << endl;
 *     }
 *  }
 *  TC_Functor<void, TL::TLMaker<const string&, int>::Result> cmd3(&tm, &TestMember::mem3);
 * cmd3("a", 33);
 * 指向类成员函数的调用用TC_FunctorWrapper封装:
 *  TC_FunctorWrapper<TC_Functor<void, TL::TLMaker<const string&, int>::Result> > fwrapper3(cmd3, "a", 10);
 *  fwrapper3();
 * 注意所有的TC_FunctorWrapper的调用, 在具体调用的时候是不带参数的, 参数在构造的时候已经传进去了
 ///////////////////////////////////////////////////////////////////////////////////////////////////
 *
 * 对于C风格的函数调用, 同名称的函数通常只有一个, 如果有多个相同名称的函数
 *不同参数类型的函数, 则在构造TC_Functor的时候需要强制类型转换, 如下:
 *  void TestFunction1()
 *  {
 *      cout << "TestFunction1()" << endl;
 * }
 *  void TestFunction1(char c)
 *  {
 *      cout << "TestFunction1(" << c << ")" << endl;
 * }
 * typedef void (*TpFunc)();
 * TC_Functor<void> cmd1(static_cast<TpFunc>(TestFunction1));
 * cmd1();
 * typedef void (*TpFunc1)(char);
 * TC_Functor<void, TL::TLMaker<char>::Result>
 * cmd11(static_cast<TpFunc1>(TestFunction1)); cmd11('c');
 ///////////////////////////////////////////////////////////////////////////////////////////////////
 *
 * 对于类成员函数的指针调用, 同名称的函数通常只有一个, 如果有多个相同名称的函数
 * 不同参数类型的函数, 则在构造TC_Functor的时候需要强制类型转换, 如下:
 *  class TestMember
 *  {
 *  public:
 *      void mem1()
 *      {
 *          cout << "TestMember::mem1() called" << endl;
 *      }
 *      void mem1(char c)
 *      {
 *          cout << "TestMember::mem1('" << c << "') called" << endl;
 *      }
 *  };
 
 *  TestMember tm;
 
 *  typedef void (TestMember::*TpMem)();
 
 *  typedef void (TestMember::*TpMem1)(char);
 
 *  TC_Functor<void> cmd1(&tm, static_cast<TpMem>(&TestMember::mem1));
 *  cmd1();
 *
 *  typedef void (TestMember::*TpMem1)(char);
 
 * TC_Functor<void, TL::TLMaker<char>::Result> cmd11(&tm,
 
 * static_cast<TpMem1>(&TestMember::mem1)); cmd11('c');
 *
 * 具体示例请参见demo/test_tc_functor.cpp.
 */

template<typename R, class TList>
class TC_FunctorImp
{
public:
    virtual R operator()() = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R>
class TC_FunctorImp<R, TL::EmptyType>
{
public:
    virtual R operator()() = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R, typename P1>
class TC_FunctorImp<R, TL::TYPELIST_1(P1)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1) = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R, typename P1, typename P2>
class TC_FunctorImp<R, TL::TYPELIST_2(P1, P2)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2) = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3>
class TC_FunctorImp<R, TL::TYPELIST_3(P1, P2, P3)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3) = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4>
class TC_FunctorImp<R, TL::TYPELIST_4(P1, P2, P3, P4)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4) = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
class TC_FunctorImp<R, TL::TYPELIST_5(P1, P2, P3, P4, P5)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4,
                         typename TL::TypeTraits<P5>::ReferenceType p5) = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
class TC_FunctorImp<R, TL::TYPELIST_6(P1, P2, P3, P4, P5, P6)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4,
                         typename TL::TypeTraits<P5>::ReferenceType p5,
                         typename TL::TypeTraits<P6>::ReferenceType p6) = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
class TC_FunctorImp<R, TL::TYPELIST_7(P1, P2, P3, P4, P5, P6, P7)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4,
                         typename TL::TypeTraits<P5>::ReferenceType p5,
                         typename TL::TypeTraits<P6>::ReferenceType p6,
                         typename TL::TypeTraits<P7>::ReferenceType p7) = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
class TC_FunctorImp<R, TL::TYPELIST_8(P1, P2, P3, P4, P5, P6, P7, P8)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4,
                         typename TL::TypeTraits<P5>::ReferenceType p5,
                         typename TL::TypeTraits<P6>::ReferenceType p6,
                         typename TL::TypeTraits<P7>::ReferenceType p7,
                         typename TL::TypeTraits<P8>::ReferenceType p8) = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
class TC_FunctorImp<R, TL::TYPELIST_9(P1, P2, P3, P4, P5, P6, P7, P8, P9)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4,
                         typename TL::TypeTraits<P5>::ReferenceType p5,
                         typename TL::TypeTraits<P6>::ReferenceType p6,
                         typename TL::TypeTraits<P7>::ReferenceType p7,
                         typename TL::TypeTraits<P8>::ReferenceType p8,
                         typename TL::TypeTraits<P9>::ReferenceType p9) = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
class TC_FunctorImp<R, TL::TYPELIST_10(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4,
                         typename TL::TypeTraits<P5>::ReferenceType p5,
                         typename TL::TypeTraits<P6>::ReferenceType p6,
                         typename TL::TypeTraits<P7>::ReferenceType p7,
                         typename TL::TypeTraits<P8>::ReferenceType p8,
                         typename TL::TypeTraits<P9>::ReferenceType p9,
                         typename TL::TypeTraits<P10>::ReferenceType p10) = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

/**
 * 封装对函数对象以及函数的调用
 */
template<class ParentFunctor, typename Fun>
class TC_FunctorHandler : public TC_FunctorImp<typename ParentFunctor::ResultType, typename ParentFunctor::ParamList>
{
public:
    typedef typename ParentFunctor::ResultType ResultType;

    TC_FunctorHandler(Fun fun) : _fun(fun)
    {

    }

    TC_FunctorHandler *clone() const
    {
        return new TC_FunctorHandler(*this);
    }

    ResultType operator()()
    { return _fun(); }

    ResultType operator()(typename ParentFunctor::Reference1 p1)
    { return _fun(p1); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2)
    { return _fun(p1, p2); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3)
    { return _fun(p1, p2, p3); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4)
    { return _fun(p1, p2, p3, p4); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4,
                        typename ParentFunctor::Reference5 p5)
    { return _fun(p1, p2, p3, p4, p5); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4,
                        typename ParentFunctor::Reference5 p5,
                        typename ParentFunctor::Reference6 p6)
    { return _fun(p1, p2, p3, p4, p5, p6); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4,
                        typename ParentFunctor::Reference5 p5,
                        typename ParentFunctor::Reference6 p6,
                        typename ParentFunctor::Reference7 p7)
    { return _fun(p1, p2, p3, p4, p5, p6, p7); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4,
                        typename ParentFunctor::Reference5 p5,
                        typename ParentFunctor::Reference6 p6,
                        typename ParentFunctor::Reference7 p7,
                        typename ParentFunctor::Reference8 p8)
    { return _fun(p1, p2, p3, p4, p5, p6, p7, p8); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4,
                        typename ParentFunctor::Reference5 p5,
                        typename ParentFunctor::Reference6 p6,
                        typename ParentFunctor::Reference7 p7,
                        typename ParentFunctor::Reference8 p8,
                        typename ParentFunctor::Reference9 p9)
    { return _fun(p1, p2, p3, p4, p5, p6, p7, p8, p9); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4,
                        typename ParentFunctor::Reference5 p5,
                        typename ParentFunctor::Reference6 p6,
                        typename ParentFunctor::Reference7 p7,
                        typename ParentFunctor::Reference8 p8,
                        typename ParentFunctor::Reference9 p9,
                        typename ParentFunctor::Reference10 p10)
    { return _fun(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); }

private:
    Fun _fun;
};

template<class ParentFunctor, typename PointerToObj, typename PointerToMemFun>
/**
 * @brief 封装对成员函数的调用
 */
class TC_MemFunHandler
    : public TC_FunctorImp<typename ParentFunctor::ResultType, typename ParentFunctor::ParamList>
{
public:
    typedef typename ParentFunctor::ResultType ResultType;

    TC_MemFunHandler(const PointerToObj &pObj, PointerToMemFun pMemFn)
        : _pObj(pObj), _pMemFn(pMemFn)
    {
    }

    TC_MemFunHandler* clone() const
    {
        return new TC_MemFunHandler(*this);
    }

    ResultType operator()()
    { return (_pObj->*_pMemFn)(); }

    ResultType operator()(typename ParentFunctor::Reference1 p1)
    { return (_pObj->*_pMemFn)(p1); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2)
    { return (_pObj->*_pMemFn)(p1, p2); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3)
    { return (_pObj->*_pMemFn)(p1, p2, p3); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4)
    { return (_pObj->*_pMemFn)(p1, p2, p3, p4); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4,
                        typename ParentFunctor::Reference5 p5)
    { return (_pObj->*_pMemFn)(p1, p2, p3, p4, p5); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4,
                        typename ParentFunctor::Reference5 p5,
                        typename ParentFunctor::Reference6 p6)
    { return (_pObj->*_pMemFn)(p1, p2, p3, p4, p5, p6); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4,
                        typename ParentFunctor::Reference5 p5,
                        typename ParentFunctor::Reference6 p6,
                        typename ParentFunctor::Reference7 p7)
    { return (_pObj->*_pMemFn)(p1, p2, p3, p4, p5, p6, p7); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4,
                        typename ParentFunctor::Reference5 p5,
                        typename ParentFunctor::Reference6 p6,
                        typename ParentFunctor::Reference7 p7,
                        typename ParentFunctor::Reference8 p8)
    { return (_pObj->*_pMemFn)(p1, p2, p3, p4, p5, p6, p7, p8); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4,
                        typename ParentFunctor::Reference5 p5,
                        typename ParentFunctor::Reference6 p6,
                        typename ParentFunctor::Reference7 p7,
                        typename ParentFunctor::Reference8 p8,
                        typename ParentFunctor::Reference9 p9)
    { return (_pObj->*_pMemFn)(p1, p2, p3, p4, p5, p6, p7, p8, p9); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4,
                        typename ParentFunctor::Reference5 p5,
                        typename ParentFunctor::Reference6 p6,
                        typename ParentFunctor::Reference7 p7,
                        typename ParentFunctor::Reference8 p8,
                        typename ParentFunctor::Reference9 p9,
                        typename ParentFunctor::Reference10 p10)
    { return (_pObj->*_pMemFn)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); }

private:
    PointerToObj    _pObj;
    PointerToMemFun _pMemFn;
};

template<class ParentFunctor>
class TC_FunctorWrapper;


template<typename R, class TList = TL::NullType>
/**
 * @brief 函数对象类, 可以封装对: 函数对象, 函数, 成员函数 
 *        的调用
 */
class TC_Functor
{
public:
    typedef R     ResultType;
    typedef TList ParamList;

    /**定义封装类型*/
    typedef TC_FunctorWrapper<TC_Functor<R, TList> > wrapper_type;

    /**定义类型列表中每个参数的原类型*/
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 0, TL::EmptyType>::Result>::ParameterType Param1;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 1, TL::EmptyType>::Result>::ParameterType Param2;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 2, TL::EmptyType>::Result>::ParameterType Param3;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 3, TL::EmptyType>::Result>::ParameterType Param4;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 4, TL::EmptyType>::Result>::ParameterType Param5;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 5, TL::EmptyType>::Result>::ParameterType Param6;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 6, TL::EmptyType>::Result>::ParameterType Param7;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 7, TL::EmptyType>::Result>::ParameterType Param8;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 8, TL::EmptyType>::Result>::ParameterType Param9;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 9, TL::EmptyType>::Result>::ParameterType Param10;

    /**定义类型列表中每个参数的引用类型*/
    typedef typename TL::TypeTraits<Param1>::ReferenceType Reference1;
    typedef typename TL::TypeTraits<Param2>::ReferenceType Reference2;
    typedef typename TL::TypeTraits<Param3>::ReferenceType Reference3;
    typedef typename TL::TypeTraits<Param4>::ReferenceType Reference4;
    typedef typename TL::TypeTraits<Param5>::ReferenceType Reference5;
    typedef typename TL::TypeTraits<Param6>::ReferenceType Reference6;
    typedef typename TL::TypeTraits<Param7>::ReferenceType Reference7;
    typedef typename TL::TypeTraits<Param8>::ReferenceType Reference8;
    typedef typename TL::TypeTraits<Param9>::ReferenceType Reference9;
    typedef typename TL::TypeTraits<Param10>::ReferenceType Reference10;

public:
    TC_Functor()
    {
    }

    TC_Functor(const TC_Functor &functor)
    : _spImpl(functor._spImpl->clone())
    {
    }

    TC_Functor& operator=(const TC_Functor &functor)
    {
        if(this != &functor)
        {
            _spImpl = std::auto_ptr<Impl>(functor._spImpl->clone());
        }

        return (*this);
    }

    template<class Fun>
    TC_Functor(Fun fun)
    : _spImpl(new TC_FunctorHandler<TC_Functor, Fun>(fun))
    {
    }

    template<typename PointerToObj, typename PointerToMemFun>
    TC_Functor(const PointerToObj &pObj, PointerToMemFun pMemFn)
    : _spImpl(new TC_MemFunHandler<TC_Functor, PointerToObj, PointerToMemFun>(pObj, pMemFn))
    {
    }

    // 判断是否已经设置了functor
    // 可以这样使用if(functor)
    operator bool() const
    {
        return _spImpl.get() ? true : false;
    }

    R operator()()
    {
        return (*_spImpl)();
    }

    R operator()(Reference1 p1)
    {
        return (*_spImpl)(p1);
    }

    R operator()(Reference1 p1, Reference2 p2)
    {
        return (*_spImpl)(p1, p2);
    }

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3)
    {
        return (*_spImpl)(p1, p2, p3);
    }

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4)
    {
        return (*_spImpl)(p1, p2, p3, p4);
    }

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4, Reference5 p5)
    {
        return (*_spImpl)(p1, p2, p3, p4, p5);
    }

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4, Reference5 p5, Reference6 p6)
    {
        return (*_spImpl)(p1, p2, p3, p4, p5, p6);
    }

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4, Reference5 p5, Reference6 p6, Reference7 p7)
    {
        return (*_spImpl)(p1, p2, p3, p4, p5, p6, p7);
    }

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4, Reference5 p5, Reference6 p6, Reference7 p7, Reference8 p8)
    {
        return (*_spImpl)(p1, p2, p3, p4, p5, p6, p7, p8);
    }

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4, Reference5 p5, Reference6 p6, Reference7 p7, Reference8 p8, Reference9 p9)
    {
        return (*_spImpl)(p1, p2, p3, p4, p5, p6, p7, p8, p9);
    }

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4, Reference5 p5, Reference6 p6, Reference7 p7, Reference8 p8, Reference9 p9, Reference10 p10)
    {
        return (*_spImpl)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
    }


private:
    template<class ParentFunctor>
    friend class TC_FunctorWrapper;

    typedef TC_FunctorImp<R, TList> Impl;

    std::auto_ptr<Impl>     _spImpl;
};

/**
 * @brief wapper基类
 */
class TC_FunctorWrapperInterface
{
public:
    virtual void operator()() = 0;
    virtual ~TC_FunctorWrapperInterface(){}
};

template<class ParentFunctor>

/**
 * @brief 对TC_Functor进行封装, 可以让TC_Functor做到事后被调用
 */
class TC_FunctorWrapper : public TC_FunctorWrapperInterface
{
public:
    typedef typename ParentFunctor::ResultType R;
    typedef typename ParentFunctor::ParamList  TList;

    /**
     * @brief 拷贝构造
     * @param fw
     */
    TC_FunctorWrapper(const TC_FunctorWrapper &fw)
    : _spImpl(fw._spImpl->clone())
    , _p1(fw._p1)
    , _p2(fw._p2)
    , _p3(fw._p3)
    , _p4(fw._p4)
    , _p5(fw._p5)
    , _p6(fw._p6)
    , _p7(fw._p7)
    , _p8(fw._p8)
    , _p9(fw._p9)
    , _p10(fw._p10)
    {
    }

    /**
     * @brief 构造函数
     * @param tf
     */
    TC_FunctorWrapper(ParentFunctor &tf)
    : _spImpl(tf._spImpl->clone())
    {
    }

    /**
     * @brief 一个参数调用的构造函数
     * @param tf
     * @param p1
     */
    TC_FunctorWrapper(ParentFunctor &tf,
                  typename ParentFunctor::Reference1 p1)
    : _spImpl(tf._spImpl->clone()), _p1(p1)
    {
    }

    /**
     * @brief 两个参数调用的构造函数
     * @param tf
     * @param p1
     * @param p2
     */
    TC_FunctorWrapper(ParentFunctor &tf,
                  typename ParentFunctor::Reference1 p1,
                  typename ParentFunctor::Reference2 p2)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2)
    {
    }

    /**
     * @brief 三个参数调用的构造函数
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     */
    TC_FunctorWrapper(ParentFunctor &tf,
                  typename ParentFunctor::Reference1 p1,
                  typename ParentFunctor::Reference2 p2,
                  typename ParentFunctor::Reference3 p3)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3)
    {
    }

    /**
     * @brief 四个参数调用的构造函数
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     */
    TC_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4)
    {
    }

    /**
     * @brief 五个参数调用的构造函数
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     */
    TC_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4,
                                            typename ParentFunctor::Reference5 p5)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4), _p5(p5)
    {
    }

    /**
     * @brief 六个参数调用的构造函数
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     */
    TC_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4,
                                            typename ParentFunctor::Reference5 p5,
                                            typename ParentFunctor::Reference6 p6)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4), _p5(p5), _p6(p6)
    {
    }

    /**
     * @brief 七个参数调用的构造函数
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     * @param p7
     */
    TC_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4,
                                            typename ParentFunctor::Reference5 p5,
                                            typename ParentFunctor::Reference6 p6,
                                            typename ParentFunctor::Reference7 p7)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4), _p5(p5), _p6(p6), _p7(p7)
    {
    }

    /**
     * @brief 八个参数调用的构造函数
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     * @param p7
     * @param p8
     */
    TC_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4,
                                            typename ParentFunctor::Reference5 p5,
                                            typename ParentFunctor::Reference6 p6,
                                            typename ParentFunctor::Reference7 p7,
                                            typename ParentFunctor::Reference8 p8)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4), _p5(p5), _p6(p6), _p7(p7), _p8(p8)
    {
    }

    /**
     * @brief 九个参数调用的构造函数
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     * @param p7
     * @param p8
     * @param p9
     */
    TC_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4,
                                            typename ParentFunctor::Reference5 p5,
                                            typename ParentFunctor::Reference6 p6,
                                            typename ParentFunctor::Reference7 p7,
                                            typename ParentFunctor::Reference8 p8,
                                            typename ParentFunctor::Reference9 p9)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4), _p5(p5), _p6(p6), _p7(p7), _p8(p8), _p9(p9)
    {
    }

    /**
     * @brief 十个参数调用的构造函数
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     * @param p7
     * @param p8
     * @param p9
     * @param p10
     */
    TC_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4,
                                            typename ParentFunctor::Reference5 p5,
                                            typename ParentFunctor::Reference6 p6,
                                            typename ParentFunctor::Reference7 p7,
                                            typename ParentFunctor::Reference8 p8,
                                            typename ParentFunctor::Reference9 p9,
                                            typename ParentFunctor::Reference10 p10)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4), _p5(p5), _p6(p6), _p7(p7), _p8(p8), _p9(p9), _p10(p10)
    {
    }

    /**
     * @brief 具体调用
     * @param tf
     * @param p1
     */
    virtual R operator()()
    {
        return todo(TL::Int2Type<TL::Length<TList>::value>());
    }

    virtual ~TC_FunctorWrapper(){ }

protected:
    /**
     * @brief 赋值函数不定义, 只声明
     * @param fw
     *
     * @return TC_FunctorWrapper&
     */
    TC_FunctorWrapper& operator=(const TC_FunctorWrapper &fw);

    R todo(TL::Int2Type<0>)
    {
        return (*_spImpl)();
    }

    R todo(TL::Int2Type<1>)
    {
        return (*_spImpl)(_p1);
    }

    R todo(TL::Int2Type<2>)
    {
        return (*_spImpl)(_p1, _p2);
    }

    R todo(TL::Int2Type<3>)
    {
        return (*_spImpl)(_p1, _p2, _p3);
    }

    R todo(TL::Int2Type<4>)
    {
        return (*_spImpl)(_p1, _p2, _p3, _p4);
    }

    R todo(TL::Int2Type<5>)
    {
        return (*_spImpl)(_p1, _p2, _p3, _p4, _p5);
    }

    R todo(TL::Int2Type<6>)
    {
        return (*_spImpl)(_p1, _p2, _p3, _p4, _p5, _p6);
    }

    R todo(TL::Int2Type<7>)
    {
        return (*_spImpl)(_p1, _p2, _p3, _p4, _p5, _p6, _p7);
    }

    R todo(TL::Int2Type<8>)
    {
        return (*_spImpl)(_p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8);
    }

    R todo(TL::Int2Type<9>)
    {
        return (*_spImpl)(_p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8, _p9);
    }

    R todo(TL::Int2Type<10>)
    {
        return (*_spImpl)(_p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8, _p9, _p10);
    }

protected:
    typedef TC_FunctorImp<R, TList> Impl;

    std::auto_ptr<Impl>             _spImpl;

    /*
    typename TL::TypeTraits<typename ParentFunctor::Param1>::ReferenceTypeEx   _p1;
    typename TL::TypeTraits<typename ParentFunctor::Param2>::ReferenceTypeEx   _p2;
    typename TL::TypeTraits<typename ParentFunctor::Param3>::ReferenceTypeEx   _p3;
    typename TL::TypeTraits<typename ParentFunctor::Param4>::ReferenceTypeEx   _p4;
    typename TL::TypeTraits<typename ParentFunctor::Param5>::ReferenceTypeEx   _p5;
    typename TL::TypeTraits<typename ParentFunctor::Param6>::ReferenceTypeEx   _p6;
    typename TL::TypeTraits<typename ParentFunctor::Param7>::ReferenceTypeEx   _p7;
    typename TL::TypeTraits<typename ParentFunctor::Param8>::ReferenceTypeEx   _p8;
    typename TL::TypeTraits<typename ParentFunctor::Param9>::ReferenceTypeEx   _p9;
    typename TL::TypeTraits<typename ParentFunctor::Param10>::ReferenceTypeEx  _p10;
    */

    typename TL::TypeTraits<typename ParentFunctor::Param1>::ParameterType   _p1;
    typename TL::TypeTraits<typename ParentFunctor::Param2>::ParameterType   _p2;
    typename TL::TypeTraits<typename ParentFunctor::Param3>::ParameterType   _p3;
    typename TL::TypeTraits<typename ParentFunctor::Param4>::ParameterType   _p4;
    typename TL::TypeTraits<typename ParentFunctor::Param5>::ParameterType   _p5;
    typename TL::TypeTraits<typename ParentFunctor::Param6>::ParameterType   _p6;
    typename TL::TypeTraits<typename ParentFunctor::Param7>::ParameterType   _p7;
    typename TL::TypeTraits<typename ParentFunctor::Param8>::ParameterType   _p8;
    typename TL::TypeTraits<typename ParentFunctor::Param9>::ParameterType   _p9;
    typename TL::TypeTraits<typename ParentFunctor::Param10>::ParameterType  _p10;
};

}

#endif

