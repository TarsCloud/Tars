#ifndef __TC_TYPETRAITS_H
#define __TC_TYPETRAITS_H

#include <memory>

namespace tars
{
/////////////////////////////////////////////////
// 说明: loki            
/////////////////////////////////////////////////

namespace TL
{
    //只声明, 不定义的类, 作为TYPELIST的末端类型
    class NullType;

    //空类型
    struct EmptyType { };

    /**
     * 数值到类型的映射
     */
    template<int v>
    struct Int2Type
    {
       enum { value = v };
    };

    /**
     * 类型到类型的映射
     */
    template<typename T>
    struct Type2Type
    {
        typedef T OriginalType;
    };

    ///////////////////////////////////////////////////////////////////////////
    // 以下是TypeList的定义(目前只支持10个参数)
    /**
     * 定义类型链表
    */
    template<typename Head, typename Tail>
    struct TypeList
    {
        typedef Head H;
        typedef Tail T;
    };

    #define TYPELIST_1(T1) TypeList<T1, TL::NullType>
    #define TYPELIST_2(T1, T2) TypeList<T1, TL::TYPELIST_1(T2)>
    #define TYPELIST_3(T1, T2, T3) TypeList<T1, TL::TYPELIST_2(T2, T3)>
    #define TYPELIST_4(T1, T2, T3, T4) TypeList<T1, TL::TYPELIST_3(T2, T3, T4)>
    #define TYPELIST_5(T1, T2, T3, T4, T5) TypeList<T1, TL::TYPELIST_4(T2, T3, T4, T5)>
    #define TYPELIST_6(T1, T2, T3, T4, T5, T6) TypeList<T1, TL::TYPELIST_5(T2, T3, T4, T5, T6)>
    #define TYPELIST_7(T1, T2, T3, T4, T5, T6, T7) TypeList<T1, TL::TYPELIST_6(T2, T3, T4, T5, T6, T7)>
    #define TYPELIST_8(T1, T2, T3, T4, T5, T6, T7, T8) TypeList<T1, TL::TYPELIST_7(T2, T3, T4, T5, T6, T7, T8)>
    #define TYPELIST_9(T1, T2, T3, T4, T5, T6, T7, T8, T9) TypeList<T1, TL::TYPELIST_8(T2, T3, T4, T5, T6, T7, T8, T9)>
    #define TYPELIST_10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) TypeList<T1, TL::TYPELIST_9(T2, T3, T4, T5, T6, T7, T8, T9, T10)>
    #define TYPELIST_11(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) TypeList<T1, TL::TYPELIST_10(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)>
    #define TYPELIST_12(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) TypeList<T1, TL::TYPELIST_11(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)>
    #define TYPELIST_13(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) TypeList<T1, TL::TYPELIST_12(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)>
    #define TYPELIST_14(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14) TypeList<T1, TL::TYPELIST_13(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)>
    #define TYPELIST_15(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15) TypeList<T1, TL::TYPELIST_14(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)>
    #define TYPELIST_16(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16) TypeList<T1, TL::TYPELIST_15(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16)>
    #define TYPELIST_17(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17) TypeList<T1, TL::TYPELIST_16(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17)>
    #define TYPELIST_18(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18) TypeList<T1, TL::TYPELIST_17(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18)>
    #define TYPELIST_19(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19) TypeList<T1, TL::TYPELIST_18(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19)>
    #define TYPELIST_20(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) TypeList<T1, TL::TYPELIST_19(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20)>


    //////////////////////////////////////////////////////////////////////////////
    // 以下定义TypeList的编译期的操作函数(通过偏特化实现)
    /**
     * Length: 取TypeList的长度
     */
    template<class TList> struct Length;
    template<> struct Length<NullType>
    {
        enum { value = 0 };
    };
    template<class Head, class Tail> struct Length<TypeList<Head, Tail> >
    {
        enum { value = 1 + Length<Tail>::value };
    };

    /**
     * TypeAt, 取链表在i位置上的类型
     */
    template<class TList, unsigned int i> struct TypeAt;
    template<class Head, class Tail> struct TypeAt<TypeList<Head, Tail>, 0>
    {
        typedef Head Result;
    };
    template<class Head, class Tail, unsigned int i> struct TypeAt<TypeList<Head, Tail>, i>
    {
        typedef typename TypeAt<Tail, i-1>::Result Result;
    };

