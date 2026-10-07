/******************************************************************
 *
 * mUPnP for ObjC
 *
 * Copyright (C) Satoshi Konno 2008
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#include <mupnp/control/control.h>
#include <mupnp/statevariable.h>

#import "CGUpnpStateVariable.h"

@implementation CGUpnpStateVariable

@synthesize cObject;
@synthesize cObjectOwner;

- (id)initWithCObject:(mUpnpStateVariable*)cobj
{
  if ((self = [super init]) == nil)
    return nil;
  /* The native state variable is borrowed: the wrapper neither owns it nor
   * writes its userdata, matching CGUpnpAction. A NULL object yields an
   * invalid wrapper whose getters return nil/empty values and whose query
   * fails with NO. */
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

- (NSString*)name
{
  if (!cObject)
    return nil;
  const char* cName = mupnp_statevariable_getname(cObject);
  if (!cName)
    return nil;
  return [[[NSString alloc] initWithUTF8String:cName] autorelease];
}

- (NSString*)value
{
  if (!cObject)
    return nil;
  const char* cValue = mupnp_statevariable_getvalue(cObject);
  if (!cValue)
    return nil;
  return [[[NSString alloc] initWithUTF8String:cValue] autorelease];
}

- (NSArray*)allowedValues
{
  if (!cObject)
    return [[[NSArray alloc] init] autorelease];
  NSMutableArray* valuesArray = [[[NSMutableArray alloc] init] autorelease];

  mUpnpAllowedValue* cAllowedValue;
  for (cAllowedValue = mupnp_statevariable_getallowedvaluelist(cObject); cAllowedValue; cAllowedValue = (mUpnpAllowedValue*)mupnp_list_next((mUpnpList*)cAllowedValue)) {
    const char* cValue = mupnp_string_getvalue(cAllowedValue->value);
    if (!cValue)
      continue;
    NSString* value = [[[NSString alloc] initWithUTF8String:cValue] autorelease];
    if (value)
      [valuesArray addObject:value];
  }
  return valuesArray;
}

- (BOOL)isAllowedValue:(NSString*)value
{
  if (!cObject || ![value isKindOfClass:[NSString class]] || ![value UTF8String])
    return NO;
  return mupnp_statevariable_is_allowed_value(cObject, [value UTF8String]);
}

- (BOOL)query
{
  if (!cObject)
    return NO;
  return mupnp_statevariable_post(cObject);
}

- (NSInteger)statusCode
{
  if (!cObject)
    return 0;
  return mupnp_statevariable_getstatuscode(cObject);
}

@end
