//
//  CGUpnpControlPoint.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/03/14.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <cybergarage/upnp/ccontrolpoint.h>
#include <cybergarage/upnp/control/ccontrol.h>
#import "CGUpnpControlPoint.h"
#import "CGUpnpDevice.h"

@implementation CGUpnpControlPoint

@synthesize cObject;

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	cObject = cg_upnp_controlpoint_new();
	if (cObject) {
		if (![self start])
			self = nil;
	}
	else
		self = nil;
	return self;
}

- (void)dealloc
{
	if (cObject)
		cg_upnp_controlpoint_delete(cObject);
	[super dealloc];
}

- (void)finalize
{
	if (cObject)
		cg_upnp_controlpoint_delete(cObject);
	[super finalize];
}

- (BOOL)start
{
	if (!cObject)
		return NO;
	return cg_upnp_controlpoint_start(cObject);
}

- (BOOL)stop
{
	if (!cObject)
		return NO;
	return cg_upnp_controlpoint_stop(cObject);
}

- (void)search
{
	[self searchWithST:[[NSString alloc] initWithUTF8String:CG_UPNP_NT_ROOTDEVICE]];
}

- (void)searchWithST:(NSString *)aST
{
	if (!cObject)
		return;
	cg_upnp_controlpoint_search(cObject, (char *)[aST UTF8String]);
	int mx = cg_upnp_controlpoint_getssdpsearchmx(cObject);
	if (0 < mx)
		cg_sleep((mx * 1000));
}

- (NSInteger)ssdpSearchMX
{
	if (!cObject)
		return 0;
	return cg_upnp_controlpoint_getssdpsearchmx(cObject);
}

- (void)setSsdpSearchMX:(NSInteger)mx;
{
	if (!cObject)
		return;
	cg_upnp_controlpoint_setssdpsearchmx(cObject, (int)mx);
}

- (NSArray *)devices
{
	if (!cObject)
		return [[[NSArray alloc] init] autorelease];
	NSMutableArray *devArray = [[[NSMutableArray alloc] init] autorelease];
	CgUpnpDevice *cDevice;
	for (cDevice = cg_upnp_controlpoint_getdevices(cObject); cDevice; cDevice = cg_upnp_device_next(cDevice)) {
		CGUpnpDevice *device = [[[CGUpnpDevice alloc] initWithCObject:cDevice] autorelease];
		[devArray addObject:device];
	}
	return devArray;
}

- (CGUpnpDevice *)deviceForUDN:(NSString *)udn
{
	if (!cObject)
		return nil;
	CgUpnpDevice *cDevice;
	for (cDevice = cg_upnp_controlpoint_getdevices(cObject); cDevice; cDevice = cg_upnp_device_next(cDevice)) {
		if (cg_strcmp(cg_upnp_device_getudn(cDevice), (char *)[udn UTF8String]) == 0) 
			return [[[CGUpnpDevice alloc] initWithCObject:cDevice] autorelease];
	}
	return nil;
}

@end
