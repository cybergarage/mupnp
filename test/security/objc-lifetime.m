/* Run via verify-macos.py: discovery wrappers must survive native cache deletion. */
#include <mupnp/upnp.h>

#import "CGUpnpAction.h"
#import "CGUpnpControlPoint.h"
#import "CGUpnpDevice.h"
#import "CGUpnpIcon.h"
#import "CGUpnpService.h"
#import "CGUpnpStateVariable.h"
#import <Foundation/Foundation.h>
#include <assert.h>

extern mUpnpDevice* upnp_test_device_new(void);

static bool native_listener(mUpnpAction* action)
{
  (void)action;
  return true;
}

int main(void)
{
  @autoreleasepool {
    mUpnpDevice* native = upnp_test_device_new();
    mUpnpService* nativeService = mupnp_device_getservices(native);
    mUpnpAction* nativeAction = mupnp_service_getactions(nativeService);
    mupnp_action_setlistener(nativeAction, native_listener);
    void* marker = (void*)0x1234;
    mupnp_action_setuserdata(nativeAction, marker);
    CGUpnpAction* borrowed = [[CGUpnpAction alloc] initWithCObject:nativeAction];
    assert(mupnp_action_getlistener(nativeAction) == native_listener);
    assert(mupnp_action_getuserdata(nativeAction) == marker);
    [borrowed release];
    assert(mupnp_action_getlistener(nativeAction) == native_listener);
    assert(mupnp_action_getlistener(nativeAction)(nativeAction));

    mupnp_statevariable_setvalue(mupnp_service_getstatevariables(nativeService), "observed-state");
    CGUpnpAction* action;
    CGUpnpStateVariable* variable;
    CGUpnpIcon* icon;
    NSString* expected;
    NSString* variableName;
    @autoreleasepool {
      CGUpnpControlPoint* cp = [[CGUpnpControlPoint alloc] init];
      mupnp_controlpoint_adddevice(cp.cObject, native);
      CGUpnpDevice* device = [[cp deviceForUDN:[NSString stringWithUTF8String:mupnp_device_getudn(native)]] retain];
      assert(device && [device friendlyName]);
      CGUpnpService* service = [[[device services] objectAtIndex:0] retain];
      action = [[[service actions] objectAtIndex:0] retain];
      variable = [[[service stateVariables] objectAtIndex:0] retain];
      icon = [[device smallestIcon] retain];
      expected = [[action name] copy];
      variableName = [[variable name] copy];
      assert([[variable value] isEqualToString:@"observed-state"]);
      mupnp_device_delete(native);
      [cp release];
      [device release];
      [service release];
    }
    /* Drain the autoreleased parent wrappers before reading the descendants. */
    @autoreleasepool {
      assert([[action name] isEqualToString:expected]);
      assert([[variable name] isEqualToString:variableName]);
      if (icon)
        assert([icon url]);
    }
    [icon release];
    [action release];
    [variable release];
    [expected release];
    [variableName release];
  }
  return 0;
}
