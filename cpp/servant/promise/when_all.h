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

#ifndef __TARS_PROMISE_WHEN_ALL_H_
#define __TARS_PROMISE_WHEN_ALL_H_

#include "promise.h"
#include <vector>
#include "tuple.h"
#include "util/tc_monitor.h"
#include "util/detail/tc_assert.h"
namespace promise 
{
    namespace detail 
    {
        // Callback implementation for parallel composition of futures.       
        template <typename FutureAllValueType>
        struct ParallelAllCallbackBase 
        {
        public:
            typedef Promise<FutureAllValueType> PromiseAll;
                
            ParallelAllCallbackBase(const PromiseAll& promise_all,
                                 size_t number_of_non_satisfied)
            : m_number_of_non_satisfied(number_of_non_satisfied)
            , m_promise_all(promise_all)
            {
            }

            virtual ~ParallelAllCallbackBase()
            {
            }

            size_t m_number_of_non_satisfied;
            PromiseAll m_promise_all;
            FutureAllValueType m_waiting_futures;
            TC_ThreadLock  m_monitor;

        private:
            ParallelAllCallbackBase(const ParallelAllCallbackBase&);
            void operator=(const ParallelAllCallbackBase&);
            
        };

        // FutureAllValueType is supposed to be of type promise::Tuple<promise::Future, ...> or
        // std::vector<promise::Future>.
        //
        // This implementation is for promise::Tuple<>.
        template <typename FutureAllValueType>
        class ParallelAllCallback : private ParallelAllCallbackBase<FutureAllValueType> 
        {
        public:
            typedef typename ParallelAllCallbackBase<FutureAllValueType>::PromiseAll PromiseAll;
            
            ParallelAllCallback(const PromiseAll& promise_all)
            : ParallelAllCallbackBase<FutureAllValueType>(promise_all, FutureAllValueType::LENGTH)
            {}

            virtual ~ParallelAllCallback() {}
        
            template <int N>
            void on_future(const typename FutureAllValueType::template element<N>::type& future)
            {
                // Note: this future must be recorded no matter what exception is thrown.
                TC_ThreadLock::Lock lock(this->m_monitor);
                this->m_waiting_futures.template get<N>() = future;
                TC_ASSERT(this->m_number_of_non_satisfied > 0);
                if (--this->m_number_of_non_satisfied == 0) 
                {
                    lock.release();
                    try 
                    {
                        this->m_promise_all.setValue(this->m_waiting_futures);
                    } 
                    catch (...) 
                    {
                        this->m_promise_all.setException(promise::currentException());
                    }
                    return;
                }
            }

        private:
            ParallelAllCallback(const ParallelAllCallback&);
            void operator=(const ParallelAllCallback&);
            
        };

        // This implementation is specialized for std::vector<promise::Future<> >.
        template <typename FutureType>
        class ParallelAllCallback<std::vector<FutureType> > : private ParallelAllCallbackBase<std::vector<FutureType> > 
        {
        public:
            typedef typename ParallelAllCallbackBase<std::vector<FutureType> >::PromiseAll PromiseAll;
            
            ParallelAllCallback(const PromiseAll& promise_all,
                             size_t number_of_non_satisfied)
            : ParallelAllCallbackBase<std::vector<FutureType> >(promise_all, number_of_non_satisfied)
            {
                this->m_waiting_futures.resize(number_of_non_satisfied);
            }

            virtual ~ParallelAllCallback() {}
        
            void on_future(const FutureType& future, size_t idx)
            {
                // Note: this future must be recorded no matter what exception is thrown.
                TC_ASSERT(idx < this->m_waiting_futures.size());

                TC_ThreadLock::Lock lock(this->m_monitor);
                this->m_waiting_futures[idx] = future;
                TC_ASSERT(this->m_number_of_non_satisfied > 0);
                if (--this->m_number_of_non_satisfied == 0) 
                {
                    lock.release();
                    try 
                    {
                        this->m_promise_all.setValue(this->m_waiting_futures);
                    } 
                    catch (...) 
                    {
                        this->m_promise_all.setException(promise::currentException());
                    }
                    return;
                }
            }
            
