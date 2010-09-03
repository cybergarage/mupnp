//
//  CGUpnpAvServer.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <cybergarage/upnp/std/av/cmediaserver.h>

#import "CGXmlNode.h"
#import "CGUpnpAvObject.h"
#import "CGUpnpAvContainer.h"
#import "CGUpnpAvItem.h"
#import "CGUpnpAvResource.h"
#import "CGUpnpAvContentDirectory.h"
#import "CGUpnpAvServer.h"

#define CGUPNPAVSERVER_BROWSE_RETRY_REQUESTEDCOUNT 999

@implementation CGUpnpAvServer

@synthesize contentDirectory;
@synthesize cAvObject;

- (id) init
{
	if ((self = [super init]) == nil)
		return nil;

	cAvObject = cg_upnpav_dms_new();
	[self setCObject:cg_upnpav_dms_getdevice(cAvObject)];
	
	contentDirectory = nil;

	return self;
}

- (id) initWithCObject:(CgUpnpDevice *)cobj
{
	if ((self = [super initWithCObject:cobj]) == nil)
		return nil;
	
	cAvObject = NULL;
	contentDirectory = [[CGUpnpAvContentDirectory alloc] init];
	
	return self;
}

- (void)setUserObject:(id)aUserObj;
{
	id userObj = [self userObject];
	if (userObj != aUserObj) {
		[userObj release];
		[aUserObj retain];
		[self setUserData:aUserObj];
	}
}

- (id)userObject
{
	void *userData = [self userData];
	if (!userData)
		return nil;
	return (id)userData;
}

- (void)dealloc
{
	id userObj = [self userObject];
	if (userObj)
		[userObj release];
	if (cAvObject)
		cg_upnpav_dms_delete(cAvObject);
	[contentDirectory release];
	[super dealloc];
}

- (void)finalize
{
	id userObj = [self userObject];
	if (userObj)
		[userObj release];
	if (cAvObject)
		cg_upnpav_dms_delete(cAvObject);
	[contentDirectory release];
	[super finalize];
}

- (CGUpnpAvContainer *)rootObject
{
	return [contentDirectory rootObject];
}

- (CGUpnpAvObject *)objectForId:(NSString *)aObjectId
{
	return [contentDirectory objectForId:aObjectId];
}

- (CGUpnpAvObject *)objectForTitlePath:(NSString *)aTitlePath
{
	return [contentDirectory objectForTitlePath:aTitlePath];
}

- (CGUpnpAction *)browseAction
{	
	CGUpnpService *conDirService = [self getServiceForType:@"urn:schemas-upnp-org:service:ContentDirectory:1"];
	if (!conDirService)
		return nil;
	return [conDirService getActionForName:@"Browse"];
}

- (BOOL)browse:(NSString *)aObjectId browseFlag:(NSString *)aBrowseFlag options:(NSDictionary *)options
{	
	CGUpnpAction *action = [self browseAction];
	if (!action)
		return NO;
	
	[action setArgumentValue:aObjectId forName:@"ObjectID"];
	[action setArgumentValue:aBrowseFlag forName:@"BrowseFlag"];
	[action setArgumentValue:@"*" forName:@"Filter"];
	[action setArgumentValue:@"0" forName:@"StartingIndex"];
	[action setArgumentValue:@"0" forName:@"RequestedCount"];
	[action setArgumentValue:@"" forName:@"SortCriteria"];

	for (NSString *argName in options) {
		[action setArgumentValue:[options objectForKey:argName] forName:argName];
	}
	
	return [action post];
}

- (BOOL)browseMetadata:(NSString *)aObjectId
{
	return [self browse:aObjectId browseFlag:@"BrowseMetadata" options:nil];
}

