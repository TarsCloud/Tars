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
//  TupAgentDelegate.h
//  tup
//
//  Created by 壬俊 易 on 12-4-11.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol TupAgentDelegate <NSObject>

// 获取服务器URL
- (NSURL*)serverUrl;

// 当使用指定的URL进行的TUP请求失败时，可以通过此接口废弃该URL，下次使用serverUrl接口将获得一
// 个服务器url列表中的新url，当没有新的url时，返回nil
- (void)invalidateServerUrl:(NSURL*)url;

- (NSString*)userAgent;
- (NSString*)qAuth;
- (NSString*)qGuid;
- (NSString*)qUA;

@end
