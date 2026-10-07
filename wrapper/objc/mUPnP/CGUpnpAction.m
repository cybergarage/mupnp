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
#include <mupnp/control/control.h>

#import "CGUpnpAction.h"

@implementation CGUpnpAction

@synthesize cObject;
@synthesize cObjectOwner;

- (id)initWithCObject:(mUpnpAction*)cobj
{
  if ((self = [super init]) == nil)
    return nil;
  cObject = cobj;
  /* Wrapping an action must not overwrite its native listener with raw self.
   * Hosted callbacks are dispatched by CGUpnpDevice's delegate listener. */
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

- (NSString*)name
{
  if (!cObject)
    return nil;
  return [[[NSString alloc] initWithUTF8String:mupnp_action_getname(cObject)] autorelease];
}

- (NSDictionary*)arguments
{
  if (!cObject)
    return [NSDictionary dictionary];
  NSMutableDictionary* argDir = [NSMutableDictionary dictionary];
  mUpnpArgument* carg;
  for (carg = mupnp_action_getarguments(cObject); carg; carg = mupnp_argument_next(carg)) {
    char* name = (char*)mupnp_argument_getname(carg);
    char* value = mupnp_argument_getvalue(carg);
    NSString* obj = [[NSString alloc] initWithUTF8String:(value ? value : "")];
    NSString* key = [[NSString alloc] initWithUTF8String:name];
    [argDir setObject:obj forKey:key];
    [obj release];
    [key release];
  }
  return argDir;
}

- (BOOL)setArgumentValue:(NSString*)value forName:(NSString*)name
{
  mUpnpArgument* cArg;

  if (!cObject)
    return NO;
  cArg = mupnp_action_getargumentbyname(cObject, (char*)[name UTF8String]);
  if (!cArg)
    return NO;
  mupnp_argument_setvalue(cArg, (char*)[value UTF8String]);
  return YES;
}

- (NSString*)argumentValueForName:(NSString*)name
{
  char* cValue;
  mUpnpArgument* cArg;

  if (!cObject)
    return nil;
  cArg = mupnp_action_getargumentbyname(cObject, (char*)[name UTF8String]);
  if (!cArg)
    return nil;
  cValue = mupnp_argument_getvalue(cArg);
  if (mupnp_strlen(cValue) <= 0)
    return nil;
  return [NSString stringWithUTF8String:cValue];
}

- (BOOL)post
{
  if (!cObject)
    return NO;
  BOOL ret = mupnp_action_post(cObject);
  return ret;
}

- (BOOL)postWithArguments:(NSDictionary*)arguments
{
  if (!cObject)
    return NO;
  if (arguments && ![arguments isKindOfClass:[NSDictionary class]])
    return NO;

  /* Validate every entry before touching the native action, so that a
   * rejected call neither sends a request nor leaves partially updated
   * argument values behind. Entries are looked up with objectForKey: so that
   * keys such as "@count" are not interpreted as key-value coding paths. */
  for (id name in arguments) {
    if (![name isKindOfClass:[NSString class]])
      return NO;
    id value = [arguments objectForKey:name];
    if (![value isKindOfClass:[NSString class]])
      return NO;
    const char* cName = [(NSString*)name UTF8String];
    if (!cName || ![(NSString*)value UTF8String])
      return NO;
    if (!mupnp_action_getargumentbyname(cObject, (char*)cName))
      return NO;
  }

  for (NSString* name in arguments) {
    if (![self setArgumentValue:[arguments objectForKey:name] forName:name])
      return NO;
  }
  return [self post];
}

- (NSInteger)statusCode
{
  if (!cObject)
    return 0;
  return mupnp_action_getstatuscode(cObject);
}

@end
