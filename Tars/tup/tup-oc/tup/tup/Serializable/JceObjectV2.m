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
//  TarsObjectV2.m
//
//  Created by 壬俊 易 on 12-3-13.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "TarsObjectV2.h"
#import "TarsStream.h"

#pragma mark - TarsPair (V2)

@interface TarsPair (V2)

+ (id)analyzeExtStr:(NSString *)str;
+ (TarsPair *)pairFromExtStr:(NSString *)str;

@end

@implementation TarsPair (V2)

+ (id)analyzeExtStr:(NSString *)str
{
    unichar flag = [str characterAtIndex:0];
    switch (flag) {
        id key = nil, value = nil;
        unichar l = 0;
        case 'V':
            value = [TarsPair analyzeExtStr:[str substringFromIndex:1]];
            return [TarsPair pairWithValue:value forKey:nil];
        case 'M':
            l = [str substringWithRange:NSMakeRange(1, 2)].intValue;
            key = [TarsPair analyzeExtStr:[str substringWithRange:NSMakeRange(3, l)]];
            value = [TarsPair analyzeExtStr:[str substringFromIndex:(3 + l)]];
            return [TarsPair pairWithValue:value forKey:key];
        case 'O':
            return NSClassFromString([str substringFromIndex:1]);
        default:
            assert(0);
            return nil;
    }
}

+ (TarsPair *)pairFromExtStr:(NSString *)str
{
    assert([str length] < 128);
    id pair = [self analyzeExtStr:str];
    assert([pair isKindOfClass:[TarsPair class]]);
    return pair;
}

@end

#pragma mark - TarsPropertyInfo

@interface TarsPropertyInfo : NSObject

@property (nonatomic, assign) NSInteger tag;    // index
@property (nonatomic, assign) BOOL flag;        // required
@property (nonatomic, retain) NSString *name;
@property (nonatomic, retain) NSString *type;
@property (nonatomic, retain) TarsPair *ext;     // for vector & map

+ (id)propertyInfo;
- (NSComparisonResult)compareWithTag:(TarsPropertyInfo *)obj;

@end

@implementation TarsPropertyInfo

@synthesize tag;
@synthesize flag;
@synthesize name;
@synthesize type;
@synthesize ext;

+ (id)propertyInfo
{
    return [[[self alloc] init] autorelease];
}

- (id)init
{
    if (self = [super init]) {
    }
    return self;
}

- (void)dealloc 
{
    self.name = nil;
    self.type = nil;
    self.ext = nil;
    [super dealloc];
}

- (NSComparisonResult)compareWithTag:(TarsPropertyInfo *)obj
{
    if (self.tag < obj.tag)
        return NSOrderedAscending;
    else if (self.tag == obj.tag)
        return NSOrderedSame;
    return NSOrderedDescending;
}

- (NSString *)description
{
    return [NSString stringWithFormat:
            @"{\n"
            @"    tag      : %d\n"
            @"    required : %d\n"
            @"    name     : %@\n"
            @"    type     : %@\n"
            @"    ext      : %@\n"
            @"}\n", self.tag, self.flag, self.name, self.type, self.ext];
}

@end

#pragma mark - TarsObjectV2

static NSMutableDictionary *tarsv2_class_props_descriptions;

@interface TarsObjectV2 ()

- (NSDictionary *)tarsPropsDescription;

@end

@implementation TarsObjectV2

+ (NSString *)tarsType
{
    return nil;
}

- (NSString *)tarsType
{
    return [[self class] tarsType];
}

