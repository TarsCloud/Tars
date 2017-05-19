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
//  MttSTStat+Adapter.m
//  MttHD
//
//  Created by 壬俊 易 on 12-4-10.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttSTStat+Adapter.h"
#import <WirelessUnifiedProtocol/MttLoginRsp.h>
#import "MttHDTupAdapters.h"
#import "MttHDStatInfo.h"

@implementation MttSTTime (Adapter)

+ (id)mtthdInstance
{
    MttSTTime *instance = [MttSTTime object];
    instance.tars_iAvgProxyTime = 0;
    instance.tars_iAvgDirectTime = 0;
    return instance;
}

@end

@implementation MttSTStat (Adapter)

+ (id)mtthdInstance
{
    MttSTStat *instance = [MttSTStat object];
    MttHDStatInfo *statInfo = [MttHDStatInfo theSpecial];
    instance.tars_stUB = [MttUserBase mtthdInstance];
    instance.tars_stTime = [MttSTTime mtthdInstance];
    instance.tars_stTotal = [statInfo totalPV];
    instance.tars_stOuterPv = statInfo.outerPV;
    instance.tars_stInnerPv = statInfo.innerPV;
    instance.tars_iUseTime = statInfo.useTime;
    instance.tars_vEntryPv = statInfo.entryPV;
    instance.tars_sProtocol = statInfo.protocol;
    return instance;
}

+ (void)mtthdInstanceReset
{
    MttHDStatInfo* oldStatInfo = [MttHDStatInfo clearTheSpecial];
    MttHDStatInfo *statInfo = [MttHDStatInfo theSpecial];
    [statInfo setMaximumTabs:oldStatInfo.maximumTabs];
}

@end
