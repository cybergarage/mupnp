/* Scoped, read-only interoperability checks. Never print URLs or media names. */
#include "diagnostic.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <mupnp/net/url.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

static volatile sig_atomic_t interrupted;
static void interrupt_handler(int signalNumber) { interrupted = signalNumber; }

static bool ipv4(const char* text)
{
  struct in_addr address;
  if (!text || inet_pton(AF_INET, text, &address) != 1)
    return false;
  unsigned long host = ntohl(address.s_addr);
  return host != 0 && host != 0xffffffffUL && (host >> 28) != 0xe && (host >> 28) != 0xf;
}

void mupnp_avchk_usage(void)
{
  puts("upnpavchk [--target IPv4 --interface local-IPv4 [--seconds 1..10] [--with-renderer]]\n"
       "No arguments: legacy directory listing. Diagnostic mode prints stage results only.\n"
       "--with-renderer: temporary MediaRenderer:1 diagnostic advertisement; NO PLAYBACK.\n"
       "Both addresses must be on the same local IPv4 subnet. No redirects/subscriptions.\n"
       "Read-only: SSDP, description and one root BrowseMetadata; no NAS policy changes.");
}

int mupnp_avchk_parse(int argc, char** argv, mUpnpAvChkOptions* options)
{
  memset(options, 0, sizeof(*options));
  options->seconds = 3;
  if (argc == 1)
    return 0;
  if (argc == 2 && strcmp(argv[1], "--help") == 0)
    return 2;
  bool haveSeconds = false;
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--with-renderer") == 0) {
      if (options->withRenderer)
        return -1;
      options->withRenderer = true;
    }
    else if (strcmp(argv[i], "--target") == 0 && !options->target && i + 1 < argc)
      options->target = argv[++i];
    else if (strcmp(argv[i], "--interface") == 0 && !options->interfaceAddress && i + 1 < argc)
      options->interfaceAddress = argv[++i];
    else if (strcmp(argv[i], "--seconds") == 0 && !haveSeconds && i + 1 < argc) {
      const char* value = argv[++i];
      if (strlen(value) != 1 || value[0] < '1' || value[0] > '9') {
        if (strcmp(value, "10") != 0)
          return -1;
      }
      options->seconds = atoi(value);
      haveSeconds = true;
    }
    else
      return -1;
  }
  if (!ipv4(options->target) || !ipv4(options->interfaceAddress))
    return -1;
#ifndef MUPNP_AVCHK_RENDERER
  if (options->withRenderer)
    return -2;
#endif
  return 1;
}

static bool local_subnet(const mUpnpAvChkOptions* options)
{
  struct ifaddrs* interfaces = NULL;
  struct in_addr local, target;
  inet_pton(AF_INET, options->interfaceAddress, &local);
  inet_pton(AF_INET, options->target, &target);
  bool found = false;
  if (getifaddrs(&interfaces) != 0)
    return false;
  for (struct ifaddrs* item = interfaces; item; item = item->ifa_next) {
    if (!item->ifa_addr || !item->ifa_netmask || item->ifa_addr->sa_family != AF_INET)
      continue;
    struct in_addr address = ((struct sockaddr_in*)item->ifa_addr)->sin_addr;
    struct in_addr mask = ((struct sockaddr_in*)item->ifa_netmask)->sin_addr;
    if (address.s_addr == local.s_addr && (address.s_addr & mask.s_addr) == (target.s_addr & mask.s_addr))
      found = true;
  }
  freeifaddrs(interfaces);
  return found;
}

/* Only http://<exact numeric target>:port/path, without credentials/fragments.
 * The caller never follows redirects or any other device's LOCATION/URLBase. */
bool mupnp_avchk_endpoint(mUpnpNetURL* url, const char* target)
{
  const char* request = mupnp_net_url_getrequest(url);
  if (!mupnp_net_uri_ishttpprotocol(url) || !mupnp_streq(mupnp_net_url_gethost(url), target)
      || mupnp_net_uri_hasuser(url) || mupnp_net_uri_haspassword(url) || mupnp_net_uri_hasfragment(url)
      || mupnp_net_url_getport(url) < 1 || mupnp_net_url_getport(url) > 65535 || !request || request[0] != '/' || request[1] == '/')
    return false;
  for (const unsigned char* p = (const unsigned char*)request; *p; p++) {
    if (*p <= 32 || *p >= 127)
      return false;
  }
  return true;
}

