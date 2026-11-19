/* Modern implementation of the cg_upnpav_dmr_* C API. BSD-style; see COPYING. */
#include "renderer_internal.h"
#include <stdlib.h>
#include <string.h>

#define SERVICE_TYPE(name) "urn:schemas-upnp-org:service:" name ":1"
#define SERVICE(name) "<service><serviceType>" SERVICE_TYPE(name) "</serviceType>"                                                             \
                                                                  "<serviceId>urn:upnp-org:serviceId:" name "</serviceId>"                     \
                                                                  "<SCPDURL>/" name ".xml</SCPDURL><controlURL>/" name "/control</controlURL>" \
                                                                  "<eventSubURL>/" name "/event</eventSubURL></service>"
#define ARG(name, direction, variable) "<argument><name>" name "</name><direction>" direction \
                                       "</direction><relatedStateVariable>" variable "</relatedStateVariable></argument>"
#define ACTION(name, args) "<action><name>" name "</name><argumentList>" args "</argumentList></action>"
#define VAR(name, type, events) "<stateVariable sendEvents=\"" events "\"><name>" name \
                                "</name><dataType>" type "</dataType></stateVariable>"
#define SCPD(actions, variables) "<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">"                               \
                                 "<specVersion><major>1</major><minor>0</minor></specVersion><actionList>" actions \
                                 "</actionList><serviceStateTable>" variables "</serviceStateTable></scpd>"
#define INSTANCE ARG("InstanceID", "in", "A_ARG_TYPE_InstanceID")
#define INSTANCE_VAR VAR("A_ARG_TYPE_InstanceID", "ui4", "no")
#define LAST_CHANGE VAR("LastChange", "string", "no")

static const char DEVICE_DESCRIPTION[] = "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"
                                         "<specVersion><major>1</major><minor>0</minor></specVersion><device>"
                                         "<deviceType>urn:schemas-upnp-org:device:MediaRenderer:1</deviceType>"
                                         "<friendlyName>mUPnP MediaRenderer (no playback)</friendlyName>"
                                         "<manufacturer>Cyber Garage</manufacturer><modelName>Service-only MediaRenderer</modelName>"
                                         "<UDN>uuid:00000000-0000-0000-0000-000000000064</UDN><serviceList>" SERVICE("ConnectionManager") SERVICE("AVTransport") SERVICE("RenderingControl") "</serviceList></device></root>";

static const char* const SERVICE_TYPES[] = {
  SERVICE_TYPE("ConnectionManager"), SERVICE_TYPE("AVTransport"), SERVICE_TYPE("RenderingControl")
};

/* Service-only profile: playback is explicitly rejected. No formats,
 * event callbacks, playback backend, DLNA or conformance claim. */
static const char* const SERVICE_DESCRIPTIONS[] = {
  SCPD(ACTION("GetProtocolInfo", ARG("Source", "out", "SourceProtocolInfo") ARG("Sink", "out", "SinkProtocolInfo")),
      VAR("SourceProtocolInfo", "string", "no") VAR("SinkProtocolInfo", "string", "no")),
  SCPD(ACTION("GetTransportInfo", INSTANCE ARG("CurrentTransportState", "out", "TransportState") ARG("CurrentTransportStatus", "out", "TransportStatus") ARG("CurrentSpeed", "out", "TransportPlaySpeed"))
           ACTION("SetAVTransportURI", INSTANCE ARG("CurrentURI", "in", "AVTransportURI") ARG("CurrentURIMetaData", "in", "AVTransportURIMetaData"))
               ACTION("Play", INSTANCE ARG("Speed", "in", "TransportPlaySpeed"))
                   ACTION("Stop", INSTANCE),
      INSTANCE_VAR LAST_CHANGE VAR("TransportState", "string", "no")
          VAR("TransportStatus", "string", "no") VAR("TransportPlaySpeed", "string", "no")
              VAR("AVTransportURI", "uri", "no") VAR("AVTransportURIMetaData", "string", "no")),
  SCPD(ACTION("GetVolume", INSTANCE ARG("Channel", "in", "A_ARG_TYPE_Channel") ARG("CurrentVolume", "out", "Volume")),
      INSTANCE_VAR LAST_CHANGE VAR("A_ARG_TYPE_Channel", "string", "no") VAR("Volume", "ui2", "no"))
};

struct CgUpnpAvRenderer {
  mUpnpDevice* device;
  mUpnpMutex* mutex;
  bool running;
  mUpnpService* services[MUPNP_RENDERER_SERVICE_COUNT];
};

