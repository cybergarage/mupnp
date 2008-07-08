//
//  CGUpnpAvServer.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import <CyberLink/UPnP.h>
#import <CGUpnpAvServer.h>

@implementation CGUpnpAvServer

@synthesize contentDirectory;

- (id) init
{
	if ((self = [super init]) == nil)
		return nil;
	contentDirectory = [[CGUpnpAvContentDirectory alloc] init];
	return self;
}

- (id) initWithCObject:(CgUpnpDevice *)cobj
{
	if ((self = [super initWithCObject:cobj]) == nil)
		return nil;
	contentDirectory = [[CGUpnpAvContentDirectory alloc] init];
	return self;
}

- (void)dealloc
{
	[contentDirectory release];
	[super dealloc];
}

- (void)finalize
{
	[contentDirectory release];
	[super finalize];
}

- (NSArray *)browse:(NSString *)objectId;
{
	CGUpnpService *conDirService = [self getServiceForType:@"urn:schemas-upnp-org:service:ContentDirectory:1"];
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

	NSMutableArray *avObjArray = [[[NSMutableArray alloc] init] autorelease];
	
	NSArray *contentArray = [xmlDoc nodesForXPath:@"/DIDL-Lite/*" error:&xmlErr];
	for (NSXMLNode *contentNode in contentArray) {
		NSString *objId = [[contentNode attributeForName:@"id"] stringValue];
		NSArray *titleArray = [contentNode elementsForName:@"dc:title"];
		NSString *title = @"";
		for (NSXMLNode *titleNode in titleArray) {
			title = [titleNode stringValue];
			break;
		}
		if ([objId length] <= 0 || [title length] <= 0)
			continue;
		CGUpnpAvObject *avObj = nil;
		if ([[contentNode name] isEqualToString:@"container"]) {
			CGUpnpAvContainer *avCon = [[[CGUpnpAvContainer alloc] init] autorelease];
			avObj = avCon;
		}
		else {
			CGUpnpAvItem *avItem = [[[CGUpnpAvItem alloc] init] autorelease];
			NSArray *resArray = [contentNode elementsForName:@"res"];
			for (NSXMLNode *resNode in resArray) {
				NSString *url = [resNode stringValue];
				CGUpnpAvResource *avRes = [[[CGUpnpAvResource alloc] init] autorelease];
				[avRes setUrl:url];
				[avItem addResource:avRes];
			}
			avObj = avItem;
		}
		if (avObj == nil)
			continue;
		[avObjArray addObject:avObj];
	}

	return avObjArray;
}

@end
