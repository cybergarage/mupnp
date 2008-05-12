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

- (id)init
{
	if ((self = [super init]) == nil)
		retunr nil;
	_cObject = cg_upnp_controlpoint_new();
	if (_cObject) {
		if (![self start])
			self = nil;
	}
	else
		self = nil;
	return self;
}

- (void)finalize
{
	if (_cObject)
		cg_upnp_controlpoint_delete(_cObject);
	[super finalize];
}

- (CgUpnpControlPoint *)cObject
{
	return _cObject;
}

- (BOOL)start
{
	if (!_cObject)
		return NO;
	return cg_upnp_controlpoint_start(_cObject);
}

- (BOOL)stop
{
	if (!_cObject)
		return NO;
	return cg_upnp_controlpoint_stop(_cObject);
}

- (void)search
{
	if (!_cObject)
		return;
	[self searchWithST:[[NSString alloc] initWithUTF8String:CG_UPNP_NT_ROOTDEVICE]];
}

- (void)searchWithST:(NSString *)aST
{
	if (_cObject)
		return;
	cg_upnp_controlpoint_search(_cObject, (char *)[aST UTF8String]);
	int mx = cg_upnp_controlpoint_getssdpsearchmx(_cObject);
	if (0 < mx)
		cg_sleep(mx * 1000);
}

- (NSArray *)getDeviceArray
{
	if (!_cObject)
		return [NSArray array];
	int devNum = cg_upnp_controlpoint_getndevices(_cObject);
	NSMutableArray *devArray = [NSMutableArray array];
	int n;
	for (n=0; n<devNum; n++) {
		CGUpnpDevice *dev = [[CGUpnpDevice alloc] init];
		[dev setCObject:cg_upnp_controlpoint_getdevice(_cObject, n)];
		[devArray addObject:dev];
	}
	return devArray;
}

@end