- (NSArray *)browseDirectChildren:(NSString *)aObjectId requestedCount:(NSUInteger)aRequestedCount
{	
	NSMutableDictionary *browseOptions = [NSMutableDictionary dictionary];
	[browseOptions setObject:[NSString stringWithFormat:@"%d", aRequestedCount] forKey:@"RequestedCount"];
	BOOL postResult = [self browse:aObjectId browseFlag:@"BrowseDirectChildren" options:browseOptions];
	
	/*
	 * ContentDirectory:1 Service Template Version 1.01
	 * 2.7.4.2. Argument Descriptions
	 *  RequestedCount ui4 Requested number of entries under the object specified by ObjectID. 
	 *  RequestedCount =0 indicates request all entries.
	 * Added to set the RequestedCount parameter using the NumberReturned result when the specified parameter is zero and
	 * the NumberReturned parameter is less than the TotalMatches parameter for XMBC.
	 */
	
	if (postResult) {
		CGUpnpAction *browseAction = [self browseAction];
		NSInteger numberReturned = [[browseAction argumentValueForName:@"NumberReturned"] integerValue];
		NSInteger totalMatches = [[browseAction argumentValueForName:@"TotalMatches"] integerValue];
		NSLog(@"browseDirectChildren numberReturned = %d", numberReturned);
		NSLog(@"browseDirectChildren totalMatches = %d", totalMatches);
		if (numberReturned == 0) {
			if (0 < totalMatches) {
				NSMutableDictionary *browseOptions = [NSMutableDictionary dictionary];
				[browseOptions setObject:[NSString stringWithFormat:@"%d", totalMatches] forKey:@"RequestedCount"];
				postResult = [self browse:aObjectId browseFlag:@"BrowseDirectChildren" options:browseOptions];
			}
			else {
				browseOptions = [NSMutableDictionary dictionary];
				[browseOptions setObject:[NSString stringWithFormat:@"%d", CGUPNPAVSERVER_BROWSE_RETRY_REQUESTEDCOUNT] forKey:@"RequestedCount"];
				postResult = [self browse:aObjectId browseFlag:@"BrowseDirectChildren" options:browseOptions];
			}
		}
		else if (0 < numberReturned) {
			if (numberReturned < totalMatches) {
				NSMutableDictionary *browseOptions = [NSMutableDictionary dictionary];
				[browseOptions setObject:[NSString stringWithFormat:@"%d", totalMatches] forKey:@"RequestedCount"];
				postResult = [self browse:aObjectId browseFlag:@"BrowseDirectChildren" options:browseOptions];
			}
		}
	}
	else {
		browseOptions = [NSMutableDictionary dictionary];
		[browseOptions setObject:[NSString stringWithFormat:@"%d", CGUPNPAVSERVER_BROWSE_RETRY_REQUESTEDCOUNT] forKey:@"RequestedCount"];
		postResult = [self browse:aObjectId browseFlag:@"BrowseDirectChildren" options:browseOptions];
	}
	
	if (!postResult)
		return nil;
	
	CGUpnpAction *browseAction = [self browseAction];
	NSString *resultStr = [browseAction argumentValueForName:@"Result"];
	NSArray *avObjArray =  [CGUpnpAvObject arrayWithXMLString:resultStr];
	
	/* Update Content Manager */
	CGUpnpAvObject *parentObj = [self objectForId:aObjectId];
	if (parentObj != nil && [parentObj isContainer]) {
		CGUpnpAvContainer *parentCon = (CGUpnpAvContainer *)parentObj;
		[parentCon removeAllChildren];
		[parentCon addChildren:avObjArray];
	}
	
	return avObjArray;	
}

- (NSArray *)browseDirectChildren:(NSString *)aObjectId
{
	return [self browseDirectChildren:aObjectId requestedCount:0];
}

- (NSArray *)search:(NSString *)aSearchCriteria;
{
	CGUpnpService *conDirService = [self getServiceForType:@"urn:schemas-upnp-org:service:ContentDirectory:1"];
	if (!conDirService)
		return nil;
	
	CGUpnpAction *action = [conDirService getActionForName:@"Search"];
	if (!action)
		return nil;
	
	[action setArgumentValue:@"0" forName:@"ContainerID"];
	[action setArgumentValue:aSearchCriteria forName:@"SearchCriteria"];
	[action setArgumentValue:@"*" forName:@"Filter"];
	[action setArgumentValue:@"0" forName:@"StartingIndex"];
	[action setArgumentValue:@"0" forName:@"RequestedCount"];
	[action setArgumentValue:@"" forName:@"SortCriteria"];
	
	if (![action post])
		return nil;
	
	NSString *resultStr = [action argumentValueForName:@"Result"];
	NSArray *avObjArray =  [CGUpnpAvObject arrayWithXMLString:resultStr];
	
	return avObjArray;	
}

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

- (void)lock
{
	if (!cAvObject)
		return;
	cg_upnpav_dms_lock(cAvObject);
}

- (void)unlock
{
	if (!cAvObject)
		return;
	cg_upnpav_dms_unlock(cAvObject);
}

@end
