//
//  CGUpnpAvObject.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import "CGXmlNode.h"
#import "CGUpnpAvObject.h"
#import "CGUpnpAvResource.h"
#import "CGUpnpAvItem.h"

@implementation CGUpnpAvItem

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	resourceArray = [[NSMutableArray alloc] init];
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
	resourceArray = [[NSMutableArray alloc] init];
	return self;
}

- (void)dealloc
{
	[resourceArray release];
	[super dealloc];
}

- (void) finalize
{
	[resourceArray release];
	[super finalize];
}

- (void)addResource:(CGUpnpAvResource *)res
{
	[resourceArray addObject:res];
}

- (void)removeResource:(CGUpnpAvResource *)res
{
	[resourceArray removeObject:res];
}

- (NSArray *)resources
{
	return resourceArray;
}

- (NSString *)thumbnailUrl
{
	NSString *tbUrl = [self albumArtURI];
	if (tbUrl && 0 < [tbUrl length])
		return [[tbUrl retain] autorelease];
	for (CGUpnpAvResource *res in [self resources]) {
		if ([res isThumbnail])
			return [[[res url] retain] autorelease];
	}
	return nil;
}

@end
