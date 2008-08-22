//
//  CGUpnpAvController.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/06/25.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import "CGUpnpAvController.h"
#import "CGUpnpAvServer.h"
#import "CGUpnpAvConstants.h"
#import "CGXml.h"
#import "CGXmlNode.h"
#import "CGUpnpAvObject.h"
#import "CGUpnpAvContainer.h"

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
			[server setUserObject:server];
		}
		else 
			server = (CGUpnpAvServer *)devData;
		if (server == nil)
			continue;
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
		if ([server isUDN:aUdn])
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
		if ([server isFriendlyName:aFriendlyName])
			return [[server retain] autorelease];
	}
	return nil;
}

- (CGUpnpAvServer *)serverForPath:(NSString *)aPath;
{
	NSArray *srvAndObjPathArray = [aPath pathComponents];
	if ([srvAndObjPathArray count] <= 0)
		return nil;
	if ([aPath isAbsolutePath] && ([srvAndObjPathArray count] <= 1))
		return nil;
	NSString *avSrvName = [CGXml unescapestring:[aPath isAbsolutePath] ? [srvAndObjPathArray objectAtIndex:1] : [srvAndObjPathArray objectAtIndex:0]];
	return [self serverForFriendlyName:avSrvName];
}

- (CGUpnpAvServer *)serverForIndexPath:(NSIndexPath *)aIndexPath
{
	int idxCnt = [aIndexPath length];
	if (idxCnt < 1)
		return nil;
	
	NSArray *servers = [self servers];
	int serverNum = [aIndexPath indexAtPosition:0];
	if ([servers count] <= serverNum)
		return nil;
	
	return [servers objectAtIndex:serverNum];
}

- (CGUpnpAvObject *)objectForTitlePath:(NSString *)aServerAndTitlePath
{
	CGUpnpAvServer *avSrv = [self serverForPath:aServerAndTitlePath];
	if (avSrv == nil)
		return nil;

	NSMutableArray *titlePathArray = [NSMutableArray arrayWithArray:[aServerAndTitlePath pathComponents]];
	[titlePathArray removeObjectAtIndex:0];
	if (aServerAndTitlePath.isAbsolutePath)
		[titlePathArray removeObjectAtIndex:0];

	NSString *titlePath = [NSString pathWithComponents:titlePathArray];
	CGUpnpAvObject *avObj = [avSrv objectForTitlePath:titlePath];
	
	return [[avObj retain] autorelease];
}

- (CGUpnpAvObject *)objectForIndexPath:(NSIndexPath *)aServerAndTitleIndexPath
{
	CGUpnpAvServer *avSrv = [self serverForIndexPath:aServerAndTitleIndexPath];
	if (!avSrv)
		return nil;
	
	CGUpnpAvContainer*rootObj = [avSrv rootObject];
	if (!rootObj)
		return nil;

	int idxCnt = [aServerAndTitleIndexPath length];
	if (idxCnt < 2)
		return rootObj;
	
	CGUpnpAvObject *avObj = [rootObj childAtIndex:[aServerAndTitleIndexPath indexAtPosition:1]];
	if (!avObj)
		return nil;

	for (int n=2; n<idxCnt; n++) {
		if ([avObj isItem])
			return nil;
		CGUpnpAvContainer *avCon = (CGUpnpAvContainer *)avObj;
		avObj = [avCon childAtIndex:[aServerAndTitleIndexPath indexAtPosition:n]];
		if (!avObj)
			return nil;
	}
	
	return [[avObj retain] autorelease];
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

- (NSArray *)browseWithIndexPath:(NSIndexPath *)aServerAndTitleIndexPath
{
	CGUpnpAvServer *avSrv = [self serverForIndexPath:aServerAndTitleIndexPath];
	if (avSrv == nil)
		return nil;
	
	CGUpnpAvObject *avObj = [self objectForIndexPath:aServerAndTitleIndexPath];
	if (avObj == nil)
		return nil;

	return [avSrv browse:[avObj objectId]];
}

@end

