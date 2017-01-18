#ifndef __TARS_PROMISE_TUPLE_H_
#define __TARS_PROMISE_TUPLE_H_

#include "util/detail/tc_assert.h"
#include <ostream>

namespace promise 
{
    namespace detail 
    {
        struct null_type {};

        inline bool operator==(const null_type&, const null_type&)
        { return true; }

        inline bool operator!=(const null_type&, const null_type&)
        { return false; }

        inline bool operator<(const null_type&, const null_type&)
        { return false; }
        
        // A helper function to make a const temporary.
        inline const null_type make_null() { return null_type(); }

        template <typename HT, typename TT>
        struct Cons 
        {
            typedef HT head_type;
            typedef TT tail_type;

            Cons()
            : head()
            , tail()
            {}

            Cons(const head_type& h, const tail_type& t)
            : head(h)
            , tail(t)
            {}

            template <typename T0, typename T1, typename T2, typename T3, typename T4,
                      typename T5, typename T6, typename T7, typename T8, typename T9>
            Cons(T0& t0, T1& t1, T2& t2, T3& t3, T4& t4,
                 T5& t5, T6& t6, T7& t7, T8& t8, T9& t9)
            : head(t0)
            , tail(t1, t2, t3, t4, t5, t6, t7, t8, t9, make_null())
            {}

            template <typename HT2, typename TT2>
            Cons(const Cons<HT2, TT2>& o)
            : head(o.head)
            , tail(o.tail)
            {}
            
            template <typename HT2, typename TT2>
            Cons& operator=(const Cons<HT2, TT2>& o)
            {
                head = o.head;
                tail = o.tail;
                return *this;
            }

            void swap(const Cons& o)
            {
                std::swap(head, o.head);
                std::swap(tail, o.tail);
            }

            // Because the C++ standard prohibits the function template partial
            // specialization, we have to define `operator==()' and `operator!=' as the
            // member function.

            bool operator==(const Cons& o) const
            {
                return head == o.head && tail == o.tail;
            }

            bool operator!=(const Cons& o) const
            {
                return !(operator==(o));
            }
            
            bool operator<(const Cons& o) const
            {
                return (head < o.head) || (!(o.head < head) && tail < o.tail);
            }
            
            head_type head;
            tail_type tail;
        };

        template <typename HT>
        struct Cons<HT, null_type> 
        {
            typedef HT head_type;
            typedef null_type tail_type;

            Cons()
            : head()
            {}

            template <typename T0>
            Cons(T0& t0, const null_type&, const null_type&, const null_type&, const null_type&,
                 const null_type&, const null_type&, const null_type&, const null_type&, const null_type&)
            : head(t0)
            {}

            template <typename HT2>
            Cons& operator=(const Cons<HT2, null_type>& o)
            {
                head = o.head;
                return *this;
            }

            void swap(Cons& o)
            {
                std::swap(head, o.head);
            }
            
            // Because the C++ standard prohibits the function template partial
            // specialization, we have to define `operator==()' and `operator!=' as the
            // member function.

            bool operator==(const Cons& o) const
            {
                return head == o.head;
            }

            bool operator!=(const Cons& o) const
            {
                return !(operator==(o));
            }

            bool operator<(const Cons& o) const
            {
                return head < o.head;
            }
            
            head_type head;
        };
        
        template <typename T0, typename T1, typename T2, typename T3, typename T4,
                  typename T5, typename T6, typename T7, typename T8, typename T9>
        struct map_tuple_to_cons 
        {
            typedef Cons<T0, typename map_tuple_to_cons<T1, T2, T3, T4, T5, T6, T7, T8, T9, null_type>::type> type;
        };

        template <>
        struct map_tuple_to_cons<null_type, null_type, null_type, null_type, null_type,
                                 null_type, null_type, null_type, null_type, null_type> 
        {
            typedef null_type type;
        };

        // Gets the N-th tail of a tuple.
        template <int N, typename Tuple>
        struct drop_front 
        {
            typedef typename drop_front<N-1, Tuple>::type::tail_type type;
            static const type& get_tail(const Tuple& tuple)
            {
                return drop_front<N-1, Tuple>::get_tail(tuple).tail;
            }
        };

