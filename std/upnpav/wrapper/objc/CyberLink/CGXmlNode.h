//
//  CGXmlNode.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/NSString.h>

/**
 * The CGXmlNode class is a wrapper class for NSXMLNode.
 */
@interface CGXmlNode : NSObject 
{
	NSXMLElement *xmlNode;
}
@property(readwrite, copy) NSXMLElement *xmlNode;
- (id)init;
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode;
- (NSString *)attributeValueForName:(NSString *)aName;
- (NSString *)elementValueForName:(NSString *)aName;
- (NSString *)stringValue;
- (void)setStringValue:(NSString *)aValue;
- (void)setAttributeWithName:(NSString *)aName stringValue:(NSString *)aValue;
@end

