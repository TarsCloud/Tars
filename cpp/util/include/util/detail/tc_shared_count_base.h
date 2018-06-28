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

#ifndef __TC_SHARED_COUNT_BASE_H__
#define __TC_SHARED_COUNT_BASE_H__

#include "../tc_atomic.h"

namespace tars 
{
    namespace detail 
    {
        class tc_shared_count_base 
        {
        public:

            tc_shared_count_base()
            : m_use_count(1)
            {}

            virtual ~tc_shared_count_base()
            {}
            
            int use_count() const
            {
                return m_use_count.get();
            }

            void increment()
            {
                ++m_use_count;
            }

            void release()
            {
                if (m_use_count.dec_and_test()) 
                {
                    dispose();
                    delete this;
                }
            }

            virtual void dispose() = 0;
            
        private:
            tc_shared_count_base(const tc_shared_count_base&);
            void operator=(const tc_shared_count_base&);
            
            tars::TC_Atomic m_use_count;
        };

    }
}

#endif
