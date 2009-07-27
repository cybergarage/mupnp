//
//  CGUpnpAvServer.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <cybergarage/upnp/std/av/cmediarenderer.h>

#import "CGUpnpAvRenderer.h"

@implementation CGUpnpAvRenderer

@synthesize cAvObject;

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;

	cAvObject = cg_upnpav_dmr_new();
	[self setCObject:cg_upnpav_dmr_getdevice(cAvObject)];
	
	return self;
}

- (id) initWithCObject:(CgUpnpDevice *)cobj
{
	if ((self = [super initWithCObject:cobj]) == nil)
		return nil;

	cAvObject = NULL;

	return self;
}

- (BOOL)setAVTransportURI:(NSString *)aURL;
{
	CGUpnpService *avTransService = [self getServiceForType:@"urn:schemas-upnp-org:service:AVTransport:1"];
	if (!avTransService)
		return NO;

	CGUpnpAction *action = [avTransService getActionForName:@"SetAVTransportURI"];
	if (!action)
		return NO;

	[action setArgumentValue:@"0" forName:@"InstanceID"];
	[action setArgumentValue:aURL forName:@"CurrentURI"];
	[action setArgumentValue:@"" forName:@"CurrentURIMetaData"];
	
	if (![action post])
		return NO;
	
	return YES;
}

- (BOOL)play;
{
	CGUpnpService *avTransService = [self getServiceForType:@"urn:schemas-upnp-org:service:AVTransport:1"];
	if (!avTransService)
		return NO;
	
	CGUpnpAction *action = [avTransService getActionForName:@"Play"];
	if (!action)
		return NO;
	
	[action setArgumentValue:@"0" forName:@"InstanceID"];
	[action setArgumentValue:@"1" forName:@"Speed"];
	
	if (![action post])
		return NO;
	
	return YES;
}

- (BOOL)stop;
{
	CGUpnpService *avTransService = [self getServiceForType:@"urn:schemas-upnp-org:service:AVTransport:1"];
	if (!avTransService)
		return NO;
	
	CGUpnpAction *action = [avTransService getActionForName:@"Stop"];
	if (!action)
		return NO;
	
	[action setArgumentValue:@"0" forName:@"InstanceID"];
	
	if (![action post])
		return NO;
	
	return YES;
}

/*
- (BOOL)start
{
	if (!cAvObject)
		return NO;
	return cg_upnpav_dms_start(cAvObject);
}

- (BOOL)stop
{
	if (!cAvObject)
		return NO;
	return cg_upnpav_dms_stop(cAvObject);
}
*/

@end
