//
//  CGUpnpControlPoint.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/03/14.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#if !defined(_CG_UPNP_CCONTROLPOINT_H_)
typedef void CgUpnpControlPoint;
#endif

@interface CGUpnpControlPoint : NSObject 
{
	CgUpnpControlPoint *cObject;
}
@property(readonly) CgUpnpControlPoint *cObject;
- (CgUpnpControlPoint *)cObject;
/**
 * Activate the control point. Starts listening for SSDP messages etc.
 * You must call this function before you can actually use a control point.
 *
 * @return TRUE if successful; otherwise FALSE
 *
 */
- (BOOL)start;
- (BOOL)stop;
/**
 * Send a M-SEARCH request to find for the all UPnP devices of UPnP in the network.
 * The method waits for the specified MX time. Use setSsdpSearchMX() to change the time.
 * The method is same as searchWithST() when the ST parameter is "ssdp:all".
 */
- (void)search;
/**
 * Send a M-SEARCH request to find for the specified UPnP devices by a ST parameter in the network.
 * The method waits for the specified MX time. Use setSsdpSearchMX() to change the time.
 *
 * @param aST The Search Target parameter (ex. "ssdp:all")
 */
- (void)searchWithST:(NSString *)aST;
/**
 * Set a MX-parameter used for SSDP searches i.e. Set the time to wait 
 * (in seconds) for device responses to an M-SEARCH
 *
 * @param ctrlPoint The control point in question
 * @param value MX; Time to wait
 */
- (void)setSsdpSearchMX:(int)mx;
- (int)ssdpSearchMX;
- (NSArray *)devices;
@end
