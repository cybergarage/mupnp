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
		if (![dev isDeviceType:@""])
			continue;
		CgUpnpDevice *cDevice = [dev cObject];
		if (!cDevice)
			continue;
		void *devData = cg_upnp_device_getuserdata(cDevice);
		CGUpnpAvServer *server = nil;
		if (!devData) {
			server = [[[CGUpnpAvServer alloc] initWithCObject:cDevice] autorelease];
		}
		else 
			server = (CGUpnpAvServer *)devData;
		[serverArray addObject:server];
	}
	return serverArray;
}

- (CGUpnpAvServer *)serverForUDN:(NSString *)udn
{
	if (udn == nil)
		return nil;
	return nil;
}

- (NSArray *)browse:(CGUpnpAvServer *)server objectId:(NSString *)objectId;
{
	return [server browse:objectId];
}

@end
