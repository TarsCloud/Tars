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

#ifndef _UTIL_H
#define _UTIL_H

#include "util/tc_common.h"

#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)

#define FILE_FUN   __FILENAME__<<":"<<__FUNCTION__<<":"<<__LINE__<<"|"
#define FILE_FUN_STR  TC_Common::tostr(__FILENAME__)+":"+TC_Common::tostr(__FUNCTION__)+":"+TC_Common::tostr(__LINE__)+"|"

#endif
