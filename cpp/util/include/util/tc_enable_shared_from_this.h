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

#ifndef __TC_ENABLE_SHARED_FROM_THIS_H__
#define __TC_ENABLE_SHARED_FROM_THIS_H__

#include "tc_shared_ptr.h"

namespace tars
{

    template <class T>
    class TC_EnableSharedFromThis 
    {
    public:

        TC_SharedPtr<T> sharedFromThis()
        {
            TC_SharedPtr<T> p(m_this, m_owner_use_count);
            return p;
        }

        TC_SharedPtr<const T> sharedFromThis() const
        {
            TC_SharedPtr<const T> p(m_this, m_owner_use_count);
            return p;
        }
        
    protected:
        TC_EnableSharedFromThis()
        : m_this(NULL)
        , m_owner_use_count(NULL)
        { }

        TC_EnableSharedFromThis(const TC_EnableSharedFromThis&)
        { }

        TC_EnableSharedFromThis& operator=(const TC_EnableSharedFromThis&)
        {
            return *this;
        }

        ~TC_EnableSharedFromThis()
        { }
        
    private:
        
        template <typename U>
        void acceptOwner(const TC_SharedPtr<U>& p) const
        {
            if (m_owner_use_count == NULL) 
            {
                m_owner_use_count = p.m_pn;
                m_this = p.get();
            }
        }

        mutable T *m_this;
        mutable detail::tc_shared_count_base *m_owner_use_count;

        template <class X, class U>
        friend void detail::tc_sp_enable_shared_from_this(const TC_SharedPtr<X> *pp, const TC_EnableSharedFromThis<U> *px);
    };
    
}

#endif
