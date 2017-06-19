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
//  MttVideoReq.h
//  MttHD
//
//  Created by bilsonzhou on 12-4-20.
//  Copyright (c) 2012年 tencent. All rights reserved.
//

#import <WirelessUnifiedProtocol/WirelessUnifiedProtocol.h>
#import <WirelessUnifiedProtocol/MttUserBase.h>

@interface MttVideoRequest : TarsObjectV2

@property (nonatomic, retain, JV2_PROP_GS(stUB)) MttUserBase * JV2_PROP_NM(o, 0, stUB);
@property (nonatomic, retain, JV2_PROP_GS(sMd5)) NSString* JV2_PROP_NM(o, 1, sMd5);

@end

@interface MttVideoRequest (adapter)

+ (id)mtthdInstance;

@end