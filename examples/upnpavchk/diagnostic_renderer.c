/* Opt-in, short-lived diagnostic service. No media/GENA callbacks are fetched. */
#include "diagnostic.h"
#include "renderer_internal.h"
#include <arpa/inet.h>
#include <mupnp/ssdp/ssdp_server.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

struct mUpnpAvChkRenderer {
  mUpnpRendererModel* model;
  mUpnpDevice* device;
  mUpnpHttpServer* http;
  mUpnpSSDPServer* ssdp;
  mUpnpMutex* mutex;
  char local[INET_ADDRSTRLEN];
  char target[INET_ADDRSTRLEN];
  bool advertised;
  bool observed;
  int notify;
};

static bool permitted(mUpnpAvChkRenderer* renderer, const char* remote)
{
  return remote && (strcmp(remote, renderer->target) == 0 || strcmp(remote, renderer->local) == 0);
}

static bool dispatch(mUpnpAction* action)
{
  mUpnpDevice* device = mupnp_service_getdevice(mupnp_action_getservice(action));
  mUpnpAvChkRenderer* renderer = mupnp_device_getuserdata(device);
  return mupnp_renderer_model_dispatch(renderer->model, action);
}

static void http_request(mUpnpHttpRequest* request)
{
  mUpnpDevice* device = mupnp_http_request_getuserdata(request);
  mUpnpAvChkRenderer* renderer = mupnp_device_getuserdata(device);
  struct sockaddr_in peer;
  socklen_t size = sizeof(peer);
  char remote[INET_ADDRSTRLEN];
  if (getpeername(mupnp_socket_getid(mupnp_http_request_getsocket(request)), (struct sockaddr*)&peer, &size) != 0
      || peer.sin_family != AF_INET || !inet_ntop(AF_INET, &peer.sin_addr, remote, sizeof(remote)) || !permitted(renderer, remote)) {
    mupnp_http_request_poststatuscode(request, 403);
    return;
  }
  /* Never subscribe, resolve CALLBACK URLs, serve files or accept uploads.
   * Core GET serves only the model's device/SCPD descriptions. SOAP actions
   * are dispatched locally, with all playback/mutation rejected. */
  const char* method = mupnp_http_request_getmethod(request);
  if (!method || (strcmp(method, "GET") && strcmp(method, "HEAD") && strcmp(method, "POST"))) {
    mupnp_http_request_poststatuscode(request, 405);
    return;
  }
  mupnp_mutex_lock(renderer->mutex);
  /* The core copies supplied SOAP arguments into persistent action objects.
   * Clear inputs first so a missing argument cannot reuse a prior request. */
  for (int i = 0; i < MUPNP_RENDERER_SERVICE_COUNT; i++) {
    mUpnpService* service = mupnp_renderer_model_service(renderer->model, i);
    for (mUpnpAction* command = mupnp_service_getactions(service); command; command = mupnp_action_next(command)) {
      for (mUpnpArgument* arg = mupnp_action_getarguments(command); arg; arg = mupnp_argument_next(arg)) {
        if (mupnp_argument_isindirection(arg))
          mupnp_argument_setvalue(arg, "");
      }
    }
  }
  mupnp_device_httprequestrecieved(request);
  mupnp_mutex_unlock(renderer->mutex);
}

