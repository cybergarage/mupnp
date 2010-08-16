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

#define CGUPNPAVSERVER_BROWSE_RETRY_REQUESTEDCOUNT 9999

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

- (NSArray *)browse:(NSString *)aObjectId browseFlag:(NSString *)aBrowseFlag withRequestedCount:(NSUInteger) aRequestedCount
{	
	CGUpnpService *conDirService = [self getServiceForType:@"urn:schemas-upnp-org:service:ContentDirectory:1"];
	if (!conDirService)
		return nil;

	CGUpnpAction *action = [conDirService getActionForName:@"Browse"];
	if (!action)
		return nil;

	[action setArgumentValue:aObjectId forName:@"ObjectID"];
	[action setArgumentValue:aBrowseFlag forName:@"BrowseFlag"];
	[action setArgumentValue:@"*" forName:@"Filter"];
	[action setArgumentValue:@"0" forName:@"StartingIndex"];
	[action setArgumentValue:[NSString stringWithFormat:@"%d", aRequestedCount] forName:@"RequestedCount"];
	[action setArgumentValue:@"" forName:@"SortCriteria"];
	
	if (![action post])
		return nil;

	/*
	 * ContentDirectory:1 Service Template Version 1.01
	 * 2.7.4.2. Argument Descriptions
	 *  RequestedCount ui4 Requested number of entries under the object specified by ObjectID. 
	 *  RequestedCount =0 indicates request all entries.
	 * Added to set the RequestedCount parameter using the NumberReturned result when the specified parameter is zero and
	 * the NumberReturned parameter is less than the TotalMatches parameter for XMBC.
	*/
	if ([aBrowseFlag isEqualToString:@"BrowseDirectChildren"]) {
		if (aRequestedCount == 0) {
			NSInteger numberReturned = [[action argumentValueForName:@"NumberReturned"] integerValue];
			NSInteger totalMatches = [[action argumentValueForName:@"TotalMatches"] integerValue];
			if (numberReturned == 0) {
				if (0 < totalMatches) {
					[action setArgumentValue:[NSString stringWithFormat:@"%d", totalMatches] forName:@"RequestedCount"];
					if (![action post])
						return nil;
				}
				else {
					[action setArgumentValue:[NSString stringWithFormat:@"%d", CGUPNPAVSERVER_BROWSE_RETRY_REQUESTEDCOUNT] forName:@"RequestedCount"];
					if (![action post])
						return nil;
				}
			}
			else if (0 < numberReturned) {
				if (numberReturned < totalMatches) {
					[action setArgumentValue:[NSString stringWithFormat:@"%d", totalMatches] forName:@"RequestedCount"];
					if (![action post])
						return nil;
				}
			}
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

- (NSArray *)browse:(NSString *)aObjectId browseFlag:(NSString *)aBrowseFlag 
{
	return [self browse:aObjectId browseFlag:aBrowseFlag withRequestedCount:0];
}

- (CGUpnpAvObject *)browseMetadata:(NSString *)aObjectId
{
	NSArray *avObjs = [self browse:aObjectId browseFlag:@"BrowseMetadata" withRequestedCount:0];
	if ([avObjs count] <= 0)
		return nil;
	id avObj = [avObjs objectAtIndex:0];
	if (![avObj isKindOfClass:[CGUpnpAvObject class]])
		return nil;
	return (CGUpnpAvObject *)avObj;		
}

- (NSArray *)browseDirectChildren:(NSString *)aObjectId
{
	return [self browseDirectChildren:aObjectId withRequestedCount:0];
}

- (NSArray *)browseDirectChildren:(NSString *)aObjectId withRequestedCount:(NSUInteger) aRequestedCount
{
#if defined(CG_CLINKAV_OBJC_USE_BROWSDIRECTMETADATA)
	NSLog(@"browseDirectChildren = %@ (%d)", aObjectId, aRequestedCount);
	if (aRequestedCount == 0) {
		CGUpnpAvObject *browseAvObj = [self browseMetadata:aObjectId];
		if (browseAvObj != nil)
			aRequestedCount = [browseAvObj childCount];
		NSLog(@"browseMetadata = %@ (%d)", aObjectId, aRequestedCount);
	}
#endif
	NSArray*resultArray = [self browse:aObjectId browseFlag:@"BrowseDirectChildren" withRequestedCount:aRequestedCount];
	if (resultArray != nil && 0 < [resultArray count])
		return resultArray;
	if (0 < aRequestedCount)
		return resultArray;
	return [self browse:aObjectId browseFlag:@"BrowseDirectChildren" withRequestedCount:CGUPNPAVSERVER_BROWSE_RETRY_REQUESTEDCOUNT];
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

@end
