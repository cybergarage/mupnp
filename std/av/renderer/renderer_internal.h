/* Internal, uninstalled construction fixture. Not a playable MediaRenderer.
 * BSD-style license; see COPYING. Never start its underlying device. */
#ifndef MUPNP_RENDERER_INTERNAL_H
#define MUPNP_RENDERER_INTERNAL_H
#include <mupnp/upnp.h>

typedef struct mUpnpRendererModel mUpnpRendererModel;
typedef enum {
  MUPNP_RENDERER_CONNECTION_MANAGER,
  MUPNP_RENDERER_AV_TRANSPORT,
  MUPNP_RENDERER_RENDERING_CONTROL,
  MUPNP_RENDERER_SERVICE_COUNT
} mUpnpRendererService;

/* Allocate/parse only: no sockets, workers, discovery or advertising. */
mUpnpRendererModel* mupnp_renderer_model_new(void);
/* Parser seam for offline failure-path tests; also internal and uninstalled. */
mUpnpRendererModel* mupnp_renderer_model_from_xml(const char* deviceXml, const char* const serviceXml[MUPNP_RENDERER_SERVICE_COUNT]);
void mupnp_renderer_model_delete(mUpnpRendererModel* model);
mUpnpService* mupnp_renderer_model_service(mUpnpRendererModel* model, mUpnpRendererService service);
/* In-memory dispatch only; no action is posted. Inputs remain caller-owned. */
bool mupnp_renderer_model_dispatch(mUpnpRendererModel* model, mUpnpAction* action);
bool mupnp_renderer_model_set_last_change(mUpnpRendererModel* model, mUpnpRendererService service, const char* value);
const char* mupnp_renderer_model_get_last_change(mUpnpRendererModel* model, mUpnpRendererService service);
#endif
