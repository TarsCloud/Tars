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
//  TupAgent.h
//
//  Created by 壬俊 易 on 11-11-29.
//  Copyright (c) 2011年 Tencent. All rights reserved.
//

#import "TupAgentDelegate.h"

@class UniPacket;

/**
 * 远程方法调用代理类TupAgent，提供对基于TUP协议封装的远程方法调用的支持
 * 对于远程方法调用，需要指定servantName和funcName，调用参数和返回值通过uniPacket传递
 */
@interface TupAgent : NSObject

@property (nonatomic, readonly) NSString *servantName;
@property (nonatomic, retain) NSObject<TupAgentDelegate> *delegate;
@property (nonatomic, assign) BOOL retryUsingOtherServers;

+ (id)agent;
- (id)initWithServant:(NSString *)name;

// 同步调用远程方法
- (UniPacket *)invocation:(NSString *)funcName parameter:(UniPacket *)uniPacket;

// 异步调用远程方法，handle将在一个"a well-known global concurrent queue"中被调用
- (id)invocation:(NSString *)funcName parameter:(UniPacket *)uniPacket completeHandle:(void (^)(UniPacket *))handle;

// 取消远程方法调用
+ (void)cancel:(id)stub;

@end
