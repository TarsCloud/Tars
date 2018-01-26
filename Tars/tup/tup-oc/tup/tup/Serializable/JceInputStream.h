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
//  TarsInputStream.h
//
//  Created by 壬俊 易 on 12-1-13.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "TarsStream.h"

@interface TarsInputStream : TarsStream
{
	int	_headType;
	int	_headTag;
}

@property (nonatomic, assign) int headType;
@property (nonatomic, readonly) int	headTag;

+ (TarsInputStream *)streamWithBuffer:(void *)buffer length:(int)length;
+ (TarsInputStream *)streamWithData:(NSData *)data;

- (BOOL)readHead;
- (BOOL)peakHead;

- (int)readInt1;
- (int)readInt2;
- (int)readInt4;
- (long long)readInt8;
- (float)readFloat;
- (double)readDouble;

- (long long)readInt:(int)tag;
- (float)readFloat:(int)tag;
- (double)readDouble:(int)tag;

- (NSNumber *)readNumber:(int)tag required:(BOOL)required;
- (NSString *)readString:(int)tag required:(BOOL)required;
- (NSData *)readData:(int)tag required:(BOOL)required;
- (id)readObject:(int)tag required:(BOOL)required description:(Class)theClass;
- (NSArray *)readArray:(int)tag required:(BOOL)required description:(id)description;
- (NSDictionary *)readDictionary:(int)tag required:(BOOL)required description:(TarsPair *)description;
- (id)readAnything:(int)tag required:(BOOL)required description:(id)description;

@end
