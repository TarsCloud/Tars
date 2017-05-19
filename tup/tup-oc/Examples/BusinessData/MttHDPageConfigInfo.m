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
//  MttHDPageConfigInfo.m
//  MttHD
//
//  Created by renjunyi on 12-4-14.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttHDPageConfigInfo.h"

@implementation MttHDPageConfigInfo

@synthesize md5StringOfStartPage = _md5StringOfStartPage;
@synthesize searchLastUpdateTime = _searchLastUpdateTime;
@synthesize keywordLastUpdateTime = _keywordLastUpdateTime;

@synthesize domainLastUpdateTime = _domainLastUpdateTime;
@synthesize innerUrls = _innerUrls;
@synthesize safeUrls = _safeUrls;

@synthesize pluginLastUpdateTime = _pluginLastUpdateTime;
@synthesize configLastUpdateTime = _configLastUpdateTime;
@synthesize configInfoDictionary = _configInfoDictionary;

+ (MttHDPageConfigInfo *)theSpecial
{
    static MttHDPageConfigInfo *special = nil;
    if (special == nil) {
        special = [[super theSpecial] retain];
    }
    return special;
}

- (id)init
{
	if (self = [super init]) {
        _md5StringOfStartPage = @"";
        _innerUrls = nil;
        _safeUrls = [[NSArray alloc] initWithObjects:
                     @"qq.com", 
                     @"soso.com",
                     @"tencent.com",
                     @"paipai.com",
                     @"117.135.128.48",
                     @"baidu.mobi",
                     @"baidu.com",
                     @"taobao.com",
                     @"kanshula.com.cn",
                     @"kanshu.la",
                     @"read.xxsy.net",
                     @"t.sina.cn",
                     @"sina.cn",
                     @"mop.com",
                     @"xs8.cn",
                     @"t.qunar.com",
                     @"qidian.com",
                     @"easou.com",
                     @"hongxiu.com",
                     @"ifeng.com",
                     @"google.com.hk",
                     @"google.com",
                     @"google.cn",
                     @"tianya.cn",
                     @"bbs.dospy.com",
                     @"58.com",
                     @"17k.com",
                     @"sogou.com",
                     @"weibo.cn",
                     @"renren.com",
                     @"sohu.com",
                     @"163.com", nil];
        _configInfoDictionary = nil;
	}
	return self;
}

- (void)dealloc
{
    [_md5StringOfStartPage release];
    [_innerUrls release];
    [_safeUrls release];
    [_configInfoDictionary release];
    [super dealloc];
}

- (void)setConfigLastUpdateTime:(NSInteger)configLastUpdateTime
{
    if (configLastUpdateTime > 0)
        _configLastUpdateTime = configLastUpdateTime;
}

- (void)setDomainLastUpdateTime:(NSInteger)domainLastUpdateTime
{
    if (domainLastUpdateTime > 0)
        _domainLastUpdateTime = domainLastUpdateTime;
}

- (void)setKeywordLastUpdateTime:(NSInteger)keywordLastUpdateTime
{
    if (keywordLastUpdateTime > 0)
        _keywordLastUpdateTime = keywordLastUpdateTime;
}

- (void)setPluginLastUpdateTime:(NSInteger)pluginLastUpdateTime
{
    if (pluginLastUpdateTime > 0)
        _pluginLastUpdateTime = pluginLastUpdateTime;
}

- (void)setSearchLastUpdateTime:(NSInteger)searchLastUpdateTime
{
    if (searchLastUpdateTime > 0)
        _searchLastUpdateTime = searchLastUpdateTime;
}

- (NSString *)startPageContentFilePath
{
    NSString *filePath = [[[self class] tupDataDirectory] stringByAppendingPathComponent:@"WebNaviDatas.plist"];
    return filePath;
}

- (void)setStartPageContent:(NSString *)startPageContent
{
    NSString *filePath = [[[self class] tupDataDirectory] stringByAppendingPathComponent:@"WebNaviDatas.plist"];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    [fileManager createFileAtPath:filePath
                         contents:[startPageContent dataUsingEncoding:NSUTF8StringEncoding] 
                       attributes:nil];
}

- (NSInteger)userNeedRateAfterDays
{
    NSString* num = [self.configInfoDictionary objectForKey:@"UserNeedRateAfterDays"];
    if (num.intValue > 0)
        return num.intValue;
    return 3;
}

- (NSInteger)userNeedRateAfterLeaveTimes
{
    NSString* num = [self.configInfoDictionary objectForKey:@"UserNeedRateAfterLeaveTimes"];
    if (num.intValue > 0)
        return num.intValue;
    return 5;
}

@end
