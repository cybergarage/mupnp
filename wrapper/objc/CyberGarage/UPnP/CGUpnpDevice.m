//
//  CGUpnpDevice.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/03/14.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <cybergarage/upnp/cdevice.h>
#include <cybergarage/upnp/cservice.h>

#import <CyberGarage/UPnP/CGUpnpDevice.h>
#import <CyberGarage/UPnP/CGUpnpService.h>

@implementation CGUpnpDevice

@synthesize cObject;

- (id) initWithCObject:(CgUpnpDevice *)cobj
{
	if ((self = [super init]) == nil)
		return nil;
	cObject = cobj;
	return self;
}

- (id) init
{
	[self initWithCObject:NULL];
	return self;
}

- (void) finalize
{
	[super finalize];
}

- (NSString *)friendlyName
{
	if (!cObject)
		return nil;
	return [[NSString alloc] initWithUTF8String:cg_upnp_device_getfriendlyname(cObject)];
}

- (NSString *)deviceType
{
	if (!cObject)
		return nil;
	return [[NSString alloc] initWithUTF8String:cg_upnp_device_getdevicetype(cObject)];
}

- (CGUpnpService *)getServiceByID:(NSString *)serviceId
{
	if (!cObject)
		return nil;
	CgUpnpService *foundService = cg_upnp_device_getservicebyserviceid(cObject, (char *)[serviceId UTF8String]);
	if (!foundService)
		return nil;
	return [[CGUpnpService alloc] initWithCObject:foundService];
}

- (CGUpnpService *)getServiceByType:(NSString *)serviceType;
{
	if (!cObject)
		return nil;
	CgUpnpService *foundService = cg_upnp_device_getservicebytype(cObject, (char *)[serviceType UTF8String]);
	if (!foundService)
		return nil;
	return [[CGUpnpService alloc] initWithCObject:foundService];
}

@end