//
//  CGUpnpAvContentManager.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import <CGUpnpAvContainer.h>
#import <CGUpnpAvItem.h>

/**
 * The CGUpnpAvContentDirectory class has UPnP/AV objects of a media servers.
 *
 */
@interface CGUpnpAvContentManager : NSObject
{
	CGUpnpAvContainer *rootContainer;
}
- (CGUpnpAvObject *)objectForTitlePath:(NSString *)titlePath;
@end

