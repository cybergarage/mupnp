/* Loopback-only fixture host and no-I/O argument/endpoint checks. */
#undef NDEBUG
#include "diagnostic.h"
#include <assert.h>
#include <mupnp/net/url.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv)
{
  setbuf(stdout, NULL);
  if (argc == 2 && strcmp(argv[1], "--renderer-fixture") == 0) {
    mUpnpAvChkRenderer* renderer = mupnp_avchk_renderer_open("127.0.0.1", "127.0.0.1", false);
    assert(renderer);
    printf("%d\n", mupnp_avchk_renderer_port(renderer));
    getchar();
    mupnp_avchk_renderer_close(renderer);
    return 0;
  }
  if (argc == 3 && strcmp(argv[1], "--inspect-fixture") == 0) {
    mUpnpAvChkOptions options = { "127.0.0.1", "127.0.0.1", 1, false };
    mUpnpNetURL* url = mupnp_net_url_new();
    mupnp_net_url_set(url, argv[2]);
    bool result = mupnp_avchk_inspect(&options, url);
    mupnp_net_url_delete(url);
    return result ? 0 : 1;
  }
  mUpnpAvChkOptions options;
  char* legacy[] = { "upnpavchk" };
  char* valid[] = { "upnpavchk", "--with-renderer", "--target", "127.0.0.1", "--interface", "127.0.0.1", "--seconds", "10" };
  assert(mupnp_avchk_parse(1, legacy, &options) == 0);
  assert(mupnp_avchk_parse(8, valid, &options) == 1 && options.withRenderer && options.seconds == 10);
  assert(mupnp_avchk_parse(2, valid, &options) == -1);
  const char* rejected[] = { "0", "11", "-1", "1x", "01", "" };
  for (size_t i = 0; i < sizeof(rejected) / sizeof(rejected[0]); i++) {
    valid[7] = (char*)rejected[i];
    assert(mupnp_avchk_parse(8, valid, &options) == -1);
  }
  valid[7] = "2";
  valid[3] = "239.255.255.250";
  assert(mupnp_avchk_parse(8, valid, &options) == -1);
  valid[3] = "127.0.0.1";
  valid[5] = "0.0.0.0";
  assert(mupnp_avchk_parse(8, valid, &options) == -1);
  valid[5] = "127.0.0.1";
  valid[6] = "--with-renderer";
  assert(mupnp_avchk_parse(8, valid, &options) == -1);
  const char* urls[] = { "https://127.0.0.1/", "http://127.0.0.2/", "http://example.invalid/", "http://user@127.0.0.1/", "http://127.0.0.1/#fragment", "http://127.0.0.1//host/", "http://127.0.0.1/a\r\nx" };
  mUpnpNetURL* url = mupnp_net_url_new();
  assert(url);
  for (size_t i = 0; i < sizeof(urls) / sizeof(urls[0]); i++) {
    mupnp_net_url_set(url, urls[i]);
    assert(!mupnp_avchk_endpoint(url, "127.0.0.1"));
  }
  mupnp_net_url_set(url, "http://127.0.0.1:12345/description.xml");
  assert(mupnp_avchk_endpoint(url, "127.0.0.1"));
  mupnp_net_url_delete(url);
  puts("parser and endpoint checks passed");
  return 0;
}
