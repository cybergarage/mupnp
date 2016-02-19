/******************************************************************
 *
 * mUPnP for ObjC
 *
 * Copyright (C) Satoshi Konno 2008
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#import <Foundation/Foundation.h>
#include <mupnp/controlpoint.h>
#include <mupnp/control/control.h>
#import "CGUpnpControlPoint.h"
#import "CGUpnpDevice.h"
#import "CGUpnpService.h"

static void CGUpnpControlPointDeviceListener(mUpnpControlPoint *ctrlPoint, const char* udn, mUpnpDeviceStatus status);
static void CGUpnpControlPointEventListener(void *aCtrlPoint, mUpnpProperty *property);


@interface CGUpnpControlPoint()
{
    NSArray<CGUpnpDevice *> *deviceArray;
}

@end


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
        mupnp_controlpoint_seteventlistener(cObject, CGUpnpControlPointEventListener);
		mupnp_controlpoint_setuserdata(cObject, (__bridge void *)self);
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
        CGUpnpDevice *device = [self deviceWithCObject:cDevice];
        if (nil == device) {
            device = [[CGUpnpDevice alloc] initWithCObject:cDevice];
        }
        [devArray addObject:device];
	}
    deviceArray = devArray;
    
	return deviceArray;
}

- (CGUpnpDevice *)deviceForUDN:(NSString *)udn
{
	if (!cObject)
		return nil;
	mUpnpDevice *cDevice;
	for (cDevice = mupnp_controlpoint_getdevices(cObject); cDevice; cDevice = mupnp_device_next(cDevice)) {
		if (mupnp_strcmp(mupnp_device_getudn(cDevice), (char *)[udn UTF8String]) == 0)
        {
            CGUpnpDevice *device = [self deviceWithCObject:cDevice];
            if (nil == device)
            {
                device = [[CGUpnpDevice alloc] initWithCObject:cDevice];
            }
            return device;
        }
	}
	return nil;
}

- (CGUpnpDevice *)deviceWithCObject:(mUpnpDevice *)cDevice
{
    __block CGUpnpDevice *device = nil;
    NSArray *tempArray = deviceArray;
    [tempArray enumerateObjectsUsingBlock:^(CGUpnpDevice * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        if (obj.cObject == cDevice) {
            device = obj;
            *stop = YES;
        }
    }];
    return device;
}

- (BOOL)subscribeEventNotificationFromService:(CGUpnpService *)service
{
    if (cObject && service.cObject)
    {
        return mupnp_controlpoint_subscribe(cObject, service.cObject, 200);
    }
    return NO;
}

- (BOOL)unsubscribeEventNotificationFromService:(CGUpnpService *)service
{
    if (cObject && service.cObject)
    {
        return mupnp_controlpoint_unsubscribe(cObject, service.cObject);
    }
    return NO;
}

@end

static void CGUpnpControlPointDeviceListener(mUpnpControlPoint *cCtrlPoint, const char* udn, mUpnpDeviceStatus status)
{
	CGUpnpControlPoint *ctrlPoint = (__bridge CGUpnpControlPoint *)mupnp_controlpoint_getuserdata(cCtrlPoint);
	if (ctrlPoint == nil)
		return;
	
	if ([ctrlPoint delegate] == nil)
		return;
    
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
}

static void CGUpnpControlPointEventListener(void *aCtrlPoint, mUpnpProperty *property)
{
    mUpnpControlPoint *cCtrlPoint = aCtrlPoint;
    CGUpnpControlPoint *ctrlPoint = (__bridge CGUpnpControlPoint *)mupnp_controlpoint_getuserdata(cCtrlPoint);
    if (ctrlPoint == nil)
        return;
    
    if ([ctrlPoint delegate] == nil)
        return;
    
    if ([[ctrlPoint delegate] respondsToSelector:@selector(controlPoint:eventNotified:)])
        [[ctrlPoint delegate] controlPoint:ctrlPoint eventNotified:property];
}
