//
//  CGUpnpAvContentManager.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import <CGUpnpAvContainer.h>
#import <CGUpnpAvRoot.h>

/**
 * The CGUpnpAvContentDirectory class has UPnP/AV objects of a media servers.
 *
 */
@interface CGUpnpAvContentManager : NSObject
{
	CGUpnpAvRoot *rootObj;
}
- (CGUpnpAvObject *)objectForTitlePath:(NSString *)titlePath;
@end

