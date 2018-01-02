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
//  TupHttpRequest.h
//  tup
//
//  Created by 壬俊 易 on 12-4-11.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ASIHTTPRequest.h"
#import "TupAgentDelegate.h"

@interface TupHttpRequest : ASIHTTPRequest

// 当TUP请求失败时，是否更换其它TUP服务器进行重试，默认为NO
@property (assign) BOOL retryUsingOtherServers;

// 当retryUsingOtherServers为YES时，必须指定agentDelegate
@property (retain) NSObject<TupAgentDelegate> *agentDelegate;

@end
