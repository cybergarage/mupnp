//
//  CGUpnpControlPoint.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/03/14.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <cybergarage/upnp/ccontrolpoint.h>
#include <cybergarage/upnp/control/ccontrol.h>

#import <CyberGarage/UPnP/CGUpnpControlPoint.h>
#import <CyberGarage/UPnP/CGUpnpDevice.h>

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
	if (cObject)
		return;
	cg_upnp_controlpoint_search(cObject, (char *)[aST UTF8String]);
	int mx = cg_upnp_controlpoint_getssdpsearchmx(cObject);
	if (0 < mx)
		cg_sleep(mx * 1000);
}

- (NSArray *)getDeviceArray
{
	if (!cObject)
		return [NSArray array];
	int devNum = cg_upnp_controlpoint_getndevices(cObject);
	NSMutableArray *devArray = [NSMutableArray array];
	int n;
	for (n=0; n<devNum; n++) {
		CGUpnpDevice *dev = [[CGUpnpDevice alloc] initWithCObject:cg_upnp_controlpoint_getdevice(cObject, n)];
		[devArray addObject:dev];
	}
	return devArray;
	
}

@end
