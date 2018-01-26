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
//  MttPageConfigReq+Adapter.m
//  MttHD
//
//  Created by 壬俊 易 on 12-4-3.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttPageConfigReq+Adapter.h"
#import "MttHDTupAdapters.h"
#import <WirelessUnifiedProtocol/MttPageBrowserReq.h>
#import <WirelessUnifiedProtocol/MttSearchReq.h>
#import <WirelessUnifiedProtocol/MttStaticConfigReq.h>
#import "MttHDPageConfigInfo.h"

#pragma mark - MttPageBrowserReq

@interface MttPageBrowserReq (adapter)

+ (id)mtthdInstance;

@end

@implementation MttPageBrowserReq (adapter)

+ (id)mtthdInstance
{
    MttPageBrowserReq *instance = [MttPageBrowserReq object];
    MttHDPageConfigInfo *pageConfigInfo = [MttHDPageConfigInfo theSpecial];
    
    instance.tars_version = MttBrowserVersion_NEWWAP2;
    instance.tars_size = 26;
    instance.tars_lastVersionMD5 = pageConfigInfo.md5StringOfStartPage;
    instance.tars_special = MTTSpecialValue_ONELINK;
    return instance;
}

@end

#pragma mark - MttSearchReq

@interface MttSearchReq (adapter)

+ (id)mtthdInstance;

@end

@implementation MttSearchReq (adapter)

+ (id)mtthdInstance
{
    MttSearchReq *instance = [MttSearchReq object];
    MttHDPageConfigInfo *pageConfigInfo = [MttHDPageConfigInfo theSpecial];
    
    instance.tars_version = 2;
    instance.tars_iSearchTime = pageConfigInfo.searchLastUpdateTime;
    return instance;
}

@end

#pragma mark - MttStaticConfigReq

@interface MttStaticConfigReq (adapter)

+ (id)mtthdInstance;

@end

@implementation MttStaticConfigReq (adapter)

+ (id)mtthdInstance
{
    MttStaticConfigReq *instance = [MttStaticConfigReq object];
    MttHDPageConfigInfo *pageConfigInfo = [MttHDPageConfigInfo theSpecial];
    instance.tars_iKeywordTime = pageConfigInfo.keywordLastUpdateTime;
    instance.tars_iDomainTime = pageConfigInfo.domainLastUpdateTime;
    instance.tars_iPluginTime = pageConfigInfo.pluginLastUpdateTime;
    instance.tars_iConfTime = pageConfigInfo.configLastUpdateTime;
    return instance;
}

@end

#pragma mark - MttPageConfigReq

@implementation MttPageConfigReq (adapter)

+ (id)mtthdInstance
{
    MttPageConfigReq *instance = [MttPageConfigReq object];
    instance.tars_stUserBase = [MttUserBase mtthdInstance];
    instance.tars_stBrowserReq = [MttPageBrowserReq mtthdInstance];
    instance.tars_stSearchReq = [MttSearchReq mtthdInstance];
    instance.tars_stStaticConfigReq = [MttStaticConfigReq mtthdInstance];
    return instance;
}

@end
