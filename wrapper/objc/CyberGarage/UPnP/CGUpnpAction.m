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

- (NSDictionary *)arguments
{
	if (!cObject)
		return [NSArray array];
	NSMutableDictionary *argDir = [NSMutableDictionary dictionary];
	CgUpnpArgument *carg;
	for (carg = cg_upnp_action_getarguments(cObject); carg; carg = cg_upnp_argument_next(carg)) {
		char *name = cg_upnp_argument_getname(carg);
		char *value = cg_upnp_argument_getvalue(carg);
		[argDir setObject:[[NSString alloc] initWithUTF8String:(value ? value : "")] forKey:[[NSString alloc] initWithUTF8String:name]];
	}
	return argDir;
}

@end
