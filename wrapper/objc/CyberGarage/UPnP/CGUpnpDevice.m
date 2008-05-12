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

- (id) init
{
	if ((self = [super init]) == nil)
		return nil;
	return self;
}

- (void) finalize
{
	[super finalize];
}

- (CgUpnpDevice *)cObject
{
	return _cObject;
}

- (void)setCObject:(CgUpnpDevice *)object
{
	_cObject = object;
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

@end
