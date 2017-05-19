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
//  TupAgent.m
//
//  Created by 壬俊 易 on 11-11-29.
//  Copyright (c) 2011年 Tencent. All rights reserved.
//

#import "TupAgent.h"
#import "UniPacket.h"
#import "TupHttpRequest.h"

@implementation TupAgent

@synthesize servantName = _servantName;
@synthesize delegate = _delegate;
@synthesize retryUsingOtherServers = _retryUsingOtherServers;

+ (id)agent
{
    return [[[self alloc] init] autorelease];
}

- (id)init 
{
	return [self initWithServant:nil];
}

- (id)initWithServant:(NSString *)name
{
    if (self = [super init]) 
    {
        _servantName = [name retain];
        _delegate = nil;
        _retryUsingOtherServers = NO;
    }
    return self;
}

- (void)dealloc
{
	[_servantName release];
    [_delegate release];
	[super dealloc];
}

- (UniPacket *)invocation:(NSString *)funcName parameter:(UniPacket *)uniPacket
{
    assert(self.servantName != nil);
    assert(self.delegate != nil);
    
    uniPacket.iVersion = [NSNumber numberWithShort:2];
    uniPacket.sServantName = self.servantName;
    uniPacket.sFuncName = funcName;
    
    TupHttpRequest *request = [TupHttpRequest requestWithURL:[self.delegate serverUrl]];
    if (self.retryUsingOtherServers) {
        request.retryUsingOtherServers = YES;
        request.agentDelegate = self.delegate;
    }
    [request setUserInfo:[NSDictionary dictionaryWithObjectsAndKeys:funcName, @"funcName", nil]];
    [request setRequestMethod:@"POST"];
    [request setShouldAttemptPersistentConnection:YES];
    [request addRequestHeader:@"User-Agent"      value:[self.delegate userAgent]];
    [request addRequestHeader:@"Content-Type"    value:@"application/multipart-formdata"];
    [request addRequestHeader:@"Accept"          value:@"*/*"];
    [request addRequestHeader:@"Accept-Encoding" value:@"gzip,deflate"];
    [request addRequestHeader:@"Q-Auth"          value:[self.delegate qAuth]];
    [request addRequestHeader:@"Q-Guid"          value:[self.delegate qGuid]];
    [request addRequestHeader:@"Q-UA"            value:[self.delegate qUA]];
    [request appendPostData:[uniPacket toData]];
    NSLog(@"%@: rmi start with url(%@)!", [request.userInfo objectForKey:@"funcName"], [request url]);
    [request startSynchronous];
    
    if (request.error == nil) {
        NSData *responseData = [request responseData];
        return [UniPacket fromData:responseData];
    }
        
    NSLog(@"%@", request.error);
    return nil;
}

- (id)invocation:(NSString *)funcName parameter:(UniPacket *)uniPacket completeHandle:(void (^)(UniPacket *))handle
{
    assert(self.servantName != nil);
    assert(self.delegate != nil);
    
    uniPacket.iVersion = [NSNumber numberWithShort:2];
    uniPacket.sServantName = self.servantName;
    uniPacket.sFuncName = funcName;

    __block TupHttpRequest *request = [TupHttpRequest requestWithURL:[self.delegate serverUrl]];
    if (self.retryUsingOtherServers) {
        request.retryUsingOtherServers = YES;
        request.agentDelegate = self.delegate;
    }
    [request setUserInfo:[NSDictionary dictionaryWithObjectsAndKeys:funcName, @"funcName", nil]];
    [request setRequestMethod:@"POST"];
    [request setShouldAttemptPersistentConnection:YES];
    [request addRequestHeader:@"User-Agent"      value:[self.delegate userAgent]];
    [request addRequestHeader:@"Content-Type"    value:@"application/multipart-formdata"];
    [request addRequestHeader:@"Accept"          value:@"*/*"];
    [request addRequestHeader:@"Accept-Encoding" value:@"gzip,deflate"];
    [request addRequestHeader:@"Q-Auth"          value:[self.delegate qAuth]];
    [request addRequestHeader:@"Q-Guid"          value:[self.delegate qGuid]];
    [request addRequestHeader:@"Q-UA"            value:[self.delegate qUA]];
    [request appendPostData:[uniPacket toData]];
    [request setCompletionBlock:^{
        NSData *responseData = [request responseData];
        dispatch_async(dispatch_get_global_queue(0, 0), ^{
            UniPacket *uniPacket = [UniPacket fromData:responseData];
            handle(uniPacket);
        });
    }];
    [request setFailedBlock:^{
        NSLog(@"%@", request.error);
        handle(nil);
    }];
    NSLog(@"%@: rmi start with url(%@)!", [request.userInfo objectForKey:@"funcName"], [request url]);
    [request startAsynchronous];

    return request;
}

+ (void)cancel:(id)stub 
{
    if ([stub respondsToSelector:@selector(clearDelegatesAndCancel)]) 
        [stub clearDelegatesAndCancel];
}

@end
