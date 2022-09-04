//
//  CGUpnpAvServer.m
//  mUPnP for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <cybergarage/upnp/std/av/cmediaserver.h>

#import "CGUpnpAvContainer.h"
#import "CGUpnpAvContentDirectory.h"
#import "CGUpnpAvItem.h"
#import "CGUpnpAvObject.h"
#import "CGUpnpAvResource.h"
#import "CGUpnpAvServer.h"
#import "CGXmlNode.h"

#define CGUPNPAVSERVER_BROWSE_RETRY_REQUESTEDCOUNT 999

//#define CGUPNPAVSERVER_ENABLED_USEROBJECT_RETAIN 1

@implementation CGUpnpAvServer

@synthesize contentDirectory;
@synthesize cAvObject;
@synthesize delegateServer;

#if defined(TARGET_OS_IPHONE)
@synthesize thumbnailImage;
#endif

- (id)init
{
  if ((self = [super init]) == nil)
    return nil;

  cAvObject = cg_upnpav_dms_new();
  [self setCObject:cg_upnpav_dms_getdevice(cAvObject)];

  self.contentDirectory = nil;

  return self;
}

- (id)initWithCObject:(CgUpnpDevice*)cobj
{
  if ((self = [super initWithCObject:cobj]) == nil)
    return nil;

  cAvObject = NULL;
  self.contentDirectory = [[[CGUpnpAvContentDirectory alloc] init] autorelease];

  return self;
}

- (void)setUserObject:(id)aUserObj;
{
  id userObj = [self userObject];
  if (userObj != nil) {
#if defined(CGUPNPAVSERVER_ENABLED_USEROBJECT_RETAIN)
    [userObj release];
#endif
    [self setUserData:NULL];
  }
  if (aUserObj != nil) {
#if defined(CGUPNPAVSERVER_ENABLED_USEROBJECT_RETAIN)
    [aUserObj retain];
#endif
    [self setUserData:aUserObj];
  }
}

- (id)userObject
{
  void* userData = [self userData];
  if (!userData)
    return nil;
  return (id)userData;
}

- (void)dealloc
{
  self.contentDirectory = nil;
#if defined(TARGET_OS_IPHONE)
  self.thumbnailImage = nil;
#endif

  id userObj = [self userObject];
  if (userObj != nil) {
#if defined(CGUPNPAVSERVER_ENABLED_USEROBJECT_RETAIN)
    [userObj release];
#endif
    [self setUserData:NULL];
  }

  if (cAvObject)
    cg_upnpav_dms_delete(cAvObject);

  [super dealloc];
}

- (CGUpnpAvContainer*)rootObject
{
  return [contentDirectory rootObject];
}

- (CGUpnpAvObject*)objectForId:(NSString*)aObjectId
{
  return [contentDirectory objectForId:aObjectId];
}

- (CGUpnpAvObject*)objectForTitlePath:(NSString*)aTitlePath
{
  return [contentDirectory objectForTitlePath:aTitlePath];
}

- (CGUpnpAction*)browseAction
{
  CGUpnpService* conDirService = [self getServiceForType:@"urn:schemas-upnp-org:service:ContentDirectory:1"];
  if (!conDirService)
    return nil;
  return [conDirService getActionForName:@"Browse"];
}

- (BOOL)browse:(NSString*)aObjectId browseFlag:(NSString*)aBrowseFlag options:(NSDictionary*)options
{
  CGUpnpAction* action = [self browseAction];
  if (!action)
    return NO;

  [action setArgumentValue:aObjectId forName:@"ObjectID"];
  [action setArgumentValue:aBrowseFlag forName:@"BrowseFlag"];
  [action setArgumentValue:@"*" forName:@"Filter"];
  [action setArgumentValue:@"0" forName:@"StartingIndex"];
  [action setArgumentValue:@"0" forName:@"RequestedCount"];
  [action setArgumentValue:@"" forName:@"SortCriteria"];

  for (NSString* argName in options) {
    [action setArgumentValue:[options objectForKey:argName] forName:argName];
  }

  return [action post];
}

- (CGUpnpAvObject*)browseMetadata:(NSString*)aObjectId
{
  BOOL postResult = [self browse:aObjectId browseFlag:@"BrowseMetadata" options:nil];
  if (!postResult)
    return nil;

  CGUpnpAction* browseAction = [self browseAction];
  NSString* resultStr = [browseAction argumentValueForName:@"Result"];
  NSArray* avObjArray = [CGUpnpAvObject arrayWithXMLString:resultStr];

  if ([avObjArray count] <= 0)
    return nil;

  return (CGUpnpAvObject*)[avObjArray objectAtIndex:0];
}

