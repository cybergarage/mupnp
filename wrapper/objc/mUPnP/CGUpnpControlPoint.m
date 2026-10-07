/******************************************************************
 *
 * mUPnP for ObjC
 *
 * Copyright (C) Satoshi Konno 2008
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#include <mupnp/control/control.h>
#include <mupnp/controlpoint.h>

#import "CGUpnpControlPoint.h"
#import "CGUpnpDevice.h"

static void cg_upnp_control_point_device_listener(mUpnpControlPoint* ctrlPoint, const char* udn, mUpnpDeviceStatus status);

#if defined(MUPNP_OBJC_TEST_HOOKS)
/* Test-only injection points for allocation-failure and ownership tests.
 * They are compiled only when MUPNP_OBJC_TEST_HOOKS is defined. */
mUpnpControlPoint* (*cg_upnp_test_controlpoint_new)(void) = NULL;
void (*cg_upnp_test_controlpoint_delete)(mUpnpControlPoint*) = NULL;
#define CG_UPNP_CONTROLPOINT_NEW() (cg_upnp_test_controlpoint_new ? cg_upnp_test_controlpoint_new() : mupnp_controlpoint_new())
#define CG_UPNP_CONTROLPOINT_DELETE(cp) (cg_upnp_test_controlpoint_delete ? cg_upnp_test_controlpoint_delete(cp) : mupnp_controlpoint_delete(cp))
#else
#define CG_UPNP_CONTROLPOINT_NEW() mupnp_controlpoint_new()
#define CG_UPNP_CONTROLPOINT_DELETE(cp) mupnp_controlpoint_delete(cp)
#endif

@implementation CGUpnpControlPoint

@synthesize cObject;
@synthesize delegate;

- (id)init
{
  if ((self = [super init]) == nil)
    return nil;
  cObject = CG_UPNP_CONTROLPOINT_NEW();
  if (!cObject) {
    [self release];
    return nil;
  }
  mupnp_controlpoint_setdevicelistener(cObject, cg_upnp_control_point_device_listener);
  mupnp_controlpoint_setuserdata(cObject, self);
  if (![self start]) {
    /* -dealloc stops and deletes the native control point exactly once. */
    [self release];
    return nil;
  }
  return self;
}

- (void)dealloc
{
  if (cObject) {
    CG_UPNP_CONTROLPOINT_DELETE(cObject);
    cObject = NULL;
  }
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

- (BOOL)isRunning
{
  if (!cObject)
    return NO;
  return mupnp_controlpoint_isrunning(cObject);
}

- (void)search
{
  [self searchWithST:[NSString stringWithUTF8String:MUPNP_NT_ROOTDEVICE]];
}

- (void)searchWithST:(NSString*)aST
{
  if (!cObject)
    return;
  mupnp_controlpoint_search(cObject, (char*)[aST UTF8String]);

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

- (NSArray*)devices
{
  if (!cObject)
    return [NSArray array];
  NSMutableArray* devArray = [NSMutableArray array];
  mupnp_controlpoint_lock(cObject);
  mUpnpDevice* cDevice;
  for (cDevice = mupnp_controlpoint_getdevices(cObject); cDevice; cDevice = mupnp_device_next(cDevice)) {
    CGUpnpDevice* device = [[[CGUpnpDevice alloc] initWithDeviceSnapshot:cDevice] autorelease];
    if (device)
      [devArray addObject:device];
  }
  mupnp_controlpoint_unlock(cObject);
  return devArray;
}

- (CGUpnpDevice*)deviceForUDN:(NSString*)udn
{
  if (!cObject)
    return nil;
  mupnp_controlpoint_lock(cObject);
  mUpnpDevice* cDevice;
  for (cDevice = mupnp_controlpoint_getdevices(cObject); cDevice; cDevice = mupnp_device_next(cDevice)) {
    if (mupnp_strcmp(mupnp_device_getudn(cDevice), (char*)[udn UTF8String]) == 0) {
      CGUpnpDevice* device = [[[CGUpnpDevice alloc] initWithDeviceSnapshot:cDevice] autorelease];
      mupnp_controlpoint_unlock(cObject);
      return device;
    }
  }
  mupnp_controlpoint_unlock(cObject);
  return nil;
}

@end

static void cg_upnp_control_point_device_listener(mUpnpControlPoint* cCtrlPoint, const char* udn, mUpnpDeviceStatus status)
{
  CGUpnpControlPoint* ctrlPoint = mupnp_controlpoint_getuserdata(cCtrlPoint);
  if (ctrlPoint == nil)
    return;

  if ([ctrlPoint delegate] == nil)
    return;

  @autoreleasepool {

    NSString* deviceUdn = [[NSString alloc] initWithUTF8String:udn];

    switch (status) {
    case mUpnpDeviceStatusAdded: {
      if ([[ctrlPoint delegate] respondsToSelector:@selector(controlPoint:deviceAdded:)])
        [[ctrlPoint delegate] controlPoint:ctrlPoint deviceAdded:deviceUdn];
    } break;
    case mUpnpDeviceStatusUpdated: {
      if ([[ctrlPoint delegate] respondsToSelector:@selector(controlPoint:deviceUpdated:)])
        [[ctrlPoint delegate] controlPoint:ctrlPoint deviceUpdated:deviceUdn];
    } break;
    case mUpnpDeviceStatusRemoved: {
      if ([[ctrlPoint delegate] respondsToSelector:@selector(controlPoint:deviceRemoved:)])
        [[ctrlPoint delegate] controlPoint:ctrlPoint deviceRemoved:deviceUdn];
    } break;
    case mUpnpDeviceStatusInvalid: {
      if ([[ctrlPoint delegate] respondsToSelector:@selector(controlPoint:deviceInvalid:)])
        [[ctrlPoint delegate] controlPoint:ctrlPoint deviceInvalid:deviceUdn];
    } break;
    default:
      break;
    }

    [deviceUdn release];
  }
}