/* The legacy host bind helper rejects port zero. Keep ephemeral-port support
 * local to this POSIX diagnostic rather than changing core/default behavior. */
mUpnpSocket* mupnp_avchk_bind_socket(const char* local, bool listening)
{
  mUpnpSocket* sock = mupnp_socket_stream_new();
  if (!sock)
    return NULL;
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    mupnp_socket_delete(sock);
    return NULL;
  }
  mupnp_socket_setid(sock, fd);
#ifdef SO_NOSIGPIPE
  int enabled = 1;
  setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &enabled, sizeof(enabled));
#endif
  struct sockaddr_in source;
  memset(&source, 0, sizeof(source));
  source.sin_family = AF_INET;
  if (inet_pton(AF_INET, local, &source.sin_addr) != 1 || bind(fd, (struct sockaddr*)&source, sizeof(source)) != 0
      || (listening && listen(fd, 8) != 0)) {
    mupnp_socket_delete(sock);
    return NULL;
  }
  socklen_t size = sizeof(source);
  if (getsockname(fd, (struct sockaddr*)&source, &size) != 0) {
    mupnp_socket_delete(sock);
    return NULL;
  }
  mupnp_socket_setaddress(sock, local);
  mupnp_socket_setport(sock, ntohs(source.sin_port));
  mupnp_socket_setdirection(sock, listening ? MUPNP_NET_SOCKET_SERVER : MUPNP_NET_SOCKET_CLIENT);
  return sock;
}

/* Bind each client explicitly to the selected interface. Two-second connect
 * and read/write bounds. Only the exact supplied numeric peer is contacted. */
static mUpnpHttpResponse* request(const char* local, const char* target, int port, const char* path, const char* body)
{
  if (interrupted)
    return NULL;
  mUpnpHttpResponse* response = mupnp_http_response_new();
  mUpnpSocket* sock = mupnp_avchk_bind_socket(local, false);
  if (!response || !sock)
    goto done;
  int fd = mupnp_socket_getid(sock);
  struct sockaddr_in peer;
  memset(&peer, 0, sizeof(peer));
  peer.sin_family = AF_INET;
  peer.sin_port = htons(port);
  if (inet_pton(AF_INET, target, &peer.sin_addr) != 1)
    goto done;
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) != 0)
    goto done;
  int connected = connect(fd, (struct sockaddr*)&peer, sizeof(peer));
  if (connected != 0) {
    if (errno != EINPROGRESS)
      goto done;
    struct pollfd pending = { fd, POLLOUT, 0 };
    int error = 0;
    socklen_t length = sizeof(error);
    if (poll(&pending, 1, 2000) <= 0 || getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &length) != 0 || error)
      goto done;
  }
  if (interrupted || fcntl(fd, F_SETFL, flags) != 0)
    goto done;
  mupnp_socket_settimeout(sock, 2);
  char header[4096];
  const char* soap = body ? "Content-Type: text/xml; charset=\"utf-8\"\r\nSOAPAction: \"urn:schemas-upnp-org:service:ContentDirectory:1#Browse\"\r\n" : "";
  int size = snprintf(header, sizeof(header), "%s %s HTTP/1.1\r\nHost: %s:%d\r\nUser-Agent: mUPnP-avchk-diagnostic/1\r\nConnection: close\r\n%sContent-Length: %zu\r\n\r\n", body ? "POST" : "GET", path, target, port, soap, body ? strlen(body) : 0);
  if (size <= 0 || (size_t)size >= sizeof(header) || mupnp_socket_write(sock, header, size) != (size_t)size)
    goto done;
  if (body && mupnp_socket_write(sock, body, strlen(body)) != strlen(body))
    goto done;
  if (!mupnp_http_response_read(response, sock, false))
    mupnp_http_response_clear(response);
done:
  if (sock)
    mupnp_socket_delete(sock);
  return response;
}

static double monotonic_seconds(void)
{
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return now.tv_sec + now.tv_nsec / 1000000000.0;
}

/* Multicast search only; all responses except the selected source are ignored
 * before parsing LOCATION, and no control point auto-fetch thread is started. */
