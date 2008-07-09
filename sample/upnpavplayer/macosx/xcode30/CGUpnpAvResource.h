//
//  CGUpnpAvResource.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

/**
 * The CGUpnpAction class is a wrapper class for CgUpnpAction of CyberLink for C.
 */
@interface CGUpnpAvResource : NSObject 
{
	NSString *mimeType;
	NSString *url;
	long size;
	/* DLNA parameter */
	NSString *dlnaOrgPN;
	NSString *dlnaOrgOP;
}
@property(readwrite, copy) NSString *mimeType;
@property(readwrite, copy) NSString *url;
@property(readwrite) long size;
@property(readwrite, copy) NSString *dlnaOrgPN;
@property(readwrite, copy) NSString *dlnaOrgOP;
@end

