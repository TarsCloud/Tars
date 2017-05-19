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
//  MttLoginAgent.m
//
//  Created by 壬俊 易 on 12-3-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttLoginAgent.h"
#import "MttHDTupEnvironment.h"

@implementation MttLoginAgent

- (id)init
{
    if (self = [super initWithServant:@"login"]) {
        self.delegate = [MttHDTupAgentHelper agentHelper];
        self.retryUsingOtherServers = YES;
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (MttLoginRsp *)login:(MttLoginReq *)request 
{
    UniPacket *uniPacket = [[[UniPacket alloc] init] autorelease];
    [uniPacket putObjectAttr:@"stLReq" type:@"MTT.LoginReq" value:request];
    UniPacket *rspPacket = [self invocation:@"login" parameter:uniPacket];
    if (rspPacket != nil)
        return [rspPacket getObjectAttr:@"stLRsp" forClass:[MttLoginRsp class]];
    return nil;
}

- (id)login:(MttLoginReq *)request withCompleteHandle:(void (^)(MttLoginRsp *))handle
{
    UniPacket *uniPacket = [[[UniPacket alloc] init] autorelease];
    [uniPacket putObjectAttr:@"stLReq" type:@"MTT.LoginReq" value:request];
    return [self invocation:@"login" parameter:uniPacket completeHandle:^(UniPacket * response) {
        MttLoginRsp *rsp = [response getObjectAttr:@"stLRsp" forClass:[MttLoginRsp class]];
        handle(rsp);
    }];
}

- (MttUpdateRsp *)update:(MttUserBase *)request 
{
    UniPacket *uniPacket = [[[UniPacket alloc] init] autorelease];
    [uniPacket putObjectAttr:@"ub" type:@"MTT.UserBase" value:request];
    UniPacket *rspPacket = [self invocation:@"update" parameter:uniPacket];
    if (rspPacket != nil)
        return [rspPacket getObjectAttr:@"uRsp" forClass:[MttUpdateRsp class]];
    return nil;
}

- (id)update:(MttUserBase *)request withCompleteHandle:(void (^)(MttUpdateRsp *))handle
{
    UniPacket *uniPacket = [[[UniPacket alloc] init] autorelease];
    [uniPacket putObjectAttr:@"ub" type:@"MTT.UserBase" value:request];
    return [self invocation:@"update" parameter:uniPacket completeHandle:^(UniPacket * response) {
        MttUpdateRsp *rsp = [response getObjectAttr:@"uRsp" forClass:[MttUpdateRsp class]];
        handle(rsp);
    }];
}

@end
