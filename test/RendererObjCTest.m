#import "CGUpnpAvRenderer.h"
#import <Foundation/Foundation.h>
#include <assert.h>
#include <mupnp/std/av/cmediarenderer.h>
static int created, deleted, started, stopped;
static bool canStart;
CgUpnpAvRenderer* renderer_test_new(void)
{
  created++;
  return cg_upnpav_dmr_new();
}
void renderer_test_delete(CgUpnpAvRenderer* renderer)
{
  if (renderer)
    deleted++;
  cg_upnpav_dmr_delete(renderer);
}
bool renderer_test_start(CgUpnpAvRenderer* renderer)
{
  assert(renderer);
  started++;
  return canStart;
}
bool renderer_test_stop(CgUpnpAvRenderer* renderer)
{
  assert(renderer);
  stopped++;
  return true;
}
int main(void)
{
  @autoreleasepool {
    assert([[CGUpnpAvRenderer alloc] initWithFriendlyName:nil] == nil);
    assert(created == 0 && deleted == 0);
    CGUpnpAvRenderer* renderer = [[CGUpnpAvRenderer alloc] initWithFriendlyName:@"Renderer 日本語"];
    assert(renderer && renderer.cRenderer && !renderer.running);
    assert([renderer.udn hasPrefix:@"uuid:"]);
    assert([renderer.friendlyName isEqualToString:@"Renderer 日本語"]);
    assert(![renderer start] && started == 1);
    canStart = true;
    assert([renderer start] && started == 2);
    assert([renderer stop] && stopped == 1);
    [renderer release];
    assert(created == 1 && deleted == 1 && stopped == 2);
  }
  return 0;
}
