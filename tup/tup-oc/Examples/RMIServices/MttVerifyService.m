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
//  MttVerifyService.m
//  MttHD
//
//  Created by 壬俊 易 on 12-3-19.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttVerifyService.h"
#import "MttVerifyAgent.h"
#import "MttHDTupAdapters.h"
#import "MttHDTupManager.h"
#import "MttHDTupEnvironment.h"

@implementation MttVerifyService 

- (id)init
{
	if (self = [super initWithAgent:[MttVerifyAgent agent]]) {
	}
	return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (id)verifyWithCompleteBlock:(TupServiceBasicBlock)completeBlock failedBlock:(TupServiceBasicBlock)failedBlock
{
    id stub = nil;
    
    MttHDLoginInfo *loginInfo = [MttHDLoginInfo theSpecial];
    if (loginInfo.auth.length > 0) {
        if (completeBlock)
            completeBlock();
    }
    else {
        MttVerifyAgent *agent = (MttVerifyAgent *)[self agent];
        stub = [agent verify:[MttVerifyReq mtthdInstance] withCompleteHandle:^(MttVerifyRsp * rsp) {
            if (rsp != nil) {
                MttHDVerifyInfo* verifyInfo = [MttHDVerifyInfo theSpecial];
                verifyInfo.verifyId = rsp.iVerifyId.intValue;
                verifyInfo.parameter = rsp.sParameter;
                [verifyInfo save];
                if (completeBlock)
                    completeBlock();
            }
            else {
                if (failedBlock)
                    failedBlock();
            }
        }];
    }
    return stub;
}

@end
