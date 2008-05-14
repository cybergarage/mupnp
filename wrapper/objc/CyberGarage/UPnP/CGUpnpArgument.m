//
//  CGUpnpArgument.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <cybergarage/upnp/cArgument.h>

#import <CyberGarage/UPnP/CGUpnpArgument.h>

@implementation CGUpnpArgument

@synthesize cObject;

- (id) initWithCObject:(CgUpnpArgument *)cobj
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
	return [[NSString alloc] initWithUTF8String:cg_upnp_Argument_getname(cObject)];
}

@end