        template <typename Tuple>
        struct drop_front<0, Tuple> 
        {
            typedef Tuple type;
            static const type& get_tail(const Tuple& tuple)
            {
                return tuple;
            }
        };

        // Gets the type of the N-th element of a tuple.
        template <int N, typename Tuple>
        struct element 
        {
            typedef typename drop_front<N, Tuple>::type::head_type type;
        };

        // Gets the length of a tuple.
        template <typename Tuple>
        struct length 
        {
            static const int value = length<typename Tuple::tail_type>::value + 1;
        };

        template <>
        struct length<null_type> 
        {
            static const int value = 0;
        };

        // Helper classes to print Cons.
        template <typename HT, typename TT>
        struct print_cons_helper 
        {
            static std::ostream& print(std::ostream& o, const Cons<HT, TT>& cons)
            {
                o << cons.head;
                o << ", ";
                print_cons_helper<typename TT::head_type, typename TT::tail_type>::print(o, cons.tail);
                return o;
            }
        };

        template <typename HT>
        struct print_cons_helper<HT, null_type> 
        {
            static std::ostream& print(std::ostream& o, const Cons<HT, null_type>& cons)
            {
                o << cons.head;
                return o;
            }
        };

        template <typename HT, typename TT>
        inline std::ostream& print_cons(std::ostream& o, const Cons<HT, TT>& cons)
        {
            return print_cons_helper<HT, TT>::print(o, cons);
        }

        inline std::ostream& print_cons(std::ostream& o, const null_type&)
        {
            return o;
        }
        
    }  // namespace detail

    template <typename T0 = detail::null_type, typename T1 = detail::null_type, typename T2 = detail::null_type,
              typename T3 = detail::null_type, typename T4 = detail::null_type, typename T5 = detail::null_type,
              typename T6 = detail::null_type, typename T7 = detail::null_type, typename T8 = detail::null_type,
              typename T9 = detail::null_type>
    class Tuple 
    {
    private:
        typedef typename detail::map_tuple_to_cons<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::type tuple_impl_type;

    public:
        
        // Returns the type of the N-th element of this tuple.
        template <int N>
        struct element 
        {
            typedef typename detail::element<N, tuple_impl_type>::type type;
        };
        
        template <typename T>
        struct access_traits 
        {
            typedef const T& parameter_type;
            typedef const T& const_type;
            typedef T& non_const_type;
        };

        template <typename T>
        struct access_traits<T&> 
        {
            typedef T& parameter_type;
            typedef T& const_type;
            typedef T& non_const_type;
        };

        // Number of elements of this tuple.
        enum 
        {
            LENGTH = detail::length<tuple_impl_type>::value
        };
            
        Tuple() {}

        Tuple(typename access_traits<T0>::parameter_type t0)
        : m_tuple_impl(t0, detail::make_null(), detail::make_null(), detail::make_null(), detail::make_null(),
             detail::make_null(), detail::make_null(), detail::make_null(), detail::make_null(), detail::make_null())
        {}

        Tuple(typename access_traits<T0>::parameter_type t0,
              typename access_traits<T1>::parameter_type t1)
        : m_tuple_impl(t0, t1, detail::make_null(), detail::make_null(), detail::make_null(),
             detail::make_null(), detail::make_null(), detail::make_null(), detail::make_null(), detail::make_null())
        {}

        Tuple(typename access_traits<T0>::parameter_type t0,
              typename access_traits<T1>::parameter_type t1,
              typename access_traits<T2>::parameter_type t2)
        : m_tuple_impl(t0, t1, t2, detail::make_null(), detail::make_null(),
             detail::make_null(), detail::make_null(), detail::make_null(), detail::make_null(), detail::make_null())
        {}

        Tuple(typename access_traits<T0>::parameter_type t0,
              typename access_traits<T1>::parameter_type t1,
              typename access_traits<T2>::parameter_type t2,
              typename access_traits<T3>::parameter_type t3)
        : m_tuple_impl(t0, t1, t2, t3, detail::make_null(), detail::make_null(), detail::make_null(), detail::make_null(),
             detail::make_null(), detail::make_null())
        {}

