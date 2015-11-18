//
//  CGUpnpAvServer.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/NSArray.h>
#import <Foundation/NSString.h>

#import <mUPnP/CGUPnPDevice.h>
#import <mUPnP/CGUpnpAVPositionInfo.h>

@class CGUpnpAVPositionInfo;

#if !defined(_CG_CLINKCAV_MEDIARENDERER_H_)
typedef void mUpnpAvRenderer;
#endif

/**
 * The CGUpnpAvRenderer class is a UPnP/AV media server class.
 */
@interface CGUpnpAvRenderer : CGUpnpDevice
{
}
@property(assign,readonly) mUpnpAvRenderer *cAvObject;
- (id)init;
- (id)initWithCObject:(mUpnpDevice *)cobj;
- (BOOL)setAVTransportURI:(NSString *)aURI;
- (BOOL)play;
- (BOOL)stop;
- (BOOL)pause;
- (BOOL)seek:(float)absTime;
- (BOOL)isPlaying;
/*
- (BOOL)start
- (BOOL)stop
*/
- (CGUpnpAVPositionInfo *)positionInfo;
@end
