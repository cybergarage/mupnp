/* Offline regression: construction/dispatch only. Never start/post/subscribe.
 * BSD-style license; see COPYING. */
#undef NDEBUG
#include "renderer_internal.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static mUpnpAction* action(CgUpnpAvRenderer* model, mUpnpRendererService kind, const char* name)
{
  mUpnpService* service = cg_upnpav_dmr_service(model, kind);
  assert(service);
  mUpnpAction* result = mupnp_service_getactionbyname(service, name);
  assert(result && mupnp_action_getservice(result) == service);
  return result;
}

static void assert_idle(CgUpnpAvRenderer* model)
{
  mUpnpService* service = cg_upnpav_dmr_service(model, MUPNP_RENDERER_CONNECTION_MANAGER);
  mUpnpDevice* device = mupnp_service_getdevice(service);
  assert(device);
  assert(strcmp(mupnp_device_getdevicetype(device), "urn:schemas-upnp-org:device:MediaRenderer:1") == 0);
  assert(mupnp_servicelist_size(mupnp_device_getservicelist(device)) == MUPNP_RENDERER_SERVICE_COUNT);
  assert(mupnp_http_serverlist_size(device->httpServerList) == 0);
  assert(mupnp_ssdp_serverlist_size(device->ssdpServerList) == 0);
  assert(!mupnp_thread_isrunnable(device->advertiser));
  for (int i = 0; i < MUPNP_RENDERER_SERVICE_COUNT; i++) {
    service = cg_upnpav_dmr_service(model, i);
    assert(mupnp_service_getdevice(service) == device);
    assert(mupnp_service_getsubscribers(service) == NULL);
    for (mUpnpAction* command = mupnp_service_getactions(service); command; command = mupnp_action_next(command)) {
      assert(mupnp_action_getservice(command) == service);
      for (mUpnpArgument* arg = mupnp_action_getarguments(command); arg; arg = mupnp_argument_next(arg)) {
        assert(mupnp_argument_isindirection(arg) || mupnp_argument_isoutdirection(arg));
        assert(mupnp_service_getstatevariablebyname(service, mupnp_argument_getrelatedstatevariable(arg)));
      }
    }
  }
}

