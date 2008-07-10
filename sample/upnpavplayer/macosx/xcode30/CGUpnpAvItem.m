//
//  CGUpnpAvObject.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <CGUpnpAvItem.h>
#import <CGUpnpAvResource.h>

@implementation CGUpnpAvItem

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	resourceArray = [[NSMutableArray alloc] init];
	return self;
}

- (id)initWithXMLNode:(NSXMLElement *)aXmlNode
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

@end
