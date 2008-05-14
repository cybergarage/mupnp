//
//  CGUpnpStateVariable.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <cybergarage/upnp/cstatevariable.h>

#import <CyberGarage/UPnP/CGUpnpStateVariable.h>

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

@end
