#!/bin/sh
# GNU ld wrapping makes the empty-interface case independent of the host NICs.
set -eu
case $(uname -s) in
  Linux) ;;
  *) echo 'SKIP: this regression requires Linux linker wrapping'; exit 77 ;;
esac
root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
build=$(mktemp -d)
trap 'rm -rf "$build"' EXIT HUP INT TERM

cat > "$build/test.c" <<'EOF'
#include "TestDevice.h"
#include <stdio.h>
#include <stdlib.h>

int __wrap_mupnp_net_gethostinterfaces(mUpnpNetworkInterfaceList* interfaces)
{
  mupnp_net_interfacelist_clear(interfaces);
  return 0;
}

bool __wrap_mupnp_http_serverlist_open(mUpnpHttpServerList* servers, int port)
{
  (void)servers;
  (void)port;
  /* Fail immediately on the old implementation, instead of hanging CI. */
  fputs("FAIL: HTTP port retry reached with no usable interfaces\n", stderr);
  exit(1);
}

int main(void)
{
  mUpnpDevice* device = upnp_test_device_new();
  if (!device)
    return 1;
  int port = mupnp_device_gethttpport(device);
  for (int attempt = 0; attempt < 2; attempt++) {
    if (mupnp_device_start(device)
        || mupnp_device_isrunning(device)
        || mupnp_device_gethttpport(device) != port
        || mupnp_http_serverlist_size(device->httpServerList) != 0
        || mupnp_ssdp_serverlist_size(device->ssdpServerList) != 0
        || !mupnp_device_stop(device)) {
      fputs("FAIL: unsuccessful start left device resources active\n", stderr);
      mupnp_device_delete(device);
      return 1;
    }
  }
  mupnp_device_delete(device);
  puts("PASS: no-interface start fails cleanly, including retry/stop/delete");
  return 0;
}
EOF

# Automake runs tests in the build tree's test/unix directory. For a manual
# invocation, cd there first. XML_LIBS is supplied by the configured Makefile.
${CC:-cc} -std=gnu11 -Wall -Wextra -Werror -g -pthread \
  -DHAVE_CONFIG_H -I../.. -I"$root/include" -I"$root/test" \
  -c "$build/test.c" -o "$build/test.o"
${CC:-cc} -g -pthread -DHAVE_CONFIG_H -I../.. \
  -I"$root/include" -I"$root/test" \
  "$build/test.o" "$root/test/TestDevice.c" ../../lib/unix/libmupnp.a \
  -Wl,--wrap=mupnp_net_gethostinterfaces \
  -Wl,--wrap=mupnp_http_serverlist_open ${XML_LIBS--lexpat} ${CODE_COVERAGE_LIBS-} \
  -o "$build/test"
"$build/test"
