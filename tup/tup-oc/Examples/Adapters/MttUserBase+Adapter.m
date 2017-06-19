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
//  MttUserBase+Adapter.m
//  MttHD
//
//  Created by 壬俊 易 on 12-4-19.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttUserBase+adapter.h"
#import "MttHDTupManager.h"

@implementation MttUserBase (adapter)

+ (id)mtthdInstance 
{
    MttUserBase *instance = [MttUserBase object];
    instance.tars_sIMEI = [MttHDTupEnvironment imei];
    instance.tars_sGUID = [MttHDLoginInfo theSpecial].guid;
    instance.tars_sQUA = [MttHDTupEnvironment qUA];
    instance.tars_sLC = [MttHDTupEnvironment lc];
    instance.tars_sCellphone = @"";
    instance.tars_sUin = [MttHDTupEnvironment qqAccount];
    instance.tars_sChannel = [MttHDTupEnvironment channel];
    instance.tars_iServerVer = MttServerVer_IPAD;
    return instance;
}

@end
