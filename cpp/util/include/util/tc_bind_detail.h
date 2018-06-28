#ifndef __TC_BIND_DETAIL_H__
#define __TC_BIND_DETAIL_H__

#include "tc_callback_detail.h"

namespace tars 
{
    namespace detail 
    {
        template <typename Signature>
        class TC_FunctorAdapter;

        template <typename R>
        class TC_FunctorAdapter<R (*)()> {
        public:
            typedef R (RunType)();
            
            explicit TC_FunctorAdapter(R (*f)())
            : m_pf(f)
            { }

            R operator()()
            { return (*m_pf)(); }
    
        private:
            R (*m_pf)();
        };

        template <typename R, typename T>
        class TC_FunctorAdapter<R (T::*)()> {
        public:
            typedef R (RunType)(T*);
            
            explicit TC_FunctorAdapter(R (T::*pmf)())
            : m_pmf(pmf)
            { }

            R operator()(T *pobj)
            { return (pobj->*m_pmf)(); }
        
        private:
            R (T::*m_pmf)();
        };

        template <typename R, typename T>
        class TC_FunctorAdapter<R (T::*)() const> {
        public:
            typedef R (RunType)(const T*);
            
            explicit TC_FunctorAdapter(R (T::*pmf)() const)
            : m_pmf(pmf)
            { }

            R operator()(const T *pobj)
            { return (pobj->*m_pmf)(); }
        
        private:
            R (T::*m_pmf)() const;
        };

        template <typename R, typename A1>
        class TC_FunctorAdapter<R (*)(A1)> {
        public:
            typedef R (RunType)(A1);
            
            TC_FunctorAdapter(R (*pf)(A1))
            : m_pf(pf)
            { }

            R operator()(typename TC_CallbackParamTraits<A1>::ForwardType a1)
            { return (*m_pf)(a1); }

        private:
            R (*m_pf)(A1);
        };

        template <typename R, typename T, typename A1>
        class TC_FunctorAdapter<R (T::*)(A1)> {
        public:
            typedef R (RunType)(T*, A1);
            
            TC_FunctorAdapter(R (T::*pmf)(A1))
            : m_pmf(pmf)
            { }

            R operator()(T *pobj, typename TC_CallbackParamTraits<A1>::ForwardType a1)
            { return (pobj->*m_pmf)(a1); }

        private:
            R (T::*m_pmf)(A1);
        };

        template <typename R, typename T, typename A1>
        class TC_FunctorAdapter<R (T::*)(A1) const> {
        public:
            typedef R (RunType)(const T*, A1);
            
            TC_FunctorAdapter(R (T::*pmf)(A1) const)
            : m_pmf(pmf)
            { }

            R operator()(const T *pobj, typename TC_CallbackParamTraits<A1>::ForwardType a1)
            { return (pobj->*m_pmf)(a1); }

        private:
            R (T::*m_pmf)(A1) const;
        };

        template <typename R, typename A1, typename A2>
        class TC_FunctorAdapter<R (*)(A1, A2)> {
        public:
            typedef R (RunType)(A1, A2);
            
            TC_FunctorAdapter(R (*pf)(A1, A2))
            : m_pf(pf)
            { }

            R operator()(typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2)
            { return (*m_pf)(a1, a2); }

        private:
            R (*m_pf)(A1, A2);
        };

        template <typename R, typename T, typename A1, typename A2>
        class TC_FunctorAdapter<R (T::*)(A1, A2)> {
        public:
            typedef R (RunType)(T*, A1, A2);
            
            TC_FunctorAdapter(R (T::*pmf)(A1, A2))
            : m_pmf(pmf)
            { }

            R operator()(T *pobj,
                         typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2)
            { return (pobj->*m_pmf)(a1, a2); }

        private:
            R (T::*m_pmf)(A1, A2);
        };

        template <typename R, typename T, typename A1, typename A2>
        class TC_FunctorAdapter<R (T::*)(A1, A2) const> {
        public:
            typedef R (RunType)(const T*, A1, A2);
            
            TC_FunctorAdapter(R (T::*pmf)(A1, A2) const)
            : m_pmf(pmf)
            { }

            R operator()(const T *pobj,
                         typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2)
            { return (pobj->*m_pmf)(a1, a2); }

        private:
            R (T::*m_pmf)(A1, A2) const;
        };

        template <typename R, typename A1, typename A2, typename A3>
        class TC_FunctorAdapter<R (*)(A1, A2, A3)> {
        public:
            typedef R (RunType)(A1, A2, A3);
            
            TC_FunctorAdapter(R (*pf)(A1, A2, A3))
            : m_pf(pf)
            { }

            R operator()(typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3)
            { return (*m_pf)(a1, a2, a3); }

        private:
            R (*m_pf)(A1, A2, A3);
        };

        template <typename R, typename T, typename A1, typename A2, typename A3>
        class TC_FunctorAdapter<R (T::*)(A1, A2, A3)> {
        public:
            typedef R (RunType)(T*, A1, A2, A3);
            
            TC_FunctorAdapter(R (T::*pmf)(A1, A2, A3))
            : m_pmf(pmf)
            { }

            R operator()(T *pobj,
                         typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3)
            { return (pobj->*m_pmf)(a1, a2, a3); }

        private:
            R (T::*m_pmf)(A1, A2, A3);
        };

        template <typename R, typename T, typename A1, typename A2, typename A3>
        class TC_FunctorAdapter<R (T::*)(A1, A2, A3) const> {
        public:
            typedef R (RunType)(const T*, A1, A2, A3);
            
            TC_FunctorAdapter(R (T::*pmf)(A1, A2, A3) const)
            : m_pmf(pmf)
            { }

            R operator()(const T *pobj,
                         typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3)
            { return (pobj->*m_pmf)(a1, a2, a3); }

        private:
            R (T::*m_pmf)(A1, A2, A3) const;
        };

        template <typename R, typename A1, typename A2, typename A3, typename A4>
        class TC_FunctorAdapter<R (*)(A1, A2, A3, A4)> {
        public:
            typedef R (RunType)(A1, A2, A3, A4);

            TC_FunctorAdapter(R (*pf)(A1, A2, A3, A4))
            : m_pf(pf)
            { }

            R operator()(typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3,
                         typename TC_CallbackParamTraits<A4>::ForwardType a4)
            { return (*m_pf)(a1, a2, a3, a4); }

        private:
            R (*m_pf)(A1, A2, A3, A4);
        };

        template <typename R, typename T, typename A1, typename A2, typename A3, typename A4>
        class TC_FunctorAdapter<R (T::*)(A1, A2, A3, A4)> {
        public:
            typedef R (RunType)(T*, A1, A2, A3, A4);

            TC_FunctorAdapter(R (T::*pmf)(A1, A2, A3, A4))
            : m_pmf(pmf)
            { }

            R operator()(T *pobj,
                         typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3,
                         typename TC_CallbackParamTraits<A4>::ForwardType a4)
            { return (pobj->*m_pmf)(a1, a2, a3, a4); }

        private:
            R (T::*m_pmf)(A1, A2, A3, A4);
        };

        template <typename R, typename T, typename A1, typename A2, typename A3, typename A4>
        class TC_FunctorAdapter<R (T::*)(A1, A2, A3, A4) const> {
        public:
            typedef R (RunType)(const T*, A1, A2, A3, A4);

            TC_FunctorAdapter(R (T::*pmf)(A1, A2, A3, A4) const)
            : m_pmf(pmf)
            { }

