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
 * The CGUpnpService class is a wrapper class for CgUpnpService of CyberLink for C.
 */
@interface CGUpnpService : NSObject 
{
	CgUpnpService *cObject;
}
@property(readonly) CgUpnpService *cObject;
- (id) initWithCObject:(CgUpnpService *)cobj;
/**
 * Get the service ID of the service.
 * 
 * @return The service ID.
 */
- (NSString *)serviceId;
/**
 * Get the service type of the service.
 * 
 * @return The service type.
 */
- (NSString *)serviceType;
/**
 * Get all actions in the service as a NSArray object. The array has the actions as instances of CGUpnpAction.
 *
 * @return NSArray of CGUpnpAction.
 */
- (NSArray *)actions;
/**
 * Get all state variables in the service as a NSArray object. The array has the state variables as instances of CGUpnpStateVariable.
 *
 * @return NSArray of CGUpnpStateVariable.
 */
- (NSArray *)stateVariables;
/**
 * Get a action in the service by the specified name.
 *
 * @param name A name string of the action.
 *
 * @return The CGUpnpAction if the specified action is found; otherwise nil.
 */
- (CGUpnpAction *)getActionForName:(NSString *)name;
/**
 * Get a state variables in the service by the specified name.
 *
 * @param name A name string of the state variable.
 *
 * @return The CGUpnpStateVariable if the specified state variable is found; otherwise nil.
 */
- (CGUpnpStateVariable *)getStateVariableForName:(NSString *)name;
@end
