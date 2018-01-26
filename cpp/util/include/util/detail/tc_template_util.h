/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016THL A29 Limited, a Tencent company. All rights reserved.
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

#ifndef __TC_TEMPLATE_UTIL_H__
#define __TC_TEMPLATE_UTIL_H__

namespace tars 
{
    template <typename T, T v>
    struct tc_integral_constant 
    {
        typedef T value_type;
        typedef tc_integral_constant<T, v> type;
        static const T value = v;
    };

    template <typename T, T v> const T tc_integral_constant<T, v>::value;
    
    typedef tc_integral_constant<bool, true> true_type;
    typedef tc_integral_constant<bool, false> false_type;

    template <typename T> struct tc_is_void : false_type {};
    template <> struct tc_is_void<void> : true_type {};
    
    template <typename T> struct tc_is_non_const_reference : false_type {};
    template <typename T> struct tc_is_non_const_reference<T&> : true_type {};
    template <typename T> struct tc_is_non_const_reference<const T&> : false_type {};

    template <typename T> struct tc_is_raw_pointer : false_type {};
    template <typename T> struct tc_is_raw_pointer<T*> : true_type {};

    template <typename U, typename V>
    struct tc_is_same_type : tc_integral_constant<bool, false> {};

    template <typename U>
    struct tc_is_same_type<U, U> : tc_integral_constant<bool, true> {};

    template <bool, typename type1, typename type2>
    struct tc_select_type_c;

    template <typename type1, typename type2>
    struct tc_select_type_c<true, type1, type2> 
    {
        typedef type1 type;
    };

    template <typename type1, typename type2>
    struct tc_select_type_c<false, type1, type2> 
    {
        typedef type2 type;
    };

    template <typename Cond, typename T1, typename T2>
    struct tc_select_type : tc_select_type_c<Cond::value, T1, T2> {};
    
    namespace detail 
    {
        typedef char (&Yes)[1];
        typedef char (&No)[2];

        struct TC_ConvertHelper 
        {
            template <typename To>
            static Yes Test(To);

            template <typename To>
            static No Test(...);
            
            template <typename From>
            static From Make();
        };

        struct TC_IsClassHelper 
        {
            template <typename C>
            static Yes Test(void (C::*)(void));

            template <typename C>
            static No Test(...);
        };

    }

    template <typename From, typename To>
    struct tc_is_convertible : tc_integral_constant<bool,
        sizeof(detail::TC_ConvertHelper::Test<To>(detail::TC_ConvertHelper::Make<From>())) == sizeof(detail::Yes)> 
    {
    };
    
    template <typename T>
    struct tc_is_class : tc_integral_constant<bool,
        sizeof(detail::TC_IsClassHelper::Test<T>(0)) == sizeof(detail::Yes)> 
    {
    };

    template <typename D, typename B>
    struct tc_is_derived_from 
    {
        enum { value = tc_is_class<D>::value && tc_is_class<B>::value && tc_is_convertible<D*, B*>::value };
    }; 
    
    template <typename T>
    class tc_has_method_foo 
    {
        struct BasMixin 
        {
            void foo();
        };

        struct Base : public T, public BasMixin {};

        template <void (BasMixin::*)(void)> struct Helper {};
        
        template <typename C>
        static detail::No Test(Helper<&C::foo>*);

        template <typename>
        static detail::Yes Test(...);

    public:

        enum { value = sizeof(Test<Base>(0)) == sizeof(detail::Yes) };
    };

    template <bool, typename T = void>
    struct tc_enable_if_c 
    {
        typedef T type;
    };

    template <typename T>
    struct tc_enable_if_c<false, T> {};

    template <typename Cond, typename T = void>
    struct tc_enable_if : public tc_enable_if_c<Cond::value, T> {};

    template <bool, typename T = void>
    struct tc_disable_if_c 
    {
        typedef T type;
    };

    template <typename T>
    struct tc_disable_if_c<true, T> {};

    template <typename Cond, typename T = void>
    struct tc_disable_if : public tc_disable_if_c<Cond::value, T> {};
    
}

#endif
