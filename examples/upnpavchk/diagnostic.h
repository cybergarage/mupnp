/* Bounded IPv4 diagnostic, not a playable renderer. BSD-style license; see COPYING. */
#ifndef MUPNP_AVCHK_DIAGNOSTIC_H
#define MUPNP_AVCHK_DIAGNOSTIC_H
#include <mupnp/upnp.h>
#include <signal.h>

typedef struct {
  const char* target;
  const char* interfaceAddress;
  int seconds;
  bool withRenderer;
} mUpnpAvChkOptions;

/* 0 = legacy invocation; 1 = diagnostic; 2 = help; -1 = invalid. No I/O. */
int mupnp_avchk_parse(int argc, char** argv, mUpnpAvChkOptions* options);
int mupnp_avchk_run(const mUpnpAvChkOptions* options);
void mupnp_avchk_usage(void);
/* Internal validation/inspection seams for exact-target loopback fixtures. */
mUpnpSocket* mupnp_avchk_bind_socket(const char* local, bool listening);
bool mupnp_avchk_endpoint(mUpnpNetURL* url, const char* target);
bool mupnp_avchk_inspect(const mUpnpAvChkOptions* options, mUpnpNetURL* location);

typedef struct mUpnpAvChkRenderer mUpnpAvChkRenderer;
/* Loopback tests pass advertise=false. No device-wide/all-interface start. */
mUpnpAvChkRenderer* mupnp_avchk_renderer_open(const char* local, const char* target, bool advertise);
bool mupnp_avchk_renderer_close(mUpnpAvChkRenderer* renderer);
bool mupnp_avchk_renderer_observed(mUpnpAvChkRenderer* renderer);
int mupnp_avchk_renderer_port(mUpnpAvChkRenderer* renderer);
#endif
