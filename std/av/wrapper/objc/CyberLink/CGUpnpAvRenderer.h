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

#if !defined(_CG_CLINKC_AV_MEDIARENDERER_H_)
typedef void CgUpnpAvRenderer;
#endif

/**
 * The CGUpnpAvRenderer class is a UPnP/AV media server class.
 */
@interface CGUpnpAvRenderer : CGUpnpDevice
{
	CgUpnpAvRenderer *cAvObject;
}
@property(readonly) CgUpnpAvRenderer *cAvObject;
- (id)init;
- (id)initWithCObject:(CgUpnpDevice *)cobj;
- (BOOL)setAVTransportURI:(NSString *)aURI;
- (BOOL)play;
- (BOOL)stop;
/*
- (BOOL)start
- (BOOL)stop
*/
@end
