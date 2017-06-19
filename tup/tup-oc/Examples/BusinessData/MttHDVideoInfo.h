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
//  MttHDVideoInfo.h
//  MttHD
//
//  Created by bilsonzhou on 12-4-20.
//  Copyright (c) 2012年 tencent. All rights reserved.
//

#import "MttHDPersistentObject.h"
//视频导航数据结构
@interface VideoNavigationData : NSObject
{
    
    NSInteger _videoType;
    NSString  *  _typeTitle;
    NSString  *_typeFileName;     
}

@property (nonatomic,assign) NSInteger videoType;
@property (nonatomic,retain) NSString *typeTitle;
@property (nonatomic,retain) NSString *typeFileName;

@end
//电视剧剧集结构
@interface EpisodeNode : NSObject 
{
    NSUInteger _episodeNum;
    NSString *_episodeUrl;
}
@property (nonatomic,retain) NSString *episodeUrl;
@property (nonatomic,assign) NSUInteger episodeNum;
@end
//视频数据结构
@interface   VideoSection : NSObject
{
    NSInteger  _rowId;
    NSString  *_videoTitle;
     NSString  *_abstract;//摘要，主演等(副标题)
    NSString  *_episodeInfro;//电视集数或者更新状态，高清   
    NSString  *_imageUrl;
    NSString  *_url;//点击图片之后的链接
    NSMutableArray   *_episodeNodes;//电视的集数加链接
}
@property (nonatomic,assign) NSInteger rowId;
@property (nonatomic,retain) NSString   *imageUrl;
@property (nonatomic,retain) NSString  *episodeInfro;
@property (nonatomic,retain) NSString  *videoTitle;
@property (nonatomic,retain) NSString  *abstract;
@property (nonatomic,retain) NSString  *url;
@property (nonatomic,retain)NSMutableArray   *episodeNodes;           

@end


@interface MttHDVideoInfo : MttHDPersistentObject

@property (nonatomic, retain)  NSString* sSearchUrl;
@property (nonatomic, assign) int iVideoNum;
@property (nonatomic, retain) NSString* sMd5;

+ (MttHDVideoInfo *)theSpecial;

//存储所有频道的数据到文件
+ (void)saveNavigationChannelData:(NSMutableArray *)vChannel;
//存储频道中的视频信息到文件
+ (void)saveVideoData:(NSMutableArray *)vVideo toFile:(NSString *)fileName withUpdatePrefix:(BOOL)addPrefix;

+ (void)readNavigationChannelData:(NSMutableArray *)vChannel; 

//readRange = 0表示全部读取，返回值=YES表示已经读取到文件结尾
+ (BOOL)readVideoData:(NSMutableArray *)vVideo fromFile:(NSString *)fileName InRange:(NSRange)readRange;
@end
