/******************************************************************
 *
 * mUPnP for ObjC
 *
 * Copyright (C) Satoshi Konno 2008
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#include <mupnp/controlpoint.h>
#include <mupnp/control/control.h>
#import "CGUpnpControlPoint.h"
#import "CGUpnpDevice.h"

static void CGUpnpControlPointDeviceListener(mUpnpControlPoint *ctrlPoint, const char* udn, mUpnpDeviceStatus status);

@implementation CGUpnpControlPoint

@synthesize cObject;
@synthesize delegate;

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	cObject = mupnp_controlpoint_new();
	if (cObject) {
		mupnp_controlpoint_setdevicelistener(cObject, CGUpnpControlPointDeviceListener);
		mupnp_controlpoint_setuserdata(cObject, self);
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
		mupnp_controlpoint_delete(cObject);
	[super dealloc];
}

- (BOOL)start
{
	if (!cObject)
		return NO;
	return mupnp_controlpoint_start(cObject);
}

- (BOOL)stop
{
	if (!cObject)
		return NO;
	return mupnp_controlpoint_stop(cObject);
}

-(BOOL)isRunning
{
	if (!cObject)
		return NO;
	return mupnp_controlpoint_isrunning(cObject);
}

- (void)search
{
	[self searchWithST:[NSString stringWithUTF8String:MUPNP_NT_ROOTDEVICE]];
}

- (void)searchWithST:(NSString *)aST
{
	if (!cObject)
		return;
	mupnp_controlpoint_search(cObject, (char *)[aST UTF8String]);
	
#if defined(CG_UPNPCONTROLPOINT_ENABLE_SEARCH_SLEEP)
	int mx = mupnp_controlpoint_getssdpsearchmx(cObject);
	if (0 < mx)
		mupnp_sleep((mx * 1000));
#endif
}

- (NSInteger)ssdpSearchMX
{
	if (!cObject)
		return 0;
	return mupnp_controlpoint_getssdpsearchmx(cObject);
}

- (void)setSsdpSearchMX:(NSInteger)mx;
{
	if (!cObject)
		return;
	mupnp_controlpoint_setssdpsearchmx(cObject, (int)mx);
}

- (NSArray *)devices
{
	if (!cObject)
		return [NSArray array];
	NSMutableArray *devArray = [NSMutableArray array];
	mUpnpDevice *cDevice;
	for (cDevice = mupnp_controlpoint_getdevices(cObject); cDevice; cDevice = mupnp_device_next(cDevice)) {
		CGUpnpDevice *device = [[[CGUpnpDevice alloc] initWithCObject:cDevice] autorelease];
		[devArray addObject:device];
	}
	return devArray;
}

- (CGUpnpDevice *)deviceForUDN:(NSString *)udn
{
	if (!cObject)
		return nil;
	mUpnpDevice *cDevice;
	for (cDevice = mupnp_controlpoint_getdevices(cObject); cDevice; cDevice = mupnp_device_next(cDevice)) {
		if (mupnp_strcmp(mupnp_device_getudn(cDevice), (char *)[udn UTF8String]) == 0) 
			return [[[CGUpnpDevice alloc] initWithCObject:cDevice] autorelease];
	}
	return nil;
}

@end

static void CGUpnpControlPointDeviceListener(mUpnpControlPoint *cCtrlPoint, const char* udn, mUpnpDeviceStatus status)
{
	CGUpnpControlPoint *ctrlPoint = mupnp_controlpoint_getuserdata(cCtrlPoint);
	if (ctrlPoint == nil)
		return;
	
	if ([ctrlPoint delegate] == nil)
		return;

    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
	NSString *deviceUdn = [[NSString alloc] initWithUTF8String:udn];
	
	switch (status) {
		case mUpnpDeviceStatusAdded:
			{
				if ([[ctrlPoint delegate] respondsToSelector:@selector(controlPoint:deviceAdded:)])
					[[ctrlPoint delegate] controlPoint:ctrlPoint deviceAdded:deviceUdn];
			}
			break;
		case mUpnpDeviceStatusUpdated:
			{
				if ([[ctrlPoint delegate] respondsToSelector:@selector(controlPoint:deviceUpdated:)])
					[[ctrlPoint delegate] controlPoint:ctrlPoint deviceUpdated:deviceUdn];
			}
			break;
		case mUpnpDeviceStatusRemoved:
			{
				if ([[ctrlPoint delegate] respondsToSelector:@selector(controlPoint:deviceRemoved:)])
					[[ctrlPoint delegate] controlPoint:ctrlPoint deviceRemoved:deviceUdn];
			}
			break;
		case mUpnpDeviceStatusInvalid:
			{
				if ([[ctrlPoint delegate] respondsToSelector:@selector(controlPoint:deviceInvalid:)])
					[[ctrlPoint delegate] controlPoint:ctrlPoint deviceInvalid:deviceUdn];
			}
			break;
		default:
			break;
	}
	
	[deviceUdn release];
    
    [pool drain];
}

