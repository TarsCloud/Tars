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
//  MttHDStatInfo.m
//  MttHD
//
//  Created by 易 壬俊 on 12-4-14.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttHDStatInfo.h"
#import "RegexKitLite.h"
#import "BrowserConfig.h"
#import "MttSystemInterface.h"

// TODO:renjunyi bad smell
#import "StartpageQuickLinkView.h"
#import "MttBookmarkManager.h"

@interface MttHDStatInfo (PrivateMethods)

- (void)addOuterPV:(NSString *)domain type:(MttHDStatPVType)type flow:(NSUInteger)flow;
- (void)addInnerPV:(NSString *)domain type:(MttHDStatPVType)type flow:(NSUInteger)flow;

- (NSInteger)quickLinkCount;
- (NSInteger)bookmarkCount;

@end

@implementation MttHDStatInfo

@dynamic statValue;
@dynamic useTime;
@dynamic totalPV;
@dynamic outerPV;
@dynamic innerPV;
@dynamic entryPV;
@synthesize behaviors = _behaviors;
@synthesize maximumTabs = _maximumTabs;
@synthesize apn = _apn;

static MttHDStatInfo *_special = nil;

+ (MttHDStatInfo *)theSpecial
{
    if (_special == nil)
        _special = [[super theSpecial] retain];
    return [[_special retain] autorelease];
}

+ (MttHDStatInfo *)clearTheSpecial
{
    MttHDStatInfo *_old = _special;
    _special = nil;
    [_old deleteObject];
    return [_old autorelease];
}

+ (NSArray *)transients
{
	return [NSArray arrayWithObjects:@"statValue", @"useTime", nil];
}

- (id)init
{
	if (self = [super init]) {
        _totalPV = [[NSMutableDictionary alloc] init];
        _outerPV = [[NSMutableDictionary alloc] init];
        _innerPV = [[NSMutableDictionary alloc] init];
        _entryPV = [[NSMutableDictionary alloc] init];
        _behaviors = [[NSMutableDictionary alloc] init];
        _apn = MttHDStatApn_NONE;
	}
	return self;
}

- (void)dealloc
{
    [_totalPV release];
    [_outerPV release];
    [_innerPV release];
    [_entryPV release];
    [_behaviors release];
    [_apn release];
    [super dealloc];
}

- (NSInteger)statValue
{
    return [MttHDLoginInfo theSpecial].statValue;
}

- (NSInteger)useTime
{
    return 0; // TODO:renjunyi 
}

- (NSString *)protocol
{
    if ((self.statValue & MttStatValue_SACTION) == 0)
        return nil;
    
    NSMutableString *protocol = [NSMutableString string];
    [protocol appendFormat:@"belogin=%d", (getConfigQQ() == NULL ? 0 : 1)];
    [protocol appendFormat:@"&fmarknum=%d", [self quickLinkCount]];
    [protocol appendFormat:@"&bkmarknum=%d", [self bookmarkCount]];
    [protocol appendFormat:@"&hw_version=%d&ios_version=%d", getSystemInfo()->hw_version, getSystemInfo()->ios_version];
    [protocol appendFormat:@"&homesetting=%d", getConfigStartPageSwitchSide()];
    [protocol appendFormat:@"&openpage=%d", (getConfigOpenInCurrentWindow() == YES ? 0 : 1)];
    [protocol appendFormat:@"&backopen=%d", (getConfigOpenInBackground() == NO ? 0 : 1)];
    [protocol appendFormat:@"&searchengineset=%d", getConfigSearchEngine()];
    [protocol appendFormat:@"&panelposition=%d", (getGlobalIsAdvancedRight() == YES ? 0 : 1)];
    [protocol appendFormat:@"&lock_the_browser=%d", (getConfigAccessLimit() == NO ? 0 : 1)];
    [protocol appendFormat:@"&openclosure=%d", (getConfigLastClose() == YES ? 0 : 1)];
    [protocol appendFormat:@"&openvisit=%d", (getConfigMostVisited() == YES ? 0 : 1)];
    [protocol appendFormat:@"&lockstate=%d", (getGlobalIsScreenOrientationLocked() == NO ? 0 : 1)];
    for (NSString *key in [self.behaviors allKeys])
        [protocol appendFormat:@"&%@=%@", key, [self.behaviors objectForKey:key]];
    return protocol;
}