        Tuple(typename access_traits<T0>::parameter_type t0,
              typename access_traits<T1>::parameter_type t1,
              typename access_traits<T2>::parameter_type t2,
              typename access_traits<T3>::parameter_type t3,
              typename access_traits<T4>::parameter_type t4)
        : m_tuple_impl(t0, t1, t2, t3, t4, detail::make_null(), detail::make_null(), detail::make_null(),
             detail::make_null(), detail::make_null())
        {}
        
        Tuple(typename access_traits<T0>::parameter_type t0,
              typename access_traits<T1>::parameter_type t1,
              typename access_traits<T2>::parameter_type t2,
              typename access_traits<T3>::parameter_type t3,
              typename access_traits<T4>::parameter_type t4,
              typename access_traits<T5>::parameter_type t5)
        : m_tuple_impl(t0, t1, t2, t3, t4, t5, detail::make_null(), detail::make_null(),
             detail::make_null(), detail::make_null())
        {}
        
        Tuple(typename access_traits<T0>::parameter_type t0,
              typename access_traits<T1>::parameter_type t1,
              typename access_traits<T2>::parameter_type t2,
              typename access_traits<T3>::parameter_type t3,
              typename access_traits<T4>::parameter_type t4,
              typename access_traits<T5>::parameter_type t5,
              typename access_traits<T6>::parameter_type t6)
        : m_tuple_impl(t0, t1, t2, t3, t4, t5, t6, detail::make_null(), detail::make_null(), detail::make_null())
        {}
        
        Tuple(typename access_traits<T0>::parameter_type t0,
              typename access_traits<T1>::parameter_type t1,
              typename access_traits<T2>::parameter_type t2,
              typename access_traits<T3>::parameter_type t3,
              typename access_traits<T4>::parameter_type t4,
              typename access_traits<T5>::parameter_type t5,
              typename access_traits<T6>::parameter_type t6,
              typename access_traits<T7>::parameter_type t7)
        : m_tuple_impl(t0, t1, t2, t3, t4, t5, t6, t7, detail::make_null(), detail::make_null())
        {}
        
        Tuple(typename access_traits<T0>::parameter_type t0,
              typename access_traits<T1>::parameter_type t1,
              typename access_traits<T2>::parameter_type t2,
              typename access_traits<T3>::parameter_type t3,
              typename access_traits<T4>::parameter_type t4,
              typename access_traits<T5>::parameter_type t5,
              typename access_traits<T6>::parameter_type t6,
              typename access_traits<T7>::parameter_type t7,
              typename access_traits<T8>::parameter_type t8)
        : m_tuple_impl(t0, t1, t2, t3, t4, t5, t6, t7, t8, detail::make_null())
        {}
        
        Tuple(typename access_traits<T0>::parameter_type t0,
              typename access_traits<T1>::parameter_type t1,
              typename access_traits<T2>::parameter_type t2,
              typename access_traits<T3>::parameter_type t3,
              typename access_traits<T4>::parameter_type t4,
              typename access_traits<T5>::parameter_type t5,
              typename access_traits<T6>::parameter_type t6,
              typename access_traits<T7>::parameter_type t7,
              typename access_traits<T8>::parameter_type t8,
              typename access_traits<T9>::parameter_type t9)
        : m_tuple_impl(t0, t1, t2, t3, t4, t5, t6, t7, t8, t9)
        {}

        template <typename TT0, typename TT1, typename TT2, typename TT3, typename TT4,
                  typename TT5, typename TT6, typename TT7, typename TT8, typename TT9>
        Tuple(const Tuple<TT0, TT1, TT2, TT3, TT4, TT5, TT6, TT7, TT8, TT9>& o)
        : m_tuple_impl(o.m_tuple_impl)
        {}
        