/* One explicitly bound multicast socket for all alive/byebye/response traffic. */
static bool send_ssdp(mUpnpAvChkRenderer* renderer, const char* nts, const char* remote, int port, const char* search)
{
  struct sockaddr_in destination;
  memset(&destination, 0, sizeof(destination));
  destination.sin_family = AF_INET;
  destination.sin_port = htons(port);
  if (inet_pton(AF_INET, remote, &destination.sin_addr) != 1)
    return false;
  const char* types[6] = { "upnp:rootdevice", mupnp_device_getudn(renderer->device), "urn:schemas-upnp-org:device:MediaRenderer:1", NULL, NULL, NULL };
  for (int i = 0; i < 3; i++)
    types[i + 3] = mupnp_service_getservicetype(mupnp_renderer_model_service(renderer->model, i));
  bool success = true;
  for (int i = 0; i < 6; i++) {
    if (search && strcmp(search, "ssdp:all") && strcmp(search, types[i]))
      continue;
    char message[2048], usn[512];
    const char* udn = mupnp_device_getudn(renderer->device);
    snprintf(usn, sizeof(usn), "%s%s%s", udn, i == 1 ? "" : "::", i == 1 ? "" : types[i]);
    int length;
    if (nts)
      length = snprintf(message, sizeof(message), "NOTIFY * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nNT: %s\r\nNTS: %s\r\nUSN: %s\r\nCACHE-CONTROL: max-age=30\r\nLOCATION: http://%s:%d/description.xml\r\nSERVER: POSIX/1 UPnP/1.0 mUPnP-diagnostic/1\r\n\r\n", types[i], nts, usn, renderer->local, mupnp_device_gethttpport(renderer->device));
    else
      length = snprintf(message, sizeof(message), "HTTP/1.1 200 OK\r\nST: %s\r\nUSN: %s\r\nEXT:\r\nCACHE-CONTROL: max-age=30\r\nLOCATION: http://%s:%d/description.xml\r\nSERVER: POSIX/1 UPnP/1.0 mUPnP-diagnostic/1\r\n\r\n", types[i], usn, renderer->local, mupnp_device_gethttpport(renderer->device));
    if (length <= 0 || (size_t)length >= sizeof(message)
        || sendto(renderer->notify, message, length, 0, (struct sockaddr*)&destination, sizeof(destination)) != length)
      success = false;
  }
  return success;
}

static void ssdp_request(mUpnpSSDPPacket* packet)
{
  mUpnpDevice* device = mupnp_ssdp_packet_getuserdata(packet);
  mUpnpAvChkRenderer* renderer = mupnp_device_getuserdata(device);
  const char* remote = mupnp_ssdp_packet_getremoteaddress(packet);
  const char* usn = mupnp_http_headerlist_getvalue(packet->headerList, "USN");
  const char* nts = mupnp_http_headerlist_getvalue(packet->headerList, "NTS");
  const char* nt = mupnp_http_headerlist_getvalue(packet->headerList, "NT");
  if (remote && strcmp(remote, renderer->local) == 0 && usn && strstr(usn, mupnp_device_getudn(device)) == usn
      && nts && strcmp(nts, "ssdp:alive") == 0 && nt && strcmp(nt, "urn:schemas-upnp-org:device:MediaRenderer:1") == 0) {
    mupnp_mutex_lock(renderer->mutex);
    renderer->observed = true;
    mupnp_mutex_unlock(renderer->mutex);
    return;
  }
  const char* mx = mupnp_http_headerlist_getvalue(packet->headerList, "MX");
  const char* st = mupnp_ssdp_packet_getst(packet);
  const char* data = mupnp_ssdp_packet_getdata(packet);
  if (!permitted(renderer, remote) || !mx || strlen(mx) != 1 || mx[0] < '1' || mx[0] > '3'
      || !st || !data || strncmp(data, "M-SEARCH * HTTP/1.1", 19) || !mupnp_ssdp_packet_isdiscover(packet))
    return;
  send_ssdp(renderer, NULL, remote, mupnp_ssdp_packet_getremoteport(packet), st);
}

bool mupnp_avchk_renderer_observed(mUpnpAvChkRenderer* renderer)
{
  mupnp_mutex_lock(renderer->mutex);
  bool observed = renderer->observed;
  mupnp_mutex_unlock(renderer->mutex);
  return observed;
}

bool mupnp_avchk_renderer_close(mUpnpAvChkRenderer* renderer)
{
  if (!renderer)
    return true;
  bool sent = true;
  if (renderer->ssdp)
    mupnp_ssdp_server_delete(renderer->ssdp);
  if (renderer->advertised)
    sent = send_ssdp(renderer, "ssdp:byebye", "239.255.255.250", 1900, NULL);
  if (renderer->notify >= 0)
    close(renderer->notify);
  if (renderer->http)
    mupnp_http_server_delete(renderer->http);
  mupnp_renderer_model_delete(renderer->model);
  if (renderer->mutex)
    mupnp_mutex_delete(renderer->mutex);
  free(renderer);
  return sent;
}

