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
//  TupHttpRequest.m
//  tup
//
//  Created by 壬俊 易 on 12-4-11.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "TupHttpRequest.h"
#import "TupAgentDelegate.h"

@interface TupHttpRequest ()

@property (retain) TupHttpRequest *retryRequest;
@property (assign) TupHttpRequest *retryRequestRef;
@property (retain) TupHttpRequest *parentRequest;

@end

@implementation TupHttpRequest

@synthesize retryRequest = _retryRequest;
@synthesize retryRequestRef = _retryRequestRef;
@synthesize parentRequest = _parentRequest;
@synthesize retryUsingOtherServers = _retryUsingOtherServers;
@synthesize agentDelegate = _agentDelegate;

- (id)initWithURL:(NSURL *)newURL
{
    if (self = [super initWithURL:newURL]) {
        self.retryUsingOtherServers = NO;
        self.agentDelegate = nil;
	}
	return self;
}

- (void)dealloc
{
    self.retryRequest = nil;
    self.parentRequest = nil;
    self.agentDelegate = nil;
    [super dealloc];
}

- (id)copyWithZone:(NSZone *)zone
{    
    TupHttpRequest* clone = [super copyWithZone:zone]; 
    clone.retryUsingOtherServers = self.retryUsingOtherServers;
    clone.agentDelegate = self.agentDelegate;
    if (clone->failureBlock == nil && self->failureBlock != nil)
        clone->failureBlock = [self->failureBlock copy];
    if (clone->completionBlock == nil && self->completionBlock != nil)
        clone->completionBlock = [self->completionBlock copy];
    return clone;
}

- (void)start
{
    if (self.retryUsingOtherServers != NO && self.retryRequest == nil) {
        self.retryRequest = [[self copy] autorelease];
        self.retryRequestRef = self.retryRequest;
    }
    [super start];
}

- (void)startSynchronous
{
    if (self.retryUsingOtherServers != NO && self.retryRequest == nil) {
        self.retryRequest = [[self copy] autorelease];
        self.retryRequestRef = self.retryRequest;
    }
    [super startSynchronous];
}

- (void)startAsynchronous
{
    if (self.retryUsingOtherServers != NO && self.retryRequest == nil) {
        self.retryRequest = [[self copy] autorelease];
        self.retryRequestRef = self.retryRequest;
    }
    [super startAsynchronous];
}

- (void)cancel
{
    [self.retryRequestRef cancel];
    [super cancel];
}

- (void)clearDelegatesAndCancel
{
    [self.retryRequestRef clearDelegatesAndCancel];
    [super clearDelegatesAndCancel];
}

- (NSData *)responseData
{
    if ([self.retryRequestRef retryRequestRef] != nil) {
        [self.retryRequestRef responseData];
    }
    return [super responseData];
}

- (void)failWithError:(NSError *)theError
{
    if (self.retryUsingOtherServers == YES && theError.code != ASIRequestCancelledErrorType) 
    {
        assert(self.retryRequest != nil);
        assert(self.agentDelegate != nil);
        
        [self.agentDelegate invalidateServerUrl:self.originalURL];
        if ([self.agentDelegate serverUrl] != nil) {
            NSLog(@"%@: rmi retry with url(%@)!", [self.userInfo objectForKey:@"funcName"], [self.agentDelegate serverUrl]);
            [self.retryRequest setURL:[self.agentDelegate serverUrl]];
            [self.retryRequest startAsynchronous];
            [self setFailedBlock:nil];
            [self setCompletionBlock:nil];
            self.retryRequest.parentRequest = self;
            self.retryRequest = nil;
        }
    }
    
    [super failWithError:theError];
}

@end
