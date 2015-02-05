/******************************************************************
 *
 * mUPnP for ObjC
 *
 * Copyright (C) Satoshi Konno 2008
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#include <mupnp/service.h>
#include <mupnp/action.h>
#include <mupnp/statevariable.h>
#import "CGUpnpService.h"
#import "CGUpnpAction.h"
#import "CGUpnpStateVariable.h"

@implementation CGUpnpService

@synthesize cObject;

- (id) initWithCObject:(mUpnpService *)cobj
{
	if ((self = [super init]) == nil)
		return nil;
	cObject = cobj;
	mupnp_service_setuserdata(cObject, self);
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

- (BOOL) parseXMLDescription:(NSString *)xmlDesc;
{
	if (!cObject)
		return NO;
	return mupnp_service_parsedescription(cObject, (char *)[xmlDesc UTF8String], [xmlDesc length]);
}

- (NSString *)serviceId
{
	if (!cObject)
		return nil;
	return [[[NSString alloc] initWithUTF8String:mupnp_service_getserviceid(cObject)] autorelease];
}

- (NSString *)serviceType
{
	if (!cObject)
		return nil;
	return [[[NSString alloc] initWithUTF8String:mupnp_service_getservicetype(cObject)] autorelease];
}

- (NSArray *)actions
{
	if (!cObject)
		return [[[NSArray alloc] init] autorelease];
	NSMutableArray *actionArray = [[[NSMutableArray alloc] init] autorelease];
	mUpnpAction *cAction;
	for (cAction = mupnp_service_getactions(cObject); cAction; cAction = mupnp_action_next(cAction)) {
		CGUpnpAction *action = [[[CGUpnpAction alloc] initWithCObject:(void *)cAction] autorelease];
		[actionArray addObject:action];
	}
	return actionArray;
}

- (NSArray *)stateVariables
{
	if (!cObject)
		return [[[NSArray alloc] init] autorelease];
	NSMutableArray *statVarArray = [[[NSMutableArray alloc] init] autorelease];
	mUpnpStateVariable *cStatVar;
	for (cStatVar = mupnp_service_getstatevariables(cObject); cStatVar; cStatVar = mupnp_statevariable_next(cStatVar)) {
		CGUpnpStateVariable *statVar = [[[CGUpnpStateVariable alloc] initWithCObject:(void *)cStatVar] autorelease];
		[statVarArray addObject:statVar];
	}
	return statVarArray;
}

- (CGUpnpAction *)getActionForName:(NSString *)name
{
	if (!cObject)
		return nil;
	mUpnpAction *cAction = mupnp_service_getactionbyname(cObject, (char *)[name UTF8String]);
	if (!cAction)
		return nil;
	return [[[CGUpnpAction alloc] initWithCObject:(void *)cAction] autorelease];
}

- (CGUpnpStateVariable *)getStateVariableForName:(NSString *)name
{
	if (!cObject)
		return nil;
	mUpnpStateVariable *cStatVar = mupnp_service_getstatevariablebyname(cObject, (char *)[name UTF8String]);
	if (!cStatVar)
		return nil;
	return [[[CGUpnpStateVariable alloc] initWithCObject:(void *)cStatVar] autorelease];
}

- (BOOL)isStateVariableImpemented:(NSString*)name;
{
	CGUpnpStateVariable* stateVariable = [self getStateVariableForName:name];
	
	if (stateVariable)
	{
		return (![stateVariable isAllowedValue:@"NOT_IMPLEMENTED"]);
	}
	
	return NO;
}

@end
