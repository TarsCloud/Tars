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
//  TarsOutputStream.m
//
//  Created by 壬俊 易 on 12-1-13.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "TarsOutputStream.h"
#import "TarsObject.h"

inline static u32 pack_8(u8* dst, u8 val8)
{
	*dst = val8;
	return 1;
}

inline static u32 pack_16(u8* dst, u16 val16)
{
#if __LITTLE_ENDIAN
	*(dst + 1) = (u8)((val16 & 0xFF00) >> 8);
	*(dst + 0) = (u8)(val16 & 0x00FF);
#else
	*(dst + 0) = (u8)((val16 & 0xFF00) >> 8);
	*(dst + 1) = (u8)(val16 & 0x00FF);
#endif
	return 2;
}

inline static u32 pack_32(u8* dst, u32 val32)
{
#if __LITTLE_ENDIAN
	*(dst + 3) = (u8)((val32 & 0xFF000000) >> 24);
	*(dst + 2) = (u8)((val32 & 0x00FF0000) >> 16);
	*(dst + 1) = (u8)((val32 & 0x0000FF00) >> 8);
	*(dst + 0) = (u8)(val32 & 0x000000FF);
#else	
	*(dst + 0) = (u8)((val32 & 0xFF000000) >> 24);
	*(dst + 1) = (u8)((val32 & 0x00FF0000) >> 16);
	*(dst + 2) = (u8)((val32 & 0x0000FF00) >> 8);
	*(dst + 3) = (u8)(val32 & 0x000000FF);
#endif	
	return 4;
}

inline static u32 pack_64(u8* dst, u64 val64)
{
#if __LITTLE_ENDIAN
    *(dst + 7) = (u8)((val64 & 0xFF00000000000000) >> 56);
    *(dst + 6) = (u8)((val64 & 0x00FF000000000000) >> 48);
    *(dst + 5) = (u8)((val64 & 0x0000FF0000000000) >> 40);
    *(dst + 4) = (u8)((val64 & 0x000000FF00000000) >> 32);
	*(dst + 3) = (u8)((val64 & 0x00000000FF000000) >> 24);
	*(dst + 2) = (u8)((val64 & 0x0000000000FF0000) >> 16);
	*(dst + 1) = (u8)((val64 & 0x000000000000FF00) >> 8);
	*(dst + 0) = (u8)(val64 & 0x00000000000000FF);
#else	
    *(dst + 0) = (u8)((val64 & 0xFF00000000000000) >> 56);
    *(dst + 1) = (u8)((val64 & 0x00FF000000000000) >> 48);
    *(dst + 2) = (u8)((val64 & 0x0000FF0000000000) >> 40);
    *(dst + 3) = (u8)((val64 & 0x000000FF00000000) >> 32);
	*(dst + 4) = (u8)((val64 & 0x00000000FF000000) >> 24);
	*(dst + 5) = (u8)((val64 & 0x0000000000FF0000) >> 16);
	*(dst + 6) = (u8)((val64 & 0x000000000000FF00) >> 8);
	*(dst + 7) = (u8)(val64 & 0x00000000000000FF);
#endif	
	return 8;
}

inline static u32 pack_n(u8* dst, u8* val, u32 size)
{
	register u32 i;
	for (i = 0; i < size; ++i)
		*dst++ = *val++;
	return size;
}

@interface TarsOutputStream ()

- (void)preventBufferOverflow:(int)size;

@end

@implementation TarsOutputStream

+ (TarsOutputStream *)stream
{
    return [[[self alloc] init] autorelease];
}

+ (TarsOutputStream *)streamWithCapability:(int)capability
{
    capability = capability + STREAM_CAPABILITY_EX;
    return [[[self alloc] initWithBufferSize:capability] autorelease];
}

- (id)init
{
    return [self initWithBufferSize:STREAM_BUFFER_SIZE];
}

