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
//  RequestF.h
//
//  Created by renjunyi on 11-12-1.
//  Copyright (c) 2011年 Tencent. All rights reserved.
//

#import "TarsObject.h"

#pragma mark -
#pragma mark STRUCT

#pragma mark -

@interface RequestPacket : TarsObject

@property (nonatomic, retain) NSNumber     *iVersion;           // short
@property (nonatomic, retain) NSNumber     *cPacketType;        // char
@property (nonatomic, retain) NSNumber     *iMessageType;       // int
@property (nonatomic, retain) NSNumber     *iRequestId;         // int
@property (nonatomic, retain) NSString     *sServantName;
@property (nonatomic, retain) NSString     *sFuncName;
@property (nonatomic, retain) NSData       *sBuffer;
@property (nonatomic, retain) NSNumber     *iTimeout;           // int
@property (nonatomic, retain) NSDictionary *context;
@property (nonatomic, retain) NSDictionary *status;

@end
