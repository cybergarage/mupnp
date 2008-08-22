//
//  CGUpnpStateVariable.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/28.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <cybergarage/upnp/cstatevariable.h>
#include <cybergarage/upnp/control/ccontrol.h>
#import "CGUpnpStateVariable.h"

@implementation CGUpnpStateVariable

@synthesize cObject;

- (id) initWithCObject:(CgUpnpStateVariable *)cobj
{
	if ((self = [super init]) == nil)
		return nil;
	cObject = cobj;
	cg_upnp_statevariable_setuserdata(cObject, self);
	return self;
}

- (id) init
{
	[self initWithCObject:NULL];
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

- (void) finalize
{
	[super finalize];
}

- (NSString *)name
{
	if (!cObject)
		return nil;
	return [[[NSString alloc] initWithUTF8String:cg_upnp_statevariable_getname(cObject)] autorelease];
}

- (NSString *)value
{
	if (!cObject)
		return nil;
	return [[[NSString alloc] initWithUTF8String:cg_upnp_statevariable_getvalue(cObject)] autorelease];
}

- (BOOL)query
{
	if (!cObject)
		return NO;
	return cg_upnp_statevariable_post(cObject);
}

- (NSInteger)statusCode
{
	if (!cObject)
		return 0;
	return cg_upnp_statevariable_getstatuscode(cObject);
}

@end
