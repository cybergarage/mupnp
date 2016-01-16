/******************************************************************
 *
 * mUPnP for ObjC
 *
 * Copyright (C) Satoshi Konno 2008
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#import <Foundation/NSString.h>
#import <Foundation/NSArray.h>

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
 * program using only Objective-C directly on MacOSX.
 * Currently, the only basic methods are wrapped to control UPnP devices.
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
 * Send a M-SEARCH request to find for the all UPnP devices of UPnP in the network.
 * The method waits for the specified MX time. Use setSsdpSearchMX:(NSInteger)aMX to change the time.
 * The method is same as searchWithST() when the ST parameter is "ssdp:all".
 */
- (void)search;
/**
 * Send a M-SEARCH request to find for the specified UPnP devices by a ST parameter in the network.
 * The method waits for the specified MX time. Use setSsdpSearchMX:(NSInteger)aMX to change the time.
 *
 * @param aST The Search Target parameter (ex. "ssdp:all")
 */
- (void)searchWithST:(NSString*)aST;
/**
 * Set a MX parameter used for SSDP searches in seconds.
 * The parameter is used to wait for M-SEARCH responses in search() and searchWithST:(NSString *)aST.
 *
 * @param aMX Time to wait
 */
- (void)setSsdpSearchMX:(NSInteger)aMX;
/**
 * Get a MX parameter used for SSDP searches in seconds.
 * The parameter is used to wait for M-SEARCH responses in search() and searchWithST:(NSString *)aST.
 *
 * @return Time to wait
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