            R operator()(const T *pobj,
                         typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3,
                         typename TC_CallbackParamTraits<A4>::ForwardType a4)
            { return (pobj->*m_pmf)(a1, a2, a3, a4); }

        private:
            R (T::*m_pmf)(A1, A2, A3, A4) const;
        };

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
        class TC_FunctorAdapter<R (*)(A1, A2, A3, A4, A5)> {
        public:
            typedef R (RunType)(A1, A2, A3, A4, A5);

            TC_FunctorAdapter(R (*pf)(A1, A2, A3, A4, A5))
            : m_pf(pf)
            { }

            R operator()(typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3,
                         typename TC_CallbackParamTraits<A4>::ForwardType a4,
                         typename TC_CallbackParamTraits<A5>::ForwardType a5)
            { return (*m_pf)(a1, a2, a3, a4, a5); }

        private:
            R (*m_pf)(A1, A2, A3, A4, A5);
        };

        template <typename R, typename T, typename A1, typename A2, typename A3, typename A4, typename A5>
        class TC_FunctorAdapter<R (T::*)(A1, A2, A3, A4, A5)> {
        public:
            typedef R (RunType)(T*, A1, A2, A3, A4, A5);

            TC_FunctorAdapter(R (T::*pmf)(A1, A2, A3, A4, A5))
            : m_pmf(pmf)
            { }

            R operator()(T *pobj,
                         typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3,
                         typename TC_CallbackParamTraits<A4>::ForwardType a4,
                         typename TC_CallbackParamTraits<A5>::ForwardType a5)
            { return (pobj->*m_pmf)(a1, a2, a3, a4, a5); }

        private:
            R (T::*m_pmf)(A1, A2, A3, A4, A5);
        };

        template <typename R, typename T, typename A1, typename A2, typename A3, typename A4, typename A5>
        class TC_FunctorAdapter<R (T::*)(A1, A2, A3, A4, A5) const> {
        public:
            typedef R (RunType)(const T*, A1, A2, A3, A4, A5);

            TC_FunctorAdapter(R (T::*pmf)(A1, A2, A3, A4, A5) const)
            : m_pmf(pmf)
            { }

            R operator()(const T *pobj,
                         typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3,
                         typename TC_CallbackParamTraits<A4>::ForwardType a4,
                         typename TC_CallbackParamTraits<A5>::ForwardType a5)
            { return (pobj->*m_pmf)(a1, a2, a3, a4, a5); }

        private:
            R (T::*m_pmf)(A1, A2, A3, A4, A5) const;
        };

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        class TC_FunctorAdapter<R (*)(A1, A2, A3, A4, A5, A6)> {
        public:
            typedef R (RunType)(A1, A2, A3, A4, A5, A6);

            TC_FunctorAdapter(R (*pf)(A1, A2, A3, A4, A5, A6))
            : m_pf(pf)
            { }

            R operator()(typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3,
                         typename TC_CallbackParamTraits<A4>::ForwardType a4,
                         typename TC_CallbackParamTraits<A5>::ForwardType a5,
                         typename TC_CallbackParamTraits<A6>::ForwardType a6)
            { return (*m_pf)(a1, a2, a3, a4, a5, a6); }

        private:
            R (*m_pf)(A1, A2, A3, A4, A5, A6);
        };

        template <typename R, typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        class TC_FunctorAdapter<R (T::*)(A1, A2, A3, A4, A5, A6)> {
        public:
            typedef R (RunType)(T*, A1, A2, A3, A4, A5, A6);

            TC_FunctorAdapter(R (T::*pmf)(A1, A2, A3, A4, A5, A6))
            : m_pmf(pmf)
            { }

            R operator()(T *pobj,
                         typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3,
                         typename TC_CallbackParamTraits<A4>::ForwardType a4,
                         typename TC_CallbackParamTraits<A5>::ForwardType a5,
                         typename TC_CallbackParamTraits<A6>::ForwardType a6)
            { return (pobj->*m_pmf)(a1, a2, a3, a4, a5, a6); }

        private:
            R (T::*m_pmf)(A1, A2, A3, A4, A5, A6);
        };

        template <typename R, typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        class TC_FunctorAdapter<R (T::*)(A1, A2, A3, A4, A5, A6) const> {
        public:
            typedef R (RunType)(const T*, A1, A2, A3, A4, A5, A6);

            TC_FunctorAdapter(R (T::*pmf)(A1, A2, A3, A4, A5, A6) const)
            : m_pmf(pmf)
            { }

            R operator()(const T *pobj,
                         typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3,
                         typename TC_CallbackParamTraits<A4>::ForwardType a4,
                         typename TC_CallbackParamTraits<A5>::ForwardType a5,
                         typename TC_CallbackParamTraits<A6>::ForwardType a6)
            { return (pobj->*m_pmf)(a1, a2, a3, a4, a5, a6); }

        private:
            R (T::*m_pmf)(A1, A2, A3, A4, A5, A6) const;
        };

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
        class TC_FunctorAdapter<R (*)(A1, A2, A3, A4, A5, A6, A7)> {
        public:
            typedef R (RunType)(A1, A2, A3, A4, A5, A6, A7);

            TC_FunctorAdapter(R (*pf)(A1, A2, A3, A4, A5, A6, A7))
            : m_pf(pf)
            { }

            R operator()(typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3,
                         typename TC_CallbackParamTraits<A4>::ForwardType a4,
                         typename TC_CallbackParamTraits<A5>::ForwardType a5,
                         typename TC_CallbackParamTraits<A6>::ForwardType a6,
                         typename TC_CallbackParamTraits<A7>::ForwardType a7)
            { return (*m_pf)(a1, a2, a3, a4, a5, a6, a7); }

        private:
            R (*m_pf)(A1, A2, A3, A4, A5, A6, A7);
        };

        template <typename R, typename T, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7>
        class TC_FunctorAdapter<R (T::*)(A1, A2, A3, A4, A5, A6, A7)> {
        public:
            typedef R (RunType)(T*, A1, A2, A3, A4, A5, A6, A7);

            TC_FunctorAdapter(R (T::*pmf)(A1, A2, A3, A4, A5, A6, A7))
            : m_pmf(pmf)
            { }

            R operator()(T *pobj,
                         typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3,
                         typename TC_CallbackParamTraits<A4>::ForwardType a4,
                         typename TC_CallbackParamTraits<A5>::ForwardType a5,
                         typename TC_CallbackParamTraits<A6>::ForwardType a6,
                         typename TC_CallbackParamTraits<A7>::ForwardType a7)
            { return (pobj->*m_pmf)(a1, a2, a3, a4, a5, a6, a7); }

        private:
            R (T::*m_pmf)(A1, A2, A3, A4, A5, A6, A7);
        };

        template <typename R, typename T, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7>
        class TC_FunctorAdapter<R (T::*)(A1, A2, A3, A4, A5, A6, A7) const> {
        public:
            typedef R (RunType)(const T*, A1, A2, A3, A4, A5, A6, A7);

            TC_FunctorAdapter(R (T::*pmf)(A1, A2, A3, A4, A5, A6, A7) const)
            : m_pmf(pmf)
            { }

            R operator()(const T *pobj,
                         typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3,
                         typename TC_CallbackParamTraits<A4>::ForwardType a4,
                         typename TC_CallbackParamTraits<A5>::ForwardType a5,
                         typename TC_CallbackParamTraits<A6>::ForwardType a6,
                         typename TC_CallbackParamTraits<A7>::ForwardType a7)
            { return (pobj->*m_pmf)(a1, a2, a3, a4, a5, a6, a7); }