    /**
     * TypeAt, 取链表在i位置上的类型, i超出了返回, 则返回DefaultType
     */
    template<class TList, unsigned int index, typename DefaultType = NullType> struct TypeAtNonStrict
    {
        typedef DefaultType Result;
    };
    template <class Head, class Tail, typename DefaultType> struct TypeAtNonStrict<TypeList<Head, Tail>, 0, DefaultType>
    {
        typedef Head Result;
    };
    template <class Head, class Tail, unsigned int i, typename DefaultType> struct TypeAtNonStrict<TypeList<Head, Tail>, i, DefaultType>
    {
        typedef typename TypeAtNonStrict<Tail, i - 1, DefaultType>::Result Result;
    };

    /**
     * 取链表上类型为T的序号, 没有则返回-1
     */
    template<class TList, class T> struct IndexOf;
    template<class T> struct IndexOf<NullType, T>
    {
        enum { value = -1 };
    };
    template<class Tail, class T> struct IndexOf<TypeList<T,Tail>, T>
    {
        enum { value = 0 };
    };
    template<class Head, class Tail, class T> struct IndexOf<TypeList<Head, Tail>, T>
    {
    private:
        enum { temp = IndexOf<Tail, T>::value };
    public:
        enum { value = temp == -1 ? -1 : 1 + temp };
    };

    /**
     * Append, 添加到链表尾部
     */
    template<class TList, class T> struct Append;
    template<> struct Append<NullType, NullType>
    {
        typedef NullType Result;
    };
    template<class T> struct Append<NullType,T>
    {
        typedef TYPELIST_1(T) Result;
    };
    template<class Head, class Tail> struct Append<NullType, TypeList<Head, Tail> >
    {
        typedef TypeList<Head, Tail> Result;
    };
    template<class Head, class Tail, class T> struct Append<TypeList<Head, Tail>, T>
    {
        typedef TypeList<Head, typename Append<Tail, T>::Result> Result;
    };

    /**
     * Erase 删除
     */
    template<class TList, class T> struct Erase;
    template<class T> struct Erase<NullType, T>
    {
        typedef NullType Result;
    };
    template<class T, class Tail> struct Erase<TypeList<T, Tail>, T>
    {
        typedef Tail Result;
    };
    template<class Head, class Tail, class T> struct Erase<TypeList<Head, Tail>, T>
    {
        typedef TypeList<Head, typename Erase<Tail, T>::Result> Result;
    };

    /**
     * EraseAll 删除
     */
    template<class TList, class T> struct EraseAll;
    template<class T> struct EraseAll<NullType, T>
    {
        typedef NullType Result;
    };
    template<class T, class Tail> struct EraseAll<TypeList<T, Tail>, T>
    {
        typedef typename EraseAll<Tail, T>::Result Result;
    };
    template<class Head, class Tail, class T> struct EraseAll<TypeList<Head, Tail>, T>
    {
        typedef TypeList<Head, typename EraseAll<Tail, T>::Result> Result;
    };

    /**
     * 生成TypeList类型
     */
    template<class T1=NullType,  class T2=NullType,  class T3=NullType,  class T4=NullType,  class T5=NullType,
             class T6=NullType,  class T7=NullType,  class T8=NullType,  class T9=NullType,  class T10=NullType,
             class T11=NullType, class T12=NullType, class T13=NullType, class T14=NullType, class T15=NullType,
             class T16=NullType, class T17=NullType, class T18=NullType, class T19=NullType, class T20=NullType>
    struct TLMaker
    {
    private:
        typedef TYPELIST_20(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
                            T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) TmpList;
    public:
        typedef typename EraseAll<TmpList, NullType>::Result Result;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //判断类型T是否可以转换成类型U(参考了wbl库, 直接采用loki, 编译时会有警告
    //关键:如果能够转换, 则接收U的函数, 也能够接收T
    template<class T, class U>
    class Conversion
    {
    protected:
        typedef char YES;
        struct NO {char dummy[2];};
        struct any_conversion
        {
            template <typename P> any_conversion(const volatile P&);
            template <typename P> any_conversion(P&);
        };

        template <typename P> struct conversion_checker
        {
            static NO  _m_check(any_conversion ...);
            static YES _m_check(P, int);
        };    