static bool discover(const mUpnpAvChkOptions* options, mUpnpNetURL* location)
{
  if (interrupted)
    return false;
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0)
    return false;
  struct sockaddr_in local, destination;
  memset(&local, 0, sizeof(local));
  memset(&destination, 0, sizeof(destination));
  local.sin_family = destination.sin_family = AF_INET;
  inet_pton(AF_INET, options->interfaceAddress, &local.sin_addr);
  inet_pton(AF_INET, "239.255.255.250", &destination.sin_addr);
  destination.sin_port = htons(1900);
  unsigned char ttl = 1;
  bool success = false;
  if (bind(fd, (struct sockaddr*)&local, sizeof(local)) != 0
      || setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &local.sin_addr, sizeof(local.sin_addr)) != 0
      || setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) != 0)
    goto done;
  const char search[] = "M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nMAN: \"ssdp:discover\"\r\nMX: 1\r\nST: upnp:rootdevice\r\n\r\n";
  if (sendto(fd, search, sizeof(search) - 1, 0, (struct sockaddr*)&destination, sizeof(destination)) != sizeof(search) - 1)
    goto done;
  double deadline = monotonic_seconds() + options->seconds;
  while (!interrupted && monotonic_seconds() < deadline) {
    struct pollfd pending = { fd, POLLIN, 0 };
    if (poll(&pending, 1, 100) <= 0)
      continue;
    char data[8192], remote[INET_ADDRSTRLEN];
    struct sockaddr_in peer;
    socklen_t length = sizeof(peer);
    ssize_t size = recvfrom(fd, data, sizeof(data) - 1, 0, (struct sockaddr*)&peer, &length);
    if (size <= 0 || peer.sin_family != AF_INET || !inet_ntop(AF_INET, &peer.sin_addr, remote, sizeof(remote)) || strcmp(remote, options->target))
      continue;
    data[size] = '\0';
    if (strncmp(data, "HTTP/1.1 200", 12) != 0)
      continue;
    char* save = NULL;
    for (char* line = strtok_r(data, "\r\n", &save); line; line = strtok_r(NULL, "\r\n", &save)) {
      if (strncasecmp(line, "LOCATION:", 9) == 0) {
        const char* value = line + 9;
        while (*value == ' ' || *value == '\t')
          value++;
        mupnp_net_url_set(location, value);
        if (mupnp_avchk_endpoint(location, options->target)) {
          success = true;
          goto done;
        }
      }
    }
  }
done:
  close(fd);
  return success;
}

bool mupnp_avchk_inspect(const mUpnpAvChkOptions* options, mUpnpNetURL* location)
{
  if (!mupnp_avchk_endpoint(location, options->target))
    return false;
  mUpnpHttpResponse* response = request(options->interfaceAddress, options->target, mupnp_net_url_getport(location), mupnp_net_url_getrequest(location), NULL);
  int status = response ? mupnp_http_response_getstatuscode(response) : 0;
  printf("description_http=%d\n", status);
  mUpnpDevice* device = mupnp_device_new();
  bool success = false;
  if (!device || status != 200 || !mupnp_device_parsedescription(device, mupnp_http_response_getcontent(response), mupnp_string_length(response->content))) {
    puts("description=failed\nbrowse=not-run");
    goto done;
  }
  puts("description=parsed");
  /* This sets the base for local URL resolution, not an automatic fetch. */
  mupnp_http_headerlist_set(device->ssdpPkt->headerList, MUPNP_HTTP_LOCATION, mupnp_net_url_getvalue(location));
  mUpnpService* service = mupnp_device_getservicebytype(device, "urn:schemas-upnp-org:service:ContentDirectory:1");
  mUpnpNetURL* control = service ? mupnp_service_getcontrolurl(service) : NULL;
  if (interrupted || !control || !mupnp_avchk_endpoint(control, options->target)) {
    puts("browse=not-run-invalid-endpoint");
    if (control)
      mupnp_net_url_delete(control);
    goto done;
  }
  mupnp_http_response_delete(response);
  /* Exactly one root metadata request; no child traversal or media fetch. */
  const char browse[] = "<?xml version=\"1.0\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:Browse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\"><ObjectID>0</ObjectID><BrowseFlag>BrowseMetadata</BrowseFlag><Filter></Filter><StartingIndex>0</StartingIndex><RequestedCount>1</RequestedCount><SortCriteria></SortCriteria></u:Browse></s:Body></s:Envelope>";
  response = request(options->interfaceAddress, options->target, mupnp_net_url_getport(control), mupnp_net_url_getrequest(control), browse);
  mupnp_net_url_delete(control);
  status = response ? mupnp_http_response_getstatuscode(response) : 0;
  printf("browse_http=%d\n", status);
  mUpnpXmlNodeList* nodes = mupnp_xml_nodelist_new();
  mUpnpXmlParser* parser = mupnp_xml_parser_new();
  if (status == 200 && nodes && parser && mupnp_xml_parse(parser, nodes, mupnp_http_response_getcontent(response), mupnp_string_length(response->content))) {
    mUpnpXmlNode* envelope = mupnp_xml_nodelist_gets(nodes);
    mUpnpXmlNode* body = envelope ? mupnp_xml_node_getchildnodewithnamespace(envelope, "Body", NULL, true) : NULL;
    /* Namespace-aware name lookup in core matches the local element name. */
    mUpnpXmlNode* result = body ? mupnp_xml_node_getchildnodewithnamespace(body, "BrowseResponse", NULL, true) : NULL;
    success = result && mupnp_xml_node_getchildnode(result, "Result");
  }
  if (parser)
    mupnp_xml_parser_delete(parser);
  if (nodes)
    mupnp_xml_nodelist_delete(nodes);
  puts(success ? "browse=success" : "browse=failed");
done:
  if (device)
    mupnp_device_delete(device);
  if (response)
    mupnp_http_response_delete(response);
  return success;
}

