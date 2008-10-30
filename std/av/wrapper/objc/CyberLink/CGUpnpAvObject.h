//
//  CGUpnpAvObject.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <CyberLink/CGXmlNode.h>

#if  !defined(_CG_CLINKC_MEDIA_CONTENT_H_)
typedef void CgUpnpMediaContent;
#endif

/**
 * The CGUpnpAvObject class is a wrapper class for CgUpnpAvObject of CyberLink for C.
 */
@interface CGUpnpAvObject : CGXmlNode 
{
	CGUpnpAvObject *parent;
}
+ (NSArray *)arrayWithXMLString:(NSString *)aXmlString;
- (id)init;
#if  !defined(TARGET_OS_IPHONE)
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode;
#else
- (id)initWithXMLNode:(CgXmlNode *)aXmlNode;
- (id)initWithMediaContent:(CgUpnpMediaContent *)aMediaContent;
#endif
- (BOOL)isEqual:(id)anObject;
- (BOOL)isContainer;
- (BOOL)isItem;
- (void)setParent:(CGUpnpAvObject *)aParent;
- (CGUpnpAvObject *)parent;
- (CGUpnpAvObject *)ancestor;
- (NSString *)objectId;
- (NSString *)title;
- (NSString *)upnpClass;
- (NSString *)date;
- (NSString *)albumArtURI;
- (void)setObjectId:(NSString *)aValue;
- (void)setTitle:(NSString *)aValue;
- (void)setUpnpClass:(NSString *)aValue;
- (BOOL)isObjectId:(NSString *)aObjectId;
- (BOOL)isTitle:(NSString *)aTitle;
- (BOOL)isUpnpClass:(NSString *)aUpnpClass;
@end

