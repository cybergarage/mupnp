//
//  CGUpnpAvController.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/06/25.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class CGUpnpControlPoint;

/**
 * The CGUpnpControlPoint class is a wrapper class for CgUpnpControlPoint of CyberLink for C to 
 * program using only Objective-C directly on MacOSX.
 * Currently, the only basic methods are wrapped to control UPnP devices.
 */
@interface CGUpnpAvController : CGUpnpControlPoint 
{
	NSMutableArray *serverArray;
}
/**
 * Activate some background threads of the control point such as SSDP and 
 * HTTP servers to listen messages and events of UPnP. You must call this 
 * method before you can actually use a control point.
 *
 * @return TRUE if successful; otherwise FALSE
 *
 */
- (NSArray *)browse:(CGUpnpDevice *)device objectId:(NSString *)objectId;
- (NSArray *)servers;
@end
