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
//  TarsOutputStream.h
//
//  Created by 壬俊 易 on 12-1-13.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "TarsStream.h"

@interface TarsOutputStream : TarsStream

+ (TarsOutputStream *)stream;
+ (TarsOutputStream *)streamWithCapability:(int)capability;

- (void)writeTag:(int)tag type:(int)type;
- (void)writeInt1:(char)val;
- (void)writeInt2:(short)val;
- (void)writeInt4:(int)val;
- (void)writeInt8:(long long)val;
- (void)writeInt:(long long)val tag:(int)tag;
- (void)writeFloat:(float)val tag:(int)tag;
- (void)writeDouble:(double)val tag:(int)tag;

- (void)writeDictionary:(NSDictionary *)dictionary tag:(int)tag required:(BOOL)required;
- (void)writeArray:(NSArray *)array tag:(int)tag required:(BOOL)required;
- (void)writeNumber:(NSNumber *)number tag:(int)tag required:(BOOL)required;
- (void)writeObject:(TarsObject *)object tag:(int)tag required:(BOOL)required;
- (void)writeString:(NSString *)string tag:(int)tag required:(BOOL)required;
- (void)writeData:(NSData *)data tag:(int)tag required:(BOOL)required;
- (void)writeAnything:(id)anything tag:(int)tag required:(BOOL)required;

@end
