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

#ifndef __TARS_PROMISE_H_
#define __TARS_PROMISE_H_

#include <functional>
#include "exception_ptr.h"
#include <pthread.h>
#include <list>
#include <memory>
#include "promise_exception.h"
#include "util/detail/tc_assert.h"
#include "util/detail/tc_template_util.h"
#include "util/tc_monitor.h"

using namespace tars;

namespace promise 
{

class PromiseAlreadySatisfiedException : public PromiseException 
{
public:

    PromiseAlreadySatisfiedException(const char *filename, int line)
    : PromiseException(filename, line)
    {}
    
protected:

    virtual const char *name() const
    {
        return "PromiseAlreadySatisfiedException";
    }
};

class FutureUninitializedException : public PromiseException 
{
public:
    FutureUninitializedException(const char *filename, int line)
    : PromiseException(filename, line)
    {}

protected:
    virtual const char *name() const
    {
        return "FutureUninitializedException";
    }
};
    
template <typename T> class Future;

template <typename T> struct is_future_type : tars::false_type {};
template <typename T> struct is_future_type<Future<T> > : tars::true_type {};

template <typename T> class Promise;
    
namespace detail
{
    template <typename T>
    struct resolved_type 
    {
        typedef T type;
    };

    template <typename T>
    struct resolved_type<Future<T> > 
    {
        typedef T type;
    };

    template <typename T>
    struct FutureTraits 
    {
        typedef std::unique_ptr<T> storage_type;
        typedef const T& rvalue_source_type;
        typedef const T& move_dest_type;
        typedef T& dest_reference_type;

        static void init(storage_type& storage, rvalue_source_type t)
        {
            storage.reset(new T(t));
        }

        static void assign(dest_reference_type dest, const storage_type& storage)
        {
            dest = *storage;
        }
    };

    template <typename T>
    struct FutureTraits<T&> 
    {
        typedef T* storage_type;
        typedef T& rvalue_source_type;
        typedef T& move_dest_type;
        typedef T*& dest_reference_type;

        static void init(storage_type& storage, T& t)
        {
            storage = &t;
        }

        static void assign(dest_reference_type dest, const storage_type& storage)
        {
            dest = storage;
        }
    };
        
    template <>
    struct FutureTraits<void> 
    {
        typedef void rvalue_source_type;
        typedef void move_dest_type;
    };
    
    template <typename T>
    class FutureObjectInterface 
    {
    public:
        typedef typename FutureTraits<T>::rvalue_source_type rvalue_source_type;
        typedef typename FutureTraits<T>::move_dest_type move_dest_type;
        typedef typename FutureTraits<T>::dest_reference_type dest_reference_type;
        using CallbackType = std::function<void(const std::shared_ptr<FutureObjectInterface>&)> ;
        
        virtual ~FutureObjectInterface() {}

        virtual bool isDone() const = 0;
        virtual bool hasValue() const = 0;
        virtual bool hasException() const = 0;
        virtual void setValue(rvalue_source_type v) = 0;
        virtual void setException(const ExceptionPtr& e) = 0;
        virtual move_dest_type get() const = 0;
        virtual bool tryGet(dest_reference_type v) const = 0;
        virtual void registerCallback(const CallbackType& callback) = 0;
    };
        
    template <>
    class FutureObjectInterface<void> 
    {
    public:
        typedef FutureTraits<void>::move_dest_type move_dest_type;
        using CallbackType = std::function<void (const std::shared_ptr<FutureObjectInterface>& )>;
        
        virtual ~FutureObjectInterface() {}

        virtual bool isDone() const = 0;
        virtual bool hasValue() const = 0;
        virtual bool hasException() const = 0;
        virtual void set() = 0;
        virtual void setException(const ExceptionPtr& e) = 0;
        virtual move_dest_type get() const = 0;
        virtual void registerCallback(const CallbackType& callback) = 0;
    };
    
