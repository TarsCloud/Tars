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
//  MttHDVideoInfo.m
//  MttHD
//
//  Created by bilsonzhou on 12-4-20.
//  Copyright (c) 2012年 tencent. All rights reserved.
//

#import "MttHDVideoInfo.h"
#import "UIMttKit.h"

@implementation  VideoNavigationData

@synthesize videoType = _videoType;
@synthesize typeTitle = _typeTitle;
@synthesize typeFileName = _typeFileName;

- (void)dealloc
{
    [_typeTitle release];
    [_typeFileName release];
    [super dealloc];
}

@end 

@implementation EpisodeNode

@synthesize episodeUrl = _episodeUrl;
@synthesize episodeNum = _episodeNum;

- (void)dealloc
{
    [_episodeUrl release];
    [super dealloc];
}

@end

@implementation  VideoSection

@synthesize rowId = _rowId;
@synthesize imageUrl = _imageUrl;
@synthesize episodeInfro = _episodeInfro;
@synthesize videoTitle = _videoTitle;
@synthesize abstract = _abstract;
@synthesize url = _url;
@synthesize episodeNodes = _episodeNodes;  

- (void)dealloc
{
    [_imageUrl release];
    [_episodeInfro release];
    [_url release];
    [_abstract release];
    [_videoTitle release];
    [_episodeNodes release];
    [super dealloc];
}

@end 

@implementation MttHDVideoInfo

@synthesize sSearchUrl = _sSearchUrl;
@synthesize iVideoNum = _iVideoNum;
@synthesize sMd5 = _sMd5;

+ (MttHDVideoInfo *)theSpecial
{
    static MttHDVideoInfo *special = nil;
    if (special == nil) {
        special = [[super theSpecial] retain];
    }
    return special;
}

- (id)init
{
	if (self = [super init]) {
	}
	return self;
}
#define ChannelItemCount 2
+ (void)saveNavigationChannelData:(NSMutableArray *)vChannel
{
    NSMutableArray *vChannelList = [[NSMutableArray alloc] initWithCapacity:vChannel.count];
    for (int i = 0; i < vChannel.count; ++i)
    {
        NSMutableArray *OneChannel = [[NSMutableArray alloc] initWithCapacity:ChannelItemCount];
        VideoNavigationData *channel = [vChannel objectAtIndex:i];
        [OneChannel addObject:channel.typeTitle];
        //[OneChannel addObject:channel.videoType];
        [OneChannel addObject:channel.typeFileName];
        
        [vChannelList addObject:OneChannel];
        [OneChannel release];
    }
    NSString *NaviNewFileName = [@"_" stringByAppendingString:@"VideoNavigation.plist"];
    NSString *NaviNewFilePath = [MTTHD_DOC_RES stringByAppendingPathComponent:NaviNewFileName];
    if ([[NSFileManager defaultManager] fileExistsAtPath:NaviNewFilePath])
    {
        [[NSFileManager defaultManager] removeItemAtPath:NaviNewFilePath error:nil];
    }
    [vChannelList writeToFile:NaviNewFilePath atomically:YES];
    [vChannelList release];
}

