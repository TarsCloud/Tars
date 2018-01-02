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
//  MttLoginReq+Adapter.m
//  MttHD
//

#import "MttLoginReq+Adapter.h"
#import "MttHDTupManager.h"
#import "MttHDCCCryptEx.h"
#import "MttHDVerifyRes.h"
#import "MttHDTupAdapters.h"

@implementation MttLoginReq (adapter)

+ (id)mtthdInstance 
{
    MttLoginReq *instance = [MttLoginReq object];    
    instance.stUB = [MttUserBase mtthdInstance];
    instance.sOrigGUID = [MttHDLoginInfo theSpecial].guid;
    instance.vCrypt = [self key];
    instance.iWidth = [NSNumber numberWithInt:[UIScreen mainScreen].bounds.size.width];
    instance.iHeight = [NSNumber numberWithInt:[UIScreen mainScreen].bounds.size.height];
    instance.sChannel = MttHDTupEnvironment.channel;
    instance.eRelayProtoVer = [NSNumber numberWithInt:MttRelayProtoVer_1];
    instance.sMark = [self signature];
    instance.iVerifyId = [NSNumber numberWithInt:[MttHDVerifyInfo theSpecial].verifyId];
    return instance;
}

+ (NSData *)key
{
    MttHDLoginInfo *loginInfo = [MttHDLoginInfo theSpecial];
    
    unsigned char buff[32] = { 0 };
    [loginInfo.guid getBytes:buff length:(loginInfo.guid.length < 16 ? loginInfo.guid.length : 16)];
    for (int i = 31; i >= 0; i = i - 2) {
        buff[i] = rand();
        buff[i - 1] = buff[i / 2];
    }
    NSData *data = [NSData dataWithBytes:buff length:32];
    char* key = "\x7f\xca\xad\x73\x40\xe2\xba\x6b\x83\x58\xda\x0f\x0e\xfc\x1d\xa9\x32\xf1\x73\x34\x26\x3c\x9f\xf5";
    NSString *dKey = [NSString stringWithCString:key encoding:NSISOLatin1StringEncoding];
    return [data tripleDesWithKey:dKey];
}

+ (NSData *)signature
{
    MttHDVerifyInfo *verifyInfo = [MttHDVerifyInfo theSpecial];
    
    NSArray *info = [verifyInfo.parameter componentsSeparatedByString:@","];
    @try {
        NSData *resData = [MttHDVerifyRes verifyResWithName:[info objectAtIndex:0]];
        NSInteger startPos = [[info objectAtIndex:1] intValue];
        NSInteger endPos = [[info objectAtIndex:2] intValue];
        NSData *data = [resData subdataWithRange:NSMakeRange(startPos, endPos - startPos + 1)];
        return [data md5];
    }
    @catch (NSException *exception) {
        NSLog(@"create signature error! e=%@", exception);
    }
    return nil;
}

@end
