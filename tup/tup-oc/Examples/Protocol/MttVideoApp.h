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
//  MttVideoApp.h
//  MttHD
//
//  Created by bilsonzhou on 12-4-20.
//  Copyright (c) 2012年 tencent. All rights reserved.
//


#import <WirelessUnifiedProtocol/WirelessUnifiedProtocol.h>
#import <WirelessUnifiedProtocol/MttUserBase.h>
@interface MttEpisodeNode:TarsObjectV2

@property (nonatomic, retain, JV2_PROP_GS(sEpisodeUrl)) NSString* JV2_PROP_NM(o, 0, sEpisodeUrl);
@property (nonatomic, retain, JV2_PROP_GS(sEpisodeTitle)) NSString* JV2_PROP_NM(o, 1, sEpisodeTitle);

@end

@interface MttIpadViewData : TarsObjectV2

@property (nonatomic, retain, JV2_PROP_GS(sWebLink)) NSString* JV2_PROP_NM(o, 0, sWebLink);
@property (nonatomic, retain, JV2_PROP_GS(sWebLinkTitle)) NSString* JV2_PROP_NM(o, 1, sWebLinkTitle);
@property (nonatomic, retain, JV2_PROP_GS(sWebSubTitle)) NSString* JV2_PROP_NM(o, 2, sWebSubTitle);
@property (nonatomic, retain, JV2_PROP_GS(sWebCoverUrl)) NSString* JV2_PROP_NM(o, 3, sWebCoverUrl);
@property (nonatomic, retain, JV2_PROP_GS(sWebEpsInfo)) NSString* JV2_PROP_NM(o, 4, sWebEpsInfo);
@property (nonatomic, retain, JV2_PROP_GS(vEpisodeList)) NSArray* JV2_PROP_EX(o, 5, vEpisodeList, VOMttEpisodeNode);

@end

@interface MttChannel :  TarsObjectV2

@property (nonatomic, assign, JV2_PROP_GS(iAppId)) int JV2_PROP_NM(o, 0, iAppId);
@property (nonatomic, retain, JV2_PROP_GS(sCid)) NSString* JV2_PROP_NM(o, 1, sCid);
@property (nonatomic, retain, JV2_PROP_GS(sName)) NSString* JV2_PROP_NM(o, 2, sName);
@property (nonatomic, assign, JV2_PROP_GS(iOrder)) int JV2_PROP_NM(o, 3, iOrder);
@property (nonatomic, retain, JV2_PROP_GS(sIconUrl)) NSString* JV2_PROP_NM(o, 4, sIconUrl);
@property (nonatomic, retain, JV2_PROP_GS(sDescritpion)) NSString* JV2_PROP_NM(o, 5, sDescritpion);
@property (nonatomic, assign, JV2_PROP_GS(iDataFrom)) int JV2_PROP_NM(o, 6, iDataFrom);

@end

@interface MttVideoChannel :  TarsObjectV2

@property (nonatomic, retain, JV2_PROP_GS(stChannel)) MttChannel* JV2_PROP_NM(o, 0, stChannel);
@property (nonatomic, retain, JV2_PROP_GS(vVideo)) NSArray* JV2_PROP_EX(o, 1, vVideo, VOMttIpadViewData);

@end

@interface MttVideoResponse : TarsObjectV2

@property (nonatomic, retain, JV2_PROP_GS(sAppTitle)) NSString* JV2_PROP_NM(o, 0, sAppTitle);
@property (nonatomic, retain, JV2_PROP_GS(sSearchUrl)) NSString* JV2_PROP_NM(o, 1, sSearchUrl);
@property (nonatomic, assign, JV2_PROP_GS(iVideoNum)) int JV2_PROP_NM(o, 2, iVideoNum);
@property (nonatomic, retain, JV2_PROP_GS(vData)) NSArray* JV2_PROP_EX(o, 3, vData, VOMttVideoChannel);
@property (nonatomic, retain, JV2_PROP_GS(sMd5)) NSString* JV2_PROP_NM(o, 4, sMd5);

@end







