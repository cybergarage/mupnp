//
//  CGUpnpDevice.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/03/14.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <CyberGarage/UPnP/CGUpnpDevice.h>

@implementation CGUpnpDevice

- (id) init
{
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
	return [NSString stringWithCString:cg_upnp_device_getfriendlyname(_cObject)];
}

@end
