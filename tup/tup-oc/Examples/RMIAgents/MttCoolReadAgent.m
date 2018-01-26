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
//  MttCoolReadAgent.m
//  MttHD
//
//  Created by bilsonzhou on 12-4-20.
//  Copyright (c) 2012年 tencent. All rights reserved.
//
#import "MttCoolReadAgent.h"
#import "MttHDTupEnvironment.h"

@implementation MttVideoAgent

- (id)init
{
    if (self = [super initWithServant:@"coolRead"]) {
        self.delegate = [MttHDTupAgentHelper proxyAgentHelper];
        self.retryUsingOtherServers = YES;
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (MttVideoResponse *)getVideo:(MttVideoRequest *)request
{
    UniPacket *uniPacket = [UniPacket packet];
    [uniPacket putObjectAttr:@"req" value:request];
    UniPacket *rspPacket = [self invocation:@"getVideo" parameter:uniPacket];
    if (rspPacket != nil)
        return [rspPacket getObjectAttr:@"VideoApp" forClass:[MttVideoResponse class]];
    return NO;
}
- (id)getVideo:(MttVideoRequest *)request withCompleteHandle:(void (^)(MttVideoResponse *))handle
{
    UniPacket *uniPacket = [UniPacket packet];
    [uniPacket putObjectAttr:@"req" value:request];
    return [self invocation:@"getVideo" parameter:uniPacket completeHandle:^(UniPacket * response) {
        MttVideoResponse *rsp = [response getObjectAttr:@"rsp" forClass:[MttVideoResponse class]];
        handle(rsp);
    }];
}

@end
