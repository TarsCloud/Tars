#ifndef __TC_CALLBACK_H__
#define __TC_CALLBACK_H__

#include "tc_shared_ptr.h"
#include "tc_callback_detail.h"

namespace tars
{

    namespace detail
    {
        template <typename FunctorType, typename BindRunType, typename BoundArgsType> class TC_BindState;
    }

    template <typename Signature>
    class TC_Callback;

    template <typename R>
    class TC_Callback<R(void)> {
    private:
        typedef void (*GenFuncPointer)(void);
        typedef GenFuncPointer unspecified_bool_type;
        
    public:

        typedef R (RunType)(void);
        
        TC_Callback()
            : m_invoke(NULL)
        { }
        
        template <typename FunctorType, typename BindRunType, typename BoundArgsType>
        TC_Callback(detail::TC_BindState<FunctorType, BindRunType, BoundArgsType>* bind_state)
            : m_bind_state_ptr(bind_state)
        {
            PolymorphicInvoke invoke = &detail::TC_BindState<FunctorType, BindRunType, BoundArgsType>::InvokerType::invoke;
            m_invoke = reinterpret_cast<GenFuncPointer>(invoke);
        }

        R operator()() const
        {
            PolymorphicInvoke f = reinterpret_cast<PolymorphicInvoke>(m_invoke);
            return f(m_bind_state_ptr.get());
        }

        operator unspecified_bool_type() const
        {
            return m_invoke;
        }
        
    private:
        typedef R (*PolymorphicInvoke)(detail::TC_BindStateBase*);

        TC_SharedPtr<detail::TC_BindStateBase> m_bind_state_ptr;
        GenFuncPointer m_invoke;
    };

    template <typename R, typename A1>
    class TC_Callback<R(A1)> {
    private:
        typedef void (*GenFuncPointer)(void);
        typedef GenFuncPointer unspecified_bool_type;
        
    public:

        typedef R (RunType)(A1);
        
        TC_Callback()
            : m_invoke(NULL)
        { }

        template <typename FunctorType, typename BindRunType, typename BoundArgsType>
        TC_Callback(detail::TC_BindState<FunctorType, BindRunType, BoundArgsType>* bind_state)
            : m_bind_state_ptr(bind_state)
        {
            PolymorphicInvoke invoke = &detail::TC_BindState<FunctorType, BindRunType, BoundArgsType>::InvokerType::invoke;
            m_invoke = reinterpret_cast<GenFuncPointer>(invoke);
        }

        R operator()(typename detail::TC_CallbackParamTraits<A1>::ForwardType a1) const
        {
            PolymorphicInvoke f = reinterpret_cast<PolymorphicInvoke>(m_invoke);
            return f(m_bind_state_ptr.get(), a1);
        }

        operator unspecified_bool_type() const
        {
            return m_invoke;
        }
        
    private:

        typedef R (*PolymorphicInvoke)(detail::TC_BindStateBase*,
                                       typename detail::TC_CallbackParamTraits<A1>::ForwardType);

        TC_SharedPtr<detail::TC_BindStateBase> m_bind_state_ptr;
        GenFuncPointer m_invoke;
    };
    
    template <typename R, typename A1, typename A2>
    class TC_Callback<R(A1, A2)> {
    private:
        typedef void (*GenFuncPointer)(void);
        typedef GenFuncPointer unspecified_bool_type;
        
    public:

        typedef R (RunType)(A1, A2);
        
        TC_Callback()
            : m_invoke(NULL)
        { }

        template <typename FunctorType, typename BindRunType, typename BoundArgsType>
        TC_Callback(detail::TC_BindState<FunctorType, BindRunType, BoundArgsType>* bind_state)
            : m_bind_state_ptr(bind_state)
        {
            PolymorphicInvoke invoke = &detail::TC_BindState<FunctorType, BindRunType, BoundArgsType>::InvokerType::invoke;
            m_invoke = reinterpret_cast<GenFuncPointer>(invoke);
        }

        R operator()(typename detail::TC_CallbackParamTraits<A1>::ForwardType a1,
                     typename detail::TC_CallbackParamTraits<A2>::ForwardType a2)
        {
            PolymorphicInvoke f = reinterpret_cast<PolymorphicInvoke>(m_invoke);
            return f(m_bind_state_ptr.get(), a1, a2);
        }

