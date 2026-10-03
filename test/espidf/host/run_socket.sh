#!/bin/sh
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/../../.." && pwd)
build=$(mktemp -d)
trap 'rm -rf "$build"' EXIT HUP INT TERM
flags=${SANITIZERS--fsanitize=address,undefined}
${CC:-cc} -std=gnu11 -g -pthread -DESP_PLATFORM $flags \
  -ffunction-sections -fdata-sections -Wl,--gc-sections \
  -I"$root/test/espidf/host/shims" -I"$root/include" \
  "$root/test/espidf/host/socket_shutdown_test.c" \
  "$root/src/mupnp/util/thread.c" "$root/src/mupnp/util/time.c" \
  "$root/src/mupnp/util/cond.c" "$root/src/mupnp/util/mutex.c" \
  "$root/src/mupnp/util/list.c" "$root/src/mupnp/util/string.c" \
  "$root/src/mupnp/util/string_function.c" "$root/src/mupnp/net/socket.c" \
  "$root/src/mupnp/net/datagram_packet.c" \
  -o "$build/socket_shutdown_test"
"$build/socket_shutdown_test"
