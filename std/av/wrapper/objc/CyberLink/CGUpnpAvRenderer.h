//
//  CGUpnpAvServer.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/NSArray.h>
#import <Foundation/NSString.h>

#import <CyberLink/UPnP.h>

@class CGUpnpAvContentDirectory;
@class CGUpnpAvObject;
@class CGUpnpAvContainer;

/**
 * The CGUpnpAvRenderer class is a UPnP/AV media server class.
 */
@interface CGUpnpAvRenderer : CGUpnpDevice
{
}
- (id) initWithCObject:(CgUpnpDevice *)cobj;
- (BOOL)setAVTransportURI:(NSString *)aURI;
- (BOOL)play;
- (BOOL)stop;
@end
