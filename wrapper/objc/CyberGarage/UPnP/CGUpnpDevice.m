//
//  CGUpnpDevice.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/03/14.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <cybergarage/upnp/cdevice.h>

#import <CyberGarage/UPnP/CGUpnpDevice.h>

@implementation CGUpnpDevice

- (id) initWithCObject(CgUpnpDevice *cobj)
{
	if ((self = [super init]) == nil)
		return nil;
	_cObject = cobj;
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
	if (!_cObject)
		return nil;
	return [[NSString alloc] initWithUTF8String:cg_upnp_device_getfriendlyname(_cObject)];
}

- (NSString *)deviceType
{
	if (!_cObject)
		return nil;
	return [[NSString alloc] initWithUTF8String:cg_upnp_device_getdevicetype(_cObject)];
}

- CGUpnpService *getServiceByID:(NSString *)type
{
	if (!_cObject)
		return nil;
	CgUpnpService *foundService = cg_upnp_device_getservicebyserviceid(_cObject, [type UTF8String]);
	if (!foundService)
		return nil;
	return [[CGUpnpService alloc] initWithCObject:foundService];
}

- CGUpnpService *getServiceByType:(NSString *)type;
{
	if (!_cObject)
		return nil;
	CgUpnpService *foundService = cg_upnp_device_getservicebytype(_cObject, [type UTF8String]);
	if (!foundService)
		return nil;
	return [[CGUpnpService alloc] initWithCObject:foundService];
}

@end
