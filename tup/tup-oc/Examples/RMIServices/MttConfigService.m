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
//  MttConfigService.m
//  MttHD
//
//  Created by 壬俊 易 on 12-3-19.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttConfigService.h"
#import "MttConfigAgent.h"
#import "MttHDTupManager.h"
#import "MttHDTupEnvironment.h"
#import "MttPageConfigReq+Adapter.h"
#import "MttSplashReq+Adapter.h"
#import "MttHDPageConfigInfo.h"
#import "MttHDSplashInfo.h"

@implementation MttConfigService

- (id)init
{
	if (self = [super initWithAgent:[MttConfigAgent agent]]) {
	}
	return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (id)getPageConfigWithCompleteBlock:(TupServiceBasicBlock)completeBlock failedBlock:(TupServiceBasicBlock)failedBlock
{
    MttConfigAgent *agent = (MttConfigAgent *)[self agent];
    id stub = [agent getPageConfig:[MttPageConfigReq mtthdInstance] withCompleteHandle:^(MttPageConfigRsp * rsp) {
        if (rsp != nil) {
            MttHDPageConfigInfo *pageConfigInfo = [MttHDPageConfigInfo theSpecial];
            if (rsp.tars_stBrowserRsp.tars_resultCode == 0 && [rsp.tars_stBrowserRsp.tars_content length] > 0) {
                pageConfigInfo.md5StringOfStartPage = rsp.tars_stBrowserRsp.tars_lastVersionMD5;
                pageConfigInfo.startPageContent = rsp.tars_stBrowserRsp.tars_content;
                [[MttHDTupManager defaultNotificationCenter] postNotificationName:MttHDTupStartPageUpdated object:nil];
            }
            if (rsp.tars_stSearchRsp.tars_resultCode == 0) {
                pageConfigInfo.searchLastUpdateTime = rsp.tars_stSearchRsp.tars_iSearchTime;
            }
            pageConfigInfo.keywordLastUpdateTime = rsp.tars_stKeywordRsp.tars_iKeywordTime;
            pageConfigInfo.domainLastUpdateTime = rsp.tars_stDomainRsp.tars_iDomainTime;
            pageConfigInfo.innerUrls = rsp.tars_stDomainRsp.tars_vDomain;
            
            NSMutableArray *safeUrls = [NSMutableArray array];
            for (MttExtDomain *extDomain in rsp.tars_stDomainRsp.tars_vExtDomain)
                if (extDomain.tars_eType == MttDomainTypeSafeWhiteList4IPAD)
                    [safeUrls addObject:extDomain.tars_sExtDomain];
            pageConfigInfo.safeUrls = safeUrls;
            
            pageConfigInfo.pluginLastUpdateTime = rsp.tars_stPluginRsp.tars_iPluginTime;
            pageConfigInfo.configLastUpdateTime = rsp.tars_stConfRsp.tars_iConfTime;
            
            NSMutableDictionary *configInfoDictionary = [NSMutableDictionary dictionary];
            for (MttConfInfo *configInfo in rsp.tars_stConfRsp.tars_vConfInfo)
                [configInfoDictionary setValue:configInfo.tars_sValue forKey:configInfo.tars_sKey];
            pageConfigInfo.configInfoDictionary = configInfoDictionary;
            
            [pageConfigInfo save];
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

- (id)getSplashWithCompleteBlock:(TupServiceBasicBlock)completeBlock failedBlock:(TupServiceBasicBlock)failedBlock
{
    MttConfigAgent *agent = (MttConfigAgent *)[self agent];
    id stub = [agent getSplash:[MttSplashReq mtthdInstance] withCompleteHandle:^(MttSplashRsp * rsp) {
        if (rsp != nil) {
            @try {
                UIImage *image = [UIImage imageWithData:rsp.tars_vPicData];
                if (image != nil) {
                    MttHDSplashInfo *splashInfo = [MttHDSplashInfo theSpecial];
                    splashInfo.splashImage = image;
                    splashInfo.md5StringOfSplashImage = rsp.tars_sPicMd5;
                    splashInfo.md5StringOfSplashConfig = rsp.tars_sCondMd5;
                    splashInfo.startTime = [NSDate dateWithTimeIntervalSince1970:rsp.tars_iEffectTime];
                    splashInfo.expirationTime = [NSDate dateWithTimeIntervalSince1970:rsp.tars_iInvalidTime];
                    splashInfo.duration = rsp.tars_iShowSecond;
                    [splashInfo save];
                }
                if (completeBlock) 
                    completeBlock();
            }
            @catch (NSException *exception) {
                NSLog(@"%@", exception);
            }
        }
        else {
            if (failedBlock)
                failedBlock();
        }
    }];
    return stub;
}

@end
