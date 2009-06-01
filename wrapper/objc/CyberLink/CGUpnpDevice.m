//
//  CGUpnpDevice.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/03/14.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <cybergarage/upnp/cdevice.h>
#include <cybergarage/upnp/cservice.h>
#include <cybergarage/upnp/cicon.h>

#import "CGUpnpDevice.h"
#import "CGUpnpService.h"
#import "CGUpnpIcon.h"

@implementation CGUpnpDevice

@synthesize cObject;

- (id) init
{
	if ((self = [super init]) == nil)
		return nil;
	cObject = cg_upnp_device_new();
	isCObjectCreated = YES;
	if (!cObject)
		return nil;
	return self;
}

- (id) initWithCObject:(CgUpnpDevice *)cobj
{
	if ((self = [super init]) == nil)
		return nil;
	cObject = cobj;
	isCObjectCreated = NO;
	return self;
}

- (id) initWithXMLDescription:(NSString *)xmlDesc
{
	if ((self = [super init]) == nil)
		return nil;
	cObject = cg_upnp_device_new();
	isCObjectCreated = YES;
	if (!cObject)
		return nil;
	if (![self parseXMLDescription:xmlDesc]) {
		cg_upnp_device_delete(cObject);
		return nil;
	}
	cg_upnp_device_setuserdata(cObject, self);
	return self;
}

- (BOOL) parseXMLDescription:(NSString *)xmlDesc;
{
	if (!cObject)
		return NO;
	return cg_upnp_device_parsedescription(cObject, (char *)[xmlDesc UTF8String], [xmlDesc length]);
}

- (void) dealloc
{
	if (isCObjectCreated && cObject)
		cg_upnp_device_delete(cObject);
	[super dealloc];
}

- (void) finalize
{
	if (isCObjectCreated && cObject)
		cg_upnp_device_delete(cObject);
	[super finalize];
}

- (NSString *)friendlyName
{
	if (!cObject)
		return nil;
	return [[[NSString alloc] initWithUTF8String:cg_upnp_device_getfriendlyname(cObject)] autorelease];
}

- (NSString *)deviceType
{
	if (!cObject)
		return nil;
	return [[[NSString alloc] initWithUTF8String:cg_upnp_device_getdevicetype(cObject)] autorelease];
}

- (NSString *)udn
{
	if (!cObject)
		return nil;
	return [[[NSString alloc] initWithUTF8String:cg_upnp_device_getudn(cObject)] autorelease];
}

- (NSString *)manufacturer
{
	if (!cObject)
		return nil;
	return [[[NSString alloc] initWithUTF8String:cg_upnp_device_getmanufacturer(cObject)] autorelease];
}

- (NSString *)urlBase
{
	if (!cObject)
		return nil;
	return [[[NSString alloc] initWithUTF8String:cg_upnp_device_geturlbase(cObject)] autorelease];
}

-(void)setDeviceType:(NSString *)aType
{
	if (!cObject)
		return;
	cg_upnp_device_setdevicetype(cObject, (char *)[aType UTF8String]);
}

-(void)setFriendlyName:(NSString *)aName
{
	if (!cObject)
		return;
	cg_upnp_device_setfriendlyname(cObject, (char *)[aName UTF8String]);
}

-(void)setUdn:(NSString *)aUdn
{
	if (!cObject)
		return;
	cg_upnp_device_setudn(cObject, (char *)[aUdn UTF8String]);
}

- (BOOL)isDeviceType:(NSString *)aType
{
	return [aType isEqualToString:[self deviceType]];
}

- (BOOL)isUDN:(NSString *)aUDN
{
	return [aUDN isEqualToString:[self udn]];
}

- (BOOL)isFriendlyName:(NSString *)aFriendlyName
{
	return [aFriendlyName isEqualToString:[self friendlyName]];
}

- (NSArray *)services
{
	if (!cObject)
		return [[[NSArray alloc] init] autorelease];
	NSMutableArray *serviceArray = [[[NSMutableArray alloc] init] autorelease];
	CgUpnpService *cService;
	for (cService = cg_upnp_device_getservices(cObject); cService; cService = cg_upnp_service_next(cService)) {
		CGUpnpService *service = [[[CGUpnpService alloc] initWithCObject:(void *)cService] autorelease];
		[serviceArray addObject:service];
	}
	return serviceArray;
}

