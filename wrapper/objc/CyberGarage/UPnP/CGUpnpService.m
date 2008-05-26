//
//  CGUpnpService.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <cybergarage/upnp/cservice.h>
#import <CyberGarage/UPnP/CGUpnpService.h>
#import <CyberGarage/UPnP/CGUpnpAction.h>

@implementation CGUpnpService

@synthesize cObject;

- (id) initWithCObject:(CgUpnpService *)cobj
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

- (NSString *)serviceId
{
	if (!cObject)
		return nil;
	return [[NSString alloc] initWithUTF8String:cg_upnp_service_getserviceid(cObject)];
}

- (NSString *)serviceType
{
	if (!cObject)
		return nil;
	return [[NSString alloc] initWithUTF8String:cg_upnp_service_getservicetype(cObject)];
}

- (NSArray *)actions
{
	if (!cObject)
		return [NSArray array];
	NSMutableArray *actionArray = [NSMutableArray array];
	CgUpnpAction *caction;
	for (caction = cg_upnp_service_getactions(cObject); caction; caction = cg_upnp_action_next(caction)) {
		CGUpnpAction *action = [[CGUpnpAction alloc] initWithCObject:caction];
		[actionArray addObject:action];
	}
	return actionArray;
}

- (NSDictionary *)stateVariables
{
	if (!cObject)
		return [NSArray array];
	NSMutableDictionary *statVarDir = [NSMutableDictionary dictionary];
	CgUpnpStateVariable *cstatVar;
	for (cstatVar = cg_upnp_service_getstatevariables(cObject); cstatVar; cstatVar = cg_upnp_statevariable_next(cstatVar)) {
		char *name = cg_upnp_statevariable_getname(cstatVar);
		char *value = cg_upnp_statevariable_getvalue(cstatVar);
		[statVarDir setObject:[[NSString alloc] initWithUTF8String:(value ? value : "")] forKey:[[NSString alloc] initWithUTF8String:name]];
	}
	return statVarDir;
}

@end
