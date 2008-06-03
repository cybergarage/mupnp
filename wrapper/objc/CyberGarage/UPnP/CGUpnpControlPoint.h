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
 * @param ctrlPoint The control point to start
 *
 * @return TRUE if successful; otherwise FALSE
 *
 */
- (BOOL)start;
- (BOOL)stop;
/**
 * Do an M-SEARCH to look for all devices using ssdp:all in the network.
 */
- (void)search;
/**
 * Do an M-SEARCH to look for devices in the network.
 *
 * @param aST The Search Target parameter (ex. "ssdp:all")
 */
- (void)searchWithST:(NSString *)aST;
- (NSArray *)devices;
@end
