//
//  CGXmlNode.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/NSString.h>
#import <Foundation/NSArray.h>

#if  defined(TARGET_OS_IPHONE) && !defined(_CG_XML_CXML_H_)
typedef void CgXmlNode;
#endif

/**
 * The CGXmlNode class is a wrapper class for NSXMLNode.
 */
@interface CGXmlNode : NSObject 
{
#if  !defined(TARGET_OS_IPHONE)
	NSXMLElement *xmlNode;
#else
	CgXmlNode *cXmlNode;
#endif
}
- (id)init;
#if  !defined(TARGET_OS_IPHONE)
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode;
#else
- (id)initWithXMLNode:(CgXmlNode *)aXmlNode;
#endif
- (NSString *)attributeValueForName:(NSString *)aName;
- (NSString *)elementValueForName:(NSString *)aName;
- (NSString *)stringValue;
- (void)setStringValue:(NSString *)aValue;
- (void)setAttributeWithName:(NSString *)aName stringValue:(NSString *)aValue;
@end

