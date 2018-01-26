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
//  MttConfigAgent.m
//  MttHD
//
//  Created by 壬俊 易 on 12-3-21.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttConfigAgent.h"
#import "MttHDTupEnvironment.h"

@implementation MttConfigAgent

- (id)init
{
    if (self = [super initWithServant:@"config"]) {
        self.delegate = [MttHDTupAgentHelper agentHelper];
        self.retryUsingOtherServers = YES;
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (MttPageConfigRsp *)getPageConfig:(MttPageConfigReq *)request
{
    UniPacket *uniPacket = [[[UniPacket alloc] init] autorelease];
    [uniPacket putObjectAttr:@"req" value:request];
    UniPacket *rspPacket = [self invocation:@"getPageConfig" parameter:uniPacket];
    if (rspPacket != nil)
        return [rspPacket getObjectAttr:@"rsp" forClass:[MttPageConfigRsp class]];
    return nil;
}

- (id)getPageConfig:(MttPageConfigReq *)request withCompleteHandle:(void (^)(MttPageConfigRsp *))handle
{
    UniPacket *uniPacket = [[[UniPacket alloc] init] autorelease];
    [uniPacket putObjectAttr:@"req" value:request];
    return [self invocation:@"getPageConfig" parameter:uniPacket completeHandle:^(UniPacket * response) {
        MttPageConfigRsp *rsp = [response getObjectAttr:@"rsp" forClass:[MttPageConfigRsp class]];
        handle(rsp);
    }];
}

- (MttSplashRsp *)getSplash:(MttSplashReq *)request
{
    UniPacket *uniPacket = [[[UniPacket alloc] init] autorelease];
    [uniPacket putObjectAttr:@"req" value:request];
    UniPacket *rspPacket = [self invocation:@"getSplash" parameter:uniPacket];
    if (rspPacket != nil)
        return [rspPacket getObjectAttr:@"rsp" forClass:[MttSplashRsp class]];
    return nil;
}

- (id)getSplash:(MttSplashReq *)request withCompleteHandle:(void (^)(MttSplashRsp *))handle
{
    UniPacket *uniPacket = [[[UniPacket alloc] init] autorelease];
    [uniPacket putObjectAttr:@"req" value:request];
    return [self invocation:@"getSplash" parameter:uniPacket completeHandle:^(UniPacket * response) {
        MttSplashRsp *rsp = [response getObjectAttr:@"rsp" forClass:[MttSplashRsp class]];
        handle(rsp);
    }];
}

@end
