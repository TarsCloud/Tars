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
//  UniAttribute.m
//  tup
//
//  Created by renjunyi on 12-4-17.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "UniAttribute.h"

@implementation UniAttribute

@synthesize JV2_PROP_EX(r, 0, uniAttributes, M09ONSStringM09ONSStringONSData); 

- (id)init
{
	if (self = [super init]) {
        self.tars_uniAttributes = [NSMutableDictionary dictionary];
	}
	return self;
}

- (void)dealloc
{
    self.tars_uniAttributes = nil;
    [super dealloc];
}

+ (UniAttribute *)fromAttributeData:(NSData *)data
{
    UniAttribute *attribute = [[[UniAttribute alloc] init] autorelease];
    TarsInputStream *attrStream = [TarsInputStream streamWithData:data];
    TarsPair *description = [TarsPair pairWithValue:[TarsPair pairWithValue:[NSData class] forKey:[NSString class]] forKey:[NSString class]];
    [attribute.tars_uniAttributes setDictionary:[attrStream readDictionary:0 required:YES description:description]];
    return attribute;
}

- (NSData *)attributeData
{
    TarsOutputStream *stream = [TarsOutputStream stream];
    [stream writeDictionary:self.tars_uniAttributes tag:0 required:YES];
    return [stream data];
}

- (NSData *)attrValueWithName:(NSString *)name andType:(NSString *)type
{
    NSDictionary *attribute = [self.tars_uniAttributes objectForKey:name];
    if (attribute) {
        NSAssert([attribute count] == 1, @"");
        NSAssert((type == nil || ([attribute objectForKey:type] == [[attribute allValues] lastObject])), @"type mismatch!");
        return [[attribute allValues] lastObject];
    }
    return nil;
}

- (void)setAttrValue:(NSData *)value withName:(NSString *)name andType:(NSString *)type
{
    NSAssert(name != nil && type != nil, @"");
    NSDictionary *attribute = [NSDictionary dictionaryWithObject:value forKey:type];
    [self.tars_uniAttributes setValue:attribute forKey:name];
}

@end

#pragma mark - categories

@implementation TarsObject (uniAttribute)

+ (TarsObject *)objectWithName:(NSString *)name andType:(NSString *)type inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:type];
    TarsInputStream *stream = [TarsInputStream streamWithData:data];
    return [stream readObject:0 required:YES description:self];
}

- (void)setInAttributes:(UniAttribute *)attrs withName:(NSString *)name andType:(NSString *)type
{
    TarsOutputStream *stream = [TarsOutputStream stream];
    [stream writeObject:self tag:0 required:YES];
    [attrs setAttrValue:[stream data] withName:name andType:type];
}

// for debug
+ (TarsObject *)objectWithAttributeData:(NSData *)data
{
    TarsInputStream *stream = [TarsInputStream streamWithData:data];
    return [stream readObject:0 required:YES description:self];
}

@end

@implementation TarsObjectV2 (uniAttribute)

+ (TarsObjectV2 *)objectWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    return (TarsObjectV2 *)[self objectWithName:name andType:[self tarsType] inAttributes:attrs];
}

- (void)setInAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    [self setInAttributes:attrs withName:name andType:[self tarsType]];
}

@end

@implementation NSData (uniAttribute)

+ (NSData *)dataWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:@"list<char>"];
    TarsInputStream *stream = [TarsInputStream streamWithData:data];
    return [stream readData:0 required:YES];
}

- (void)setInAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    TarsOutputStream *stream = [TarsOutputStream streamWithCapability:self.length];
    [stream writeData:self tag:0 required:YES];
    [attrs setAttrValue:[stream data] withName:name andType:@"list<char>"];
}

@end

@implementation NSString (uniAttribute)

+ (NSString *)stringWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:@"string"];
    TarsInputStream *stream = [TarsInputStream streamWithData:data];
    return [stream readString:0 required:YES];
}

- (void)setInAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    TarsOutputStream *stream = [TarsOutputStream streamWithCapability:[self lengthOfBytesUsingEncoding:NSUTF8StringEncoding]];
    [stream writeString:self tag:0 required:YES];
    [attrs setAttrValue:[stream data] withName:name andType:@"string"];
}

