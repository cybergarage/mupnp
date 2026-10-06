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

#if !defined(_MUPNP_STATEVARIABLE_H_)
typedef void mUpnpStateVariable;
#endif

/**
 * The CGUpnpStateVariable class is a wrapper class for mUpnpStateVariable of mUPnP for C.
 */
@interface CGUpnpStateVariable : NSObject {
}
/* Retains the native object's owning wrapper, including discovery snapshots. */
@property (retain) NSObject* cObjectOwner;
@property (assign, readonly) mUpnpStateVariable* cObject;
/**
 * Create a wrapper for the specified native state variable.
 *
 * The native object is borrowed: the wrapper does not take ownership of it
 * and does not modify its userdata. Passing NULL (or using -init) creates an
 * invalid wrapper: name, value and statusCode return nil/0, allowedValues
 * returns an empty array, and isAllowedValue: and query return NO.
 *
 * @param cobj The native state variable, or NULL
 */
- (id)initWithCObject:(mUpnpStateVariable*)cobj;
/**
 * Get the name of the state variable.
 *
 * @return The name.
 */
- (NSString*)name;
/**
 * Get the value of the state variable.
 *
 * @return The value.
 */
- (NSString*)value;

/**
 * Get an array of the allowed values for the state variable
 *
 * @return The array.
 */
- (NSArray*)allowedValues;

/**
 * Checks whether value is allowed for this state variable
 *
 * @param value to be checked
 *
 * @return YES if true
 */
- (BOOL)isAllowedValue:(NSString*)value;

/**
 * Send query
 *
 * @return YES if successfull; otherwise NO
 */
- (BOOL)query;
/**
 * Get a states code of the last query.
 *
 * @return The status code
 */
- (NSInteger)statusCode;
@end
