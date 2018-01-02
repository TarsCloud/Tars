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
//  MttLoginService.m
//
//  Created by 壬俊 易 on 12-3-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttLoginService.h"
#import "MttLoginAgent.h"
#import "MttHDTupAdapters.h"
#import "MttHDTupManager.h"
#import "MttHDTupEnvironment.h"

@implementation MttLoginService

- (id)init
{
	if (self = [super initWithAgent:[MttLoginAgent agent]]) {
	}
	return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (id)loginWithCompleteBlock:(TupServiceBasicBlock)completeBlock failedBlock:(TupServiceBasicBlock)failedBlock;
{
    MttLoginAgent *agent = (MttLoginAgent *)[self agent];
    id stub = [agent login:[MttLoginReq mtthdInstance] withCompleteHandle:^(MttLoginRsp * rsp) {
        if (rsp != nil) {
            MttHDLoginInfo *loginInfo = [MttHDLoginInfo theSpecial];
            loginInfo.guid = rsp.sGUID;
            loginInfo.auth = rsp.vAuth;
            loginInfo.loginTime = rsp.iTime.intValue;
            loginInfo.statValue = rsp.iStatValue.intValue;
            [loginInfo updateProxyList:rsp.vProxyList];
            [loginInfo updateRelayList:rsp.vRelayList];
            [loginInfo save];
            if (completeBlock) 
                completeBlock();
        }
        else {
            if (failedBlock)
                failedBlock();
        }
    }];
    return stub;
}

- (id)updateWithCompleteBlock:(void (^)(MttUpdateRsp *))completeBlock failedBlock:(TupServiceBasicBlock)failedBlock
{
    MttLoginAgent *agent = (MttLoginAgent *)[self agent];
    id stub = [agent update:[MttUserBase mtthdInstance] withCompleteHandle:^(MttUpdateRsp * rsp) {
        if (rsp != nil) {
            NSMutableDictionary *userInfo = [NSMutableDictionary dictionary];
            [userInfo setObject:rsp.iStatus forKey:@"status"];
            [userInfo setObject:rsp.sMark forKey:@"title"];
            [userInfo setObject:rsp.sText forKey:@"prompt"];
            [userInfo setObject:rsp.sURL forKey:@"url"];
            // TODO:renjunyi
            if (completeBlock) 
                completeBlock(rsp);
        }
        else {
            if (failedBlock)
                failedBlock();
        }
    }];
    return stub;
}

@end
