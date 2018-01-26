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

#ifndef __TC_SCOPED_PTR_H__
#define __TC_SCOPED_PTR_H__

#include "detail/tc_assert.h"
#include "detail/tc_checked_delete.h"

namespace tars
{

    template <typename T>
    class TC_ScopedPtr 
    {
    public:
        typedef T element_type;
        typedef T* (TC_ScopedPtr::*unspecified_bool_type)() const;
        
        explicit TC_ScopedPtr(T *p = NULL)
        : m_p(p)
        { }
            
        ~TC_ScopedPtr()
        {
            tc_checked_delete(m_p);
        }

        T& operator*() const
        {
            TC_ASSERT(m_p != NULL);
            return *m_p;
        }
                
        T *operator->() const
        {
            TC_ASSERT(m_p != NULL);
            return m_p;
        }
        
        T *get() const
        {
            return m_p;
        }

        operator unspecified_bool_type() const
        {
            return m_p ? &TC_ScopedPtr::get : 0;
        }

        void reset(T *p = NULL)
        {
            TC_ScopedPtr(p).swap(*this);
        }

        void swap(TC_ScopedPtr& other)
        {
            std::swap(m_p, other.m_p);
        }

    private:
        TC_ScopedPtr(const TC_ScopedPtr&);
        void operator=(const TC_ScopedPtr&);
        
        T *m_p;
    };

}

#endif
