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

//
//  tup-Debug.h
//  tup
//
//  Created by 壬俊 易 on 12-6-13.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#ifndef tup_tup_Debug_h
#define tup_tup_Debug_h

#define ASSERT_FAIL_TRHOW_TARS_SERIALIZABLE_EXCEPTION(x)                         \
do {                                                                            \
    if (!(x)) {                                                                 \
        NSString *reason = [NSString stringWithFormat:@"%s(%d) %s: assert(%@) fail!", __FILE__, __func__, __LINE__, @#x] userInfo:nil]; \
        NSException *exception = [NSException exceptionWithName:@"TarsSerializableException" reason:reason]; \
        @throw exception;                                                       \
    }                                                                           \
} while(0)

#endif