    template <typename T>
    class PromptFutureObject : public FutureObjectInterface<T>,
                               public std::enable_shared_from_this<PromptFutureObject<T> > 
    {
    public:
        typedef typename FutureObjectInterface<T>::move_dest_type move_dest_type;
        typedef typename FutureObjectInterface<T>::rvalue_source_type rvalue_source_type;
        typedef typename FutureObjectInterface<T>::dest_reference_type dest_reference_type;
        typedef typename FutureObjectInterface<T>::CallbackType CallbackType;
        
        PromptFutureObject(rvalue_source_type v)
        {
            FutureTraits<T>::init(m_value, v);
        }

        PromptFutureObject(const ExceptionPtr& e)
            : m_exception_ptr(e)
        {}
        
        virtual ~PromptFutureObject() {}

        virtual bool isDone() const { return true; }

        virtual bool hasValue() const
        {
            if (m_exception_ptr)
                return false;
            return true;
        }
        
        virtual bool hasException() const { return m_exception_ptr; }

        virtual void setValue(rvalue_source_type /*v*/) {}
        virtual void setException(const ExceptionPtr& /*e*/) {}
        
        virtual move_dest_type get() const
        {
            TC_ASSERT(m_exception_ptr || m_value);
            if (m_exception_ptr)
                m_exception_ptr->rethrow();
            return *m_value;
        }
        
        virtual bool tryGet(dest_reference_type v) const { FutureTraits<T>::assign(v, m_value); return true; }
        
        virtual void registerCallback(const CallbackType& callback)
        {
            TC_ASSERT(callback);
            try 
            {
                callback(this->shared_from_this());
            } 
            catch (...) 
            {
            }
        }
        
    private:
        typename FutureTraits<T>::storage_type m_value;
        ExceptionPtr m_exception_ptr;
    };
    
    template <>
    class PromptFutureObject<void> : public FutureObjectInterface<void>,
                                     public std::enable_shared_from_this<PromptFutureObject<void> >
    {
    public:
        typedef FutureObjectInterface<void>::move_dest_type move_dest_type;
        typedef FutureObjectInterface<void>::CallbackType CallbackType;
        
        PromptFutureObject() {}

        PromptFutureObject(const ExceptionPtr& e)
            : m_exception_ptr(e)
        {}
        
        virtual ~PromptFutureObject() {}

        virtual bool isDone() const { return true; }

        virtual bool hasValue() const
        {
            if (m_exception_ptr)
                return false;
            return true;
        }
        
        virtual bool hasException() const { return !!m_exception_ptr; }

        virtual void set() {}
        virtual void setException(const ExceptionPtr& /*e*/) {}
        
        virtual move_dest_type get() const
        {
            if (m_exception_ptr)
                m_exception_ptr->rethrow();
        }
        
        virtual void registerCallback(const CallbackType& callback)
        {
            TC_ASSERT(callback);
            try 
            {
                callback(this->shared_from_this());
            } 
            catch (...) 
            {
            }
        }
        
    private:
        ExceptionPtr m_exception_ptr;
    };

    struct FutureObjectBase 
    {
        FutureObjectBase()
        : m_is_done(false)
        {}

        virtual ~FutureObjectBase() {}
        
        bool isDone() const
        {
            TC_ThreadLock::Lock lock(m_monitor);
            return m_is_done;
        }
        
        bool hasValue() const
        {
            TC_ThreadLock::Lock lock(m_monitor);
            return m_is_done && !m_exception_ptr;
        }

        bool hasException() const
        {
            TC_ThreadLock::Lock lock(m_monitor);
            return m_is_done && m_exception_ptr;
        }
        
        void markFinishedWithException(const ExceptionPtr& e)
        {
            TC_ASSERT(e);

            TC_ThreadLock::Lock lock(m_monitor);
            if (m_is_done) 
            {
                throw PromiseAlreadySatisfiedException(__FILE__, __LINE__);
            }
            m_exception_ptr = e;
            markFinishedInternal();
        }

        void markFinishedInternal()
        {
            m_is_done = true;

            m_monitor.notifyAll();
        }

        void wait() const
        {
            {
                TC_ThreadLock::Lock lock(m_monitor);
                while(!m_is_done)
                {
                    m_monitor.wait();
                }
            }

            if (m_exception_ptr)
                m_exception_ptr->rethrow();
        }

        TC_ThreadLock  m_monitor;
        bool m_is_done;
        ExceptionPtr m_exception_ptr;
    };
    
