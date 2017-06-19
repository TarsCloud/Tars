//
//  RequestF.mm
//
//  Created by renjunyi on 11-12-1.
//  Copyright (c) 2011年 Tencent. All rights reserved.
//

#import "RequestF.h"

#pragma mark -

@implementation RequestPacket

@synthesize iVersion = _iVersion;
@synthesize cPacketType = _cPacketType;
@synthesize iMessageType = _iMessageType;
@synthesize iRequestId = _iRequestId;
@synthesize sServantName = _sServantName;
@synthesize sFuncName = _sFuncName;
@synthesize sBuffer = _sBuffer;
@synthesize iTimeout = _iTimeout;
@synthesize context = _context;
@synthesize status = _status;

- (id)init
{
	if (self = [super init])
	{
		self.iVersion = [NSNumber numberWithShort:2];
		self.cPacketType = [NSNumber numberWithShort:0];
		self.iMessageType = [NSNumber numberWithInt:0];
		self.iRequestId = [NSNumber numberWithInt:[NSDate timeIntervalSinceReferenceDate]];
		self.sServantName = @"";
		self.sFuncName = @"";
		self.sBuffer = [NSData data];
		self.iTimeout = [NSNumber numberWithInt:0];
		self.context = [NSDictionary dictionary];
		self.status = [NSDictionary dictionary];
	}
	return self;
}

- (void)dealloc
{
    self.iVersion = nil;
    self.cPacketType = nil;
    self.iMessageType = nil;
    self.iRequestId = nil;
    self.sServantName = nil;
    self.sFuncName = nil;
    self.sBuffer = nil;
    self.iTimeout = nil;
    self.context = nil;
    self.status = nil;
	[super dealloc];
}

- (void)__pack:(TarsOutputStream *)stream
{
	AUTO_COLLECT

    [stream writeNumber:self.iVersion tag:1 required:YES];
    [stream writeNumber:self.cPacketType tag:2 required:YES];
    [stream writeNumber:self.iMessageType tag:3 required:YES];
    [stream writeNumber:self.iRequestId tag:4 required:YES];
    [stream writeString:self.sServantName tag:5 required:YES];
    [stream writeString:self.sFuncName tag:6 required:YES];
    [stream writeData:self.sBuffer tag:7 required:YES];
    [stream writeNumber:self.iTimeout tag:8 required:YES];
    [stream writeDictionary:_context tag:9 required:YES];
    [stream writeDictionary:_status tag:10 required:YES];

	AUTO_RELEASE
}

- (void)__unpack:(TarsInputStream *)stream
{
	AUTO_COLLECT

    self.iVersion = [stream readNumber:1 required:YES];
    self.cPacketType = [stream readNumber:2 required:YES];
    self.iMessageType = [stream readNumber:3 required:YES];
    self.iRequestId = [stream readNumber:4 required:YES];
    self.sServantName = [stream readString:5 required:YES];
    self.sFuncName = [stream readString:6 required:YES];
    self.sBuffer = [stream readData:7 required:YES];
    self.iTimeout = [stream readNumber:8 required:YES];
    self.context = [stream readDictionary:9 required:YES description:[TarsPair pairWithValue:[NSString class] forKey:[NSString class]]];
    self.status = [stream readDictionary:10 required:YES description:[TarsPair pairWithValue:[NSString class] forKey:[NSString class]]];
    
	AUTO_RELEASE
}

@end
