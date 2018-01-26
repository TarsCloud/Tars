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
//  MttStatService.m
//  MttHD
//
//  Created by 壬俊 易 on 12-3-19.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttStatService.h"
#import "MttStatAgent.h"
#import "MttHDTupAdapters.h"
#import "MttSTStat+Adapter.h"
#import "MttHDTupManager.h"

@implementation MttStatService

- (id)init
{
	if (self = [super initWithAgent:[MttStatAgent agent]]) {
	}
	return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (id)statWithCompleteBlock:(TupServiceBasicBlock)completeBlock failedBlock:(TupServiceBasicBlock)failedBlock
{
    MttStatAgent *agent = (MttStatAgent *)[self agent];
    id stub = [agent stat:[MttSTStat mtthdInstance] withCompleteHandle:^(bool ret) {
        if (ret != NO) {
            [MttSTStat mtthdInstanceReset];
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

@end