        operator unspecified_bool_type() const
        {
            return m_invoke;
        }
        
    private:

        typedef R (*PolymorphicInvoke)(detail::TC_BindStateBase*,
                                       typename detail::TC_CallbackParamTraits<A1>::ForwardType,
                                       typename detail::TC_CallbackParamTraits<A2>::ForwardType);

        TC_SharedPtr<detail::TC_BindStateBase> m_bind_state_ptr;
        GenFuncPointer m_invoke;
    };

    template <typename R, typename A1, typename A2, typename A3>
    class TC_Callback<R(A1, A2, A3)> {
    private:
        typedef void (*GenFuncPointer)(void);
        typedef GenFuncPointer unspecified_bool_type;
        
    public:

        typedef R (RunType)(A1, A2, A3);
        
        TC_Callback()
            : m_invoke(NULL)
        { }

        template <typename FunctorType, typename BindRunType, typename BoundArgsType>
        TC_Callback(detail::TC_BindState<FunctorType, BindRunType, BoundArgsType>* bind_state)
            : m_bind_state_ptr(bind_state)
        {
            PolymorphicInvoke invoke = &detail::TC_BindState<FunctorType, BindRunType, BoundArgsType>::InvokerType::invoke;
            m_invoke = reinterpret_cast<GenFuncPointer>(invoke);
        }

        R operator()(typename detail::TC_CallbackParamTraits<A1>::ForwardType a1,
                     typename detail::TC_CallbackParamTraits<A2>::ForwardType a2,
                     typename detail::TC_CallbackParamTraits<A3>::ForwardType a3) const
        {
            PolymorphicInvoke f = reinterpret_cast<PolymorphicInvoke>(m_invoke);
            return f(m_bind_state_ptr.get(), a1, a2, a3);
        }

        operator unspecified_bool_type() const
        {
            return m_invoke;
        }
        
    private:

        typedef R (*PolymorphicInvoke)(detail::TC_BindStateBase*,
                                       typename detail::TC_CallbackParamTraits<A1>::ForwardType a1,
                                       typename detail::TC_CallbackParamTraits<A2>::ForwardType a2,
                                       typename detail::TC_CallbackParamTraits<A3>::ForwardType a3);

        TC_SharedPtr<detail::TC_BindStateBase> m_bind_state_ptr;
        GenFuncPointer m_invoke;
    };

    template <typename R, typename A1, typename A2, typename A3, typename A4>
    class TC_Callback<R(A1, A2, A3, A4)> {
    private:
        typedef void (*GenFuncPointer)(void);
        typedef GenFuncPointer unspecified_bool_type;
        
    public:

        typedef R (RunType)(A1, A2, A3, A4);
        
        TC_Callback()
            : m_invoke(NULL)
        { }

        template <typename FunctorType, typename BindRunType, typename BoundArgsType>
        TC_Callback(detail::TC_BindState<FunctorType, BindRunType, BoundArgsType>* bind_state)
            : m_bind_state_ptr(bind_state)
        {
            PolymorphicInvoke invoke = &detail::TC_BindState<FunctorType, BindRunType, BoundArgsType>::InvokerType::invoke;
            m_invoke = reinterpret_cast<GenFuncPointer>(invoke);
        }

        R operator()(typename detail::TC_CallbackParamTraits<A1>::ForwardType a1,
                     typename detail::TC_CallbackParamTraits<A2>::ForwardType a2,
                     typename detail::TC_CallbackParamTraits<A3>::ForwardType a3,
                     typename detail::TC_CallbackParamTraits<A4>::ForwardType a4) const
        {
            PolymorphicInvoke f = reinterpret_cast<PolymorphicInvoke>(m_invoke);
            return f(m_bind_state_ptr.get(), a1, a2, a3, a4);
        }

        operator unspecified_bool_type() const
        {
            return m_invoke;
        }
        
    private:

        typedef R (*PolymorphicInvoke)(detail::TC_BindStateBase*,
                                       typename detail::TC_CallbackParamTraits<A1>::ForwardType a1,
                                       typename detail::TC_CallbackParamTraits<A2>::ForwardType a2,
                                       typename detail::TC_CallbackParamTraits<A3>::ForwardType a3,
                                       typename detail::TC_CallbackParamTraits<A4>::ForwardType a4);