        static T _m_from;
    public:
        enum
        {
            //是否可以转换(如果Test(makeT())匹配到了static Small Test(U), 则可以转换)
            exists      = (sizeof(conversion_checker<U>::_m_check(_m_from, 0)) == sizeof(YES)),
            //是否可以双向转换
            exists2Way  = exists && Conversion<U, T>::exists,
            //是否相同类型
            sameType    = false
        };
    };

    //偏特化来确定sameType
    template<class T>
    class Conversion<T, T>
    {
    public:
        enum
        {
            exists      = true,
            exists2Way  = true,
            sameType    = true
        };
    };

    //判断两个类是否可以继承
    //关键:子类指针可以转换成父类指针, 且不是void*类型
    //相同类型, SUPERSUBCLASS判断为true
    #define SUPERSUBCLASS(T, U) (TL::Conversion<const U*, const T*>::exists && !TL::Conversion<const T*, const void*>::sameType)
    //相同类型, SUPERSUBCLASS_STRICT判断为false
    #define SUPERSUBCLASS_STRICT(T, U) (SUPERSUBCLASS(T, U) && !TL::Conversion<const T, const U>::sameType)

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // 类型选择器
    template<bool flag, typename U, typename V>
    struct TypeSelect
    {
        typedef U Result;
    };

    template<typename U, typename V>
    struct TypeSelect<false, U, V>
    {
        typedef V Result;
    };

    ///////////////////////////////////////////////////////////////////////////////////////
    /**
     * 类型萃取器, copy至Loki库
     */
    template<typename T>
    class TypeTraits
    {
    private:

        ///////////////////////////////////////////////////////
        //提取引用的原始类型(即去掉引用类型)
        template<class U>
        struct ReferenceTraits
        {
            enum { result = false };
            typedef U Result;
        };

        template<class U>
        struct ReferenceTraits<U&>
        {
            enum { result = true };
            typedef U Result;
        };

        ///////////////////////////////////////////////////////
        //指针类型
        template<class U>
        struct PointerTraits
        {
            enum { result = false };
            typedef TL::NullType Result;
        };

        template<class U>
        struct PointerTraits<U*>
        {
            enum { result = true };
            typedef U Result;
        };

        template<class U>
        struct PointerTraits<U*&>
        {
            enum { result = true };
            typedef U Result;
        };

        ///////////////////////////////////////////////////////
        //成员函数指针, gcc下面支持有问题, 屏蔽之
        template<typename U>
        struct PointerToMemberTraits
        {
            enum { result = false };
        };

        template<class U, class V>
        struct PointerToMemberTraits<U V::*>
        {
            enum { result = true };
        };

        template<class U, class V>
        struct PointerToMemberTraits<U V::*&>
        {
            enum { result = true };
        };

        ///////////////////////////////////////////////////////
        // const
        template<typename U>
        struct UnConstTraits
        {
            enum { result = false };
            typedef U Result;
        };
        template<typename U>
        struct UnConstTraits<const U>
        {
            enum { result = true };
            typedef U Result;
        };
        template<typename U>
        struct UnConstTraits<const U&>
        {
            enum { result = true };
            typedef U& Result;
        };

        ///////////////////////////////////////////////////////
        // volatile
        template<typename U>
        struct UnVolatileTraits
        {
            enum { result = false };
            typedef U Result;
        };
        template<typename U>
        struct UnVolatileTraits<volatile U>
        {
            enum { result = true };
            typedef U Result;
        };
        template<typename U>
        struct UnVolatileTraits<volatile U&>
        {
            enum { result = true };
            typedef U& Result;
        };
    public:
        //T是否是指针类型
        enum { isPointer        = PointerTraits<T>::result };
        //T是否是引用类型
        enum { isReference         = ReferenceTraits<T>::result };
        //T是否指向成员函数的指针
        enum { isMemberPointer     = PointerToMemberTraits<T>::result };

        //T是否是Const类型
        enum { isConst          = UnConstTraits<T>::result };
        //T是否是Volatile类型
        enum { isVolatile       = UnVolatileTraits<T>::result };

