//
//  CGUpnpAvObject.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <CGXmlNode.h>
#import <CGUpnpAvObject.h>
#import <CGUpnpAvContainer.h>
#import <CGUpnpAvItem.h>
#import <CGUpnpAvConstants.h>

@implementation CGUpnpAvObject

@synthesize xmlNode;

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	return self;
}

- (id)initWithXMLNode:(NSXMLElement *)aXmlNode
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

- (BOOL)isEqual:(id)anObject
{
	if (![anObject isKindOfClass:[CGUpnpAvContainer class]])
		return NO;
	CGUpnpAvObject *avObj = (CGUpnpAvObject *)anObject;
	NSString *selfObjectId = [self objectId];
	if (selfObjectId == nil)
		return NO;
	if (!([selfObjectId isEqualToString:[avObj objectId]]))
		return NO;
	return YES;
}

- (BOOL)isContainer
{
	return [self isKindOfClass:[CGUpnpAvContainer class]];
}

- (BOOL)isItem
{
	return [self isKindOfClass:[CGUpnpAvItem class]];
}

- (void)setParent:(CGUpnpAvObject *)aParent
{
	if (parent != aParent) {
		[parent release];
		parent = [aParent retain];
	}		
}

- (CGUpnpAvObject *)parent
{
	return parent;
}

- (CGUpnpAvObject *)ancestor
{
	CGUpnpAvObject *ancestorObj = self;
	while ([ancestorObj parent] != nil)
		ancestorObj = [ancestorObj parent];
	return ancestorObj;
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

- (void)setObjectId:(NSString *)aValue
{
	[self setAttributeWithName:CG_UPNPAV_OBJECT_ID stringValue:aValue];
}

- (void)setTitle:(NSString *)aValue;
{
	[self setAttributeWithName:CG_UPNPAV_OBJECT_TITLE stringValue:aValue];
}

- (void)setUpnpClass:(NSString *)aValue;
{
	[self setAttributeWithName:CG_UPNPAV_OBJECT_UPNPCLASS stringValue:aValue];
}

- (BOOL)isObjectId:(NSString *)aObjectId
{
	return [aObjectId isEqualToString:[self objectId]];
}

- (BOOL)isTitle:(NSString *)aTitle
{
	return [aTitle isEqualToString:[self title]];
}

- (BOOL)isUpnpClass:(NSString *)aUpnpClass
{
	return [aUpnpClass isEqualToString:[self upnpClass]];
}

@end

