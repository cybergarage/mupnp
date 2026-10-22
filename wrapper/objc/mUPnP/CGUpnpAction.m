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
#import "CGUpnpStateVariable.h"

_Static_assert((int)CGUpnpActionResultIncomplete == (int)MUPNP_ACTION_RESULT_INCOMPLETE, "Result codes must agree");

@interface CGUpnpActionResult ()
- (id)initWithNativeResult:(const mUpnpActionResult*)result;
@end

@implementation CGUpnpActionResult
@synthesize outcome, httpStatusCode, upnpErrorCode, outputs, faultCode, faultString, errorDescription;
- (id)initWithNativeResult:(const mUpnpActionResult*)result
{
  if (!(self = [super init]))
    return nil;
  outcome = (CGUpnpActionResultCode)mupnp_action_result_getcode(result);
  httpStatusCode = mupnp_action_result_gethttpstatuscode(result);
  upnpErrorCode = mupnp_action_result_getupnperrorcode(result);
  NSMutableDictionary* values = [NSMutableDictionary dictionary];
  for (mUpnpDictionaryElement* entry = mupnp_dictionary_gets(mupnp_action_result_getoutputs(result)); entry; entry = mupnp_dictionary_element_next(entry)) {
    const char* name = mupnp_dictionary_element_getkey(entry);
    const char* value = mupnp_action_result_getoutput(result, name);
    NSString* key = name ? [NSString stringWithUTF8String:name] : nil;
    NSString* text = value ? [NSString stringWithUTF8String:value] : nil;
    if (key && text)
      values[key] = text;
  }
  outputs = [[NSDictionary alloc] initWithDictionary:values];
  const char* code = mupnp_action_result_getfaultcode(result);
  const char* text = mupnp_action_result_getfaultstring(result);
  const char* description = mupnp_action_result_geterrordescription(result);
  faultCode = code ? [[NSString alloc] initWithUTF8String:code] : nil;
  faultString = text ? [[NSString alloc] initWithUTF8String:text] : nil;
  errorDescription = description ? [[NSString alloc] initWithUTF8String:description] : nil;
  return self;
}
- (void)dealloc
{
  [outputs release];
  [faultCode release];
  [faultString release];
  [errorDescription release];
  [super dealloc];
}
@end

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

- (NSArray<NSDictionary<NSString*, id>*>*)argumentMetadata
{
  if (!cObject)
    return @[];
  NSMutableArray* snapshots = [NSMutableArray array];
  mUpnpService* service = mupnp_action_getservice(cObject);
  for (mUpnpArgument* argument = mupnp_action_getarguments(cObject); argument; argument = mupnp_argument_next(argument)) {
    NSMutableDictionary* snapshot = [NSMutableDictionary dictionary];
    const char* rawName = mupnp_argument_getname(argument);
    const char* rawDirection = mupnp_argument_getdirection(argument);
    const char* rawRelated = mupnp_argument_getrelatedstatevariable(argument);
    NSString* name = rawName ? [NSString stringWithUTF8String:rawName] : nil;
    NSString* direction = rawDirection ? [NSString stringWithUTF8String:rawDirection] : nil;
    NSString* related = rawRelated ? [NSString stringWithUTF8String:rawRelated] : nil;
    if (name)
      snapshot[@"name"] = name;
    if (direction)
      snapshot[@"direction"] = direction;
    if (related)
      snapshot[@"relatedStateVariable"] = related;
    mUpnpStateVariable* native = service && rawRelated ? mupnp_service_getstatevariablebyname(service, rawRelated) : NULL;
    if (native) {
      CGUpnpStateVariable* variable = [[CGUpnpStateVariable alloc] initWithCObject:native];
      snapshot[@"stateVariable"] = [variable metadata];
      [variable release];
    }
    [snapshots addObject:[NSDictionary dictionaryWithDictionary:snapshot]];
  }
  return [NSArray arrayWithArray:snapshots];
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

- (CGUpnpActionResult*)postResultWithArguments:(NSDictionary<NSString*, NSString*>*)arguments
{
  BOOL valid = !arguments || [arguments isKindOfClass:[NSDictionary class]];
  mUpnpDictionary* inputs = mupnp_dictionary_new();
  if (!inputs)
    return nil;
  NSString* nul = [NSString stringWithCharacters:(const unichar[]) { 0 } length:1];
  if (valid) {
    for (id key in arguments) {
      id value = [arguments objectForKey:key];
      if (![key isKindOfClass:[NSString class]] || ![value isKindOfClass:[NSString class]]
          || ![key UTF8String] || ![value UTF8String]
          || [key rangeOfString:nul].location != NSNotFound
          || [value rangeOfString:nul].location != NSNotFound) {
        valid = NO;
        break;
      }
      mupnp_dictionary_setvalue(inputs, [key UTF8String], [value UTF8String]);
    }
  }
  mUpnpActionResult* native = mupnp_action_postwitharguments(valid ? cObject : NULL, inputs);
  mupnp_dictionary_delete(inputs);
  if (!native)
    return nil;
  CGUpnpActionResult* result = [[[CGUpnpActionResult alloc] initWithNativeResult:native] autorelease];
  mupnp_action_result_delete(native);
  return result;
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
