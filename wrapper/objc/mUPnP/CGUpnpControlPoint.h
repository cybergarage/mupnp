/******************************************************************
 *
 * mUPnP for ObjC
 *
 * Copyright (C) Satoshi Konno 2008
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#import <Foundation/NSArray.h>
#import <Foundation/NSString.h>

#if !defined(_MUPNP_CONTROLPOINT_H_)
typedef void mUpnpControlPoint;
#endif

@class CGUpnpDevice;
@class CGUpnpControlPoint;

@protocol CGUpnpControlPointDelegate <NSObject>
@optional
- (void)controlPoint:(CGUpnpControlPoint*)controlPoint deviceAdded:(NSString*)deviceUdn;
- (void)controlPoint:(CGUpnpControlPoint*)controlPoint deviceUpdated:(NSString*)deviceUdn;
- (void)controlPoint:(CGUpnpControlPoint*)controlPoint deviceRemoved:(NSString*)deviceUdn;
- (void)controlPoint:(CGUpnpControlPoint*)controlPoint deviceInvalid:(NSString*)deviceUdn;
@end

/**
 * The CGUpnpControlPoint class is a wrapper class for mUpnpControlPoint of mUPnP for C to
 * program using only Objective-C directly on macOS and iOS.
 * Currently, the only basic methods are wrapped to control UPnP devices.
 *
 * -init creates and starts the native control point. It returns nil, without
 * leaking the native object, when the native control point cannot be created
 * or started.
 */
@interface CGUpnpControlPoint : NSObject {
}
@property (assign, readonly) mUpnpControlPoint* cObject;
@property (assign) id<CGUpnpControlPointDelegate> delegate;
- (mUpnpControlPoint*)cObject;
/**
 * Activate some background threads of the control point such as SSDP and
 * HTTP servers to listen messages and events of UPnP. You must call this
 * method before you can actually use a control point.
 *
 * @return TRUE if successful; otherwise FALSE
 *
 */
- (BOOL)start;
/**
 * Stop some background threads of the control point such as SSDP and
 * HTTP servers to listen messages and events of UPnP.
 *
 * @return TRUE if successful; otherwise FALSE
 *
 */
- (BOOL)stop;
/**
 * Check if the controll point is running.
 *
 * @return YES if the device is started normally, otherwise NO.
 */
- (BOOL)isRunning;
/**
 * Send an M-SEARCH request for root devices in the network.
 * The method is the same as searchWithST: with the ST parameter "upnp:rootdevice"
 * (MUPNP_NT_ROOTDEVICE); it does not search for "ssdp:all".
 * See searchWithST: for when the method returns and how results are delivered.
 */
- (void)search;
/**
 * Send an M-SEARCH request for the specified search target (ST) in the network.
 *
 * By default the method returns once the native layer has sent the request
 * (it repeats the send mupnp_ssdp_getannouncecount() times with a short delay
 * in between); it does not wait for the MX period and does not report whether
 * sending succeeded. Responses arrive asynchronously: found devices are reported
 * through the delegate and are returned by devices and deviceForUDN: afterwards.
 * Only when the wrapper is compiled with CG_UPNPCONTROLPOINT_ENABLE_SEARCH_SLEEP
 * does the method additionally sleep for ssdpSearchMX seconds before returning.
 *
 * @param aST The Search Target parameter (ex. "upnp:rootdevice" or "ssdp:all")
 */
- (void)searchWithST:(NSString*)aST;
/**
 * Set the MX parameter, in seconds, sent in SSDP M-SEARCH requests.
 * MX is the maximum time devices may wait before responding. The search methods
 * wait for this time only when compiled with CG_UPNPCONTROLPOINT_ENABLE_SEARCH_SLEEP.
 *
 * @param aMX Maximum response delay in seconds
 */
- (void)setSsdpSearchMX:(NSInteger)aMX;
/**
 * Get the MX parameter, in seconds, sent in SSDP M-SEARCH requests.
 *
 * @return Maximum response delay in seconds
 */
- (NSInteger)ssdpSearchMX;
/**
 * Get all UPnP devices which the control point found as a NSArray object. The array has the devices as instances of CGUpnpDevice.
 * @return NSArray of CGUpnpDevice.
 */
- (NSArray*)devices;
/**
 * Get a specified UPnP devices by the UDN.
 * @return CGUpnpDevice when the specified device is found, otherwise nil.
 */
- (CGUpnpDevice*)deviceForUDN:(NSString*)udn;
@end
