/* Exercise the real CLI orchestration with inert native owners and fake starts. */
#undef NDEBUG
#include <assert.h>
#include <mupnp/std/av/cmediarenderer.h>
#include <mupnp/upnp.h>
#include <signal.h>
#include <string.h>

static char trace[64];
static int count, failure;
static int stopSignal;
static void record(char value)
{
  trace[count++] = value;
  trace[count] = 0;
}
static CgUpnpAvRenderer* renderer_new(void)
{
  record('D');
  return failure == 1 ? NULL : cg_upnpav_dmr_new();
}
static bool renderer_start(CgUpnpAvRenderer* renderer)
{
  assert(renderer);
  record('S');
  return failure != 2;
}
static bool renderer_stop(CgUpnpAvRenderer* renderer)
{
  assert(renderer);
  record('Q');
  return true;
}
static void renderer_delete(CgUpnpAvRenderer* renderer)
{
  record('R');
  cg_upnpav_dmr_delete(renderer);
}
static mUpnpControlPoint* cp_new(void)
{
  record('C');
  return failure == 3 ? NULL : mupnp_controlpoint_new();
}
static bool cp_start(mUpnpControlPoint* cp)
{
  assert(cp);
  record('T');
  return failure != 4;
}
static bool cp_search(mUpnpControlPoint* cp, const char* target)
{
  assert(cp && strcmp(target, MUPNP_ST_ROOT_DEVICE) == 0);
  record('F');
  return failure != 5;
}
static bool cp_stop(mUpnpControlPoint* cp)
{
  assert(cp);
  record('X');
  return true;
}
static void cp_delete(mUpnpControlPoint* cp)
{
  record('Y');
  mupnp_controlpoint_delete(cp);
}
static void wait_for_search(long milliseconds)
{
  assert(milliseconds == 100);
  if (!strchr(trace, 'W'))
    record('W');
  if (stopSignal)
    raise(stopSignal);
}

#define main avchk_main
#define cg_upnpav_dmr_new renderer_new
#define cg_upnpav_dmr_start renderer_start
#define cg_upnpav_dmr_stop renderer_stop
#define cg_upnpav_dmr_delete renderer_delete
#define mupnp_controlpoint_new cp_new
#define mupnp_controlpoint_start cp_start
#define mupnp_controlpoint_search cp_search
#define mupnp_controlpoint_stop cp_stop
#define mupnp_controlpoint_delete cp_delete
#undef mupnp_sleep
#define mupnp_sleep wait_for_search
#include "../examples/upnpavchk/upnpavchk.c"
#undef main

static void check(int argc, char** argv, int expected, const char* sequence)
{
  count = 0;
  trace[0] = 0;
  assert(avchk_main(argc, argv) == expected);
  assert(strcmp(trace, sequence) == 0);
}
int main(void)
{
  char* normal[] = { "upnpavchk" };
  char* enabled[] = { "upnpavchk", "--with-renderer" };
  char* invalid[] = { "upnpavchk", "--unknown" };
  check(2, invalid, 2, "");
  check(1, normal, 0, "CTFWXY");
  check(2, enabled, 0, "DSCTFWXYQR");
  const char* failed[] = { "", "D", "DSQR", "DSCQR", "DSCTXYQR", "DSCTFXYQR" };
  for (failure = 1; failure <= 5; failure++)
    check(2, enabled, 1, failed[failure]);
  failure = 0;
  const int stopSignals[] = {
    SIGINT,
    SIGTERM,
#ifdef SIGBREAK
    SIGBREAK,
#endif
  };
  for (size_t i = 0; i < sizeof(stopSignals) / sizeof(stopSignals[0]); i++) {
    stopSignal = stopSignals[i];
    check(2, enabled, 128 + stopSignal, "DSCTFWXYQR");
  }
  return 0;
}