        //如果T是指针类型,则获取T的原类型, 即去掉指针类型
        typedef typename PointerTraits<T>::Result             PointeeType;
        //如果T是引用类型,则获取T的原类型, 即去掉引用类型
        typedef typename ReferenceTraits<T>::Result         ReferencedType;
        //如果T是Const类型,则获取T的原类型, 即去掉Const类型
        typedef typename UnConstTraits<T>::Result           NonConstType;
        //如果T是volatile类型,则获取T的原类型, 即去掉volatile类型
        typedef typename UnVolatileTraits<T>::Result        NonVolatileType;
        //去掉const volatile类型
        typedef typename UnVolatileTraits<typename UnConstTraits<T>::Result>::Result UnqualifiedType;

    public:

        //////////////////////////////////////////////////////
        //
        typedef TL::TLMaker<unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>::Result UnsignedInts;
        typedef TL::TLMaker<signed char, short, int, long, long long>::Result SignedInts;
        typedef TL::TLMaker<bool, char, wchar_t>::Result OtherInts;
        typedef TL::TLMaker<float, double, long double>::Result Floats;
        typedef TL::TYPELIST_2(TL::EmptyType, TL::NullType) NullTypes;

        //无符号整形
        enum { isStdUnsignedInt = TL::IndexOf<UnsignedInts, T>::value >= 0 };
        //有符号整形
        enum { isStdSignedInt     = TL::IndexOf<SignedInts, T>::value >= 0 };
        //整形
        enum { isStdInt         = isStdUnsignedInt || isStdSignedInt || TL::IndexOf<OtherInts, T>::value >= 0 };
        //浮点类型
        enum { isStdFloat         = TL::IndexOf<Floats, T>::value >= 0 };
        //数值类型
        enum { isStdArith         = isStdInt || isStdFloat };
        //基础类型(包括void)
        enum { isStdFundamental    = isStdArith || TL::IndexOf<TL::TYPELIST_1(void), T>::value >= 0};
        //空类型
        enum { isNullType       = TL::IndexOf<NullTypes, T>::value >= 0 };
        //简单类型
        enum { isBaseType       = isStdArith || isPointer || isMemberPointer };

        //对于复杂类型, 获取数据的引用类型, 即加上引用类型
        typedef typename TypeSelect<isBaseType, T, ReferencedType&>::Result ReferenceType;

        //对于复杂类型且非空类型, 获取数据的引用类型, 即加上引用类型
        //typedef typename TypeSelect<isBaseType || isNullType, T, ReferencedType&>::Result ReferenceTypeEx;

        //获取数据的原类型, 消除引用的引用这种情况
        typedef typename TypeSelect<!isReference, T, ReferencedType&>::Result ParameterType;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //下面的使用开始展示TypeList的威力, 用于自动生成class

    //散乱的继承体系
    template<class TList, template <class> class Unit>
    class ScatterHierarchy;

    /*
    namespace p
    {
        //注释copy至loki库
        // The following type helps to overcome subtle flaw in the original
        // implementation of GenScatterHierarchy.
        // The flaw is revealed when the input type list of GenScatterHierarchy
        // contains more then tars element of the same type (e.g. LOKI_TYPELIST_2(int, int)).
        // In this case GenScatterHierarchy will contain multiple bases of the same
        // type and some of them will not be reachable (per 10.3).
        // For example before the fix the first element of Tuple<LOKI_TYPELIST_2(int, int)>
        // is not reachable in any way!
        template<class, class>
        struct ScatterHierarchyTag;
    }

    template<class T1, class T2, template <class> class Unit>
    class ScatterHierarchy<TypeList<T1, T2>, Unit> : public ScatterHierarchy<p::ScatterHierarchyTag<T1, T2>, Unit>
                                                   , public ScatterHierarchy<T2, Unit>
    {
    public:
        typedef TypeList<T1, T2> TList;
        typedef ScatterHierarchy<p::ScatterHierarchyTag<T1, T2>, Unit> LeftBase;
        typedef ScatterHierarchy<T2, Unit> RightBase;

        template<typename T> struct Rebind
        {
            typedef Unit<T> Result;
        };
    };

    // In the middle *unique* class that resolve possible ambiguity
    template <class T1, class T2, template <class> class Unit>
    class ScatterHierarchy<p::ScatterHierarchyTag<T1, T2>, Unit>
        : public ScatterHierarchy<T1, Unit>
    {
    };
    */

