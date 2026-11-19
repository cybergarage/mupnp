/* Experimental in-memory MediaRenderer service model. BSD-style license; see COPYING. */
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
#define LAST_CHANGE VAR("LastChange", "string", "yes")

static const char DEVICE_DESCRIPTION[] = "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"
                                         "<specVersion><major>1</major><minor>0</minor></specVersion><device>"
                                         "<deviceType>urn:schemas-upnp-org:device:MediaRenderer:1</deviceType>"
                                         "<friendlyName>Internal renderer construction fixture</friendlyName>"
                                         "<manufacturer>Cyber Garage</manufacturer><modelName>In-memory model</modelName>"
                                         "<UDN>uuid:00000000-0000-0000-0000-000000000064</UDN><serviceList>" SERVICE("ConnectionManager") SERVICE("AVTransport") SERVICE("RenderingControl") "</serviceList></device></root>";

static const char* const SERVICE_TYPES[] = {
  SERVICE_TYPE("ConnectionManager"), SERVICE_TYPE("AVTransport"), SERVICE_TYPE("RenderingControl")
};

/* Deliberately incomplete, never published. Unsupported actions are included
 * solely to test explicit rejection; there is no DLNA/conformance claim. */
static const char* const SERVICE_DESCRIPTIONS[] = {
  SCPD(ACTION("GetProtocolInfo", ARG("Source", "out", "SourceProtocolInfo") ARG("Sink", "out", "SinkProtocolInfo")),
      VAR("SourceProtocolInfo", "string", "yes") VAR("SinkProtocolInfo", "string", "yes")),
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

struct mUpnpRendererModel {
  mUpnpDevice* device;
  mUpnpService* services[MUPNP_RENDERER_SERVICE_COUNT];
};

mUpnpService* mupnp_renderer_model_service(mUpnpRendererModel* model, mUpnpRendererService service)
{
  if (!model || service < 0 || service >= MUPNP_RENDERER_SERVICE_COUNT)
    return NULL;
  return model->services[service];
}

void mupnp_renderer_model_delete(mUpnpRendererModel* model)
{
  if (!model)
    return;
  if (model->device)
    mupnp_device_delete(model->device);
  free(model);
}

mUpnpRendererModel* mupnp_renderer_model_from_xml(const char* deviceXml, const char* const serviceXml[MUPNP_RENDERER_SERVICE_COUNT])
{
  if (!deviceXml || !serviceXml)
    return NULL;
  for (int i = 0; i < MUPNP_RENDERER_SERVICE_COUNT; i++) {
    if (!serviceXml[i])
      return NULL;
  }
  mUpnpRendererModel* model = calloc(1, sizeof(*model));
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
  mupnp_renderer_model_set_last_change(model, MUPNP_RENDERER_AV_TRANSPORT, "<Event xmlns=\"urn:schemas-upnp-org:metadata-1-0/AVT/\"/>");
  mupnp_renderer_model_set_last_change(model, MUPNP_RENDERER_RENDERING_CONTROL, "<Event xmlns=\"urn:schemas-upnp-org:metadata-1-0/RCS/\"/>");
  return model;
fail:
  mupnp_renderer_model_delete(model);
  return NULL;
}

mUpnpRendererModel* mupnp_renderer_model_new(void)
{
  return mupnp_renderer_model_from_xml(DEVICE_DESCRIPTION, SERVICE_DESCRIPTIONS);
}

bool mupnp_renderer_model_dispatch(mUpnpRendererModel* model, mUpnpAction* action)
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

static mUpnpStateVariable* last_change(mUpnpRendererModel* model, mUpnpRendererService service)
{
  if (service != MUPNP_RENDERER_AV_TRANSPORT && service != MUPNP_RENDERER_RENDERING_CONTROL)
    return NULL;
  mUpnpService* owner = mupnp_renderer_model_service(model, service);
  return owner ? mupnp_service_getstatevariablebyname(owner, "LastChange") : NULL;
}

bool mupnp_renderer_model_set_last_change(mUpnpRendererModel* model, mUpnpRendererService service, const char* value)
{
  mUpnpStateVariable* variable = last_change(model, service);
  if (!variable || !value)
    return false;
  /* Never invoke GENA/subscriber notification from the construction model. */
  mupnp_statevariable_setvaluewithoutnotify(variable, value);
  return true;
}

const char* mupnp_renderer_model_get_last_change(mUpnpRendererModel* model, mUpnpRendererService service)
{
  mUpnpStateVariable* variable = last_change(model, service);
  return variable ? mupnp_statevariable_getvalue(variable) : NULL;
}
