//
//  CGUpnpAvController.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/06/25.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <mupnp/ssdp/ssdp.h>
#include <mupnp/std/av/cmediaserver.h>

#import "CGUpnpAvController.h"
#import "CGUpnpAvServer.h"
#import "CGUpnpAvConstants.h"
#import "CGXml.h"
#import "CGXmlNode.h"
#import "CGUpnpAvObject.h"
#import "CGUpnpAvContainer.h"
#import "CGUpnpAvRenderer.h"


@interface CGUpnpAvController() {
    NSArray<CGUpnpAvRenderer *> *rendererArray;
    NSArray<CGUpnpAvServer *> *serverArray;
}

@end

@implementation CGUpnpAvController

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	
	[self setSsdpSearchMX:1];
	
	return self;
}

- (void)dealloc
{
}

////////////////////////////////////////////////////////////
// Media Server
////////////////////////////////////////////////////////////

- (NSArray *)servers;
{
	NSArray *devices = [self devices];
	NSMutableArray *tempServerArray = [[NSMutableArray alloc] init];

	for (CGUpnpDevice *dev in devices) {
		if (![dev isDeviceType:@CG_UPNPAV_DMS_DEVICE_TYPE])
			continue;
		CGUpnpAvServer *server = nil;		
		void *devData = [dev userData];
		if (!devData) {
			mUpnpDevice *cDevice = [dev cObject];
			if (!cDevice)
				continue;
            
            server = [self serverWithCObject:cDevice];
            if (nil == server) {
                server = [[CGUpnpAvServer alloc] initWithCObject:(__bridge CgUpnpDevice *)(cDevice)];
            }
			
			[server setUserObject:server];
		}
		else
			server = (CGUpnpAvServer *)((__bridge id)devData);
		if (server == nil)
			continue;
		[tempServerArray addObject:server];
	}
    serverArray = tempServerArray;
	return serverArray;
}

- (CGUpnpAvServer *)serverForUDN:(NSString *)aUdn
{
	if (aUdn == nil)
		return nil;
	
    __block CGUpnpAvServer *server = nil;
    [serverArray enumerateObjectsUsingBlock:^(CGUpnpAvServer * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        if ([obj isUDN:aUdn])
        {
            server = obj;
            *stop = YES;
        }
    }];
    return server;
}

- (CGUpnpAvServer *)serverWithCObject:(mUpnpDevice *)cDevice
{
    __block CGUpnpAvServer *server = nil;
    [serverArray enumerateObjectsUsingBlock:^(CGUpnpAvServer * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        if (obj.cObject == cDevice)
        {
            server = obj;
            *stop = YES;
        }
    }];
    return server;
}

- (CGUpnpAvServer *)serverForFriendlyName:(NSString *)aFriendlyName
{
	if (aFriendlyName == nil)
		return nil;
    
    __block CGUpnpAvServer *server = nil;
    [serverArray enumerateObjectsUsingBlock:^(CGUpnpAvServer * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        if ([obj isFriendlyName:aFriendlyName])
        {
            server = obj;
            *stop = YES;
        }
    }];
    return server;
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
	NSUInteger idxCnt = [aIndexPath length];
	if (idxCnt < 1)
		return nil;
	
	NSArray *servers = [self servers];
	NSUInteger serverNum = [aIndexPath indexAtPosition:0];
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
	
	return avObj;
}

- (CGUpnpAvObject *)objectForIndexPath:(NSIndexPath *)aServerAndTitleIndexPath
{
	CGUpnpAvServer *avSrv = [self serverForIndexPath:aServerAndTitleIndexPath];
	if (!avSrv)
		return nil;
	
	CGUpnpAvContainer*rootObj = [avSrv rootObject];
	if (!rootObj)
		return nil;

	CGUpnpAvObject *avObj = rootObj;
	NSUInteger idxPathLength = [aServerAndTitleIndexPath length];
	NSUInteger n;
	for (n=1; n<idxPathLength; n++) {
		if ([avObj isItem])
			return nil;
		CGUpnpAvContainer *avCon = (CGUpnpAvContainer *)avObj;
		NSUInteger avObjIdx = [aServerAndTitleIndexPath indexAtPosition:n];
		avObj = [avCon childAtIndex:avObjIdx];
		NSLog(@"objectForIndexPath = [%tu/%tu][%tu/%tu] %@ (%@)", n, idxPathLength, avObjIdx,  [avCon childCount], [avObj title], [avObj objectId]);
		if (!avObj)
			return nil;
	}
	
	
	return avObj;
}

- (NSArray *)browseDirectChildrenWithTitlePath:(NSString *)aServerAndTitlePath
{
	CGUpnpAvServer *avSrv = [self serverForPath:aServerAndTitlePath];
	if (avSrv == nil)
		return nil;
	
	CGUpnpAvObject *avObj = [self objectForTitlePath:aServerAndTitlePath];
	if (avObj == nil)
		return nil;
	
	return [avSrv browseDirectChildren:[avObj objectId]];
}

