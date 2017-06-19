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
//  TarsStream.m
//
//  Created by 壬俊 易 on 12-1-13.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "TarsStream.h"

@implementation TarsStream

@class TarsInputStream;
@class TarsOutputStream;

@synthesize streamBuffer = _streamBuffer;
@synthesize streamSize = _streamSize;
@synthesize cursor = _cursor;

- (id)init
{
	if (self = [super init])
    {
		_streamBuffer = nil;
		_streamSize = 0;
		_cursor = 0;
	}
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

- (NSData *)data
{
    return nil;
}

- (NSString *)description
{
    NSData *originData = [self data];
    assert(_cursor <= originData.length);
    
    UInt8 *buff = (UInt8 *)malloc(sizeof(UInt8) * originData.length);
    [originData getBytes:buff length:originData.length];
    NSString *description = [NSString stringWithFormat:@"%@ ^cursor %@",
                             [NSData dataWithBytes:buff length:_cursor],
                             [NSData dataWithBytes:(buff + _cursor) length:(originData.length - _cursor)]];
    free(buff);
    return description;
}

@end

#pragma mark -

@implementation TarsPair

@synthesize key = _key;
@synthesize value = _value;

+ (TarsPair *)pairWithValue:(id)value forKey:(id)key
{
    TarsPair *pair = [[TarsPair alloc] init];
    pair.value = value;
    pair.key = key;
    return [pair autorelease];
}

- (id)init
{
    if (self = [super init]) 
    {
        _key = nil;
        _value = nil;
    }
    return self;
}

- (void)dealloc
{
    [_key release];
    [_value release];
    [super dealloc];
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@, %@>", NSStringFromClass([self.key class]), NSStringFromClass([self.value class])];
}

@end
