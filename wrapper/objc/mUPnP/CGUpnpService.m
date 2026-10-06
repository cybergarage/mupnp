/******************************************************************
 *
 * mUPnP for ObjC
 *
 * Copyright (C) Satoshi Konno 2008
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#include <mupnp/action.h>
#include <mupnp/service.h>
#include <mupnp/statevariable.h>

#import <Foundation/NSData.h>

#import "CGUpnpAction.h"
#import "CGUpnpService.h"
#import "CGUpnpStateVariable.h"

/* Returns the UTF-8 bytes of a description string, or nil for nil, empty,
 * non-string or non-UTF-8-convertible input. The byte length (not the UTF-16
 * NSString length) is what the native XML parser expects. */
static NSData* cg_upnp_service_description_bytes(NSString* xmlDesc)
{
  if (![xmlDesc isKindOfClass:[NSString class]])
    return nil;
  NSData* bytes = [xmlDesc dataUsingEncoding:NSUTF8StringEncoding allowLossyConversion:NO];
  if ([bytes length] == 0)
    return nil;
  return bytes;
}

@implementation CGUpnpService

@synthesize cObject;
@synthesize cObjectOwner;

- (id)initWithCObject:(mUpnpService*)cobj
{
  if ((self = [super init]) == nil)
    return nil;
  cObject = cobj;

  return self;
}

- (id)init
{
  if ((self = [self initWithCObject:NULL]) == nil)
    return nil;
  return self;
}

- (void)dealloc
{
  [cObjectOwner release];
  [super dealloc];
}

- (BOOL)parseXMLDescription:(NSString*)xmlDesc;
{
  if (!cObject)
    return NO;
  NSData* bytes = cg_upnp_service_description_bytes(xmlDesc);
  if (!bytes)
    return NO;
  return mupnp_service_parsedescription(cObject, (const char*)[bytes bytes], [bytes length]);
}

- (NSString*)serviceId
{
  if (!cObject)
    return nil;
  return [[[NSString alloc] initWithUTF8String:mupnp_service_getserviceid(cObject)] autorelease];
}

- (NSString*)serviceType
{
  if (!cObject)
    return nil;
  return [[[NSString alloc] initWithUTF8String:mupnp_service_getservicetype(cObject)] autorelease];
}

- (NSArray*)actions
{
  if (!cObject)
    return [[[NSArray alloc] init] autorelease];
  NSMutableArray* actionArray = [[[NSMutableArray alloc] init] autorelease];
  mUpnpAction* cAction;
  for (cAction = mupnp_service_getactions(cObject); cAction; cAction = mupnp_action_next(cAction)) {
    CGUpnpAction* action = [[[CGUpnpAction alloc] initWithCObject:(void*)cAction] autorelease];
    action.cObjectOwner = self;
    [actionArray addObject:action];
  }
  return actionArray;
}

- (NSArray*)stateVariables
{
  if (!cObject)
    return [[[NSArray alloc] init] autorelease];
  NSMutableArray* statVarArray = [[[NSMutableArray alloc] init] autorelease];
  mUpnpStateVariable* cStatVar;
  for (cStatVar = mupnp_service_getstatevariables(cObject); cStatVar; cStatVar = mupnp_statevariable_next(cStatVar)) {
    CGUpnpStateVariable* statVar = [[[CGUpnpStateVariable alloc] initWithCObject:(void*)cStatVar] autorelease];
    statVar.cObjectOwner = self;
    [statVarArray addObject:statVar];
  }
  return statVarArray;
}

- (CGUpnpAction*)getActionForName:(NSString*)name
{
  if (!cObject)
    return nil;
  mUpnpAction* cAction = mupnp_service_getactionbyname(cObject, (char*)[name UTF8String]);
  if (!cAction)
    return nil;
  CGUpnpAction* action = [[[CGUpnpAction alloc] initWithCObject:(void*)cAction] autorelease];
  action.cObjectOwner = self;
  return action;
}

- (CGUpnpStateVariable*)getStateVariableForName:(NSString*)name
{
  if (!cObject)
    return nil;
  mUpnpStateVariable* cStatVar = mupnp_service_getstatevariablebyname(cObject, (char*)[name UTF8String]);
  if (!cStatVar)
    return nil;
  CGUpnpStateVariable* variable = [[[CGUpnpStateVariable alloc] initWithCObject:(void*)cStatVar] autorelease];
  variable.cObjectOwner = self;
  return variable;
}

- (BOOL)isStateVariableImpemented:(NSString*)name;
{
  CGUpnpStateVariable* stateVariable = [self getStateVariableForName:name];

  if (stateVariable) {
    return (![stateVariable isAllowedValue:@"NOT_IMPLEMENTED"]);
  }

  return NO;
}

@end
