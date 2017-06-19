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
//  MttHDTupManager.m
//  MttHD
//
//  Created by 壬俊 易 on 12-3-20.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttHDTupManager.h"
#import "MttVerifyService.h"
#import "MttLoginService.h"
#import "MttConfigService.h"
#import "MttStatService.h"
#import "MttHDTupEnvironment.h"
#import "MttCoolReadService.h"

@interface MttHDTupManager ()

@property (readonly) NSNotificationCenter *tupNotificationCenter;

@end

@implementation MttHDTupManager

@synthesize tupNotificationCenter = _tupNotificationCenter;

+ (MttHDTupManager *)sharedMttHDTupManager
{
    static MttHDTupManager *sharedMttHDTupManager = nil; 
    @synchronized(self) {
        if (sharedMttHDTupManager == nil)
            sharedMttHDTupManager = [[[self alloc] init] retain];
    }
    return sharedMttHDTupManager;
}

+ (NSNotificationCenter *)defaultNotificationCenter
{
    return [self sharedMttHDTupManager].tupNotificationCenter;
}

- (id)init
{
	if (self = [super init]) {
        _tupNotificationCenter = [[NSNotificationCenter alloc] init];
	}
	return self;
}

- (void)dealloc
{
    [_tupNotificationCenter release];
    [super dealloc];
}

- (void)startStandardProcedure
{
    [self startVerify];
    //加快启动速度，推迟10秒拉取视频
    [self performSelector:@selector(startVideo) withObject:nil afterDelay:10];
}

- (void)startVerify
{
    MttVerifyService *verifyService = [MttVerifyService service];
    
    NSLog(@"===> start verify!");
    [verifyService verifyWithCompleteBlock:^{
        NSLog(@"verify success!");
        [self startLogin];
    } failedBlock: ^{
        NSLog(@"verify failed!");
    }];
}

- (void)startLogin
{
    MttLoginService *loginService = [MttLoginService service];
    
    NSLog(@"===> start login!");
    [loginService loginWithCompleteBlock:^{
        NSLog(@"login success!");
        [self startConfig];
        [self startStat];
    } failedBlock:^{
        NSLog(@"login failed!");
    }];
}

- (void)startConfig
{
    MttConfigService *configService = [MttConfigService service];
    
    NSLog(@"===> start getPageConfig!");
    [configService getPageConfigWithCompleteBlock:^{
        NSLog(@"getPageConfig success!");
    } failedBlock:^{
        NSLog(@"getPageConfig failed!");
    }];
    
    NSLog(@"===> start getSplash!");
    [configService getSplashWithCompleteBlock:^{
        NSLog(@"getSplash success!");
    } failedBlock:^{
        NSLog(@"getSplash failed!");
    }];
}

- (void)startStat
{
    MttStatService *statService = [MttStatService service];
    
    NSLog(@"===> start stat!");
    [statService statWithCompleteBlock:^{
        NSLog(@"stat success!");
    } failedBlock:^{
        NSLog(@"stat failed!");
    }];
}
-(void)startVideo
{
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(startVideo) object:nil];
    MttVideoService *videoSevice = [MttVideoService service];
    NSLog(@"===> start getVideo!");
    [videoSevice getVideoWithCompleteBlock:^{
        NSLog(@"getVideo success!");
    } failedBlock:^{
        NSLog(@"getVideo failed!");
    }];
    [self performSelector:@selector(startVideo) withObject:nil afterDelay:60*60];
}

@end
