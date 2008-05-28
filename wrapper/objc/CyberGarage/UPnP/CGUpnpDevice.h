//
//  CGUpnpDevice.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/03/14.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#if !defined(_CG_UPNP_CDEVICE_H_)
typedef void CgUpnpDevice;
#endif

@class CGUpnpService;

@interface CGUpnpDevice : NSObject 
{
	CgUpnpDevice *cObject;
}
@property(readonly) CgUpnpDevice *cObject;
- (id) initWithCObject:(CgUpnpDevice *)cobj;
- (NSString *)friendlyName;
- (NSString *)deviceType;
- (BOOL)isDeviceType:(NSString *)type;
- (NSArray *)services;
- (CGUpnpService *)getServiceByID:(NSString *)serviceId;
- (CGUpnpService *)getServiceByType:(NSString *)serviceType;
@end
