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
//  TarsEnumHelper.h
//  tup
//
//  Created by 壬俊 易 on 12-6-13.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <Foundation/Foundation.h>

BOOL isTarsEnumStringEqual(NSString *s1, NSString *s2);

#define TarsEnum             NSInteger
#define TarsEnumUndefined    NSIntegerMin

@interface TarsEnumHelper : NSObject

+ (NSString *)etos:(TarsEnum)e;
+ (TarsEnum)stoe:(NSString *)s;

/**
 * 以下接口用于转换不带枚举类型名前缀的枚举值字符串
 * 假设枚举值定义为TarsEnum_Value1，此接口对应处理的字符串是"Value1"
 */
+ (NSString *)eto_s:(TarsEnum)e;
+ (TarsEnum)_stoe:(NSString *)s;

@end
