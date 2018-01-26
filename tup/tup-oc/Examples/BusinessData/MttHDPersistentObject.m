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
//  MttHDPersistentObject.m
//  MttHD
//
//  Created by renjunyi on 12-4-14.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttHDPersistentObject.h"

@implementation MttHDPersistentObject

+ (id)theSpecial
{
    id object = nil;
    @synchronized (self) {
        if ((object = [self findFirstByCriteria:@""]) == nil) {
            object = [[[self alloc] init] autorelease];
            [object save];
        }
    }
    return object;
}

+ (NSString *)tupDataDirectory
{
    static NSString *directoryPath = nil;
    if (directoryPath == nil) {
        directoryPath = [[NSHomeDirectory() stringByAppendingPathComponent:@"Library/Tup/"] retain];
        [[NSFileManager defaultManager] createDirectoryAtPath:directoryPath withIntermediateDirectories:YES attributes:nil error:nil];
    }
    return directoryPath;
}

@end
