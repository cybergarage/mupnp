//
//  CGUpnpAction.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <cybergarage/upnp/caction.h>
#import <CGUpnpAction.h>

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

- (BOOL)setArgumentValue:(NSString *)value forName:(NSString *)name
{
	CgUpnpArgument *cArg;

	if (!cObject)
		return NO;
	cArg = cg_upnp_action_getargumentbyname(cObject, (char *)[name UTF8String]);
	if (!cArg)
		return NO;
	cg_upnp_argument_setvalue(cArg, (char *)[value UTF8String]);
	return YES;
}

- (NSString *)argumentValueForName:(NSString *)name
{
	char *cValue;
	CgUpnpArgument *cArg;

	if (!cObject)
		return nil;
	cArg = cg_upnp_action_getargumentbyname(cObject, (char *)[name UTF8String]);
	if (!cArg)
		return nil;
	cValue = cg_upnp_argument_getvalue(cArg);
	if (cg_strlen(cValue) <= 0)
		return nil;
	return [[NSString alloc] initWithUTF8String:cValue];
}

- (BOOL)post
{
	if (!cObject)
		return NO;
	return cg_upnp_action_post(cObject);
}

- (BOOL)postWithArguments:(NSDictionary *)arguments
{
	NSString *name;

	for (name in arguments) {
		NSString *value = [arguments valueForKey:name];
		[self setArgumentValue:value forName:name];
	}
	return [self post];
}

- (int)statusCode
{
	if (!cObject)
		return 0;
	return cg_upnp_action_getstatuscode(cObject);
}

@end