- (NSArray *)totalPV
{
    @synchronized(self) {
        return [_totalPV allValues];
    }
}

- (void)setTotalPV:(NSArray *)totalPV
{
    @synchronized(self) {
        [_totalPV removeAllObjects];
        for (MttSTTotal *stTotal in totalPV) {
            [_totalPV setValue:stTotal forKey:stTotal.tars_sAPN];
        }
    }
}

- (NSArray *)outerPV
{
    @synchronized(self) {
        return [_outerPV allValues];
    }
}

- (void)setOuterPV:(NSArray *)outerPV
{
    @synchronized(self) {
        [_outerPV removeAllObjects];
        for (MttSTPV *stPV in outerPV) {
            [_entryPV setValue:stPV forKey:stPV.tars_sDomain];
        }
    }
}

- (NSArray *)innerPV
{
    @synchronized(self) {
        return [_innerPV allValues];
    }
}

- (void)setInnerPV:(NSArray *)innerPV
{
    @synchronized(self) {
        [_innerPV removeAllObjects];
        for (MttSTPV *stPV in innerPV) {
            [_innerPV setValue:stPV forKey:stPV.tars_sDomain];
        }
    }
}

- (NSArray *)entryPV
{
    @synchronized(self) {
        return [_entryPV allValues];
    }
}

- (void)setEntryPV:(NSArray *)entryPV
{
    @synchronized(self) {
        [_entryPV removeAllObjects];
        for (MttETPV *etPV in entryPV) {
            [_entryPV setValue:etPV forKey:etPV.tars_sURL];
        }
    }
}

- (NSInteger)maximumTabs
{
    @synchronized(self) {
        return _maximumTabs;
    }
}

- (void)setMaximumTabs:(NSInteger)maximumTabs
{
    @synchronized(self) {
        if (maximumTabs > _maximumTabs) {
            _maximumTabs = maximumTabs;
        }
    }
}

- (void)addPV:(NSString *)domain type:(MttHDStatPVType)type apn:(MttHDStatApn)apn flow:(NSUInteger)flow
{
    @synchronized(self) {
        
        // 如果域名为空则不进行统计
        if (domain == nil || domain.length == 0)
            return;
        
        // 将domain转换为小写
        domain = [domain lowercaseString];
        
        if (![apn isEqual:MttHDStatApn_NET] || ![apn isEqual:MttHDStatApn_WAP] || ![apn isEqual:MttHDStatApn_WIFI])
            return;
        
        MttSTTotal *stTotal = [_totalPV objectForKey:apn];
        if (stTotal == nil) {
            stTotal = [[[MttSTTotal alloc] init] autorelease];
            [_totalPV setObject:stTotal forKey:apn];
        }
        
        switch (type) {
            case MttHDStatPVType_WAP:
                stTotal.tars_iWapPV += 1;
                stTotal.tars_iWapFlow += flow;
                break;
            case MttHDStatPVType_WEB:
                stTotal.tars_iWebPV += 1;
                stTotal.tars_iWebFlow += flow;
                break;
            case MttHDStatPVType_RES:
                stTotal.tars_iResPv += 1;
                stTotal.tars_iResFlow += flow;
                break;
            case MttHDStatPVType_ERR:
                stTotal.tars_iErrorPV += 1;
                break;
            default:
                assert(0);
                return;
        }
        
        // 不统计使用ip地址的PV
        if (![domain isMatchedByRegex:@"^[\\d\\.]+$"]) 
        {    
            // 如果domain的二级域名为qq.com则需要进行特殊处理 比如xx.13.qq.com => xx.qq.com 又如xx.15.1.qq.com => xx.qq.com
            // 如果是其它域名，需要提取出二级域名进行统计 比如xx.ww.sina.com => sina.com 又如xx.ww.aa.edu.cn => aa.edu.cn
            if ([domain isMatchedByRegex:@"^[a-zA-Z0-9\\-\\.]+\\.qq\\.com$"])
                domain = [domain stringByReplacingOccurrencesOfRegex:@"^([a-zA-Z0-9\\-\\.]+?)\\.[\\d+\\.]*qq\\.com$" withString:@"$1.qq.com"];
            else
                domain = [domain stringByMatching:@"[\\w-]+\\.(?:com|edu|gov|int|net|org|biz|info|mobi|xxx)(?:\\.\\w+)?$"];
            [self addOuterPV:domain type:type flow:flow];
        }
    }
}