- (NSDictionary *)tarsPropsDescription
{
    NSMutableDictionary *propsDescription = nil;
    @synchronized(self) {
        if (tarsv2_class_props_descriptions == nil)
            tarsv2_class_props_descriptions = [[[NSMutableDictionary alloc] init] retain];
        assert(tarsv2_class_props_descriptions != nil);
        
        NSString* className = NSStringFromClass([self class]);
        if ((propsDescription = [tarsv2_class_props_descriptions objectForKey:className]) == nil) 
        {
            NSDictionary *props = [[self class] tarsPropertiesWithEncodedTypes];
            propsDescription = [NSMutableDictionary dictionaryWithCapacity:[props count]];
            [tarsv2_class_props_descriptions setObject:propsDescription forKey:className];
            for (NSString *propName in props) 
            {
                if ([propName hasPrefix:JV2_PROP_LFX_STR]) {
                    NSArray *strs = [propName componentsSeparatedByString:@"_"];
                    assert([strs count] >= 5 && [strs count] <= 8);
                    TarsPropertyInfo *propInfo = [TarsPropertyInfo propertyInfo];
                    propInfo.tag = [[strs objectAtIndex:2] integerValue];
                    propInfo.flag = [[strs objectAtIndex:3] isEqualToString:@"r"];
                    propInfo.name = [NSString stringWithFormat:@"%@%@", JV2_PROP_NFX_STR, [strs objectAtIndex:4]];
                    propInfo.type = [props objectForKey:propName];
                    propInfo.ext = ([strs count] == 8 ? [TarsPair pairFromExtStr:[strs objectAtIndex:7]] : nil);
                    [propsDescription setObject:propInfo forKey:[NSNumber numberWithInt:propInfo.tag]];
                }
            }
        }
        return propsDescription;
    }
}

- (void)__pack:(TarsOutputStream *)stream
{
	AUTO_COLLECT
    
    NSDictionary *description = [self tarsPropsDescription];
    assert(description != nil);
    
    NSArray *keys = [description keysSortedByValueUsingSelector:@selector(compareWithTag:)];
    for (NSNumber* key in keys) 
    {
        TarsPropertyInfo *propInfo = [description objectForKey:key];
        
        // 关于类型编码，请参看《Object-C Runtime Programming Guide》的“Type Encodings”部分
        id theProperty = [self valueForKey:propInfo.name];
        switch ([propInfo.type characterAtIndex:0]) 
        {
            case 'B':   // bool
            case 'c':   // char
            case 'C':   // unsigned char
            case 's':   // short
            case 'S':   // unsigned short
            case 'i':   // int
            case 'I':   // unsigned int
            case 'l':   // long
            case 'L':   // unsigned long
            case 'q':   // long long
                if (propInfo.flag != NO || theProperty != 0)
                    [stream writeInt:[theProperty longLongValue] tag:propInfo.tag];
                break;
            case 'f':   // float
                if (propInfo.flag != NO || theProperty != 0)
                    [stream writeFloat:[theProperty floatValue] tag:propInfo.tag];
                break;
            case 'd':   // double
                if (propInfo.flag != NO || theProperty != 0)
                    [stream writeDouble:[theProperty doubleValue] tag:propInfo.tag];
                break;
            case '@':   // objects
                if (propInfo.flag != NO || theProperty != nil)
                    [stream writeAnything:theProperty tag:propInfo.tag required:propInfo.flag];
                break;
            default:
                assert(0);
                break;
        }
    }
    
	AUTO_RELEASE
}

- (void)__unpack:(TarsInputStream *)stream
{
	AUTO_COLLECT
    
    NSDictionary *description = [self tarsPropsDescription];
    assert(description != nil);
    
    NSArray *keys = [description keysSortedByValueUsingSelector:@selector(compareWithTag:)];
    for (NSNumber* key in keys) 
    {
        TarsPropertyInfo *propInfo = [description objectForKey:key];
        
        // 关于类型编码，请参看《Object-C Runtime Programming Guide》的“Type Encodings”部分
        switch ([propInfo.type characterAtIndex:0]) 
        {
            case 'B':   // bool
            case 'c':   // char
            case 'C':   // unsigned char
            case 's':   // short
            case 'S':   // unsigned short
            case 'i':   // int
            case 'I':   // unsigned int
            case 'l':   // long
            case 'L':   // unsigned long
            case 'q':   // long long
            case 'f':   // float
            case 'd':   // double
            {
                NSNumber *value = [stream readNumber:propInfo.tag required:propInfo.flag];
                if (value != nil)
                    [self setValue:value forKey:propInfo.name];
                break;
            }
            case '@':   // objects
            {
                Class cls = NSClassFromString([propInfo.type substringWithRange:NSMakeRange(2, [propInfo.type length] - 3)]);
                id value = [stream readAnything:propInfo.tag required:propInfo.flag description:(propInfo.ext != nil ? propInfo.ext : cls)];
                if (value != nil) {
                    assert([[value class] isSubclassOfClass:cls]);
                    [self setValue:value forKey:propInfo.name];
                }
                break;
            }
            default:
                assert(0);
                break;
        }
    }
    
	AUTO_RELEASE
}


@end