        template <typename TT0, typename TT1, typename TT2, typename TT3, typename TT4,
                  typename TT5, typename TT6, typename TT7, typename TT8, typename TT9>
        Tuple& operator=(const Tuple<TT0, TT1, TT2, TT3, TT4, TT5, TT6, TT7, TT8, TT9>& o)
        {
            m_tuple_impl = o.m_tuple_impl;
            return *this;
        }
        
        template <int N>
        typename access_traits<typename element<N>::type>::const_type
        get() const
        {
            TC_STATIC_ASSERT(N < LENGTH, index_out_of_bounds);
            return detail::drop_front<N, tuple_impl_type>::get_tail(m_tuple_impl).head;
        }

        template <int N>
        typename access_traits<typename element<N>::type>::non_const_type
        get()
        {
            TC_STATIC_ASSERT(N < LENGTH, index_out_of_bounds);
            typedef detail::drop_front<N, tuple_impl_type> impl;
            typedef typename impl::type cons_element;
            return const_cast<cons_element&>(impl::get_tail(m_tuple_impl)).head;
        }

        void swap(Tuple& o)
        {
            m_tuple_impl.swap(o.m_tuple_impl);
        }

    private:
        tuple_impl_type m_tuple_impl;

        template <typename TT0, typename TT1, typename TT2, typename TT3, typename TT4,
                  typename TT5, typename TT6, typename TT7, typename TT8, typename TT9>
        friend class Tuple;

        friend bool operator==(const Tuple& lhs, const Tuple& rhs)
        {
            return lhs.m_tuple_impl == rhs.m_tuple_impl;
        }

        friend bool operator!=(const Tuple& lhs, const Tuple& rhs)
        {
            return lhs.m_tuple_impl != rhs.m_tuple_impl;
        }

        friend bool operator<(const Tuple& lhs, const Tuple& rhs)
        {
            return lhs.m_tuple_impl < rhs.m_tuple_impl;
        }

        friend std::ostream& operator<<(std::ostream& o, const Tuple& t)
        {
            o << '(';
            detail::print_cons(o, t.m_tuple_impl);
            o << ')';
            return o;
        }
    };
    
    namespace detail 
    {
        // Call traits for make_tuple
        //
        // Conversions:
        //   T -> T
        //   T& -> T
        //   const T& -> T
        //   T[] -> const T*
        //   const T[] -> const T*

        template <typename T>
        struct make_tuple_traits 
        {
            typedef T type;
        };

        template <typename T>
        struct make_tuple_traits<T&> 
        {
            typedef T type;
        };

        // To produce compilation error intentionally by referencing an non-exsitent
        // member of this class.
        template <typename T>
        class generate_error {};
        
        template <typename T, size_t n>
        struct make_tuple_traits<T[n]> 
        {
            typedef const T* type;
        };

        template <typename T>
        struct make_tuple_traits<const T[]> 
        {
            typedef const T* type;
        };
        
        template <typename T0 = null_type, typename T1 = null_type, typename T2 = null_type,
                  typename T3 = null_type, typename T4 = null_type, typename T5 = null_type,
                  typename T6 = null_type, typename T7 = null_type, typename T8 = null_type,
                  typename T9 = null_type>
        struct make_tuple_mapper 
        {
            typedef Tuple<typename make_tuple_traits<T0>::type,
                          typename make_tuple_traits<T1>::type,
                          typename make_tuple_traits<T2>::type,
                          typename make_tuple_traits<T3>::type,
                          typename make_tuple_traits<T4>::type,
                          typename make_tuple_traits<T5>::type,
                          typename make_tuple_traits<T6>::type,
                          typename make_tuple_traits<T7>::type,
                          typename make_tuple_traits<T8>::type,
                          typename make_tuple_traits<T9>::type> type;
        };

    }  // namespace detail

    // make_tuple function templates for arities 0~10

    inline Tuple<> make_tuple()
    { return Tuple<>(); }

    template <typename T0>
    inline typename detail::make_tuple_mapper<T0>::type
    make_tuple(const T0& t0)
    {
        typedef typename detail::make_tuple_mapper<T0>::type tuple_type;
        return tuple_type(t0);
    }

