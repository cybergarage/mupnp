/******************************************************************
 *
 * mUPnP for ObjC
 *
 * Copyright (C) Satoshi Konno 2008
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#include <mupnp/icon.h>
#import "CGUpnpIcon.h"

@implementation CGUpnpIcon

@synthesize cObject;
#if defined(TARGET_OS_IPHONE)
@synthesize resourceName;
#endif

- (id) initWithCObject:(mUpnpIcon *)cobj
{
	if ((self = [super init]) == nil)
		return nil;
	cObject = cobj;
	return self;
}

- (id) init
{
	if ((self = [self initWithCObject:NULL]) == nil)
    	return nil;
	return self;
}

- (void) dealloc
{
	self.resourceName = nil;
    
	[super dealloc];
}

- (NSString *)url
{
	if (!cObject)
		return nil;
	return [[[NSString alloc] initWithUTF8String:mupnp_icon_geturl(cObject)] autorelease];
}

- (NSString *)mimeType
{
	if (!cObject)
		return nil;
	return [[[NSString alloc] initWithUTF8String:mupnp_icon_getmimetype(cObject)] autorelease];
}

- (NSInteger)width
{
	if (!cObject)
		return 0;
	return mupnp_icon_getwidth(cObject);
}

- (NSInteger)height
{
	if (!cObject)
		return 0;
	return mupnp_icon_getheight(cObject);
}

- (NSInteger)depth
{
	if (!cObject)
		return 0;
	return mupnp_icon_getdepth(cObject);
}

#if defined(TARGET_OS_IPHONE)
- (void)setResourceName:(NSString *) aResourceName
{
	//UIImage *image = [UIImage imageNamed:aResourceName];
	//CGSize imageSize = CGSizeMake(CGImageGetWidth([image CGImage]), CGImageGetHeight([image CGImage]));
}
#endif

@end
