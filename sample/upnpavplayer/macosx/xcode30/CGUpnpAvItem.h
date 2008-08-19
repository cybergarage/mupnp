//
//  CGUpnpAvItem.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#if defined(__MACOSX__)
#import <Cocoa/Cocoa.h>
#else
#import <UIKit/UIKit.h>
#endif

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
