//
//  CGUpnpService.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#if !defined(_CG_UPNP_CSERVICE_H_)
typedef void CgUpnpService;
#endif

@class CGUpnpAction;
@class CGUpnpStateVariable;

/**
 * The CGUpnpControlPoint class is a wrapper class for CgUpnpControlPoint of CyberLink for C to 
 * program using Objective-C directly on MacOSX.
 * Currently, the only basic methods are wrapped to control UPnP devices.
 */

@interface CGUpnpService : NSObject 
{
	CgUpnpService *cObject;
}
@property(readonly) CgUpnpService *cObject;
- (id) initWithCObject:(CgUpnpService *)cobj;
- (NSString *)serviceId;
- (NSString *)serviceType;
- (NSArray *)actions;
- (NSArray *)stateVariables;
- (CGUpnpAction *)getActionByName:(NSString *)name;
- (CGUpnpStateVariable *)getStateVariableByName:(NSString *)name;
@end
