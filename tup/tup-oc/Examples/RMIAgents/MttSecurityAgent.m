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
//  MttSecurityAgent.m
//  MttHD
//
//  Created by 壬俊 易 on 12-4-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttSecurityAgent.h"
#import "MttHDTupEnvironment.h"

@implementation MttSecurityAgent

- (id)init
{
    if (self = [super initWithServant:@"Security"]) {
        self.delegate = [MttHDTupAgentHelper relayAgentHelper];
        self.retryUsingOtherServers = YES;
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (MttBrokerSecurityResponse *)security:(MttBrokerSecurityRequest *)request
{
    UniPacket *uniPacket = [UniPacket packet];
    [uniPacket putObjectAttr:@"Security" value:request];
    UniPacket *rspPacket = [self invocation:@"Security" parameter:uniPacket];
    if (rspPacket != nil)
        return [rspPacket getObjectAttr:@"BrokerSecurityResponse" forClass:[MttBrokerSecurityResponse class]];
    return NO;
}

- (id)security:(MttBrokerSecurityRequest *)request withCompleteHandle:(void (^)(MttBrokerSecurityResponse *))handle
{    
    UniPacket *uniPacket = [UniPacket packet];
    [uniPacket putObjectAttr:@"Security" value:request];
    return [self invocation:@"Security" parameter:uniPacket completeHandle:^(UniPacket * response) {
        MttBrokerSecurityResponse *rsp = [response getObjectAttr:@"BrokerSecurityResponse" forClass:[MttBrokerSecurityResponse class]];
        handle(rsp);
    }];
}

@end