        TC_SharedPtr<detail::TC_BindStateBase> m_bind_state_ptr;
        GenFuncPointer m_invoke;
    };

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
    class TC_Callback<R(A1, A2, A3, A4, A5)> {
    private:
        typedef void (*GenFuncPointer)(void);
        typedef GenFuncPointer unspecified_bool_type;
        
    public:

        typedef R (RunType)(A1, A2, A3, A4, A5);
        
        TC_Callback()
            : m_invoke(NULL)
        { }

        template <typename FunctorType, typename BindRunType, typename BoundArgsType>
        TC_Callback(detail::TC_BindState<FunctorType, BindRunType, BoundArgsType>* bind_state)
            : m_bind_state_ptr(bind_state)
        {
            PolymorphicInvoke invoke = &detail::TC_BindState<FunctorType, BindRunType, BoundArgsType>::InvokerType::invoke;
            m_invoke = reinterpret_cast<GenFuncPointer>(invoke);
        }

        R operator()(typename detail::TC_CallbackParamTraits<A1>::ForwardType a1,
                     typename detail::TC_CallbackParamTraits<A2>::ForwardType a2,
                     typename detail::TC_CallbackParamTraits<A3>::ForwardType a3,
                     typename detail::TC_CallbackParamTraits<A4>::ForwardType a4,
                     typename detail::TC_CallbackParamTraits<A5>::ForwardType a5) const
        {
            PolymorphicInvoke f = reinterpret_cast<PolymorphicInvoke>(m_invoke);
            return f(m_bind_state_ptr.get(), a1, a2, a3, a4, a5);
        }

        operator unspecified_bool_type() const
        {
            return m_invoke;
        }
        
    private:

        typedef R (*PolymorphicInvoke)(detail::TC_BindStateBase*,
                                       typename detail::TC_CallbackParamTraits<A1>::ForwardType a1,
                                       typename detail::TC_CallbackParamTraits<A2>::ForwardType a2,
                                       typename detail::TC_CallbackParamTraits<A3>::ForwardType a3,
                                       typename detail::TC_CallbackParamTraits<A4>::ForwardType a4,
                                       typename detail::TC_CallbackParamTraits<A5>::ForwardType a5);