mUpnpService* cg_upnpav_dmr_service(CgUpnpAvRenderer* model, mUpnpRendererService service)
{
  if (!model || service < 0 || service >= MUPNP_RENDERER_SERVICE_COUNT)
    return NULL;
  return model->services[service];
}

void cg_upnpav_dmr_delete(CgUpnpAvRenderer* model)
{
  if (!model)
    return;
  cg_upnpav_dmr_stop(model);
  if (model->device)
    mupnp_device_delete(model->device);
  if (model->mutex)
    mupnp_mutex_delete(model->mutex);
  free(model);
}

static bool renderer_action(mUpnpAction* action);
static void renderer_http(mUpnpHttpRequest* request);

CgUpnpAvRenderer* cg_upnpav_dmr_from_xml(const char* deviceXml, const char* const serviceXml[MUPNP_RENDERER_SERVICE_COUNT])
{
  if (!deviceXml || !serviceXml)
    return NULL;
  for (int i = 0; i < MUPNP_RENDERER_SERVICE_COUNT; i++) {
    if (!serviceXml[i])
      return NULL;
  }
  CgUpnpAvRenderer* model = calloc(1, sizeof(*model));
  if (!model)
    return NULL;
  model->device = mupnp_device_new();
  if (!model->device || !mupnp_device_parsedescription(model->device, deviceXml, strlen(deviceXml)))
    goto fail;
  for (int i = 0; i < MUPNP_RENDERER_SERVICE_COUNT; i++) {
    model->services[i] = mupnp_device_getservicebyexacttype(model->device, SERVICE_TYPES[i]);
    if (!model->services[i] || !mupnp_service_parsedescription(model->services[i], serviceXml[i], strlen(serviceXml[i])))
      goto fail;
  }
  if (!mupnp_service_getactionbyname(model->services[0], "GetProtocolInfo")
      || !mupnp_service_getstatevariablebyname(model->services[0], "SourceProtocolInfo")
      || !mupnp_service_getstatevariablebyname(model->services[0], "SinkProtocolInfo")
      || !mupnp_service_getactionbyname(model->services[1], "GetTransportInfo")
      || !mupnp_service_getactionbyname(model->services[2], "GetVolume")
      || !mupnp_service_getstatevariablebyname(model->services[1], "LastChange")
      || !mupnp_service_getstatevariablebyname(model->services[2], "LastChange"))
    goto fail;
  cg_upnpav_dmr_set_last_change(model, MUPNP_RENDERER_AV_TRANSPORT, "<Event xmlns=\"urn:schemas-upnp-org:metadata-1-0/AVT/\"/>");
  cg_upnpav_dmr_set_last_change(model, MUPNP_RENDERER_RENDERING_CONTROL, "<Event xmlns=\"urn:schemas-upnp-org:metadata-1-0/RCS/\"/>");
  model->mutex = mupnp_mutex_new();
  if (!model->mutex)
    goto fail;
  mupnp_device_updateudn(model->device);
  mupnp_device_setleasetime(model->device, 30);
  mupnp_device_setuserdata(model->device, model);
  mupnp_device_setactionlistener(model->device, renderer_action);
  mupnp_device_sethttplistener(model->device, renderer_http);
  return model;
fail:
  cg_upnpav_dmr_delete(model);
  return NULL;
}

CgUpnpAvRenderer* cg_upnpav_dmr_new(void)
{
  return cg_upnpav_dmr_from_xml(DEVICE_DESCRIPTION, SERVICE_DESCRIPTIONS);
}

bool cg_upnpav_dmr_dispatch(CgUpnpAvRenderer* model, mUpnpAction* action)
{
  if (!action)
    return false;
  mupnp_action_setstatuscode(action, 401);
  mupnp_action_setstatusdescription(action, "Invalid Action");
  if (!model)
    return false;
  mUpnpService* service = mupnp_action_getservice(action);
  const char* name = mupnp_action_getname(action);
  /* Require the exact action owned by this model, not a foreign action
   * carrying a forged parent pointer or service type. */
  if (!name || !service || mupnp_service_getactionbyname(service, name) != action)
    return false;
  if (service == model->services[MUPNP_RENDERER_CONNECTION_MANAGER] && strcmp(name, "GetProtocolInfo") == 0) {
    if (!mupnp_action_setargumentvaluebyname(action, "Source", "") || !mupnp_action_setargumentvaluebyname(action, "Sink", ""))
      return false;
  }
  else if (service == model->services[MUPNP_RENDERER_AV_TRANSPORT]) {
    const char* instance = mupnp_action_getargumentvaluebyname(action, "InstanceID");
    if (!instance || strcmp(instance, "0") != 0) {
      mupnp_action_setstatuscode(action, 718);
      mupnp_action_setstatusdescription(action, "Invalid InstanceID");
      return false;
    }
    if (strcmp(name, "GetTransportInfo") != 0) {
      mupnp_action_setstatuscode(action, 701);
      mupnp_action_setstatusdescription(action, "Transition not available");
      return false;
    }
    if (!mupnp_action_setargumentvaluebyname(action, "CurrentTransportState", "NO_MEDIA_PRESENT")
        || !mupnp_action_setargumentvaluebyname(action, "CurrentTransportStatus", "OK")
        || !mupnp_action_setargumentvaluebyname(action, "CurrentSpeed", "1"))
      return false;
  }
  else
    return false;
  mupnp_action_setstatuscode(action, 0);
  mupnp_action_setstatusdescription(action, "");
  return true;
}