static void test_dispatch(CgUpnpAvRenderer* model)
{
  mUpnpAction* info = action(model, MUPNP_RENDERER_CONNECTION_MANAGER, "GetProtocolInfo");
  assert(mupnp_argument_isoutdirection(mupnp_action_getargumentbyname(info, "Source")));
  assert(mupnp_argument_isoutdirection(mupnp_action_getargumentbyname(info, "Sink")));
  /* Both outputs must be cleared, including values left by a previous call. */
  assert(mupnp_action_setargumentvaluebyname(info, "Source", "stale source"));
  assert(mupnp_action_setargumentvaluebyname(info, "Sink", "stale sink"));
  assert(cg_upnpav_dmr_dispatch(model, info));
  assert(strcmp(mupnp_action_getargumentvaluebyname(info, "Source"), "") == 0);
  assert(strcmp(mupnp_action_getargumentvaluebyname(info, "Sink"), "") == 0);
  assert(mupnp_action_getstatuscode(info) == 0);

  info = action(model, MUPNP_RENDERER_AV_TRANSPORT, "GetTransportInfo");
  assert(mupnp_argument_isindirection(mupnp_action_getargumentbyname(info, "InstanceID")));
  assert(mupnp_argument_isoutdirection(mupnp_action_getargumentbyname(info, "CurrentTransportState")));
  assert(!cg_upnpav_dmr_dispatch(model, info));
  assert(mupnp_action_getstatuscode(info) == 718);
  assert(mupnp_action_setargumentvaluebyname(info, "InstanceID", "1"));
  assert(!cg_upnpav_dmr_dispatch(model, info));
  assert(mupnp_action_getstatuscode(info) == 718);
  assert(mupnp_action_setargumentvaluebyname(info, "InstanceID", "0"));
  assert(cg_upnpav_dmr_dispatch(model, info));
  assert(strcmp(mupnp_action_getargumentvaluebyname(info, "CurrentTransportState"), "NO_MEDIA_PRESENT") == 0);
  assert(strcmp(mupnp_action_getargumentvaluebyname(info, "CurrentTransportStatus"), "OK") == 0);
  assert(strcmp(mupnp_action_getargumentvaluebyname(info, "CurrentSpeed"), "1") == 0);
  assert(mupnp_action_getstatuscode(info) == 0);

  const char* unsupported[] = { "SetAVTransportURI", "Play", "Stop" };
  for (size_t i = 0; i < sizeof(unsupported) / sizeof(unsupported[0]); i++) {
    mUpnpAction* command = action(model, MUPNP_RENDERER_AV_TRANSPORT, unsupported[i]);
    assert(mupnp_action_setargumentvaluebyname(command, "InstanceID", "0"));
    assert(!cg_upnpav_dmr_dispatch(model, command));
    assert(mupnp_action_getstatuscode(command) == 701);
  }
  /* Rejection cannot turn a query into a false success/playback claim. */
  assert(cg_upnpav_dmr_dispatch(model, info));
  assert(strcmp(mupnp_action_getargumentvaluebyname(info, "CurrentTransportState"), "NO_MEDIA_PRESENT") == 0);
  info = action(model, MUPNP_RENDERER_RENDERING_CONTROL, "GetVolume");
  assert(!cg_upnpav_dmr_dispatch(model, info));
  assert(mupnp_action_getstatuscode(info) == 401);
  assert(!cg_upnpav_dmr_dispatch(NULL, info));
  assert(!cg_upnpav_dmr_dispatch(model, NULL));

  CgUpnpAvRenderer* other = cg_upnpav_dmr_new();
  assert(other);
  info = action(other, MUPNP_RENDERER_CONNECTION_MANAGER, "GetProtocolInfo");
  assert(!cg_upnpav_dmr_dispatch(model, info));
  assert(mupnp_action_getstatuscode(info) == 401);
  mUpnpService* owner = mupnp_action_getservice(info);
  mupnp_action_setservice(info, cg_upnpav_dmr_service(model, MUPNP_RENDERER_CONNECTION_MANAGER));
  assert(!cg_upnpav_dmr_dispatch(model, info));
  assert(mupnp_action_getstatuscode(info) == 401);
  mupnp_action_setservice(info, owner);
  cg_upnpav_dmr_delete(other);
}

static void test_last_change(CgUpnpAvRenderer* model)
{
  const char* avt = "<Event xmlns=\"urn:schemas-upnp-org:metadata-1-0/AVT/\"><InstanceID val=\"0\"/></Event>";
  const char* rcs = "<Event xmlns=\"urn:schemas-upnp-org:metadata-1-0/RCS/\"><InstanceID val=\"0\"/></Event>";
  assert(strstr(cg_upnpav_dmr_get_last_change(model, MUPNP_RENDERER_AV_TRANSPORT), "/AVT/"));
  assert(strstr(cg_upnpav_dmr_get_last_change(model, MUPNP_RENDERER_RENDERING_CONTROL), "/RCS/"));
  assert(cg_upnpav_dmr_set_last_change(model, MUPNP_RENDERER_AV_TRANSPORT, avt));
  assert(cg_upnpav_dmr_set_last_change(model, MUPNP_RENDERER_RENDERING_CONTROL, rcs));
  assert(strcmp(cg_upnpav_dmr_get_last_change(model, MUPNP_RENDERER_AV_TRANSPORT), avt) == 0);
  assert(strcmp(cg_upnpav_dmr_get_last_change(model, MUPNP_RENDERER_RENDERING_CONTROL), rcs) == 0);
  assert(!cg_upnpav_dmr_set_last_change(model, MUPNP_RENDERER_CONNECTION_MANAGER, avt));
  assert(!cg_upnpav_dmr_set_last_change(model, MUPNP_RENDERER_RENDERING_CONTROL, NULL));
  assert(cg_upnpav_dmr_get_last_change(model, MUPNP_RENDERER_CONNECTION_MANAGER) == NULL);
  assert(cg_upnpav_dmr_get_last_change(NULL, MUPNP_RENDERER_AV_TRANSPORT) == NULL);
}