@end

@implementation NSNumber (uniAttribute)

+ (BOOL)boolValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:@"bool"];
    if (data == nil) return NO; // ASSERT_TRHOW_WS_EXCEPTION(data != nil);
    TarsInputStream *stream = [TarsInputStream streamWithData:data];
    return [stream readInt:0];
}

+ (void)setBool:(BOOL)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    TarsOutputStream *stream = [TarsOutputStream streamWithCapability:16];
    [stream writeInt:value tag:0];
    [attrs setAttrValue:[stream data] withName:name andType:@"bool"];
}

+ (char)charValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:@"char"];
    if (data == nil) return 0; // ASSERT_TRHOW_WS_EXCEPTION(data != nil);
    TarsInputStream *stream = [TarsInputStream streamWithData:data];
    return [stream readInt:0];
}

+ (void)setChar:(char)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    TarsOutputStream *stream = [TarsOutputStream streamWithCapability:16];
    [stream writeInt:value tag:0];
    [attrs setAttrValue:[stream data] withName:name andType:@"char"];
}

+ (char)shortValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:@"short"];
    if (data == nil) return 0; // ASSERT_TRHOW_WS_EXCEPTION(data != nil);
    TarsInputStream *stream = [TarsInputStream streamWithData:data];
    return [stream readInt:0];
}

+ (void)setShort:(short)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    TarsOutputStream *stream = [TarsOutputStream streamWithCapability:16];
    [stream writeInt:value tag:0];
    [attrs setAttrValue:[stream data] withName:name andType:@"short"];
}

+ (char)intValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:@"int32"];
    if (data == nil) return 0; // ASSERT_TRHOW_WS_EXCEPTION(data != nil);
    TarsInputStream *stream = [TarsInputStream streamWithData:data];
    return [stream readInt:0];    
}

+ (void)setInt:(int)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    TarsOutputStream *stream = [TarsOutputStream streamWithCapability:16];
    [stream writeInt:value tag:0];
    [attrs setAttrValue:[stream data] withName:name andType:@"int32"];
}

+ (long long)longlongValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:@"int64"];
    if (data == nil) return 0; // ASSERT_TRHOW_WS_EXCEPTION(data != nil);
    TarsInputStream *stream = [TarsInputStream streamWithData:data];
    return [stream readInt:0]; 
}

+ (void)setLonglong:(long long)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    TarsOutputStream *stream = [TarsOutputStream streamWithCapability:16];
    [stream writeInt:value tag:0];
    [attrs setAttrValue:[stream data] withName:name andType:@"int64"];    
}

+ (float)floatValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:@"float"];
    if (data == nil) return 0.0; // ASSERT_TRHOW_WS_EXCEPTION(data != nil);
    TarsInputStream *stream = [TarsInputStream streamWithData:data];
    return [stream readFloat:0];
}

+ (void)setFloat:(float)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    TarsOutputStream *stream = [TarsOutputStream streamWithCapability:16];
    [stream writeFloat:value tag:0];
    [attrs setAttrValue:[stream data] withName:name andType:@"float"];
}

+ (double)doubleValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:@"double"];
    if (data == nil) return 0.0; // ASSERT_TRHOW_WS_EXCEPTION(data != nil);
    TarsInputStream *stream = [TarsInputStream streamWithData:data];
    return [stream readDouble:0]; 
}

+ (void)setDouble:(double)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    TarsOutputStream *stream = [TarsOutputStream streamWithCapability:16];
    [stream writeDouble:value tag:0];
    [attrs setAttrValue:[stream data] withName:name andType:@"double"];
}

+ (NSNumber *)numberWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:nil];
    TarsInputStream *stream = [TarsInputStream streamWithData:data];
    return [stream readNumber:0 required:YES];
}

- (void)setInAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    TarsOutputStream *stream = [TarsOutputStream streamWithCapability:16];
    [stream writeNumber:self tag:0 required:YES];
    [attrs setAttrValue:[stream data] withName:name andType:[NSString stringWithUTF8String:[self objCType]]];
}

@end