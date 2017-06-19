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
//  MttVideoApp.m
//  MttHD
//
//  Created by bilsonzhou on 12-4-20.
//  Copyright (c) 2012年 tencent. All rights reserved.
//


#import "MttVideoApp.h"

@implementation MttEpisodeNode

@synthesize  JV2_PROP_NM(o, 0, sEpisodeUrl);
@synthesize  JV2_PROP_NM(o, 1, sEpisodeTitle);

- (id)init
{
	if (self = [super init]) {
	}
	return self;
}

- (void)dealloc
{
    self.tars_sEpisodeUrl = nil;
    self.tars_sEpisodeTitle = nil;
    [super dealloc];
}

+ (NSString *)tarsType
{
    return @"MTT.CoolReadEpisodeStruct";
}

@end

@implementation MttEpisodeNode (adapter)

+ (id)mtthdInstance
{
    MttEpisodeNode *instance = [MttEpisodeNode object];
    return instance;
}

@end

@implementation MttIpadViewData

@synthesize  JV2_PROP_NM(o, 0, sWebLink);
@synthesize  JV2_PROP_NM(o, 1, sWebLinkTitle);
@synthesize  JV2_PROP_NM(o, 2, sWebSubTitle);
@synthesize  JV2_PROP_NM(o, 3, sWebCoverUrl);
@synthesize  JV2_PROP_NM(o, 4, sWebEpsInfo);
@synthesize  JV2_PROP_EX(o, 5, vEpisodeList, VOMttEpisodeNode);

- (id)init
{
	if (self = [super init]) {
	}
	return self;
}

- (void)dealloc
{
    self.tars_sWebLink = nil;
    self.tars_sWebLinkTitle = nil;
    self.tars_sWebSubTitle = nil;
    self.tars_sWebCoverUrl = nil;
    self.tars_sWebEpsInfo = nil;
    self.tars_vEpisodeList = nil;
    [super dealloc];
}

+ (NSString *)tarsType
{
    return @"MTT.IpadViewData";
}

@end

@implementation MttIpadViewData (adapter)

+ (id)mtthdInstance
{
    MttIpadViewData *instance = [MttIpadViewData object];
    return instance;
}

@end

@implementation MttChannel


@synthesize  JV2_PROP_NM(o, 0, iAppId);
@synthesize  JV2_PROP_NM(o, 1, sCid);
@synthesize  JV2_PROP_NM(o, 2, sName);
@synthesize  JV2_PROP_NM(o, 3, iOrder);
@synthesize  JV2_PROP_NM(o, 4, sIconUrl);
@synthesize  JV2_PROP_NM(o, 5, sDescritpion);
@synthesize  JV2_PROP_NM(o, 6, iDataFrom);

- (id)init
{
	if (self = [super init]) {
	}
	return self;
}

- (void)dealloc
{
    self.tars_sCid = nil;
    self.tars_sName = nil;
    self.tars_sIconUrl = nil;
    self.tars_sDescritpion = nil;
    [super dealloc];
}

+ (NSString *)tarsType
{
    return @"MTT.Channel";
}

@end

@implementation MttChannel (adapter)

+ (id)mtthdInstance
{
    MttChannel *instance = [MttChannel object];
    return instance;
}

@end

@implementation MttVideoChannel

@synthesize JV2_PROP_NM(o, 0, stChannel);
@synthesize JV2_PROP_EX(o, 1, vVideo, VOMttIpadViewData);

- (id)init
{
	if (self = [super init]) {
	}
	return self;
}

- (void)dealloc
{
    self.tars_stChannel = nil;
    self.tars_vVideo = nil;
    [super dealloc];
}

+ (NSString *)tarsType
{
    return @"MTT.VideoChannel";
}

@end

@implementation MttVideoChannel (adapter)

+ (id)mtthdInstance
{
    MttVideoChannel *instance = [MttVideoChannel object];
    return instance;
}

@end

@implementation MttVideoResponse

@synthesize  JV2_PROP_NM(o, 0, sAppTitle);
@synthesize  JV2_PROP_NM(o, 1, sSearchUrl);
@synthesize  JV2_PROP_NM(o, 2, iVideoNum);
@synthesize  JV2_PROP_EX(o, 3, vData, VOMttVideoChannel);
@synthesize  JV2_PROP_NM(o, 4, sMd5);

- (id)init
{
	if (self = [super init]) {
	}
	return self;
}

- (void)dealloc
{
    self.tars_sAppTitle = nil;
    self.tars_sSearchUrl = nil;
    self.tars_vData = nil;
    self.tars_sMd5 = nil;
    [super dealloc];
}

+ (NSString *)tarsType
{
    return @"MTT.VideoApp";
}

@end

@implementation MttVideoResponse (adapter)

+ (id)mtthdInstance
{
    MttVideoResponse *instance = [MttVideoResponse object];
    return instance;
}

@end