static void test_constructor_errors(CgUpnpAvRenderer* model)
{
  const char* scpds[MUPNP_RENDERER_SERVICE_COUNT] = { "<scpd/>", "<scpd/>", "<scpd/>" };
  assert(cg_upnpav_dmr_from_xml(NULL, scpds) == NULL);
  assert(cg_upnpav_dmr_from_xml("<root/>", NULL) == NULL);
  assert(cg_upnpav_dmr_from_xml("<root", scpds) == NULL);
  assert(cg_upnpav_dmr_from_xml("<root/>", scpds) == NULL);
  assert(cg_upnpav_dmr_from_xml("<root><device/></root>", scpds) == NULL);
  mUpnpString* xml = mupnp_string_new();
  assert(xml);
  mUpnpDevice* device = mupnp_service_getdevice(cg_upnpav_dmr_service(model, MUPNP_RENDERER_AV_TRANSPORT));
  const char* description = mupnp_xml_node_tostring(mupnp_device_getrootnode(device), true, xml);
  assert(description);
  mUpnpString* serviceStrings[MUPNP_RENDERER_SERVICE_COUNT];
  for (int i = 0; i < MUPNP_RENDERER_SERVICE_COUNT; i++) {
    serviceStrings[i] = mupnp_string_new();
    assert(serviceStrings[i]);
    scpds[i] = mupnp_xml_node_tostring(mupnp_service_getscpdnode(cg_upnpav_dmr_service(model, i)), true, serviceStrings[i]);
    assert(scpds[i]);
  }
  /* Exercise cleanup before each service parse and after earlier successes. */
  for (int i = 0; i < MUPNP_RENDERER_SERVICE_COUNT; i++) {
    scpds[i] = "<scpd";
    assert(cg_upnpav_dmr_from_xml(description, scpds) == NULL);
    scpds[i] = mupnp_string_getvalue(serviceStrings[i]);
  }
  CgUpnpAvRenderer* roundtrip = cg_upnpav_dmr_from_xml(description, scpds);
  assert(roundtrip);
  assert_idle(roundtrip);
  cg_upnpav_dmr_delete(roundtrip);
  scpds[2] = "<scpd/>";
  assert(cg_upnpav_dmr_from_xml(description, scpds) == NULL);
  scpds[2] = NULL;
  assert(cg_upnpav_dmr_from_xml(description, scpds) == NULL);
  for (int i = 0; i < MUPNP_RENDERER_SERVICE_COUNT; i++)
    mupnp_string_delete(serviceStrings[i]);
  mupnp_string_delete(xml);
}

int main(void)
{
  cg_upnpav_dmr_delete(NULL);
  assert(cg_upnpav_dmr_service(NULL, 0) == NULL);
  for (int i = 0; i < 20; i++) {
    CgUpnpAvRenderer* model = cg_upnpav_dmr_new();
    assert(model);
    assert(cg_upnpav_dmr_service(model, -1) == NULL);
    assert(cg_upnpav_dmr_service(model, MUPNP_RENDERER_SERVICE_COUNT) == NULL);
    assert_idle(model);
    test_dispatch(model);
    test_last_change(model);
    test_constructor_errors(model);
    assert_idle(model);
    cg_upnpav_dmr_delete(model);
  }
  puts("renderer construction/dispatch passed (no network lifecycle)");
  return 0;
}
