//
//  CGUpnpStateVariable.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/NSString.h>

#if !defined(_MUPNP_STATEVARIABLE_H_)
typedef void mUpnpStateVariable;
#endif

/**
 * The CGUpnpStateVariable class is a wrapper class for mUpnpStateVariable of CyberLink for C.
 */
@interface CGUpnpStateVariable : NSObject 
{
}
@property(assign, readonly) mUpnpStateVariable *cObject;
- (id)initWithCObject:(mUpnpStateVariable *)cobj;
/**
 * Get the name of the state variable.
 * 
 * @return The name.
 */
- (NSString *)name;
/**
 * Get the value of the state variable.
 * 
 * @return The value.
 */
- (NSString *)value;

/**
 * Get an array of the allowed values for the state variable
 * 
 * @return The array.
 */
- (NSArray *)allowedValues;

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