- (NSArray *)browseDirectChildrenWithIndexPath:(NSIndexPath *)aServerAndTitleIndexPath
{
	CGUpnpAvServer *avSrv = [self serverForIndexPath:aServerAndTitleIndexPath];
	if (avSrv == nil)
		return nil;
	
	CGUpnpAvObject *avObj = [self objectForIndexPath:aServerAndTitleIndexPath];
	if (avObj == nil)
		return nil;

	return [avSrv browseDirectChildren:[avObj objectId]];
}

////////////////////////////////////////////////////////////
// Media Renderer
////////////////////////////////////////////////////////////

- (NSArray *)renderers;
{
	NSArray *devices = [self devices];
	NSMutableArray *tempRendererArray = [[NSMutableArray alloc] init];
	
	for (CGUpnpDevice *dev in devices) {
		if (![dev isDeviceType:@CG_UPNPAV_DMR_DEVICE_TYPE])
			continue;
		mUpnpDevice *cDevice = [dev cObject];
		if (!cDevice)
			continue;
        
        CGUpnpAvRenderer *renderer = [self rendererWithCObject:cDevice];
        if (nil == renderer)
        {
            renderer = [[CGUpnpAvRenderer alloc] initWithCObject:cDevice];
            dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
                [self subscribeEventNotificationFromDevice:renderer];
            });
        }
        
		if (renderer == nil)
			continue;
		[tempRendererArray addObject:renderer];
	}
    rendererArray = tempRendererArray;
    
	return rendererArray;
}

- (CGUpnpAvRenderer *)rendererForUDN:(NSString *)aUdn
{
	if (aUdn == nil)
		return nil;
    
    __block CGUpnpAvRenderer *renderer = nil;
	[rendererArray enumerateObjectsUsingBlock:^(CGUpnpAvRenderer * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        if ([obj isUDN:aUdn]) {
            renderer = obj;
            *stop = YES;
        }
    }];
    
    return renderer;
}

- (CGUpnpAvRenderer *)rendererWithCObject:(mUpnpDevice *)cDevice
{
    __block CGUpnpAvRenderer *renderer = nil;
    [rendererArray enumerateObjectsUsingBlock:^(CGUpnpAvRenderer * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        if (obj.cObject == cDevice)
        {
            renderer = obj;
            *stop = YES;
        }
    }];
    return renderer;
}

- (CGUpnpAvRenderer *)rendererWithSubscriptionID:(NSString *)sid
{
    if (nil == sid)
    {
        return nil;
    }
    
    __block CGUpnpAvRenderer *renderer = nil;
    [rendererArray enumerateObjectsUsingBlock:^(CGUpnpAvRenderer * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        if (nil != [obj serviceWithSubscriptionID:sid])
        {
            renderer = obj;
            *stop = YES;
        }
    }];
    return renderer;
}

////////////////////////////////////////////////////////////
// Search
////////////////////////////////////////////////////////////

- (void)search
{
	[super search];
#if defined(CG_UPNPAVCONTROLLER_SERCH_DEVICETYPES)
	[super searchWithST:[NSString stringWithUTF8String:CG_UPNPAV_DMS_DEVICE_TYPE]];
	[super searchWithST:[NSString stringWithUTF8String:CG_UPNPAV_DMR_DEVICE_TYPE]];
#endif
}

- (BOOL)subscribeEventNotificationFromDevice:(CGUpnpAvRenderer *)renderer
{
    BOOL bRet = YES;
    
    CGUpnpService *renderingControlService = [renderer renderControlService];
    if (renderingControlService) {
        bRet &= [super subscribeEventNotificationFromService:renderingControlService];
    }
    
    CGUpnpService *transportService = [renderer transportService];
    if (transportService) {
        bRet &= [super subscribeEventNotificationFromService:transportService];
    }
    
    CGUpnpService *connectionManagerService = [renderer connectionManagerService];
    if (connectionManagerService) {
        bRet &= [super subscribeEventNotificationFromService:connectionManagerService];
    }
    
    return bRet;
}

- (BOOL)unsubscribeEventNotificationFromDevice:(CGUpnpAvRenderer *)renderer
{
    BOOL bRet = YES;
    
    CGUpnpService *renderingControlService = [renderer renderControlService];
    if (renderingControlService) {
        bRet &= [super unsubscribeEventNotificationFromService:renderingControlService];
    }
    
    CGUpnpService *transportService = [renderer transportService];
    if (transportService) {
        bRet &= [super unsubscribeEventNotificationFromService:transportService];
    }
    
    CGUpnpService *connectionManagerService = [renderer connectionManagerService];
    if (connectionManagerService) {
        bRet &= [super unsubscribeEventNotificationFromService:connectionManagerService];
    }
    
    return bRet;
}

@end

