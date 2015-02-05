/******************************************************************
 *
 * mUPnP for ObjC
 *
 * Copyright (C) Satoshi Konno 2008
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#include <mupnp/statevariable.h>
#include <mupnp/control/control.h>
#import "CGUpnpStateVariable.h"

@implementation CGUpnpStateVariable

@synthesize cObject;

- (id) initWithCObject:(mUpnpStateVariable *)cobj
{
	if ((self = [super init]) == nil)
		return nil;
	cObject = cobj;
	mupnp_statevariable_setuserdata(cObject, self);
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
	[super dealloc];
}

- (NSString *)name
{
	if (!cObject)
		return nil;
	return [[[NSString alloc] initWithUTF8String:mupnp_statevariable_getname(cObject)] autorelease];
}

- (NSString *)value
{
	if (!cObject)
		return nil;
	return [[[NSString alloc] initWithUTF8String:mupnp_statevariable_getvalue(cObject)] autorelease];
}

- (NSArray *)allowedValues
{
	if (!cObject)
		return [[[NSArray alloc] init] autorelease];
	NSMutableArray *valuesArray = [[[NSMutableArray alloc] init] autorelease];
	
	mUpnpAllowedValue *cAllowedValue;
	for (cAllowedValue = mupnp_statevariable_getallowedvaluelist(cObject); cAllowedValue; cAllowedValue = (mUpnpAllowedValue*)mupnp_list_next((mUpnpList*)cAllowedValue)) {
		NSString *value = [[[NSString alloc] initWithUTF8String:mupnp_string_getvalue(cAllowedValue->value)] autorelease];
		[valuesArray addObject:value];
	}
	return valuesArray;
}

- (BOOL)isAllowedValue:(NSString*)value
{
	return mupnp_statevariable_is_allowed_value(cObject, [value UTF8String]);
}
		 
- (BOOL)query
{
	if (!cObject)
		return NO;
	return mupnp_statevariable_post(cObject);
}

- (NSInteger)statusCode
{
	if (!cObject)
		return 0;
	return mupnp_statevariable_getstatuscode(cObject);
}

@end