        private:
            R (T::*m_pmf)(A1, A2, A3, A4, A5, A6, A7) const;
        };

        template <typename R, typename A1, typename A2, typename A3, typename A4,
                  typename A5, typename A6, typename A7, typename A8>
        class TC_FunctorAdapter<R (*)(A1, A2, A3, A4, A5, A6, A7, A8)> {
        public:
            typedef R (RunType)(A1, A2, A3, A4, A5, A6, A7, A8);

            TC_FunctorAdapter(R (*pf)(A1, A2, A3, A4, A5, A6, A7, A8))
            : m_pf(pf)
            { }

            R operator()(typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3,
                         typename TC_CallbackParamTraits<A4>::ForwardType a4,
                         typename TC_CallbackParamTraits<A5>::ForwardType a5,
                         typename TC_CallbackParamTraits<A6>::ForwardType a6,
                         typename TC_CallbackParamTraits<A7>::ForwardType a7,
                         typename TC_CallbackParamTraits<A8>::ForwardType a8)
            { return (*m_pf)(a1, a2, a3, a4, a5, a6, a7, a8); }

        private:
            R (*m_pf)(A1, A2, A3, A4, A5, A6, A7, A8);
        };

        template <typename R, typename T, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8>
        class TC_FunctorAdapter<R (T::*)(A1, A2, A3, A4, A5, A6, A7, A8)> {
        public:
            typedef R (RunType)(T*, A1, A2, A3, A4, A5, A6, A7, A8);

            TC_FunctorAdapter(R (T::*pmf)(A1, A2, A3, A4, A5, A6, A7, A8))
            : m_pmf(pmf)
            { }

            R operator()(T *pobj,
                         typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3,
                         typename TC_CallbackParamTraits<A4>::ForwardType a4,
                         typename TC_CallbackParamTraits<A5>::ForwardType a5,
                         typename TC_CallbackParamTraits<A6>::ForwardType a6,
                         typename TC_CallbackParamTraits<A7>::ForwardType a7,
                         typename TC_CallbackParamTraits<A8>::ForwardType a8)
            { return (pobj->*m_pmf)(a1, a2, a3, a4, a5, a6, a7, a8); }

        private:
            R (T::*m_pmf)(A1, A2, A3, A4, A5, A6, A7, A8);
        };

        template <typename R, typename T, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8>
        class TC_FunctorAdapter<R (T::*)(A1, A2, A3, A4, A5, A6, A7, A8) const> {
        public:
            typedef R (RunType)(const T*, A1, A2, A3, A4, A5, A6, A7, A8);

            TC_FunctorAdapter(R (T::*pmf)(A1, A2, A3, A4, A5, A6, A7, A8) const)
            : m_pmf(pmf)
            { }

            R operator()(const T *pobj,
                         typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3,
                         typename TC_CallbackParamTraits<A4>::ForwardType a4,
                         typename TC_CallbackParamTraits<A5>::ForwardType a5,
                         typename TC_CallbackParamTraits<A6>::ForwardType a6,
                         typename TC_CallbackParamTraits<A7>::ForwardType a7,
                         typename TC_CallbackParamTraits<A8>::ForwardType a8)
            { return (pobj->*m_pmf)(a1, a2, a3, a4, a5, a6, a7, a8); }

        private:
            R (T::*m_pmf)(A1, A2, A3, A4, A5, A6, A7, A8) const;
        };

        template <typename R, typename A1, typename A2, typename A3, typename A4,
                  typename A5, typename A6, typename A7, typename A8, typename A9>
        class TC_FunctorAdapter<R (*)(A1, A2, A3, A4, A5, A6, A7, A8, A9)> {
        public:
            typedef R (RunType)(A1, A2, A3, A4, A5, A6, A7, A8, A9);

            TC_FunctorAdapter(R (*pf)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
            : m_pf(pf)
            { }

            R operator()(typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3,
                         typename TC_CallbackParamTraits<A4>::ForwardType a4,
                         typename TC_CallbackParamTraits<A5>::ForwardType a5,
                         typename TC_CallbackParamTraits<A6>::ForwardType a6,
                         typename TC_CallbackParamTraits<A7>::ForwardType a7,
                         typename TC_CallbackParamTraits<A8>::ForwardType a8,
                         typename TC_CallbackParamTraits<A9>::ForwardType a9)
            { return (*m_pf)(a1, a2, a3, a4, a5, a6, a7, a8, a9); }

        private:
            R (*m_pf)(A1, A2, A3, A4, A5, A6, A7, A8, A9);
        };

        template <typename R, typename T, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        class TC_FunctorAdapter<R (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9)> {
        public:
            typedef R (RunType)(T*, A1, A2, A3, A4, A5, A6, A7, A8, A9);

            TC_FunctorAdapter(R (T::*pmf)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
            : m_pmf(pmf)
            { }

            R operator()(T *pobj,
                         typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3,
                         typename TC_CallbackParamTraits<A4>::ForwardType a4,
                         typename TC_CallbackParamTraits<A5>::ForwardType a5,
                         typename TC_CallbackParamTraits<A6>::ForwardType a6,
                         typename TC_CallbackParamTraits<A7>::ForwardType a7,
                         typename TC_CallbackParamTraits<A8>::ForwardType a8,
                         typename TC_CallbackParamTraits<A9>::ForwardType a9)
            { return (pobj->*m_pmf)(a1, a2, a3, a4, a5, a6, a7, a8, a9); }

        private:
            R (T::*m_pmf)(A1, A2, A3, A4, A5, A6, A7, A8, A9);
        };

        template <typename R, typename T, typename A1, typename A2, typename A3, typename A4,
                  typename A5, typename A6, typename A7, typename A8, typename A9>
        class TC_FunctorAdapter<R (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9) const> {
        public:
            typedef R (RunType)(const T*, A1, A2, A3, A4, A5, A6, A7, A8, A9);

            TC_FunctorAdapter(R (T::*pmf)(A1, A2, A3, A4, A5, A6, A7, A8, A9) const)
            : m_pmf(pmf)
            { }

            R operator()(const T *pobj,
                         typename TC_CallbackParamTraits<A1>::ForwardType a1,
                         typename TC_CallbackParamTraits<A2>::ForwardType a2,
                         typename TC_CallbackParamTraits<A3>::ForwardType a3,
                         typename TC_CallbackParamTraits<A4>::ForwardType a4,
                         typename TC_CallbackParamTraits<A5>::ForwardType a5,
                         typename TC_CallbackParamTraits<A6>::ForwardType a6,
                         typename TC_CallbackParamTraits<A7>::ForwardType a7,
                         typename TC_CallbackParamTraits<A8>::ForwardType a8,
                         typename TC_CallbackParamTraits<A9>::ForwardType a9)
            { return (pobj->*m_pmf)(a1, a2, a3, a4, a5, a6, a7, a8, a9); }

        private:
            R (T::*m_pmf)(A1, A2, A3, A4, A5, A6, A7, A8, A9) const;
        };

        template <typename T>
        class TC_UnretainedWrapper {
        public:
            explicit TC_UnretainedWrapper(T *p) : m_ptr(p) { }
            T *get() const { return m_ptr; }
        private:
            T *m_ptr;
        };

        template <typename T>
        class TC_OwnedWrapper {
        public:
            explicit TC_OwnedWrapper(T *p) : m_ptr(p) { }
            ~TC_OwnedWrapper() 
            { 
                if(m_ptr != NULL)
                {
                    delete m_ptr; 
                    m_ptr = NULL;
                }
            }
            TC_OwnedWrapper(const TC_OwnedWrapper& o)
            {
                m_ptr = o.m_ptr;
                o.m_ptr = NULL;
            }
            TC_OwnedWrapper& operator=(const TC_OwnedWrapper& o)
            {
                if(m_ptr != NULL)
                {
                    delete m_ptr;
                    m_ptr = NULL;
                }
                m_ptr = o.m_ptr;
                o.m_ptr = NULL;
            }
            T *get() const { return m_ptr; }
            
        private:
            mutable T *m_ptr;
        };
        
        template <typename T>
        struct TC_SharedWrapper {
        public:
            explicit TC_SharedWrapper(const TC_SharedPtr<T>& p)
                : m_ptr(p)
            { }
            T *get() const { return m_ptr.get(); }
        private:
            TC_SharedPtr<T> m_ptr;
        };
        
        template <typename T>
        struct TC_UnwrapTraits {
            typedef const T& ForwardType;
            static ForwardType unwrap(const T& o) { return o; }
        };
        
        template <typename T>
        struct TC_UnwrapTraits<TC_UnretainedWrapper<T> > {
            typedef T* ForwardType;
            static ForwardType unwrap(const TC_UnretainedWrapper<T>& unretained)
            { return unretained.get(); }
        };

        template <typename T>
        struct TC_UnwrapTraits<TC_OwnedWrapper<T> > {
            typedef T* ForwardType;
            static ForwardType unwrap(const TC_OwnedWrapper<T>& owned)
            { return owned.get(); }
        };

        template <typename T>
        struct TC_UnwrapTraits<TC_SharedWrapper<T> > {
            typedef T* ForwardType;
            static ForwardType unwrap(const TC_SharedWrapper<T>& shared)
            { return shared.get(); }
        };
        
        template <typename TC_BindState, typename RunType, int NumBound>
        class TC_Invoker;

        template <typename TC_BindState, typename R>
        class TC_Invoker<TC_BindState, R(), 0> {
        public:
            typedef R (UnboundRunType)();
            
            static R invoke(TC_BindStateBase *base)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                return (bind_state->m_functor)();
            }
        };

