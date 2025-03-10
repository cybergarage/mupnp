/******************************************************************
 *
 * mUPnP for ObjC
 *
 * Copyright (C) Satoshi Konno 2008
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#import <Foundation/NSDictionary.h>
#import <Foundation/NSString.h>

#if !defined(_MUPNP_ACTION_H_)
typedef void mUpnpAction;
#endif

/**
 * The CGUpnpAction class is a wrapper class for mUpnpAction of mUPnP for C.
 */
@interface CGUpnpAction : NSObject {
}
@property (assign, readonly) mUpnpAction* cObject;
- (id)initWithCObject:(mUpnpAction*)cobj;
/**
 * Get the name of the action.
 *
 * @return The name.
 */
- (NSString*)name;
/**
 * Get all arguments in the action as a NSDictionary object.
 *
 * @return NSDictionary of the arguments.
 */
- (NSDictionary*)arguments;
/**
 * Set a value of the specified argument from the action by the argument's name directly.
 *
 * @param value The value to set
 * @param name The name of the argument to look for
 * @return YES if successfull; otherwise NO
 */
- (BOOL)setArgumentValue:(NSString*)value forName:(NSString*)name;
/**
 * Get a value of the specified argument from the action by the argument's name directly.
 *
 * @param name The name of the argument to look for
 * @return The target value if successfull; otherwise NULL
 */
- (NSString*)argumentValueForName:(NSString*)name;
/**
 * Send action
 *
 * @return YES if successfull; otherwise NO
 */
- (BOOL)post;
/**
 * Send action
 *
 * @param arguments Arguments to set
 *
 * @return YES if successfull; otherwise NO
 */
- (BOOL)postWithArguments:(NSDictionary*)arguments;
/**
 * Get a states code of the last post.
 *
 * @return The status code
 */
- (NSInteger)statusCode;
@end
