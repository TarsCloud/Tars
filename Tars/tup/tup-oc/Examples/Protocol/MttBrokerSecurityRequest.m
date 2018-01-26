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
//  MttBrokerSecurityRequest.m
//  MttHD
//
//  Created by 壬俊 易 on 12-4-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttBrokerSecurityRequest.h"

@implementation MttBrokerSecurityRequest

@synthesize JV2_PROP_NM(o, 0, stUserInfo);
@synthesize JV2_PROP_NM(r, 1, cType);
@synthesize JV2_PROP_NM(o, 2, strUrl);
@synthesize JV2_PROP_NM(o, 3, strSha1);

- (id)init
{
	if (self = [super init]) {
	}
	return self;
}

- (void)dealloc
{
    self.tars_stUserInfo = nil;
    self.tars_strUrl = nil;
    self.tars_strSha1 = nil;
    [super dealloc];
}

+ (NSString *)tarsType
{
    return @"MTT.BrokerSecurityRequest";
}

@end

@implementation MttBrokerSecurityRequest (adapter)

+ (id)mtthdInstance
{
    MttBrokerSecurityRequest *instance = [MttBrokerSecurityRequest object];
    return instance;
}

@end
