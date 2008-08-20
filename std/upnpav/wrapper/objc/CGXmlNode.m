//
//  CGXmlNode.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import "CGXmlNode.h"

@implementation CGXmlNode

@synthesize xmlNode;

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	NSXMLElement *aXmlNode = [[NSXMLElement alloc] init];
	[self initWithXMLNode:aXmlNode];
	[aXmlNode release];
	return self;
}

- (id)initWithXMLNode:(NSXMLElement *)aXmlNode
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

- (NSString *)attributeValueForName:(NSString *)aName
{
	NSString *attrValue = [[xmlNode attributeForName:aName] stringValue];
	[[attrValue retain] autorelease];
	return attrValue;
}

- (NSString *)elementValueForName:(NSString *)aName
{
	NSArray *elemArray = [xmlNode elementsForName:aName];
	NSString *elemValue = @"";
	for (NSXMLElement *elemNode in elemArray) {
		elemValue = [elemNode stringValue];
		[[elemValue retain] autorelease];
		break;
	}
	return elemValue;
}

- (NSString *)stringValue
{
	return [xmlNode stringValue];
}

- (void)setStringValue:(NSString *)aValue
{
	[xmlNode setStringValue:aValue];
}

- (void)setAttributeWithName:(NSString *)aName stringValue:(NSString *)aValue
{
	[xmlNode removeAttributeForName:aName];
	NSXMLNode *attrNode = [NSXMLNode attributeWithName:aName stringValue:aValue];
	[xmlNode addAttribute:attrNode];
}

@end