        template <typename TC_BindState, typename R, typename A1>
        class TC_Invoker<TC_BindState, R(A1), 0> {
        public:

            typedef R (UnboundRunType)(A1);
            
            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A1>::ForwardType a1)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                return (bind_state->m_functor)(a1);
            }
        };

        template <typename TC_BindState, typename R, typename A1>
        class TC_Invoker<TC_BindState, R(A1), 1> {
        public:

            typedef R (UnboundRunType)();
            
            static R invoke(TC_BindStateBase *base)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);

                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                return (bind_state->m_functor)(a1);
            }
        };
        
        template <typename TC_BindState, typename R, typename A1, typename A2>
        class TC_Invoker<TC_BindState, R(A1, A2), 0> {
        public:

            typedef R (UnboundRunType)(A1, A2);
            
            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A1>::ForwardType a1,
                            typename TC_CallbackParamTraits<A2>::ForwardType a2)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                return (bind_state->m_functor)(a1, a2);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2>
        class TC_Invoker<TC_BindState, R(A1, A2), 1> {
        public:

            typedef R (UnboundRunType)(A2);
            
            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A2>::ForwardType a2)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);

                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                return (bind_state->m_functor)(a1, a2);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2>
        class TC_Invoker<TC_BindState, R(A1, A2), 2> {
        public:

            typedef R (UnboundRunType)();
            
            static R invoke(TC_BindStateBase *base)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);

                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                return (bind_state->m_functor)(a1, a2);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3>
        class TC_Invoker<TC_BindState, R(A1, A2, A3), 0> {
        public:

            typedef R (UnboundRunType)(A1, A2, A3);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A1>::ForwardType a1,
                            typename TC_CallbackParamTraits<A2>::ForwardType a2,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
        
                return (bind_state->m_functor)(a1, a2, a3);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3>
        class TC_Invoker<TC_BindState, R(A1, A2, A3), 1> {
        public:

            typedef R (UnboundRunType)(A2, A3);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A2>::ForwardType a2,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                return (bind_state->m_functor)(a1, a2, a3);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3>
        class TC_Invoker<TC_BindState, R(A1, A2, A3), 2> {
        public:

            typedef R (UnboundRunType)(A3);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                return (bind_state->m_functor)(a1, a2, a3);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3>
        class TC_Invoker<TC_BindState, R(A1, A2, A3), 3> {
        public:

            typedef R (UnboundRunType)();

            static R invoke(TC_BindStateBase *base)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                return (bind_state->m_functor)(a1, a2, a3);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3, typename A4>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4), 0> {
        public:

            typedef R (UnboundRunType)(A1, A2, A3, A4);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A1>::ForwardType a1,
                            typename TC_CallbackParamTraits<A2>::ForwardType a2,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
        
                return (bind_state->m_functor)(a1, a2, a3, a4);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3, typename A4>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4), 1> {
        public:

            typedef R (UnboundRunType)(A2, A3, A4);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A2>::ForwardType a2,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                return (bind_state->m_functor)(a1, a2, a3, a4);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3, typename A4>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4), 2> {
        public:

            typedef R (UnboundRunType)(A3, A4);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                return (bind_state->m_functor)(a1, a2, a3, a4);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3, typename A4>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4), 3> {
        public:

            typedef R (UnboundRunType)(A4);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                return (bind_state->m_functor)(a1, a2, a3, a4);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3, typename A4>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4), 4> {
        public:

            typedef R (UnboundRunType)();

            static R invoke(TC_BindStateBase *base)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                return (bind_state->m_functor)(a1, a2, a3, a4);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5), 0> {
        public:

            typedef R (UnboundRunType)(A1, A2, A3, A4, A5);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A1>::ForwardType a1,
                            typename TC_CallbackParamTraits<A2>::ForwardType a2,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
        
                return (bind_state->m_functor)(a1, a2, a3, a4, a5);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5), 1> {
        public:

            typedef R (UnboundRunType)(A2, A3, A4, A5);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A2>::ForwardType a2,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5), 2> {
        public:

            typedef R (UnboundRunType)(A3, A4, A5);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5), 3> {
        public:

            typedef R (UnboundRunType)(A4, A5);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5), 4> {
        public:

            typedef R (UnboundRunType)(A5);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5), 5> {
        public:

            typedef R (UnboundRunType)();

            static R invoke(TC_BindStateBase *base)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typedef typename TC_BindState::Bound5UnwrapTraits Bound5UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                typename Bound5UnwrapTraits::ForwardType a5 = Bound5UnwrapTraits::unwrap(bind_state->m_a5);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6), 0> {
        public:

            typedef R (UnboundRunType)(A1, A2, A3, A4, A5, A6);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A1>::ForwardType a1,
                            typename TC_CallbackParamTraits<A2>::ForwardType a2,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
        
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6), 1> {
        public:

            typedef R (UnboundRunType)(A2, A3, A4, A5, A6);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A2>::ForwardType a2,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6), 2> {
        public:

            typedef R (UnboundRunType)(A3, A4, A5, A6);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6), 3> {
        public:

            typedef R (UnboundRunType)(A4, A5, A6);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6), 4> {
        public:

            typedef R (UnboundRunType)(A5, A6);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6), 5> {
        public:

            typedef R (UnboundRunType)(A6);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typedef typename TC_BindState::Bound5UnwrapTraits Bound5UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                typename Bound5UnwrapTraits::ForwardType a5 = Bound5UnwrapTraits::unwrap(bind_state->m_a5);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6), 6> {
        public:

            typedef R (UnboundRunType)();

            static R invoke(TC_BindStateBase *base)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typedef typename TC_BindState::Bound5UnwrapTraits Bound5UnwrapTraits;
                typedef typename TC_BindState::Bound6UnwrapTraits Bound6UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                typename Bound5UnwrapTraits::ForwardType a5 = Bound5UnwrapTraits::unwrap(bind_state->m_a5);
                typename Bound6UnwrapTraits::ForwardType a6 = Bound6UnwrapTraits::unwrap(bind_state->m_a6);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7), 0> {
        public:

            typedef R (UnboundRunType)(A1, A2, A3, A4, A5, A6, A7);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A1>::ForwardType a1,
                            typename TC_CallbackParamTraits<A2>::ForwardType a2,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
        
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7), 1> {
        public:

            typedef R (UnboundRunType)(A2, A3, A4, A5, A6, A7);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A2>::ForwardType a2,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7), 2> {
        public:

            typedef R (UnboundRunType)(A3, A4, A5, A6, A7);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7), 3> {
        public:

            typedef R (UnboundRunType)(A4, A5, A6, A7);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7), 4> {
        public:

            typedef R (UnboundRunType)(A5, A6, A7);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7), 5> {
        public:

            typedef R (UnboundRunType)(A6, A7);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typedef typename TC_BindState::Bound5UnwrapTraits Bound5UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                typename Bound5UnwrapTraits::ForwardType a5 = Bound5UnwrapTraits::unwrap(bind_state->m_a5);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7), 6> {
        public:

            typedef R (UnboundRunType)(A7);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typedef typename TC_BindState::Bound5UnwrapTraits Bound5UnwrapTraits;
                typedef typename TC_BindState::Bound6UnwrapTraits Bound6UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                typename Bound5UnwrapTraits::ForwardType a5 = Bound5UnwrapTraits::unwrap(bind_state->m_a5);
                typename Bound6UnwrapTraits::ForwardType a6 = Bound6UnwrapTraits::unwrap(bind_state->m_a6);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7), 7> {
        public:

            typedef R (UnboundRunType)();

            static R invoke(TC_BindStateBase *base)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typedef typename TC_BindState::Bound5UnwrapTraits Bound5UnwrapTraits;
                typedef typename TC_BindState::Bound6UnwrapTraits Bound6UnwrapTraits;
                typedef typename TC_BindState::Bound7UnwrapTraits Bound7UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                typename Bound5UnwrapTraits::ForwardType a5 = Bound5UnwrapTraits::unwrap(bind_state->m_a5);
                typename Bound6UnwrapTraits::ForwardType a6 = Bound6UnwrapTraits::unwrap(bind_state->m_a6);
                typename Bound7UnwrapTraits::ForwardType a7 = Bound7UnwrapTraits::unwrap(bind_state->m_a7);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7, A8), 0> {
        public:

            typedef R (UnboundRunType)(A1, A2, A3, A4, A5, A6, A7, A8);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A1>::ForwardType a1,
                            typename TC_CallbackParamTraits<A2>::ForwardType a2,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7,
                            typename TC_CallbackParamTraits<A8>::ForwardType a8)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
        
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7, a8);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7, A8), 1> {
        public:

            typedef R (UnboundRunType)(A2, A3, A4, A5, A6, A7, A8);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A2>::ForwardType a2,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7,
                            typename TC_CallbackParamTraits<A8>::ForwardType a8)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7, a8);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7, A8), 2> {
        public:

            typedef R (UnboundRunType)(A3, A4, A5, A6, A7, A8);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7,
                            typename TC_CallbackParamTraits<A8>::ForwardType a8)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7, a8);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7, A8), 3> {
        public:

            typedef R (UnboundRunType)(A4, A5, A6, A7, A8);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7,
                            typename TC_CallbackParamTraits<A8>::ForwardType a8)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7, a8);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7, A8), 4> {
        public:

            typedef R (UnboundRunType)(A5, A6, A7, A8);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7,
                            typename TC_CallbackParamTraits<A8>::ForwardType a8)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7, a8);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7, A8), 5> {
        public:

            typedef R (UnboundRunType)(A6, A7, A8);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7,
                            typename TC_CallbackParamTraits<A8>::ForwardType a8)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typedef typename TC_BindState::Bound5UnwrapTraits Bound5UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                typename Bound5UnwrapTraits::ForwardType a5 = Bound5UnwrapTraits::unwrap(bind_state->m_a5);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7, a8);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7, A8), 6> {
        public:

            typedef R (UnboundRunType)(A7, A8);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7,
                            typename TC_CallbackParamTraits<A8>::ForwardType a8)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typedef typename TC_BindState::Bound5UnwrapTraits Bound5UnwrapTraits;
                typedef typename TC_BindState::Bound6UnwrapTraits Bound6UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                typename Bound5UnwrapTraits::ForwardType a5 = Bound5UnwrapTraits::unwrap(bind_state->m_a5);
                typename Bound6UnwrapTraits::ForwardType a6 = Bound6UnwrapTraits::unwrap(bind_state->m_a6);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7, a8);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7, A8), 7> {
        public:

            typedef R (UnboundRunType)(A8);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A8>::ForwardType a8)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typedef typename TC_BindState::Bound5UnwrapTraits Bound5UnwrapTraits;
                typedef typename TC_BindState::Bound6UnwrapTraits Bound6UnwrapTraits;
                typedef typename TC_BindState::Bound7UnwrapTraits Bound7UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                typename Bound5UnwrapTraits::ForwardType a5 = Bound5UnwrapTraits::unwrap(bind_state->m_a5);
                typename Bound6UnwrapTraits::ForwardType a6 = Bound6UnwrapTraits::unwrap(bind_state->m_a6);
                typename Bound7UnwrapTraits::ForwardType a7 = Bound7UnwrapTraits::unwrap(bind_state->m_a7);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7, a8);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7, A8), 8> {
        public:

            typedef R (UnboundRunType)();

            static R invoke(TC_BindStateBase *base)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typedef typename TC_BindState::Bound5UnwrapTraits Bound5UnwrapTraits;
                typedef typename TC_BindState::Bound6UnwrapTraits Bound6UnwrapTraits;
                typedef typename TC_BindState::Bound7UnwrapTraits Bound7UnwrapTraits;
                typedef typename TC_BindState::Bound8UnwrapTraits Bound8UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                typename Bound5UnwrapTraits::ForwardType a5 = Bound5UnwrapTraits::unwrap(bind_state->m_a5);
                typename Bound6UnwrapTraits::ForwardType a6 = Bound6UnwrapTraits::unwrap(bind_state->m_a6);
                typename Bound7UnwrapTraits::ForwardType a7 = Bound7UnwrapTraits::unwrap(bind_state->m_a7);
                typename Bound8UnwrapTraits::ForwardType a8 = Bound8UnwrapTraits::unwrap(bind_state->m_a8);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7, a8);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7, A8, A9), 0> {
        public:

            typedef R (UnboundRunType)(A1, A2, A3, A4, A5, A6, A7, A8, A9);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A1>::ForwardType a1,
                            typename TC_CallbackParamTraits<A2>::ForwardType a2,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7,
                            typename TC_CallbackParamTraits<A8>::ForwardType a8,
                            typename TC_CallbackParamTraits<A9>::ForwardType a9)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
        
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7, a8, a9);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7, A8, A9), 1> {
        public:

            typedef R (UnboundRunType)(A2, A3, A4, A5, A6, A7, A8, A9);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A2>::ForwardType a2,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7,
                            typename TC_CallbackParamTraits<A8>::ForwardType a8,
                            typename TC_CallbackParamTraits<A9>::ForwardType a9)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7, a8, a9);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7, A8, A9), 2> {
        public:

            typedef R (UnboundRunType)(A3, A4, A5, A6, A7, A8, A9);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A3>::ForwardType a3,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7,
                            typename TC_CallbackParamTraits<A8>::ForwardType a8,
                            typename TC_CallbackParamTraits<A9>::ForwardType a9)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7, a8, a9);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7, A8, A9), 3> {
        public:

            typedef R (UnboundRunType)(A4, A5, A6, A7, A8, A9);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A4>::ForwardType a4,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7,
                            typename TC_CallbackParamTraits<A8>::ForwardType a8,
                            typename TC_CallbackParamTraits<A9>::ForwardType a9)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7, a8, a9);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7, A8, A9), 4> {
        public:

            typedef R (UnboundRunType)(A5, A6, A7, A8, A9);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A5>::ForwardType a5,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7,
                            typename TC_CallbackParamTraits<A8>::ForwardType a8,
                            typename TC_CallbackParamTraits<A9>::ForwardType a9)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7, a8, a9);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7, A8, A9), 5> {
        public:

            typedef R (UnboundRunType)(A6, A7, A8, A9);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A6>::ForwardType a6,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7,
                            typename TC_CallbackParamTraits<A8>::ForwardType a8,
                            typename TC_CallbackParamTraits<A9>::ForwardType a9)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typedef typename TC_BindState::Bound5UnwrapTraits Bound5UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                typename Bound5UnwrapTraits::ForwardType a5 = Bound5UnwrapTraits::unwrap(bind_state->m_a5);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7, a8, a9);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7, A8, A9), 6> {
        public:

            typedef R (UnboundRunType)(A7, A8, A9);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A7>::ForwardType a7,
                            typename TC_CallbackParamTraits<A8>::ForwardType a8,
                            typename TC_CallbackParamTraits<A9>::ForwardType a9)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typedef typename TC_BindState::Bound5UnwrapTraits Bound5UnwrapTraits;
                typedef typename TC_BindState::Bound6UnwrapTraits Bound6UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                typename Bound5UnwrapTraits::ForwardType a5 = Bound5UnwrapTraits::unwrap(bind_state->m_a5);
                typename Bound6UnwrapTraits::ForwardType a6 = Bound6UnwrapTraits::unwrap(bind_state->m_a6);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7, a8, a9);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7, A8, A9), 7> {
        public:

            typedef R (UnboundRunType)(A8, A9);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A8>::ForwardType a8,
                            typename TC_CallbackParamTraits<A9>::ForwardType a9)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typedef typename TC_BindState::Bound5UnwrapTraits Bound5UnwrapTraits;
                typedef typename TC_BindState::Bound6UnwrapTraits Bound6UnwrapTraits;
                typedef typename TC_BindState::Bound7UnwrapTraits Bound7UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                typename Bound5UnwrapTraits::ForwardType a5 = Bound5UnwrapTraits::unwrap(bind_state->m_a5);
                typename Bound6UnwrapTraits::ForwardType a6 = Bound6UnwrapTraits::unwrap(bind_state->m_a6);
                typename Bound7UnwrapTraits::ForwardType a7 = Bound7UnwrapTraits::unwrap(bind_state->m_a7);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7, a8, a9);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7, A8, A9), 8> {
        public:

            typedef R (UnboundRunType)(A9);

            static R invoke(TC_BindStateBase *base,
                            typename TC_CallbackParamTraits<A9>::ForwardType a9)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typedef typename TC_BindState::Bound5UnwrapTraits Bound5UnwrapTraits;
                typedef typename TC_BindState::Bound6UnwrapTraits Bound6UnwrapTraits;
                typedef typename TC_BindState::Bound7UnwrapTraits Bound7UnwrapTraits;
                typedef typename TC_BindState::Bound8UnwrapTraits Bound8UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                typename Bound5UnwrapTraits::ForwardType a5 = Bound5UnwrapTraits::unwrap(bind_state->m_a5);
                typename Bound6UnwrapTraits::ForwardType a6 = Bound6UnwrapTraits::unwrap(bind_state->m_a6);
                typename Bound7UnwrapTraits::ForwardType a7 = Bound7UnwrapTraits::unwrap(bind_state->m_a7);
                typename Bound8UnwrapTraits::ForwardType a8 = Bound8UnwrapTraits::unwrap(bind_state->m_a8);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7, a8, a9);
            }
        };

        template <typename TC_BindState, typename R, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        class TC_Invoker<TC_BindState, R(A1, A2, A3, A4, A5, A6, A7, A8, A9), 9> {
        public:

            typedef R (UnboundRunType)();

            static R invoke(TC_BindStateBase *base)
            {
                TC_BindState *bind_state = static_cast<TC_BindState*>(base);
                typedef typename TC_BindState::Bound1UnwrapTraits Bound1UnwrapTraits;
                typedef typename TC_BindState::Bound2UnwrapTraits Bound2UnwrapTraits;
                typedef typename TC_BindState::Bound3UnwrapTraits Bound3UnwrapTraits;
                typedef typename TC_BindState::Bound4UnwrapTraits Bound4UnwrapTraits;
                typedef typename TC_BindState::Bound5UnwrapTraits Bound5UnwrapTraits;
                typedef typename TC_BindState::Bound6UnwrapTraits Bound6UnwrapTraits;
                typedef typename TC_BindState::Bound7UnwrapTraits Bound7UnwrapTraits;
                typedef typename TC_BindState::Bound8UnwrapTraits Bound8UnwrapTraits;
                typedef typename TC_BindState::Bound9UnwrapTraits Bound9UnwrapTraits;
                typename Bound1UnwrapTraits::ForwardType a1 = Bound1UnwrapTraits::unwrap(bind_state->m_a1);
                typename Bound2UnwrapTraits::ForwardType a2 = Bound2UnwrapTraits::unwrap(bind_state->m_a2);
                typename Bound3UnwrapTraits::ForwardType a3 = Bound3UnwrapTraits::unwrap(bind_state->m_a3);
                typename Bound4UnwrapTraits::ForwardType a4 = Bound4UnwrapTraits::unwrap(bind_state->m_a4);
                typename Bound5UnwrapTraits::ForwardType a5 = Bound5UnwrapTraits::unwrap(bind_state->m_a5);
                typename Bound6UnwrapTraits::ForwardType a6 = Bound6UnwrapTraits::unwrap(bind_state->m_a6);
                typename Bound7UnwrapTraits::ForwardType a7 = Bound7UnwrapTraits::unwrap(bind_state->m_a7);
                typename Bound8UnwrapTraits::ForwardType a8 = Bound8UnwrapTraits::unwrap(bind_state->m_a8);
                typename Bound9UnwrapTraits::ForwardType a9 = Bound9UnwrapTraits::unwrap(bind_state->m_a9);
                return (bind_state->m_functor)(a1, a2, a3, a4, a5, a6, a7, a8, a9);
            }
        };

        template <typename FunctorType, typename RunType, typename BoundArgsType>
        struct TC_BindState;
        
        template <typename FunctorType, typename RunType>
        class TC_BindState<FunctorType, RunType, void()> : public TC_BindStateBase {
        public:
            typedef TC_Invoker<TC_BindState, RunType, 0> InvokerType;
            typedef typename InvokerType::UnboundRunType UnboundRunType;
            
            explicit TC_BindState(const FunctorType& functor)
                : m_functor(functor)
            { }

            FunctorType m_functor;
        };

        template <typename FunctorType, typename RunType, typename A1>
        class TC_BindState<FunctorType, RunType, void(A1)> : public TC_BindStateBase {
        public:
            typedef TC_Invoker<TC_BindState, RunType, 1> InvokerType;
            typedef typename InvokerType::UnboundRunType UnboundRunType;
            typedef TC_UnwrapTraits<A1> Bound1UnwrapTraits;
            
            TC_BindState(const FunctorType& functor, const A1& a1)
                : m_functor(functor),
                  m_a1(a1)
            { }

            FunctorType m_functor;
            A1 m_a1;
        };

        template <typename FunctorType, typename RunType, typename A1, typename A2>
        class TC_BindState<FunctorType, RunType, void(A1, A2)> : public TC_BindStateBase {
        public:
            typedef TC_Invoker<TC_BindState, RunType, 2> InvokerType;
            typedef typename InvokerType::UnboundRunType UnboundRunType;
            typedef TC_UnwrapTraits<A1> Bound1UnwrapTraits;
            typedef TC_UnwrapTraits<A2> Bound2UnwrapTraits;
            
            TC_BindState(const FunctorType& functor, const A1& a1, const A2& a2)
                : m_functor(functor),
                  m_a1(a1),
                  m_a2(a2)
            { }

            FunctorType m_functor;
            A1 m_a1;
            A2 m_a2;
        };

        template <typename FunctorType, typename RunType, typename A1, typename A2, typename A3>
        class TC_BindState<FunctorType, RunType, void(A1, A2, A3)> : public TC_BindStateBase {
        public:
            typedef TC_Invoker<TC_BindState, RunType, 3> InvokerType;
            typedef typename InvokerType::UnboundRunType UnboundRunType;
            typedef TC_UnwrapTraits<A1> Bound1UnwrapTraits;
            typedef TC_UnwrapTraits<A2> Bound2UnwrapTraits;
            typedef TC_UnwrapTraits<A3> Bound3UnwrapTraits;

            TC_BindState(const FunctorType& functor, const A1& a1, const A2& a2, const A3& a3)
                : m_functor(functor),
                  m_a1(a1),
                  m_a2(a2),
                  m_a3(a3)
            { }

            FunctorType m_functor;
            A1 m_a1;
            A2 m_a2;
            A3 m_a3;
        };

        template <typename FunctorType, typename RunType, typename A1, typename A2, typename A3,
                  typename A4>
        class TC_BindState<FunctorType, RunType, void(A1, A2, A3, A4)> : public TC_BindStateBase {
        public:
            typedef TC_Invoker<TC_BindState, RunType, 4> InvokerType;
            typedef typename InvokerType::UnboundRunType UnboundRunType;
            typedef TC_UnwrapTraits<A1> Bound1UnwrapTraits;
            typedef TC_UnwrapTraits<A2> Bound2UnwrapTraits;
            typedef TC_UnwrapTraits<A3> Bound3UnwrapTraits;
            typedef TC_UnwrapTraits<A4> Bound4UnwrapTraits;

            TC_BindState(const FunctorType& functor, const A1& a1, const A2& a2, const A3& a3, const A4& a4)
                : m_functor(functor),
                  m_a1(a1),
                  m_a2(a2),
                  m_a3(a3),
                  m_a4(a4)
            { }

            FunctorType m_functor;
            A1 m_a1;
            A2 m_a2;
            A3 m_a3;
            A4 m_a4;
        };

        template <typename FunctorType, typename RunType, typename A1, typename A2, typename A3,
                  typename A4, typename A5>
        class TC_BindState<FunctorType, RunType, void(A1, A2, A3, A4, A5)> : public TC_BindStateBase {
        public:
            typedef TC_Invoker<TC_BindState, RunType, 5> InvokerType;
            typedef typename InvokerType::UnboundRunType UnboundRunType;
            typedef TC_UnwrapTraits<A1> Bound1UnwrapTraits;
            typedef TC_UnwrapTraits<A2> Bound2UnwrapTraits;
            typedef TC_UnwrapTraits<A3> Bound3UnwrapTraits;
            typedef TC_UnwrapTraits<A4> Bound4UnwrapTraits;
            typedef TC_UnwrapTraits<A5> Bound5UnwrapTraits;

            TC_BindState(const FunctorType& functor, const A1& a1, const A2& a2, const A3& a3,
                      const A4& a4, const A5& a5)
                : m_functor(functor),
                  m_a1(a1),
                  m_a2(a2),
                  m_a3(a3),
                  m_a4(a4),
                  m_a5(a5)
            { }

            FunctorType m_functor;
            A1 m_a1;
            A2 m_a2;
            A3 m_a3;
            A4 m_a4;
            A5 m_a5;
        };

        template <typename FunctorType, typename RunType, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6>
        class TC_BindState<FunctorType, RunType, void(A1, A2, A3, A4, A5, A6)> : public TC_BindStateBase {
        public:
            typedef TC_Invoker<TC_BindState, RunType, 6> InvokerType;
            typedef typename InvokerType::UnboundRunType UnboundRunType;
            typedef TC_UnwrapTraits<A1> Bound1UnwrapTraits;
            typedef TC_UnwrapTraits<A2> Bound2UnwrapTraits;
            typedef TC_UnwrapTraits<A3> Bound3UnwrapTraits;
            typedef TC_UnwrapTraits<A4> Bound4UnwrapTraits;
            typedef TC_UnwrapTraits<A5> Bound5UnwrapTraits;
            typedef TC_UnwrapTraits<A6> Bound6UnwrapTraits;

            TC_BindState(const FunctorType& functor, const A1& a1, const A2& a2, const A3& a3,
                      const A4& a4, const A5& a5, const A6& a6)
                : m_functor(functor),
                  m_a1(a1),
                  m_a2(a2),
                  m_a3(a3),
                  m_a4(a4),
                  m_a5(a5),
                  m_a6(a6)
            { }

            FunctorType m_functor;
            A1 m_a1;
            A2 m_a2;
            A3 m_a3;
            A4 m_a4;
            A5 m_a5;
            A6 m_a6;
        };

        template <typename FunctorType, typename RunType, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7>
        class TC_BindState<FunctorType, RunType, void(A1, A2, A3, A4, A5, A6, A7)> : public TC_BindStateBase {
        public:
            typedef TC_Invoker<TC_BindState, RunType, 7> InvokerType;
            typedef typename InvokerType::UnboundRunType UnboundRunType;
            typedef TC_UnwrapTraits<A1> Bound1UnwrapTraits;
            typedef TC_UnwrapTraits<A2> Bound2UnwrapTraits;
            typedef TC_UnwrapTraits<A3> Bound3UnwrapTraits;
            typedef TC_UnwrapTraits<A4> Bound4UnwrapTraits;
            typedef TC_UnwrapTraits<A5> Bound5UnwrapTraits;
            typedef TC_UnwrapTraits<A6> Bound6UnwrapTraits;
            typedef TC_UnwrapTraits<A7> Bound7UnwrapTraits;

            TC_BindState(const FunctorType& functor, const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                      const A5& a5, const A6& a6, const A7& a7)
                : m_functor(functor),
                  m_a1(a1),
                  m_a2(a2),
                  m_a3(a3),
                  m_a4(a4),
                  m_a5(a5),
                  m_a6(a6),
                  m_a7(a7)
            { }

            FunctorType m_functor;
            A1 m_a1;
            A2 m_a2;
            A3 m_a3;
            A4 m_a4;
            A5 m_a5;
            A6 m_a6;
            A7 m_a7;
        };

        template <typename FunctorType, typename RunType, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8>
        class TC_BindState<FunctorType, RunType, void(A1, A2, A3, A4, A5, A6, A7, A8)> : public TC_BindStateBase {
        public:
            typedef TC_Invoker<TC_BindState, RunType, 8> InvokerType;
            typedef typename InvokerType::UnboundRunType UnboundRunType;
            typedef TC_UnwrapTraits<A1> Bound1UnwrapTraits;
            typedef TC_UnwrapTraits<A2> Bound2UnwrapTraits;
            typedef TC_UnwrapTraits<A3> Bound3UnwrapTraits;
            typedef TC_UnwrapTraits<A4> Bound4UnwrapTraits;
            typedef TC_UnwrapTraits<A5> Bound5UnwrapTraits;
            typedef TC_UnwrapTraits<A6> Bound6UnwrapTraits;
            typedef TC_UnwrapTraits<A7> Bound7UnwrapTraits;
            typedef TC_UnwrapTraits<A8> Bound8UnwrapTraits;

            TC_BindState(const FunctorType& functor, const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                      const A5& a5, const A6& a6, const A7& a7, const A8& a8)
                : m_functor(functor),
                  m_a1(a1),
                  m_a2(a2),
                  m_a3(a3),
                  m_a4(a4),
                  m_a5(a5),
                  m_a6(a6),
                  m_a7(a7),
                  m_a8(a8)
            { }

            FunctorType m_functor;
            A1 m_a1;
            A2 m_a2;
            A3 m_a3;
            A4 m_a4;
            A5 m_a5;
            A6 m_a6;
            A7 m_a7;
            A8 m_a8;
        };

        template <typename FunctorType, typename RunType, typename A1, typename A2, typename A3,
                  typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        class TC_BindState<FunctorType, RunType, void(A1, A2, A3, A4, A5, A6, A7, A8, A9)> : public TC_BindStateBase {
        public:
            typedef TC_Invoker<TC_BindState, RunType, 9> InvokerType;
            typedef typename InvokerType::UnboundRunType UnboundRunType;
            typedef TC_UnwrapTraits<A1> Bound1UnwrapTraits;
            typedef TC_UnwrapTraits<A2> Bound2UnwrapTraits;
            typedef TC_UnwrapTraits<A3> Bound3UnwrapTraits;
            typedef TC_UnwrapTraits<A4> Bound4UnwrapTraits;
            typedef TC_UnwrapTraits<A5> Bound5UnwrapTraits;
            typedef TC_UnwrapTraits<A6> Bound6UnwrapTraits;
            typedef TC_UnwrapTraits<A7> Bound7UnwrapTraits;
            typedef TC_UnwrapTraits<A8> Bound8UnwrapTraits;
            typedef TC_UnwrapTraits<A9> Bound9UnwrapTraits;

            TC_BindState(const FunctorType& functor, const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                      const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
                : m_functor(functor),
                  m_a1(a1),
                  m_a2(a2),
                  m_a3(a3),
                  m_a4(a4),
                  m_a5(a5),
                  m_a6(a6),
                  m_a7(a7),
                  m_a8(a8),
                  m_a9(a9)
            { }

            FunctorType m_functor;
            A1 m_a1;
            A2 m_a2;
            A3 m_a3;
            A4 m_a4;
            A5 m_a5;
            A6 m_a6;
            A7 m_a7;
            A8 m_a8;
            A9 m_a9;
        };

        template <typename T>
        struct TC_FunctorTraits {
            typedef TC_FunctorAdapter<T> FunctorType;
            typedef typename FunctorType::RunType RunType;
        };

        template <typename T>
        struct TC_FunctorTraits<TC_Callback<T> > {
            typedef TC_Callback<T> FunctorType;
            typedef typename TC_Callback<T>::RunType RunType;
        };

        template <typename Signature>
        struct TC_FunctionTraits;

        template <typename R>
        struct TC_FunctionTraits<R()> {
            typedef R ReturnType;
        };

        template <typename R, typename A1>
        struct TC_FunctionTraits<R(A1)> {
            typedef R ReturnType;
            typedef A1 A1Type;
        };

        template <typename R, typename A1, typename A2>
        struct TC_FunctionTraits<R(A1, A2)> {
            typedef R ReturnType;
            typedef A1 A1Type;
            typedef A2 A2Type;
        };

        template <typename R, typename A1, typename A2, typename A3>
        struct TC_FunctionTraits<R(A1, A2, A3)> {
            typedef R ReturnType;
            typedef A1 A1Type;
            typedef A2 A2Type;
            typedef A3 A3Type;
        };

        template <typename R, typename A1, typename A2, typename A3, typename A4>
        struct TC_FunctionTraits<R(A1, A2, A3, A4)> {
            typedef R ReturnType;
            typedef A1 A1Type;
            typedef A2 A2Type;
            typedef A3 A3Type;
            typedef A4 A4Type;
        };

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
        struct TC_FunctionTraits<R(A1, A2, A3, A4, A5)> {
            typedef R ReturnType;
            typedef A1 A1Type;
            typedef A2 A2Type;
            typedef A3 A3Type;
            typedef A4 A4Type;
            typedef A5 A5Type;
        };

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        struct TC_FunctionTraits<R(A1, A2, A3, A4, A5, A6)> {
            typedef R ReturnType;
            typedef A1 A1Type;
            typedef A2 A2Type;
            typedef A3 A3Type;
            typedef A4 A4Type;
            typedef A5 A5Type;
            typedef A6 A6Type;
        };

        template <typename R, typename A1, typename A2, typename A3, typename A4,
                  typename A5, typename A6, typename A7>
        struct TC_FunctionTraits<R(A1, A2, A3, A4, A5, A6, A7)> {
            typedef R ReturnType;
            typedef A1 A1Type;
            typedef A2 A2Type;
            typedef A3 A3Type;
            typedef A4 A4Type;
            typedef A5 A5Type;
            typedef A6 A6Type;
            typedef A7 A7Type;
        };

        template <typename R, typename A1, typename A2, typename A3, typename A4,
                  typename A5, typename A6, typename A7, typename A8>
        struct TC_FunctionTraits<R(A1, A2, A3, A4, A5, A6, A7, A8)> {
            typedef R ReturnType;
            typedef A1 A1Type;
            typedef A2 A2Type;
            typedef A3 A3Type;
            typedef A4 A4Type;
            typedef A5 A5Type;
            typedef A6 A6Type;
            typedef A7 A7Type;
            typedef A8 A8Type;
        };

        template <typename R, typename A1, typename A2, typename A3, typename A4,
                  typename A5, typename A6, typename A7, typename A8, typename A9>
        struct TC_FunctionTraits<R(A1, A2, A3, A4, A5, A6, A7, A8, A9)> {
            typedef R ReturnType;
            typedef A1 A1Type;
            typedef A2 A2Type;
            typedef A3 A3Type;
            typedef A4 A4Type;
            typedef A5 A5Type;
            typedef A6 A6Type;
            typedef A7 A7Type;
            typedef A8 A8Type;
            typedef A9 A9Type;
        };

    }

}

#endif
