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

/**
 * The CGUpnpDevice class is a wrapper class for CgUpnpDevice of CyberLink for C.
 */
@interface CGUpnpDevice : NSObject 
{
	CgUpnpDevice *cObject;
}
@property(readonly) CgUpnpDevice *cObject;
- (id) initWithCObject:(CgUpnpDevice *)cobj;
/**
 * Get the friendly name of the device.
 * 
 * @return Friendly name.
 */
- (NSString *)friendlyName;
/**
 * Get the device type of the device.
 * 
 * @return Device Type.
 */
- (NSString *)deviceType;
/**
 * Check whether the device type is the specified type.
 *
 * @param type A type string of the device.
 * 
 * @return YES if the specified type is same as the device type; otherwise NO.
 */
- (BOOL)isDeviceType:(NSString *)type;
/**
 * Get all services in the device as a NSArray object. The array has the services as instances of CGUpnpService.
 *
 * @return NSArray of CGUpnpService.
 */
- (NSArray *)services;
/**
 * Get a service in the device by the specified service ID.
 *
 * @param serviceId A service ID string of the service.
 *
 * @return The CGUpnpService if the specified service is found; otherwise nil.
 */
- (CGUpnpService *)getServiceForID:(NSString *)serviceId;
/**
 * Get a service in the device by the specified service type.
 *
 * @param serviceType A service type string of the service.
 *
 * @return The CGUpnpService if the specified service is found; otherwise nil.
 */
- (CGUpnpService *)getServiceForType:(NSString *)serviceType;
@end
