//
//  CGUpnpAvObject.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class CGXmlNode;

/**
 * The CGUpnpAction class is a wrapper class for CgUpnpAction of CyberLink for C.
 */
@interface CGUpnpAvObject : CGXmlNode 
{
	CGUpnpAvObject *parent;
}
- (id)init;
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode;
- (BOOL)isContainer;
- (BOOL)isItem;
- (void)setParent:(CGUpnpAvObject *)aParent;
- (CGUpnpAvObject *)parent;
- (CGUpnpAvObject *)ancestor;
- (NSString *)objectId;
- (NSString *)title;
- (NSString *)upnpClass;
- (void)setObjectId:(NSString *)aValue;
- (void)setTitle:(NSString *)aValue;
- (void)setUpnpClass:(NSString *)aValue;
- (BOOL)isObjectId:(NSString *)aObjectId;
- (BOOL)isTitle:(NSString *)aTitle;
- (BOOL)isUpnpClass:(NSString *)aUpnpClass;
@end

