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
//  MttStatAgent.m
//  MttHD
//
//  Created by 壬俊 易 on 12-3-21.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttStatAgent.h"
#import "MttHDCCCryptEx.h"
#import "MttHDTupEnvironment.h"

#define MTT_STAT_KEY "\x62\xe8\x39\xac\x8d\x75\x37\x79"

@implementation MttStatAgent

- (id)init
{
    if (self = [super initWithServant:@"stat"]) {
        self.delegate = [MttHDTupAgentHelper agentHelper];
        self.retryUsingOtherServers = YES;
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (bool)stat:(MttSTStat *)request
{
    UniPacket *uniPacket = [UniPacket packet];
    NSString *key = [NSString stringWithCString:MTT_STAT_KEY encoding:NSISOLatin1StringEncoding];
    [uniPacket putDataAttr:@"crypt" value:[[request toData] desWithKey:key]];
    UniPacket *rspPacket = [self invocation:@"stat" parameter:uniPacket];
    if (rspPacket != nil)
        return [NSNumber boolValueWithName:@"" inAttributes:rspPacket.attributes];
    return NO;
}

- (id)stat:(MttSTStat *)request withCompleteHandle:(void (^)(bool))handle
{    
    UniPacket *uniPacket = [UniPacket packet];
    NSString *key = [NSString stringWithCString:MTT_STAT_KEY encoding:NSISOLatin1StringEncoding];
    [uniPacket putDataAttr:@"crypt" value:[[request toData] desWithKey:key]];
    return [self invocation:@"stat" parameter:uniPacket completeHandle:^(UniPacket * response) {
        int value = [NSNumber intValueWithName:@"" inAttributes:response.attributes];
        handle((value == 0 ? YES : NO));
    }];
}

@end