- (CGUpnpService *)getServiceForID:(NSString *)serviceId
{
	if (!cObject)
		return nil;
	CgUpnpService *foundService = cg_upnp_device_getservicebyserviceid(cObject, (char *)[serviceId UTF8String]);
	if (!foundService)
		return nil;
	return [[[CGUpnpService alloc] initWithCObject:(void *)foundService] autorelease];
}

- (CGUpnpService *)getServiceForType:(NSString *)serviceType
{
	if (!cObject)
		return nil;
	CgUpnpService *foundService = cg_upnp_device_getservicebytype(cObject, (char *)[serviceType UTF8String]);
	if (!foundService)
		return nil;
	return [[[CGUpnpService alloc] initWithCObject:(void *)foundService] autorelease];
}

- (NSArray *)icons
{
	if (!cObject)
		return [[[NSArray alloc] init] autorelease];
	NSMutableArray *iconArray = [[[NSMutableArray alloc] init] autorelease];
	CgUpnpIcon *cIcon;
	for (cIcon = cg_upnp_device_geticons(cObject); cIcon; cIcon = cg_upnp_icon_next(cIcon)) {
		CGUpnpIcon *icon = [[[CGUpnpIcon alloc] initWithCObject:(void *)cIcon] autorelease];
		[iconArray addObject:icon];
	}
	return iconArray;
}

-(BOOL)start
{
	if (!cObject)
		return NO;
	return cg_upnp_device_start(cObject);
}

-(BOOL)stop
{
	if (!cObject)
		return NO;
	return cg_upnp_device_stop(cObject);
}

/**
 * Announce the device.
 *
 * @return YES if the device is started normally, otherwise NO.
 */
- (BOOL)announce;
{
	if (!cObject)
		return NO;
	cg_upnp_device_announce(cObject);
	return TRUE;
}

- (void)setUserData:(void *)aUserData
{
	if (!cObject)
		return;
	cg_upnp_device_setuserdata(cObject, aUserData);
}

- (void *)userData
{
	if (!cObject)
		return NULL;
	return cg_upnp_device_getuserdata(cObject);
}

- (NSString *)ipaddress
{
	if (!cObject)
		return nil;
	
	NSString *ipaddr = nil;
	
	char *location_str = cg_upnp_device_getlocationfromssdppacket(cObject);	
	if (0 < cg_strlen(location_str)) {
		CgNetURL *url = cg_net_url_new();
		cg_net_url_set(url, location_str);
		ipaddr = [[[NSString alloc] initWithUTF8String:cg_net_url_gethost(url)] autorelease];
		cg_net_url_delete(url);
	}
	
	return ipaddr;
}

- (CGUpnpIcon *)smallestIcon
{
	if (!cObject)
		return nil;
	CgUpnpIcon *cIcon = cg_upnp_device_getsmallesticon(cObject);
	if (!cIcon)
		return nil;
	return [[[CGUpnpIcon alloc] initWithCObject:(void *)cIcon] autorelease];
}

- (CGUpnpIcon *)smallestIconWithMimeType:(NSString *)mimeType;
{
	if (!cObject)
		return nil;
	CgUpnpIcon *cIcon = cg_upnp_device_getsmallesticonbymimetype(cObject, (char *)[mimeType UTF8String]);
	if (!cIcon)
		return nil;
	return [[[CGUpnpIcon alloc] initWithCObject:(void *)cIcon] autorelease];
}

- (NSString *)absoluteIconUrl:(CGUpnpIcon *)anIcon
{
	if (!cObject)
		return nil;
	CgString *url = cg_string_new();
	if (!cg_upnp_device_getabsoluteiconurl(cObject, [anIcon cObject], url)) {
		cg_string_delete(url);
		return nil;
	}
	NSString *urlStr = [NSString stringWithUTF8String:cg_string_getvalue(url)];
	cg_string_delete(url);
	return urlStr;
}

@end