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
//  MttCoolReadService.m
//  MttHD
//
//  Created by bilsonzhou on 12-4-20.
//  Copyright (c) 2012年 tencent. All rights reserved.
//


#import "MttCoolReadService.h"
#import "MttHDTupManager.h"
#import "MttHDTupEnvironment.h"
#import "MttCoolReadAgent.h"
#import "MttHDVideoInfo.h"
#import "SQLiteInstanceManager.h"

@implementation MttVideoService

- (id)init
{
	if (self = [super initWithAgent:[MttVideoAgent agent]]) {
	}
	return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (id)getVideoWithCompleteBlock:(TupServiceBasicBlock)completeBlock failedBlock:(TupServiceBasicBlock)failedBlock
{
    MttVideoAgent *agent = (MttVideoAgent *)[self agent];
    id stub = [agent getVideo:[MttVideoRequest mtthdInstance] withCompleteHandle:^(MttVideoResponse * rsp) {
        if (rsp != nil) {
            @try {
               @synchronized([SQLiteInstanceManager sharedManager])
                {
                    MttHDVideoInfo* videoInfo = [MttHDVideoInfo theSpecial];
                    //videoInfo.sAppTitle = rsp.tars_sAppTitle;
                    videoInfo.sSearchUrl = rsp.tars_sSearchUrl;
                    videoInfo.sMd5 = rsp.tars_sMd5;
                    videoInfo.iVideoNum = rsp.tars_iVideoNum;
                    [videoInfo save];
                }

                //保存频道数据信息
                NSArray * vChannel = rsp.tars_vData;
                NSMutableArray *vNavigationList = [[NSMutableArray alloc] init];
                for (int i = 0; i < vChannel.count; ++i)
                {
                    VideoNavigationData *channel = [[VideoNavigationData alloc] init];
                    MttVideoChannel *videoChannel = [vChannel objectAtIndex:i];
                    channel.typeTitle = videoChannel.tars_stChannel.tars_sName;
                    channel.typeFileName = [videoChannel.tars_stChannel.tars_sCid stringByAppendingString:@".plist"];
                    [vNavigationList addObject:channel];
                    [channel release];
                
                    //保存对应频道的视频信息
                    NSMutableArray *videoArray = [[NSMutableArray alloc] init];
                    NSArray *vVideoArray = videoChannel.tars_vVideo;
                    for (int j = 0; j < vVideoArray.count; ++j)
                    {
                        MttIpadViewData *ipadVideoData = [vVideoArray objectAtIndex:j];
                         VideoSection *section = [[VideoSection alloc] init];
                        section.rowId = j;
                        section.imageUrl = ipadVideoData.tars_sWebCoverUrl;
                        section.abstract = ipadVideoData.tars_sWebSubTitle;
                        section.episodeInfro = ipadVideoData.tars_sWebEpsInfo;
                        section.videoTitle = ipadVideoData.tars_sWebLinkTitle;
                        section.url = ipadVideoData.tars_sWebLink;
                        section.episodeNodes = nil;
                        NSMutableArray *tmpEpisodeNodes = [[NSMutableArray alloc] init];
                        section.episodeNodes = tmpEpisodeNodes;
                        [tmpEpisodeNodes release];
                        for (int k = 0; k < ipadVideoData.tars_vEpisodeList.count; ++k)
                        {
                            MttEpisodeNode *mttNode = [ipadVideoData.tars_vEpisodeList objectAtIndex:k];
                            EpisodeNode *node = [[EpisodeNode alloc] init];
                            node.episodeNum = k;
                            node.episodeUrl = mttNode.tars_sEpisodeUrl;
                            [section.episodeNodes addObject:node];
                            [node release];
                        }
                        [videoArray addObject:section];
                        [section release];
                    }
                    [MttHDVideoInfo saveVideoData:videoArray toFile:[videoChannel.tars_stChannel.tars_sCid stringByAppendingString:@".plist"] withUpdatePrefix:YES];
                    [videoArray release];
                }
                [MttHDVideoInfo saveNavigationChannelData:vNavigationList];
                [vNavigationList release];

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

