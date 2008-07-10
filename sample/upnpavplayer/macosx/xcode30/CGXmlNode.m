//
//  CGXmlNode.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <CGXmlNode.h>

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

- (NSString *)attributeValueForName:(NSString *)name
{
	NSString *attrValue = [[xmlNode attributeForName:name] stringValue];
	[[attrValue retain] autorelease];
	return attrValue;
}

- (NSString *)elementValueForName:(NSString *)name
{
	NSArray *elemArray = [xmlNode elementsForName:name];
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

@end