    template <typename T0, typename T1>
    inline typename detail::make_tuple_mapper<T0, T1>::type
    make_tuple(const T0& t0, const T1& t1)
    {
        typedef typename detail::make_tuple_mapper<T0, T1>::type tuple_type;
        return tuple_type(t0, t1);
    }
    
    template <typename T0, typename T1, typename T2>
    inline typename detail::make_tuple_mapper<T0, T1, T2>::type
    make_tuple(const T0& t0, const T1& t1, const T2& t2)
    {
        typedef typename detail::make_tuple_mapper<T0, T1, T2>::type tuple_type;
        return tuple_type(t0, t1, t2);
    }
    
    template <typename T0, typename T1, typename T2, typename T3>
    inline typename detail::make_tuple_mapper<T0, T1, T2, T3>::type
    make_tuple(const T0& t0, const T1& t1, const T2& t2, const T3& t3)
    {
        typedef typename detail::make_tuple_mapper<T0, T1, T2, T3>::type tuple_type;
        return tuple_type(t0, t1, t2, t3);
    }
    
    template <typename T0, typename T1, typename T2, typename T3, typename T4>
    inline typename detail::make_tuple_mapper<T0, T1, T2, T3, T4>::type
    make_tuple(const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4)
    {
        typedef typename detail::make_tuple_mapper<T0, T1, T2, T3, T4>::type tuple_type;
        return tuple_type(t0, t1, t2, t3, t4);
    }
    
    template <typename T0, typename T1, typename T2, typename T3, typename T4,
              typename T5>
    inline typename detail::make_tuple_mapper<T0, T1, T2, T3, T4, T5>::type
    make_tuple(const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4,
               const T5& t5)
    {
        typedef typename detail::make_tuple_mapper<T0, T1, T2, T3, T4, T5>::type tuple_type;
        return tuple_type(t0, t1, t2, t3, t4, t5);
    }
    
    template <typename T0, typename T1, typename T2, typename T3, typename T4,
              typename T5, typename T6>
    inline typename detail::make_tuple_mapper<T0, T1, T2, T3, T4, T5, T6>::type
    make_tuple(const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4,
               const T5& t5, const T6& t6)
    {
        typedef typename detail::make_tuple_mapper<T0, T1, T2, T3, T4, T5, T6>::type tuple_type;
        return tuple_type(t0, t1, t2, t3, t4, t5, t6);
    }
    
    template <typename T0, typename T1, typename T2, typename T3, typename T4,
              typename T5, typename T6, typename T7>
    inline typename detail::make_tuple_mapper<T0, T1, T2, T3, T4, T5, T6, T7>::type
    make_tuple(const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4,
               const T5& t5, const T6& t6, const T7& t7)
    {
        typedef typename detail::make_tuple_mapper<T0, T1, T2, T3, T4, T5, T6, T7>::type tuple_type;
        return tuple_type(t0, t1, t2, t3, t4, t5, t6, t7);
    }
    
    template <typename T0, typename T1, typename T2, typename T3, typename T4,
              typename T5, typename T6, typename T7, typename T8>
    inline typename detail::make_tuple_mapper<T0, T1, T2, T3, T4, T5, T6, T7, T8>::type
    make_tuple(const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4,
               const T5& t5, const T6& t6, const T7& t7, const T8& t8)
    {
        typedef typename detail::make_tuple_mapper<T0, T1, T2, T3, T4, T5, T6, T7, T8>::type tuple_type;
        return tuple_type(t0, t1, t2, t3, t4, t5, t6, t7, t8);
    }
    
    template <typename T0, typename T1, typename T2, typename T3, typename T4,
              typename T5, typename T6, typename T7, typename T8, typename T9>
    inline typename detail::make_tuple_mapper<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::type
    make_tuple(const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4,
               const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9)
    {
        typedef typename detail::make_tuple_mapper<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::type tuple_type;
        return tuple_type(t0, t1, t2, t3, t4, t5, t6, t7, t8, t9);
    }
    
}  // namespace promise

#endif  // __TARS_PROMISE_TUPLE_H__
