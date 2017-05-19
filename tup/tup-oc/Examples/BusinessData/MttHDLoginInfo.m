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
//  MttHDLoginInfo.m
//  MttHD
//
//  Created by 壬俊 易 on 12-4-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttHDLoginInfo.h"
#import "UIDevice+Identifier.h"
#import "MttHDCCCryptEx.h"

@implementation MttHDLoginInfo

@synthesize guid = _guid;
@synthesize auth = _auth;
@synthesize proxyList = _proxyList;
@synthesize relayList = _relayList;
@synthesize loginTime = _loginTime;
@synthesize statValue = _statValue;

+ (MttHDLoginInfo *)theSpecial
{
    static MttHDLoginInfo *special = nil;
    if (special == nil) {
        special = [[super theSpecial] retain];
    }
    return special;
}

- (id)init
{
    if (self = [super init]) {
        _guid = [[NSData alloc] initWithData:[[[UIDevice currentDevice] uniqueDeviceIdentifier] md5]];
        _auth = [[NSData alloc] init];
        _proxyList = [[NSMutableArray alloc] init];
        _relayList = [[NSMutableArray alloc] init];
        [self updateProxyList:nil];
        [self updateRelayList:nil];
    }
    return self;
}

- (void)dealloc
{
    [_guid release];
    [_auth release];
    [_proxyList release];
    [_relayList release];
    [super dealloc];
}

- (NSMutableArray *)proxyList
{
    @synchronized(self) {
        if (_proxyList && [_proxyList count] <= 0) {
#ifdef JOINT_DEBUGGING
            [_proxyList addObject:@"http://61.172.204.175:18000"];
            [_proxyList addObject:@"http://61.172.204.182:18001"];
#else /*JOINT_DEBUGGING*/
            [_proxyList addObject:@"http://117.135.128.20:28000"];
            [_proxyList addObject:@"http://117.135.128.16:28000"];
            [_proxyList addObject:@"http://mtt.3g.qq.com:28000"];
#endif /*JOINT_DEBUGGING*/
        }
    }
    return _proxyList;
}

- (void)setProxyList:(NSMutableArray *)proxyList
{
    @synchronized(self) {
        if (_proxyList != proxyList) {
            [_proxyList release];
            _proxyList = [proxyList retain];
        }
    }
}

- (NSMutableArray *)relayList
{
    @synchronized(self) {
        if (_relayList && [_relayList count] == 0) {
            [_relayList addObject:@"http://mttbro.3g.qq.com:8080"];
        }
    }
    return _relayList;
}

- (void)setRelayList:(NSMutableArray *)relayList
{
    @synchronized(self) {
        if (_relayList != relayList) {
            [_relayList release];
            _relayList = [relayList retain];
        }
    }
}

- (void)updateProxyList:(NSArray *)list
{
    @synchronized(self) {
        [_proxyList removeAllObjects];
        NSMutableArray *proxyList = [self proxyList];
        for (int i = list.count - 1; i >= 0; i--) {
            NSString *proxyUrl = [list objectAtIndex:i];
            if (![proxyUrl hasPrefix:@"http://"] && ![proxyUrl hasPrefix:@"https://"])
                proxyUrl = [NSString stringWithFormat:@"http://%@", proxyUrl];
            [proxyList removeObject:proxyUrl];
#ifdef JOINT_DEBUGGING
            [proxyList addObject:proxyUrl];
#else /*JOINT_DEBUGGING*/
            [proxyList insertObject:proxyUrl atIndex:0];
#endif /*JOINT_DEBUGGING*/
        }
    }
}

- (void)updateRelayList:(NSArray *)list
{
    @synchronized(self) {
        [_relayList removeAllObjects];        
        NSMutableArray *relayList = [self relayList];
        for (int i = list.count - 1; i >= 0; i--) {
            NSString *relayUrl = [list objectAtIndex:i];
            if (![relayUrl hasPrefix:@"http://"] && ![relayUrl hasPrefix:@"https://"])
                relayUrl = [NSString stringWithFormat:@"http://%@", relayUrl];
            [relayList removeObject:relayUrl];
#ifdef JOINT_DEBUGGING
            [relayList addObject:relayUrl];
#else /*JOINT_DEBUGGING*/
            [relayList insertObject:relayUrl atIndex:0];
#endif /*JOINT_DEBUGGING*/
        }
        
    }
}

@end
