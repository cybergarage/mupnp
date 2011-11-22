//
//  CGXmlNode.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#if  defined(TARGET_OS_IPHONE)
#import <cybergarage/xml/cxml.h>
#endif
#import <cybergarage/util/cstring.h>

#import "CGXmlNode.h"

@implementation CGXmlNode

@synthesize userInfo;

#if !defined(TARGET_OS_IPHONE)

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
	[self setXmlNode:nil];
    [self setUserInfo:nil];
    
	[super dealloc];
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

#else // defined(TARGET_OS_IPHONE)

@synthesize cXmlNode;

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	self.cXmlNode = cg_xml_node_new();
	return self;
}

- (id)initWithXMLNode:(CgXmlNode *)aXmlNode
{
	if ((self = [super init]) == nil)
		return nil;
        
	self.cXmlNode = cg_xml_node_new();
	cg_xml_node_copy(cXmlNode, aXmlNode);

	return self;
}

- (void)dealloc
{
	cg_xml_node_delete([self cXmlNode]);
    
    [self setUserInfo:nil];
    [self setCXmlNode:nil];
    
	[super dealloc];
}

- (NSString *)attributeValueForName:(NSString *)aName
{
	if (!cXmlNode)
		return nil;
        
    NSString *attributeValueString = nil;
	const char* attributeName = [aName UTF8String];
	const char* attributeValue = cg_xml_node_getattributevalue(cXmlNode, (char *)attributeName);
	if (attributeValue)
		attributeValueString = [NSString stringWithUTF8String:attributeValue];
	return [[attributeValueString retain] autorelease];
}

- (NSString *)elementValueForName:(NSString *)aName
{
	if (!cXmlNode)
		return nil;
	CgXmlNode *elemNode = cg_xml_node_getchildnode(cXmlNode, (char *)[aName UTF8String]);
	if (!elemNode)
		return nil;
	const char* nodeValue = cg_xml_node_getvalue(elemNode);
	if (nodeValue)
	{
		return [NSString stringWithUTF8String:nodeValue];
	}
	return nil;
}
- (NSString *)stringValue
{
	if (!cXmlNode)
		return nil;
	const char* nodeValue = cg_xml_node_getvalue(cXmlNode);
	if (nodeValue)
	{
		return [NSString stringWithUTF8String:nodeValue];
	}
	return nil;
}

- (void)setStringValue:(NSString *)aValue
{
	if (!cXmlNode)
		return;
	cg_xml_node_setvalue(cXmlNode, (char *)[aValue UTF8String]);
}

- (void)setAttributeWithName:(NSString *)aName stringValue:(NSString *)aValue
{
	if (!cXmlNode)
		return;
	cg_xml_node_setattribute(cXmlNode, (char *)[aName UTF8String], (char *)[aValue UTF8String]);
}

#endif

@end