+ (void)readNavigationChannelData:(NSMutableArray *)vChannel
{
    NSArray *fileDataList = nil;
    NSString *navigationFilePath = [MTTHD_DOC_RES stringByAppendingPathComponent:@"VideoNavigation.plist"];
    //查看是否有更新的数据
    NSString *NaviNewFileName = [@"_" stringByAppendingString:@"VideoNavigation.plist"];
    NSString *NaviNewFilePath = [MTTHD_DOC_RES stringByAppendingPathComponent:NaviNewFileName];
    if ([[NSFileManager defaultManager] fileExistsAtPath:NaviNewFilePath])
    {
        [[NSFileManager defaultManager] removeItemAtPath:navigationFilePath error:nil];
        [[NSFileManager defaultManager] moveItemAtPath:NaviNewFilePath toPath:navigationFilePath error:nil];
    }
    
    
    if ([[NSFileManager defaultManager] fileExistsAtPath:navigationFilePath])
    {
        fileDataList = [[NSArray alloc] initWithContentsOfFile:navigationFilePath];
        
        if ( fileDataList )
        {
            if ( [fileDataList count] <= 0 )
            {
                [fileDataList release];
                fileDataList = nil;
            }
        }
    }
    
    if ( nil == fileDataList )
    {
        NSString *bundlePath = [NSString stringWithFormat:@"/res/data/VideoNavigation.plist"];
        NSString *bundleFilePath = MttLocalizedFileForPath(bundlePath,nil);
        fileDataList = [[NSArray alloc] initWithContentsOfFile:bundleFilePath];
        if (fileDataList == nil) {
            return;
        }
    }     
    
    for (int i = 0; i < [fileDataList count]; ++i) {
        NSArray *anItem = [fileDataList objectAtIndex:i];
        if ([anItem count] < ChannelItemCount) {
            [fileDataList release];
            return;
        }
        VideoNavigationData *aNode = [[VideoNavigationData alloc] init];
        aNode.videoType = [vChannel count];
        aNode.typeTitle = [anItem objectAtIndex:0];
        aNode.typeFileName = [anItem objectAtIndex:1];
        
        [vChannel addObject:aNode];
        [aNode release];
    }
    
    [fileDataList release];
}
#define VideoSectionItem 6
+ (void)saveVideoData:(NSMutableArray *)vVideo toFile:(NSString *)fileName withUpdatePrefix:(BOOL)addPrefix
{
    NSMutableArray *vVideoDataList = [[NSMutableArray alloc] initWithCapacity:vVideo.count];
    for (int i = 0; i < vVideo.count; ++i)
    {
        NSMutableArray *OneVideo = [[NSMutableArray alloc] initWithCapacity:VideoSectionItem];
        VideoSection *section = [vVideo objectAtIndex:i];
        [OneVideo addObject:section.videoTitle];
        [OneVideo addObject:section.abstract];
        [OneVideo addObject:section.episodeInfro];
        [OneVideo addObject:section.imageUrl];
        [OneVideo addObject:section.url];
        NSMutableArray *episodeUrlList = [[NSMutableArray alloc] initWithCapacity:1];
        for (int j = 0; j < section.episodeNodes.count; ++j)
        {
            EpisodeNode *node = [section.episodeNodes objectAtIndex:j];
            [episodeUrlList addObject:node.episodeUrl];
        }
        [OneVideo addObject:episodeUrlList];
        [episodeUrlList release];
        
        [vVideoDataList addObject:OneVideo];
        [OneVideo release];
    }
    if (addPrefix)
    {
        fileName = [@"_" stringByAppendingString:fileName];
    }
    NSString *filePath = [MTTHD_DOC_RES stringByAppendingPathComponent:fileName];
    if ([[NSFileManager defaultManager] fileExistsAtPath:filePath])
    {
        [[NSFileManager defaultManager] removeItemAtPath:filePath error:nil];
    }
    [vVideoDataList writeToFile:filePath atomically:YES];
    [vVideoDataList release];
}
+ (BOOL)readVideoData:(NSMutableArray *)vVideo fromFile:(NSString *)fileName InRange:(NSRange)readRange
{
    NSArray *fileDataList = nil;
    NSString *typeFileName = [MTTHD_DOC_RES stringByAppendingPathComponent:fileName];
    
    if (readRange.location == 0) //只有从头开始读取也就是切换类型时才查看是否有更新
    {
        NSString *typeNewFileName = [@"_" stringByAppendingString:fileName];
        NSString *typeNewFilePath = [MTTHD_DOC_RES stringByAppendingPathComponent:typeNewFileName];
        if ([[NSFileManager defaultManager] fileExistsAtPath:typeNewFilePath])
        {
            [[NSFileManager defaultManager] removeItemAtPath:typeFileName error:nil];
            [[NSFileManager defaultManager] moveItemAtPath:typeNewFilePath toPath:typeFileName error:nil];
        }
    }
       
    if ([[NSFileManager defaultManager] fileExistsAtPath:typeFileName])
    {
        fileDataList = [[NSArray alloc] initWithContentsOfFile:typeFileName];
        
        if ( fileDataList )
        {
            if ( [fileDataList count] <= 0 )
            {
                [fileDataList release];
                fileDataList = nil;
            }
        }
    }
    
    NSString *bundlePath = [NSString stringWithFormat:@"/res/data/%@", fileName];
    NSString *bundleFilePath = MttLocalizedFileForPath(bundlePath,nil);
    
    if ( nil == fileDataList )
    {
        fileDataList = [[NSArray alloc] initWithContentsOfFile:bundleFilePath];
        if (fileDataList == nil) {
            return YES;
        }
    }  
    if (readRange.length == 0) 
    {
        readRange.location = 0;
        readRange.length = fileDataList.count;
    }
    for (int i = readRange.location; i < readRange.length; ++i) 
    {
        if (i >= fileDataList.count) 
        {
            [fileDataList release];
            return YES;
        }
        NSArray *array = [fileDataList objectAtIndex:i];
        if ([array count] < VideoSectionItem) 
        {
            [fileDataList release];
            return YES;
        }
        
        VideoSection *aNode = [[VideoSection alloc] init];
        aNode.episodeNodes = nil;
        aNode.rowId = i;
        aNode.videoTitle = [array objectAtIndex:0];
        aNode.abstract = [array objectAtIndex:1];
        aNode.episodeInfro = [array objectAtIndex:2];
        aNode.imageUrl = [array objectAtIndex:3];
        aNode.url = [array objectAtIndex:4];
        
        NSArray *episodeArray = [array objectAtIndex:5] ;
        NSMutableArray *tmpEpisodeNodes = [[NSMutableArray alloc] init];
        aNode.episodeNodes = tmpEpisodeNodes;
        [tmpEpisodeNodes release];
        for (int j = 0; j < [episodeArray count]; ++j) 
        {
            EpisodeNode *aEpsideNode = [[EpisodeNode alloc] init];
            aEpsideNode.episodeNum = j;
            aEpsideNode.episodeUrl = [episodeArray objectAtIndex:j] ;
            [aNode.episodeNodes addObject:aEpsideNode];
            [aEpsideNode release];
        }
        [vVideo addObject:aNode];
        [aNode release];
    }

    if (fileDataList.count == readRange.length) 
    {
       [fileDataList release];
        return YES;
    }
    [fileDataList release];
    return NO;
}
- (void)dealloc
{
    self.sSearchUrl = nil;
    self.sMd5 = nil;
    [super dealloc];
}

@end
