//
//  CGUpnpAvContentDirectory.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class CGUpnpAvContentManager;

/**
 * The CGUpnpAvContentDirectory class has UPnP/AV objects of a media servers.
 *
 */
@interface CGUpnpAvContentDirectory : NSObject
{
	CGUpnpAvContentManager *contentMgr;
}
- (CGUpnpAvObject *)objectForId:(NSString *)aObjectId;
- (CGUpnpAvObject *)objectForTitlePath:(NSString *)aTitlePath;
@end