- (id)initWithBufferSize:(int)size
{
	if (self = [super init])
	{
		_streamSize = size > 0 ? size : 1;
		_streamBuffer = malloc(_streamSize);
		_cursor = 0;
	}
	return self;
}

- (void)dealloc
{
	free(_streamBuffer);
	[super dealloc];
}

- (void)preventBufferOverflow:(int)size
{
    assert(_streamSize != 0);
    while ((_streamSize - _cursor) < size)
        _streamSize = _streamSize * 2;
    _streamBuffer = realloc(_streamBuffer, _streamSize);
}

- (NSData *)data
{
    return [NSData dataWithBytes:_streamBuffer length:_cursor];
}

- (void)writeTag:(int)tag type:(int)type
{
	if (tag >= 15) {
        [self preventBufferOverflow:2];
		_streamBuffer[_cursor++] = (unsigned char)((type & 0x0F) | 0xF0);
		_streamBuffer[_cursor++] = (unsigned char)tag;
	}
	else {
        [self preventBufferOverflow:1];
		_streamBuffer[_cursor++] = (unsigned char)((type & 0x0F) | ((tag & 0x0F) << 4));
	}
}

- (void)writeInt1:(char)val
{
    [self preventBufferOverflow:1];
	_cursor += pack_8(&_streamBuffer[_cursor], val);
}

- (void)writeInt2:(short)val
{
    [self preventBufferOverflow:2];
	_cursor += pack_16(&_streamBuffer[_cursor], val);
}

- (void)writeInt4:(int)val
{
    [self preventBufferOverflow:4];
	_cursor += pack_32(&_streamBuffer[_cursor], val);
}

- (void)writeInt8:(long long)val
{
    [self preventBufferOverflow:8];
    _cursor += pack_64(&_streamBuffer[_cursor], val);
}

- (void)writeFloat:(float)val tag:(int)tag
{
	[self writeTag:tag type:TARS_TYPE_FLOAT];
    [self preventBufferOverflow:4];
    _cursor += pack_32(&_streamBuffer[_cursor], val);
}

- (void)writeDouble:(double)val tag:(int)tag
{
	[self writeTag:tag type:TARS_TYPE_DOUBLE];
    [self preventBufferOverflow:8];
    _cursor += pack_32(&_streamBuffer[_cursor], ((((long long)val) >> 32) & 0x00000000FFFFFFFF));
	_cursor += pack_32(&_streamBuffer[_cursor], (((long long)val) & 0xFFFFFFFF));
}

- (void)writeBytes:(const void*)data size:(int)size
{
    [self preventBufferOverflow:size];
    memcpy(&_streamBuffer[_cursor], data, size);
    _cursor += size;
}

- (void)writeInt:(long long)val tag:(int)tag;
{
	if (val == 0l) {
        [self writeTag:tag type:TARS_TYPE_ZERO];
    }
    else if (val >= INT8_MIN && val <= INT8_MAX) {
        [self writeTag:tag type:TARS_TYPE_INT1];
        [self writeInt1:val];
    }
    else if (val >= INT16_MIN && val <= INT16_MAX) {
        [self writeTag:tag type:TARS_TYPE_INT2];
        [self writeInt2:val];
    }
    else if (val >= INT32_MIN && val <= INT32_MAX) {
        [self writeTag:tag type:TARS_TYPE_INT4];
        [self writeInt4:val];
    }
    else if (val >= INT64_MIN && val <= INT64_MAX) {
        [self writeTag:tag type:TARS_TYPE_INT8];
        [self writeInt8:val];
    }
    else {
        assert(0);
    }
}

- (void)writeDictionary:(NSDictionary *)dictionary tag:(int)tag required:(BOOL)required
{
    if (dictionary == nil) {
        assert(required == NO);
    }
    else {
        assert([dictionary isKindOfClass:[NSDictionary class]]);
        [self writeTag:tag type:TARS_TYPE_MAP];
        [self writeInt:[dictionary count] tag:0];
        NSEnumerator *enumerator = [dictionary keyEnumerator];
        id key = nil;
        while (key = [enumerator nextObject]) {
            [self writeAnything:key tag:0 required:YES];
            [self writeAnything:[dictionary objectForKey:key] tag:1 required:YES];
        }
    }
}

