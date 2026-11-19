/* The normal Device lifecycle is replaced only in this executable. No sockets. */
#undef NDEBUG
#include <assert.h>
#include <mupnp/std/av/cmediarenderer.h>

static int starts, stops;
static bool succeed, alive;
bool mupnp_test_device_start(mUpnpDevice* device)
{
  assert(device);
  starts++;
  alive = succeed;
  return succeed;
}
bool mupnp_test_device_stop(mUpnpDevice* device)
{
  assert(device);
  stops++;
  alive = false;
  return true;
}
bool mupnp_test_device_isrunning(mUpnpDevice* device)
{
  assert(device);
  return alive;
}
int main(void)
{
  assert(!cg_upnpav_dmr_start(NULL));
  assert(!cg_upnpav_dmr_stop(NULL));
  assert(!cg_upnpav_dmr_isrunning(NULL));
  CgUpnpAvRenderer* renderer = cg_upnpav_dmr_new();
  assert(renderer && cg_upnpav_dmr_getdevice(renderer));
  assert(!cg_upnpav_dmr_isrunning(renderer));
  assert(cg_upnpav_dmr_stop(renderer) && stops == 0);
  assert(!cg_upnpav_dmr_start(renderer));
  assert(starts == 1 && stops == 1); // partial failure cleaned through Device
  succeed = true;
  assert(cg_upnpav_dmr_start(renderer));
  assert(cg_upnpav_dmr_isrunning(renderer) && starts == 2);
  assert(cg_upnpav_dmr_start(renderer) && starts == 2); // idempotent
  assert(cg_upnpav_dmr_stop(renderer) && stops == 2);
  assert(cg_upnpav_dmr_stop(renderer) && stops == 2);
  assert(cg_upnpav_dmr_start(renderer) && starts == 3);
  cg_upnpav_dmr_delete(renderer);
  assert(stops == 3); // deleting a running owner stops it first
  return 0;
}
