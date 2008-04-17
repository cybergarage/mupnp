//
//  CGUpnpDevice.m
//  clinkc
//
//  Created by Satoshi Konno on 08/03/14.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import "CGUpnpDevice.h"

@implementation CGUpnpDevice

- (id) init
{
	return self;
}

- (void) finalize
{
	[super finalize];
}

- (CgUpnpDevice *)object
{
	return _object;
}

- (void)setObject:(CgUpnpDevice *)object
{
	_object = object;
}

- (NSString *)friendlyName
{
	if (!_object)
		return nil;
	return [NSString stringWithCString:cg_upnp_device_getfriendlyname(_object)];
}

@end
