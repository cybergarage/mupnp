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

#if !defined(_MUPNP_ICON_H_)
typedef void mUpnpIcon;
#endif

/**
 * The CGUpnpIcon class is a wrapper class for mUpnpIcon of mUPnP for C.
 */
@interface CGUpnpIcon : NSObject {
#if defined(TARGET_OS_IPHONE)
  NSString* resourceName;
#endif
}
@property (assign, readonly) mUpnpIcon* cObject;
#if defined(TARGET_OS_IPHONE)
@property (readonly) NSString* resourceName;
#endif
- (id)initWithCObject:(mUpnpIcon*)cobj;
/**
 * Get the url of the icon.
 * 
 * @return The url.
 */
- (NSString*)url;
/**
 * Get the url of the width.
 * 
 * @return The width.
 */
- (NSInteger)width;
/**
 * Get the height of the icon.
 * 
 * @return The height.
 */
- (NSInteger)height;
/**
 * Get the depth of the icon.
 * 
 * @return The depth.
 */
- (NSInteger)depth;
#if defined(TARGET_OS_IPHONE)
- (void)setResourceName:(NSString*)aResourceName;
#endif
@end