static mUpnpStateVariable* last_change(CgUpnpAvRenderer* model, mUpnpRendererService service)
{
  if (service != MUPNP_RENDERER_AV_TRANSPORT && service != MUPNP_RENDERER_RENDERING_CONTROL)
    return NULL;
  mUpnpService* owner = cg_upnpav_dmr_service(model, service);
  return owner ? mupnp_service_getstatevariablebyname(owner, "LastChange") : NULL;
}

bool cg_upnpav_dmr_set_last_change(CgUpnpAvRenderer* model, mUpnpRendererService service, const char* value)
{
  mUpnpStateVariable* variable = last_change(model, service);
  if (!variable || !value)
    return false;
  /* Never invoke GENA/subscriber notification from the construction model. */
  mupnp_statevariable_setvaluewithoutnotify(variable, value);
  return true;
}

const char* cg_upnpav_dmr_get_last_change(CgUpnpAvRenderer* model, mUpnpRendererService service)
{
  mUpnpStateVariable* variable = last_change(model, service);
  return variable ? mupnp_statevariable_getvalue(variable) : NULL;
}

mUpnpDevice* cg_upnpav_dmr_getdevice(CgUpnpAvRenderer* renderer)
{
  return renderer ? renderer->device : NULL;
}

bool cg_upnpav_dmr_isrunning(CgUpnpAvRenderer* renderer)
{
  return renderer && renderer->running && mupnp_device_isrunning(renderer->device);
}

bool cg_upnpav_dmr_stop(CgUpnpAvRenderer* renderer)
{
  if (!renderer)
    return false;
  if (!renderer->running)
    return true;
  bool stopped = mupnp_device_stop(renderer->device);
  renderer->running = false;
  return stopped;
}

bool cg_upnpav_dmr_start(CgUpnpAvRenderer* renderer)
{
  if (!renderer)
    return false;
  if (renderer->running)
    return cg_upnpav_dmr_isrunning(renderer);
  if (!mupnp_device_start(renderer->device) || !mupnp_device_isrunning(renderer->device)) {
    mupnp_device_stop(renderer->device);
    return false;
  }
  renderer->running = true;
  return true;
}

static bool renderer_action(mUpnpAction* action)
{
  mUpnpDevice* device = mupnp_service_getdevice(mupnp_action_getservice(action));
  CgUpnpAvRenderer* renderer = mupnp_device_getuserdata(device);
  return cg_upnpav_dmr_dispatch(renderer, action);
}

static void renderer_http(mUpnpHttpRequest* request)
{
  mUpnpDevice* device = mupnp_http_request_getuserdata(request);
  CgUpnpAvRenderer* renderer = mupnp_device_getuserdata(device);
  const char* method = mupnp_http_request_getmethod(request);
  /* No eventing/playback backend: never accept or fetch CALLBACK URLs. */
  if (!method || (strcmp(method, "GET") && strcmp(method, "HEAD") && strcmp(method, "POST"))) {
    mupnp_http_request_poststatuscode(request, 405);
    return;
  }
  mupnp_mutex_lock(renderer->mutex);
  for (int i = 0; i < MUPNP_RENDERER_SERVICE_COUNT; i++) {
    for (mUpnpAction* command = mupnp_service_getactions(renderer->services[i]); command; command = mupnp_action_next(command)) {
      for (mUpnpArgument* arg = mupnp_action_getarguments(command); arg; arg = mupnp_argument_next(arg)) {
        if (mupnp_argument_isindirection(arg))
          mupnp_argument_setvalue(arg, "");
      }
    }
  }
  mupnp_device_httprequestrecieved(request);
  mupnp_mutex_unlock(renderer->mutex);
}
