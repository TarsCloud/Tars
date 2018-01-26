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
//  MttSecurityService.h
//  MttHD
//
//  Created by 壬俊 易 on 12-4-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WirelessUnifiedProtocol/TupService.h>
#import "MttBrokerSecurityRequest.h"
#import "MttBrokerSecurityResponse.h"

@interface MttSecurityService : TupService

- (id)security:(NSString *)url checkType:(int)type WithCompleteBlock:(void (^)(MttBrokerSecurityResponse *))completeBlock failedBlock:(TupServiceBasicBlock)failedBlock;

@end