int mupnp_avchk_run(const mUpnpAvChkOptions* options)
{
  setbuf(stdout, NULL);
  if (!local_subnet(options)) {
    fputs("Diagnostic requires a selected local interface and same-subnet target.\n", stderr);
    return 2;
  }
  struct sigaction action, oldInt, oldTerm;
  memset(&action, 0, sizeof(action));
  action.sa_handler = interrupt_handler;
  sigemptyset(&action.sa_mask);
  interrupted = 0;
  if (sigaction(SIGINT, &action, &oldInt) != 0)
    return 2;
  if (sigaction(SIGTERM, &action, &oldTerm) != 0) {
    sigaction(SIGINT, &oldInt, NULL);
    return 2;
  }
#ifdef MUPNP_AVCHK_RENDERER
  mUpnpAvChkRenderer* renderer = NULL;
#endif
  int result = 1;
  puts(options->withRenderer ? "mode=diagnostic-renderer-no-playback" : "mode=control-point-only");
#ifdef MUPNP_AVCHK_RENDERER
  if (options->withRenderer) {
    renderer = mupnp_avchk_renderer_open(options->interfaceAddress, options->target, true);
    if (!renderer) {
      puts("renderer=failed");
      goto done;
    }
    puts("renderer=started-announcement-sent");
    for (int i = 0; i < 10 && !interrupted && !mupnp_avchk_renderer_observed(renderer); i++)
      poll(NULL, 0, 50);
    puts(mupnp_avchk_renderer_observed(renderer) ? "renderer_advertisement=observed" : "renderer_advertisement=not-observed");
    mUpnpHttpResponse* self = request(options->interfaceAddress, options->interfaceAddress, mupnp_avchk_renderer_port(renderer), "/description.xml", NULL);
    printf("renderer_description_http=%d\n", self ? mupnp_http_response_getstatuscode(self) : 0);
    if (self)
      mupnp_http_response_delete(self);
  }
#endif
  mUpnpNetURL* location = mupnp_net_url_new();
  if (location) {
    bool found = discover(options, location);
    puts(found ? "ssdp=target-response" : "ssdp=no-target-response");
    if (found && !interrupted)
      result = mupnp_avchk_inspect(options, location) ? 0 : 1;
    else
      puts("description=not-run\nbrowse=not-run");
    mupnp_net_url_delete(location);
  }
#ifdef MUPNP_AVCHK_RENDERER
done:
  if (renderer) {
    bool sent = mupnp_avchk_renderer_close(renderer);
    puts(sent ? "renderer=stopped-byebye-sent" : "renderer=stopped-byebye-failed");
  }
#endif
  sigaction(SIGINT, &oldInt, NULL);
  sigaction(SIGTERM, &oldTerm, NULL);
  return interrupted ? 128 + interrupted : result;
}
