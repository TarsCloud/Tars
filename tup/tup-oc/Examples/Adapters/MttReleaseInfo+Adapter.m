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
//  MttReleaseInfo+Adapter.m
//  MttHD
//
//  Created by 壬俊 易 on 12-3-21.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttReleaseInfo+Adapter.h"
#import "NSPathEx.h"

/**
 * 浏览器的QUA相关宏定义(构建平台自动替换)，iPad版本QUA定义格式如下:
 * "IPQB23_GA/230004&IMTT_3/230004&IP&644820&AppleiPad&50060&6312&V3"
 */
#define QUA3_PART1      "IPQB25_GA/25"          // 版本号，如“IPQB23_GA/23”
#define QUA3_BUILD      "VersionName"           // build号
#define QUA3_PART2      "&IMTT_3/25"            // 版本号，如“&IMTT_3/23“
#define QUA3_PART3      "IP&644820&AppleiPad&"  // 编译号
#define QUA3_POSTFIX    "&V3"

 
/**
 * 浏览器的LC与LCID定义
 */

/**********************************************************
 *iPad 2.4
 **********************************************************
#define USER_LICENSE    "D63204D5BED24B7"       // iPad 2.4
#define USER_LICENSE_ID "6312"                  // iPad 2.4
*/

/**********************************************************
 *iPad 2.5
 **********************************************************/
#define USER_LICENSE    "069F9AA7C78F4AE"       // iPad 2.5
#define USER_LICENSE_ID "6355"                  // iPad 2.5
 
 
/**
 * 渠道号
 */
#define CHANNEL         "50060"

@implementation MttReleaseInfo

+ (NSString *)qua 
{
    static NSString *qUA = nil;
    if (qUA == nil) {
        NSString *format = @"" QUA3_PART1 QUA3_BUILD QUA3_PART2 QUA3_BUILD QUA3_PART3 "%@&" USER_LICENSE_ID QUA3_POSTFIX;
        qUA = [[NSString alloc] initWithFormat:format, [self channel]];
    }
    return qUA;
}

+ (NSString *)lc
{
    return @"" USER_LICENSE;
}

+ (NSString *)lcId
{
    return @"" USER_LICENSE_ID;
}

+ (NSString *)channel
{
    static NSString *channelID = nil;
    if (channelID == nil) {
        NSDictionary *channelInfo = [[[NSDictionary alloc] initWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"MttHD-Channel" ofType:@"plist"]] autorelease];
        channelID = [[channelInfo objectForKey:@"ChannelId"] retain];
        if (channelID == nil)
            channelID = [(@"" CHANNEL) retain];
    }
    return channelID;
}

@end