    template <typename T>
    class FutureObject : public FutureObjectInterface<T>,
                         public std::enable_shared_from_this<FutureObject<T> >,
                         private FutureObjectBase 
    {
    public:

        typedef typename FutureObjectInterface<T>::move_dest_type move_dest_type;
        typedef typename FutureObjectInterface<T>::rvalue_source_type rvalue_source_type;
        typedef typename FutureObjectInterface<T>::CallbackType CallbackType;
        typedef typename FutureObjectInterface<T>::dest_reference_type dest_reference_type;
        
        FutureObject() {}

        virtual ~FutureObject() {}
        
        virtual bool isDone() const
        {
            return FutureObjectBase::isDone();
        }
        
        virtual bool hasValue() const
        {
            return FutureObjectBase::hasValue();
        }

        virtual bool hasException() const
        {
            return FutureObjectBase::hasException();
        }
        
        virtual move_dest_type get() const
        {
            wait();

            TC_ASSERT(m_value);
            return *m_value;
        }

        virtual bool tryGet(dest_reference_type v) const
        {
            if (!isDone())
                return false;
            if (m_exception_ptr)
                m_exception_ptr->rethrow();
            FutureTraits<T>::assign(v, m_value);
            return true;
        }
        
        virtual void setException(const ExceptionPtr& e)
        {
            markFinishedWithException(e);
            doPendingCallbacks();
        }
        
        virtual void setValue(rvalue_source_type t)
        {
            {
                TC_ThreadLock::Lock lock(m_monitor);
                if (m_is_done) 
                {
                    throwException(PromiseAlreadySatisfiedException(__FILE__, __LINE__));
                }
                m_value.reset(new T(t));
                markFinishedInternal();
            }
            doPendingCallbacks();
        }
        
        virtual void registerCallback(const CallbackType& callback)
        {
            TC_ASSERT(callback);

            TC_ThreadLock::Lock lock(m_monitor);
            if (m_is_done) 
            {
                lock.release();
                try 
                {
                    callback(this->shared_from_this());
                } 
                catch (...) 
                {
                }
            } 
            else 
            {
                m_pending_callbacks.push_back(callback);
            }
        }

    private:

        void doPendingCallbacks()
        {
            std::list<CallbackType> callbacks;
            {
                TC_ThreadLock::Lock lock(m_monitor);
                callbacks.swap(m_pending_callbacks);
            }

            for (typename std::list<CallbackType>::const_iterator it = callbacks.begin(),
                                                                 end = callbacks.end();
                    it != end; ++it) 
            {
                try 
                {
                    (*it)(this->shared_from_this());
                } 
                catch (...) 
                {
                
                }
            }
        }

        typename FutureTraits<T>::storage_type m_value;
        std::list<CallbackType> m_pending_callbacks;
    };

    template <>
    class FutureObject<void> : public FutureObjectInterface<void>,
                               public std::enable_shared_from_this<FutureObject<void> >,
                               private FutureObjectBase 
    {
    public:
        
        using typename FutureObjectInterface<void>::CallbackType;
        
        FutureObject() {}

        virtual ~FutureObject() {}
        
        virtual bool isDone() const
        {
            return FutureObjectBase::isDone();
        }
        
        virtual bool hasValue() const
        {
            return FutureObjectBase::hasValue();
        }

        virtual bool hasException() const
        {
            return FutureObjectBase::hasException();
        }
        
        virtual void get() const
        {
            wait();
        }

        virtual void set()
        {
            {
                TC_ThreadLock::Lock lock(m_monitor);
                if (m_is_done) 
                {
                    throwException(PromiseAlreadySatisfiedException(__FILE__, __LINE__));
                }
                markFinishedInternal();
            }

            doPendingCallbacks();
        }

        virtual void setException(const ExceptionPtr& e)
        {
            markFinishedWithException(e);
            doPendingCallbacks();
        }

        void registerCallback(const CallbackType& callback)
        {
            TC_ASSERT(callback);

            TC_ThreadLock::Lock lock(m_monitor);
            if (m_is_done) 
            {
                lock.release();
                try 
                {
                    callback(this->shared_from_this());
                } 
                catch (...) 
                {
                    
                }
                return;
            }
            m_pending_callbacks.push_back(callback);
        }

    private:
        void doPendingCallbacks()
        {
            std::list<CallbackType> callbacks;
            {
                TC_ThreadLock::Lock lock(m_monitor);
                callbacks.swap(m_pending_callbacks);
            }

            for (std::list<CallbackType>::const_iterator it = callbacks.begin(),
                                                        end = callbacks.end();
                    it != end; ++it) 
            {
                (*it)(this->shared_from_this());
            }
        }

        std::list<CallbackType> m_pending_callbacks;
    };

