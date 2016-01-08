//
//  CGXmlNode.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#if defined(TARGET_OS_IPHONE)
#import <Foundation/NSString.h>
#import <Foundation/NSArray.h>
#else
#import <Foundation/Foundation.h>
#endif

#if defined(TARGET_OS_IPHONE) && !defined(_MUPNP_XML_XML_H_)
typedef void mUpnpXmlNode;
#endif

/**
 * The CGXmlNode class is a wrapper class for NSXMLNode.
 */
@interface CGXmlNode : NSObject 
{
	id	userInfo;
}

#if defined(TARGET_OS_IPHONE)
@property(assign) mUpnpXmlNode *cXmlNode;
#else
@property(retain) NSXMLElement *xmlNode;
#endif

@property(retain) id userInfo;
- (id)init;

#if  !defined(TARGET_OS_IPHONE)
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode;
#else
- (id)initWithXMLNode:(mUpnpXmlNode *)aXmlNode;
#endif

- (NSString *)attributeValueForName:(NSString *)aName;
- (CGXmlNode *)elementForName:(NSString *)aName;
- (NSString *)elementValueForName:(NSString *)aName;
- (NSString *)stringValue;
- (void)setStringValue:(NSString *)aValue;
- (void)setAttributeWithName:(NSString *)aName stringValue:(NSString *)aValue;
- (NSString *)xmlNodeToString;

@end

