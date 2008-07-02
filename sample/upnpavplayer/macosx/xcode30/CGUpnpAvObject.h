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
	NSString *objectId;
	NSString *title;
	NSString *upnpClass;
	CGUpnpAvObject *parentObject;
}
@property(readwrite, copy) NSString *objectId;
@property(readwrite, copy) NSString *title;
@property(readwrite, copy) NSString *upnpClass;
@property(readwrite, copy) CGUpnpAvObject *parentObject;
- (CGUpnpAvObject *)ancestorObject;
@end

