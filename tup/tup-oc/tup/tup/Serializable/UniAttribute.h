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
//  UniAttribute.h
//  tup
//
//  Created by renjunyi on 12-4-17.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "TarsObjectV2.h"

@interface UniAttribute : TarsObjectV2

@property (nonatomic, retain, JV2_PROP_GS(uniAttributes)) NSMutableDictionary* JV2_PROP_EX(r, 0, uniAttributes, M09ONSStringM09ONSStringONSData);

+ (UniAttribute *)fromAttributeData:(NSData *)data;
- (NSData *)attributeData;

- (NSData *)attrValueWithName:(NSString *)name andType:(NSString *)type;
- (void)setAttrValue:(NSData *)data withName:(NSString *)name andType:(NSString *)type;

@end

#pragma mark - categories

@interface TarsObject (uniAttribute)

+ (TarsObject *)objectWithName:(NSString *)name andType:(NSString *)type inAttributes:(UniAttribute *)attrs;
- (void)setInAttributes:(UniAttribute *)attrs withName:(NSString *)name andType:(NSString *)type;

// for debug
+ (TarsObject *)objectWithAttributeData:(NSData *)data;

@end

@interface TarsObjectV2 (uniAttribute)

+ (TarsObjectV2 *)objectWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
- (void)setInAttributes:(UniAttribute *)attrs withName:(NSString *)name;

@end

@interface NSData (uniAttribute)

+ (NSData *)dataWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
- (void)setInAttributes:(UniAttribute *)attrs withName:(NSString *)name;

@end

@interface NSString (uniAttribute)

+ (NSString *)stringWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
- (void)setInAttributes:(UniAttribute *)attrs withName:(NSString *)name;

@end

@interface NSNumber (uniAttribute)

+ (BOOL)boolValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
+ (void)setBool:(BOOL)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name;

+ (char)charValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
+ (void)setChar:(char)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name;

+ (char)shortValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
+ (void)setShort:(short)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name;

+ (char)intValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
+ (void)setInt:(int)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name;

+ (long long)longlongValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
+ (void)setLonglong:(long long)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name;

+ (float)floatValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
+ (void)setFloat:(float)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name;

+ (double)doubleValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
+ (void)setDouble:(double)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name;

+ (NSNumber *)numberWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
- (void)setInAttributes:(UniAttribute *)attrs withName:(NSString *)name;

@end
