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

#ifndef __TC_ASSERT_H__
#define __TC_ASSERT_H__

#include <cstdlib>
#include <iostream>

namespace tars 
{
    namespace detail 
    {        
        inline void tc_assertion_failed(const char *expr, const char *function, const char *file, int line)
        {
            std::cerr << "at " << file << ':' << line << ": in " << function
                      << "(): Assertion `" << expr << "' failed" << std::endl;
            std::abort();
        }

        template<bool> struct TC_CompileTimeError;
        template<> struct TC_CompileTimeError<true> {};

    }

}

#define TC_ASSERT(expr)                                                \
    ((expr) ? ((void)0) : ::tars::detail::tc_assertion_failed(#expr, __func__, __FILE__, __LINE__))

#define TC_STATIC_ASSERT(exp, msg)                                         \
    { tars::detail::TC_CompileTimeError<((exp) != 0)> ERROR_##msg; (void) ERROR_##msg; }

#endif
