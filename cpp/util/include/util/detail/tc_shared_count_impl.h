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

#ifndef __TC_SHARED_COUNT_IMPL_H__
#define __TC_SHARED_COUNT_IMPL_H__

#include "tc_shared_count_base.h"
#include "tc_checked_delete.h"

namespace tars 
{
    namespace detail 
    {    
        template <typename T>
        class tc_shared_count_impl_p : public tc_shared_count_base 
        {
        public:

            template <typename U>
            tc_shared_count_impl_p(U *p)
            : m_px(p)
            {}

            virtual void dispose()
            {
                tc_checked_delete(m_px);
            }
            
        private:

            T *m_px;
        };

        template <typename T, typename D>
        class tc_shared_count_impl_pd : public tc_shared_count_base 
        {
        public:

            tc_shared_count_impl_pd(T *p, D& d)
            : m_px(p)
            , m_deleter(d)
            {}

            virtual void dispose()  // no throw
            {
                m_deleter(m_px);
            }

        private:

            T *m_px;
            D m_deleter;
        };

    }
}

#endif