        private:
            ParallelAllCallback(const ParallelAllCallback&);
            void operator=(const ParallelAllCallback&);
            
        };

        // A helper class to notify ParallelAllCallback<std::vector<> > which future is done.
        template <typename T>
        class VectorParallelCallback 
        {
        public:
            VectorParallelCallback(const std::shared_ptr<ParallelAllCallback<std::vector<Future<T> > > >& cb, size_t idx)
                : m_parallel_callback(cb),
                  m_idx(idx)
            {}

            void on_future(const Future<T>& future)
            {
                m_parallel_callback->on_future(future, m_idx);
            }

        private:
            std::shared_ptr<ParallelAllCallback<std::vector<Future<T> > > > m_parallel_callback;
            size_t m_idx;
        };
        
    }  // namespace detail
    
    /**
     * Parallelly composite two or more futues. Only when all futures are satisfied is
     * the composited future satisfied.
     */
    template <typename T1, typename T2>
    Future<Tuple<Future<T1>, Future<T2> > >
    whenAll(const Future<T1>& future1, const Future<T2>& future2)
    {
        typedef Tuple<Future<T1>, Future<T2> > FutureAllValueType;
        typedef Promise<FutureAllValueType> PromiseAll;
        typedef detail::ParallelAllCallback<FutureAllValueType> WhenAllCallback;

        PromiseAll promise_all;
        std::shared_ptr<WhenAllCallback> future_callback(new WhenAllCallback(promise_all));
        future1.then(tars::TC_Bind(&WhenAllCallback::template on_future<0>, tars::tc_shared(future_callback)));
        future2.then(tars::TC_Bind(&WhenAllCallback::template on_future<1>, tars::tc_shared(future_callback)));
        return promise_all.getFuture();
    }

    template <typename T1, typename T2, typename T3>
    Future<Tuple<Future<T1>, Future<T2>, Future<T3> > >
    whenAll(const Future<T1>& future1, const Future<T2>& future2, const Future<T3>& future3)
    {
        typedef Tuple<Future<T1>, Future<T2>, Future<T3> > FutureAllValueType;
        typedef Promise<FutureAllValueType> PromiseAll;
        typedef detail::ParallelAllCallback<FutureAllValueType> WhenAllCallback;

        PromiseAll promise_all;
        std::shared_ptr<WhenAllCallback> future_callback(new WhenAllCallback(promise_all));
        future1.then(tars::TC_Bind(&WhenAllCallback::template on_future<0>, tars::tc_shared(future_callback)));
        future2.then(tars::TC_Bind(&WhenAllCallback::template on_future<1>, tars::tc_shared(future_callback)));
        future3.then(tars::TC_Bind(&WhenAllCallback::template on_future<2>, tars::tc_shared(future_callback)));
        return promise_all.getFuture();
    }