- (void)writeArray:(NSArray *)array tag:(int)tag required:(BOOL)required
{
    if (array == nil) {
        assert(required == NO);
    }
    else {
        assert([array isKindOfClass:[NSArray class]]);
        [self writeTag:tag type:TARS_TYPE_LIST];
        [self writeInt:[array count] tag:0];
        for(id object in array) {
            [self writeAnything:object tag:0 required:YES];
        }
    }
}

- (void)writeNumber:(NSNumber *)number tag:(int)tag required:(BOOL)required
{
    if (number == nil) {
        assert(required == NO);
    }
    else {
        assert([number isKindOfClass:[NSNumber class]]);
        if (strcmp([number objCType], @encode(double)) == 0) {  
            [self writeDouble:[number doubleValue] tag:tag];
        }
        else if(strcmp([number objCType], @encode(float)) == 0) {
            [self writeFloat:[number floatValue] tag:tag];
        }
        else {
            [self writeInt:[number longLongValue] tag:tag];
        }
    }
}

- (void)writeObject:(TarsObject *)object tag:(int)tag required:(BOOL)required
{
    if (object == nil) {
        assert(required == NO);
    }
    else {
        assert([object isKindOfClass:[TarsObject class]]);
        TarsOutputStream *stream = [[TarsOutputStream alloc] init];
        [object __pack:stream];
        NSData *data = [NSData dataWithBytes:stream.streamBuffer length:stream.cursor];
        [stream release];
        [self writeTag:tag type:TARS_TYPE_STRUCT_S];
        [self writeBytes:[data bytes] size:[data length]];
        [self writeTag:0 type:TARS_TYPE_STRUCT_E];
    }
}

- (void)writeString:(NSString *)string tag:(int)tag required:(BOOL)required
{
    if (string == nil) {
        assert(required == NO);
    }
    else {
        assert([string isKindOfClass:[NSString class]]);
        const char *rawString = [string UTF8String];
        int length = [string lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        if (length > UCHAR_MAX) {
            [self writeTag:tag type:TARS_TYPE_STRING4];
            [self writeInt4:length];
            [self writeBytes:rawString size:length];
        }
        else {
            [self writeTag:tag type:TARS_TYPE_STRING1];
            [self writeInt1:length];
            [self writeBytes:rawString size:length];
        }
    }
}

- (void)writeData:(NSData *)data tag:(int)tag required:(BOOL)required
{
    if (data == nil) {
        assert(required == NO);
    }
    else {
        assert([data isKindOfClass:[NSData class]]);
        [self writeTag:tag type:TARS_TYPE_SIMPLE_LIST];
        [self writeTag:0 type:TARS_TYPE_INT1];
        [self writeInt:[data length] tag:0];
        [self writeBytes:[data bytes] size:[data length]];
    }
}

- (void)writeAnything:(id)anything tag:(int)tag required:(BOOL)required
{
    if ([anything isKindOfClass:[NSDictionary class]]) {
        [self writeDictionary:anything tag:tag required:required];
    }
    else if ([anything isKindOfClass:[NSArray class]]) {
        [self writeArray:anything tag:tag required:required];
    }
    else if ([anything isKindOfClass:[NSNumber class]]) {
        [self writeNumber:anything tag:tag required:required];
    }
    else if ([anything isKindOfClass:[TarsObject class]]) {
        [self writeObject:anything tag:tag required:required];
    }
    else if ([anything isKindOfClass:[NSString class]]) {
        [self writeString:anything tag:tag required:required];
    }
    else if ([anything isKindOfClass:[NSData class]]) {
        [self writeData:anything tag:tag required:required];
    }
    else {
        assert(0);
    }
}

@end
