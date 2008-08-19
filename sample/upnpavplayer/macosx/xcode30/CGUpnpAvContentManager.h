//
//  CGUpnpAvContentManager.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#if defined(__MACOSX__)
#import <Cocoa/Cocoa.h>
#else
#import <UIKit/UIKit.h>
#endif

@class CGUpnpAvRoot;
@class CGUpnpAvObject;

/**
 * The CGUpnpAvContentDirectory class has UPnP/AV objects of a media servers.
 *
 */
@interface CGUpnpAvContentManager : NSObject
{
	CGUpnpAvRoot *rootObj;
}
- (CGUpnpAvObject *)objectForId:(NSString *)aObjectId;
- (CGUpnpAvObject *)objectForTitlePath:(NSString *)aTitlePath;
@end