        TC_SharedPtr<detail::TC_BindStateBase> m_bind_state_ptr;
        GenFuncPointer m_invoke;
    };

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    class TC_Callback<R(A1, A2, A3, A4, A5, A6)> {
    private:
        typedef void (*GenFuncPointer)(void);
        typedef GenFuncPointer unspecified_bool_type;
        
    public:

        typedef R (RunType)(A1, A2, A3, A4, A5, A6);
        
        TC_Callback()
            : m_invoke(NULL)
        { }

        template <typename FunctorType, typename BindRunType, typename BoundArgsType>
        TC_Callback(detail::TC_BindState<FunctorType, BindRunType, BoundArgsType>* bind_state)
            : m_bind_state_ptr(bind_state)
        {
            PolymorphicInvoke invoke = &detail::TC_BindState<FunctorType, BindRunType, BoundArgsType>::InvokerType::invoke;
            m_invoke = reinterpret_cast<GenFuncPointer>(invoke);
        }

        R operator()(typename detail::TC_CallbackParamTraits<A1>::ForwardType a1,
                     typename detail::TC_CallbackParamTraits<A2>::ForwardType a2,
                     typename detail::TC_CallbackParamTraits<A3>::ForwardType a3,
                     typename detail::TC_CallbackParamTraits<A4>::ForwardType a4,
                     typename detail::TC_CallbackParamTraits<A5>::ForwardType a5,
                     typename detail::TC_CallbackParamTraits<A6>::ForwardType a6) const
        {
            PolymorphicInvoke f = reinterpret_cast<PolymorphicInvoke>(m_invoke);
            return f(m_bind_state_ptr.get(), a1, a2, a3, a4, a5, a6);
        }

        operator unspecified_bool_type() const
        {
            return m_invoke;
        }
        
    private:

        typedef R (*PolymorphicInvoke)(detail::TC_BindStateBase*,
                                       typename detail::TC_CallbackParamTraits<A1>::ForwardType a1,
                                       typename detail::TC_CallbackParamTraits<A2>::ForwardType a2,
                                       typename detail::TC_CallbackParamTraits<A3>::ForwardType a3,
                                       typename detail::TC_CallbackParamTraits<A4>::ForwardType a4,
                                       typename detail::TC_CallbackParamTraits<A5>::ForwardType a5,
                                       typename detail::TC_CallbackParamTraits<A6>::ForwardType a6);

        TC_SharedPtr<detail::TC_BindStateBase> m_bind_state_ptr;
        GenFuncPointer m_invoke;
    };

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    class TC_Callback<R(A1, A2, A3, A4, A5, A6, A7)> {
    private:
        typedef void (*GenFuncPointer)(void);
        typedef GenFuncPointer unspecified_bool_type;
        
    public:

        typedef R (RunType)(A1, A2, A3, A4, A5, A6, A7);
        
        TC_Callback()
            : m_invoke(NULL)
        { }

        template <typename FunctorType, typename BindRunType, typename BoundArgsType>
        TC_Callback(detail::TC_BindState<FunctorType, BindRunType, BoundArgsType>* bind_state)
            : m_bind_state_ptr(bind_state)
        {
            PolymorphicInvoke invoke = &detail::TC_BindState<FunctorType, BindRunType, BoundArgsType>::InvokerType::invoke;
            m_invoke = reinterpret_cast<GenFuncPointer>(invoke);
        }

        R operator()(typename detail::TC_CallbackParamTraits<A1>::ForwardType a1,
                     typename detail::TC_CallbackParamTraits<A2>::ForwardType a2,
                     typename detail::TC_CallbackParamTraits<A3>::ForwardType a3,
                     typename detail::TC_CallbackParamTraits<A4>::ForwardType a4,
                     typename detail::TC_CallbackParamTraits<A5>::ForwardType a5,
                     typename detail::TC_CallbackParamTraits<A6>::ForwardType a6,
                     typename detail::TC_CallbackParamTraits<A7>::ForwardType a7) const
        {
            PolymorphicInvoke f = reinterpret_cast<PolymorphicInvoke>(m_invoke);
            return f(m_bind_state_ptr.get(), a1, a2, a3, a4, a5, a6, a7);
        }

        operator unspecified_bool_type() const
        {
            return m_invoke;
        }
        
    private:

        typedef R (*PolymorphicInvoke)(detail::TC_BindStateBase*,
                                       typename detail::TC_CallbackParamTraits<A1>::ForwardType a1,
                                       typename detail::TC_CallbackParamTraits<A2>::ForwardType a2,
                                       typename detail::TC_CallbackParamTraits<A3>::ForwardType a3,
                                       typename detail::TC_CallbackParamTraits<A4>::ForwardType a4,
                                       typename detail::TC_CallbackParamTraits<A5>::ForwardType a5,
                                       typename detail::TC_CallbackParamTraits<A6>::ForwardType a6,
                                       typename detail::TC_CallbackParamTraits<A7>::ForwardType a7);

        TC_SharedPtr<detail::TC_BindStateBase> m_bind_state_ptr;
        GenFuncPointer m_invoke;
    };

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5,
              typename A6, typename A7, typename A8>
    class TC_Callback<R(A1, A2, A3, A4, A5, A6, A7, A8)> {
    private:
        typedef void (*GenFuncPointer)(void);
        typedef GenFuncPointer unspecified_bool_type;
        
    public:

        typedef R (RunType)(A1, A2, A3, A4, A5, A6, A7, A8);
        
        TC_Callback()
            : m_invoke(NULL)
        { }

        template <typename FunctorType, typename BindRunType, typename BoundArgsType>
        TC_Callback(detail::TC_BindState<FunctorType, BindRunType, BoundArgsType>* bind_state)
            : m_bind_state_ptr(bind_state)
        {
            PolymorphicInvoke invoke = &detail::TC_BindState<FunctorType, BindRunType, BoundArgsType>::InvokerType::invoke;
            m_invoke = reinterpret_cast<GenFuncPointer>(invoke);
        }

        R operator()(typename detail::TC_CallbackParamTraits<A1>::ForwardType a1,
                     typename detail::TC_CallbackParamTraits<A2>::ForwardType a2,
                     typename detail::TC_CallbackParamTraits<A3>::ForwardType a3,
                     typename detail::TC_CallbackParamTraits<A4>::ForwardType a4,
                     typename detail::TC_CallbackParamTraits<A5>::ForwardType a5,
                     typename detail::TC_CallbackParamTraits<A6>::ForwardType a6,
                     typename detail::TC_CallbackParamTraits<A7>::ForwardType a7,
                     typename detail::TC_CallbackParamTraits<A8>::ForwardType a8) const
        {
            PolymorphicInvoke f = reinterpret_cast<PolymorphicInvoke>(m_invoke);
            return f(m_bind_state_ptr.get(), a1, a2, a3, a4, a5, a6, a7, a8);
        }

