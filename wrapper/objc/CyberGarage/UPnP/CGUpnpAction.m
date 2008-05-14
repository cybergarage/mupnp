//
//  CGUpnpAction.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <cybergarage/upnp/caction.h>

#import <CyberGarage/UPnP/CGUpnpAction.h>

@implementation CGUpnpAction

@synthesize cObject;

- (id) initWithCObject:(CgUpnpAction *)cobj
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
	return [[NSString alloc] initWithUTF8String:cg_upnp_action_getname(cObject)];
}

@end
