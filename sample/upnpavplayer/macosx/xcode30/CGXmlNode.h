//
//  CGXmlNode.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

/**
 * The CGXmlNode class is a wrapper class for NSXMLNode.
 */
@interface CGXmlNode : NSObject 
{
	NSXMLElement *xmlNode;
}
@property(readwrite, copy) NSXMLNode *xmlNode;
- (id)init;
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode;
- (NSString *)attributeValueForName:(NSString *)name;
- (NSString *)elementValueForName:(NSString *)name;
- (NSString *)stringValue;
@end

