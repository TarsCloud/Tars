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
//  MttHDTupManager.h
//  MttHD
//
//  Created by 壬俊 易 on 12-3-20.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MttHDTupEnvironment.h"
#import "MttHDVerifyInfo.h"
#import "MttHDLoginInfo.h"
#import "MttHDPageConfigInfo.h"
#import "MttHDSplashInfo.h"
#import "MttHDStatInfo.h"

/**
 * 起始页更新成功后将发送此通知到Tup通知中心
 */
#define MttHDTupStartPageUpdated @"MttHDTupStartPageUpdated"

/**
 * 收到update响应将发送此通知到Tup通知中心，其userInfo包含下面几个键值：
 * "status" - NSNumber  0-不更新 1-提示更新 2-强制更新 3-检查更新
 * "title"  - NSString
 * "prompt" - NSString
 * "url"    - NSString
 */
#define MttHDTupReceiveUpdateResponse @"MttHDTupReceiveUpdateResponse"

@interface MttHDTupManager : NSObject

+ (MttHDTupManager *)sharedMttHDTupManager;
+ (NSNotificationCenter *)defaultNotificationCenter;

- (void)startStandardProcedure;

- (void)startVerify;
- (void)startLogin;
- (void)startConfig;
- (void)startStat;
- (void)startVideo;

@end
