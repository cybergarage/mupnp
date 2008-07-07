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

- (NSArray *)browse:(CGUpnpAvServer *)device objectId:(NSString *)objectId;
{
	CGUpnpService *conDirService = [device getServiceForType:@"urn:schemas-upnp-org:service:ContentDirectory:1"];
	if (!conDirService)
		return nil;

	CGUpnpAction *browseAction = [conDirService getActionForName:@"Browse"];
	if (!browseAction)
		return nil;

	[browseAction setArgumentValue:objectId forName:@"ObjectID"];
	[browseAction setArgumentValue:@"BrowseDirectChildren" forName:@"BrowseFlag"];
	[browseAction setArgumentValue:@"*" forName:@"Filter"];
	[browseAction setArgumentValue:@"0" forName:@"StartingIndex"];
	[browseAction setArgumentValue:@"0" forName:@"RequestedCount"];
	[browseAction setArgumentValue:@"" forName:@"SortCriteria"];
	
	if (![browseAction post])
		return nil;
	
	NSString *resultStr = [browseAction argumentValueForName:@"Result"];
	
	NSError *xmlErr;
	NSXMLDocument *xmlDoc = [[NSXMLDocument alloc] initWithXMLString:resultStr options:0 error:&xmlErr];
	if (!xmlDoc)
		return nil;

	NSArray *contentArray = [xmlDoc nodesForXPath:@"/DIDL-Lite/*" error:&xmlErr];
	for (NSXMLNode *contentNode in contentArray) {
		/*
		NSString *objId = [[contentNode attributeForName:@"id"] stringValue];
		NSArray *titleArray = [contentNode elementsForName:@"dc:title"];
		NSString *title = @"";
		for (NSXMLNode *titleNode in titleArray) {
			title = [titleNode stringValue];
			break;
		}
		if ([[contentNode name] isEqualToString:@"container"]) {
			NSLog(@"%@  [%@] %@", indentStr, objId, title);
			PrintContentDirectory(browseAction, (indent+1), objId);
		}
		else {
			NSArray *resArray = [contentNode elementsForName:@"res"];
			NSString *url = @"";
			for (NSXMLNode *resNode in resArray) {
				url = [resNode stringValue];
				break;
			}
			NSLog(@"%@  [%@] %@ (%@)", indentStr, objId, title, url);
		}
		*/
	}

	return nil;
}

@end
