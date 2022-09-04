//
//  CGUpnpAvContentManager.h
//  mUPnP for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/NSString.h>

@class CGUpnpAvRoot;
@class CGUpnpAvObject;
@class CGUpnpAvContainer;

/**
 * The CGUpnpAvContentDirectory class has UPnP/AV objects of a media servers.
 *
 */
@interface CGUpnpAvContentManager : NSObject {
}
@property (retain) CGUpnpAvRoot* rootObj;
- (CGUpnpAvObject*)objectForId:(NSString*)aObjectId;
- (CGUpnpAvObject*)objectForTitlePath:(NSString*)aTitlePath;
@end
