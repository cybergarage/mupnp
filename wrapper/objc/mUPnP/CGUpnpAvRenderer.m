#import "CGUpnpAvRenderer.h"
#include <mupnp/std/av/cmediarenderer.h>

@implementation CGUpnpAvRenderer
- (instancetype)init
{
  return [self initWithFriendlyName:@"mUPnP MediaRenderer (no playback)"];
}

- (instancetype)initWithFriendlyName:(NSString*)friendlyName
{
  if (!(self = [super init]))
    return nil;
  if (![friendlyName isKindOfClass:[NSString class]] || [friendlyName length] == 0 || ![friendlyName UTF8String]) {
    [self release];
    return nil;
  }
  _cRenderer = cg_upnpav_dmr_new();
  if (!_cRenderer) {
    [self release];
    return nil;
  }
  mupnp_device_setfriendlyname(cg_upnpav_dmr_getdevice(_cRenderer), [friendlyName UTF8String]);
  return self;
}

- (CgUpnpAvRenderer*)cRenderer
{
  return _cRenderer;
}

- (NSString*)friendlyName
{
  @synchronized(self) {
    const char* name = mupnp_device_getfriendlyname(cg_upnpav_dmr_getdevice(_cRenderer));
    return name ? [NSString stringWithUTF8String:name] : nil;
  }
}

- (NSString*)udn
{
  @synchronized(self) {
    const char* value = mupnp_device_getudn(cg_upnpav_dmr_getdevice(_cRenderer));
    return value ? [NSString stringWithUTF8String:value] : nil;
  }
}

- (BOOL)isRunning
{
  @synchronized(self) {
    return cg_upnpav_dmr_isrunning(_cRenderer);
  }
}

- (BOOL)start
{
  @synchronized(self) {
    return cg_upnpav_dmr_start(_cRenderer);
  }
}

- (BOOL)stop
{
  @synchronized(self) {
    return _cRenderer ? cg_upnpav_dmr_stop(_cRenderer) : YES;
  }
}

- (void)dealloc
{
  [self stop];
  cg_upnpav_dmr_delete(_cRenderer);
  [super dealloc];
}
@end
