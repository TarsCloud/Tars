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
//  TarsStream.h
//
//  Created by 壬俊 易 on 12-1-13.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <Foundation/Foundation.h>

@class TarsObject;

#pragma mark -

typedef unsigned char           u8;
typedef unsigned short          u16;
typedef unsigned int            u32;
typedef unsigned long long      u64;

#define __LITTLE_ENDIAN	        (0)
#define STREAM_CAPABILITY_EX    (64)
#define STREAM_BUFFER_SIZE      (4096)

#define ASSERT_TRHOW_WS_EXCEPTION(x) ASSERT_FAIL_TRHOW_TUP_SERIALIZABLE_EXCEPTION(x)
#define ASSERT_FAIL_TRHOW_TUP_SERIALIZABLE_EXCEPTION(x)                         \
do {                                                                            \
    if (!(x)) {                                                                 \
        NSException *exception = [NSException exceptionWithName:@"TupSerializableException" reason:[NSString stringWithFormat:@"%s, %d: assert(%@) fail!", __func__, (__LINE__), @#x] userInfo:nil];  \
        @throw exception;                                                       \
    }                                                                           \
} while(0)

#pragma mark -

#define TARS_TYPE_INT1			(0)		// int1 紧跟1个字节整型数据
#define TARS_TYPE_INT2			(1)		// int2 紧跟2个字节整型数据
#define TARS_TYPE_INT4			(2)		// int4 紧跟4个字节整型数据
#define TARS_TYPE_INT8			(3)		// int8 紧跟8个字节整型数据
#define TARS_TYPE_FLOAT			(4)		// float 紧跟4个字节浮点型数据
#define TARS_TYPE_DOUBLE			(5)		// double 紧跟8个字节浮点型数据
#define TARS_TYPE_STRING1		(6)		// String1 紧跟1个字节长度，再跟内容
#define TARS_TYPE_STRING4		(7)		// String4 紧跟4个字节长度，再跟内容
#define TARS_TYPE_MAP			(8)		// Map 紧跟一个整型数据表示Map的大小，再跟[key, value]对列表
#define TARS_TYPE_LIST			(9)		// List 紧跟一个整型数据表示List的大小，再跟元素列表
#define TARS_TYPE_STRUCT_S		(10)	// 自定义结构开始 自定义结构开始标志
#define TARS_TYPE_STRUCT_E		(11)	// 自定义结构结束 自定义结构结束标志，Tag为0
#define TARS_TYPE_ZERO			(12)	// 数字0 表示数字0，后面不跟数据
#define TARS_TYPE_SIMPLE_LIST	(13)	// SimpleList 简单列表（目前用在byte数组），紧跟一个类型字段（目前只支持byte），紧跟4字节表示长度，再跟byte数据
#define TARS_TYPE_UNUSED			(14)

#pragma mark -

@interface TarsStream : NSObject
{
    unsigned char *_streamBuffer;
    int _streamSize;
    int	_cursor;
}

@property (nonatomic, assign) unsigned char *streamBuffer;
@property (nonatomic, assign) int			 streamSize;
@property (nonatomic, assign) int			 cursor;

- (NSData *)data;
- (NSString *)description;

@end

#pragma mark -

@interface TarsPair : NSObject

@property (nonatomic, retain) id key;
@property (nonatomic, retain) id value;

+ (TarsPair *)pairWithValue:(id)value forKey:(id)key;

@end