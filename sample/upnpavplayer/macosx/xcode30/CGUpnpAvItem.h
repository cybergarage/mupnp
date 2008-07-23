//
//  CGUpnpAvItem.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class CGUpnpAvObject;
@class CGUpnpAvResource;

/**
 * The CGUpnpAction class is a wrapper class for CgUpnpAction of CyberLink for C.
 */
@interface CGUpnpAvItem : CGUpnpAvObject 
{
	NSMutableArray *resourceArray;
}
- (id)init;
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode;
- (void)addResource:(CGUpnpAvResource *)res;
- (void)removeResource:(CGUpnpAvResource *)res;
- (NSArray *)resources;
@end
