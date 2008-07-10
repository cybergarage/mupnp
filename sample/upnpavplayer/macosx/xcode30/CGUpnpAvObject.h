//
//  CGUpnpAvObject.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

/**
 * The CGUpnpAction class is a wrapper class for CgUpnpAction of CyberLink for C.
 */
@interface CGUpnpAvObject : NSObject 
{
	NSXMLNode *xmlNode;
	CGUpnpAvObject *parent;
}
@property(readwrite, copy) NSXMLNode *xmlNode;
@property(readwrite) CGUpnpAvObject *parent;
- (id)init;
- (id)initWithXMLNode:NSXMLNode *aXmlNode;
- (NSString *)attributeForName:(NSString *)name;
- (NSString *)elementsForName:(NSString *)name;
- (BOOL)isContainer;
- (BOOL)isItem;
- (CGUpnpAvObject *)ancestorObject;
- (NSString *)objectId;
- (NSString *)title;
- (NSString *)upnpClass;
- (BOOL)isObjectId;
- (BOOL)isTitle;
- (BOOL)isUpnpClass;
@end