    template <typename R> class ForwardValue;

    template <typename R, typename T>
    class SequentialCallback 
    {
    private:
        typedef typename detail::resolved_type<R>::type value_type;
        typedef std::shared_ptr<detail::FutureObjectInterface<T> > FuturePtr;
    public:
        SequentialCallback(const std::function<R (const Future<T>&)>& callback,
                           const Promise<value_type>& promise)
        : m_callback(callback)
        , m_promise(promise)
        {}
        
        template <typename U>
        typename tc_enable_if<tc_is_void<U> >::type run(const FuturePtr& future)
        {
            try 
            {
                m_callback(future);
                m_promise.set();
            } 
            catch (...) 
            {
                m_promise.setException(currentException());
            }
        }

        template <typename U>
        typename tc_enable_if_c<!tc_is_void<U>::value && !is_future_type<U>::value>::type
        run(const FuturePtr& future)
        {
            try 
            {
                m_promise.setValue(m_callback(future));
            } 
            catch (...) 
            {
                m_promise.setException(currentException());
            }
        }

        template <typename U>
        typename tc_enable_if<is_future_type<U> >::type run(const FuturePtr& future)
        {
            try 
            {
                m_callback(future).then(TC_Bind(&ForwardValue<value_type>::template run<value_type>,
                                             tc_owned(new ForwardValue<value_type>(m_promise))));
            } 
            catch (...) 
            {
                m_promise.setException(currentException());
            }
        }
        
    private:

        std::function<R (const Future<T>&)> m_callback;
        Promise<value_type> m_promise;
    };

    template <typename R>
    class ForwardValue 
    {
    public:
        ForwardValue(const Promise<R>& promise)
            : m_promise(promise)
        {}

        template <typename V>
        typename tc_enable_if<tc_is_void<V> >::type run(const Future<V>& future)
        {
            try 
            {
                future.get();
                m_promise.set();
            } 
            catch (...) 
            {
                m_promise.setException(currentException());
            }
        }

        template <typename V>
        typename tc_disable_if<tc_is_void<V> >::type run(const Future<V>& future)
        {
            try 
            {
                m_promise.setValue(future.get());
            } 
            catch (...) 
            {
                m_promise.setException(currentException());
            }
        }

    private:
        Promise<R> m_promise;
    };
    
    template <typename T>
    class FutureBase 
    {
    private:
        typedef typename detail::FutureTraits<T>::move_dest_type move_dest_type;
        typedef typename detail::FutureTraits<T>::rvalue_source_type rvalue_source_type;
    public:
        typedef bool (FutureBase::*unspecified_bool_type)() const;
        
        FutureBase() {}

        FutureBase(const ExceptionPtr& e)
        : m_future(std::shared_ptr<detail::PromptFutureObject<T> >(new detail::PromptFutureObject<T>(e)))
        {}
    
        virtual ~FutureBase() {}

        move_dest_type get() const
        {
            if (!m_future)
            {
                throwException(FutureUninitializedException(__FILE__, __LINE__));
            }
            return m_future->get();
        }

        bool isDone() const
        {
            if (!m_future)
                return false;
            return m_future && m_future->isDone();
        }

        bool hasValue() const
        {
            if (!m_future)
                return false;
            return m_future && m_future->hasValue();
        }
    
        bool hasException() const
        {
            if (!m_future)
                return false;
            return m_future && m_future->hasException();
        }

        operator unspecified_bool_type() const
        {
            return m_future ? &FutureBase::isDone : NULL;
        }
        
    protected:

