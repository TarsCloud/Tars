//
//  UniPacket.m
//
//  Created by 壬俊 易 on 11-12-1.
//  Copyright (c) 2011年 Tencent. All rights reserved.
//

#import "UniPacket.h"
#import "TarsInputStream.h"
#import "TarsOutputStream.h"

@implementation UniPacket

@synthesize attributes = _attributes;

+ (UniPacket *)packet
{
    return [[[self alloc] init] autorelease];
}

- (id)init
{
	if (self = [super init]) {
        _attributes = [[UniAttribute alloc] init];
	}
	return self;
}

- (void)dealloc
{
    [_attributes release];
	[super dealloc];
}

- (NSString *)description
{
    NSString *result;
    result = [NSString stringWithFormat:
              @"\n--------------- UniPacket ---------------\n"
              @"version     : %@\n"
              @"packet-type : %@\n"
              @"message-type: %@\n"
              @"request-id  : %@\n"
              @"servant-name: %@\n"
              @"func-name   : %@\n"
              @"buffer      : %@\n"
              @"timeout     : %@\n"
              @"context     : %@\n"
              @"status      : %@\n"
              @"----------------- Attributes --------------\n"
              @"%@\n",
              self.iVersion, 
              self.cPacketType, 
              self.iMessageType, 
              self.iRequestId, 
              self.sServantName, 
              self.sFuncName, 
              self.sBuffer, 
              self.iTimeout, 
              self.context, 
              self.status,
              self.attributes.tars_uniAttributes];
    return result;
}

#pragma mark - inner

- (void)__pack:(TarsOutputStream *)stream
{
	AUTO_COLLECT
    
    // 将attributes打包成二进制数据，并将数据添加到RequestPacket的_buffer域中
    self.sBuffer = [self.attributes attributeData];
    
    // 为UniPacket包设置4个字节长度标识
    [stream writeInt4:0];
    
    // 然后调用父类方法完成RequestPacket的数据打包
    [super __pack:stream];
    
    // 更改UniPacket包长度标识为实际的长度
    {
        int size = stream.cursor;
        unsigned char *dst = stream.streamBuffer;
#if 0
        dst[3] = (unsigned char)((size & 0xFF000000) >> 24);
        dst[2] = (unsigned char)((size & 0x00FF0000) >> 16);
        dst[1] = (unsigned char)((size & 0x0000FF00) >> 8);
        dst[0] = (unsigned char) (size & 0x000000FF);
#else	
        dst[0] = (unsigned char)((size & 0xFF000000) >> 24);
        dst[1] = (unsigned char)((size & 0x00FF0000) >> 16);
        dst[2] = (unsigned char)((size & 0x0000FF00) >> 8);
        dst[3] = (unsigned char) (size & 0x000000FF);
#endif
    }
    
	AUTO_RELEASE
}

- (void)__unpack:(TarsInputStream *)stream
{
	AUTO_COLLECT
    
    // 读取UniPacket4个字节的长度标志
    stream.headType = TARS_TYPE_INT4;
    int size = [stream readInt4];
    ASSERT_TRHOW_WS_EXCEPTION(stream.streamSize == size);
    
    // 调用父类方法完成RequestPacket的数据解包
    [super __unpack:stream];
    
    // 将RequestPacket的_buffer域中的数据进行解包
    [_attributes release];
    _attributes = [[UniAttribute fromAttributeData:self.sBuffer] retain];

	AUTO_RELEASE
}

#pragma mark - deprecated

- (id)getObjectAttr:(NSString *)attrName forClass:(Class)theClass
{
    assert([theClass isSubclassOfClass:[TarsObject class]]);
    return [theClass objectWithName:attrName andType:nil inAttributes:self.attributes];
}

- (void)putDataAttr:(NSString *)attrName value:(NSData *)attrValue
{
    [attrValue setInAttributes:self.attributes withName:attrName];
}

- (void)putObjectAttr:(NSString *)attrName type:(NSString *)attrType value:(TarsObject *)attrValue
{
    [attrValue setInAttributes:self.attributes withName:attrName andType:attrType];
}

- (void)putObjectAttr:(NSString *)attrName value:(TarsObjectV2 *)attrValue
{
    [attrValue setInAttributes:self.attributes withName:attrName];
}

@end
