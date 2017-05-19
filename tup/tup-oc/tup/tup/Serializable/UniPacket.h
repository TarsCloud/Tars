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
//  UniPacket.h
//
//  Created by 壬俊 易 on 11-12-1.
//  Copyright (c) 2011年 Tencent. All rights reserved.
//

#import "RequestF.h"
#import "TarsObjectV2.h"
#import "UniAttribute.h"

@interface UniPacket : RequestPacket

@property (nonatomic, readonly) UniAttribute* attributes;

+ (UniPacket *)packet;

#pragma mark - deprecated

// 下面的这些接口已经废弃不用了，要设置/获取参数，建议使用UniAttribute.h中的方法
- (id)getObjectAttr:(NSString *)attrName forClass:(Class)theClass;
- (void)putDataAttr:(NSString *)attrName value:(NSData *)attrValue;
- (void)putObjectAttr:(NSString *)attrName value:(TarsObjectV2 *)attrValue;
- (void)putObjectAttr:(NSString *)attrName type:(NSString *)attrType value:(TarsObject *)attrValue;

@end