        typedef std::shared_ptr<detail::FutureObjectInterface<T> > FuturePtr;
    
        FutureBase(const FuturePtr& future)
            : m_future(future)
        {}

        FuturePtr m_future;
    };

}  // namespace detail

template <typename T>
class Future : public detail::FutureBase<T> 
{
public:
    Future() {}

    explicit Future(typename detail::FutureTraits<T>::rvalue_source_type t)
        : detail::FutureBase<T>(std::shared_ptr<detail::PromptFutureObject<T> >(new detail::PromptFutureObject<T>(t)))
    {}
        
    Future(const ExceptionPtr& e)
        : detail::FutureBase<T>(e)
    {}

    virtual ~Future() {}
    
    template <typename R>
    Future<typename detail::resolved_type<R>::type> then(const std::function<R (const Future&)>& callback) const
    {
        typedef typename detail::resolved_type<R>::type value_type;

        if (!this->m_future)
        {
            throwException(FutureUninitializedException(__FILE__, __LINE__));
        }

        Promise<value_type> promise;
        this->m_future->registerCallback(TC_Bind(&detail::SequentialCallback<R, T>::template run<R>,
                                              tc_owned(new detail::SequentialCallback<R, T>(callback, promise))));
        return promise.getFuture();
    }
    
private:

    Future(const typename detail::FutureBase<T>::FuturePtr& future)
        : detail::FutureBase<T>(future)
    {}

    template <typename R, typename U> friend class detail::SequentialCallback;
    friend class Promise<T>;
};

template <>
class Future<void> : public detail::FutureBase<void> 
{
public:
    Future() {}

    Future(const ExceptionPtr& e)
        : detail::FutureBase<void>(e)
    {}

    virtual ~Future() {}

    template <typename R>
    Future<typename detail::resolved_type<R>::type> then(const std::function<R (const Future&)>& callback) const
    {
        typedef typename detail::resolved_type<R>::type value_type;

        if (!this->m_future)
        {
            throwException(FutureUninitializedException(__FILE__, __LINE__));
        }

        Promise<value_type> promise;
        this->m_future->registerCallback(TC_Bind(&detail::SequentialCallback<R, void>::template run<R>,
                                              tc_owned(new detail::SequentialCallback<R, void>(callback, promise))));
        return promise.getFuture();
    }
    
private:

    Future(const detail::FutureBase<void>::FuturePtr& future)
        : detail::FutureBase<void>(future)
    {}

    friend Future<void> makeFuture();

    template <typename R, typename U> friend class detail::SequentialCallback;
    friend class Promise<void>;
};

template <typename T>
typename tc_disable_if<tc_is_void<T>, Future<T> >::type
makeFuture(typename detail::FutureTraits<T>::rvalue_source_type t)
{
    return Future<T>(t);
}

inline Future<void> makeFuture()
{
    return Future<void>(std::shared_ptr<detail::PromptFutureObject<void> >(new detail::PromptFutureObject<void>()));
}

template <typename T>
class Promise 
{
public:

    Promise()
        : m_future(std::shared_ptr<detail::FutureObject<T> >(new detail::FutureObject<T>()))
    {}
    
    void setValue(typename detail::FutureTraits<T>::rvalue_source_type v)
    {
        TC_ASSERT(m_future);
        m_future->setValue(v);
    }
            
    void setException(const ExceptionPtr& e)
    {
        m_future->setException(e);
    }

    Future<T> getFuture() const
    {
        return Future<T>(m_future);
    }

private:
    std::shared_ptr<detail::FutureObjectInterface<T> > m_future;
};

template <>
class Promise<void> 
{
private:
    std::shared_ptr<detail::FutureObjectInterface<void> > m_future;
public:

    Promise()
        : m_future(std::shared_ptr<detail::FutureObject<void> >(new detail::FutureObject<void>()))
    {}
    
    void set()
    {
        TC_ASSERT(m_future);
        m_future->set();
    }
            
    void setException(const ExceptionPtr& e)
    {
        m_future->setException(e);
    }

    Future<void> getFuture() const
    {
        return Future<void>(m_future);
    }
};

}  // namespace promise

#endif  // __TARS_PROMISE_H_