    template <typename T1, typename T2, typename T3, typename T4>
    Future<Tuple<Future<T1>, Future<T2>, Future<T3>, Future<T4> > >
    whenAll(const Future<T1>& future1, const Future<T2>& future2, const Future<T3>& future3, const Future<T4>& future4)
    {
        typedef Tuple<Future<T1>, Future<T2>, Future<T3>, Future<T4> > FutureAllValueType;
        typedef Promise<FutureAllValueType> PromiseAll;
        typedef detail::ParallelAllCallback<FutureAllValueType> WhenAllCallback;

        PromiseAll promise_all;
        std::shared_ptr<WhenAllCallback> future_callback(new WhenAllCallback(promise_all));
        future1.then(tars::TC_Bind(&WhenAllCallback::template on_future<0>, tars::tc_shared(future_callback)));
        future2.then(tars::TC_Bind(&WhenAllCallback::template on_future<1>, tars::tc_shared(future_callback)));
        future3.then(tars::TC_Bind(&WhenAllCallback::template on_future<2>, tars::tc_shared(future_callback)));
        future4.then(tars::TC_Bind(&WhenAllCallback::template on_future<3>, tars::tc_shared(future_callback)));
        return promise_all.getFuture();
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5>
    Future<Tuple<Future<T1>, Future<T2>, Future<T3>, Future<T4>, Future<T5> > >
    whenAll(const Future<T1>& future1, const Future<T2>& future2, const Future<T3>& future3, const Future<T4>& future4,
            const Future<T5>& future5)
    {
        typedef Tuple<Future<T1>, Future<T2>, Future<T3>, Future<T4>, Future<T5> > FutureAllValueType;
        typedef Promise<FutureAllValueType> PromiseAll;
        typedef detail::ParallelAllCallback<FutureAllValueType> WhenAllCallback;

        PromiseAll promise_all;
        std::shared_ptr<WhenAllCallback> future_callback(new WhenAllCallback(promise_all));
        future1.then(tars::TC_Bind(&WhenAllCallback::template on_future<0>, tars::tc_shared(future_callback)));
        future2.then(tars::TC_Bind(&WhenAllCallback::template on_future<1>, tars::tc_shared(future_callback)));
        future3.then(tars::TC_Bind(&WhenAllCallback::template on_future<2>, tars::tc_shared(future_callback)));
        future4.then(tars::TC_Bind(&WhenAllCallback::template on_future<3>, tars::tc_shared(future_callback)));
        future5.then(tars::TC_Bind(&WhenAllCallback::template on_future<4>, tars::tc_shared(future_callback)));
        return promise_all.getFuture();
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    Future<Tuple<Future<T1>, Future<T2>, Future<T3>, Future<T4>, Future<T5>, Future<T6> > >
    whenAll(const Future<T1>& future1, const Future<T2>& future2, const Future<T3>& future3, const Future<T4>& future4,
            const Future<T5>& future5, const Future<T6>& future6)
    {
        typedef Tuple<Future<T1>, Future<T2>, Future<T3>, Future<T4>, Future<T5>, Future<T6> > FutureAllValueType;
        typedef Promise<FutureAllValueType> PromiseAll;
        typedef detail::ParallelAllCallback<FutureAllValueType> WhenAllCallback;

        PromiseAll promise_all;
        std::shared_ptr<WhenAllCallback> future_callback(new WhenAllCallback(promise_all));
        future1.then(tars::TC_Bind(&WhenAllCallback::template on_future<0>, tars::tc_shared(future_callback)));
        future2.then(tars::TC_Bind(&WhenAllCallback::template on_future<1>, tars::tc_shared(future_callback)));
        future3.then(tars::TC_Bind(&WhenAllCallback::template on_future<2>, tars::tc_shared(future_callback)));
        future4.then(tars::TC_Bind(&WhenAllCallback::template on_future<3>, tars::tc_shared(future_callback)));
        future5.then(tars::TC_Bind(&WhenAllCallback::template on_future<4>, tars::tc_shared(future_callback)));
        future6.then(tars::TC_Bind(&WhenAllCallback::template on_future<5>, tars::tc_shared(future_callback)));
        return promise_all.getFuture();
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    Future<Tuple<Future<T1>, Future<T2>, Future<T3>, Future<T4>, Future<T5>, Future<T6>, Future<T7> > >
    whenAll(const Future<T1>& future1, const Future<T2>& future2, const Future<T3>& future3, const Future<T4>& future4,
            const Future<T5>& future5, const Future<T6>& future6, const Future<T7>& future7)
    {
        typedef Tuple<Future<T1>, Future<T2>, Future<T3>, Future<T4>, Future<T5>, Future<T6>, Future<T7> > FutureAllValueType;
        typedef Promise<FutureAllValueType> PromiseAll;
        typedef detail::ParallelAllCallback<FutureAllValueType> WhenAllCallback;

        PromiseAll promise_all;
        std::shared_ptr<WhenAllCallback> future_callback(new WhenAllCallback(promise_all));
        future1.then(tars::TC_Bind(&WhenAllCallback::template on_future<0>, tars::tc_shared(future_callback)));
        future2.then(tars::TC_Bind(&WhenAllCallback::template on_future<1>, tars::tc_shared(future_callback)));
        future3.then(tars::TC_Bind(&WhenAllCallback::template on_future<2>, tars::tc_shared(future_callback)));
        future4.then(tars::TC_Bind(&WhenAllCallback::template on_future<3>, tars::tc_shared(future_callback)));
        future5.then(tars::TC_Bind(&WhenAllCallback::template on_future<4>, tars::tc_shared(future_callback)));
        future6.then(tars::TC_Bind(&WhenAllCallback::template on_future<5>, tars::tc_shared(future_callback)));
        future7.then(tars::TC_Bind(&WhenAllCallback::template on_future<6>, tars::tc_shared(future_callback)));
        return promise_all.getFuture();
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    Future<Tuple<Future<T1>, Future<T2>, Future<T3>, Future<T4>, Future<T5>, Future<T6>, Future<T7>, Future<T8> > >
    whenAll(const Future<T1>& future1, const Future<T2>& future2, const Future<T3>& future3, const Future<T4>& future4,
            const Future<T5>& future5, const Future<T6>& future6, const Future<T7>& future7, const Future<T8>& future8)
    {
        typedef Tuple<Future<T1>, Future<T2>, Future<T3>, Future<T4>, Future<T5>, Future<T6>, Future<T7>, Future<T8> > FutureAllValueType;
        typedef Promise<FutureAllValueType> PromiseAll;
        typedef detail::ParallelAllCallback<FutureAllValueType> WhenAllCallback;

        PromiseAll promise_all;
        std::shared_ptr<WhenAllCallback> future_callback(new WhenAllCallback(promise_all));
        future1.then(tars::TC_Bind(&WhenAllCallback::template on_future<0>, tars::tc_shared(future_callback)));
        future2.then(tars::TC_Bind(&WhenAllCallback::template on_future<1>, tars::tc_shared(future_callback)));
        future3.then(tars::TC_Bind(&WhenAllCallback::template on_future<2>, tars::tc_shared(future_callback)));
        future4.then(tars::TC_Bind(&WhenAllCallback::template on_future<3>, tars::tc_shared(future_callback)));
        future5.then(tars::TC_Bind(&WhenAllCallback::template on_future<4>, tars::tc_shared(future_callback)));
        future6.then(tars::TC_Bind(&WhenAllCallback::template on_future<5>, tars::tc_shared(future_callback)));
        future7.then(tars::TC_Bind(&WhenAllCallback::template on_future<6>, tars::tc_shared(future_callback)));
        future8.then(tars::TC_Bind(&WhenAllCallback::template on_future<7>, tars::tc_shared(future_callback)));
        return promise_all.getFuture();
    }

    template <typename T, typename InputIterator>
    Future<std::vector<Future<T> > >
    whenAll(InputIterator first, InputIterator last)
    {
        typedef std::vector<Future<T> > FutureAllValueType;
        typedef Promise<FutureAllValueType> PromiseAll;
        typedef detail::ParallelAllCallback<FutureAllValueType> WhenAllCallback;

        PromiseAll promise_all;
        if (first == last) 
        {
            promise_all.setValue(FutureAllValueType());
        } 
        else 
        {
            std::shared_ptr<WhenAllCallback> when_all_callback(new WhenAllCallback(promise_all, std::distance(first, last)));
            size_t i;

            for (i = 0; first != last; ++first, ++i) 
            {
                first->then(tars::TC_Bind(&detail::VectorParallelCallback<T>::on_future,
                                      tars::tc_owned(new detail::VectorParallelCallback<T>(when_all_callback, i))));
            }
        }
        return promise_all.getFuture();
    }
    
}  // namespace promise

#endif  // __TARS_PROMISE_WHEN_ALL_H__