- (NSUInteger)browseDirectChildrenTotalMatches:(NSString*)aObjectId
{
  if (![self browse:aObjectId browseFlag:@"BrowseDirectChildren" options:[NSDictionary dictionary]])
    return 0;
  return [[[self browseAction] argumentValueForName:@"TotalMatches"] integerValue];
}

- (NSArray*)browseDirectChildren:(NSString*)aObjectId requestedCount:(NSUInteger)aRequestedCount startingIndex:(NSUInteger)aStartingIndex
{
  NSMutableDictionary* browseOptions = [NSMutableDictionary dictionary];
  [browseOptions setObject:[NSString stringWithFormat:@"%tu", aRequestedCount] forKey:@"RequestedCount"];
  [browseOptions setObject:[NSString stringWithFormat:@"%tu", aStartingIndex] forKey:@"StartingIndex"];
  BOOL postResult = [self browse:aObjectId browseFlag:@"BrowseDirectChildren" options:browseOptions];

#if defined(CG_UPNPAVSERVER_BROWSEACTION_RETRY_ENABLED)
  /*
	 * ContentDirectory:1 Service Template Version 1.01
	 * 2.7.4.2. Argument Descriptions
	 *  RequestedCount ui4 Requested number of entries under the object specified by ObjectID. 
	 *  RequestedCount =0 indicates request all entries.
	 * Added to set the RequestedCount parameter using the NumberReturned result when the specified parameter is zero and
	 * the NumberReturned parameter is less than the TotalMatches parameter for XMBC.
	 */

  if (postResult) {
    CGUpnpAction* browseAction = [self browseAction];
    NSInteger numberReturned = [[browseAction argumentValueForName:@"NumberReturned"] integerValue];
    NSInteger totalMatches = [[browseAction argumentValueForName:@"TotalMatches"] integerValue];
    NSLog(@"browseDirectChildren [%@] numberReturned = %d, totalMatches = %d", aObjectId, numberReturned, totalMatches);
    if (numberReturned == 0) {
      if (0 < totalMatches) {
        NSMutableDictionary* browseOptions = [NSMutableDictionary dictionary];
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
        NSMutableDictionary* browseOptions = [NSMutableDictionary dictionary];
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
#endif

  if (!postResult)
    return nil;

  CGUpnpAction* browseAction = [self browseAction];
  NSString* resultStr = [browseAction argumentValueForName:@"Result"];
  NSArray* avObjArray = [CGUpnpAvObject arrayWithXMLString:resultStr];

  /* Update Content Manager */
  CGUpnpAvObject* parentObj = [self objectForId:aObjectId];
  if (parentObj != nil && [parentObj isContainer]) {
    CGUpnpAvContainer* parentCon = (CGUpnpAvContainer*)parentObj;
    [parentCon removeAllChildren];
    [parentCon addChildren:avObjArray];
  }

  /* CGUpnpAvServerDelegate */
  if ([[self delegateServer] respondsToSelector:@selector(upnpAvServer:browse:avObject:)]) {
    for (CGUpnpAvObject* avObj in avObjArray)
      [[self delegateServer] upnpAvServer:self browse:browseAction avObject:avObj];
  }

  return avObjArray;
}

- (NSArray*)browseDirectChildren:(NSString*)aObjectId requestedCount:(NSUInteger)aRequestedCount
{
  return [self browseDirectChildren:aObjectId requestedCount:aRequestedCount startingIndex:0];
}

- (NSArray*)browseDirectChildren:(NSString*)aObjectId
{
#if !defined(CG_UPNPAVSERVER_BROWSEACTION_CHECK_TOTALMATCHES)
  return [self browseDirectChildren:aObjectId requestedCount:0];
#else
  NSUInteger totalMatches = [self browseDirectChildrenTotalMatches:aObjectId];
  if (totalMatches <= 0)
    return [self browseDirectChildren:aObjectId requestedCount:0];

  NSMutableArray* avObjArray = [NSMutableArray array];
  while ([avObjArray count] < totalMatches) {
    int startingIndex = [avObjArray count];
    NSArray* avObjs = [self browseDirectChildren:aObjectId requestedCount:0 startingIndex:startingIndex];
    if (avObjs == nil || [avObjs count] <= 0)
      break;
    [avObjArray addObjectsFromArray:avObjs];
  }
  return avObjArray;
#endif
}

- (CGUpnpAction*)searchAction
{
  CGUpnpService* conDirService = [self getServiceForType:@"urn:schemas-upnp-org:service:ContentDirectory:1"];
  if (!conDirService)
    return nil;

  return [conDirService getActionForName:@"Search"];
}

- (BOOL)hasSearchAvObjectDelegate
{
  return ([[self delegateServer] respondsToSelector:@selector(upnpAvServer:search:avObject:)]);
}

- (NSArray*)search:(NSString*)aSearchCriteria;
{
  CGUpnpAction* action = [self searchAction];
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

  NSString* resultStr = [action argumentValueForName:@"Result"];
  NSArray* avObjArray = [CGUpnpAvObject arrayWithXMLString:resultStr];

  /* CGUpnpAvServerDelegate */
  if ([self hasSearchAvObjectDelegate]) {
    for (CGUpnpAvObject* avObj in avObjArray)
      [[self delegateServer] upnpAvServer:self search:action avObject:avObj];
  }

  return avObjArray;
}

- (BOOL)hasAvObject:(NSArray*)avObjs objectId:(NSString*)objectId
{
  if (objectId == nil)
    return NO;

  for (CGUpnpAvObject* avObj in avObjs) {
    NSString* avObjectId = [avObj objectId];
    if ([objectId isEqualToString:avObjectId])
      return YES;
  }

  return NO;
}

- (BOOL)hasAvObject:(NSArray*)avObjs avObject:(CGUpnpAvObject*)targetAvObj
{
  if (targetAvObj == nil)
    return NO;

  NSString* targetAvObjectId = [targetAvObj objectId];
  if (targetAvObjectId == nil || [targetAvObjectId length] <= 0)
    return NO;

  for (CGUpnpAvObject* avObj in avObjs) {
    if ([targetAvObjectId isEqualToString:[avObj objectId]])
      return YES;
  }

  return NO;
}

- (void)addAvObjectsFromArray:(NSMutableArray*)toAvObjs newAvObjs:(NSArray*)newAvObjs
{
  for (CGUpnpAvObject* avObj in newAvObjs) {
    // TODO FIX Memory Leaks
    /*
        NSString *objectId = [avObj objectId];
		if ([self hasAvObject:toAvObjs objectId:objectId])
			 continue;
        */
    [toAvObjs addObject:avObj];
  }
}

#if defined(CGUPNPAV_USE_DEPTHFIRSTSEARCH)
- (NSArray*)searchByBrowse:(NSString*)aSearchCriteria objectId:(NSString*)objectId
{
  NSMutableArray* avObjs = [NSMutableArray array];

  NSArray* browseAvObjs = [self browseDirectChildren:objectId];
  for (CGUpnpAvObject* avObj in browseAvObjs) {
    if ([avObj isItem]) {
      [avObjs addObject:avObj];
    }
  }

  for (CGUpnpAvObject* avObj in browseAvObjs) {
    if ([avObj isContainer] == NO)
      continue;
    [self addAvObjectsFromArray:avObjs newAvObjs:[self searchByBrowse:aSearchCriteria objectId:[avObj objectId]]];
  }

  return avObjs;
}
#else
- (NSArray*)searchByBrowse:(NSString*)aSearchCriteria objectId:(NSString*)objectId
{
  NSMutableArray* avObjs = [NSMutableArray array];
  NSMutableArray* searchObjectIds = [NSMutableArray array];

  [searchObjectIds addObject:objectId];

  while (0 < [searchObjectIds count]) {

    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

    NSString* searchObjectId = [searchObjectIds objectAtIndex:0];
    NSArray* childAvObjs = [self browseDirectChildren:searchObjectId];

    [searchObjectIds removeObjectAtIndex:0];

    for (CGUpnpAvObject* childAvObj in childAvObjs) {
      if ([childAvObj isContainer]) {
        [searchObjectIds addObject:[childAvObj objectId]];
        continue;
      }
      if ([self hasSearchAvObjectDelegate] == NO) {
        if ([childAvObj isItem]) {
          [avObjs addObject:childAvObj];
          continue;
        }
      }
    }

    [pool drain];
  }

  return avObjs;
}
#endif

- (NSArray*)searchByBrowse:(NSString*)aSearchCriteria
{
  NSMutableArray* avObjs = [NSMutableArray array];
  [self addAvObjectsFromArray:avObjs newAvObjs:[self searchByBrowse:aSearchCriteria objectId:@"0"]];
  return avObjs;
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