    //具现化继承体系
    template <class T1, class T2, template <class> class Unit>
    class ScatterHierarchy<TypeList<T1, T2>, Unit>
        : public ScatterHierarchy<T1, Unit>
        , public ScatterHierarchy<T2, Unit>
    {
    public:
        typedef TypeList<T1, T2> TList;
        typedef ScatterHierarchy<T1, Unit> LeftBase;
        typedef ScatterHierarchy<T2, Unit> RightBase;
        template <typename T> struct Rebind
        {
            typedef Unit<T> Result;
        };
    };

    template<class AtomicType, template <class> class Unit>
    class ScatterHierarchy : public Unit<AtomicType>
    {
    public:
        typedef Unit<AtomicType> LeftBase;

        template<typename T> struct Rebind
        {
            typedef Unit<T> Result;
        };
    };

    template<template <class> class Unit>
    class ScatterHierarchy<NullType, Unit>
    {
    public:
        template<typename T> struct Rebind
        {
            typedef Unit<T> Result;
        };
    };

    /////////////////////////////////////////////////////////////////////
    //构建继承体系后, 采用下面的函数获取继承体系中某个类

    template<class T, class H>
    struct FieldHelperByType
    {
        typedef typename H::template Rebind<T>::Result ResultType;
        static ResultType& dochange(H &obj)
        {
            return static_cast<ResultType&>(obj);
        }
    };
    template<class T, class H>
    struct FieldHelperByType<T, const H>
    {
        typedef const typename H::template Rebind<T>::Result ResultType;
        static ResultType& dochange(const H &obj)
        {
            return (ResultType&)obj;
        }
    };

    //直接按照类型获取, 如果有两个相同的类型, 则编译不过
    template<class T, class H>
    typename FieldHelperByType<T, H>::ResultType& field(H &obj)
    {
        return FieldHelperByType<T, H>::dochange(obj);
    }

    /////////////////////////////////////////////////////////////////////
    // 根据索引获取字段
    //定义Tuple, 默认的数据操作器(Unit)
    template<typename T>
    struct TupleUnit
    {
        T _value;
        operator T&()               { return _value; }
        operator const T&() const   { return _value; }
    };

    template<class TList>
    struct Tuple : public ScatterHierarchy<TList, TupleUnit>
    {
    };

    //定义FieldHelperByIndex
    template<class H, unsigned int i> struct FieldHelperByIndex;

    //特化版本的FieldHelperByIndex, 推导出最后一个元素
    template<class H>
    struct FieldHelperByIndex<H, 0>
    {
        typedef typename H::TList::H ElementType;
        typedef typename H::template Rebind<ElementType>::Result UnitType;

        enum
        {
            isTuple = Conversion<UnitType, TupleUnit<ElementType> >::sameType,
            isConst = TypeTraits<H>::isConst
        };

        typedef const typename H::LeftBase ConstLeftBase;
        typedef typename TypeSelect<isConst, ConstLeftBase, typename H::LeftBase>::Result LeftBase;
        typedef typename TypeSelect<isTuple, ElementType, UnitType>::Result UnqualifiedResultType;
        typedef typename TypeSelect<isConst, const UnqualifiedResultType, UnqualifiedResultType>::Result ResultType;

        static ResultType &dochange(H& obj)
        {
            LeftBase &leftBase = obj;
            return (ResultType&)leftBase;
        }
    };

    //根据索引获取FieldHelper
    template<class H, unsigned int i>
    struct FieldHelperByIndex
    {
        typedef typename TypeAt<typename H::TList, i>::Result ElementType;
        typedef typename H::template Rebind<ElementType>::Result UnitType;

        enum
        {
            isTuple = Conversion<UnitType, TupleUnit<ElementType> >::sameType,
            isConst = TypeTraits<H>::isConst
        };

        typedef const typename H::RightBase ConstRightBase;
        typedef typename TypeSelect<isConst, ConstRightBase, typename H::RightBase>::Result RightBase;
        typedef typename TypeSelect<isTuple, ElementType, UnitType>::Result UnqualifiedResultType;
        typedef typename TypeSelect<isConst, const UnqualifiedResultType, UnqualifiedResultType>::Result ResultType;

        static ResultType &dochange(H& obj)
        {
            RightBase &rightBase = obj;
            return FieldHelperByIndex<RightBase, i-1>::dochange(rightBase);
        }
    };

    //定义按照索引获取
    template<unsigned int i, class H>
    typename FieldHelperByIndex<H, i>::ResultType &field(H& obj)
    {
        return FieldHelperByIndex<H, i>::dochange(obj);
    }
}

}
#endif

