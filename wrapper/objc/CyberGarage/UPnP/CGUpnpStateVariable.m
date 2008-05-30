//
//  CGUpnpStateVariable.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/28.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <cybergarage/upnp/cstatevariable.h>
#import <CGUpnpStateVariable.h>

@implementation CGUpnpStateVariable

@synthesize cObject;

- (id) initWithCObject:(CgUpnpStateVariable *)cobj
{
	if ((self = [super init]) == nil)
		return nil;
	cObject = cobj;
	return self;
}

- (id) init
{
	[self initWithCObject:NULL];
	return self;
}

- (void) finalize
{
	[super finalize];
}

- (NSString *)name
{
	if (!cObject)
		return nil;
	return [[NSString alloc] initWithUTF8String:cg_upnp_statevariable_getname(cObject)];
}

- (NSString *)value
{
	if (!cObject)
		return nil;
	return [[NSString alloc] initWithUTF8String:cg_upnp_statevariable_getvalue(cObject)];
}

- (BOOL)query
{
	if (!cObject)
		return NO;
	return cg_upnp_statevariable_post(cObject);
}

- (int)statusCode
{
	if (!cObject)
		return 0;
	return cg_upnp_statevariable_getstatuscode(cObject);
}

@end
