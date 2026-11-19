/* Private construction/service test seams. BSD-style license; see COPYING. */
#ifndef MUPNP_RENDERER_INTERNAL_H
#define MUPNP_RENDERER_INTERNAL_H
#include <mupnp/std/av/cmediarenderer.h>
typedef enum {
  MUPNP_RENDERER_CONNECTION_MANAGER,
  MUPNP_RENDERER_AV_TRANSPORT,
  MUPNP_RENDERER_RENDERING_CONTROL,
  MUPNP_RENDERER_SERVICE_COUNT
} mUpnpRendererService;

/* Parser seam for offline failure-path tests; also internal and uninstalled. */
CgUpnpAvRenderer* cg_upnpav_dmr_from_xml(const char* deviceXml, const char* const serviceXml[MUPNP_RENDERER_SERVICE_COUNT]);

mUpnpService* cg_upnpav_dmr_service(CgUpnpAvRenderer* model, mUpnpRendererService service);
/* In-memory dispatch only; no action is posted. Inputs remain caller-owned. */
bool cg_upnpav_dmr_dispatch(CgUpnpAvRenderer* model, mUpnpAction* action);
bool cg_upnpav_dmr_set_last_change(CgUpnpAvRenderer* model, mUpnpRendererService service, const char* value);
const char* cg_upnpav_dmr_get_last_change(CgUpnpAvRenderer* model, mUpnpRendererService service);
#endif