        operator unspecified_bool_type() const
        {
            return m_invoke;
        }
        
    private:

        typedef R (*PolymorphicInvoke)(detail::TC_BindStateBase*,
                                       typename detail::TC_CallbackParamTraits<A1>::ForwardType a1,
                                       typename detail::TC_CallbackParamTraits<A2>::ForwardType a2,
                                       typename detail::TC_CallbackParamTraits<A3>::ForwardType a3,
                                       typename detail::TC_CallbackParamTraits<A4>::ForwardType a4,
                                       typename detail::TC_CallbackParamTraits<A5>::ForwardType a5,
                                       typename detail::TC_CallbackParamTraits<A6>::ForwardType a6,
                                       typename detail::TC_CallbackParamTraits<A7>::ForwardType a7,
                                       typename detail::TC_CallbackParamTraits<A8>::ForwardType a8);

        TC_SharedPtr<detail::TC_BindStateBase> m_bind_state_ptr;
        GenFuncPointer m_invoke;
    };

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5,
              typename A6, typename A7, typename A8, typename A9>
    class TC_Callback<R(A1, A2, A3, A4, A5, A6, A7, A8, A9)> {
    private:
        typedef void (*GenFuncPointer)(void);
        typedef GenFuncPointer unspecified_bool_type;
        
    public:

        typedef R (RunType)(A1, A2, A3, A4, A5, A6, A7, A8, A9);
        
        TC_Callback()
            : m_invoke(NULL)
        { }

        template <typename FunctorType, typename BindRunType, typename BoundArgsType>
        TC_Callback(detail::TC_BindState<FunctorType, BindRunType, BoundArgsType>* bind_state)
            : m_bind_state_ptr(bind_state)
        {
            PolymorphicInvoke invoke = &detail::TC_BindState<FunctorType, BindRunType, BoundArgsType>::InvokerType::invoke;
            m_invoke = reinterpret_cast<GenFuncPointer>(invoke);
        }

        R operator()(typename detail::TC_CallbackParamTraits<A1>::ForwardType a1,
                     typename detail::TC_CallbackParamTraits<A2>::ForwardType a2,
                     typename detail::TC_CallbackParamTraits<A3>::ForwardType a3,
                     typename detail::TC_CallbackParamTraits<A4>::ForwardType a4,
                     typename detail::TC_CallbackParamTraits<A5>::ForwardType a5,
                     typename detail::TC_CallbackParamTraits<A6>::ForwardType a6,
                     typename detail::TC_CallbackParamTraits<A7>::ForwardType a7,
                     typename detail::TC_CallbackParamTraits<A8>::ForwardType a8,
                     typename detail::TC_CallbackParamTraits<A9>::ForwardType a9) const
        {
            PolymorphicInvoke f = reinterpret_cast<PolymorphicInvoke>(m_invoke);
            return f(m_bind_state_ptr.get(), a1, a2, a3, a4, a5, a6, a7, a8, a9);
        }

        operator unspecified_bool_type() const
        {
            return m_invoke;
        }
        
    private:

        typedef R (*PolymorphicInvoke)(detail::TC_BindStateBase*,
                                       typename detail::TC_CallbackParamTraits<A1>::ForwardType a1,
                                       typename detail::TC_CallbackParamTraits<A2>::ForwardType a2,
                                       typename detail::TC_CallbackParamTraits<A3>::ForwardType a3,
                                       typename detail::TC_CallbackParamTraits<A4>::ForwardType a4,
                                       typename detail::TC_CallbackParamTraits<A5>::ForwardType a5,
                                       typename detail::TC_CallbackParamTraits<A6>::ForwardType a6,
                                       typename detail::TC_CallbackParamTraits<A7>::ForwardType a7,
                                       typename detail::TC_CallbackParamTraits<A8>::ForwardType a8,
                                       typename detail::TC_CallbackParamTraits<A9>::ForwardType a9);

        TC_SharedPtr<detail::TC_BindStateBase> m_bind_state_ptr;
        GenFuncPointer m_invoke;
    };

}

#endif