- (void)addEntryPV:(NSString *)url type:(MttEntryType)type
{
    @synchronized(self) {
        
        if ((self.statValue & MttStatValue_ENTRY) == 0)
            return;
        
        MttETPV *entryPV = [_entryPV objectForKey:url];
        if (entryPV == nil) {
            entryPV = [[[MttETPV alloc] init] autorelease];
            entryPV.tars_sURL = url;
            entryPV.tars_stURLPV = nil;
            [_entryPV setValue:entryPV forKey:url];
        }
        
        MttURLPV *urlPV = nil;
        if (entryPV.tars_stURLPV == nil) {
            entryPV.tars_stURLPV = [NSMutableArray array];
        }
        if ([entryPV.tars_stURLPV isKindOfClass:[NSArray class]] == YES &&
            [entryPV.tars_stURLPV isKindOfClass:[NSMutableArray class]] == NO) {
            entryPV.tars_stURLPV = [NSMutableArray arrayWithArray:entryPV.tars_stURLPV];
        }
        NSMutableArray *urlPVs = (NSMutableArray *)entryPV.tars_stURLPV;
        for (urlPV in urlPVs) {
            if (urlPV.tars_eEntryType == type)
                break;
        }
        if (urlPV == nil) {
            urlPV = [[[MttURLPV alloc] init] autorelease];
            urlPV.tars_iEntry = 2;
            urlPV.tars_eEntryType = type;
            [urlPVs addObject:urlPV];
        }
        urlPV.tars_iPV += 1;
    }
}

- (void)statUserBehavior:(MttHDUserBehavior)behavior
{
    NSNumber* number = [self.behaviors objectForKey:behavior];
    [self.behaviors setValue:[NSNumber numberWithInt:(number.intValue + 1)] forKey:behavior];
}

@end

@implementation MttHDStatInfo (PrivateMethods)

// TODO:renjunyi bad smell
- (NSInteger)quickLinkCount
{
    StartpageQuickLinkView *qlink = [StartpageQuickLinkView sharedInstance];
    if (qlink != nil)
        return [qlink.quicklinkItems count] - 1;
    return 0;
}

// TODO:renjunyi bad smell
- (NSInteger)bookmarkCount
{
    return [MttBookmarkManager getApproximatelyBookmarkCountForStatisticPurpose];
}

- (void)addOuterPV:(NSString *)domain type:(MttHDStatPVType)type flow:(NSUInteger)flow
{
    if ((self.statValue & MttStatValue_DOMAINPV) == 0) 
        return;
    
    if (domain == nil || domain.length == 0)
        return;
    
    MttSTPV *stPV = [_outerPV objectForKey:domain];
    if (stPV == nil) {
        stPV = [[[MttSTPV alloc] init] autorelease];
        [_outerPV setObject:stPV forKey:domain];
    }
    
    stPV.tars_sDomain = domain;
    stPV.tars_iFlow += flow;
    switch (type) {
        case MttHDStatPVType_WAP:
            stPV.tars_iWapPV += 1;
            break;
        case MttHDStatPVType_WEB:
            stPV.tars_iWebPV += 1;
            break;
        case MttHDStatPVType_RES:
            stPV.tars_iResPv += 1;
            break;
        case MttHDStatPVType_ERR:
            break;
        default:
            assert(0);
            return;
    }
}

- (void)addInnerPV:(NSString *)domain type:(MttHDStatPVType)type flow:(NSUInteger)flow
{
    if ((self.statValue & MttStatValue_STATURL) == 0) 
        return;
    
    if (domain == nil || domain.length == 0)
        return;
    
    MttSTPV *stPV = [_innerPV objectForKey:domain];
    if (stPV == nil) {
        stPV = [[[MttSTPV alloc] init] autorelease];
        [_innerPV setObject:stPV forKey:domain];
    }
    
    stPV.tars_sDomain = domain;
    stPV.tars_iFlow += flow;
    switch (type) {
        case MttHDStatPVType_WAP:
            stPV.tars_iWapPV += 1;
            break;
        case MttHDStatPVType_WEB:
            stPV.tars_iWebPV += 1;
            break;
        case MttHDStatPVType_RES:
            stPV.tars_iResPv += 1;
            break;
        case MttHDStatPVType_ERR:
            break;
        default:
            assert(0);
            return;
    }
}

@end
