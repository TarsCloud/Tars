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

#ifndef __TC_SHARED_PTR_H__
#define __TC_SHARED_PTR_H__

#include <iostream>
#include "detail/tc_assert.h"
#include "detail/tc_shared_count_impl.h"
#include "detail/tc_checked_delete.h"
#include "detail/tc_template_util.h"

namespace tars
{
    
    template <typename T> class TC_SharedPtr;
    template <typename T> class TC_EnableSharedFromThis;

    namespace detail 
    {
        template <class X, class T>
        inline void tc_sp_enable_shared_from_this(const tars::TC_SharedPtr<X> *pp,
                                               const tars::TC_EnableSharedFromThis<T> * px)
        {
            if (px != NULL) 
            {
                px->acceptOwner(*pp);
            }
        }

        inline void tc_sp_enable_shared_from_this(...)
        {
        }

    }
    
    template <typename T>
    class TC_SharedPtr 
    {
    public:
        typedef T* (TC_SharedPtr::*unspecified_bool_type)() const;
        
        TC_SharedPtr()
        : m_px(NULL)
        , m_pn(NULL)
        { }
        
        template <typename U>
        explicit TC_SharedPtr(U *p)
        : m_px(p)
        , m_pn(NULL)
        {
            try 
            {
                if (m_px)
                    m_pn = new detail::tc_shared_count_impl_p<U>(p);
            } 
            catch (...) 
            {
                tc_checked_delete(p);
                throw;
            }
            detail::tc_sp_enable_shared_from_this(this, p);
        }

        template <typename U, typename D>
        TC_SharedPtr(U *p, D d)
        : m_px(p)
        , m_pn(NULL)
        {
            try 
            {
                if (m_px)
                    m_pn = new detail::tc_shared_count_impl_pd<U, D>(p, d);
            } 
            catch (...) 
            {
                d(p);
                throw;
            }
            detail::tc_sp_enable_shared_from_this(this, p);
        }

        TC_SharedPtr(const TC_SharedPtr& rhs)
        : m_px(rhs.m_px)
        , m_pn(rhs.m_pn)
        {
            if (m_pn)
                m_pn->increment();
        }

        template <typename U>
        TC_SharedPtr(const TC_SharedPtr<U>& rhs, typename tc_enable_if<tc_is_convertible<U*, T*> >::type* = 0)
        : m_px(rhs.m_px)
        , m_pn(rhs.m_pn)
        {
            if (m_pn)
                m_pn->increment();
        }
    
        ~TC_SharedPtr()
        {
            TC_ASSERT((m_px && m_pn && m_pn->use_count() > 0)
                       || (m_px == NULL && m_pn == NULL));
            
            if (m_pn)
                m_pn->release();
        }

        /**
         * \throws nothing
         */
        TC_SharedPtr& operator=(const TC_SharedPtr& rhs)
        {
            TC_SharedPtr(rhs).swap(*this);
            return *this;
        }
        
        template <typename U>
        TC_SharedPtr& operator=(const TC_SharedPtr<U>& rhs)
        {
            TC_SharedPtr(rhs).swap(*this);
            return *this;
        }
    
        T *operator->() const
        {
            TC_ASSERT(m_px);
            return m_px;
        }
        
        T& operator*() const
        {
            TC_ASSERT(m_px);
            return *m_px;
        }
        
        T *get() const
        { return m_px; }

        operator unspecified_bool_type() const
        {
            return m_px == NULL ? NULL : &TC_SharedPtr::get;
        }

        size_t usecount() const
        {
            if (m_pn)
                return m_pn->use_count();
            return 1;
        }
    
        bool unique() const
        {
            return usecount() == 1;
        }
        
        void reset(T *p = NULL)
        {
            TC_SharedPtr(p).swap(*this);
        }

        template <typename D>
        void reset(T *p, D d)
        {
            TC_SharedPtr(p, d).swap(*this);
        }
        
        void swap(TC_SharedPtr& other)
        {
            std::swap(m_px, other.m_px);
            std::swap(m_pn, other.m_pn);
        }

    private:

        template <typename U>
        explicit TC_SharedPtr(U *p, detail::tc_shared_count_base *pn)
        : m_px(p), m_pn(pn)
        {
            if (m_pn == NULL)
                TC_SharedPtr(p);
            else
                m_pn->increment();
        }

        T *m_px;
        detail::tc_shared_count_base *m_pn;
        
        template <typename U> friend class TC_SharedPtr;
        template <typename U> friend class TC_EnableSharedFromThis;
    };

    template <typename T>
    inline bool operator==(const TC_SharedPtr<T>& lhs, const TC_SharedPtr<T>& rhs)
    {
        return lhs.get() == rhs.get();
    }

    template <typename T>
    inline bool operator!=(const TC_SharedPtr<T>& lhs, const TC_SharedPtr<T>& rhs)
    {
        return !(lhs == rhs);
    }

    template <typename T>
    inline bool operator<(const TC_SharedPtr<T>& lhs, const TC_SharedPtr<T>& rhs)
    {
        return lhs.get() < rhs.get();
    }

    template <typename T>
    inline bool operator>(const TC_SharedPtr<T>& lhs, const TC_SharedPtr<T>& rhs)
    {
        return rhs.get() < lhs.get();
    }

    template <typename T>
    inline std::ostream& operator<<(std::ostream& os, const TC_SharedPtr<T>& p)
    {
        os << p.get();
        return os;
    }

    template <typename T>
    inline void swap(TC_SharedPtr<T>& lhs, TC_SharedPtr<T>& rhs)
    {
        lhs.swap(rhs);
    }
    
}

#endif
