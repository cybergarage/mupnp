//
//  CGUpnpAvObject.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <CGUpnpAvObject.h>
#import <CGUpnpAvContainer.h>
#import <CGUpnpAvItem.h>

@implementation CGUpnpAvObject

@synthesize xmlNode;
@synthesize parent;

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	NSXMLNode *aXmlNode = [[NSXMLNode alloc] init];
	[self initWithXMLNode:aXmlNode];
	[aXmlNode release];
	return self;
}

- (id)initWithXMLNode:NSXMLNode *aXmlNode
{
	if ((self = [super init]) == nil)
		return nil;
	[self setXmlNode:aXmlNode];
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

- (BOOL)isContainer
{
	return [self isKindOfClass:[CGUpnpAvContainer class]];
}

- (BOOL)isItem
{
	return [self isKindOfClass:[CGUpnpAvItem class]];
}

- (CGUpnpAvObject *)ancestorObject
{
	CGUpnpAvObject *ancestorObj = self;
	while ([ancestorObj parentObject] != nil)
		ancestorObj = [ancestorObj parentObject];
	return ancestorObj;
}

- (NSString *)attributeValueForName:(NSString *)name
{
	NSString *attrValue = [aXmlNode attributeForName:name] stringValue];
	[[attrValue retain] autorelease];
	return attrValue;
}

- (NSString *)elementValueForName:(NSString *)name
{
	NSArray *elemArray = [contentNode elementsForName:name];
	NSString *elemValue = @"";
	for (NSXMLNode *xmlNode in elemArray) {
		elemValue = [xmlNode stringValue];
		[[elemValue retain] autorelease];
		break;
	}
	return elemValue;
}

- (NSString *)objectId
{
	return [self attributeValueForName:CG_UPNPAV_OBJECT_ID];
}

- (NSString *)title;
{
	return [self elementValueForName:CG_UPNPAV_OBJECT_TITLE];
}

- (NSString *)upnpClass;
{
	return [self elementValueForName:CG_UPNPAV_OBJECT_UPNPCLASS];
}

@end

