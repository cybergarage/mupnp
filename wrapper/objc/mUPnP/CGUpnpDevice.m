/******************************************************************
 *
 * mUPnP for ObjC
 *
 * Copyright (C) Satoshi Konno 2008
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#include <mupnp/device.h>
#include <mupnp/service.h>
#include <mupnp/icon.h>

#import "CGUpnpDevice.h"
#import "CGUpnpService.h"
#import "CGUpnpAction.h"
#import "CGUpnpIcon.h"

static BOOL CGUpnpDeviceActionListener(mUpnpAction *action);

@implementation CGUpnpDevice

@synthesize cObject;
@synthesize delegate;

- (id) init
{
	if ((self = [super init]) == nil)
		return nil;
	cObject = mupnp_device_new();
	if (!cObject)
		return nil;
	isCObjectCreated = YES;
	mupnp_device_setuserdata(cObject, self);
	mupnp_device_setactionlistener(cObject, CGUpnpDeviceActionListener);
	return self;
}

- (id) initWithCObject:(mUpnpDevice *)cobj
{
	if ((self = [super init]) == nil)
		return nil;
	cObject = cobj;
	isCObjectCreated = NO;
	return self;
}

- (id) initWithXMLDescription:(NSString *)xmlDesc
{
	if ((self = [self init]) == nil)
		return nil;
	if (!cObject)
		return nil;
	if (![self parseXMLDescription:xmlDesc]) {
		mupnp_device_delete(cObject);
		return nil;
	}
	return self;
}

- (BOOL) parseXMLDescription:(NSString *)xmlDesc;
{
	if (!cObject)
		return NO;
	return mupnp_device_parsedescription(cObject, (char *)[xmlDesc UTF8String], [xmlDesc length]);
}

- (void) dealloc
{
	if (isCObjectCreated && cObject) {
		mupnp_device_delete(cObject);
		cObject = NULL;
	}
	[super dealloc];
}

- (NSString *)friendlyName
{
	if (!cObject)
		return nil;
	const char* name = mupnp_device_getfriendlyname(cObject);
	if (name) {
		return [NSString stringWithUTF8String:name];
	}
	return nil;
}

- (NSString *)deviceType
{
	if (!cObject)
		return nil;
	const char* device_type_cstring = mupnp_device_getdevicetype(cObject);
	if (device_type_cstring)
	{
		return [NSString stringWithUTF8String:device_type_cstring];
	}
	return nil;
}

- (NSString *)udn
{
	if (!cObject)
		return nil;
	const char* udn_cstring = mupnp_device_getudn(cObject);
	if (udn_cstring)
	{
		return [NSString stringWithUTF8String:udn_cstring];
	}
	return nil;
}

- (NSString *)manufacturer
{
	if (!cObject)
		return nil;
	const char* manufacturer_cstring = mupnp_device_getmanufacturer(cObject);
	if (manufacturer_cstring)
	{
		return [NSString stringWithUTF8String:manufacturer_cstring];
	}
	return nil;
}

- (NSString *)modelNumber
{
	if (!cObject)
		return nil;
	const char* model_number_cstring = mupnp_device_getmodelnumber(cObject);
	if (model_number_cstring)
	{
		return [NSString stringWithUTF8String:model_number_cstring];
	}
	return nil;
}

- (NSString *)modelName
{
	if (!cObject)
		return nil;
	const char* model_name_cstring = mupnp_device_getmodelname(cObject);
	if (model_name_cstring)
	{
		return [NSString stringWithUTF8String:model_name_cstring];
	}
	return nil;
}

- (NSString *)urlBase
{
	if (!cObject)
		return nil;
	const char *cUrlBase = mupnp_device_geturlbase(cObject);
	if (!cUrlBase)
		return [NSString string];
	return [[[NSString alloc] initWithUTF8String:cUrlBase] autorelease];
}

- (NSString *)descriptionURL;
{
	if (!cObject)
		return nil;
	const char *cDescrUrl = mupnp_device_getdescriptionuri(cObject);
	if (!cDescrUrl)
		return [NSString string];
	return [[[NSString alloc] initWithUTF8String:cDescrUrl] autorelease];
}

- (NSString *)locationURL;
{
	if (!cObject)
		return nil;
	const char *cLocationUrl = mupnp_device_getlocationfromssdppacket(cObject);
	if (!cLocationUrl)
		return [NSString string];
	return [[[NSString alloc] initWithUTF8String:cLocationUrl] autorelease];
}

- (NSString *)presentationURL;
{
	if (!cObject)
		return nil;
	const char *cPresentatilnURL = mupnp_device_getpresentationurl(cObject);
	if (!cPresentatilnURL)
		return [NSString string];
	return [[[NSString alloc] initWithUTF8String:cPresentatilnURL] autorelease];
}

-(void)setDeviceType:(NSString *)aType
{
	if (!cObject)
		return;
	mupnp_device_setdevicetype(cObject, (char *)[aType UTF8String]);
}

-(void)setFriendlyName:(NSString *)aName
{
	if (!cObject)
		return;
	mupnp_device_setfriendlyname(cObject, (char *)[aName UTF8String]);
}

-(void)setUdn:(NSString *)aUdn
{
	if (!cObject)
		return;
	mupnp_device_setudn(cObject, (char *)[aUdn UTF8String]);
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
		return [NSArray array];
	NSMutableArray *serviceArray = [NSMutableArray array];
	mUpnpService *cService;
	for (cService = mupnp_device_getservices(cObject); cService; cService = mupnp_service_next(cService)) {
		CGUpnpService *service = [[[CGUpnpService alloc] initWithCObject:(void *)cService] autorelease];
		[serviceArray addObject:service];
	}
	return serviceArray;
}

- (CGUpnpService *)getServiceForID:(NSString *)serviceId
{
	if (!cObject)
		return nil;
	mUpnpService *foundService = mupnp_device_getservicebyserviceid(cObject, (char *)[serviceId UTF8String]);
	if (!foundService)
		return nil;
	return [[[CGUpnpService alloc] initWithCObject:(void *)foundService] autorelease];
}

- (CGUpnpService *)getServiceForType:(NSString *)serviceType
{
	if (!cObject)
		return nil;
	mUpnpService *foundService = mupnp_device_getservicebytype(cObject, (char *)[serviceType UTF8String]);
	if (!foundService)
		return nil;
	return [[[CGUpnpService alloc] initWithCObject:(void *)foundService] autorelease];
}

- (NSArray *)icons
{
	if (!cObject)
		return [NSArray array];
	NSMutableArray *iconArray =[NSMutableArray array];
	mUpnpIcon *cIcon;
	for (cIcon = mupnp_device_geticons(cObject); cIcon; cIcon = mupnp_icon_next(cIcon)) {
		CGUpnpIcon *icon = [[CGUpnpIcon alloc] initWithCObject:(void *)cIcon];
		[iconArray addObject:icon];
        [icon release];
	}
	return iconArray;
}

-(BOOL)start
{
	if (!cObject)
		return NO;
	return mupnp_device_start(cObject);
}

-(BOOL)stop
{
	if (!cObject)
		return NO;
	return mupnp_device_stop(cObject);
}

-(BOOL)isRunning
{
	if (!cObject)
		return NO;
	return mupnp_device_isrunning(cObject);
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
	mupnp_device_announce(cObject);
	return TRUE;
}

- (void)setUserData:(void *)aUserData
{
	if (!cObject)
		return;
	mupnp_device_setuserdata(cObject, aUserData);
}

- (void *)userData
{
	if (!cObject)
		return NULL;
	return mupnp_device_getuserdata(cObject);
}

- (NSString *)ipaddress
{
	if (!cObject)
		return nil;
	
	NSString *ipaddr = nil;
	
	char *location_str = (char*)mupnp_device_getlocationfromssdppacket(cObject);
	if (0 < mupnp_strlen(location_str)) {
		mUpnpNetURL *url = mupnp_net_url_new();
		mupnp_net_url_set(url, location_str);
		ipaddr = [[[NSString alloc] initWithUTF8String:mupnp_net_url_gethost(url)] autorelease];
		mupnp_net_url_delete(url);
	}
	
	return ipaddr;
}

- (CGUpnpIcon *)smallestIcon
{
	if (!cObject)
		return nil;
	mUpnpIcon *cIcon = mupnp_device_getsmallesticon(cObject);
	if (!cIcon)
		return nil;
	return [[[CGUpnpIcon alloc] initWithCObject:(void *)cIcon] autorelease];
}

- (CGUpnpIcon *)smallestIconWithMimeType:(NSString *)mimeType;
{
	if (!cObject)
		return nil;
	mUpnpIcon *cIcon = mupnp_device_getsmallesticonbymimetype(cObject, (char *)[mimeType UTF8String]);
	if (!cIcon)
		return nil;
	return [[[CGUpnpIcon alloc] initWithCObject:(void *)cIcon] autorelease];
}

- (NSString *)absoluteIconUrl:(CGUpnpIcon *)anIcon
{
	if (!cObject)
		return nil;
	mUpnpString *url = mupnp_string_new();
	if (!mupnp_device_getabsoluteiconurl(cObject, [anIcon cObject], url)) {
		mupnp_string_delete(url);
		return nil;
	}
	NSString *urlStr = [NSString stringWithUTF8String:mupnp_string_getvalue(url)];
	mupnp_string_delete(url);
	return urlStr;
}

@end

static BOOL CGUpnpDeviceActionListener(mUpnpAction *cUpnpAction)
{
	if (!cUpnpAction)
		return NO;
	
	mUpnpService *cUpnpService = mupnp_action_getservice(cUpnpAction);
	if (!cUpnpService)
		return NO;
	
	mUpnpDevice *cUpnpDevice = mupnp_service_getdevice(cUpnpService);
	if (!cUpnpDevice)
		return NO;
	
	CGUpnpDevice *upnpDevice = mupnp_device_getuserdata(cUpnpDevice);
	if (upnpDevice == nil)
		return NO;
	
	if ([[upnpDevice delegate] respondsToSelector:@selector(device:service:actionReceived:)]) {
		CGUpnpService *upnpService = [[CGUpnpService alloc] initWithCObject:(void *)cUpnpService];
		CGUpnpAction *upnpAction = [[CGUpnpAction alloc] initWithCObject:(void *)upnpAction];
		BOOL doActionResult = [[upnpDevice delegate] device:upnpDevice service:upnpService actionReceived:upnpAction];
		[upnpAction release];
		[upnpService release];
		return doActionResult;
	}
		
	return YES;
}

