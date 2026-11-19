/* MediaRenderer service host, without a playback backend. BSD-style; see COPYING. */
#ifndef MUPNP_STD_AV_MEDIARENDERER_H
#define MUPNP_STD_AV_MEDIARENDERER_H
#include <mupnp/device.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct CgUpnpAvRenderer CgUpnpAvRenderer;
/* Construction is inert. Configure the borrowed device before starting.
 * The host exposes MediaRenderer:1 services but accepts no media formats,
 * rejects playback/mutating actions and does not implement event callbacks.
 * There is no DLNA/conformance claim. Serialize lifecycle calls yourself. */
CgUpnpAvRenderer* cg_upnpav_dmr_new(void);
void cg_upnpav_dmr_delete(CgUpnpAvRenderer* renderer);
/* Explicitly use the normal mUPnP Device lifecycle and discovered interfaces.
 * Start is idempotent; failure drains any partially started resources.
 * Stop is safe before start and may block while workers drain. */
bool cg_upnpav_dmr_start(CgUpnpAvRenderer* renderer);
bool cg_upnpav_dmr_stop(CgUpnpAvRenderer* renderer);
bool cg_upnpav_dmr_isrunning(CgUpnpAvRenderer* renderer);
mUpnpDevice* cg_upnpav_dmr_getdevice(CgUpnpAvRenderer* renderer);
#ifdef __cplusplus
}
#endif
#endif
