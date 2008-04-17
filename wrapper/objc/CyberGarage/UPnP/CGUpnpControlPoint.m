//
//  CGUpnpControlPoint.m
//  clinkc
//
//  Created by Satoshi Konno on 08/03/14.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import "CGUpnpControlPoint.h"

@implementation CGUpnpControlPoint

- (id) init
{
	_object = cg_upnp_controlpoint_new();
	if (_object)
		self = nil;
	return self;
}

- (void) finalize
{
	if (_object)
		cg_upnp_controlpoint_delete(_object);
	[super finalize];
}

- (CgUpnpControlPoint *)object
{
	return _object;
}

- (NSArray *)getDeviceArray
{
	if (!_object)
		return [NSArray array];
	int devNum = cg_upnp_controlpoint_getndevices(_object);
	CgUpnpDevice **devCArray = (CgUpnpDevice **)malloc(sizeof(CgUpnpDevice*) *devNum);
	int n;
	for (n=0; n<devNum; n++)
		devCArray[n] = cg_upnp_controlpoint_getdevice(n);
	
	NSArray *devArray = [NSArray arrayWithObjects:devCArray count:devNum];
	free(devCArray);
	return devArray;
}

@end
