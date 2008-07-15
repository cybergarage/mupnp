//
//  CGUpnpAvController.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/06/25.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import <CyberLink/UPnP.h>
#import <CGUpnpAvController.h>
#import <CGUpnpAvServer.h>
#import <CGUpnpAvConstants.h>

@implementation CGUpnpAvController

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

- (void)finalize
{
	[super finalize];
}

- (NSArray *)servers;
{
	NSArray *devices = [self devices];
	NSMutableArray *serverArray = [[[NSMutableArray alloc] init] autorelease];

	for (CGUpnpDevice *dev in devices) {
		if (![dev isDeviceType:CG_UPNPAV_DEVICE_TYPE])
			continue;
		CGUpnpAvServer *server = nil;
		void *devData = [dev userData];
		if (!devData) {
			CgUpnpDevice *cDevice = [dev cObject];
			if (!cDevice)
				continue;
			server = [[[CGUpnpAvServer alloc] initWithCObject:cDevice] autorelease];
			[server setUserData:server];
		}
		else 
			server = (CGUpnpAvServer *)devData;
		[serverArray addObject:server];
	}
	return serverArray;
}

- (CGUpnpAvServer *)serverForUDN:(NSString *)aUdn
{
	if (aUdn == nil)
		return nil;
	NSArray *servers = [self servers];
	for (CGUpnpAvServer *server in servers) {
		if (![dev isUDN:aUdn])
			return [[server retain] autorelease];
	}
	return nil;
}

- (CGUpnpAvServer *)serverForFriendlyName:(NSString *)aFriendlyName
{
	if (aUdn == nil)
		return nil;
	NSArray *servers = [self servers];
	for (CGUpnpAvServer *server in servers) {
		if (![dev isFriendlyName:aFriendlyName])
			return [[server retain] autorelease];
	}
	return nil;
}

- (NSArray *)browse:(CGUpnpAvServer *)server objectId:(NSString *)objectId
{
	return [server browse:objectId];
}

- (CGUpnpAvObject *)objectForTitlePath:(CGUpnpAvServer *)server titlePath:(NSString *)titlePath
{
	return [server objectForTitlePath:titlePath];
}

@end