int mupnp_avchk_renderer_port(mUpnpAvChkRenderer* renderer)
{
  return renderer ? mupnp_device_gethttpport(renderer->device) : 0;
}

mUpnpAvChkRenderer* mupnp_avchk_renderer_open(const char* local, const char* target, bool advertise)
{
  struct in_addr address;
  if (!local || !target || strlen(local) >= INET_ADDRSTRLEN || strlen(target) >= INET_ADDRSTRLEN
      || inet_pton(AF_INET, local, &address) != 1 || address.s_addr == INADDR_ANY
      || inet_pton(AF_INET, target, &address) != 1)
    return NULL;
  mUpnpAvChkRenderer* renderer = calloc(1, sizeof(*renderer));
  if (!renderer)
    return NULL;
  renderer->notify = -1;
  strcpy(renderer->local, local);
  strcpy(renderer->target, target);
  renderer->mutex = mupnp_mutex_new();
  renderer->model = mupnp_renderer_model_new();
  if (!renderer->mutex || !renderer->model)
    goto fail;
  renderer->device = mupnp_service_getdevice(mupnp_renderer_model_service(renderer->model, MUPNP_RENDERER_CONNECTION_MANAGER));
  mupnp_device_setfriendlyname(renderer->device, "mUPnP diagnostic renderer (no playback)");
  mupnp_device_updateudn(renderer->device);
  mupnp_device_setleasetime(renderer->device, 30);
  mupnp_device_setuserdata(renderer->device, renderer);
  mupnp_device_setactionlistener(renderer->device, dispatch);
  /* This diagnostic does not implement event delivery. Do not advertise
   * eventable variables that would require accepting CALLBACK URLs. */
  for (int i = 0; i < MUPNP_RENDERER_SERVICE_COUNT; i++) {
    mUpnpService* service = mupnp_renderer_model_service(renderer->model, i);
    for (mUpnpStateVariable* variable = mupnp_service_getstatevariables(service); variable; variable = mupnp_statevariable_next(variable))
      mupnp_statevariable_setsendevents(variable, "no");
  }
  renderer->http = mupnp_http_server_new();
  if (renderer->http)
    renderer->http->sock = mupnp_avchk_bind_socket(local, true);
  if (!renderer->http || !renderer->http->sock)
    goto fail;
  struct sockaddr_in bound;
  socklen_t size = sizeof(bound);
  if (getsockname(mupnp_socket_getid(mupnp_http_server_getsocket(renderer->http)), (struct sockaddr*)&bound, &size) != 0)
    goto fail;
  mupnp_device_sethttpport(renderer->device, ntohs(bound.sin_port));
  mupnp_http_server_settimeout(renderer->http, 2);
  mupnp_http_server_setuserdata(renderer->http, renderer->device);
  mupnp_http_server_setlistener(renderer->http, http_request);
  if (!mupnp_http_server_start(renderer->http))
    goto fail;
  if (advertise) {
    renderer->notify = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in source;
    memset(&source, 0, sizeof(source));
    source.sin_family = AF_INET;
    inet_pton(AF_INET, local, &source.sin_addr);
    unsigned char ttl = 1;
    if (renderer->notify < 0 || bind(renderer->notify, (struct sockaddr*)&source, sizeof(source)) != 0
        || setsockopt(renderer->notify, IPPROTO_IP, IP_MULTICAST_IF, &source.sin_addr, sizeof(source.sin_addr)) != 0
        || setsockopt(renderer->notify, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) != 0)
      goto fail;
    renderer->ssdp = mupnp_ssdp_server_new();
    if (!renderer->ssdp || !mupnp_ssdp_server_open(renderer->ssdp, renderer->local))
      goto fail;
    mupnp_ssdp_server_setuserdata(renderer->ssdp, renderer->device);
    mupnp_ssdp_server_setlistener(renderer->ssdp, ssdp_request);
    if (!mupnp_ssdp_server_start(renderer->ssdp))
      goto fail;
    /* Set before sending, so partial failures also attempt a byebye. */
    renderer->advertised = true;
    if (!send_ssdp(renderer, "ssdp:alive", "239.255.255.250", 1900, NULL))
      goto fail;
  }
  return renderer;
fail:
  mupnp_avchk_renderer_close(renderer);
  return NULL;
}
