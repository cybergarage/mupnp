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

@implementation CGUpnpAvServer

@synthesize contentDirectory;
@synthesize cAvObject;

- (id) init
{
	if ((self = [super init]) == nil)
		return nil;

	cAvObject = cg_upnp_dms_new();
	[self setCObject:cg_upnp_dms_getdevice(cAvObject)];
	
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
		cg_upnp_dms_delete(cAvObject);
	[contentDirectory release];
	[super dealloc];
}

- (void)finalize
{
	id userObj = [self userObject];
	if (userObj)
		[userObj release];
	if (cAvObject)
		cg_upnp_dms_delete(cAvObject);
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

- (NSArray *)browse:(NSString *)aObjectId withRequestedCount:(NSUInteger) aRequestedCount
{
	CGUpnpService *conDirService = [self getServiceForType:@"urn:schemas-upnp-org:service:ContentDirectory:1"];
	if (!conDirService)
		return nil;

	CGUpnpAction *action = [conDirService getActionForName:@"Browse"];
	if (!action)
		return nil;

	[action setArgumentValue:aObjectId forName:@"ObjectID"];
	[action setArgumentValue:@"BrowseDirectChildren" forName:@"BrowseFlag"];
	[action setArgumentValue:@"*" forName:@"Filter"];
	[action setArgumentValue:@"0" forName:@"StartingIndex"];
	[action setArgumentValue:[NSString stringWithFormat:@"%d", aRequestedCount] forName:@"RequestedCount"];
	[action setArgumentValue:@"" forName:@"SortCriteria"];
	
	if (![action post])
		return nil;

	if (aRequestedCount == 0) {
		NSInteger numberReturened = [[action argumentValueForName:@"NumberReturened"] integerValue];
		NSInteger totalMaches = [[action argumentValueForName:@"TotalMaches"] integerValue];
		if ((numberReturened == 0) && (0 < totalMaches)) {
			[action setArgumentValue:[NSString stringWithFormat:@"%d", totalMaches] forName:@"RequestedCount"];
			if (![action post])
				return nil;
		}
	}
	
	NSString *resultStr = [action argumentValueForName:@"Result"];
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

- (NSArray *)browse:(NSString *)aObjectId;
{
	return [self browse:aObjectId withRequestedCount:@"0"];
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
	return cg_upnp_dms_start(cAvObject);
}

- (BOOL)stop
{
	if (!cAvObject)
		return NO;
	return cg_upnp_dms_stop(cAvObject);
}

@end
