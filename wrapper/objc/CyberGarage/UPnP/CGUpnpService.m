//
//  CGUpnpService.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <cybergarage/upnp/cservice.h>

#import <CyberGarage/UPnP/CGUpnpService.h>s

@implementation CGUpnpService

- (id) initWithCObject(CGUpnpService *cobj)
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

- (NSString *)serviceType
{
	if (!_cObject)
		return nil;
	return [[NSString alloc] initWithUTF8String:cg_upnp_service_getservicetype(_cObject)];
}

@end
