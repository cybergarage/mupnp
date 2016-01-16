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
#import <Foundation/NSDictionary.h>

#if !defined(_MUPNP_SERVICE_H_)
typedef void mUpnpService;
#endif

@class CGUpnpAction;
@class CGUpnpStateVariable;

/**
 * The CGUpnpService class is a wrapper class for mUpnpService of mUPnP for C.
 */
@interface CGUpnpService : NSObject {
}
@property (assign, readonly) mUpnpService* cObject;
/**
 * Create a new UPnP service with the C instance.
 * 
 * @return New instance if successfull; otherwise nil.
 */
- (id)initWithCObject:(mUpnpService*)cobj;
/**
 * Parse the specified XML description.
 * 
 * @return YES if successfull; otherwise nui.
 */
- (BOOL)parseXMLDescription:(NSString*)xmlDesc;
/**
 * Get the service ID of the service.
 * 
 * @return The service ID.
 */
- (NSString*)serviceId;
/**
 * Get the service type of the service.
 * 
 * @return The service type.
 */
- (NSString*)serviceType;
/**
 * Get all actions in the service as a NSArray object. The array has the actions as instances of CGUpnpAction.
 *
 * @return NSArray of CGUpnpAction.
 */
- (NSArray*)actions;
/**
 * Get all state variables in the service as a NSArray object. The array has the state variables as instances of CGUpnpStateVariable.
 *
 * @return NSArray of CGUpnpStateVariable.
 */
- (NSArray*)stateVariables;
/**
 * Get a action in the service by the specified name.
 *
 * @param name A name string of the action.
 *
 * @return The CGUpnpAction if the specified action is found; otherwise nil.
 */
- (CGUpnpAction*)getActionForName:(NSString*)name;
/**
 * Get a state variables in the service by the specified name.
 *
 * @param name A name string of the state variable.
 *
 * @return The CGUpnpStateVariable if the specified state variable is found; otherwise nil.
 */
- (CGUpnpStateVariable*)getStateVariableForName:(NSString*)name;

/**
 * Checks whether state variable is implemented
 *
 * @param name A name string of the state variable.
 *
 * @return YES if implemented
 */
- (BOOL)isStateVariableImpemented:(NSString*)name;

@end
