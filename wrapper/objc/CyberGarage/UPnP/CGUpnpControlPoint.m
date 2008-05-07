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
	_cObject = cg_upnp_controlpoint_new();
	if (!_cObject)
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

- (void)search
{
	if (!_cObject)
		return;
	cg_upnp_controlpoint_search(_cObject, CG_UPNP_NT_ROOTDEVICE);
}

- (void)searchWithST:(NSString *)aST
{
	if (_cObject)
		return;
	cg_upnp_controlpoint_search(_cObject, (char *)[aST UTF8String]);
}

- (NSArray *)getDeviceArray
{
	if (!_cObject)
		return [NSArray array];
	int devNum = cg_upnp_controlpoint_getndevices(_cObject);
	NSMutableArray *devArray = [NSMutableArray array];
	int n;
	for (n=0; n<devNum; n++) {
		CGUpnpDevice *dev = [CGUpnpDevice init];
		[dev setCObject:cg_upnp_controlpoint_getdevice(_cObject, n)];
		[devArray addObject:dev];
	}
	return devArray;
}

@end
