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
//  TarsObject.h
//
//  Created by godwin.guo on 11-9-29. Modified by renjunyi on 11-12-1.
//  Copyright (c) 2011年 Tencent. All rights reserved.
//

#import "TarsInputStream.h"
#import "TarsOutputStream.h"
#import "TarsEnumHelper.h"

#pragma mark -

typedef BOOL                    TarsBool;
typedef char                    TarsInt8;
typedef unsigned char           TarsUInt8;
typedef short                   TarsInt16;
typedef unsigned short          TarsUInt16;
typedef int                     TarsInt32;
typedef unsigned int            TarsUInt32;
typedef long long               TarsInt64;
typedef unsigned long long      TarsUInt64;
typedef float                   TarsFloat;
typedef double                  TarsDouble;

#define AUTO_COLLECT            NSAutoreleasePool *autoReleasePool = [[NSAutoreleasePool alloc] init];
#define AUTO_RELEASE            [autoReleasePool release];

#define DefaultTarsString        @""
#define DefaultTarsData          [NSData data]
#define DefaultTarsArray         [NSArray array]
#define DefaultTarsDictionary    [NSDictionary dictionary]

#pragma mark - 

@interface TarsObject : NSObject <NSCoding>

+ (id)object;
+ (NSDictionary *)tarsPropertiesWithEncodedTypes;

+ (id)fromData:(NSData *)data;
- (id)fromData:(NSData *)data;
- (NSData *)toData;

+ (NSString *)tarsType;
- (NSString *)tarsType;

- (void)__pack:(TarsOutputStream *)stream;	// !!! INTERNAL USE ONLY
- (void)__unpack:(TarsInputStream *)stream;	// !!! INTERNAL USE ONLY

@end
