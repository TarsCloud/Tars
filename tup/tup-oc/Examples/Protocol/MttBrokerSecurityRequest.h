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
//  MttBrokerSecurityRequest.h
//  MttHD
//
//  Created by 壬俊 易 on 12-4-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <WirelessUnifiedProtocol/WirelessUnifiedProtocol.h>
#import <WirelessUnifiedProtocol/MttUserBase.h>

enum {
    MttSecurityCheckType_WEBURL,    //网址检测
    MttSecurityCheckType_SOFT       //资源检测 
};
#define MttSecurityCheckType NSInteger

@interface MttBrokerSecurityRequest : TarsObjectV2

@property (nonatomic, retain, JV2_PROP_GS(stUserInfo)) MttUserBase* JV2_PROP_NM(o, 0, stUserInfo);
@property (nonatomic, assign, JV2_PROP_GS(cType)) MttSecurityCheckType JV2_PROP_NM(r, 1, cType);
@property (nonatomic, retain, JV2_PROP_GS(strUrl)) NSString* JV2_PROP_NM(o, 2, strUrl);
@property (nonatomic, retain, JV2_PROP_GS(strSha1)) NSString* JV2_PROP_NM(o, 3, strSha1);

@end

@interface MttBrokerSecurityRequest (adapter)

+ (id)mtthdInstance;

@end
