//
//  CGUpnpAvResource.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import "CGXmlNode.h"
#import "CGUpnpAvResource.h"
#import "CGUpnpAvConstants.h"

@implementation CGUpnpAvResource

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	return self;
}

#if  !defined(TARGET_OS_IPHONE)
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode
#else
- (id)initWithXMLNode:(CgXmlNode *)aXmlNode
#endif
{
	if ((self = [super initWithXMLNode:aXmlNode]) == nil)
		return nil;
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

- (void) finalize
{
	[super finalize];
}

- (NSString *)url
{
	return [self stringValue];
}

- (long long)size
{
	return [[self attributeValueForName:@"size"] longLongValue];
}

- (NSString *)protocolInfo
{
	return [self attributeValueForName:@"protocolInfo"];
}

- (NSString *)protocolInfoAtIndex:(NSUInteger)anIndex
{
	NSString *protocolInfo = [self protocolInfo];
	if (!protocolInfo)
		return nil;
	NSArray *protocols = [protocolInfo componentsSeparatedByString:@":"];
	if ([protocols count] <= anIndex)
		return nil;
	return [[[protocols objectAtIndex:anIndex] retain] autorelease];
}

- (NSString *)protocol
{
	return [self protocolInfoAtIndex:0];
}

- (NSString *)network
{
	return [self protocolInfoAtIndex:1];
}

- (NSString *)contentFormat
{
	return [self protocolInfoAtIndex:2];
}

- (NSString *)additionalInfo
{
	return [self protocolInfoAtIndex:3];
}

- (NSString *)additionalInfoForKey:(NSString*)aKey
{
	NSString *fullAddInfo = [self additionalInfo];
	if (!fullAddInfo)
		return nil;
	NSArray *addInfos = [fullAddInfo componentsSeparatedByString:@";"];
	for (NSString *addInfo in addInfos) {
		if ([addInfo hasPrefix:aKey]) {
			NSArray *tokens = [addInfo componentsSeparatedByString:@"="];
			if ([tokens count] < 2)
				continue;
			NSString *value = [tokens objectAtIndex:1];
			return[[value retain] autorelease];
		}
	}
	return nil;
}

- (NSString *)dlnaOrgPn
{
	return [self additionalInfoForKey:@"DLNA.ORG_PN"];
}

- (NSString *)dlnaOrgOp
{
	return [self additionalInfoForKey:@"DLNA.ORG_OP"];
}

- (NSString *)dlnaOrgFlags
{
	return [self additionalInfoForKey:@"DLNA.ORG_FLAGS"];
}

- (BOOL)isThumbnail
{
	NSString *dlnaOrgPn = [self dlnaOrgPn];
	if (!dlnaOrgPn)
		return NO;
	if ([dlnaOrgPn hasSuffix:@"_TN"])
		return YES;
	return NO;
}


@end

