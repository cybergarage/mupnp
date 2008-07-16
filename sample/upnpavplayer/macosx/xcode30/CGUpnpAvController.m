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
		if (![server isUDN:aUdn])
			return [[server retain] autorelease];
	}
	return nil;
}

- (CGUpnpAvServer *)serverForFriendlyName:(NSString *)aFriendlyName
{
	if (aFriendlyName == nil)
		return nil;
	NSArray *servers = [self servers];
	for (CGUpnpAvServer *server in servers) {
		if (![server isFriendlyName:aFriendlyName])
			return [[server retain] autorelease];
	}
	return nil;
}

- (CGUpnpAvServer *)serverForPath:(NSString *)aPath;
{
	NSArray *srvAndObjPathArray = [aPath pathComponents];
	if ([srvAndObjPathArray count] <= 0)
		return nil;
	if (aPath.isAbsolutePath && ([srvAndObjPathArray count] <= 1))
		return nil;
	NSString *avSrvName = aPath.isAbsolutePath ? srvAndObjPathArray[1] : srvAndObjPathArray[0];
	return [dmc serverForFriendlyName:avSrvName];
}

- (NSArray *)browse:(CGUpnpAvServer *)server objectId:(NSString *)aObjectId
{
	return [server browse:aObjectId];
}

- (NSArray *)browseWithTitlePath:(NSString *)aServerAndTitlePath
{
	CGUpnpAvServer *avSrv = [self serverForPath:aServerAndTitlePath];
	if (avSrv == nil)
		return nil;

	CGUpnpAvObject *avObj = [self objectForTitlePath:aServerAndTitlePath];
	if (avObj == nil)
		return nil;

	return [avSrv browse:[avObj objectId]];
}

- (CGUpnpAvObject *)objectForTitlePath:(NSString *)aServerAndTitlePath
{
	CGUpnpAvServer *avSrv = [self serverForPath:aServerAndTitlePath];
	if (avSrv == nil)
		return nil;

	NSMutableArray *titlePathArray = [[NSMutableArray alloc] arrayWithArray:srvAndObjPathArray];
	[titlePathArray removeObjectAtIndex:0];
	if (aServerAndTitlePath.isAbsolutePath)
		[titlePathArray removeObjectAtIndex:0];

	NSString *titlePath = [NSString pathWithComponents:titlePathArray];
	CGUpnpAvObject *avObj = [[[avSrv objectForTitlePath:titlePathArray] retain] autorelease];
	[titlePath release];
	return avObj;
}

@end

