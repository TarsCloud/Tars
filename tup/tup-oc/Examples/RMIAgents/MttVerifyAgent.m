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
//  MttVerifyAgent.m
//  MttHD
//
//  Created by 壬俊 易 on 12-3-19.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttVerifyAgent.h"
#import "MttHDCCCryptEx.h"
#import "MttHDTupEnvironment.h"

#define MTT_VERIFY_KEY "\x25\x92\x3c\x7f\x2a\xe5\xef\x92"

@implementation MttVerifyAgent

- (id)init
{
    if (self = [super initWithServant:@"v"]) {
        self.delegate = [MttHDTupAgentHelper agentHelper];
        self.retryUsingOtherServers = YES;
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (MttVerifyRsp *)verify:(MttVerifyReq *)request
{
    UniPacket *uniPacket = [UniPacket packet];
    NSString *key = [NSString stringWithCString:MTT_VERIFY_KEY encoding:NSISOLatin1StringEncoding];
    [uniPacket putDataAttr:@"v" value:[[request toData] desWithKey:key]];
    UniPacket *rspPacket = [self invocation:@"verify" parameter:uniPacket];
    if (rspPacket != nil)
        return [rspPacket getObjectAttr:@"rsp" forClass:[MttVerifyRsp class]];
    return nil;
}

- (id)verify:(MttVerifyReq *)request withCompleteHandle:(void (^)(MttVerifyRsp *))handle
{
    UniPacket *uniPacket = [UniPacket packet];
    NSString *key = [NSString stringWithCString:MTT_VERIFY_KEY encoding:NSISOLatin1StringEncoding];
    [uniPacket putDataAttr:@"v" value:[[request toData] desWithKey:key]];
    return [self invocation:@"verify" parameter:uniPacket completeHandle:^(UniPacket * response) {
        MttVerifyRsp *rsp = [response getObjectAttr:@"rsp" forClass:[MttVerifyRsp class]];
        handle(rsp);
    }];
}

@end
